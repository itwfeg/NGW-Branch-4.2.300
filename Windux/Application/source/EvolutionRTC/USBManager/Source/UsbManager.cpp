/*

   Module Name:

   UsbManager.cpp 

Description:

This module handles OS-generated device interface notifications, and 
maintains a map of connected USB devices and OS-specific data related 
to those devices. Manages "Code updater" classes to download initial 
device firmware. Manages "Comm manager" classes by notifying the
RealTimeController class. "Code updater" classes inherit from the
IUsbCodeUpdater class, and "Comm manager" classes inherit from 
CUsbCommBase.

Also searches for key files on removable drives. Search is triggered on 
arrival or removal of disk device interfaces (e.g. USB flash drive).

*/

// local conditionals

// Disable notifications to RealTimeController. Disables comm 
// manager classes but code updater classes are still created
// and managed.
//#define DISABLE_RTC_NOTIFY

// Check for existence of key file but do not verify the contents.
#define KEY_FILE_SIMPLE_TEST

// generate multiple test labels
//#define USB_TRIGGER_MULTIPLE_TEST_LABELS

//
// headers for primary classes
//

#include "PAL_platformheader.h"
#include "evolution.h"
#include "RealTimeController.h"
#include "UsbManager.h"
#include "IUsbCodeUpdater.h"
#include "UsbAddComponent.h"
#include "SimpleTimer.h"
#include "UsbNode.h"
#include "Hex.h"

#include "MessageLogger.h"

//
// headers for updater classes
//


#include "HobVersion.h"
#if defined ( __linux__ )
#include "UsbManagerLinux.h"
#else
#include "UsbManagerWin32.h"
#endif


//
// headers for key file search 
//

// TODO: move key file search to separate class
#include "SessionEvent.h"
#include "AccessKeyActivation.h"
#include "AccessKeyDeactivation.h"
#include "AccessKeys.h"
#include "Configuration.h"

//
// miscellaneous headers
//

#include "Debug.h"
#include "Hex.h"
#include "Integer.h"
#include "UInteger.h"
#include "HobString.h"
#include "OutputLogger.h"
#include <algorithm>
#include <string>


//#if defined ( __linux__ )
//#define UNREFERENCED_PARAMETER(...)
//#endif

using namespace util_filesystem;

//#ifdef USB_TRIGGER_MULTIPLE_TEST_LABELS
//#include "PrinterPrintTestLabel.h"
//#endif

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int UsbManager::dataType = 0;
std::string UsbManager::dataTypeName("UsbManager");
#endif


#if !defined ( __linux__ )
DEFINE_GUID(GUID_DEVINTERFACE_KEYBOARD, 0x884B96C3, 0x56EF, 0x11d1, 0xbc,
                 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);
    
DEFINE_GUID(GUID_DEVINTERFACE_MOUSE, 0x378DE44C, 0x56EF, 0x11d1, 0xbc,
                 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);

// List of device interface class guids which
// are of interest to us. 
const GUID UsbManager::SupportedClassGuids [] = 
{
    GUID_DEVINTERFACE_USB_DEVICE,
    GUID_DEVINTERFACE_KEYBOARD,
    GUID_DEVINTERFACE_MOUSE,
};
#endif


// Devices reporting these bProductId (PIDs) will have a code updater 
// class created for them on arrival or enumeration.
const USHORT UsbManager::CodeUpdaterPids[] =
{
    HOBART_EVO_WEIGHER_ID_CU,
    HOBART_EVO_PRINTER_ID_CU,
    HOBART_ESS_PRINTER_ID_CU,
    HOBART_ESC_PRINTER_ID_CU,
    HOBART_APC_PRINTER_ID_CU,
    HOBART_XPE_WEIGHER_ID_CU,
	HOBART_XPE_PRINTER_ID_CU	//Alex Zavala 7/31
};

// List PIDs for supported/managed devices. If the device's PID is not 
// in this list, no action will be taken, however, the device will still 
// be tracked via the usbNodeMap.
const USHORT UsbManager::ManagedDevicePids[] =
{
    HOBART_APC_PRINTER_ID,
    HOBART_APC_PRINTER_ID_CU,
    HOBART_ESC_PRINTER_ID,
    HOBART_ESC_PRINTER_ID_CU,
    HOBART_ESS_PRINTER_ID,
    HOBART_ESS_PRINTER_ID_CU,
    HOBART_EVO_PRINTER_ID,
    HOBART_EVO_PRINTER_ID_CU,
    HOBART_EVO_WEIGHER_ID,
    HOBART_EVO_WEIGHER_ID_CU,
    HOBART_EVO_WRAPPER_ID,
    HOBART_LSMN_ID,
    HOBART_XPE_PRINTER_ID,
	HOBART_XPE_PRINTER_ID_CU,	//Alex Zavala 8/1
    HOBART_XPE_WEIGHER_ID,
    HOBART_XPE_WEIGHER_ID_CU,
    HOBART_FASTRACK_FPC,
    OLD_HOBART_EVO_PRINTER_ID,
    HOBART_CLA_ID
};

// List of PIDs for devices based on a Cypress EZ-USB device. 
const USHORT UsbManager::EzUsbDevicePids[] =
{
    HOBART_EVO_WEIGHER_ID,
    HOBART_EVO_WEIGHER_ID_CU
};

// List of PIDs for devices based on an Atmel AT91 device. 
const USHORT UsbManager::AtmelDevicePids[] =
{
    HOBART_APC_PRINTER_ID,
    HOBART_APC_PRINTER_ID_CU,
    HOBART_FASTRACK_FPC
};

util_thread::Mutex<> UsbManager::instanceMutex_;
//
// instance pointer for singleton
//
UsbManager *UsbManager::instance = (UsbManager *) NULL;

