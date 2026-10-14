#include "EvolMessage.h"
#include "UsbManager.h"
#include "UsbManagerLinux.h"
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
#include "PAL_platformheader.h"

#include "UsbNode.h"

#include "Configuration.h"
#include "RealTimeController.h"

#include "SessionManagerHelper.h"
//SOF-TIME-71
#include "FpmManager.h"


extern "C"
{
	#include <string.h>  /* String function definitions */
	#include <libudev.h>
}

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
	int AtmelDeviceHandler::dataType = 0;
	std::string AtmelDeviceHandler::dataTypeName("AtmelDeviceHandler");
#endif

using namespace std;


AtmelDeviceHandler * AtmelDeviceHandler::instance = 0;
util_thread::Mutex<> AtmelDeviceHandler::instanceMutex_;

const unsigned char AtmelDeviceHandler::loaderImage[SAM7S256_LOADER_IMAGE_SIZE] =
	{
		0x0c, 0xd0, 0x9f, 0xe5, 0x00, 0x40, 0x2d, 0xe9, 0x2c, 0x00, 0x00, 0xeb, 0x01, 0x00, 0xfd, 0xe8, 0x10, 0xff, 0x2f, 0xe1,
		0x00, 0x1c, 0x20, 0x00, 0x90, 0x80, 0x8f, 0xe2, 0x0f, 0x00, 0x98, 0xe8, 0x08, 0x00, 0x80, 0xe0, 0x08, 0x10, 0x81, 0xe0,
		0x08, 0x20, 0x82, 0xe0, 0x08, 0x30, 0x83, 0xe0, 0x01, 0xb0, 0x40, 0xe2, 0x01, 0xc0, 0x42, 0xe2, 0x01, 0x00, 0x50, 0xe1,
		0x0e, 0x00, 0x00, 0x0a, 0x70, 0x00, 0xb0, 0xe8, 0x05, 0x00, 0x54, 0xe1, 0xfa, 0xff, 0xff, 0x0a, 0x01, 0x00, 0x14, 0xe3,
		0x0b, 0x40, 0x84, 0x10, 0x01, 0x00, 0x15, 0xe3, 0x0b, 0x50, 0x85, 0x10, 0x02, 0x00, 0x15, 0xe3, 0x09, 0x50, 0x85, 0x10,
		0x03, 0x50, 0xc5, 0xe3, 0x04, 0x60, 0x56, 0xe2, 0x04, 0x70, 0x94, 0x24, 0x04, 0x70, 0x85, 0x24, 0xfb, 0xff, 0xff, 0x8a,
		0xee, 0xff, 0xff, 0xea, 0x03, 0x00, 0x52, 0xe1, 0x30, 0x00, 0x00, 0x0b, 0x00, 0x70, 0xa0, 0xe3, 0x30, 0x00, 0xb2, 0xe8,
		0x01, 0x00, 0x14, 0xe3, 0x0c, 0x40, 0x84, 0x10, 0x02, 0x00, 0x14, 0xe3, 0x09, 0x40, 0x84, 0x10, 0x03, 0x40, 0xc4, 0xe3,
		0x04, 0x50, 0x55, 0xe2, 0x04, 0x70, 0x84, 0x24, 0xfc, 0xff, 0xff, 0x8a, 0xf2, 0xff, 0xff, 0xea, 0x60, 0x03, 0x00, 0x00,
		0x84, 0x03, 0x00, 0x00, 0x84, 0x03, 0x00, 0x00, 0x9c, 0x03, 0x00, 0x00, 0x10, 0x40, 0x2d, 0xe9, 0x6c, 0x40, 0x9f, 0xe5,
		0x40, 0x09, 0xa0, 0xe3, 0x00, 0x10, 0x94, 0xe5, 0x00, 0xc0, 0xa0, 0xe3, 0x00, 0x31, 0x91, 0xe5, 0x04, 0xe0, 0x11, 0xe5,
		0x03, 0x14, 0xa0, 0xe1, 0x21, 0x11, 0xa0, 0xe1, 0x01, 0x11, 0x80, 0xe0, 0x98, 0x00, 0x1c, 0xe5, 0x01, 0x00, 0x10, 0xe3,
		0xfc, 0xff, 0xff, 0x0a, 0x00, 0x00, 0xa0, 0xe3, 0x00, 0x20, 0x94, 0xe5, 0x00, 0x21, 0x92, 0xe7, 0x00, 0x21, 0x81, 0xe7,
		0x01, 0x00, 0x80, 0xe2, 0x40, 0x00, 0x50, 0xe3, 0xf9, 0xff, 0xff, 0x3a, 0x24, 0x00, 0x9f, 0xe5, 0x03, 0x04, 0x00, 0xe0,
		0x01, 0x00, 0x80, 0xe3, 0x5a, 0x04, 0x80, 0xe3, 0x9c, 0x00, 0x0c, 0xe5, 0x00, 0x00, 0x5e, 0xe3, 0x06, 0x00, 0xa0, 0x13,
		0xe4, 0x0d, 0x0c, 0x15, 0x01, 0x00, 0xa0, 0xe3, 0x10, 0x80, 0xbd, 0xe8, 0x4c, 0x1a, 0x20, 0x00, 0x00, 0xff, 0x03, 0x00,
		0x0e, 0xf0, 0xa0, 0xe1, 0x0e, 0xf0, 0xa0, 0xe1, 0x4d, 0x00, 0x00, 0xeb, 0x0d, 0x00, 0x00, 0xeb, 0xda, 0xff, 0xff, 0xeb,
		0x05, 0x00, 0x00, 0xea, 0x01, 0x40, 0x2d, 0xe9, 0x42, 0x00, 0x00, 0xeb, 0x01, 0x40, 0xbd, 0xe8, 0x00, 0x00, 0x00, 0xea,
		0x00, 0x00, 0xe0, 0xe3, 0x83, 0x00, 0x00, 0xea, 0x10, 0x40, 0x2d, 0xe9, 0x00, 0x40, 0xa0, 0xe1, 0xff, 0xff, 0xff, 0xeb,
		0x04, 0x00, 0xa0, 0xe1, 0x10, 0x40, 0xbd, 0xe8, 0xf3, 0xff, 0xff, 0xea, 0xf0, 0x40, 0x2d, 0xe9, 0x0c, 0xd0, 0x4d, 0xe2,
		0x01, 0x50, 0xa0, 0xe1, 0x00, 0x40, 0xa0, 0xe1, 0x97, 0x00, 0x00, 0xeb, 0x00, 0x00, 0xa0, 0xe3, 0x00, 0x40, 0x8d, 0xe5,
		0x04, 0x50, 0x8d, 0xe5, 0xff, 0xff, 0xff, 0xeb, 0x08, 0x00, 0x8d, 0xe5, 0xc8, 0x00, 0x9f, 0xe5, 0x0d, 0x10, 0xa0, 0xe1,
		0x00, 0x00, 0x8f, 0xe0, 0xff, 0xff, 0xff, 0xeb, 0x00, 0x50, 0xa0, 0xe1, 0x01, 0x60, 0xa0, 0xe1, 0x03, 0x00, 0x9d, 0xe8,
		0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb, 0x00, 0x01, 0xa0, 0xe3, 0x00, 0x00, 0xa0, 0xe3,
		0xff, 0xff, 0xff, 0xeb, 0x00, 0x70, 0xa0, 0xe1, 0x6a, 0x00, 0x00, 0xeb, 0x00, 0x40, 0xa0, 0xe1, 0x20, 0x70, 0x80, 0xe5,
		0x00, 0x00, 0xa0, 0xe3, 0x00, 0x10, 0xa0, 0xe3, 0xff, 0xff, 0xff, 0xeb, 0x01, 0x10, 0x80, 0xe2, 0x24, 0x10, 0x84, 0xe5,
		0x00, 0x10, 0xa0, 0xe3, 0x00, 0x00, 0xa0, 0xe3, 0xff, 0xff, 0xff, 0xeb, 0x28, 0x00, 0x84, 0xe5, 0x00, 0x00, 0xa0, 0xe3,
		0x00, 0x01, 0xa0, 0xe3, 0xff, 0xff, 0xff, 0xeb, 0x2c, 0x00, 0x84, 0xe5, 0x00, 0x00, 0xa0, 0xe3, 0x00, 0x10, 0xa0, 0xe3,
		0xff, 0xff, 0xff, 0xeb, 0x30, 0x00, 0x84, 0xe5, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb,
		0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb,
		0x05, 0x00, 0xa0, 0xe1, 0x06, 0x10, 0xa0, 0xe1, 0x0c, 0xd0, 0x8d, 0xe2, 0xf0, 0x00, 0xbd, 0xe8, 0x04, 0xf0, 0x9d, 0xe4,
		0x08, 0x40, 0x2d, 0xe9, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb, 0xff, 0xff, 0xff, 0xeb, 0x08, 0x80, 0xbd, 0xe8,
		0xc0, 0x00, 0x00, 0x00, 0x0e, 0x50, 0xa0, 0xe1, 0x42, 0x00, 0x00, 0xeb, 0x05, 0xe0, 0xa0, 0xe1, 0x00, 0x40, 0xa0, 0xe1,
		0x0d, 0x10, 0xa0, 0xe1, 0x0a, 0x30, 0xa0, 0xe1, 0x07, 0x00, 0xc0, 0xe3, 0x60, 0xd0, 0x80, 0xe2, 0x10, 0x40, 0x2d, 0xe9,
		0x3d, 0x00, 0x00, 0xeb, 0x10, 0x40, 0xbd, 0xe8, 0x07, 0xd0, 0xc1, 0xe3, 0x00, 0x60, 0xa0, 0xe3, 0x00, 0x70, 0xa0, 0xe3,
		0x00, 0x80, 0xa0, 0xe3, 0x00, 0xb0, 0xa0, 0xe3, 0x04, 0xc0, 0xa0, 0xe1, 0xc0, 0x09, 0xac, 0xe8, 0xc0, 0x09, 0xac, 0xe8,
		0xc0, 0x09, 0xac, 0xe8, 0xc0, 0x09, 0xac, 0xe8, 0x13, 0x40, 0x2d, 0xe9, 0x00, 0x00, 0xa0, 0xe3, 0x00, 0x10, 0xa0, 0xe3,
		0xff, 0xff, 0xff, 0xeb, 0x40, 0x10, 0x81, 0xe2, 0x01, 0x20, 0x80, 0xe0, 0x44, 0x2f, 0x82, 0xe2, 0x1c, 0x20, 0x84, 0xe5,
		0x18, 0x10, 0x84, 0xe5, 0x01, 0x00, 0xa0, 0xe3, 0x10, 0x00, 0x84, 0xe5, 0x13, 0x40, 0xbd, 0xe8, 0x14, 0x00, 0x84, 0xe5,
		0x00, 0x10, 0xa0, 0xe1, 0x0e, 0xf0, 0xa0, 0xe1, 0x10, 0x40, 0x2d, 0xe9, 0x03, 0x00, 0x2d, 0xe9, 0x1d, 0x00, 0x00, 0xeb,
		0x00, 0x40, 0xa0, 0xe1, 0x03, 0x00, 0xbd, 0xe8, 0x1c, 0x30, 0x94, 0xe5, 0x03, 0x30, 0x4d, 0xe0, 0x14, 0x20, 0x94, 0xe5,
		0x00, 0x20, 0x81, 0xe5, 0x00, 0xe0, 0x82, 0xe0, 0x03, 0x00, 0x5e, 0xe1, 0x0a, 0x00, 0x00, 0x8a, 0x0e, 0x30, 0x93, 0xe0,
		0x63, 0x30, 0xa0, 0xe1, 0x07, 0x30, 0xc3, 0xe3, 0x40, 0x1d, 0x8e, 0xe2, 0x07, 0x10, 0x81, 0xe2, 0x07, 0x10, 0xc1, 0xe3,
		0x03, 0x00, 0x51, 0xe1, 0x03, 0x10, 0xa0, 0x81, 0x02, 0x00, 0x41, 0xe0, 0x14, 0x10, 0x84, 0xe5, 0x10, 0x80, 0xbd, 0xe8,
		0x00, 0x20, 0xa0, 0xe1, 0x00, 0x00, 0xa0, 0xe3, 0xff, 0xff, 0xff, 0xeb, 0x10, 0x80, 0xbd, 0xe8, 0x18, 0x00, 0xa0, 0xe3,
		0x08, 0x10, 0x9f, 0xe5, 0x56, 0x34, 0x12, 0xef, 0x0e, 0xf0, 0xa0, 0xe1, 0x58, 0x00, 0x00, 0x00, 0x26, 0x00, 0x02, 0x00,
		0x00, 0x00, 0x9f, 0xe5, 0x0e, 0xf0, 0xa0, 0xe1, 0x50, 0x1a, 0x20, 0x00, 0x00, 0x40, 0x2d, 0xe9, 0x14, 0xd0, 0x4d, 0xe2,
		0x0d, 0x10, 0xa0, 0xe1, 0x04, 0x20, 0x8d, 0xe2, 0x00, 0x20, 0x81, 0xe5, 0x16, 0x00, 0xa0, 0xe3, 0x56, 0x34, 0x12, 0xef,
		0x04, 0x00, 0x9d, 0xe5, 0x0c, 0x10, 0x9d, 0xe5, 0x08, 0x20, 0x9d, 0xe5, 0x10, 0x30, 0x9d, 0xe5, 0x00, 0x00, 0x50, 0xe3,
		0x08, 0x00, 0x9f, 0x05, 0x14, 0xd0, 0x8d, 0xe2, 0x00, 0x80, 0xbd, 0xe8, 0x08, 0x00, 0x00, 0x00, 0xb0, 0x1a, 0x20, 0x00,
		0x0e, 0xf0, 0xa0, 0xe1, 0x10, 0x40, 0x2d, 0xe9, 0xe8, 0xff, 0xff, 0xeb, 0x04, 0x00, 0x80, 0xe2, 0x10, 0x80, 0xbd, 0xe8,
		0x10, 0x40, 0x2d, 0xe9, 0xf9, 0xff, 0xff, 0xeb, 0x00, 0x10, 0xa0, 0xe3, 0x00, 0x10, 0x80, 0xe5, 0x10, 0x80, 0xbd, 0xe8,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x1a, 0x20, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x50, 0x1a, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x1a, 0x20, 0x00, 0x60, 0x00, 0x00, 0x00,
		0x00, 0x14, 0x20, 0x00
	};


