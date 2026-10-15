#include "EvolMessage.h"
#include "IUsbCodeUpdater.h"
#include "IPeripheralUpdater.h"

#include "VersionManagerEvent.h"
#include "PeripheralCodeUpdate.h"
#include "PeripheralRequestCodeUpdate.h"
#include "VmStatusMsg.h"

#include "Integer.h"
#include "Hex.h"
#include "filesys/file.h"
#include "filesys/directory.h"
#include "UsbManager.h"

// include subclass declarations here
#include "EvolWeigherCodeUpdater.h"
#include "EvolPrinterCodeUpdater.h"
//#include "EvolWrapperCodeUpdater.h" // TODO: EvolWrapperCodeUpdater
#include "AtmelSambaUpdater.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int IUsbCodeUpdater::dataType = 0;
std::string IUsbCodeUpdater::dataTypeName("IUsbCodeUpdater");
#endif

IUsbCodeUpdater::IUsbCodeUpdater(USBD_NODE_ID usbNodeId, UINT16 usbDeviceType) : Hobart::CEvolMsgReceiver("IUsbCodeUpdater")
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    Debug::trace("IUsbCodeUpdater", HobString("Constructor.  NodeId: ") + Hex::toString(usbNodeId) + HobString(",  device type: ") + Integer::toString(usbDeviceType) + HobString(",  this = ") + Hex::toString(PtrToUlong(this)));

    updateComplete_ = false;

    usbNodeId_ = usbNodeId;
    usbDeviceType_ = usbDeviceType;

    registerEvent(VersionManagerDev, PeripheralUpdateCode);
    registerEvent(VersionManagerDev, BeginUpgrade);
    registerEvent(VersionManagerDev, CancelUpgrade);
    registerEvent(VersionManagerDev, ComponentCheckComplete);

    timedOut_ = false;
    timeoutDuration_ = 10000;
	update_lock = NULL;

	//UPDATER_SYNC
    //
    //DeleteUpdater uses this  mutex to dertermine if the updater ready for 
    //deletion
    //
    //update_lock = PAL_absclass::PAL_createMutex(NULL, NULL, NULL);
	
//    if (NULL != update_lock)
//    {
#if defined ( __linux__ )
//    	if (WAIT_OBJECT_0 != PAL_absclass::PAL_waitForSingleObj(update_lock, 0))
//            Debug::trace("IUsbCodeUpdater", "Constructor.  Unable to lock Updater sync mutex!");
#endif

//		Debug::trace("IUsbCodeUpdater", HobString("Mutex created: ") + Integer::toString64((long long)update_lock) );
//    }
//    else
//	{
//        Debug::trace("IUsbCodeUpdater", "Constructor.  Failed to create Updater sync mutex.");
//  }
}

IUsbCodeUpdater::~IUsbCodeUpdater()
{
    Debug::trace("IUsbCodeUpdater", HobString("IUsbCodeUpdater Destructor Called"));
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

	deregisterAllEvents();

	//UPDATER_SYNC
	//PAL_absclass::PAL_closeHandle(update_lock);
    //shutdown the msg thread
    ShutdownNicely();
}

void IUsbCodeUpdater::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
   if ( msg.get() != NULL )
    {
        switch (msg->getDeviceType())
        {
           case VersionManagerDev:
                responseTimoutTime_ = PAL_absclass::PAL_getTickCount();
                handleVersionManagerDevEvent((VersionManagerEvent) msg->getEventType(), msg->Data());
                break;
            default:
                break;
        }

        if ( (PAL_absclass::PAL_getTickCount() - responseTimoutTime_) > timeoutDuration_)
           timedOut_ = true;
    }

}


void IUsbCodeUpdater::cleanUpWorkspace(util_filesystem::Directory *pDir)
{
    pDir->remove(true);
}

bool IUsbCodeUpdater::isUpdateComplete()
{
    return updateComplete_;
}
void IUsbCodeUpdater::forceTerminate()
{
    updateComplete_ = true;
}

