#include "EvolMessage.h"
#include "UsbManager.h"
#include "VersionManagerEvent.h"
#include "VmStatusMsg.h"
#include "PeripheralRequestCodeUpdate.h"
#include "PeripheralCodeUpdate.h"

#include "filesys/file.h"
#include "filesys/directory.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"
#include "HobVersion.h"
#include "FCError.h"

#include "AtmelDeviceHandler.h"

#include "SessionManagerHelper.h"
#include "Configuration.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int AtmelDeviceHandler::_dataType_ = 0;
std::string AtmelDeviceHandler::_dataTypeName_("AtmelDeviceHandler");
#endif

#if !defined ( __linux__ )
using namespace AT91BOOTDLL_Lib;
#endif
using namespace std;

//static
AtmelDeviceHandler * AtmelDeviceHandler::instance=0;

// singleton. returns instance pointer
AtmelDeviceHandler *AtmelDeviceHandler::getInstance()
{
    if (instance == 0)
    {
        instance = new AtmelDeviceHandler();
    }
    return instance;
}


AtmelDeviceHandler::AtmelDeviceHandler()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);
	util_acc::addInstance(dataType);
#endif

    Debug::trace("AtmelDeviceHandler.Clare", HobString("Constructor"));

    bComInitialized = false;
    bNodesRemoved = false;
    isPrepack = Configuration::getInstance()->isPrepackScale();
    processingDevice = false;
    usbNodeId = 0;
    procState = S_IDLE;
    hOpenDevice = 0;
	needsUpdate = false;

    // Allocate device path buffers for the DLL. DevListPtr is used
    // to contain the malloc pointers. DevListPtr is copied to DevList
    // just prior to passing it to the DLL. This is necessary because
    // the DLL overwrites the array of pointers that is passed in.

    memset(DevListPtr, 0, sizeof(DevListPtr));
    memset(DevList, 0, sizeof(DevList));
    for (int i=0; i< MAX_NUM_AT91_DEVICES; i++)
    {
        DevListPtr[i] = (char *) malloc(100);
    }

    // COM initialization must occur on the worker thread context,
    // i.e. from within the processEvent method. Therefore we send
    // ourselves a private message to initialize.
    SendPrivateMessage(PrivateMsg_t::Init);

    // register to receive VersionManager events
    registerEvent(VersionManagerDev, PeripheralUpdateCode);
    registerEvent(VersionManagerDev, BeginUpgrade);
    registerEvent(VersionManagerDev, CancelUpgrade);
	registerEvent(VersionManagerDev, ResetFPCComponent);
}

AtmelDeviceHandler::~AtmelDeviceHandler()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("AtmelDeviceHandler.Clare", HobString("Destructor"));

    cancelTimer(WaitTimerId);
    cancelTimer(RemovalTimerId);

#if 0
    // TODO: how to invoke Couninitialize on worker thread context from ~AtmelDeviceHandler?
    // COM uninitialization must occur on the worker thread context,
    // i.e. from within the processEvent method (MessageCheckingThread.
    SendPrivateMessage(PrivateMsg_t::Deinit);
#endif

    // Free the device path buffers which were
    // allocated in the constructor.
    for (int i=0; i< MAX_NUM_AT91_DEVICES; i++)
    {
        if (DevListPtr[i])
        {
            free(DevListPtr[i]);
            DevListPtr[i] = 0;
        }
    }
}

void AtmelDeviceHandler::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    Debug::trace("AtmelDeviceHandler.run", HobString("processEvent: "));

    bool bRunFSM = true;

    if ( msg.get() != NULL )
    {
        if (HandlePrivateMessage(msg))
        {
            // private message was handled
        }
        else if (HandlePublicMessage(msg))
        {
            // public message was handled
        }
        else
        {
            Debug::trace("AtmelDeviceHandler.error",
							HobString("processEvent:  Received unknown msg:  ")
                    + HobString("device 0x")
                    + Hex::toString((long)msg->getDeviceType())
							+ HobString(",  event 0x")
                    + Hex::toString((long)msg->getEventType()));
            bRunFSM = false;
        }

        if (bRunFSM)
            RunFSM();
    }
}