// returns instance pointer
UsbManager *UsbManager::getInstance()
{
    if (instance == 0)
    {
        instanceMutex_.acquire();

        if( instance == 0 ) {
#if defined ( __linux__ )
        instance = new UsbManagerLinux();
#else
        instance = new UsbManagerWin32();
#endif
    }
        
		instanceMutex_.release();
    }
    return instance;
}

// default constructor (private for singleton)
UsbManager::UsbManager()
    : Hobart::CEvolMsgReceiver("UsbManager")
{
//	HTA-63 & HTA-110:  This is the old search for Service & Supervisor keys (Windux). Python scripts now handle this for the HT series.    InitKeySearch();
#if !defined ( __linux__ )
    dev_entries = sizeof(SupportedClassGuids);
#endif

#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);
    util_acc::addInstance(dataType);
#endif
}

// default destructor
UsbManager::~UsbManager()
{
    deregisterAllEvents();
    cancelTimer(KeySearchTimerId);
    shutdownTimerThreads();
    ShutdownNicely();

#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

// Handle timer timeout.
// Overrides method declared in CHobartTimer:ITimerHandler
void UsbManager::timeoutProcess( __int32 timer_id )
{
    if (timer_id == KeySearchTimerId && bKeySearchEnabled)
    {
        KeySearchTimerHandler();
    }
}

// Open device path for the specified node.
// Return HANDLE or INVALID_HANDLE_VALUE.
// TODO: queue open handles in CUsbNode, and close them in its destructor
HANDLE UsbManager::OpenDevice(USBD_NODE_ID usbNodeId)
{
    HANDLE hOut = INVALID_HANDLE_VALUE;

    UsbCritSec.Enter();
    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( usbNodeId );
    if ( iter != usbNodeMap.end() )
    {
        CUsbNode* p = iter->second;
        hOut = PAL_absclass::PAL_createFile( (LPCSTR)p->DevicePath,
                PAL_FILE_GEN_RD | PAL_FILE_GEN_WR,
                PAL_FILE_SHARE_RD | PAL_FILE_SHARE_WR,
                NULL,
                PAL_FILE_OPEN,
                0,
                NULL);
    }
    UsbCritSec.Leave();
    return hOut;
}

// Close the previously opened handle for the specified node.
void UsbManager::CloseDevice(USBD_NODE_ID UNUSED(usbNodeId), HANDLE h)
{
    UNREFERENCED_PARAMETER(usbNodeId);
    PAL_absclass::PAL_closeHandle(h);
}

#if 0
// Read the USB descriptors for the specified node.
// Return a pointer to the CUsbNode's data member (!)
// TODO: disallow caller writing into the descriptor struct
SUsbDescriptors * UsbManager::getUsbDescs( USBD_NODE_ID  nodeId )
{
    UsbCritSec.Enter();

    SUsbDescriptors * pd = NULL;
    HANDLE hDev = INVALID_HANDLE_VALUE;
    USHORT vid = getVendorId(nodeId);
    USHORT pid = getProductId(nodeId);
    bool bUseEzUsbIoctl = isDeviceEzUsb(vid,pid) && isCodeUpdateRequired(vid,pid);
    try
    {
        HANDLE hDev = OpenDevice(nodeId);
        if (hDev == INVALID_HANDLE_VALUE)
        {
            Debug::trace("UsbManager.Attach", HobString("getUsbDescs: failed to open device"));
            goto leave;
        }

        CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );

        if ( iter != usbNodeMap.end() )
        {
            Debug::trace("UsbManager.Attach", HobString("getUsbDescs: reading descriptors for node ") + string(Hex::toString((long long)nodeId)));

#if 1
            CUsbNode* p = iter->second;

            // read the device's descriptors into the CUsbNode desc member
            if ( readDescriptors( hDev, &(p->desc), bUseEzUsbIoctl ) )
            {
                // all good, point to descriptors
                pd = &(p->getDescriptors());
            }
            else
            {
                // problem reading descriptors from device
                Debug::trace("UsbManager.Attach", HobString("getUsbDescs: failed to read descriptors for node ") + string(Hex::toString(nodeId)));
            }
#endif
        }
        else
        {
            // the nodeId that was passed in is not in our map.
            Debug::trace("UsbManager.Attach", HobString("getUsbDescs: usb node not found in map "));
        }
    }
    catch (...)
    {
        Debug::trace("UsbManager.Exception",  HobString("getUsbDescs: caught exception "));
        LogCatch;
    }

leave:
    if (hDev != INVALID_HANDLE_VALUE)
    {
        PAL_absclass::PAL_closeHandle(hDev);
    }
    UsbCritSec.Leave();
    return pd;
}
#endif


// Process message events generated by the internal messaging subsystem.
// Overrides method declared in CEvolMsgReceiver:IEvolMsgHandler.
void UsbManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    switch (msg->getDeviceType())
    {
        case SessionDev:
        {
            switch (msg->getEventType())
            {
                case SnRequestKeyActivation:
                {
                    Debug::trace("UsbManager.KeySearch", HobString("UsbManager::processEvent::received  SnRequestKeyActivation"));
//	HTA-63 & HTA-110:  This is the old search for Service & Supervisor keys (Windux). Python scripts now handle this for the HT series.                    EnableKeySearch(true);

                    //send the session peripherals it missed on scale powerup
                    if(powerUp)
                    {
                    	if(foundMouse)
                    	{
                    		HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::MOUSE_ATTACHED);
                    		//notify the session that mouse was detected at power up.
                    		dispatchEvent(&deviceEvent);
						}
						if(foundKeyboard)
						{
							HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::KEYBOARD_ATTACHED);
							//notify the session that keyboard was detected at power up.
							dispatchEvent(&deviceEvent);
						}
						if(foundBarcodeScanner)
						{
							HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::BARCODE_SCANNER_ATTACHED);
							//notify the session that barcode scanner was detected at power up.
							dispatchEvent(&deviceEvent);
						}
						powerUp = false;
                    }
                }
                break;
            }
        }
        break;

        default:
        {
            Debug::trace("UsbManager.Attach", HobString("UsbManager::processEvent::received msg ") +
                    HobString("dev= ") + Hex::toString((ULONG)msg->getDeviceType()) + 
                    HobString("evt= ") + Hex::toString((ULONG)msg->getEventType())  );
        }
    }
}

