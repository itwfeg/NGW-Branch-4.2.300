//
// File : UsbManagerWin32.cpp
//
// Implements the UsbManager class for win32
//

#include "PAL_platformheader.h"

#include "RealTimeController.h"
#include "UsbManager.h"
#include "UsbManagerWin32.h"
#include "IUsbCodeUpdater.h"
#include "HobVersion.h"
#include "Configuration.h"
#include "filesys/directory.h"

#include "Debug.h"
#include "Hex.h"
#include "Integer.h"
#include "UInteger.h"
#include "HobString.h"
#include "OutputLogger.h"
#include "FCError.h"
#include "SimpleTimer.h"
#include "device_event.h"

#include "MessageLogger.h"

#include "win32/usb_win_node.h"
#include "WghIoctl.h" //from weigher driver

#ifdef USB_TRIGGER_MULTIPLE_TEST_LABELS
#include "PrinterPrintTestLabel.h"
#endif

#pragma warning ( disable: 4311 4312 )

#if defined( PAL_WIN32_RUN )

UsbManagerWin32::UsbManagerWin32()
{
    // HW-131
    foundMouse = false;
    foundKeyboard = false;
    foundBarcodeScanner = false;		
}

UsbManagerWin32::~UsbManagerWin32()
{
}

// Read the USB descriptors for the specified node.
// Return a pointer to the CUsbNode's data member (!)
// TODO: disallow caller writing into the descriptor struct
SUsbDescriptors * UsbManagerWin32::getUsbDescs( USBD_NODE_ID  nodeId )
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
            Debug::trace("UsbManager.Attach", HobString("getUsbDescs:  failed to open device"));
            goto leave;
        }

        CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );

        if ( iter != usbNodeMap.end() )
        {
            Debug::trace("UsbManager.Attach", HobString("getUsbDescs:  reading descriptors for node ") + string(Hex::toString((long long)nodeId)));

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
        Debug::trace("UsbManager.Attach",  HobString("getUsbDescs: caught exception "));
		MessageLogger::logMessage(MessageLogger::InfoLog, "getUsbDescs("  + Hex::toString((ULONG)nodeId) + "):  Exception! " + Hex::toString(vid) + "_" + Hex::toString(pid));
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



// Read the device descriptors using a custom vendor request and populate
// the USB descriptor structure at *descs. The handle must already be open
// prior to calling. If bEzUsb is true, use the Cypress EZUSB IOCTL codes,
// otherwise use the "standard" IOCTL codes.
bool UsbManagerWin32::readDescriptors( HANDLE handle, SUsbDescriptors * descs, bool bEzUsb )
{
    BOOL    bSuccess;
    DWORD   nBytes, nBufSize, nRequired;
    char*   buf;
    bool bResult=false;

    // device must be open prior to calling
    if (handle == INVALID_HANDLE_VALUE)
    {
        Debug::trace("UsbManager.Attach", HobString("readDescriptors: invalid handle"));
        return bResult;
    }

    // init buffer pointer and available bytes
    buf = descs->buf;
    nBufSize = sizeof(descs->buf);

    // call custom ioctl in driver to retrieve device descriptor
    bSuccess = DeviceIoControl(handle,
        bEzUsb ? IOCTL_Ezusb_GET_DEVICE_DESCRIPTOR : IOCTL_USB_GET_DEVICE_DESCRIPTOR,
        NULL, 0,
        buf, sizeof(USB_DEVICE_DESCRIPTOR),
        &nBytes,  NULL);

    if ( !bSuccess )
    {
        DWORD err = GetLastError();
        Debug::trace("UsbManager.Attach", HobString("readDescriptors: get device descriptor failed, err ") + Hex::toString(err));
        return bResult;
    }

    // set device desc struct pointer to point to buf
    descs->deviceDesc = (PUSB_DEVICE_DESCRIPTOR) buf;

    // verify we got the entire descriptor from the driver
    DBGASSERT(nBytes == descs->deviceDesc->bLength);

    // point past the device descriptor
    // and reduce the available bytes
    nBufSize -= nBytes;
    buf += nBytes;

    // Since it is not possible to know the size of the entire
    // configuration descriptor data in advance, we call the ioctl with
    // the buffer set to the maximum possible size. The wTotalLength
    // field reflects the number of bytes in the full descriptor hierarchy.

    bSuccess = DeviceIoControl(handle,
        bEzUsb ? IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR : IOCTL_USB_GET_CONFIG_DESCRIPTOR,
        NULL, 0,
        buf, nBufSize,
        &nBytes, NULL);

    if (!bSuccess)
    {
        DWORD err = GetLastError();
        Debug::trace("UsbManager.Attach", HobString("readDescriptors: get config descriptor failed, err ") + Hex::toString(err));
        return bResult;
    }

    //DBGASSERT(nBytes == sizeof(USB_CONFIGURATION_DESCRIPTOR));
    nRequired = ((PUSB_CONFIGURATION_DESCRIPTOR) buf)->wTotalLength;
    //DBGASSERT(nRequired >= sizeof(USB_CONFIGURATION_DESCRIPTOR));

    // Call custom ioctl in driver to retrieve the full config descriptor
    // This returns the entire configuration hierarchy, which includes
    // all related interface and endpoint descriptors.
    bSuccess = DeviceIoControl(handle,
        bEzUsb ? IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR : IOCTL_USB_GET_CONFIG_DESCRIPTOR,
        NULL, 0,
        buf, nRequired,
        &nBytes, NULL);

    if ( !bSuccess )
    {
        DWORD err = GetLastError();
        Debug::trace("UsbManager.Attach", HobString("readDescriptors: get config descriptor failed, err ") + Hex::toString(err));
        return bResult;
    }

    // set config desc pointer
    descs->configDesc = (PUSB_CONFIGURATION_DESCRIPTOR) buf;

    // verify we got the entire descriptor hierarchy from the driver
    DBGASSERT(nBytes == descs->configDesc->wTotalLength);

    // set the interface desc pointer
    descs->interfaceDesc = (PUSB_INTERFACE_DESCRIPTOR) (buf + descs->configDesc->bLength);

    // point to the first endpoint descriptor
    PUSB_ENDPOINT_DESCRIPTOR pEP = (PUSB_ENDPOINT_DESCRIPTOR) ((char*)descs->interfaceDesc + descs->interfaceDesc->bLength);

    // determine # bytes remaining in buffer
    nBufSize = (DWORD)((size_t)(pEP) - (size_t)(&descs->buf[0]));

    // add endpoint descriptor pointers to vector
    for (int i=0; i < descs->interfaceDesc->bNumEndpoints; i++)
    {
        descs->endpointDescs.push_back( pEP );
        pEP++;
        // guard against overrun
        DBGASSERT(nBufSize >= sizeof(USB_ENDPOINT_DESCRIPTOR));
        nBufSize -= sizeof(USB_ENDPOINT_DESCRIPTOR);
    }
    Debug::trace("UsbManager.Attach", HobString("readDescriptors: descriptors loaded"));
    bResult = true;
    return bResult;
}

// return true if the class guid is one we're interested in.
bool UsbManagerWin32::isSupportedClassGuid(GUID& guid)
{
    bool supported  = false;

    for (UINT i=0; i <  UsbManager::dev_entries/sizeof(GUID); i++)
    {
        if (IsEqualGUID(guid, SupportedClassGuids[i]))
        {
            supported = true;
            break;
        }
    }
    return supported;
}

using namespace std;
// Parse the devName string to derive the required substrings.
// Concatenate the substrings to form a device instance ID string.
// Compute and return the unique node value based on the instance ID.
USBD_NODE_ID UsbManagerWin32::NodeIdFromDeviceName(TCHAR * devName)
{
    USBD_NODE_ID nodeId = 0;

    Debug::trace("UsbManager.Attach", HobString("NodeIdFromDeviceName: "));

    // convert to ascii and force upper case
    std::string s;
    Util::wchartostring(s, devName);
    for ( std::string::size_type i = 0; i < s.size() ; ++i )
        s[i] = (char)::toupper(s[i]);

    // parse dbcc_name components
    string sVidPid = s.substr(s.find("VID_"),8).c_str();
    sVidPid += "&";
    sVidPid += s.substr(s.find("PID_"),8).c_str();
    if (s.find("MI_") != string::npos)
    {
        sVidPid += "&";
        sVidPid += s.substr(s.find("MI_"),7).c_str();
    }

    string sUid;
    sUid = s.substr(0, s.rfind("#")); // trim right portion (interface class guid)
    sUid = sUid.substr(sUid.rfind("#")+1); // trim left portion (enum/vid/pid/mi etc)

    // create device instance ID from components
    // USB\VID_1241&PID_E000\5&1DC3342F&0&2
    string devInstId = string("USB\\") + sVidPid + string("\\") + sUid;

    // hash device instance ID into node value.
    CUsbNode *UsbNode = new CUsbWinNode;
    if (NULL == UsbNode)
        return NULL;
    nodeId = UsbNode->createNodeId((char*) devInstId.c_str());

    Debug::trace("UsbManager.Attach", HobString("NodeIdFromDeviceName: '") + devInstId + string("' -> node ") + Hex::toString((ULONG)nodeId));
    delete UsbNode;

    return nodeId;
}


//
// HandleDeviceNotification
//
// Purpose:
//
// Handle device notification events.
//
// Parameters:
//
//  PEVENT_INFO
//      Pointer to EVENT_INFO structure containing the following:
//        HANDLE service;
//          Service handle for registering/unregistering additional notifications.
//        DWORD  event_dat;
//          lParam from WM_DEVICECHANGE message
//        DWORD  event_type;
//          wParam from WM_DEVICECHANGE message
//
// Description:
//
// The main service thread registers for notifications from all interfaces at
// startup. The PnP Manager sends device notifications to the service whenever
// an interface is added or removed. The service handle is passed in in case
// we wish to register for additional notifications here.
//
//      Notification types handled:
//
//          DBT_DEVICEARRIVAL
//              device interface attached
//
//          DBT_DEVICEREMOVECOMPLETE
//              device interface remove completed
//
// DBT_DEVICEARRIVAL occurs when the USB device is plugged in.  On
// receiving this notification, a CUsbNode is created, populated,
// and added to the map. If a firmware update is required by the
// device, the apppropriate IUsbCodeUpdater object is created.
// Otherwise, the RTC is notified to create a service class
// instance for the device.
//
// DBT_DEVICEREMOVECOMPLETE occurs when the USB device is unplugged.
// On receiving this notification, if an update was in progress, it is
// terminated and the IUsbCodeUpdater object deleted. Otherwise, the
// RTC is notified to delete the service class instance for the device.
// Finally, the node is removed from the map.
//
// When a device is added or removed, UpdateMap is called to
// re-enumerate all attached devices, in order to maintain agreement
// between the map and the hardware.
//
// http://msdn.microsoft.com/en-us/library/aa363480%28VS.85%29.aspx
//
//

void UsbManagerWin32::HandleDeviceNotification(PEVENT_INFO ev_info, int32_t vid, int32_t pid)
{
//	Debug::trace("UsbManager.Attach", HobString("UsbManager::HandleDeviceNotification():  Entry"));

    PEVENT_INFO ev =   ev_info;
    if (NULL == ev)
    {
        OutputDebugStringA("UsbManager::HandleDeviceNotification: Invalid param passed\r\n");
        return;
    }

    UsbCritSec.Enter();

    LPVOID i_EventData = ev->event_dat;
    DWORD i_EventType = ev->event_type;
    PDEV_BROADCAST_HDR lpbh = (PDEV_BROADCAST_HDR) i_EventData;
    try
    {
        if (lpbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
        {
            //
            // Device interface changed
            //

            PDEV_BROADCAST_DEVICEINTERFACE lpdbi = (PDEV_BROADCAST_DEVICEINTERFACE) i_EventData;

            if (  i_EventType == DBT_DEVICEARRIVAL
              ||  i_EventType == DBT_DEVICEREMOVECOMPLETE )
            {
                bool bAttach = (i_EventType == DBT_DEVICEARRIVAL);

#if 0	//	HTA-63 & HTA-110:  This is the old search for Service & Supervisor keys (Windux). Python scripts now handle this for the HT series.
//                // Initiate a search for key files whenever a drive is added or
//                // removed. The key file detection logic generates the appropriate
//                // notification message.
//                if (lpdbi->dbcc_classguid == GUID_CLASS_WINDOWS_DISK_DEVICE)
//                {
//                    if (  i_EventType == DBT_DEVICEARRIVAL )
//                    {
//                        Debug::trace("UsbManager.KeySearch", HobString("HandleDeviceNotification: disk device attached ")  );
//                    }
//                    else // i_EventType == DBT_DEVICEREMOVECOMPLETE
//                    {
//                        Debug::trace("UsbManager.KeySearch", HobString("HandleDeviceNotification: disk device removed ")  );
//                    }
//
//					EnableKeySearch(true);
//                    goto leave;
//                }
#endif
                // dump notification struct
                {
                    static wchar_t ws[MAX_PATH];
                    string str;
                    Debug::trace("UsbManager.Attach", string(bAttach ? "attach" : "remove"));
                    wsprintf(ws, L"%s", lpdbi->dbcc_name);
                    Util::wchartostring(str, ws);
                    Debug::trace("UsbManager.Attach", str );
//                    GUID* pguid = &(lpdbi->dbcc_classguid);
//                    wsprintf(ws, L"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
//                             pguid->Data1,    pguid->Data2,    pguid->Data3,
//                             pguid->Data4[0], pguid->Data4[1], pguid->Data4[2], pguid->Data4[3],
//                             pguid->Data4[4], pguid->Data4[5], pguid->Data4[6], pguid->Data4[7]);
//                    Util::wchartostring(str, ws);
//                    Debug::trace("UsbManager.Attach",str);
					//Sleep(200); // allow time to flush debug output
                }

                // Notifications occur for devices of ALL interface classes.
                // We are only interested in those devices which expose
                // selected interface classes. If the device's interface class
                // isn't one that we are interested in, do nothing.

                if (!isSupportedClassGuid(lpdbi->dbcc_classguid))
                {
                    Debug::trace("UsbManager.Attach","HandleDeviceNotification: unsupported interface class, ignored ");
                    goto leave;
                }

                // At this point, we know that a supported device is involved.
                // create a temporary nodeId for the device, and search the map for it.
                USBD_NODE_ID nodeId = NodeIdFromDeviceName((TCHAR*)lpdbi->dbcc_name);
                if (!nodeId)
                {
                    Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: failed to get node id from notification, ignoring device event"));
                    goto leave;
                }

                CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );

                if ( iter != usbNodeMap.end() )
                {
                    // the nodeId was found in our map

                    Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: node ") + Hex::toString((long)nodeId) + HobString(" was found in map"));
                    if ( i_EventType == DBT_DEVICEARRIVAL )
                    {
                        // got device arrival notification, but node was already in map, error/ignore
                        Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: device arrival, node ") + Hex::toString((long)nodeId)  + HobString(" found in map, ignoring "));
                    }
                    else if ( i_EventType == DBT_DEVICEREMOVECOMPLETE )
                    {
                        // got device remove notification for a device in the map, remove from map
                        UpdateDeviceMap();
						Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: device remove complete, node ") + Hex::toString((long)nodeId) + HobString(" found in map, updating device map") );
                    }
                }
                else
                {
                    // the nodeId is not in our map.

                    Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: node ") + Hex::toString((long)nodeId) + HobString(" not found in map"));
                    if ( i_EventType == DBT_DEVICEARRIVAL )
                    {
                        // got device arrival notification, but node was not found in map.
                        // rescan attached devices and add new UsbNodes to map
                        Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: device arrival, node ") + Hex::toString((long)nodeId)  + HobString(" not found in map, updating device map"));
                        UpdateDeviceMap();
                    }
                    else if ( i_EventType == DBT_DEVICEREMOVECOMPLETE )
                    {
                        // device was removed but it was not in our map, ignore
                        Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: device remove complete, node ") + Hex::toString((long)nodeId) + HobString(" not found in map, ignoring "));
                    }
                }
            }
            else
            {
                // unrecognized event type
                Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: ??? interface change event type ") + Hex::toString((long)i_EventType));
            }
        }

        else // unknown lpbh->dbch_devicetype
        {
            // unrecognized notification type
            Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: ??? dbch_devicetype ") + Hex::toString((long)lpbh->dbch_devicetype));
        }
    }
    catch (...)
    {
		MessageLogger::logMessage(MessageLogger::InfoLog, "HandleDeviceNotification():  Exception");
        Debug::trace("UsbManager.Exception",  HobString("HandleDeviceNotification: caught exception "));
        LogCatch;
    }
leave:
    //Debug::trace("UsbManager.Attach", HobString("HandleDeviceNotification: exit"));
    UsbCritSec.Leave();
//	Debug::trace("UsbManager.Attach", HobString("UsbManager::HandleDeviceNotification():  Exit"));
}

// selectively disable warning for 'while (1)' loop
#pragma warning ( push )
#pragma warning ( disable:4127 )

// UpdateDeviceMap identifies the USB devices currently attached to the
// system,and maintains the usbNodeMap in agreement. It creates and
// initializes a CUsbNode for each detected device. If the node is not
// already in the usbNodeMap, it adds the CUsbNode to the map.
//
// After the map has been updated, it walks the map and for managed
// devices, it launches the code updater for devices requiring an initial
// download, otherwise it notifies RTC to start the service class for the
// device. Lastly, it removes any nodes which were already in the map but
// were not detected, terminating the updater or notifying the RTC if needed.
void UsbManagerWin32::UpdateDeviceMap()
{
//	Debug::trace("UsbManager.Attach", HobString("UsbManager::UpdateDeviceMap(): entry"));

    CUsbNode* pUsbNode = 0;

    // clear detected flag on all nodes
    for (CUsbNodeMap::iterator i = usbNodeMap.begin(); i != usbNodeMap.end(); i++)
    {
        CUsbNode* p = i->second;
        if (p)
        {
            p->bDetected = false;
        }
    }

    // Walk the array of supported interface class guids
    int nClassGuids = (int) dev_entries/sizeof(GUID);
    for (int nEnum=0; nEnum < nClassGuids; nEnum++)
    {
        // Build a device information set for all currently attached devices
        // which expose an interface that matches the currently indexed
        // interface class guid.

        HANDLE hDevInfo = SetupDiGetClassDevs(
            &(SupportedClassGuids[nEnum]),
            NULL,
            NULL,
            DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

        if (hDevInfo == INVALID_HANDLE_VALUE)
        {
            Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: SetupDiGetClassDevs returned invalid handle"));
            continue; // ->next interface class guid
        }

        // initialize the index into the device information set
        int MemberIndex = 0;

        while (1)
        {
            // Load an SP_DEVICE_INTERFACE_DATA struct for the nth
            // detected device in the device information set. This struct
            // is passed into subsequent SetupDiXXX calls to specify the
            // interface of the nth element in the device information set.

            SP_DEVICE_INTERFACE_DATA devIntfcData;

            // The struct's cbSize field must be set prior to API call
            devIntfcData.cbSize = sizeof(devIntfcData);

            BOOL Result = SetupDiEnumDeviceInterfaces(
                hDevInfo,                      //IN HDEVINFO  DeviceInfoSet,
                0,                             //IN PSP_DEVINFO_DATA  DeviceInfoData  OPTIONAL,
                &(SupportedClassGuids[nEnum]), //IN LPGUID  InterfaceClassGuid,
                MemberIndex,                   //IN DWORD  MemberIndex,
                &devIntfcData);                //OUT PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData

            if (!Result)
            {
                //DWORD error = GetLastError();
                //if (error == ERROR_NO_MORE_ITEMS)
                {
                    // There are no more interfaces matching the specified
                    // device interface class. Move on to the next device
                    // interface class.
                    break;  // out of while loop
                }
            }

            // Load an SP_DEVINFO_DATA struct for the nth detected
            // device in the device information set. This struct is
            // passed into subsequent SetupDiXXX calls to specify the
            // device of the nth element in the device information set.
            // This call never fails because there is always a matching
            // device for each device interface identified in the previous
            // call to SetupDiEnumDeviceInterfaces.

            SP_DEVINFO_DATA devInfoData;
            devInfoData.cbSize = sizeof(devInfoData);
            SetupDiEnumDeviceInfo(
                hDevInfo, //IN HDEVINFO  DeviceInfoSet,
                MemberIndex, //IN DWORD  MemberIndex,
                &devInfoData //OUT PSP_DEVINFO_DATA  DeviceInfoData
                );

            // Create the CUsbNode.

            pUsbNode = new CUsbWinNode();

            if (!pUsbNode)
            {
                // failed to create the node, skip to next device
                Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: failed to create node"));
                goto nextDevice;
            }

            // initialize the node from setup API information
            DEVICE_INFO dev_i;

            dev_i.dev_info = hDevInfo;
            dev_i.dev_if_dat = devIntfcData;
            dev_i.dev_info_dat = devInfoData;
            USBD_NODE_ID nodeId = pUsbNode->Initialize(dev_i);

            if (!nodeId)
            {
                // failed to initialize node
                Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: failed to initialize node"));
                delete pUsbNode;
                pUsbNode = 0;
            }
            else
            {
                CUsbNodeMap::iterator i = usbNodeMap.find(nodeId);
                if ( i == usbNodeMap.end())
                {
                    // Device was detected, but node was not
                    // found in map. Add node to the map

//					Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: Adding node to map: ") + Hex::toString((long)nodeId));
                    addToMap(nodeId, pUsbNode);

                    // Set initial state to DETECTED to indicate
                    // the device has just been added.
                    pUsbNode->state = CUsbNode::DETECTED;

                    // set flag to prevent removal below
                    pUsbNode->bDetected = true;
                }
                else
                {
                    // Device was detected, but is already in map.
//                    Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: device already in map at node ") + Hex::toString((long)nodeId));
                    // Delete the temp object and flag the node as detected.
                    delete pUsbNode;
                    pUsbNode = 0;
                    // set flag to prevent removal below
                    i->second->bDetected = true;
                }
            }
nextDevice:
            // Examine the next interface in the device information set.
            MemberIndex++;
        }

        // Required call to release memory which was internally
        // allocated during SetupDiGetClassDevs.
        SetupDiDestroyDeviceInfoList(hDevInfo);
    } // for (supported interface class guid array)

    // Walk the map. Remove nodes that were not detected.
    // Launch updater/service classes if needed.
    //
    // pseudo code:
    //  if bDetected==false
    //      node was not detected in the scan we just performed above.
    //      handle cleanup/delete for associated updater or service class
    //      remove the node from the map
    //  else
    //      if node state == DETECTED
    //          if managed device
    //              if (needs update)
    //                  create updater
    //                  state = UPDATING
    //              else
    //                  create service class
    //                  state = RUNNING
    //          else
    //              state = UNMANAGED

    for (CUsbNodeMap::iterator i = usbNodeMap.begin(); i != usbNodeMap.end();)
    {
        CUsbNode* p = i->second;
        USBD_NODE_ID nodeId = i->first;
        bool bRemoved = false;
        if (p)
        {
            if (!p->bDetected)
            {
                // device in map was not detected
                // do some cleanup and remove from map

                if (p->pUpdater)
                {
                    DeleteUpdater(nodeId, p);
                    ++i;
                    removeFromMap(nodeId);
                    bRemoved = true;
                }
                else if (p->bServiceObject)
                {
                    DeleteCommMgr(nodeId, p);
                    ++i;
                    removeFromMap(nodeId);
                    bRemoved = true;
                }
                else
                {
                    ++i;
                    removeFromMap(nodeId);
                    bRemoved = true;
                }
                if (p->state == CUsbNode::HID_KEYBOARD)
                {
                    Debug::trace("UsbManagerWin32.Attach", "HID Keyboard device removed...");
                    HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::KEYBOARD_REMOVED);

                    foundKeyboard = false;

                    dispatchEvent(&deviceEvent);
                }
                else if(p->state == CUsbNode::HID_MOUSE)
                {
                    Debug::trace("UsbManagerWin32.Attach", "HID Mouse device removed...");
                    HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::MOUSE_REMOVED);

                    foundMouse = false;

                    dispatchEvent(&deviceEvent);
                }
                else if(p->state == CUsbNode::HID_BARCODE)
                {
                    Debug::trace("UsbManagerWin32.Attach", "HID barcode scanner device removed...");
                    HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::BARCODE_SCANNER_REMOVED);

                    foundBarcodeScanner = false;

                    dispatchEvent(&deviceEvent);
                }
            }
            else
            {
                // Node was detected in rescan above.

                if (p->state == CUsbNode::DETECTED)
                {
                    // Node state = DETECTED means device was just added,
                    // If the device PID is in our list of managed devices,
                    // then create an updater or service class for it.
                    USHORT vid = getVendorId(nodeId);
                    USHORT pid = getProductId(nodeId);

                    if (isManagedDevice(vid, pid))
                    {
                        if (isCodeUpdateRequired(vid, pid))
                        {
                            // Code update is required for this device,
                            // so create an updater for it. The updater
                            // begins to run at the point it is constructed.

                            Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: creating device updater for node ") + Hex::toString((long)nodeId));
                            //string s = string("UsbManager::UpdateDeviceMap: creating device updater for node ") + Hex::toString((long)nodeId) + "\r\n";
                            //OutputDebugStringA(s.c_str());

                            // Set the node state accordingly.
                            p->state = CUsbNode::UPDATING;

							p->pUpdater = IUsbCodeUpdater::UpdaterFactory(nodeId);

                            if (!p->pUpdater)
                            {
                                // Failed to create updater class for this device,
                                // mark node as zombie. Zombie nodes are removed from
                                // the map when the device removal notification comes in.
                                Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: failed to create device updater for node ") + Hex::toString((long)nodeId));
                                p->state = CUsbNode::ZOMBIE;
                            }
                        }
                        else
                        {
                            // A firmware update is not required for this device.
                            // Notify the RTC that this device was attached. RTC
                            // launches the appropriate service class for the device.

                            // Set the node state accordingly.
                            p->state = CUsbNode::RUNNING;
                            Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: creating comm manager for node ") + Hex::toString((long)nodeId));
                            //string s = string("UsbManager::UpdateDeviceMap: creating comm manager for node ") + Hex::toString((long)nodeId) + "\r\n";
                            //OutputDebugStringA(s.c_str());
                            if (!AttachDevice( nodeId ))
                            {
                                // Failed to create service class for the device,
                                // mark node as zombie. Zombie nodes are removed from
                                // the map when the device removal notification comes in.
                                Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: failed to create device service class for node ") + Hex::toString((long)nodeId));
                                p->state = CUsbNode::ZOMBIE;
                            }
                        }
                    }
                    else if (p->getServiceName() == "MOUHID")
                    {
                        Debug::trace("UsbManagerWin32.Attach", HobString("UpdateDeviceMap: adding mouse to map ") + Hex::toString((long)nodeId));
                        p->state = CUsbNode::HID_MOUSE;
                        HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::MOUSE_ATTACHED);

                        foundMouse = true;

                        //notify the session of the event
                        dispatchEvent(&deviceEvent);
                    }
                    else if (p->getServiceName() == "KBDHID")
                    {
                    	Debug::trace("UsbManagerWin32.Attach", HobString("UpdateDeviceMap: adding keyboard to map ") + Hex::toString((long)nodeId));
                    	p->state = CUsbNode::HID_KEYBOARD;
                    	HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::KEYBOARD_ATTACHED);

                    	foundKeyboard = true;

                        //notify the session of the event
                        dispatchEvent(&deviceEvent);
                    }
                    else if (p->getServiceName() == "BARCODE")
                    {
                    	Debug::trace("UsbManagerWin32.Attach", HobString("UpdateDeviceMap: adding barcode scanner to map ") + Hex::toString((long)nodeId));
                    	p->state = CUsbNode::HID_BARCODE;
                    	HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::BARCODE_SCANNER_ATTACHED);

                    	foundBarcodeScanner = true;

                        //notify the session of the event
                        dispatchEvent(&deviceEvent);
                    }
                    else
                    {
                        //  not a managed device, nothing to do
//                        Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap: adding unmanaged device at node ") + Hex::toString((long)nodeId));
                        p->state = CUsbNode::UNMANAGED;
                    }
                }
            }
        } // if p
        else
        {
            // node pointer was null, remove node from map
            Debug::trace("UsbManager.Attach",  HobString("UpdateDeviceMap: null pointer found at node ") + Hex::toString((ULONG)nodeId) + HobString(", removing node"));
            ++i;
            removeFromMap(nodeId);
            bRemoved = true;
        }

        // if the current node was not
        // removed, bump the iterator

        if (!bRemoved)
            ++i;
    } // for iterator