// singleton. returns instance pointer
AtmelDeviceHandler *AtmelDeviceHandler::getInstance()
{
	if (instance == 0)
	{
		instanceMutex_.acquire();
		instance = new AtmelDeviceHandler();
		instanceMutex_.release();
	}
	return instance;
}


AtmelDeviceHandler::AtmelDeviceHandler() : Hobart::CEvolMsgReceiver("AtmelDeviceHandler")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);
	util_acc::addInstance(dataType);
#endif

	Debug::trace("AtmelDeviceHandler", HobString("Constructor"));

	bNodesRemoved = false;
    isPrepack = Configuration::getInstance()->isPrepackScale();
    processingDevice = false;
    usbNodeId = 0;
	procState = S_IDLE;
	hOpenDevice = 0;
	needsUpdate = false;
    identifyFailureCount = 0;

#ifdef UDEV_PATCH
    patchComplete = false;
    hobDevice = 0;
#endif
	//SOF-TIME-71
	Debug::trace("AtmelDeviceHandler", "prCounter: " + Integer::toString( prCounter ) );  
	prCounter = 0;
	chipRev = NoRevision;

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

	Debug::trace("AtmelDeviceHandler", HobString("Destructor"));

	deregisterAllEvents();

	ShutdownNicely();


	cancelTimer(WaitTimerId);
	cancelTimer(RemovalTimerId);
#ifdef UDEV_PATCH
    cancelTimer(PatchTimerId);
#endif

#if 0
	// TODO: how to invoke Couninitialize on worker thread context from ~AtmelDeviceHandler?
	// COM uninitialization must occur on the worker thread context,
	// i.e. from within the processEvent method (MessageCheckingThread.
	SendPrivateMessage(PrivateMsg_t::Deinit);
#endif
}

void AtmelDeviceHandler::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	Debug::trace("AtmelDeviceHandler", HobString("processEvent: "));

	bool bRunFSM = true;

	if ( msg.get() != NULL )
	{
		std::lock_guard<std::recursive_mutex> g( mutex );

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
			Debug::trace("AtmelDeviceHandler",
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
	char path[MAX_PATH];

	switch (procState)
	{

		case S_IDLE:
			Debug::trace("AtmelDeviceHandler.run", HobString("procState = S_IDLE"));
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
			Debug::trace("AtmelDeviceHandler.run", HobString("procState = S_SCAN"));
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
				//	  AT91 device detaches from bus (following application firmware launch)
				//	  PnP manager removes SAM-BA driver (atm6124.sys) instance
				//	  PnP manager sends device removal notification for the node
				//	  UsbManager receives device removal notification for the node
				//	  UsbManager destroys the AtmelSambaUpdater instance associated with the node
				//	  AtmelSambaUpdater destructor calls AtmelDeviceHandler::DeviceRemoval
				//	  AtmelDeviceHandler::DeviceRemoval removes the node from usbNodeList
				//
				//
				Debug::trace("AtmelDeviceHandler.run", HobString("S_SCAN:  No devices found"));
                Debug::trace("AtmelDeviceHandler.run", HobString("S_SCAN:  Clearing processDevice+usbNodeId"));
                processingDevice = false;
                usbNodeId = 0;
				nextState = S_IDLE;
			}
			break;
		case S_PROCESS:
		{			
			if( !deviceList.empty() ) {
				Debug::trace("AtmelDeviceHandler", "S_PROCESS: processing " + Integer::toString(deviceList.size()) + " devices" );
				handleDevicesOnBus();	
			} else {
				Debug::trace("AtmelDeviceHandler", "S_PROCESS: device list is empty!");
			}
			nextState = procState = S_IDLE;
			break;
		} 
		case S_IDENT:
		{
			Debug::trace("AtmelDeviceHandler.run", HobString("procState = S_IDENT"));

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
                processingDevice = false;
                usbNodeId = 0;
				m_cs.Leave();
				nextState = S_SCAN;
				break;
			}
			if (0 == Devs.size())
			{
				// There are no more devices in the previously scanned list.
				// Wait for signal for the next device.
				Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  No more device paths in list, switch to Idle state."));
                processingDevice = false;
                usbNodeId = 0;
				nextState = S_IDLE;
				break;
			}

			Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  Open device"));
            processingDevice = true;
			memset(path, 0, MAX_PATH);
			strncpy(path, Devs.back().c_str(), MAX_PATH);
			Devs.pop_back();
			//added -- ats
			usbNodeListIter i = usbNodeList.begin();
			if(i != usbNodeList.end())
			{
				hOpenDevice = OpenDevice(*i);
                usbNodeId = *i;
                Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  usbNodeId:" + Hex::toString(usbNodeId)));
			}

			// identify the device
			//hOpenDevice = OpenDevice(path);
			if (hOpenDevice == 0)
			{
				Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  Failed to open device, skipping to next device."));
                detachDevice(usbNodeId);
                //if failed to open remove from the list!!
    			if(i != usbNodeList.end())
    				usbNodeList.erase(i);
                usbNodeId = 0;

                Devs.clear();

                //Clearing the node info
				processingDevice = false;

                if(usbNodeList.empty()) {
                 	nextState =  S_IDLE;
                }
                bContinue = false;
				break;
			}
			if (Identify(hOpenDevice, ProductInfo) == false)
			{
				// The device failed identification.
				// Continue processing in the IDENT state. The next device
				// path found in the list will be identified.
                if(identifyFailureCount < 5)
                {
                    ++identifyFailureCount;
				    Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  Identify failed!!! Rescanning Try:") + HobString(Integer::toString(identifyFailureCount)));
                    
                    //Clearing the node info
                    processingDevice = false;
                    usbNodeId = 0;

                    //A node may have been removed from the list, rescan and reset devs
                    Devs.clear();

                    //Release the device and try again
                    CloseDevice(hOpenDevice);
					hOpenDevice = NULL;

                    nextState = S_SCAN;
                }
                else
                {
                    Debug::trace("AtmelDeviceHandler.run", "S_IDENT: Identify failed more than 5 times, detaching device and moving on!");
                    detachDevice(usbNodeId);
                    identifyFailureCount = 0;
                }
				break;
			}
            else
            {
                Debug::trace("AtmelDeviceHandler.run", "S_IDENT: Resetting failure count to 0!");
                identifyFailureCount = 0;//Reset this count
            }

			Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  Device identified."));

			// The device identified successfully. Send a message to the
			// VersionManager requesting a firmware update.

#if 1 /* remove version manager from upgrade process --ats */
			Debug::trace("AtmelDeviceHandler.run", HobString("S_IDENT:  Requesting code update from VersionManager"));

			// Reset flags
			bWaitTimeout = bGotCodeUpdate = false;

			/// SOF-1750: timer no longer needed since version manager is no longer used for upgrades!
			// createTimer(WaitTimerDuration, WaitTimerId, false); // Enable the one-shot timeout counter
			///

			// Format and send the request msg to the VersionManager.
			// This return value from RequestCodeUpdate indicates whether we should continue or pause.
			bContinue = RequestCodeUpdate(ProductInfo);
#else
			/* automatically launch the firmware */
			bWaitTimeout = true;
#endif
			nextState = S_WAITVER;
			break;
		}
		case S_WAITVER:
			Debug::trace("AtmelDeviceHandler.run", HobString("procState = S_WAITVER"));
			if (bWaitTimeout)
			{
				Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER:  Timeout waiting for VersionManager response"));
				// Timeout occurred while waiting on the VersionManager response
				// to our request for a firmware update for the current device.
				// Try to launch the device's firmware.
				if (!LaunchFirmware(hOpenDevice))
				{
					Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER:  Firmware launch failed"));
					if (bGotCodeUpdate)
					{
                        if( isPrepack )
                        {
                            ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
						    ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
                        }
                        else
                        {
						    ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
						    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, true);
                            
                            Debug::trace("AtmelDeviceHandler.error", HobString("SENT VersionManagerUsbDeviceDownloadFailed-1"));
                            
						    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
                        }
					}
                    detachDevice(usbNodeId);
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

					CloseDevice(hOpenDevice);
					hOpenDevice = NULL;

					nextState = S_DETACH;
				}
			}
			else if (bGotCodeUpdate)
			{
				/// SOF-1750: timer no longer needed since version manager is no longer used for upgrades!
				//cancelTimer(WaitTimerId);
				///

				Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER:  Got update from VersionManager"));
				if(needsUpdate)
				{
					if (VerifyFirmwareFile(FirmwareFilename))
					{
                        if( isPrepack )
                        {
                            ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
                        }
                        else
                        {
						    ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
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
						    ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
						    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, true);
                            
                            Debug::trace("AtmelDeviceHandler.error", HobString("SENT VersionManagerUsbDeviceDownloadFailed-2"));
                            
						    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
                        }
						Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER:  Invalid file"));
						LaunchFirmware(hOpenDevice);

						createTimer(RemovalTimerDuration, RemovalTimerId, true);
						nRemovalTimeout = 0;

						CloseDevice(hOpenDevice);
						hOpenDevice = NULL;

                        //detachDevice(usbNodeId);
                        UsbManager::getInstance()->DetachDevice(usbNodeId);
                        Debug::trace("AtmelDeviceHandler.run", "Setting Processed Node id: " + Hex::toString(usbNodeId));
                        UsbManager::getInstance()->setJustProcessed(usbNodeId);
						nextState = S_DETACH;
					}
				}
				else
				{
                    Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER: needsUpdate is false, launching firmware"));
					LaunchFirmware(hOpenDevice);

					createTimer(RemovalTimerDuration, RemovalTimerId, true);
					nRemovalTimeout = 0;

					CloseDevice(hOpenDevice);
					hOpenDevice = NULL;

                    UsbManager::getInstance()->DetachDevice(usbNodeId);
					nextState = S_DETACH;
				}
			}
			else
			{
				Debug::trace("AtmelDeviceHandler.run", HobString("S_WAITVER:  Did not get update, but was not timeout either."));

                //Don't launch just yet, wait for the version manager to respond OR the timer to go off
				bContinue = false;
			}
			break;

		case S_UPDATE:
		{
			//make sure the fpc probe time is not running if we are upgrading.
			RealTimeController::getInstance()->stopFpcProbeTimer();

			Debug::trace("AtmelDeviceHandler.run", HobString("procState = S_UPDATE:  Downloading firmware"));
			if (!DownloadFirmware(hOpenDevice, FirmwareFilename))
			{
				Debug::trace("AtmelDeviceHandler.run", HobString("S_UPDATE:  Failed to download firmware"));
				if (bGotCodeUpdate)
				{
                    if( isPrepack )
                    {
                        ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
						ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
                    }
                    else
                    {
					    Debug::trace("AtmelDeviceHandler.error", HobString("SENT VersionManagerUsbDeviceDownloadFailed-3"));
					
					    ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
					    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, true);
					    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
                    }
				}
                detachDevice(usbNodeId);
				nextState = S_IDENT;
			}
			else
			{
				Debug::trace("AtmelDeviceHandler.run", HobString("S_UPDATE:  Firmware download OK. Launching firmware."));

				//Sleep is needed to allow device finish final page burn before telling it to launch the app
				PAL_absclass::PAL_sleep( 300 );

				if (!LaunchFirmware(hOpenDevice))
				{
					Debug::trace("AtmelDeviceHandler.run", HobString("S_UPDATE:  Firmware launch failed"));
					if (bGotCodeUpdate)
					{
                        if( isPrepack )
                        {
                            ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
						    ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
                        }
                        else
                        {
						    ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
						    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, true);
							
							Debug::trace("AtmelDeviceHandler.error", HobString("SENT VersionManagerUsbDeviceDownloadFailed-4"));
							
						    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
                        }
					}
                    detachDevice(usbNodeId);
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
						    ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
						    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, true);
						    ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadSucceeded);
                        }
					}

					// wait for the device removal
					// enable the removal timeout counter
					// and clear the timeout count
					createTimer(RemovalTimerDuration, RemovalTimerId, true);
					nRemovalTimeout = 0;
                    //detachDevice(usbNodeId);
                    UsbManager::getInstance()->DetachDevice(usbNodeId);

					nextState = S_DETACH;
				}
			}
			// ORIGINAL CODE
			// 1: Commenting out this block made both printers upgrade
			// 2: Leaving this block in just updated a single printer
			// 
			//Debug::trace("AtmelDeviceHandler", "remove file: " + FirmwareFilename);
			//File firmwareFile(FirmwareFilename);
			//if (firmwareFile.exists()) {
			//	Debug::trace("AtmelDeviceHandler", "remove file: " + FirmwareFilename);
			//	firmwareFile.remove();
			//}

			// NEW CODE
			// SOF-TIME-71 for upgrade of 2 printer (Epp2PrinterSW) system of the same type!	isWrapperTwoPrinter()		 
			if( FpmManager::getInstance()->isWrapperTwoPrinter() ) {
				Debug::trace("AtmelDeviceHandler", "prCounter: " + Integer::toString( prCounter ) ); 
				if( prCounter == 2 ) {
					File firmwareFile(FirmwareFilename);
					if (firmwareFile.exists()) {
						Debug::trace("AtmelDeviceHandler", "remove file: "+ FirmwareFilename );
						firmwareFile.remove();
					}
					prCounter = 0;
				}
			} else {
				Debug::trace("AtmelDeviceHandler", "remove file: "+ FirmwareFilename );
				File firmwareFile(FirmwareFilename);
				if (firmwareFile.exists()) {
					Debug::trace("AtmelDeviceHandler", "remove file: "+ FirmwareFilename );
					firmwareFile.remove();
				}
			}

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
#ifdef UDEV_PATCH
                //Create a one time check to make sure we receive an attach from udev
                Debug::trace("AtmelDeviceHandler.run", "S_DETACH: Creating udev patch timer0");
                createTimer(UdevPatchTimeoutLimit, PatchTimerId, false);