// RemoveAllDevices is responsible for clearing the usbNodeMap and 
// releasing all resources. It is called from EnumerateDevices at startup, 
// and again from CHobartScaleService::OnStop() when the service is stopped. 
// Because it is called from OnStop(), it must acquire and hold the critsec
// for the duration.
void UsbManager::RemoveAllDevices()
{
    UsbCritSec.Enter();

    CUsbNodeMap::iterator i;
    CUsbNode* pNode;
    USBD_NODE_ID nodeId;

    Debug::trace("UsbManager.Attach",  HobString("RemoveAllDevices - entry"));

    for (i = usbNodeMap.begin(); i != usbNodeMap.end(); i++)
    {
        try
        {
            pNode = i->second;
            nodeId = i->first;
            if (pNode)
            {
                if (pNode->bServiceObject)
                {
                    Debug::trace("UsbManager.Attach",  HobString("RemoveAllDevices: notifying RTC to detach node ") + Hex::toString((long long)nodeId));
                    DeleteCommMgr(nodeId, pNode);
                }
                if (pNode->pUpdater)
                {
                    Debug::trace("UsbManager.Attach",  HobString("RemoveAllDevices: deleting updater for node ") + Hex::toString((long long)nodeId));
                    DeleteUpdater(nodeId, pNode);
                }
                Debug::trace("UsbManager.Attach",  HobString("RemoveAllDevices: removing node ") + Hex::toString((long long)nodeId));
            }
            else
            {
                Debug::trace("UsbManager.Attach",  HobString("null pointer at node ") + Hex::toString((long long)nodeId) + ", removing" );
            }
        }
        catch (...)
        {
            Debug::trace("UsbManager.Exception",  HobString("RemoveAllDevices - caught exception "));
            LogCatch;
        }
    }

    usbNodeMap.clear();

    Debug::trace("UsbManager.Attach",  HobString("RemoveAllDevices - exit"));

    UsbCritSec.Leave();
}

// Return the CUsbNode pointer at the specified node.
CUsbNode* UsbManager::getNode( USBD_NODE_ID nodeId )
{
    CUsbNode* pNode = 0;

    UsbCritSec.Enter();

    CUsbNodeMap::const_iterator i = usbNodeMap.find(nodeId);
    if (i != usbNodeMap.end() )
    {
        pNode = i->second;
    }

    UsbCritSec.Leave();

    return pNode;
}

// Mark the node as DETACHED.
void UsbManager::DetachDevice( USBD_NODE_ID nodeId )
{
    UsbCritSec.Enter();

    Debug::trace("UsbManager.Attach",  HobString("DetachDevice: Entered ") + Hex::toString((long long)nodeId));

    CUsbNode* pNode = getNode(nodeId);

    if (pNode)
    {
        pNode->state = CUsbNode::DETACHED;
        removeFromMap(nodeId);

        Debug::trace("UsbManager.Attach",  HobString("DetachDevice: detached node ") + Hex::toString((long long)nodeId));
		MessageLogger::logMessage(MessageLogger::InfoLog, "DetachDevice:  Detached node "  + Hex::toString((ULONG)nodeId));
    }
    else
    {
        Debug::trace("UsbManager.Attach",  HobString("DetachDevice: ?? null pointer for node ") + Hex::toString((long long)nodeId));
		MessageLogger::logMessage(MessageLogger::InfoLog, "DetachDevice:  NULL pointer for node "  + Hex::toString((ULONG)nodeId) + "!!!");
    }

    UsbCritSec.Leave();
}