void AtmelDeviceHandler::RunFSM()
{
    ProcState_t nextState = procState;
    bool bContinue = true;

    switch (procState)
    {
	    case S_IDLE:
	        if (usbNodeList.size())
	        {
	            // If any devices are connected, begin processing
				Debug::trace("AtmelDeviceHandler.run", HobString("S_IDLE:  Devices attached"));
	            nextState = S_SCAN;
	        }
	        else
	        {
	            // If no devices are connected, discontinue processing.
				Debug::trace("AtmelDeviceHandler.run", HobString("S_IDLE:  No devices attached."));
	            bContinue = false;
	        }
	        break;

	    case S_SCAN:
		{
bool fakeIt = Configuration::getInstance()->toBoolCfgI("fakeAtmelDevice");
if (!fakeIt)
{

	        if (ScanDevices())
	        {
	            // At least one device was detected by the DLL. Continue processing
	            // by reading and parsing the on-chip data.
				Debug::trace("AtmelDeviceHandler.run", HobString("S_SCAN:  Scan found devices"));
	            nextState = S_IDENT;
	        }
	        else
	        {
	            // No devices were found by the DLL.
	            // Continue processing in the IDLE state
	            // so that the usbNodeList is re-checked.
	            //
	            // A holdoff interval is required to allow time for the following
	            // sequence of events to complete:
	            //      AT91 device detaches from bus (following application firmware launch)
	            //      PnP manager removes SAM-BA driver (atm6124.sys) instance
	            //      PnP manager sends device removal notification for the node
	            //      UsbManager receives device removal notification for the node
	            //      UsbManager destroys the AtmelSambaUpdater instance associated with the node
	            //      AtmelSambaUpdater destructor calls AtmelDeviceHandler::DeviceRemoval
	            //      AtmelDeviceHandler::DeviceRemoval removes the node from usbNodeList
	            //
	            // TODO: enhance by using a semaphore to sync DeviceArrival/Removal and FSM.
	            //
				Debug::trace("AtmelDeviceHandler.run", HobString("S_SCAN:  No devices found"));
//                Debug::trace("AtmelDeviceHandler.run", HobString("S_SCAN:  Clearing processDevice+usbNodeId"));
//                processingDevice = false;
//                usbNodeId = 0;
	            nextState = S_IDLE;
	        }
}
else nextState = S_IDENT;
			
	        break;
		}

	    case S_IDENT:
		{
bool fakeIt = Configuration::getInstance()->toBoolCfgI("fakeAtmelDevice");
if (!fakeIt)
{
	        if (bNodesRemoved)
	        {
	            // A mapped node has been removed. Since there is
	            // no way to know *which* of the device paths this
	            // node was associated with, we re-enter the SCAN state
	            // and rescan all AT91 devices present.  This invalidates
	            // the device path list.
	            m_cs.Enter();
				Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  USB node removed"));
	            bNodesRemoved = false;
//                processingDevice = false;
//                usbNodeId = 0;
	            m_cs.Leave();
	            nextState = S_SCAN;
	            break;
	        }
	        DevPath = GetNextDevicePath();
	        if (!DevPath)
	        {
	            // There are no more devices in the previously scanned list.
	            // Continue processing by entering the SCAN state
	            // and requesting the DLL re-scan for devices.
	            Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT: no more device paths in list"));
	            nextState = S_SCAN;
	            break;
	        }

	        // Open and identify the device
	        hOpenDevice = OpenDevice(DevPath);
	        if (NULL == hOpenDevice || !Identify(hOpenDevice, ProductInfo))
	        {
	            // The device failed identification. Close the device and
	            // continue processing in the IDENT state. The next device
	            // path found in the list will be identified.
	            CloseDevice(hOpenDevice);
	            hOpenDevice = NULL;
	            Debug::trace("AtmelDeviceHandler.Clare", HobString("S_IDENT:  Identify failed, skipping to next device."));
	            break;
	        }
}

			Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  Device identified."));

	        // The device identified successfully. Send a message to the
	        // VersionManager requesting a firmware update.

			Debug::trace("AtmelDeviceHandler.Clare", HobString("S_IDENT:  Requesting code update from VersionManager"));

	        // Reset flags
	        bWaitTimeout = bGotCodeUpdate = false;

	        // Enable the one-shot timeout counter
	        createTimer(WaitTimerDuration, WaitTimerId, false);

	        // Format and send the request msg to the VersionManager.
			// This return value from RequestCodeUpdate indicates whether we should continue or pause.
	        bContinue = RequestCodeUpdate(ProductInfo);
	#ifndef CHECK_FT

	        // Discontinue processing until the VersionManager
	        // sends a response or timeout occurs.
	        bContinue = false;
			nextState = S_WAITVER;
	#endif
			break;
		}

	    case S_WAITVER:
	        if (bWaitTimeout)
	        {
				Debug::trace("AtmelDeviceHandler.Clare", HobString("S_WAITVER:  Timeout waiting for VersionManager response"));
	            // Timeout occurred while waiting on the VersionManager response
	            // to our request for a firmware update for the current device.
				// Try to launch the device's firmware.
	            if (!LaunchFirmware(hOpenDevice))
	            {
					Debug::trace("AtmelDeviceHandler.error", HobString("S_WAITVER:  Firmware launch failed"));
	                if (bGotCodeUpdate)
	                {
	                    if( isPrepack )
	                    {
	                        ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
						    ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
	                    }
	                    else
	                    {
			                ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
	                    }
					}
//	                detachDevice(usbNodeId);
	                nextState = S_IDENT;
	            }
	            else
	            {
					Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER:  Firmware launched OK"));

					// wait for the device removal
	                // enable the removal timeout counter
	                // and clear the timeout count
	                createTimer(RemovalTimerDuration, RemovalTimerId, true);
	                nRemovalTimeout = 0;
//	                UsbManager::getInstance()->DetachDevice(usbNodeId);
	                nextState = S_DETACH;
	            }
	        }
		    else if (bGotCodeUpdate)
	        {
	            cancelTimer(WaitTimerId);
				Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER:  Got update from VersionManager"));
	            if (VerifyFirmwareFile(FirmwareFilename))
	            {
                    if( isPrepack )
                    {
                        ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
                    }
                    else
                    {
		                ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading);
                    }
					Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER:  File verified"));
	                nextState = S_UPDATE;
	            }
	            else
	            {
                    if( isPrepack )
                    {
                        ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
					    ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
                    }
                    else
                    {
		                ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
                    }
	                Debug::trace("AtmelDeviceHandler.error", HobString("S_WAITVER: invalid file"));
	                nextState = S_IDENT;
	            }
	        }
	        else
	        {
				Debug::trace("AtmelDeviceHandler.Clare", HobString("S_WAITVER:  Did not get update, but was not timeout either."));
	            //nextState = S_IDENT;
	            bContinue = false;
	        }
	        break;

	    case S_UPDATE:
		{
			Debug::trace("AtmelDeviceHandler.Clare", HobString("procState = S_UPDATE:  Downloading firmware"));
	        if (!DownloadFirmware(hOpenDevice, FirmwareFilename))
	        {
				Debug::trace("AtmelDeviceHandler.error", HobString("S_UPDATE:  Failed to download firmware"));
	            if (bGotCodeUpdate)
	            {
                    if( isPrepack )
                    {
                        ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
						ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
                    }
                    else
                    {
						ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, false);
		                ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
		            }
				}
//                detachDevice(usbNodeId);
	            nextState = S_IDENT;
	        }
	        else
	        {
				Debug::trace("AtmelDeviceHandler.run", HobString("S_UPDATE:  Firmware download OK. Launching firmware."));

				//Sleep is needed to allow device finish final page burn before telling it to launch the app
				PAL_absclass::PAL_sleep( 300 );

	            if (!LaunchFirmware(hOpenDevice))
	            {
					Debug::trace("AtmelDeviceHandler.error", HobString("S_UPDATE:  Firmware launch failed"));
	                if (bGotCodeUpdate)
	                {
                        if( isPrepack )
                        {
                            ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
						    ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
                        }
                        else
                        {
							ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, false);
		                    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
		                }
					}
//                    detachDevice(usbNodeId);
	                nextState = S_IDENT;
	            }
	            else
	            {
					Debug::trace("AtmelDeviceHandler.run", HobString("S_UPDATE:  Firmware launched OK"));
	                if (bGotCodeUpdate)
	                {
                        if( isPrepack )
                        {
                            ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
						    ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadSucceeded);
                        }
                        else
                        {
							ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, false);
		                    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadSucceeded);
		                }
					}
	                // wait for the device removal
	                // enable the removal timeout counter
	                // and clear the timeout count
	                createTimer(RemovalTimerDuration, RemovalTimerId, true);
	                nRemovalTimeout = 0;