#endif
				nRemovalTimeout = 0;
				m_cs.Enter();
				Debug::trace("AtmelDeviceHandler.run", HobString("S_DETACH:  USB node removed"));
                //Successfully removed usbNodeId
                processingDevice = false;
                usbNodeId = 0;
				bNodesRemoved = false;
				m_cs.Leave();

#ifdef UDEV_PATCH
                nextState = S_UDEV_PATCH;
#else
				nextState = S_IDENT;
#endif
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

#ifdef UDEV_PATCH
                    //Create a one time check to make sure we receive an attach from udev
                    Debug::trace("AtmelDeviceHandler.run", "S_DETACH: Creating udev patch timer1");
                    createTimer(UdevPatchTimeoutLimit, PatchTimerId, false);
#endif

					nRemovalTimeout = 0;
					Debug::trace("AtmelDeviceHandler.run", HobString("S_DETACH:  Timeout while waiting for usb DeviceRemoval"));
                    if( usbNodeId )
                        DeviceRemoval(usbNodeId);
					
                    processingDevice = false;
                    usbNodeId = 0;
#ifdef UDEV_PATCH
                    nextState = S_UDEV_PATCH;
#else
                    nextState = S_IDENT;
#endif
				}
			}
			break;
#ifdef UDEV_PATCH
        case S_UDEV_PATCH:
            Debug::trace("AtmelDeviceHandler.run", HobString("procState = S_UDEV_PATCH"));
            if( patchComplete )//Wait for the patch to finish sending attaches to the usbmanager
            {
                cancelTimer(PatchTimerId);
                Debug::trace("AtmelDeviceHandler.run", HobString("S_UDEV_PATCH: finish checking for udev"));
                patchComplete = false;

                nextState = S_IDENT;
            }
            else
            {
                Debug::trace("AtmelDeviceHandler.run", HobString("S_UDEV_PATCH: waiting for scan to finish"));
                bContinue = false;
                nextState = S_UDEV_PATCH;
            }
            break;
#endif
	}

	Debug::trace("AtmelDeviceHandler.run", HobString(GetProcStateName(procState)) + HobString("->") + HobString(GetProcStateName(nextState)) );

	procState = nextState;
	if (bContinue)
		ContinueProcessing();
}

void AtmelDeviceHandler::DeviceArrival(USBD_NODE_ID usbNodeId)
{
	Debug::trace("AtmelDeviceHandler", HobString("DeviceArrival():   SendPrivateMessage(PrivateMsg_t::Arrival, ") + Hex::toString(usbNodeId) + ")");

	// NGW-73 moved processing device logic into event loop

	SendPrivateMessage(PrivateMsg_t::Arrival, usbNodeId);
}

void AtmelDeviceHandler::ProcessDevice( USBD_NODE_ID usbNodeId, UINT8 bus, UINT8 address )
{
	Debug::trace("AtmelDeviceHandler", "ProcessDevice():   SendPrivateMessage(PrivateMsg_t::Process, )" + Hex::toString(usbNodeId) );
	SendPrivateMessage(PrivateMsg_t::Process, usbNodeId, bus, address );
}

void AtmelDeviceHandler::DeviceListArrival()
{
	//process the list of samba devices on the bus
	Debug::trace("AtmelDeviceHandler", "DeviceListArrival():   SendPrivateMessage(PrivateMsg_t::Process, )" );
	SendPrivateMessage(PrivateMsg_t::ListArrival);
}

bool AtmelDeviceHandler::isIdle()
{
	bool result = false;
	if( procState == S_IDLE ) {
		result = true;
	}
	return result;
}

void AtmelDeviceHandler::DeviceRemoval(USBD_NODE_ID usbNodeId)
{
	Debug::trace("AtmelDeviceHandler", HobString("DeviceRemoval():   SendPrivateMessage(PrivateMsg_t::Removal, ") + Hex::toString(usbNodeId) + ")");
	SendPrivateMessage(PrivateMsg_t::Removal, usbNodeId);
}

// send a private message to force processEvent to run
void AtmelDeviceHandler::ContinueProcessing()
{
	Debug::trace("AtmelDeviceHandler", HobString("ContinueProcessing():   SendPrivateMessage(PrivateMsg_t::Continue)"));
	SendPrivateMessage(PrivateMsg_t::Continue);
}

void AtmelDeviceHandler::cleanMap()
{
	std::lock_guard<std::recursive_mutex> g( mutex );

	Devs.clear();
}