// Notify the RealTimeController class (RTC) that the specified node 
// has been attached. RTC instantiates the appropriate comm manager.
// TODO: move AttachDevice into RTC class so that UsbManager does not
// require knowledge of comm manager-derived classes.
bool UsbManager::AttachDevice( USBD_NODE_ID nodeId )
{
    // collect the basic USB device info into locals
    USHORT pid = getProductId(nodeId);
    USHORT vid = getVendorId(nodeId);
    USHORT bcdDevice = getDeviceVersion(nodeId);
    UINT16 majorVersion = (bcdDevice & 0xFF00) >> 8;
    UINT16 minorVersion = bcdDevice & 0x00FF;
    bool bResult = false;

    // dump the basic USB device info
    if (Debug::isTracing("UsbManager.Attach"))
    {
        Debug::trace("UsbManager.Attach",
                HobString("AttachDevice: node: ") + HobString(Hex::toString((long long)nodeId)) +
                HobString(" idVendor: ") + HobString(Hex::toString( (USHORT)vid)) +
                HobString(" idProduct: ") + HobString(Hex::toString( (USHORT)pid)) +
                HobString(" bcdDevice: ") + HobString(Hex::toString( (USHORT)bcdDevice)) );
    }

#ifdef DISABLE_RTC_NOTIFY
    OutputDebugStringA( "UsbManager::AttachDevice: exit\r\n" );
    return true;
#else
    if (! ((vid == HOBART_VENDOR_ID) || (vid == 0xfffe)) )
    {
        Debug::trace("UsbManager.Attach", "unknown idVendor for attached device");
        return bResult;
    }

 	if ( (pid == HOBART_EVO_PRINTER_ID) || (pid == OLD_HOBART_EVO_PRINTER_ID) )
    {
        Debug::trace("UsbManager.Attach", "Hobart USB EPP Printer Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB EPP Printer Attached. node = "  + Hex::toString((long long)nodeId));
        
        RealTimeController::getInstance()->USBPrinterAttached(nodeId, bcdDevice, EppPrinter);
        HobVersion printerHardwareVersion(HobVersion::EppPrinterHW, majorVersion, minorVersion);
        UsbAddComponent addComponent(nodeId, printerHardwareVersion);
        dispatchEvent(&addComponent);
        bResult = true;
    }
// (haberrc) #if 0 // new xpe printer
    else if (pid == HOBART_XPE_PRINTER_ID)
    {
        Debug::trace("UsbManager.Attach", "Hobart USB EPP2 Printer Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB EPP2 Printer Attached. node = "  + Hex::toString((long long)nodeId));
        RealTimeController::getInstance()->USBPrinterAttached(nodeId, bcdDevice, Epp2Printer);
        HobVersion printerHardwareVersion(HobVersion::Epp2PrinterHW, majorVersion, minorVersion);
        UsbAddComponent addComponent(nodeId, printerHardwareVersion);
        dispatchEvent(&addComponent);
        bResult = true;
    }
    else if (pid == HOBART_ESS_PRINTER_ID)
    {
        Debug::trace("UsbManager.Attach", "Hobart USB ESS Printer Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB ESS Printer Attached. node = "  + Hex::toString((long long)nodeId));
        RealTimeController::getInstance()->USBPrinterAttached(nodeId, bcdDevice, EssPrinter);
        HobVersion printerHardwareVersion(HobVersion::EssPrinterHW, majorVersion, minorVersion);
        UsbAddComponent addComponent(nodeId, printerHardwareVersion);
        dispatchEvent(&addComponent);
        bResult = true;
    }
    else if (pid == HOBART_ESC_PRINTER_ID)
    {
        Debug::trace("UsbManager.Attach", "Hobart USB ESC Printer Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB ESC Printer Attached. node = "  + Hex::toString((long long)nodeId));
        RealTimeController::getInstance()->USBESCPrinterAttached(nodeId, bcdDevice);
        HobVersion printerHardwareVersion(HobVersion::EscPrinterHW, majorVersion, minorVersion);
        UsbAddComponent addComponent(nodeId, printerHardwareVersion);
        dispatchEvent(&addComponent);
        bResult = true;
    }
    else if (pid == HOBART_EVO_WRAPPER_ID)
    {
        Debug::trace("UsbManager.Attach", "Hobart USB Wrapper Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB Wrapper Attached. node = "  + Hex::toString((long long)nodeId));
        RealTimeController::getInstance()->USBWrapperAttached(nodeId, bcdDevice);
        HobVersion wrapperHardwareVersion(HobVersion::WrapperControllerHW, 1, 0);
        UsbAddComponent addComponent(nodeId, wrapperHardwareVersion);
        dispatchEvent(&addComponent);
        bResult = true;
    }
    else if (pid == HOBART_EVO_WEIGHER_ID)
    {
        Debug::trace("UsbManager.Attach", "Hobart USB EZ-USB Weigher Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB Wrapper Attached. node = "  + Hex::toString((long long)nodeId));
        RealTimeController::getInstance()->USBWeigherAttached(nodeId, bcdDevice, WgUsbEvol);
        HobVersion weigherHardwareVersion(HobVersion::UsbWeigher15KgHW, majorVersion, minorVersion);
        UsbAddComponent addComponent(nodeId, weigherHardwareVersion);
        dispatchEvent(&addComponent);
        bResult = true;
    }
    else if (pid == HOBART_APC_PRINTER_ID)
    {
        Debug::trace("UsbManager.Attach", "Hobart USB APC Printer Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB APC Printer Attached. node = "  + Hex::toString((long long)nodeId));
        RealTimeController::getInstance()->USBAPCAttached(nodeId, bcdDevice);
        bResult = true;
    }
    else if(pid == HOBART_FASTRACK_FPC)
    {
        Debug::trace("UsbManager.Attach", "Hobart USB FPC Printer Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB FPC Printer Attached. node = "  + Hex::toString((long long)nodeId));
        RealTimeController::getInstance()->USBFPCAttached(nodeId, bcdDevice);
        bResult = true;
    }
    else if((pid == HOBART_XPE_WEIGHER_ID))
    {
        Debug::trace("UsbManager.Attach", "Hobart USB XPE Weigher Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB XPE Weigher Attached. node = "  + Hex::toString((long long)nodeId));
		RealTimeController::getInstance()->USBWeigherAttached(nodeId, bcdDevice, WgUsbAWS);
        HobVersion weigherHardwareVersion(HobVersion::Aws2WeigherHW, majorVersion, minorVersion);
        UsbAddComponent addComponent(nodeId, weigherHardwareVersion);
        dispatchEvent(&addComponent);
        bResult = true;
    }
	else if (pid == HOBART_CLA_ID)
	{
		Debug::trace("UsbManager.Attach", "Hobart USB CLA Attached");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Hobart USB CLA Attached. node = "  + Hex::toString((long long)nodeId));
		RealTimeController::getInstance()->USBCLAAttached(nodeId);
		HobVersion claHardwareVersion(HobVersion::CLAControllerHW, 1, 0);//Changed 10/15 Alex Zavala
		UsbAddComponent addComponent(nodeId, claHardwareVersion);
        dispatchEvent(&addComponent);
        bResult = true;
    }
    else
    {
        Debug::trace("UsbManager.Attach", "unknown idProduct for attached device");
		MessageLogger::logMessage(MessageLogger::InfoLog, "AttachDevice:  Unknown idProduct for device. node = "  + Hex::toString((long long)nodeId));
    }

    // If the node was succesfully handed off to RTC, 
    // flag the node to indicate the RTC should be notified 
    // upon device removal.
    if (bResult)
    {
        CUsbNode* p = getNode( nodeId );
        if (p)
        {
            p->bServiceObject = true;
        }
    }

    return bResult;
#endif // DISABLE_RTC_NOTIFY
}