//	                UsbManager::getInstance()->DetachDevice(usbNodeId);

	                nextState = S_DETACH;
	            }
	        }

			File firmwareFile(FirmwareFilename);
			if (firmwareFile.exists())
				firmwareFile.remove();

	        CloseDevice(hOpenDevice);
	        hOpenDevice = NULL;
	        break;
		}

	    case S_DETACH:
			Debug::trace("AtmelDeviceHandler.run", HobString("procState = S_DETACH"));
	        if (bNodesRemoved)
	        {
	            // A mapped node has been removed.
	            cancelTimer(RemovalTimerId);
	            nRemovalTimeout = 0;
	            m_cs.Enter();
				Debug::trace("AtmelDeviceHandler.run", HobString("S_DETACH:  USB node removed"));
//                processingDevice = false;
//                usbNodeId = 0;
	            bNodesRemoved = false;
	            m_cs.Leave();
	            nextState = S_IDENT;
	        }
	        else
	        {
	            // check how long we have waited for device removal so far
	            unsigned int totalTime = RemovalTimerDuration*nRemovalTimeout;
	            if (totalTime < RemovalTimeoutLimit)
	            {
					Debug::trace("AtmelDeviceHandler.run", HobString("S_DETACH:  Waiting for usb DeviceRemoval"));
	                // continue to wait on timer or device removal
	                bContinue = false;
	                nextState = S_DETACH;
	            }
	            else
	            {
	                // We have exceeded the maximum time for device removal
	                // but we have not received a device removal msg yet.
	                // We assume the application firmware did not succeed
	                // in causing the AT91 device to detach from the bus.
	                // All we can really do in this case is move on to the
	                // next device path that was scanned (if any).
	                cancelTimer(RemovalTimerId);
	                nRemovalTimeout = 0;
					Debug::trace("AtmelDeviceHandler.run", HobString("S_DETACH:  Timeout while waiting for usb DeviceRemoval"));
//                    if( usbNodeId )
//                        DeviceRemoval(usbNodeId);
//                    processingDevice = false;
//                    usbNodeId = 0;
#ifndef CHECK_FT
					Debug::trace("AtmelDeviceHandler.run", HobString("S_DETACH: closing device and moving onto the next device path"));
					CloseDevice(hOpenDevice);
					Debug::trace("AtmelDeviceHandler.run", HobString("S_DETACH: device has been closed.  Moving to IDENT state."));
					hOpenDevice = NULL;
#endif
	                nextState = S_IDENT;
	            }
	        }
	        break;
    }

    Debug::trace("AtmelDeviceHandler.run", HobString(GetProcStateName(procState)) + HobString("->") + HobString(GetProcStateName(nextState)) );

    procState = nextState;
    if (bContinue)
        ContinueProcessing();
}

void AtmelDeviceHandler::UninitializeCOM()
{
    Debug::trace("AtmelDeviceHandler", HobString("UninitializeCOM"));
    if (bComInitialized)
    {
        CoUninitialize();
        bComInitialized = false;
    }
}

bool AtmelDeviceHandler::InitializeCOM()
{
    bool bResult = false;
    Debug::trace("AtmelDeviceHandler", HobString("InitializeCOM"));

    // CoInitialize must be called once per thread
    // if not already called, returns S_OK on success
    // if already called, returns S_FALSE on success
    // other return values indicate COM init failure.
    // Each successful call must be balanced with a
    // matching call to CoUninitialize.

    HRESULT hr = CoInitialize(NULL);
    if (hr == S_FALSE || hr == S_OK)
    {
        bResult = true;
        try
        {
            pDll = IAT91BootDLLPtr(__uuidof(AT91BootDLL));
            Debug::trace("AtmelDeviceHandler", HobString("COM initialized successfully."));
        }
        catch(...)
        {
            Debug::trace("AtmelDeviceHandler.error", HobString("COM init failed. Exception when creating DLL interface."));
        }
    }
    else
    {
        // COM init failed
        Debug::trace("AtmelDeviceHandler.error", HobString("failed to initialize COM."));
    }
    return bResult;
}

void AtmelDeviceHandler::DeviceArrival(USBD_NODE_ID usbNodeId)
{
    Debug::trace("AtmelDeviceHandler", HobString("sending DeviceArrival msg ") + Hex::toString(usbNodeId));
    SendPrivateMessage(PrivateMsg_t::Arrival, usbNodeId);
}

void AtmelDeviceHandler::DeviceRemoval(USBD_NODE_ID usbNodeId)
{
    Debug::trace("AtmelDeviceHandler", HobString("sending DeviceRemoval msg ") + Hex::toString(usbNodeId));

    SendPrivateMessage(PrivateMsg_t::Removal, usbNodeId);
}

// send a private message to force processEvent to run
void AtmelDeviceHandler::ContinueProcessing()
{
    SendPrivateMessage(PrivateMsg_t::Continue);
}

// Call the DLL to scan for connected AT91 devices. The DLL
// builds a list of sym links for the detected devices.
int AtmelDeviceHandler::ScanDevices()
{
    int n = 0;          // local count of USB paths in DevList
    nDevices = 0;       // member count of USB paths in DevList
    DevListIndex = 0;   // member index of current USB path
    DevPath = 0;        // pointer to current USB path

    try
    {
        // if a device is open, close it
        if (hOpenDevice != NULL)
        {
            CloseDevice(hOpenDevice);
            hOpenDevice = NULL;
        }

        // Copy the pointers from the malloc'd array into the working array
        // which is then passed to the AT91Boot_Scan method. This 2-level
        // approach must be done because the DLL overwrites one or more array
        // locations.

        memcpy(DevList,DevListPtr,sizeof(DevList));

        // Call the DLL method.
        pDll->AT91Boot_Scan((unsigned char *)DevList);
    }
    catch(_com_error e)
    {
        Debug::trace("AtmelDeviceHandler.error",
            HobString("ScanDevices: COM error 0x") + Hex::toString(e.Error()) );
        return 0;
    }
    catch (...)
    {
        Debug::trace("AtmelDeviceHandler.error",
            HobString("ScanDevices: failed, error 0x") + Hex::toString(PAL_absclass::PAL_getLastError()) );
        return 0;
    }

    try
    {
        // The DLL returns both USB and non-USB paths in the array, and
        // overwrites an element with a 0 to indicate end-of-data.

        // We scan the returned paths to count only the USB paths.
        // On return, nDevices is set to the count of USB paths found.

        for (int i= 0; i< MAX_NUM_AT91_DEVICES; i++)
        {
            if (!DevList[i])
            {
                // A sentinal value of 0 is written by the DLL method to
                // indicate end-of-data. At this point all paths returned
                // by the DLL have been processed.
                break;
            }

            // convert to STL string
            string s(DevList[i]);

            // force upper case
            transform(s.begin(), s.end(), s.begin(), (int(*)(int)) toupper);

            // filter anything that isn't USB - serial, JLink, etc
            if (s.find("USB\\") != string::npos)
            {
                // bump our USB device count
                n++;
            }
        }
    }
    catch(...)
    {
		Debug::trace("AtmelDeviceHandler.error", HobString("ScanDevices:  Failed, error 0x") + Hex::toString(PAL_absclass::PAL_getLastError()) );
		return 0;
    }

    nDevices = n;

    return nDevices;
}