// Create a code updater class instance for the specified USB node.
// The specific derived type is determined by the node's VID and PID.
IUsbCodeUpdater* IUsbCodeUpdater::UpdaterFactory(USBD_NODE_ID nodeId)
{
    IUsbCodeUpdater* pUpdater = 0;
    UsbManager* p = UsbManager::getInstance();

	if (p == 0)
	{
        Debug::trace("IUsbCodeUpdater", "UpdaterFactory:  Unable to obtain UsbManager instance. Abort!");
		return pUpdater;
	}

    USHORT pid = p->getProductId(nodeId);
    USHORT vid = p->getVendorId(nodeId);
    USHORT bcdDevice = p->getDeviceVersion(nodeId);
    UINT16 majorVersion = (bcdDevice & 0xFF00) >> 8;
    UINT16 minorVersion = bcdDevice & 0x00FF;
    HobVersion ver(HobVersion::UnknownComponent, majorVersion, minorVersion);

    try
    {
        if (HOBART_VENDOR_ID == vid)
        {
            switch(pid)
            {
                case HOBART_EVO_WEIGHER_ID_CU: // Cypress EZ-USB
                    Debug::trace("IUsbCodeUpdater", "UpdaterFactory: HOBART_EVO_WEIGHER_ID_CU");
                    ver.setVersionType( HobVersion::UsbWeigher15KgHW );
                    pUpdater = new EvolWeigherCodeUpdater( nodeId, pid, ver );
                    break;

                case HOBART_EVO_PRINTER_ID_CU: //
                    Debug::trace("IUsbCodeUpdater", "UpdaterFactory: HOBART_EVO_PRINTER_ID_CU");
                    //EppPrinterHW,
                    //EppPrinterSW,
                    //EppPrnBootCodeSW,
                    //EppPrnFlashUpdateSW,
                    ver.setVersionType( HobVersion::EppPrinterHW );
                    pUpdater = new EvolPrinterCodeUpdater(nodeId, pid, ver);
                    break;
					/* Alex Zavala's code for XPE version */
				case HOBART_XPE_PRINTER_ID_CU:	//Alex Zavala 8/1
					/*Debug::trace("IUsbCodeUpdater", "UpdaterFactory: HOBART_XPE_PRINTER_ID");
					ver.setVersionType( HobVersion::Epp2PrinterHW );
					pUpdater = new XPEPrinterCodeUpdater(nodeId,pid);*/
                    break;

                case HOBART_ESS_PRINTER_ID_CU:
                    Debug::trace("IUsbCodeUpdater", "UpdaterFactory: HOBART_ESS_PRINTER_ID_CU");
                    // TODO: add updater class for ESS_PRINTER_ID_CU
                    break;
                case HOBART_ESC_PRINTER_ID_CU:
                    // TODO: add updater class for HOBART_ESC_PRINTER_ID_CU
                    Debug::trace("IUsbCodeUpdater", "UpdaterFactory: HOBART_ESC_PRINTER_ID_CU");
                    break;
#if 0
                case HOBART_APC_PRINTER_ID_CU:
                    // TODO: add updater class for HOBART_APC_PRINTER_ID_CU
                    Debug::trace("UsbManager.Attach", "UpdaterFactory: HOBART_APC_PRINTER_ID_CU");
                    ver.setVersionType( HobVersion::ApcPrinterHW);
                    pUpdater = new ApcPrinterCodeUpdater(nodeId, pid, ver);
                    break;
#endif
                case HOBART_XPE_WEIGHER_ID_CU:
                    // TODO: add updater class for HOBART_XPE_WEIGHER_ID_CU Alex Zavala
                    Debug::trace("IUsbCodeUpdater", "UpdaterFactory: HOBART_XPE_WEIGHER_ID_CU");
                    break;

                default:
                    Debug::trace("IUsbCodeUpdater", std::string("UpdaterFactory: unsupported Hobart VID/PID") + Hex::toString(vid) + std::string(":") + Hex::toString(pid) );
                    ;
            }
        }
        else if (UsbManager::getInstance()->isDeviceAtmelSamba(vid,pid))
        {
            Debug::trace("IUsbCodeUpdater", "UpdaterFactory: Atmel SAM-BA");
            pUpdater = new AtmelSambaUpdater(nodeId);
        }/* 8/2 Alex Zavala's code for XPE printer */
		/*else if (UsbManager::getInstance()->isDeviceAtmel(vid,pid))
		{
			Debug::trace("IUsbCodeUpdater", "UpdaterFactory: Atmel Product: XPE");
			pUpdater = new AtmelSambaUpdater(nodeId);
		}*/
        else
        {
            Debug::trace("IUsbCodeUpdater", std::string("UpdaterFactory: unsupported VID/PID ") + Hex::toString(vid) + std::string(":") + Hex::toString(pid) );
        }
    }
    catch(...)
    {
        Debug::trace("IUsbCodeUpdater", "UpdaterFactory: caught exception, fail.");
        LogCatch;
    }
    return pUpdater;
}