bool AtmelDeviceHandler::handleDevicesOnBus()
{
	AT91DeviceLoc loc;
	std::vector<AT91DeviceLoc>::iterator dlIter = deviceList.begin();

	//We know these devices are present on the bus so no need to scan for it.
	while( dlIter != deviceList.end() ) {
		Debug::trace("AtmelDeviceHandler.process", "handleDeviceOnBus() nodeId: " + Hex::toString(dlIter->id) +
	 	             " bus: " + Integer::toString(dlIter->bus) + " address: " + Integer::toString(dlIter->address));

		hOpenDevice = OpenDevice( dlIter->id );
		if( hOpenDevice != 0 ) {
			if( Identify( hOpenDevice, ProductInfo ) != false ) {			
				RequestCodeUpdate( ProductInfo );
				if( bGotCodeUpdate ) {
					Debug::trace("AtmelDeviceHandler.process", "Device needs firmware upgrade");
					if( isPrepack ) {
						ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
					} else {
						ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
						ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading);
					}
					if( DownloadFirmware( hOpenDevice, FirmwareFilename ) ) {
						Debug::trace("AtmelDeviceHandler.process", "DownloadFirmware was successfull");
						//Sleep is needed to allow device finish final page burn before telling it to launch the app
						PAL_absclass::PAL_sleep( 300 );
						if( !LaunchFirmware( hOpenDevice ) ) {
							Debug::trace("AtmelDeviceHandler.process","Error: Failed to launch after firmware download!"); 		
							if( isPrepack ) {
								ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
								ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
							} else {
								ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
								ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, true);
								
								Debug::trace("AtmelDeviceHandler.error", HobString("SENT VersionManagerUsbDeviceDownloadFailed-4"));
								
								ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
							}
						} else {
							Debug::trace("AtmelDeviceHandler.process", "S_PROCESS: Firmware launched OK");
							if( isPrepack ) {
								ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
								ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadSucceeded);
							} else {
								ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
								ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, true);
								ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadSucceeded);
							}
							//SOF-5375	
							// SOF-TIME-71 for upgrade of 2 printer (Epp2PrinterSW) system of the same type!	isWrapperTwoPrinter()		 
							if( FpmManager::getInstance()->isWrapperTwoPrinter() )
							{
								if( prCounter == 2 )
								{	
									Debug::trace("AtmelDeviceHandler", "( B ) prCounter: " + Integer::toString( prCounter ) ); 					
									File firmwareFile(FirmwareFilename);
									if (firmwareFile.exists())
									{
										Debug::trace("AtmelDeviceHandler", "remove file: "+ FirmwareFilename );
										firmwareFile.remove();
									}							
								}
							} else
							{
									File firmwareFile(FirmwareFilename);
									if (firmwareFile.exists())
									{
										Debug::trace("AtmelDeviceHandler", "remove file: "+ FirmwareFilename );
										firmwareFile.remove();
									}							
							}
						} 
					} else {
						Debug::trace("AtmelDeviceHandler.process","Error: Failed to download firmware!");
						if( isPrepack ) {
							ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloading, true);
							ReportStatusToVersionManager(VersionManagerAtmelDeviceDownloadFailed);
						} else {
							Debug::trace("AtmelDeviceHandler.error", HobString("SENT VersionManagerUsbDeviceDownloadFailed-3"));
				
							ReportStatusToVersionManager(VersionManagerUsbDeviceResetting, true);
							ReportStatusToVersionManager(VersionManagerUsbDeviceDownloading, true);
							ReportStatusToVersionManager(VersionManagerUsbDeviceDownloadFailed);
						}
					}
					bGotCodeUpdate = false;
				} else {
					Debug::trace("AtmelDeviceHandler.process", "No firmware to burn");
					if( !LaunchFirmware( hOpenDevice ) ) {
						Debug::trace("AtmelDeviceHandler.process","Error: Failed to launch firmware!"); 		
					}
				}
			}
		} else {
			Debug::trace("AtmelDeviceHandler","handleDeviceOnBus() Error: Could not open device id: " + Hex::toString(dlIter->id) +
						" on bus: " + Integer::toString(dlIter->bus) + " with address: " + Integer::toString(dlIter->address) ); 	
		}
		dlIter++;
	}
	deviceList.clear();
	usbNodeList.clear();	//just in case anything has been added to this list.
	procState = S_IDLE;

	return 0;
}

// Call the DLL to scan for connected AT91 devices. The DLL
// builds a list of sym links for the detected devices.
int AtmelDeviceHandler::ScanDevices()
{
	int n( 0 );

	pMyDescriptors = NULL;

	nDevices = 0;	   // member count of USB paths in DevList
	DevListIndex = 0;   // member index of current USB path

	try
	{
		struct udev *udev( NULL );

		if ( (udev = udev_new()) )
		{
			struct udev_enumerate *enumerate;

			if ( (enumerate = udev_enumerate_new( udev )) )
			{
				struct udev_list_entry *dev_list_entry;

				udev_enumerate_add_match_subsystem( enumerate, "usb" );
				udev_enumerate_scan_devices( enumerate );

				if ( (dev_list_entry = udev_enumerate_get_list_entry( enumerate )) )
				{
					/* For each item enumerated, print out its information.
					 * udev_list_entry_foreach is a macro which expands to
					 * a loop. The loop will be executed for each member in
					 * devices, setting dev_list_entry to a list entry
					 * which contains the device's path in /sys. */

					Debug::trace("ScaleService", ("Run:  Enumerating attached USB devices"));

					do
					{
						const char *path;
						struct udev_device *dev;

						/* Get the filename of the /sys entry for the device
						 * and create a udev_device object (dev) representing it */
						path = udev_list_entry_get_name( dev_list_entry );

						Debug::trace("AtmelDeviceHandler", HobString("device path: ") + HobString(path));

						if ( (dev = udev_device_new_from_syspath( udev, path )) )
						{
							struct udev_device *parent;

							/* The device pointed to by dev contains information about
							 * the tty device. In order to get information about the
							 * USB device, get the parent device with the
							 * subsystem/devtype pair of "usb"/"usb_device". This will
							 * be several levels up the tree, but the function will find
							 * it.*/
							if ( (parent = udev_device_get_parent_with_subsystem_devtype( dev, "usb", "usb_device" )) )
							{
								int vid = 0;
								int pid = 0;

								const char *cvid = udev_device_get_sysattr_value( parent, "idVendor" );
								const char *cpid = udev_device_get_sysattr_value( parent, "idProduct" );

								if (( cvid ) && ( cpid ))
								{
									vid = strtol( cvid, NULL, 16 );
									pid = strtol( cpid, NULL, 16 );
								}

								// look for Atmel devices
								if (( ATMEL_SAMBA_VENDOR_ID == vid ) && ( ATMEL_SAMBA_PRODUCT_ID == pid ))
								{
									/* usb_device_get_devnode() returns the path to the device node
									 * itself in /dev. */
									const char *s( udev_device_get_devnode( parent ) );

									if ( !s )
										Debug::trace("AtmelDeviceHandler", HobString("Bad devnode!" ));
									else
									{
										const string devnode_name( s );
										bool found = false;

										Debug::trace( "AtmelDeviceHandler", HobString( "Atmel device on " + devnode_name ) );

										// iterate through list, make sure not in list already
										for ( std::vector<std::string>::iterator i = Devs.begin(); i != Devs.end(); i++ )
											found |= ( devnode_name == *i );

										if ( found )
											Debug::trace( "AtmelDeviceHandler", "skipping, already in list" );
										else
										{
											Devs.push_back( devnode_name );
											++n;
										}
									}
								}
							}

							udev_device_unref( dev );
						}
					} while ( (dev_list_entry= udev_list_entry_get_next( dev_list_entry )) );
				}

				udev_enumerate_unref( enumerate );
			}

			udev_unref( udev );
		}
	}
	catch (...)
	{
		Debug::trace("AtmelDeviceHandler", HobString("ScanDevices:  Failed, error 0x") + Hex::toString(PAL_absclass::PAL_getLastError()) );
		return 0;
	}

	return (nDevices = n);
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
	}
	else if (pmsg->op == PrivateMsg_t::Deinit)
	{
		Debug::trace("AtmelDeviceHandler", HobString("HandlePrivateMessage:  Received Deinitialize msg"));
	}
	else if (pmsg->op == PrivateMsg_t::Arrival)
	{
		Debug::trace("AtmelDeviceHandler",
			HobString("HandlePrivateMessage:  Adding usb node ")
					+ Hex::toString(pmsg->usbnode));

		if( !processingDevice )
	    {
	        Debug::trace("AtmelDeviceHandler", HobString("DeviceArrival: Now Processing nodeId:") + Hex::toString(usbNodeId));
			processingDevice = true;
			AtmelDeviceHandler::usbNodeId = usbNodeId;
	    }

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
	else if( pmsg->op == PrivateMsg_t::Process ) 
	{	
		Debug::trace("AtmelDeviceHandler", "HandlePrivateMessage:  PrivateMsg_t::Process procState: " + Integer::toString(procState));		
		AT91DeviceLoc loc;

		loc.id = pmsg->usbnode;
		loc.bus = pmsg->bus;
		loc.address = pmsg->address;

		deviceList.push_back(loc);
	} 
	else if( pmsg->op == PrivateMsg_t::ListArrival )
	{
		Debug::trace("AtmelDeviceHandler", "HandlePrivateMessage:  PrivateMsg_t::Process procState: " + Integer::toString(procState));		
		if( procState == S_IDLE ) {
			Debug::trace("AtmelDeviceHandler", "HandlePrivateMessage: Processing device list");
			procState = S_PROCESS;
		} else {
			Debug::trace("AtmelDeviceHandler", "HandlePrivateMessage: Cannot process device list state machine not idle!");
		}
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
				//codeUpdate.show();

                if(codeUpdate.getNodeId() == usbNodeId)//check if this message is for us
				{
				    switch (codeUpdate.getResponseType())
				    {
					    case SoftwareSystem::AvailableSameVersion:
					    {
						    bGotCodeUpdate = true;
						    bWaitTimeout = false;
						    needsUpdate = false;
						    Debug::trace("AtmelDeviceHandler", HobString("AvailableSameVersion") );
						    break;
					    }

					    case SoftwareSystem::AvailableUpgrade:							 
					    {
						    Debug::trace("AtmelDeviceHandler", HobString("AvailableUpgrade") );						
						    util_filesystem::File sourceBin(codeUpdate.getPath()->getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME));
						    bGotCodeUpdate = true;
						    bWaitTimeout = false;

						    if (sourceBin.exists() == true)
						    {
							    FirmwareFilename.assign(sourceBin.getFullPath().c_str());
							    needsUpdate = true;
						    }
						    else
						    {
							    needsUpdate = false;
						    }
						    break;
					    }
					    case SoftwareSystem::NotAvailable:
					    {
						    bGotCodeUpdate = true;
						    bWaitTimeout = false;
						    needsUpdate = false;
						    Debug::trace("AtmelDeviceHandler", HobString("NotAvailable"));
						    break;
					    }

					    case SoftwareSystem::retryCntrOver:
						    // TODO: abort and prevent further update attempts
						    Debug::trace("AtmelDeviceHandler", HobString("retryCntrOver"));
						    break;

					    default:
						    Debug::trace("AtmelDeviceHandler", HobString("unknown 0x")
									    + Hex::toString((long)codeUpdate.getResponseType()) );
						    break;
				    }
                }
				break;
			}
			case UpgradeAutomatic:
				Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage: got UpgradeAutomatic event from Version Manager"));
				break;
			case UpgradeManual:
				Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage: got UpgradeManual event from Version Manager"));
				break;
			case CancelUpgrade:
				Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage: got CancelUpgrade event from Version Manager"));
				break;
			case BeginUpgrade:
				Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage: got BeginUpgrade event from Version Manager"));
				break;
			case ResetFPCComponent:
				Debug::trace("AtmelDeviceHandler", HobString("HandlePublicMessage(): got ResetFPCComponent event from Version Manager - Linux"));
                if( !isPrepack )
				{
				    handleResetFPCComponent();
				}
				else
				{
					// SOF-4447: Need to re-boot to complete the weigher firmware upgrade on a wrapper

					Debug::trace("AtmelDeviceHandler", HobString("Creating message dialog with reboot comment for pre-pack"));

					SessionManagerHelper::getInstance()->addDialog(0, 1, "You need to reboot the console to complete the upgrade.");
				}
				break;
			default:
				Debug::trace("AtmelDeviceHandler",
						HobString("HandlePublicMessage: got unknown event 0x")
						+ Hex::toString((long)ev)
						+ HobString(" from Version Manager") );
				break;
		}
		rc = true;
	}
	else
	{
		Debug::trace("AtmelDeviceHandler",
				HobString("HandlePublicMessage: got unknown message type 0x")
				+ Hex::toString((long)devType));
	}
	return rc;
}