// TODO: printUsbError redundant vs CUsbCommBase impl?
void UsbManager::printUSBError(int UNUSED(errNumber))
{
    UNREFERENCED_PARAMETER(errNumber);
}

// return true if device is based on Cypress EZ-USB
bool UsbManager::isDeviceEzUsb(USHORT vid, USHORT pid)
{
    bool isEzUsb = false;

    if (HOBART_VENDOR_ID == vid)
    {
        for (unsigned int i=0; i <  sizeof(EzUsbDevicePids)/sizeof(USHORT); i++)
        {
            if (pid == EzUsbDevicePids[i])
            {
                isEzUsb = true;
                break;
            }
        }
    }
    return isEzUsb;
}

// return true if device is based on Atmel AT91 
bool UsbManager::isDeviceAtmel(USHORT vid, USHORT pid)
{
    bool isAtmel = false;

    if ( ATMEL_SAMBA_VENDOR_ID == vid && ATMEL_SAMBA_PRODUCT_ID == pid )
    {
        isAtmel = true;
    }
    else if ( HOBART_VENDOR_ID == vid ) 
    {
        for (unsigned int i=0; i <  sizeof(AtmelDevicePids)/sizeof(USHORT); i++)
        {
            if (pid == AtmelDevicePids[i])
            {
                isAtmel = true;
                break;
            }
        }
    }
    return isAtmel;
}

// return true if device is Atmel SAM-BA based 
bool UsbManager::isDeviceAtmelSamba(USHORT vid, USHORT pid)
{
    bool isAtmel = false;

    if ( ATMEL_SAMBA_VENDOR_ID == vid && ATMEL_SAMBA_PRODUCT_ID == pid )
    {
        isAtmel = true;
    }
    return isAtmel;
}

// return true if the device requires a code updater
bool UsbManager::isCodeUpdateRequired(USHORT vid, USHORT pid)
{
    bool needUpdate = false;

    if (isDeviceAtmelSamba(vid,pid))
    {
        needUpdate = true;
    }
    else if ( HOBART_VENDOR_ID == vid ) 
    {
        for (unsigned int i=0; i <  sizeof(CodeUpdaterPids)/sizeof(USHORT); i++)
        {
            if (pid == CodeUpdaterPids[i])
            {
                needUpdate = true;
            }
        }
    }
    return needUpdate;
}

// return true if this device should be managed by us.
bool UsbManager::isManagedDevice(USHORT vid, USHORT pid)
{
    bool managed = false;

    if (  ATMEL_SAMBA_VENDOR_ID == vid && ATMEL_SAMBA_PRODUCT_ID == pid )
    {
        managed = true;
    }
    else if ( HOBART_VENDOR_ID == vid ) 
    {
        for (unsigned int i=0; i <  sizeof(ManagedDevicePids)/sizeof(USHORT); i++)
        {
            if (pid == ManagedDevicePids[i])
            {
                managed = true;
            }
        }
    }
    return managed;
}


// Add CUsbNode* and nodeId to the map, with nodeId as the key.
void UsbManager::addToMap( USBD_NODE_ID nodeId, CUsbNode* pNode )
{
    UsbCritSec.Enter();
    usbNodeMap.insert(make_pair(nodeId, pNode));
    UsbCritSec.Leave();
}

// Find the map entry matching nodeId and remove it.
// Delete the updater if not already deleted.
void UsbManager::removeFromMap( USBD_NODE_ID nodeId )
{
	UsbCritSec.Enter();

	Debug::trace( "UsbManager.Event", "removeFromMap: Entered" );
    CUsbNodeMap::iterator i( usbNodeMap.find( nodeId ) );

    if ( usbNodeMap.end() == i )
        Debug::trace( "UsbManager.Event", "removeFromMap: node not found in map " + Hex::toString((long long)nodeId) );
    else
    {
    	CUsbNode *p( i->second );

        if ( !p )
            Debug::trace( "UsbManager.Error", "removeFromMap: null object pointer at node " + Hex::toString((long long)nodeId) );
        else
        {
			// SOF-2931: Need to set addSamba to False when removing node
			if (isDeviceAtmelSamba( getVendorId(nodeId), getProductId(nodeId) ) ) {
				resetSamba();
			}
			
            if( p->pUpdater ) {
				DeleteUpdater(nodeId, p);
            }

            //all managed devices have a comm manager!
            DeleteCommMgr( nodeId, p );
            
            i->second = NULL;
            delete p;
        }

        usbNodeMap.erase( nodeId );
    }

    UsbCritSec.Leave();

    Debug::trace( "UsbManager.Event", "removeFromMap: Exiting" );
}

#if defined ( __linux__ )
uint8_t UsbManager::getInterfaceClass(USBD_NODE_ID nodeId)
{
    uint8_t dp = 0;
    UsbCritSec.Enter();
    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );
    if (iter != usbNodeMap.end())
    {
        CUsbNode* p = iter->second;
        dp = p->getInterfaceClass();
    }
    UsbCritSec.Leave();
    return dp;
}