char * AtmelDeviceHandler::GetNextDevicePath()
{
    char * p = 0;
    int i;

    // return the next available "USB" device path
    for (i= DevListIndex; i< MAX_NUM_AT91_DEVICES; i++)
    {
        if (DevList[i] == 0)
        {
            DevListIndex = MAX_NUM_AT91_DEVICES;
            break;
        }

        string s(DevList[i]);
        transform(s.begin(), s.end(), s.begin(), (int(*)(int)) toupper);
        if (s.find("USB\\") != string::npos)
        {
            p = DevList[i];
            DevListIndex = i+1;
            break;
        }
    }
    return p;
}

// Process the EvolMessage assuming message is our private format.
// If not a private message, return false, else return true.
bool AtmelDeviceHandler::HandlePrivateMessage(Util::CSmartPtr< EvolMessageWrapper> & msg)
{
    bool rc = false;

    PrivateMsg_t* pmsg = (PrivateMsg_t*)(msg->Data());
    int msglen = msg->Length();

    if (!pmsg || msglen != sizeof(PrivateMsg_t))
        return false;

    if (pmsg->magic != msg_magic_value)
        return false;

    rc = true;
    if (pmsg->op == PrivateMsg_t::Continue)
    {
		Debug::trace("AtmelDeviceHandler", HobString("HandlePrivateMessage:  Received Continue msg"));
    }
    else if (pmsg->op == PrivateMsg_t::Init)
    {
		Debug::trace("AtmelDeviceHandler", HobString("HandlePrivateMessage:  Received Initialize msg"));
        bComInitialized = InitializeCOM();
    }
    else if (pmsg->op == PrivateMsg_t::Deinit)
    {
		Debug::trace("AtmelDeviceHandler", HobString("HandlePrivateMessage:  Received Deinitialize msg"));
        UninitializeCOM();
    }
    else if (pmsg->op == PrivateMsg_t::Arrival)
    {
        Debug::trace("AtmelDeviceHandler",
			HobString("HandlePrivateMessage:  Adding usb node ")
                    + Hex::toString(pmsg->usbnode));

        m_cs.Enter();
        // add node to list
        usbNodeList.push_back(pmsg->usbnode);
        m_cs.Leave();
    }
    else if (pmsg->op == PrivateMsg_t::Removal)
    {
        Debug::trace("AtmelDeviceHandler",
			HobString("HandlePrivateMessage:  Removing usb node ")
                    + Hex::toString(pmsg->usbnode));

        // remove node from list
        m_cs.Enter();
        usbNodeListIter i = usbNodeList.begin();
        while (i != usbNodeList.end())
        {
            if (*i == (USBD_NODE_ID) pmsg->usbnode)
            {
                usbNodeList.erase(i);
                break;
            }
        }
        bNodesRemoved = true;
        m_cs.Leave();
    }


    return rc;
}


bool AtmelDeviceHandler::HandlePublicMessage(Util::CSmartPtr< EvolMessageWrapper> & msg)
{
    bool rc = false;
    EvolDeviceType devType = (EvolDeviceType) msg->getDeviceType();
    VersionManagerEvent ev = (VersionManagerEvent) msg->getEventType();

    if (VersionManagerDev == devType)
    {
        switch (ev)
        {
            case PeripheralUpdateCode:
			{
				Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage: Received PeripheralUpdateCode event from Version Manager"));
                    // message contains a PeripheralCodeUpdate object
                    char * recvBuffer = (char *) msg->Data();
                    PeripheralCodeUpdate codeUpdate(recvBuffer);
                    codeUpdate.show();

                if(codeUpdate.getNodeId() == usbNodeId)//check if this message is for us
				{
                    switch (codeUpdate.getResponseType())
                    {
                        case SoftwareSystem::AvailableSameVersion:
					    {
//						    bGotCodeUpdate = true;
//						    bWaitTimeout = false;
//						    needsUpdate = false;
							Debug::trace("AtmelDeviceHandler", HobString("AvailableSameVersion") );
							break;
					    }

						case SoftwareSystem::AvailableUpgrade:                                                 
                        {
							Debug::trace("AtmelDeviceHandler", HobString("AvailableUpgrade") );   
							util_filesystem::File sourceBin(codeUpdate.getPath()->getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME));
//						    bGotCodeUpdate = true;
//						    bWaitTimeout = false;

                            if (sourceBin.exists() == true)
                            {
                                FirmwareFilename.assign(sourceBin.getFullPath().c_str());
//							    needsUpdate = true;
                                bGotCodeUpdate = true;
                                bWaitTimeout = false;
								Debug::trace("AtmelDeviceHandler", HobString("source.bin was found for the path:") + HobString(codeUpdate.getPath()->getFullPath()));
                            }
							else
							{
							    needsUpdate = false;
								Debug::trace("AtmelDeviceHandler.error", HobString("Could not find source.bin for the path:") + HobString(codeUpdate.getPath()->getFullPath()));
							}
						    break;
                        }
                        case SoftwareSystem::NotAvailable:
					    {
//						    bGotCodeUpdate = true;
//						    bWaitTimeout = false;
//						    needsUpdate = false;
                            Debug::trace("AtmelDeviceHandler", HobString("NotAvailable") );
                            break;
					    }

                        case SoftwareSystem::retryCntrOver:
                            // TODO: abort and prevent further update attempts
                            Debug::trace("AtmelDeviceHandler", HobString("retryCntrOver") );
                            break;

                        default:
                            Debug::trace("AtmelDeviceHandler.error", HobString("unknown 0x")
                                    + Hex::toString((long)codeUpdate.getResponseType()) );
                            break;
                    }
                }
                break;
			}
            case UpgradeAutomatic:
                Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage:  Got UpgradeAutomatic event from Version Manager"));
                break;
            case UpgradeManual:
                Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage:  Got UpgradeManual event from Version Manager"));
                break;
            case CancelUpgrade:
                Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage:  Got CancelUpgrade event from Version Manager"));
                break;
            case BeginUpgrade:
                Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage:  Got BeginUpgrade event from Version Manager"));
                break;
			case ResetFPCComponent:
                if( !isPrepack )
				    handleResetFPCComponent();
				break;
            default:
                Debug::trace("AtmelDeviceHandler.error",
                        HobString("HandlePublicMessage: got unknown event 0x")
                        + Hex::toString((long)ev)
                        + HobString(" from Version Manager") );
                break;
        }
        rc = true;
    }
    else
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("HandlePublicMessage: got unknown message type 0x")
                + Hex::toString((long)devType));
    }
    return rc;
}