void AtmelDeviceHandler::AT91Boot_Write_Int(AT91_DEVICE_HANDLE fd, int value, int address, int *error)
{
	*error = -1;

	if(fd != NULL)
	{
		char outMsg[256];
		int transfer = 0;

		memset(outMsg, 0 , 256);

		//build command
		sprintf(outMsg, "W%x,%08x#", address, value);

		//send bulk out read command to the Atmel device without timeout.
		int rval;
		rval = libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)outMsg, strlen(outMsg), &transfer, 0 );

		Debug::trace("AtmelDeviceHandler", HobString("AT91Boot_Write_Int() - libusb_bulk_transfer()->rval=") + Integer::toString(rval));
		Debug::trace("AtmelDeviceHandler", HobString("AT91Boot_Write_Int() - libusb_bulk_transfer() - message length (bytes):") + Integer::toString(strlen(outMsg)) + HobString(" Bytes transfered:") + Integer::toString(transfer));

		if (transfer != (int)strlen(outMsg))
		{
			Debug::trace("AtmelDeviceHandler", HobString("Write_Int: BULK OUT transfer failed!!! [1] message length (bytes):") + Integer::toString(strlen(outMsg)) + HobString(" Bytes transfered:") + Integer::toString(transfer));
		}
		else
		{
			*error = 0;
		}
	}
}

void AtmelDeviceHandler::AT91Boot_Read_Int(AT91_DEVICE_HANDLE fd, int *value, int address, int *error)
{
	*error = -1;

	if(fd != NULL)
	{
		int bulkError;

		char outMsg[256], inMsg[64], modMsg[2] = {'N','#'};
		int transfer = 0;

		memset(outMsg, 0 , 256);
		memset(inMsg, 0 , 64);

		//build command
		sprintf(outMsg, "w%x,4#", address);

		if ( (bulkError = libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)modMsg, 2, &transfer, 5000)) )
		{
			Debug::trace("AtmelDeviceHandler", HobString("Read_Int: BULK Out Mod transfer error:") + Integer::toString(bulkError));
		}
		else if ( (bulkError = libusb_bulk_transfer(fd, (unsigned char)bulkInAddress, (unsigned char *)inMsg, 2, &transfer, 1000)) )
		{
			Debug::trace("AtmelDeviceHandler", HobString("Read_Int: read transfer error:") + Integer::toString(bulkError));
		}
		else
		{
			//response not needed, toss it!
			memset(inMsg, 0 , 64);

			//send bulk out read command to the Atmel device without timeout.
			bulkError = libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)outMsg, strlen(outMsg), &transfer, 5000);
			if(transfer == (int)strlen(outMsg))
			{
				transfer = 0;
				Debug::trace("AtmelDeviceHandler", HobString("Read_Int: BULK Out transfer passed error:") + Integer::toString(bulkError));
				//get response to the bulk out read command without timeout. 64 = max numb bytes to rx
				bulkError = libusb_bulk_transfer(fd, (unsigned char)bulkInAddress, (unsigned char *)inMsg, 4, &transfer, 5000);
				if(!bulkError)
				{
					*error = 0;
					//long version = strtol(inMsg, NULL, 16);
					long version = 0;
					memcpy(&version, inMsg, 4);

					*value = (int)version;
				}
				else
				{
					Debug::trace("AtmelDeviceHandler", HobString("Read_Int: BULK IN transfer failed!!! error:") + Integer::toString(bulkError) + ("Read_Int: BULK IN bytes read:") + Integer::toString(transfer));
					long version = strtol(inMsg, NULL, 16);
					Debug::trace("AtmelDeviceHandler", HobString("Read_Int: BULK IN transfer failed!!! error:") + Integer::toString(bulkError) + ("buffer:") + Integer::toString((int)version));
				}
			}
			else
			{

				Debug::trace("AtmelDeviceHandler", HobString("Read_Int: BULK OUT transfer failed!!! [2] message length (bytes):") + Integer::toString(strlen(outMsg)) + HobString(" Bytes transfered:") + Integer::toString(transfer)  + HobString("Bulk Error:") + Integer::toString(bulkError));
			}
		}
	}
}

void AtmelDeviceHandler::AT91Boot_Read_Short(AT91_DEVICE_HANDLE fd, short *value, int address, int *error)
{
	*error = -1;
	int size = 2;

	if(fd != NULL)
	{
		char outMsg[256], inMsg[256];
		int transfer = 0;

		memset(outMsg, 0 , 256);
		memset(inMsg, 0 , 256);

		//build command
		sprintf(outMsg, "h%08x,%x#", address, size);

		//send bulk out read command to the Atmel device without timeout.
		libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)outMsg, strlen(outMsg), &transfer, 0);
		if(transfer == (int)strlen(outMsg))
		{
			//get response to the bulk out read command without timeout. 64 = max numb bytes to rx
			if(!libusb_bulk_transfer(fd, (unsigned char)bulkInAddress, (unsigned char *)inMsg, size, &transfer, 0))
			{
				*error = 0;

				memcpy(value, inMsg, size);

			}
			else
			{
				Debug::trace("AtmelDeviceHandler", HobString("Read_Short: BULK IN transfer failed!!! message length (bytes):") + Integer::toString(transfer));
			}
		}
		else
		{
			Debug::trace("AtmelDeviceHandler", HobString("Read_Short: BULK OUT transfer failed!!! [3] message length (bytes):") + Integer::toString(strlen(outMsg)) + HobString(" Bytes transfered:") + Integer::toString(transfer));
		}
	}
}

void AtmelDeviceHandler::AT91Boot_Read_Byte(AT91_DEVICE_HANDLE fd, unsigned char *value, int address, int *error)
{
	*error = -1;


	 if(fd != NULL)
	 {
	 	char outMsg[256], inMsg[256];
	 	int transfer = 0;

	 	memset(outMsg, 0 , 256);
		memset(inMsg, 0 , 256);

		//build command
		sprintf(outMsg, "o%08x,#", address);


	 	//send bulk out read command to the Atmel device without timeout.
	 	libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)outMsg, strlen(outMsg), &transfer, 0);
	 	if(transfer == (int)strlen(outMsg))
	 	{
		 	//get response to the bulk out read command without timeout. 64 = max numb bytes to rx
		 	if(!libusb_bulk_transfer(fd, (unsigned char)bulkInAddress, (unsigned char *)inMsg, 1, &transfer, 0))
		 	{
		 		*error = 0;

				*value = inMsg[0];
		 	}
		 	else
		 	{
		 		Debug::trace("AtmelDeviceHandler", HobString("Read_Byte: BULK IN transfer failed!!! message length (bytes):") + Integer::toString(transfer));
		 	}
	 	}
	 	else
	 	{
	 		Debug::trace("AtmelDeviceHandler", HobString("Read_Byte: BULK OUT transfer failed!!! [4] message length (bytes):") + Integer::toString(strlen(outMsg)) + HobString(" Bytes transfered:") + Integer::toString(transfer));
	 	}
	 }
}

void AtmelDeviceHandler::AT91Boot_SAM7S256_Send_LoaderImage(AT91_DEVICE_HANDLE h, int *error)
{
	
	*error = -1;

	if(h != NULL)
	{
	 	char outMsg[256], inMsg[256];
	 	int transfer = 0;

	 	memset(outMsg, 0 , 256);


		//build command
		sprintf(outMsg, "S%08X,%08X#", SAM7S256_LOADER_IMAGE_ADDRESS, SAM7S256_LOADER_IMAGE_SIZE);


	 	//send bulk out command: "Send File" to the Atmel device
	 	libusb_bulk_transfer(h, (unsigned char)bulkOutAddress, (unsigned char *)outMsg, strlen(outMsg), &transfer, 0);
	 	if(transfer == (int)strlen(outMsg))
	 	{
	 		transfer = 0;

	 		//write the loader image to the Atmel device
	 		if(!libusb_bulk_transfer(h, (unsigned char)bulkOutAddress, (unsigned char *)loaderImage, SAM7S256_LOADER_IMAGE_SIZE, &transfer, 0))
	 		{
	 			if(transfer == (int)SAM7S256_LOADER_IMAGE_SIZE)
	 			{
	 				int i = 0;

	 				//build command to read the flash memory controller status register
	 				sprintf(outMsg, "w%X,%X#", EFC_FMR_STATUS, 4);

	 				// the windows Dll reads the status register 15 times not sure why, status per page?
	 				for(i = 0; i < 15; i++ )
	 				{
	 					transfer = 0;
	 					if (!libusb_bulk_transfer(h, (unsigned char)bulkOutAddress, (unsigned char *)outMsg,  strlen(outMsg), &transfer, 0))
	 					{
	 						memset(inMsg, 0 , 256);
	 						transfer = 0;

	 						//read the flash status register
	 						if (!libusb_bulk_transfer(h, (unsigned char)bulkInAddress, (unsigned char *)inMsg,  64, &transfer, 0))
	 						{
	 							*error = 0;

	 							short statusRegister = (short) strtol(&inMsg[0], NULL, 16);
	 							if(statusRegister != (short)0x01030300)
	 							{
									//This is not an error.... 
	 		 						Debug::trace("AtmelDeviceHandler.Status", HobString("Flash not ready: ") + Hex::toString(statusRegister));
	 							}
	 						}
	 						else
	 						{
		 						Debug::trace("AtmelDeviceHandler", HobString("Send_LoaderImage: Read of Flash Status Register failed!!! message length (bytes):"));
	 							break;
	 						}
	 					}
	 					else
	 					{
	 						Debug::trace("AtmelDeviceHandler", HobString("Send_LoaderImage: Status Cmd BULK OUT transfer failed!!! [5] message length (bytes):") + Integer::toString(strlen(outMsg)) + HobString(" Bytes transfered:") + Integer::toString(transfer));
	 						break;
	 					}
	 				}
	 			}
	 			else
	 			{
	 				Debug::trace("AtmelDeviceHandler", HobString("Send_LoaderImage: Image BULK OUT transfer failed!!! [6] message length (bytes):") + Integer::toString(SAM7S256_LOADER_IMAGE_SIZE) + HobString(" Bytes transfered:") + Integer::toString(transfer));
	 			}
	 		}
	 		else
	 		{
	 			Debug::trace("AtmelDeviceHandler", HobString("Send_LoaderImage: Image BULK OUT transfer failed!!! [7] message length (bytes):") + Integer::toString(SAM7S256_LOADER_IMAGE_SIZE) + HobString(" Bytes transfered:") + Integer::toString(transfer));
	 		}
	 	}
	 	else
	 	{
	 		Debug::trace("AtmelDeviceHandler", HobString("Send_LoaderImage: CMD BULK OUT transfer failed!!! [8] message length (bytes):") + Integer::toString(strlen(outMsg)) + HobString(" Bytes transfered:") + Integer::toString(transfer));
	 	}
	}
}