uint8_t UsbManager::getDeviceProtocol(USBD_NODE_ID nodeId)
{
    uint8_t dp = 0;
    UsbCritSec.Enter();
    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );
    if (iter != usbNodeMap.end())
    {
        CUsbNode* p = iter->second;
        dp = p->getDeviceProtocol();
    }
    UsbCritSec.Leave();
    return dp;
}

uint8_t UsbManager::getiInterface(USBD_NODE_ID nodeId)
{
    uint8_t dp = 0;
    UsbCritSec.Enter();
    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );
    if (iter != usbNodeMap.end())
    {
        CUsbNode* p = iter->second;
        dp = p->getiInterface();
    }
    UsbCritSec.Leave();
    return dp;
}

#endif

// Return the USB PID for the specified node.
USHORT UsbManager::getProductId(USBD_NODE_ID nodeId)
{
    USHORT pid = 0;
    UsbCritSec.Enter();
    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );
    if (iter != usbNodeMap.end())
    {
        CUsbNode* p = iter->second;
        pid = p->getProductId();
    }
    UsbCritSec.Leave();
    return pid;
}

// Return the USB VID for the specified node.
USHORT UsbManager::getVendorId(USBD_NODE_ID nodeId)
{
    USHORT vid = 0;
    UsbCritSec.Enter();
    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId    );
    if (iter != usbNodeMap.end())
    {
        CUsbNode* p = iter->second;
        vid = p->getVendorId();
    }
    UsbCritSec.Leave();
    return vid;
}

// Return the USB version (descriptor bcdDevice field)
USHORT UsbManager::getDeviceVersion(USBD_NODE_ID nodeId)
{
    USHORT bcdVer = 0;
    UsbCritSec.Enter();
    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId    );
    if (iter != usbNodeMap.end())
    {
        CUsbNode* p = iter->second;
        bcdVer = p->getDeviceVersion();
    }
    UsbCritSec.Leave();
    return bcdVer;
}


// Return the full device path for the specified node.
string& UsbManager::getDevicePath(USBD_NODE_ID nodeId)
{
    UsbCritSec.Enter();
    static string nullstr="";
    string& s = nullstr;
    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );
    if (iter != usbNodeMap.end())
    {
        CUsbNode* p = iter->second;
        s = p->getDevicePath();
    }
    UsbCritSec.Leave();
    return s;
}



// EnumerateDevices is called from CHobartScaleService::Run() after 
// all supervisory classes have been initialized at EvolMain.startAll. 
void UsbManager::EnumerateDevices()
{
    Debug::trace("UsbManager.Attach", HobString("EnumerateDevices: "));

    try
    {
        RemoveAllDevices();
        UpdateDeviceMap();
    }
    catch (...)
    {
        Debug::trace("UsbManager.Exception",  HobString("EnumerateDevices: caught exception "));
        LogCatch;
    }
}

// Delete the updater class instance associated with the node.
void UsbManager::DeleteUpdater(USBD_NODE_ID nodeId, CUsbNode* p)
{
    Debug::trace("UsbManager.Attach",  HobString("DeleteUpdater: deleting updater for node ") + string(Hex::toString((ULONG)nodeId)));
 
    try
    {
		//UPDATER_SYNC
		//updater holds a mutex for the time it is active it would
		//release this lock when it's ready to be deleted.  We want
		//to make sure we wait for the updater to be done before we delete it.
		//
		USHORT lock_trys = 0;
		Debug::trace("UsbManager.Attach",  HobString("DeleteUpdater: Leaving Critical section and preparing to wait."));
 
	  	//UsbCritSec.Leave();
		DWORD ret=WAIT_TIMEOUT;

		while ((WAIT_OBJECT_0 != ret) && (3 >= lock_trys++))
		{
			Debug::trace("UsbManager.Attach",  "State: " + Integer::toString(p->state) + (" ") + string(Hex::toString((ULONG)nodeId)));
			if (NULL != p->pUpdater->update_lock)
				ret = PAL_absclass::PAL_waitForSingleObj(p->pUpdater->update_lock, 500);

				if(p->pUpdater == NULL)
					return;				
		}

		//UsbCritSec.Enter();
		Debug::trace("UsbManager.Attach",  HobString("DeleteUpdater: Reentered Crit section again. After waiting for ") + string(Integer::toString(lock_trys)) +
											HobString(" trys. ") + string(Hex::toString((ULONG)nodeId)));
		
		if (WAIT_OBJECT_0 == ret)
		{
			ret = PAL_absclass::PAL_releaseMutex(p->pUpdater->update_lock);

			if (CUsbNode::DETACHED != p->state)
			{
				Debug::trace("UsbManager.Attach",  HobString("DeleteUpdater: Updater status not set to Detached.") + string(Hex::toString((ULONG)nodeId)));
			}
			else
			{	
		    	delete p->pUpdater;
				p->pUpdater = 0;
				Debug::trace("UsbManager.Attach",  HobString("DeleteUpdater: Updater deleted.") + string(Hex::toString((ULONG)nodeId)));
			}
		
		}
		else
			Debug::trace("UsbManager.Attach",  HobString("DeleteUpdater: Failed to delete updater.") + string(Hex::toString((ULONG)nodeId)) + 
													HobString(" Curent state is " ) + Integer::toString(p->state)+ " " + string(Hex::toString((ULONG)nodeId)));
		}	
    catch(...)
    {
        Debug::trace("UsbManager.Attach",  HobString("DeleteUpdater: caught exception while deleting updater for node ") + Hex::toString((ULONG)nodeId));
        LogCatch;
    }

    p->pUpdater = NULL;

    //UsbCritSec.Leave();
}