void AtmelDeviceHandler::handleResetFPCComponent()
{
#if defined ( __linux__ )

	// From Aaron: I have mapped the FPC device reset i/o to:  /usr/local/gpio/hub1_enable/value 
	// You can use a system call to access this pin and you want to set the pin low with the following:
	// system(/bin/echo 0 >> /usr/local/gpio/hub1_enable/value);
	// sleep(100)          #min of 100mS
	// system(/bin/echo 1 >> /usr/local/gpio/hub1_enable/value);

	string command = "/bin/echo 0 >> /usr/local/gpio/hub1_enable/value";
	Debug::trace("AtmelDeviceHandler", "before setting gpio pin low to put FPC into reset, command: " + command);
	string connectionStatusOutput;
	PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
	// We don't care about any output from the command.

	PAL_absclass::PAL_sleep( 100 );

	command = "/bin/echo 1 >> /usr/local/gpio/hub1_enable/value";
	Debug::trace("AtmelDeviceHandler", "before setting gpio pin high to bring FPC out of reset, command: " + command);
	string connectionStatusOutput;
	PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
	// We don't care about any output from the command.
#else

	//SessionManagerHelper::getInstance()->addDialog(0, 1, "We need a way to reset the FPC for Windows!!");
	SessionManagerHelper::getInstance()->addDialog(0, 1, "You need to reboot the scale to complete the FPC upgrade.");

#endif
}


bool AtmelDeviceHandler::Identify(AT91_DEVICE_HANDLE h, AT91ProductInfo& info)
{
    bool rc = false;
    int uValue = 0;
    int error_code = 0;

// JHC - pulled this from AtmelAT91Updater.cpp since its making the same compare. But I didnt use it.
//       #define DBGU_CIDR_MASK ~0x000F001F
//     - because we had the next line in our own header for this file so I used it but dont know for sure which is right.
// #define DBGU_CIDR_MASK ~0xFFF0FFE0;   // see this in the header
 

    memset(&info, 0, sizeof(AT91ProductInfo));

    try
    {
        pDll->AT91Boot_Read_Int(h, &uValue, DBGU_CIDR_ADDR, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        if ((uValue & DBGU_CIDR_MASK_2) != (SAM7S256_DBGU_CIDR & DBGU_CIDR_MASK_2))
        {
            Debug::trace("AtmelDeviceHandler", HobString("Identify: failed, not a AT91SAM7S256 device") );
            goto _identExit;
        }

        pDll->AT91Boot_Read_Short(h, (short *)&info.VendorId,  SAM7S256_CMD_ADDR_VENDOR_ID,  &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        pDll->AT91Boot_Read_Short(h, (short *)&info.ProductId, SAM7S256_CMD_ADDR_PRODUCT_ID, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        pDll->AT91Boot_Read_Byte(h,  &info.HardwareVerMajor,   SAM7S256_CMD_HWVERSION_MAJOR, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        pDll->AT91Boot_Read_Byte(h,  &info.HardwareVerMinor,   SAM7S256_CMD_HWVERSION_MINOR, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        pDll->AT91Boot_Read_Short(h,  (short *)&info.SoftwareVerMajor,   SAM7S256_CMD_SWVERSION_MAJOR, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        pDll->AT91Boot_Read_Short(h,  (short *)&info.SoftwareVerMinor,   SAM7S256_CMD_SWVERSION_MINOR, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        pDll->AT91Boot_Read_Short(h, (short *)&info.checksum,  SAM7S256_CMD_CHECKSUM,        &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        rc = true;
        goto _identExit;

_errDll:
        Debug::trace("AtmelDeviceHandler.error", HobString("Identify:  Failed, DLL error 0x")
                + Hex::toString((ULONG)error_code));
    }
    catch (_com_error e)
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("Identify: failed, COM error 0x") + Hex::toString(e.Error()) );
    }
    catch (...)
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("Identify: failed on exception") );
    }

_identExit:

#ifdef _DEBUG
    if (rc && Debug::isTracing("AtmelDeviceHandler.Identify"))
    {
        // dump product info
        Debug::trace("AtmelDeviceHandler.Identify", HobString("VendorId  0x") + Hex::toString(info.VendorId        ) );
        Debug::trace("AtmelDeviceHandler.Identify", HobString("ProductId 0x") + Hex::toString(info.ProductId       ) );
        Debug::trace("AtmelDeviceHandler.Identify", HobString("hw major  0x") + Hex::toString(info.HardwareVerMajor) );
        Debug::trace("AtmelDeviceHandler.Identify", HobString("hw minor  0x") + Hex::toString(info.HardwareVerMinor) );
        Debug::trace("AtmelDeviceHandler.Identify", HobString("checksum  0x") + Hex::toString(info.checksum        ) );
    }
#endif
    return rc;
}

// verify file exists and is of correct size
bool AtmelDeviceHandler::VerifyFirmwareFile(std::string& fwfilename)
{
    bool rc = false;

    Debug::trace("AtmelDeviceHandler",
            string("VerifyFirmwareFile: verifying '") + fwfilename + string("'"));

    {
        util_filesystem::Filename fn(fwfilename);
        util_filesystem::File fwfile(fn);
        if (!fwfile.exists())
        {
            Debug::trace("AtmelDeviceHandler.error", HobString("VerifyFirmwareFile: File ") + fwfilename + " does not exist" );
            goto _vfwExit;
        }
        long64 fileLength = fwfile.size();
        if (fileLength == 0 )
        {
            Debug::trace("AtmelDeviceHandler.error", HobString("VerifyFirmwareFile:  File ") + fwfilename + " is zero bytes" );
            goto _vfwExit;
        }
        if (fileLength > SAM7S256_FLASH_SIZE)
        {
			Debug::trace("AtmelDeviceHandler.error", HobString("VerifyFirmwareFile:  File ") + fwfilename + " is too big: 0x" + Hex::toString(fileLength));
            goto _vfwExit;
        }
        /*
           if (fileLength < SAM7S256_FLASH_SIZE )
           {
           Debug::trace("AtmelDeviceHandler", HobString("VerifyFirmwareFile: file is too small") );
           goto _vfwExit;
           }
           */
        rc = true;
    }
_vfwExit:
    return rc;
}


bool AtmelDeviceHandler::DownloadFirmware(AT91_DEVICE_HANDLE h, std::string& fwfilename)
{
    bool rc = false;
    bool bFileValid = false;

    if (NULL == h)
    {
        Debug::trace("AtmelDeviceHandler.error", HobString("DownloadFirmware: device not open") );
        return false;
    }

    // allocate two buffers
    char fwfileBuf[SAM7S256_FLASH_SIZE];
    char flashBuf[SAM7S256_FLASH_SIZE];

    // number of bytes to be flashed
    UINT32 fwLength=0;

    // read device flash into buffer
    if (!ReadDeviceFirmware(h, flashBuf, (UINT32) sizeof(flashBuf)))
    {
        Debug::trace("AtmelDeviceHandler.error", HobString("DownloadFirmware: failed to read device flash") );
        return false;
    }

    {
        // get file length
        util_filesystem::Filename fn(fwfilename);
        util_filesystem::File fwfile(fn);
        long64 fileLength = fwfile.size();
        fwLength = (UINT32) fileLength & 0xffffffff;
        if (!fwfile.exists())
        {
			Debug::trace("AtmelDeviceHandler.error", HobString("DownloadFirmware:  Firmware file does not exist!") );
            goto _dlfwCmpFail;
        }
        if (fileLength > sizeof(fwfileBuf)) // || fileLength == 0
        {
			Debug::trace("AtmelDeviceHandler.error", HobString("DownloadFirmware:  Firmware file is too big") );
            goto _dlfwCmpFail;
        }
        if (fileLength == 0 )
        {
			Debug::trace("AtmelDeviceHandler.error", HobString("DownloadFirmware:  Firmware file is zero bytes") );
            goto _dlfwCmpFail;
        }

        // open file
        try
        {
            ifstream sourceFile(fwfile.getFullPath().c_str(), ios::in|ios::binary);

            if ( !sourceFile )
            {
				Debug::trace("AtmelDeviceHandler.error", HobString("DownloadFirmware1:  Couldn't read firmware file") );
                goto _dlfwCmpFail;
            }

            sourceFile.read(fwfileBuf,fwLength);
            bFileValid = true;

        }
        catch(...)
        {
			Debug::trace("AtmelDeviceHandler.error", HobString("DownloadFirmware2:  Couldn't read firmware file!") );
            goto _dlfwCmpFail;
        }

        if (bFileValid)
        {
            if (0 == memcmp(fwfileBuf,flashBuf,fwLength))
            {
                // device flash matches the firmware file.
                // skip the download and report success.
                Debug::trace("AtmelDeviceHandler", HobString("DownloadFirmware:  Device flash matches firmware file, download skipped") );
                return true;
            }
        }

_dlfwCmpFail:
        if (!bFileValid)
            return false;
    }

    //
    // program device flash from fwfileBuf
    //

    try
    {

        int error_code;

        // Program EFC_FMR reg prior to writing data to flash.
        pDll->AT91Boot_Write_Int(h, EFC_FMR_ADDR, EFC_FMR_VALUE, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        // write the data to flash
        pDll->AT91Boot_SAM7S256_Send_Flash(h, SAM7S256_FIRMWARE_FLASH_ADDRESS, (unsigned char*) fwfileBuf, fwLength, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        // TODO: readback and verify the data

        // success
        rc = true;
        goto _dlfwExit;

_errDll:
        Debug::trace("AtmelDeviceHandler.error", HobString("DownloadFirmware: failed, DLL error 0x")
                + Hex::toString((ULONG)error_code));
    }
    catch (_com_error e)
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("DownloadFirmware: COM error 0x") + Hex::toString(e.Error()) );
    }
    catch (...)
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("DownloadFirmware: failed on exception") );
    }
_dlfwExit:
    return rc;
}

bool AtmelDeviceHandler::LaunchFirmware(AT91_DEVICE_HANDLE h)
{
    bool rc = false;
    int error_code = 0;

    if (h == NULL)
    {
        Debug::trace("AtmelDeviceHandler", HobString("LaunchFirmware: device not open") );
        return false;
    }

    try
    {

        pDll->AT91Boot_Go(h, SAM7S256_FIRMWARE_START_ADDRESS, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        rc = true;

        Debug::trace("AtmelDeviceHandler", HobString("LaunchFirmware: application firmware successfully launched.") );
        goto _launchExit;

_errDll:
        Debug::trace("AtmelDeviceHandler.error", HobString("LaunchFirmware:  Failed, DLL error 0x")
                + Hex::toString((ULONG)error_code));
    }
    catch (_com_error e)
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("LaunchFirmware: failed, COM error 0x") + Hex::toString(e.Error()) );
    }
    catch (...)
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("LaunchFirmware: failed on exception") );
    }