//	Debug::trace("UsbManager.Attach", HobString("UpdateDeviceMap():  Exit"));
}

using namespace util_filesystem;

// Search all removable drives for key files.
// If a key file is found, change its state to FOUND.
//TO DO: key search function should be place in the USBManager using Linux version (PAL)
//TO DO: modify for new fastrack key search method
bool UsbManagerWin32::searchForKeyFiles()
{
    bool bFound = false;
    char* buffer = 0;
    int stringSize = 0, readSize = 0, pPtr = 0;

    stringSize = GetLogicalDriveStringsA(0, buffer);
    buffer = new char[stringSize];
    readSize = GetLogicalDriveStringsA(stringSize, buffer);
    //Debug::trace("UsbManager.Attach", HobString("searchForKeyFiles:: drive string size = ") + Integer::toString((ULONG)readSize) );

    while (pPtr < readSize)
    {
        if (GetDriveTypeA(&buffer[pPtr]) == DRIVE_REMOVABLE)
        {
            string driveName (&buffer[pPtr]);

            Directory servDir ("");
            servDir.appendPath(driveName);

#if 0	//	HTA-63 & HTA-110:  This is the old search for Service & Supervisor keys (Windux). Python scripts now handle this for the HT series.
//            File serviceKeyFile(servDir.getFullPath(), Configuration::getInstance()->getServiceKeyFile());
//
//            //Debug::trace("UsbManager.Attach", HobString("searchForKeyFiles: searching for service key at ") + serviceKeyFile.getFullPath() );
//
//            if (checkForKeyValue(serviceKeyFile))
//            {
//				/*we want to send the key found state even if we are in notified state
//				  as the UI may have missed a notification */
//                if ( KEY_FILE_STATE_UNKNOWN == keyFileState[KEY_FILE_TYPE_SERVICE]
//                  || KEY_FILE_STATE_REMOVED == keyFileState[KEY_FILE_TYPE_SERVICE] 
//				  || KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE])
//                {
//					if ( serviceKeyFile.size() == 42 )
//					{
//						Debug::trace("UsbManager.KeySearch", HobString("searchForKeyFiles: found service key at ") + serviceKeyFile.getFullPath() );
//						keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_FOUND;
//						bFound = true;
//					}
//					else
//					{
//						Debug::trace("UsbManager.KeySearch", HobString("searchForKeyFiles: found service key at ") + serviceKeyFile.getFullPath() );
//						keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_FOUND;
//						bFound = true;
//					}
//                }
//            }
//
//            File superKeyFile(servDir.getFullPath(), Configuration::getInstance()->getSuperKeyFile());
//
//            //Debug::trace("UsbManager.Attach", HobString("searchForKeyFiles: searching for supervisor key at ") + superKeyFile.getFullPath() );
//
//            if (checkForKeyValue(superKeyFile))
//            {
//				/*we want to send the key found state even if we are in notified state
//				  as the UI may have missed a notification */
//                if ( KEY_FILE_STATE_UNKNOWN == keyFileState[KEY_FILE_TYPE_SUPERVISOR]
//                  || KEY_FILE_STATE_REMOVED == keyFileState[KEY_FILE_TYPE_SUPERVISOR] 
//				  || KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE])
//                {
//                    Debug::trace("UsbManager.KeySearch", HobString("searchForKeyFiles: found supervisor key at ") + superKeyFile.getFullPath() );
//                    keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_FOUND;
//                    bFound = true;
//                }
//            }
#endif

#ifdef USE_ACCOUNTANT
            File accountantKeyFile(servDir.getFullPath(), "accountant\\accountant.dat");

            if (checkForKeyValue(accountantKeyFile))
            {
                util_acc::showInstanceCounters(false);
            }
#endif
#ifdef USB_TRIGGER_MULTIPLE_TEST_LABELS
            File labelTriggerKeyFile(servDir.getFullPath(), "labelTrigger\\key.dat");

            if (checkForKeyValue(labelTriggerKeyFile))
            {
                static int toggle = 0;

                if ((toggle % 2) == 0)
                {
                    PrinterPrintTestLabel testLabel(this, FirstPrinter, PrVerticalLines, PrStartContinuous);
                    dispatchEvent(&testLabel);
                }
                else
                {
                    PrinterPrintTestLabel testLabel(this, FirstPrinter, PrVerticalLines, PrStopContinuous);
                    dispatchEvent(&testLabel);
                }
                toggle++;
            }
#endif
        }
        pPtr += (int)(strlen (&buffer[pPtr]) + 1);
    }

    delete[] buffer;

    //Debug::trace("UsbManager.Attach", HobString("searchForKeyFiles: search pass complete. ") );

    return bFound;
}


// Check if there is any USB Storage Device (thumb drive) present in the system.
// Need this method because we don't want to shutdown with one present,
// since the scale will try to boot from it the next time it boots.
// *****************
// Note:  This method is not yet implemented for Windows.
//        Will need to revisit this when Windows testing begins (again).
//        A Tracker entry has been created for this situation.
// *****************
bool UsbManagerWin32::isBulkStorageDevicePresent() const
{
    return false;  
}

// SOF-2931
bool UsbManagerWin32::isSambaPresent() const
{
	return false;  //addSamba;
}

void UsbManagerWin32::resetSamba()
{
}
#endif  // PAL_WIN32_RUN