// Trigger the destruction of the comm manager class instance associated 
// with the USB node, by notifying the RealTimeController class that the 
// underlying USB device was removed.
void UsbManager::DeleteCommMgr(USBD_NODE_ID nodeId, CUsbNode* p)
{
    // If currently under control of RTC/service class.
    // signal the RTC to unload the service class for this node.
    Debug::trace("UsbManager.Attach",  HobString("DeleteCommMgr: calling RTC to delete node ") + string(Hex::toString((ULONG)nodeId)));

#ifndef DISABLE_RTC_NOTIFY
    try
    {
        RealTimeController::getInstance()->USBDeviceDetached( nodeId );
    }
    catch(...)
    {
        Debug::trace("UsbManager.Exception",  HobString("DeleteCommMgr: caught exception while RTC detaching node ") + Hex::toString((ULONG)nodeId));
		MessageLogger::logMessage(MessageLogger::InfoLog, "DeleteCommMgr:  Exception in RTC USBDeviceDetached for node "  + Hex::toString((ULONG)nodeId));
        LogCatch;
    }
#endif
    p->bServiceObject = false;
    PAL_absclass::PAL_sleep(0);
}

// Enable or disable the key search. If the timer is already running, it is 
// stopped prior to enabling, then restarted.
void UsbManager::EnableKeySearch(bool bEnable)
{
    Hobart::SimpleTimer * pMyTimer = getTimer(KeySearchTimerId);

    // first, stop the search and 
    // destroy the timer if it exists
    bKeySearchEnabled = false;

    if (pMyTimer)
    {
        // timer is destroyed when stopped
        pMyTimer->stopPeriodicTimer();
        // allow reschedule
        PAL_absclass::PAL_sleep(0);
    }

    if (bEnable) 
    {
        keyAttemptCount = 0;
        bKeySearchEnabled = true;

        // create periodic timer to trigger KeySearchTimerHandler
        // which performs the key file search and notifications
        createTimer(KeySearchTimerInterval, KeySearchTimerId, true);
    }
}

// Initialize the key file search data members.
void UsbManager::InitKeySearch()
{
    bKeySearchEnabled = false;
    keyAttemptCount = 0;

	keyFileState[KEY_FILE_TYPE_SERVICE] =
	keyFileState[KEY_FILE_TYPE_SUPERVISOR] =
	keyFileState[KEY_FILE_TYPE_MFGTEST] =
	keyFileState[KEY_FILE_TYPE_SELFSERVE] =
	keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_UNKNOWN;

    registerEvent(SessionDev, SnRequestKeyActivation);
}

// Handle the periodic timer event for key file search.
// Call searchForKeyFiles, and process the key file states.
// If key files are found, generate the appropriate activate or
// deactivate messages over the internal messaging subsystem. 
void UsbManager::KeySearchTimerHandler()
{
    Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler"));

    if (!bKeySearchEnabled)
    {
        Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: search disabled"));
        return;
    }

#if 0
    if (searchForKeyFiles())
    {
//.        if (++keyAttemptCount >= NUM_KEY_SEARCH_ATTEMPTS)
        {
            Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: max key search attempts reached. terminating search."));
            EnableKeySearch(false);
	    //
            // one or more keys were found. update state and notify the system
	    //
            if ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SUPERVISOR] )
            {
                Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: supervisor key found. notifying system. ") );
                AccessKeyActivation active(AccessKeys::SupervisorPermanent);
                dispatchEvent(&active);
                keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_NOTIFIED;
            }

            if ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SERVICE] )
            {
                Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: service key found. notifying system. ") );
                AccessKeyActivation active(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent);
                dispatchEvent(&active);
                keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_NOTIFIED;
            }

            if ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_QATEST] )
            {
                Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: QATest key found. notifying system. ") );
                AccessKeyActivation active(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent | AccessKeys::QATest);
                dispatchEvent(&active);
                keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_NOTIFIED;
            }
        }//if (++keyAttemptCount >= NUM_KEY_SEARCH_ATTEMPTS)
    }//if (searchForKeyFiles())
    else
    {
        Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: no keys found during this search attempt."));

        if (++keyAttemptCount >= NUM_KEY_SEARCH_ATTEMPTS)
        {
            Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: max key search attempts reached. terminating search."));
            EnableKeySearch(false);

            // If key files were previously detected, update their state 
            // to REMOVED and notify the system to deactivate them.

            if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE] )
            {
                keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_REMOVED;
                Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: service key removed, notifying system ")  );
//.                AccessKeyDeactivation deactive(AccessKeys::ServicePermanent);
                AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent);
                dispatchEvent(&deactive);
            }

            if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SUPERVISOR] )
            {
                keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_REMOVED;
                Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: supervisor key removed, notifying system ")  );
                AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent);
                dispatchEvent(&deactive);
            }

            if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_QATEST] )
            {
                keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_REMOVED;
                Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: QATest key removed, notifying system ")  );
                AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent | AccessKeys::QATest);
                dispatchEvent(&deactive);
            }
        }
    }