_launchExit:
    return rc;
}

bool AtmelDeviceHandler::RequestCodeUpdate(AT91ProductInfo& info)
{
	bool rc = false;

	Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - begin");
    HobVersion hwver, swver;
    SetVersionInfo(info, hwver, swver);

#ifndef CHECK_FT
bool fakeIt = Configuration::getInstance()->toBoolCfgI("fakeAtmelDevice");
if (fakeIt)
{
	Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate():  fakeIt is true!");
	swver.setVersionType(HobVersion::ApcPrinterSW);
}
#endif

	if ( swver.getVersionType() == HobVersion::ApcPrinterSW )
	{
		Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - software dev is ApcPrinterSW");

		Directory directoryFromWhereFirmwareGetsBurnedToDevice("");
		HobComplexVersion::getExtractionDir(swver.getVersionType(), directoryFromWhereFirmwareGetsBurnedToDevice, 0); // the last parameter isn't used, but has to be supplied

		File firmwareFile(directoryFromWhereFirmwareGetsBurnedToDevice.getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME));
		if (firmwareFile.exists())
		{
			FirmwareFilename = firmwareFile.getFullPath();
			Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - Have firmware to burn to FPC: " + FirmwareFilename);
			bWaitTimeout = false;
			bGotCodeUpdate = true;
//			needsUpdate = true;
		}
		else
		{
			Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - Did not find firmware to burn to FPC: " + firmwareFile.getFullPath());
			bWaitTimeout = true;
			bGotCodeUpdate = false;
//			needsUpdate = false;
		}
		return true;
	}
	else if ( swver.getVersionType() == HobVersion::FpcPrinterSW )
	{
		Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - software dev is FpcPrinterSW");

		Directory directoryFromWhereFirmwareGetsBurnedToDevice("");
		HobComplexVersion::getExtractionDir(swver.getVersionType(), directoryFromWhereFirmwareGetsBurnedToDevice, 0);

		File firmwareFile(directoryFromWhereFirmwareGetsBurnedToDevice.getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME));
		if (firmwareFile.exists())
		{
			FirmwareFilename = firmwareFile.getFullPath();
			Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - Have firmware to burn to FPC: " + FirmwareFilename);
			bWaitTimeout = false;
			bGotCodeUpdate = true;
			needsUpdate = true;
		}
		else
		{
			Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - Did not find firmware to burn to FPC: " + firmwareFile.getFullPath());
			bWaitTimeout = true;
			bGotCodeUpdate = false;
			needsUpdate = false;
		}
		return true;

	}
	else
	{
		Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - software dev is not ApcPrinterSW, sending request to VersionManager : " + inttostring(swver.getVersionType()));
		PeripheralRequestCodeUpdate req(usbNodeId, hwver, swver);
		dispatchEvent(&req);
		rc = true;
	}

	// returning false indicates that we should pause while we wait for the VersionManager to check for a firmware update,
	// that we should discontinue processing until the VersionManager sends a response or timeout occurs.
	return false;
    //return rc;
}