void AtmelDeviceHandler::AT91Boot_SAM7S256_Send_Flash(AT91_DEVICE_HANDLE fd, int UNUSED(start_addr), unsigned char* fwfileBuf, int UNUSED(fwLength), int *error_code)
{
	//command buffers
	char sendFileCmd[20];
	char pageCntCmd[20];

	char imageBuff[266];	
	/*Go command	:	G00201600# Jump back to the laoder image*/		
	char goCommand[SAM7S256_GO_SIZE] = {0x47, 0x30, 0x30, 0x32, 0x30, 0x31, 0x36, 0x30, 0x30,0x23};

	unsigned int pageCounter = SAM7S256_START_PAGE_OFFSET;
	int transfer = 0;
	
	*error_code = -1;
	
	if(fd != NULL)
	{
		memset(sendFileCmd, 0, 19);
		memset(pageCntCmd, 0, 19);
		memset(imageBuff, 0, 266);

		//copy the go command into the last 10 bytes of the image buffer. =	new page + command
		memcpy(&imageBuff[SAM7S256_PAGE_SIZE], goCommand,  SAM7S256_GO_SIZE);

		do
		{
			transfer = 0;

			//setup the message to write page counter value to the Atmel device
			sprintf(pageCntCmd, "W%08x,%08x#", SAM7S256_Page_Counter_Address, pageCounter);

			if (!libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)pageCntCmd,  strlen(pageCntCmd), &transfer, 0))
			{
				transfer = 0;
				//setup the send file command
				sprintf(sendFileCmd, "S%08x,%08x#", SAM7S256_IMAGE_BUFF_ADDRESS, SAM7S256_PAGE_SIZE);

				if (!libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)sendFileCmd,  strlen(sendFileCmd), &transfer, 0))
				{
					transfer = 0;
					//copy the page image to burn into our buffer
					memcpy(imageBuff, fwfileBuf,  SAM7S256_PAGE_SIZE);

					//send the firmware image to the device
					if (!libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)imageBuff,  (SAM7S256_PAGE_SIZE + SAM7S256_GO_SIZE), &transfer, 0))
					{
						transfer = 0;
						*error_code = 0;
					}
					else
					{
						Debug::trace("AtmelDeviceHandler", HobString("Flashing Image: failed to send image to device. page counter:") + Integer::toString(pageCounter) + HobString("number of pages on device:") + Integer::toString(SAM7S256_MAX_PAGES));
						*error_code = -1;
					}
				}
				else
				{
					Debug::trace("AtmelDeviceHandler", HobString("Flashing Image: failed to send file command. command size:") + Integer::toString(strlen(sendFileCmd)) + HobString("number of bytes transfered:") + Integer::toString(transfer));
					*error_code = -1;
				}
			}
			else
			{
				Debug::trace("AtmelDeviceHandler", HobString("Flashing Image: failed to send page counter. command size:") + Integer::toString(strlen(pageCntCmd)) + HobString("number of bytes transfered:") + Integer::toString(transfer));
				*error_code = -1;
			}

			pageCounter++;
			if(pageCounter != SAM7S256_MAX_PAGES)
			{
				//get next page to burn
				fwfileBuf += SAM7S256_PAGE_SIZE;
			}
		}
		while( (pageCounter != SAM7S256_MAX_PAGES) && (!*error_code) );
	}
}

void AtmelDeviceHandler::AT91Boot_Go(AT91_DEVICE_HANDLE fd, int start_addr, int *error)
{
	char outMsg[20];
	int transfer = 0;
	int libError = 0;

	*error = -1;


	memset(outMsg, 0, 20);
	sprintf(outMsg, "G%08x#", start_addr);

	if(fd != NULL)
	{
		libError = libusb_bulk_transfer(fd, (unsigned char)bulkOutAddress, (unsigned char *)outMsg,  strlen(outMsg), &transfer, 0);
		if(libError == 0)
		{
			if(transfer == (int)strlen(outMsg))
			{
				*error = AT91C_BOOT_DLL_OK;
			}
		}
		else
		{
			if(libError == -4)
			{
				*error = AT91C_BOOT_DLL_OK;
			}
			else
			{
				Debug::trace("AtmelDeviceHandler", HobString("Go Command Transfer Failed: ") + Integer::toString(libError));
			}
		}
	}
	else
	{
		Debug::trace("AtmelDeviceHandler", HobString("Go: No Handle to device!!:"));
	}
}

bool AtmelDeviceHandler::Identify(AT91_DEVICE_HANDLE h, AT91ProductInfo& info)
{
	bool rc = false;
	int uValue = 0;
	int error_code = 0;

	memset(&info, 0, sizeof(AT91ProductInfo));

	try
	{

		AT91Boot_Read_Int(h, &uValue, DBGU_CIDR_ADDR, &error_code);
		if (error_code) 
		{	
			Debug::trace("AtmelDeviceHandler", HobString("Read failure!!: ") +  Integer::toString(error_code));

			chipRev = NoRevision;
			goto _errDll;
		}

		//is this a SAM7S256 device and what is the chip revision?
		if((unsigned int)(uValue) == SAM7S256A_DBGU_CIDR)
		{
			Debug::trace("AtmelDeviceHandler", "Silicon Revision A "  +  Hex::toString(uValue));
			chipRev = Revision_A;
		}
		else if((unsigned int)(uValue) == SAM7S256B_DBGU_CIDR)
		{
			Debug::trace("AtmelDeviceHandler", "Silicon Revision B "  +  Hex::toString(uValue));

			chipRev = Revision_B;
		}
		else if((unsigned int)(uValue) == SAM7S256C_DBGU_CIDR)
		{
			Debug::trace("AtmelDeviceHandler", "Silicon Revision C "  +  Hex::toString(uValue));
			chipRev = Revision_C;
		}
		else if((unsigned int)(uValue) == SAM7S256D_DBGU_CIDR)
		{
			Debug::trace("AtmelDeviceHandler", "Silicon Revision D "  +  Hex::toString(uValue));
			chipRev = Revision_D;
		}
		else
		{
			Debug::trace("AtmelDeviceHandler", HobString("Identify failed.  Value = ") +  Hex::toString(uValue) + 
												HobString(". Expected either ") + Hex::toString(SAM7S256A_DBGU_CIDR) +
												HobString(" OR: ") + Hex::toString(SAM7S256B_DBGU_CIDR) + 
												HobString(" OR: ") + Hex::toString(SAM7S256C_DBGU_CIDR) +
												HobString(" OR: ") + Hex::toString(SAM7S256D_DBGU_CIDR));
			chipRev = NoRevision;
//	Removed as of 1/8/14. Even if we don't recognize this chip revision, allow it to continue. Subsequent revisions have not impacted our weigher board.
//			goto _identExit;
		}


		AT91Boot_Read_Short(h, (short *)&info.VendorId,  SAM7S256_CMD_ADDR_VENDOR_ID, &error_code);
		if (error_code) goto _errDll;

		AT91Boot_Read_Short(h, (short *)&info.ProductId, SAM7S256_CMD_ADDR_PRODUCT_ID, &error_code);
		if (error_code) goto _errDll;

		AT91Boot_Read_Byte(h,  &info.HardwareVerMajor,   SAM7S256_CMD_HWVERSION_MAJOR, &error_code);
		if (error_code) goto _errDll;

		AT91Boot_Read_Byte(h,  &info.HardwareVerMinor,   SAM7S256_CMD_HWVERSION_MINOR, &error_code);
		if (error_code) goto _errDll;

		AT91Boot_Read_Short(h, (short *)&info.SoftwareVerMajor,  SAM7S256_CMD_SWVERSION_MAJOR, &error_code);
		if (error_code) goto _errDll;

		AT91Boot_Read_Short(h, (short *)&info.SoftwareVerMinor,  SAM7S256_CMD_SWVERSION_MINOR, &error_code);
		if (error_code) goto _errDll;

		AT91Boot_Read_Short(h, (short *)&info.checksum,  SAM7S256_CMD_CHECKSUM, &error_code);
		if (error_code) goto _errDll;

		if((info.HardwareVerMajor == 0) && (info.HardwareVerMinor == 0))
		{
			/*older versions of APC does not provide a hardware revision.
			 * we need a rev for Version Manager to provide an upgrade.
			 */
			info.HardwareVerMajor = 1;
			info.HardwareVerMinor = 0;
		}
		rc = true;
		goto _identExit;

_errDll:
		Debug::trace("AtmelDeviceHandler", HobString("Identify:  Failed, DLL ERROR 0x")
				+ Hex::toString((ULONG)error_code));
	}
	catch (...)
	{
		Debug::trace("AtmelDeviceHandler",
				HobString("Identify:  Failed on exception") );
	}

_identExit:

	if (rc)
	{
		// dump product info
		Debug::trace("AtmelDeviceHandler.Identify", HobString("VendorId  0x") + Hex::toString(info.VendorId		) );
		Debug::trace("AtmelDeviceHandler.Identify", HobString("ProductId 0x") + Hex::toString(info.ProductId	   ) );
		Debug::trace("AtmelDeviceHandler.Identify", HobString("HW major  0x") + Hex::toString(info.HardwareVerMajor) );
		Debug::trace("AtmelDeviceHandler.Identify", HobString("HW minor  0x") + Hex::toString(info.HardwareVerMinor) );
		Debug::trace("AtmelDeviceHandler.Identify", HobString("SW major  0x") + Hex::toString(info.SoftwareVerMajor) );
		Debug::trace("AtmelDeviceHandler.Identify", HobString("SW minor  0x") + Hex::toString(info.SoftwareVerMinor) );
		Debug::trace("AtmelDeviceHandler.Identify", HobString("Checksum  0x") + Hex::toString(info.checksum		) );
	}

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
			Debug::trace("AtmelDeviceHandler", HobString("VerifyFirmwareFile: file does not exist") );
			goto _vfwExit;
		}
		long64 fileLength = fwfile.size();
		if (fileLength == 0 )
		{
			Debug::trace("AtmelDeviceHandler", HobString("VerifyFirmwareFile: file is zero bytes") );
			goto _vfwExit;
		}
		if (fileLength > SAM7S256_FLASH_SIZE)
		{
			Debug::trace("AtmelDeviceHandler", HobString("VerifyFirmwareFile: file is too big") );
			goto _vfwExit;
		}
		rc = true;
	}
_vfwExit:
	return rc;
}