#else

    bool key_found = searchForKeyFiles();

    if (TRUE == key_found)
    {
    	Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Found Key.  Preparing to notify and terminate timer. Don't call EnableKeySearch."));
//    	EnableKeySearch(false);
	//
       	// one or more keys were found. update state and notify the system
	//
    	int key_ = 0;
       	if ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SUPERVISOR] )
       	{
       	    Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: supervisor key found. notifying system. ") );
 //      	    AccessKeyActivation active(AccessKeys::SupervisorPermanent);
 //      	    dispatchEvent(&active);
       	    key_ |= AccessKeys::SupervisorPermanent;
       	    keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_NOTIFIED;
       	}

       	if ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SERVICE] )
       	{
       	    Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: service key found. notifying system. ") );
 //      	    AccessKeyActivation active(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent);
 //      	    dispatchEvent(&active);
       	    key_ |= AccessKeys::ServicePermanent;
       	    keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_NOTIFIED;
       	}

       	if ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_QATEST] )
       	{
       	    Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: QATest key found. notifying system. ") );
//       	    AccessKeyActivation active(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent | AccessKeys::QATest);
//       	    dispatchEvent(&active);
       	    key_ |= AccessKeys::QATest;
       	    keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_NOTIFIED;
       	}
       	if ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_MFGTEST] )
       	{
       	    Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: MFGTest key found. notifying system. ") );
//       	    AccessKeyActivation active(AccessKeys::MFGTest | AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent | AccessKeys::QATest);
//       	    dispatchEvent(&active);
       	    key_ |= AccessKeys::MFGTest;
       	    keyFileState[KEY_FILE_TYPE_MFGTEST] = KEY_FILE_STATE_NOTIFIED;
       	}
       	if ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SELFSERVE] )
       	{
       	    Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: SelfServePermanent key found. notifying system. ") );
//       	    AccessKeyActivation active(AccessKeys::SelfServePermanent | AccessKeys::MFGTest | AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent | AccessKeys::QATest);
//       	    dispatchEvent(&active);
       	    key_ |= AccessKeys::SelfServePermanent;
       	    keyFileState[KEY_FILE_TYPE_SELFSERVE] = KEY_FILE_STATE_NOTIFIED;
       	}
       	AccessKeyActivation active(key_);
       	dispatchEvent(&active);

    }//if key_found	
    //
    //Check for KEY_FILE_STATE_FOUND is not needed here but was put in so that any future changes to the logic still keeps
    //this code functional.
    //
    else if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE] ) || ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SUPERVISOR] )
		|| ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_QATEST] )  || ( KEY_FILE_STATE_NOTIFIED  == keyFileState[KEY_FILE_TYPE_SELFSERVE] )
		|| ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_MFGTEST] ) )
    {
    	Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Max key search attempts reached. terminating search. Don't call EnableKeySearch."));
//    	EnableKeySearch(false);

		// If key files were previously detected, update their state
		// to REMOVED and notify the system to deactivate them.
		if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SERVICE] ))
		{
			Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Service key removed.")  );
			if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE] )
			{
				Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Notifying system.")  );
				AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent);
				dispatchEvent(&deactive);
			}
			keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_REMOVED;
		}

		if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SUPERVISOR] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SUPERVISOR] ))
		{
			Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Supervisor key removed.")  );
			if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SUPERVISOR] )
			{
				Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Notifying system.")  );
				AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent);
				dispatchEvent(&deactive);
			}
			keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_REMOVED;
		}

		if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_QATEST] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_QATEST] ))
		{
		   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: QATest key removed.")  );
		   if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_QATEST] )
		   {
			   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler:Notifying system ")  );
				// In the next line, I'm commenting out the flag for QATest because QA areed that the QATest flag does not need to be turned off
				// after it has been turned on (it will get turned off due to a reboot).  I still have to turn off the other two flags though.
			   AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent /* | AccessKeys::QATest */);
			   dispatchEvent(&deactive);
		   }
		   keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_REMOVED;
		}
		if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_MFGTEST] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_MFGTEST] ))
		{
		   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: MFGTest key removed.")  );
		   if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_MFGTEST] )
		   {
			   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler:Notifying system ")  );
			   AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent  | AccessKeys::MFGTest );
			   dispatchEvent(&deactive);
		   }
		   keyFileState[KEY_FILE_TYPE_MFGTEST] = KEY_FILE_STATE_REMOVED;
		}
		if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SELFSERVE] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SELFSERVE] ))
		{
		   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: SelfServePermanent key removed.")  );
		   if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SELFSERVE] )
		   {
			   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler:Notifying system ")  );
			   AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent  | AccessKeys::MFGTest | AccessKeys::SelfServePermanent );
			   dispatchEvent(&deactive);
		   }
		   keyFileState[KEY_FILE_TYPE_SELFSERVE] = KEY_FILE_STATE_REMOVED;
		}
    }
    else
    {
		if (++keyAttemptCount >= NUM_KEY_SEARCH_ATTEMPTS)
		{
		    Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: No keys found after ") + Hex::toString(keyAttemptCount) + 
									(" attempts. Don't call EnableKeySearch."));
//	HTA-63 & HTA-110:  This is the old search for Service & Supervisor keys (Windux). Python scripts now handle this for the HT series.	    	EnableKeySearch(false);
		}
		else
		   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: no keys found, will try again."));

    }//else if no key was detected
#endif //if 0
}

// attempt to open key file and verify contents
bool UsbManager::checkForKeyValue( File& keyFilePath )
{
    bool rc = false;

#ifdef KEY_FILE_SIMPLE_TEST
    if (keyFilePath.exists())
        rc = true;
#else
    if (keyFilePath.exists())
    {
        std::ifstream keyFileStr (keyFilePath.getFullPath().c_str());

        if (keyFileStr.rdbuf( )->is_open( ))
        {
            char entry[32];
            int bytesRead = 0;

            int sizeofArray = sizeof(entry);

            keyFileStr.getline(entry, sizeofArray);

            bytesRead = keyFileStr.gcount();

            if (bytesRead > 0)
            {
                std::string password(entry);

                if (password == string("45374"))
                {
                    rc = true;
                }
            }
        }
    }
#endif // KEY_FILE_SIMPLE_TEST
    return rc;
}