// init version info members
// based on reported device info
void AtmelDeviceHandler::SetVersionInfo(AT91ProductInfo& info, HobVersion& hwver, HobVersion& swver)
{
    hwver.setVersionType(HobVersion::UnknownComponent);
    hwver.setMajorRevision(info.HardwareVerMajor);
    hwver.setMinorRevision(info.HardwareVerMinor);

    swver.setVersionType(HobVersion::UnknownComponent);
    swver.setMajorRevision(info.SoftwareVerMajor);
	swver.setMinorRevision(info.SoftwareVerMinor);

    if (info.VendorId != HOBART_VENDOR_ID)
        return;

    switch (info.ProductId)
    {
    case HOBART_APC_PRINTER_ID:
        hwver.setVersionType(HobVersion::ApcPrinterHW);
        swver.setVersionType(HobVersion::ApcPrinterSW);
        break;
    case HOBART_XPE_WEIGHER_ID:
        hwver.setVersionType(HobVersion::Aws2WeigherHW);
        swver.setVersionType(HobVersion::Aws2WeigherSW);
        hobDevice = HOBART_XPE_WEIGHER_ID;
        break;
    case HOBART_XPE_PRINTER_ID:
        //TBD-Alex Zavala 8/1
		hwver.setVersionType(HobVersion::Epp2PrinterHW);
        swver.setVersionType(HobVersion::Epp2PrinterSW);
        hobDevice = HOBART_XPE_PRINTER_ID;
        break;
    case HOBART_XPE_ST_PRINTER_ID:
        hwver.setVersionType(HobVersion::Epp3PrinterHW);
        swver.setVersionType(HobVersion::Epp3PrinterSW);
        hobDevice = HOBART_XPE_ST_PRINTER_ID;
        break;
    case HOBART_FASTRACK_FPC:
        hwver.setVersionType(HobVersion::FpcPrinterHW);
        swver.setVersionType(HobVersion::FpcPrinterSW);
        break;
    }
}


// Handle timeout for all timers that we create.
// This overrides method declared in CHobartTimer:ITimerHandler.
void AtmelDeviceHandler::timeoutProcess( __int32 timer_id )
{
    if (timer_id == this->WaitTimerId)
    {
        // set flag to indicate
        // VersionManager response timeout
        bWaitTimeout = true;
        ContinueProcessing();
    }
    if (timer_id == this->RemovalTimerId)
    {
        // bump removal timeout count
        nRemovalTimeout++;
        ContinueProcessing();
    }
}

// Format and send a private message.
void AtmelDeviceHandler::SendPrivateMessage(PrivateMsg_t::opEnum_t op, USBD_NODE_ID node, UINT8 bus, UINT8 addr)
{
    PrivateMsg_t msg;
    PrivateMsg_t* pmsg = &msg;
    if (pmsg)
    {
        pmsg->magic = msg_magic_value;
        pmsg->op = op;
        pmsg->usbnode = node;
        send((const char *)pmsg, (uint32_t) sizeof(PrivateMsg_t));
    }
    else
    {
        Debug::trace("AtmelDeviceHandler.error", HobString("SendPrivateMessage:  Failed to create message"));
    }
}

bool AtmelDeviceHandler::ReadDeviceFirmware(AT91_DEVICE_HANDLE h, char * flashBuf, UINT32 nBytes)
{
    bool rc = false;

    int error_code = 0;

    if (h == NULL)
    {
        Debug::trace("AtmelDeviceHandler.error", HobString("ReadDeviceFirmware: device not open") );
        return false;
    }

    memset(flashBuf, 0, nBytes);

    try
    {

        int uAddress = SAM7S256_FIRMWARE_START_ADDRESS;

        pDll->AT91Boot_Read_Data(h, uAddress, (unsigned char *) flashBuf, (int) nBytes, &error_code);
        if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

        rc = true;
        goto _rdfwExit;

_errDll:
        Debug::trace("AtmelDeviceHandler.error", HobString("ReadDeviceFirmware: failed, DLL error 0x")
                + Hex::toString((ULONG)error_code));
    }
    catch (_com_error e)
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("ReadDeviceFirmware: failed, COM error 0x") + Hex::toString(e.Error()) );
    }
    catch (...)
    {
        Debug::trace("AtmelDeviceHandler.error",
                HobString("ReadDeviceFirmware: failed on exception"));
    }

_rdfwExit:
    return rc;
}

void AtmelDeviceHandler::ReportStatusToVersionManager(VersionManagerDevError code, bool removingDialog)
{
    VmStatusMsg status;
    status.setStatusCode(code);
    status.setResponseType(HfcResponse::RESPONSESTYLE_OK);
    status.setRemoveDialog(removingDialog);
    dispatchEvent(&status);
}

AtmelDeviceHandler::AT91_DEVICE_HANDLE AtmelDeviceHandler::OpenDevice(char * devpath)
{
    AT91_DEVICE_HANDLE h = NULL;
    try
    {
        pDll->AT91Boot_Open((unsigned char *)devpath, &h);
    }
    catch(...)
    {
        Debug::trace("AtmelDeviceHandler.error", HobString("OpenDevice: exception on AT91Boot_Open") );
    }
    if (h == NULL)
        Debug::trace("AtmelDeviceHandler.error", HobString("Identify: failed to open device") );
    return h;
}