bool AtmelDeviceHandler::DownloadFirmware(AT91_DEVICE_HANDLE h, std::string& fwfilename)
{
	bool rc = false;
	bool bFileValid = false;

	if (0 > h)
	{
		Debug::trace("AtmelDeviceHandler", HobString("DownloadFirmware: device not open") );
		return false;
	}

	// allocate two buffers
	char fwfileBuf[SAM7S256_FLASH_SIZE];

	// number of bytes to be flashed
	UINT32 fwLength=0;

	{
		// get file length
		util_filesystem::Filename fn(fwfilename);
		util_filesystem::File fwfile(fn);
		long64 fileLength = fwfile.size();
		fwLength = (UINT32) fileLength & 0xffffffff;
		if (!fwfile.exists())
		{
			Debug::trace("AtmelDeviceHandler", HobString("DownloadFirmware:  Firmware file does not exist!") );
			goto _dlfwCmpFail;
		}
		if (fileLength > sizeof(fwfileBuf)) // || fileLength == 0
		{
			Debug::trace("AtmelDeviceHandler", HobString("DownloadFirmware:  Firmware file is too big") );
			goto _dlfwCmpFail;
		}
		if (fileLength == 0 )
		{
			Debug::trace("AtmelDeviceHandler", HobString("DownloadFirmware:  Firmware file is zero bytes") );
			goto _dlfwCmpFail;
		}

		// open file
		try
		{
			ifstream sourceFile(fwfile.getFullPath().c_str(), ios::in|ios::binary);

			if ( !sourceFile )
			{
				Debug::trace("AtmelDeviceHandler", HobString("DownloadFirmware:  Couldn't read firmware file") );
				goto _dlfwCmpFail;
			}

			sourceFile.read(fwfileBuf,fwLength);
			bFileValid = true;

		}
		catch(...)
		{
			Debug::trace("AtmelDeviceHandler", HobString("DownloadFirmware:  Couldn't read firmware file!") );
			goto _dlfwCmpFail;
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
		AT91Boot_Write_Int(h, EFC_FMR_ADDR, EFC_FMR_VALUE, &error_code);
		if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;

		//send the loader image
		AT91Boot_SAM7S256_Send_LoaderImage(h, &error_code);
		if(error_code != AT91C_BOOT_DLL_OK) goto _errDll;

		// write the data to flash
		AT91Boot_SAM7S256_Send_Flash(h, SAM7S256_FIRMWARE_FLASH_ADDRESS, (unsigned char*) fwfileBuf, fwLength, &error_code);
		if (error_code != AT91C_BOOT_DLL_OK) goto _errDll;


		// success
		rc = true;
		goto _dlfwExit;

_errDll:
		Debug::trace("AtmelDeviceHandler", HobString("DownloadFirmware: failed, DLL error 0x")
				+ Hex::toString((ULONG)error_code));
	}
	catch (...)
	{
		Debug::trace("AtmelDeviceHandler",
				HobString("DownloadFirmware: failed on exception") );
	}
_dlfwExit:
	return rc;
}

bool AtmelDeviceHandler::LaunchFirmware(AT91_DEVICE_HANDLE h)
{
	bool rc = false;
	int error_code = 0;

	if (0 > h)
	{
		Debug::trace("AtmelDeviceHandler", HobString("LaunchFirmware: device not open") );
		return false;
	}

	try
	{

		AT91Boot_Go(h, SAM7S256_FIRMWARE_START_ADDRESS, &error_code);
		if (error_code) goto _errDll;

		Debug::trace("AtmelDeviceHandler", "LaunchFirmware");

		rc = true;
		if( !isPrepack ) {
			Debug::trace("AtmelDeviceHandler", HobString("LaunchFirmware: application firmware successfully launched.") );
			RealTimeController::getInstance()->startFpcProbeTimer();
		}
		goto _launchExit;

_errDll:
		Debug::trace("AtmelDeviceHandler", HobString("LaunchFirmware: failed, DLL error 0x")
				+ Hex::toString((ULONG)error_code));
	}
	catch (...)
	{
		Debug::trace("AtmelDeviceHandler",
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

	Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate swverType: " + Integer::toString(swver.getVersionType()));

	if ( swver.getVersionType() == HobVersion::ApcPrinterSW )
	{
		Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - software dev is ApcPrinterSW");

		Directory directoryFromWhereFirmwareGetsBurnedToDevice("");
		HobComplexVersion::getExtractionDir(swver.getVersionType(), directoryFromWhereFirmwareGetsBurnedToDevice, 0); // the last parameter isn't used, but has to be supplied

		File firmwareFile(directoryFromWhereFirmwareGetsBurnedToDevice.getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME));
		if (firmwareFile.exists())
		{
			FirmwareFilename = firmwareFile.getFullPath();
			Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - Have firmware to burn to APC: " + FirmwareFilename);
			bWaitTimeout = false;
			bGotCodeUpdate = true;
			needsUpdate = true;
		}
		else
		{
			Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - Did not find firmware to burn to APC: " + firmwareFile.getFullPath());
			bWaitTimeout = true;
			bGotCodeUpdate = false;
			needsUpdate = false;
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
	else if( swver.getVersionType() == HobVersion::Epp2PrinterSW ) 
	{
		Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - software dev is Epp2PrinterSW");
		Directory directoryFromWhereFirmwareGetsBurnedToDevice("");
		HobComplexVersion::getExtractionDir(swver.getVersionType(), directoryFromWhereFirmwareGetsBurnedToDevice, 0);

		File firmwareFile( directoryFromWhereFirmwareGetsBurnedToDevice.getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME ) );
		if( firmwareFile.exists() ) {
			FirmwareFilename = firmwareFile.getFullPath();
			Debug::trace("AtmelDeviceHandler", "Epp2PrinterSW: "+ FirmwareFilename );
			bGotCodeUpdate = true;
		} else {
			bGotCodeUpdate = false;
		}
		return true;
	}
	else if( swver.getVersionType() == HobVersion::Aws2WeigherSW ) 
	{
		Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - software dev is Aws2WeigherSW");
		Directory directoryFromWhereFirmwareGetsBurnedToDevice("");
		HobComplexVersion::getExtractionDir(swver.getVersionType(), directoryFromWhereFirmwareGetsBurnedToDevice, 0);

		File firmwareFile( directoryFromWhereFirmwareGetsBurnedToDevice.getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME ) );
		if( firmwareFile.exists() )
		{
			FirmwareFilename = firmwareFile.getFullPath();
			Debug::trace("AtmelDeviceHandler", "Aws2WeigherSW: "+ FirmwareFilename );
			bGotCodeUpdate = true;
		} else {
			bGotCodeUpdate = false;
		}
		return true;
	}
	else if( swver.getVersionType() == HobVersion::	Epp3PrinterSW ) //SOF-TIME-71
	{		
		Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - software dev is Epp3PrinterSW");
		Directory directoryFromWhereFirmwareGetsBurnedToDevice("");
		HobComplexVersion::getExtractionDir(swver.getVersionType(), directoryFromWhereFirmwareGetsBurnedToDevice, 0);

		File firmwareFile( directoryFromWhereFirmwareGetsBurnedToDevice.getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME ) );
		if( firmwareFile.exists() ) {
			FirmwareFilename = firmwareFile.getFullPath();
			Debug::trace("AtmelDeviceHandler", "Epp3PrinterSW: "+ FirmwareFilename );
			bGotCodeUpdate = true;
		} else {
			bGotCodeUpdate = false;
		}
		prCounter++;
		Debug::trace("AtmelDeviceHandler", "prCounter: " + Integer::toString( prCounter ) ); 
		return true;
	}
	else
	{
		Debug::trace("AtmelDeviceHandler", "RequestCodeUpdate - software dev is some other device, sending request to VersionManager : " + inttostring(swver.getVersionType()));
		PeripheralRequestCodeUpdate req(usbNodeId, hwver, swver);
		dispatchEvent(&req);
		rc = true;
	}
	// returning false indicates that we should pause while we wait for the VersionManager to check for a firmware update,
	// that we should discontinue processing until the VersionManager sends a response or timeout occurs.
	//return false;
    return rc;
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
#ifdef UDEV_PATCH
            hobDevice = HOBART_XPE_WEIGHER_ID;
#endif
            break;
		case HOBART_XPE_PRINTER_ID:
			//TBD-Alex Zavala 3/5
			hwver.setVersionType(HobVersion::Epp2PrinterHW);
			swver.setVersionType(HobVersion::Epp2PrinterSW);
#ifdef UDEV_PATCH
            hobDevice = HOBART_XPE_PRINTER_ID;
#endif
            break;
	    case HOBART_FASTRACK_FPC:
	        hwver.setVersionType(HobVersion::FpcPrinterHW);
	        swver.setVersionType(HobVersion::FpcPrinterSW);
	        break;
		case HOBART_XPE_ST_PRINTER_ID:	//SOF-TIME-71
			hwver.setVersionType(HobVersion::Epp3PrinterHW);
			swver.setVersionType(HobVersion::Epp3PrinterSW);
#ifdef UDEV_PATCH
			hobDevice = HOBART_XPE_ST_PRINTER_ID;
#endif			

	}
}


// Handle timeout for all timers that we create.
// This overrides method declared in CHobartTimer:ITimerHandler.
void AtmelDeviceHandler::timeoutProcess( __int32 timer_id )
{
	std::lock_guard<std::recursive_mutex> g( mutex );

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
#ifdef UDEV_PATCH
    if(timer_id == this->PatchTimerId)
    {
        //udev failed to respond to the launch of the firmware, so attach the device ourselves
        Debug::trace("AtmelDeviceHandler","Udev patch timer expired! Calling scan for hobart devices.");
        scanForHobartDevices();
        ContinueProcessing();
    }
#endif
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
		pmsg->bus = bus;
		pmsg->address = addr;
		send((const char *)pmsg, (uint32_t) sizeof(PrivateMsg_t));
	}
	else
	{
		Debug::trace("AtmelDeviceHandler", HobString("SendPrivateMessage: failed to create message"));
	}
}

void AtmelDeviceHandler::ReportStatusToVersionManager(VersionManagerDevError code, bool removingDialog)
{
	// if not removingDialog, then displaying it; default is false, so displaying it
	VmStatusMsg status;
	status.setStatusCode(code);
	status.setResponseType(HfcResponse::RESPONSESTYLE_OK);
	status.setRemoveDialog(removingDialog);
	dispatchEvent(&status);
}

AtmelDeviceHandler::AT91_DEVICE_HANDLE AtmelDeviceHandler::OpenDevice(USBD_NODE_ID nodeId)
{
	int rc;
	AT91_DEVICE_HANDLE phandle( NULL );

	Debug::trace("AtmelDeviceHandler", HobString("OpenDevice: node ") + Hex::toString(nodeId));

	if ( NULL == (pMyDescriptors = UsbManager::getInstance()->getUsbDescs( nodeId )) )
		Debug::trace("AtmelDeviceHandler", HobString("OpenDevice: pMyDescriptors is NULL!!!") );

	else
	{
		Debug::trace("AtmelDeviceHandler", HobString("OpenDevice: opening..."));

		// get handle to the first samba device in the list!
		if ( 0 != (rc = libusb_open(pMyDescriptors->dev, &phandle)) )
			Debug::trace("AtmelDeviceHandler", HobString("OpenDevice: Failed ") + Integer::toString(rc) );

		else
		{
			Debug::trace("AtmelDeviceHandler", HobString("Device Open"));

			if ( libusb_kernel_driver_active(phandle, 1) )
			{
				Debug::trace("AtmelDeviceHandler", HobString("Kernel has claimed interface... detach..."));

				if ( 0 != (rc = libusb_detach_kernel_driver(phandle, 1)) )
					Debug::trace("AtmelDeviceHandler", HobString("Failed to detach kernel driver ") + Integer::toString(rc));
			}

			Debug::trace("AtmelDeviceHandler", HobString("Configuring and claiming device..."));

			if ( 0 != (rc = libusb_set_configuration(phandle, 1)) )
				Debug::trace("AtmelDeviceHandler", HobString("Failed to set configuration ") + Integer::toString(rc));
			else if ( 0 != (rc = libusb_claim_interface(phandle, 1)) )
				Debug::trace("AtmelDeviceHandler", HobString("Failed to claimed interface for NodeId:") + Integer::toString(rc));
			else
			{
				Debug::trace("AtmelDeviceHandler", HobString("Successfully claimed interface!"));
			}
		}
	}

	Debug::trace("AtmelDeviceHandler", HobString("OpenDevice: done ") + Hex::toString((unsigned long)phandle));
	return phandle;
}

void AtmelDeviceHandler::CloseDevice(AT91_DEVICE_HANDLE fd)
{
	Debug::trace("AtmelDeviceHandler", HobString("CloseDevice: ") );

	//let the kernel reclaim the interface
	libusb_attach_kernel_driver(fd, 1);

	libusb_release_interface(fd, 1);

	libusb_close(fd);
}


const char* AtmelDeviceHandler::GetProcStateName(ProcState_t s)
{
	switch (s)
	{
		case S_IDLE:	return "S_IDLE"   ;
		case S_SCAN:	return "S_SCAN"   ;
		case S_IDENT:   return "S_IDENT"  ;
		case S_WAITVER: return "S_WAITVER";
		case S_UPDATE:  return "S_UPDATE" ;
		case S_DETACH:  return "S_DETACH" ;
#ifdef UDEV_PATCH
        case S_UDEV_PATCH:  return "S_UDEV_PATCH";
#endif
	}
	return "Unknown";
}