void AtmelDeviceHandler::CloseDevice(AT91_DEVICE_HANDLE h)
{
    Debug::trace("AtmelDeviceHandler", HobString("CloseDevice: ") );
    try
    {
        pDll->AT91Boot_Close(h);
    }
    catch(...)
    {
        Debug::trace("AtmelDeviceHandler.error", HobString("CloseDevice: exception on AT91Boot_Close") );
    }
}

const char* AtmelDeviceHandler::GetProcStateName(ProcState_t s)
{
    switch (s)
    {
        case S_IDLE:    return "S_IDLE"   ;
        case S_SCAN:    return "S_SCAN"   ;
        case S_IDENT:   return "S_IDENT"  ;
        case S_WAITVER: return "S_WAITVER";
        case S_UPDATE:  return "S_UPDATE" ;
        case S_DETACH:  return "S_DETACH" ;
    }
    return "Unknown";
}

void AtmelDeviceHandler::detachDevice(USBD_NODE_ID nodeId)
{
//    Debug::trace("AtmelDeviceHandler", "detachDevice(" + Hex::toString(nodeId) + "): Entry");
//    UsbManager::getInstance()->DetachDevice(nodeId);
//    processingDevice = false;
//    usbNodeId = 0;
//    Debug::trace("AtmelDeviceHandler", "detachDevice(): Exit");
}

/*

Requirements

3.1. Normal Operation

3.1.1. No Code Update Needed

3.1.1.1. SAM Boot Code Execution
Boot Code enumerates with Atmel Vendor and Product ID.
The System Controller issues a �Version� command. The Boot Code responds with the current SAM-BA version.
In the unlikely event that there is a Boot Code version change, the System Controller can adjust its operation to accommodate the change.
System Controller issues �Jump to Application� command.
System Controller sets a timer event for 3-4 seconds. The peripheral controller should enumerate prior to this timer expiring.

 3.1.1.2. Peripheral Code Execution
The Peripheral Controller Code will assume full control of the processor, with the exception of the first two sectors of Flash ROM where the SAM-BA program resides and the first 0x100 of the third sector (where the �USB Product ID� is stored). It will assume control of all RAM and all peripherals, as if the SAM-BA code had never executed. The list of tasks the Peripheral Controller Code will perform (in rough order) is listed below:
Disable all interrupts (just in case the Boot Code had enabled them).
Detach from USB (it is not prepared to respond to any USB requests).
The System Controller must  not talk to Peripheral Controller via USB after issuing the �Jump to Application� command. If it does this before the application has had the chance to disable interrupts and �detach� from USB, a SAM-BA ISR may execute, leading to unknown results.
Initialize variables (part of run-time libraries � happens before �main()�).
Initialize all peripheral interrupt service routines by writing the ISR vectors for all thirty two AIC interrupt sources in the corresponding Source Mode Register (see Table 24-2).
Initialize the ARM Core Interrupt Vectors by remapping RAM to addresses 0x0 to 0x20 and writing the associated ISR vectors in those locations (see section 19.3.3 �Remap Command�).
Calculate a checksum on the Peripheral Controller Code and verify this checksum against the stored checksum.
Read the serial EEP and verify its stored checksum matches its calculated checksum.
Complete all other hardware and software initialization.
Enable interrupts.
�Attach� to USB to initiate enumeration.
Respond to System Controller�s enumeration requests. Enumerate as �LXScale Peripheral Controller� by using the �USB Product ID� for the LXScale Peripheral Controller.
After enumeration, send a �System Status� message (requested by the System Controller). This message contains the Peripheral Controller Software Revision, and the Stored and Calculated Checksums for the code.

3.1.1.3. System Controller Action
The System Controller Performs the following checks to verify the peripheral code doesn�t need to be updated.
Verifies the peripheral�s calculated checksum matches its stored checksum.
Verifies the code  is the correct version.
Verifies the code is appropriate for the hardware revision.
If any of the above conditions aren�t meant, the execution proceeds as described in the �Code Updated Needed�
If all of the above conditions are met, execution continues as normal (weigher is calibrated, printer teach tables downloaded, etc).

3.1.2. Code Update Needed
The System Controller resets the Peripheral Controller by turning off USB5V power, waits 10mS, and then turns it back on.
The System Controller can also reset the Peripheral Controller by sending a �Reset Peripheral� message over USB, IF the peripheral code is responding. If it�s not, USB5V power must be cycled.
The System Controller sets an �Attachment� timer. The Peripheral Controller must �attach� to USB prior to this timer expiring.
The System Controller waits till the peripheral controller �attaches� then proceeds with enumeration.
The Boot Code enumerates with the Atmel Vendor and Product ID.
The System Controller reads the �USB Product ID� from the first page of the third Flash sector. It reads the board revision from it�s own serial EEP.  It uses this information to select the code to download.
The System Controller then downloads the code with the �Send a File� command.
After the code has been downloaded the System Controller issues �Jump to Application� command and sets a timer event for 3-4 seconds. The peripheral controller should enumerate prior to this timer expiring..
Execution continues as described in the �Peripheral Code Execution� section.


3.2. Error Handling
3.2.1. No Response from Peripheral out of reset
Setup (the first time) or increment (subsequent times) a counter that counts the number of consecutive times this error occurs.
If the peripheral doesn�t enumerate as a Boot Loader within a few seconds, the System Controller should reset the peripheral controller
(by turning off USB5V power) and wait for the peripheral to enumerate.
If this error has occurred more than three times, alert the supervisor of the failure and stop resetting the peripheral controller.

11/30/06 A simple hardware change could be made to allow the System Controller to restore the �Boot Code� from ROM to Flash in the SAM7S.
This option will be considered before the next board revision.

3.2.2. Peripheral Code Does Not Enumerate after the �Go� command.
Setup (the first time) or increment (subsequent times) a counter that counts the number of consecutive times this error occurs.
Assume there is a peripheral code software problem. Reset the system controller by turning USB5V power off for 10mS.
Then  proceed from   the �Code Update Needed� section of this document.
If this error has occurred more than three times, alert the supervisor of the failure and stop resetting the
peripheral controller. Alternately, try loading the previous version of Peripheral Code.

3.2.3. Peripheral Controller Stored Checksum doesn�t match calculated checksum.
Setup (the first time) or increment (subsequent times) a counter that counts the number of consecutive times this error occurs.
Try downloading the code again. Go to the �Code Update Needed� section of this document.
If this error has occurred more than three times, alert the supervisor of the failure and stop resetting the peripheral controller.

3.2.4. �USB Product ID� or Hardware Version lost or corrupt
In LXScale, download the current version of printer code and set �alert� that will notify the next person to enter Service Mode.
Create a Service Utility that allows the service person to manually enter the HW revision and the USB Product ID (from a menu of legitimate choices).


*/