void AtmelDeviceHandler::handleResetFPCComponent()
{

	// SOF-2931: Do not reset the device if it has already present and has been detected
	if (!UsbManager::getInstance()->isSambaPresent())
	{
		Debug::trace("AtmelDeviceHandler", HobString("handleResetFPCComponent(): isSambaPresent=FALSE"));

		// From Aaron: I have mapped the FPC device reset i/o to:  /usr/local/gpio/hub1_enable/value 
		// You can use a system call to access this pin and you want to set the pin low with the following:
		// system(/bin/echo 0 >> /usr/local/gpio/hub1_enable/value);
		// sleep(100)		  #min of 100mS
		// system(/bin/echo 1 >> /usr/local/gpio/hub1_enable/value);

		ReportStatusToVersionManager(VersionManagerUsbDeviceResetting);
#if 0
		string command = "/bin/echo 0 >> /usr/local/gpio/hub1_enable/value";
		Debug::trace("AtmelDeviceHandler.Reset", "before setting gpio pin low to put FPC into reset, command: " + command);
		string connectionStatusOutput;
		PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
		// We don't care about any output from the command.

		PAL_absclass::PAL_sleep(100);

		command = "/bin/echo 1 >> /usr/local/gpio/hub1_enable/value";
		Debug::trace("AtmelDeviceHandler", "before setting gpio pin high to bring FPC out of reset, command: " + command);
		PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
		// We don't care about any output from the command.
#endif	
	}
	else
	{
		Debug::trace("AtmelDeviceHandler", HobString("handleResetFPCComponent(): isSambaPresent=TRUE"));
	}
}

void AtmelDeviceHandler::detachDevice(USBD_NODE_ID nodeId)
{
	if ( !usbNodeId )
		return;

    Debug::trace("AtmelDeviceHandler", "detachDevice(): Entry");
    UsbManager::getInstance()->DetachDevice(nodeId);
    processingDevice = false;
    usbNodeId = 0;
    Debug::trace("AtmelDeviceHandler", "detachDevice(): Exit");
}

#ifdef UDEV_PATCH
// This function looks for the functions that failed to receive an 'add' event from udev
void AtmelDeviceHandler::scanForHobartDevices()
{
	struct udev *udev;
	struct udev_device *dev;

	try
	{
		// next look for usb devices with Atmel VIDs and PIDs
		struct udev_enumerate *enumerate;
		struct udev_list_entry *devices, *dev_list_entry;

		/* Create a list of the devices in the 'usb' subsystem. */
		udev = udev_new();
		enumerate = udev_enumerate_new(udev);
		udev_enumerate_add_match_subsystem(enumerate, "usb");
		udev_enumerate_scan_devices(enumerate);
		devices = udev_enumerate_get_list_entry(enumerate);
		/* For each item enumerated, print out its information.
		 * udev_list_entry_foreach is a macro which expands to
		 * a loop. The loop will be executed for each member in
		 * devices, setting dev_list_entry to a list entry
		 * which contains the device's path in /sys. */

		Debug::trace("ScaleService", ("ScanForHobartDevices()  Enumerating attached USB devices"));

		udev_list_entry_foreach(dev_list_entry, devices) 
		{
			const char *path;

			/* Get the filename of the /sys entry for the device
			 * and create a udev_device object (dev) representing it */
			path = udev_list_entry_get_name(dev_list_entry);
			dev = udev_device_new_from_syspath(udev, path);

			Debug::trace("AtmelDeviceHandler", HobString("ScanForHobartDevices() device path: " + string(path)));

			/* The device pointed to by dev contains information about
			 * the usb device. In order to get information about the
			 * USB device, get the parent device with the
			 * subsystem/devtype pair of "usb"/"usb_device". This will
			 * be several levels up the tree, but the function will find
			 * it.*/
			struct udev_device *parent = udev_device_get_parent_with_subsystem_devtype(
					dev,
					"usb",
					"usb_device");
			if (parent) 
			{
				int vid = 0, pid = 0;
				const char *cvid = udev_device_get_sysattr_value(parent, "idVendor");
				const char *cpid = udev_device_get_sysattr_value(parent, "idProduct");
				if (cvid && cpid)
				{
					vid = strtol(cvid, NULL, 16);
					pid = strtol(cpid, NULL, 16);
				}
				// look for Hobart devices
				if ((vid == HOBART_VENDOR_ID) && (pid == hobDevice) )
				{
					/* usb_device_get_devnode() returns the path to the device node
					 * itself in /dev. */
//					const char *s = udev_device_get_devnode(dev);
					const char *s = udev_device_get_devnode( parent );

					if ( !s )
						Debug::trace("AtmelDeviceHandler", HobString("ScanForHobartDevices() Bad devnode!" ));
					else
					{
						string devnode_name( s );
						Debug::trace( "AtmelDeviceHandler", HobString( "ScanForHobartDevices() Hobart device on " + devnode_name ) );

						//send a device attached message to the UsbManager!
						EVENT_INFO ev_info;
                    	ev_info.event_type = HOB_DEVICE_ATTACHED;
                    	ev_info.dev_path = udev_device_get_devnode( parent );

                        Debug::trace( "AtmelDeviceHandler", HobString( "ScanForHobartDevices() Finished setting event_info for the device, calling HandleDeviceNotification!"));
						UsbManager::getInstance()->HandleDeviceNotification(&ev_info);
					}
				}
                else
                {
                    Debug::trace("AtmelDeviceHandler", "ScanForHobartDevices() pid:" + Integer::toString(pid) +
                    " does not match hobDevice:" + Integer::toString(hobDevice) );
                }
			}
			udev_device_unref(dev);
		}
		/* Free the enumerator object */
		udev_enumerate_unref(enumerate);
	}
	catch (...)
	{
		Debug::trace("AtmelDeviceHandler", HobString("ScanForHobartDevices()  Failed, error 0x") + Hex::toString(PAL_absclass::PAL_getLastError()) );
	}
    Debug::trace("AtmelDeviceHandler", HobString("ScanForHobartDevices() Setting patchComplete to true") );
    patchComplete = true;
}
#endif

/*

Requirements

3.1. Normal Operation

3.1.1. No Code Update Needed

3.1.1.1. SAM Boot Code Execution
Boot Code enumerates with Atmel Vendor and Product ID.
The System Controller issues a Version command. The Boot Code responds with the current SAM-BA version.
In the unlikely event that there is a Boot Code version change, the System Controller can adjust its operation to accommodate the change.
System Controller issues Jump to Application command.
System Controller sets a timer event for 3-4 seconds. The peripheral controller should enumerate prior to this timer expiring.

 3.1.1.2. Peripheral Code Execution
The Peripheral Controller Code will assume full control of the processor, with the exception of the first two sectors of Flash ROM where the SAM-BA program resides and the first 0x100 of the third sector (where the USB Product ID is stored). It will assume control of all RAM and all peripherals, as if the SAM-BA code had never executed. The list of tasks the Peripheral Controller Code will perform (in rough order) is listed below:
Disable all interrupts (just in case the Boot Code had enabled them).
Detach from USB (it is not prepared to respond to any USB requests).
The System Controller must  not talk to Peripheral Controller via USB after issuing the Jump to Application command. If it does this before the application has had the chance to disable interrupts and detach from USB, a SAM-BA ISR may execute, leading to unknown results.
Initialize variables (part of run-time libraries  happens before main()).
Initialize all peripheral interrupt service routines by writing the ISR vectors for all thirty two AIC interrupt sources in the corresponding Source Mode Register (see Table 24-2).
Initialize the ARM Core Interrupt Vectors by remapping RAM to addresses 0x0 to 0x20 and writing the associated ISR vectors in those locations (see section 19.3.3 Remap Command).
Calculate a checksum on the Peripheral Controller Code and verify this checksum against the stored checksum.
Read the serial EEP and verify its stored checksum matches its calculated checksum.
Complete all other hardware and software initialization.
Enable interrupts.
Attach to USB to initiate enumeration.
Respond to System Controllers enumeration requests. Enumerate as LXScale Peripheral Controller by using the USB Product ID for the LXScale Peripheral Controller.
After enumeration, send a System Status message (requested by the System Controller). This message contains the Peripheral Controller Software Revision, and the Stored and Calculated Checksums for the code.

3.1.1.3. System Controller Action
The System Controller Performs the following checks to verify the peripheral code doesnt need to be updated.
Verifies the peripherals calculated checksum matches its stored checksum.
Verifies the code  is the correct version.
Verifies the code is appropriate for the hardware revision.
If any of the above conditions arent meant, the execution proceeds as described in the Code Updated Needed
If all of the above conditions are met, execution continues as normal (weigher is calibrated, printer teach tables downloaded, etc).

3.1.2. Code Update Needed
The System Controller resets the Peripheral Controller by turning off USB5V power, waits 10mS, and then turns it back on.
The System Controller can also reset the Peripheral Controller by sending a Reset Peripheral message over USB, IF the peripheral code is responding. If its not, USB5V power must be cycled.
The System Controller sets an Attachment timer. The Peripheral Controller must attach to USB prior to this timer expiring.
The System Controller waits till the peripheral controller attaches then proceeds with enumeration.
The Boot Code enumerates with the Atmel Vendor and Product ID.
The System Controller reads the USB Product ID from the first page of the third Flash sector. It reads the board revision from its own serial EEP.  It uses this information to select the code to download.
The System Controller then downloads the code with the Send a File command.
After the code has been downloaded the System Controller issues Jump to Application command and sets a timer event for 3-4 seconds. The peripheral controller should enumerate prior to this timer expiring..
Execution continues as described in the Peripheral Code Execution section.


3.2. Error Handling
3.2.1. No Response from Peripheral out of reset
Setup (the first time) or increment (subsequent times) a counter that counts the number of consecutive times this error occurs.
If the peripheral doesnt enumerate as a Boot Loader within a few seconds, the System Controller should reset the peripheral controller
(by turning off USB5V power) and wait for the peripheral to enumerate.
If this error has occurred more than three times, alert the supervisor of the failure and stop resetting the peripheral controller.

11/30/06 A simple hardware change could be made to allow the System Controller to restore the Boot Code from ROM to Flash in the SAM7S.
This option will be considered before the next board revision.

3.2.2. Peripheral Code Does Not Enumerate after the Go command.
Setup (the first time) or increment (subsequent times) a counter that counts the number of consecutive times this error occurs.
Assume there is a peripheral code software problem. Reset the system controller by turning USB5V power off for 10mS.
Then  proceed from   the Code Update Needed section of this document.
If this error has occurred more than three times, alert the supervisor of the failure and stop resetting the
peripheral controller. Alternately, try loading the previous version of Peripheral Code.

3.2.3. Peripheral Controller Stored Checksum doesnt match calculated checksum.
Setup (the first time) or increment (subsequent times) a counter that counts the number of consecutive times this error occurs.
Try downloading the code again. Go to the Code Update Needed section of this document.
If this error has occurred more than three times, alert the supervisor of the failure and stop resetting the peripheral controller.

3.2.4. USB Product ID or Hardware Version lost or corrupt
In LXScale, download the current version of printer code and set alert that will notify the next person to enter Service Mode.
Create a Service Utility that allows the service person to manually enter the HW revision and the USB Product ID (from a menu of legitimate choices).


*/
