#define MAX_PAGE_SIZE	64
#include "PAL_platformheader.h"
//#include <winsock2.h>	  /* for htons, ntohs, etc. */ // replaced by PAL header

//Alex Zavala's includes 8/20
#include "CompactLabelApplierInquire.h"
#include "CompactLabelApplierConfiguration.h"
#include "CompactLabelApplierSensors.h"
#include "CLASystemInfo.h"
#include "CLASystemStatus.h"//Alex Zavala 9/5
#include "CLASystemInfoWrite.h"
#include "CLATestComplete.h"
#include "CLAPackageDischarge.h"//Alex Zavala 1/3/14
#include "CLACommManager.h"

#include "RealTimeController.h"
#include "UsbManager.h"
//#include "WrapperManager.h"
#include "LabelApplierManager.h"
#include "USBEvolHeader.h"

#include "WeigherSystemConfiguration.h"
#include "WeigherSystemInquire.h"
#include "ILACycle.h"

#include "SystemStatusInfo.h"

#include "LabelApplierWakeup.h"
#include "LabelApplierStatus.h"
#include "LabelApplierInquire.h"

#include "LabelApplierMode.h"
#include "LabelApplierTest.h"
#include "LabelApplierLabelReady.h"
#include "LabelApplierLabelTaken.h"
#include "LabelApplierSetup.h"
#include "LabelApplierOff.h"
#include "LabelApplierPrint.h"
#include "LabelApplierRequestCloseWindow.h"
#include "LabelApplierRequestWakeup.h"
#include "LabelApplierOpenWindow.h"
#include "LabelApplierCloseWindow.h"
#include "LabelApplierPackageReady.h"
#include "LabelApplierUltimaAutoMode.h"

#include "LabelApplierSystemWakeup.h"
//#include "LabelApplierSystemStatus.h"	//Alex Zavala 8/20
#include "LabelApplierSystemConfiguration.h"
#include "LabelApplierSystemInquire.h"

#include "HobDateTime.h"
#include "UsbAddComponent.h"
#include "UsbRemoveComponent.h"

//#include "UsbWrapperCodeUpdater.h"//Removed 9/23 Alex Zavala
//#include "CUEvolWrapperStatus.h"
#include "UsbCLACodeUpdater.h"//Added 9/23 Alex Zavala
#include "CUCLAStatus.h"
#include "UpgradeTransferStatus.h"
#include "UpgradePeripheral.h"
#if defined ( __linux__ )
#include "filesys/file.h"
#include "filesys/directory.h"
#else
#include "filesys\file.h"
#include "filesys\directory.h"
#endif	  // __linux__

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "Long.h"
#include "HobString.h"

//static /*LOCAL*/ bool gPhysicalAddress = false;
#define SWAP_BYTES(x)		(MSB(x) | LSB(x) << 8)  
#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CLACommManager::_dataType_ = 0;
std::string CLACommManager::_dataTypeName_("CLACommManager"); 
#endif


CLACommManager::CLACommManager(USBD_NODE_ID node_id, SUsbDescriptors * descs ): CUsbCommBase( descs, node_id ), nodeId( node_id )
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("CLACommManager", "CLACommManager");   

	this->nodeId = nodeId;

	pMyLabelApplier = NULL;
	pCodeUpdater = new UsbCLACodeUpdater(node_id, 0, this);
	//pCodeUpdater = NULL;

	codeUpdaterStateMachine_ = WAITING_TO_SEND_UPDATE;
	lastSRecord_ = false;
	attemptToSendUpdateCntr_ = 0;
	retryCount = 4;
	sizeModeSRecord_ = false;
	pIntData_ = NULL;
	intDataLength_ = 0;
	xmitting_ = false;
	flashImgBuffer_ = 0;
	EWSWrapperType_ = false;	
	resetTransferCounters();

	/*intialize the lists*/
	processingList_.clear();
	pendingList_.clear();
	successfullList_.clear();
	failedList_.clear();
	popedDialogs_.clear();

	sleepTime = 10000;  //inital run thread sleep time
	//create run thread for code updates, but do not start till needed! -- ats
	pClaCommManagerThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "CLACommManager Thread", false, false, false );

	//PAL_absclass::PAL_sleep( 10000 );
    Debug::trace("CLACommManager", "Create CLA Pipes:");
	createPipes();
	startPipes();

#if defined ( __linux__ )
    Debug::trace("CLACommManager", HobString("PipeHandle =") + Hex::toString(getMyPipeHandle()));
#endif

	claWakeUp();
}

CLACommManager::~CLACommManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	Debug::trace("CLACommManager", "~CLACommManager(): Entry");  


	UsbRemoveComponent removeMe(nodeId, this);
	dispatchEvent(&removeMe);

	deregisterAllEvents();
	Debug::trace("CLACommManager", "~CLACommManager():  deregisterAllEvents()");  

	//shutdown the msg thread
	ShutdownNicely();
	Debug::trace("CLACommManager", "~CLACommManager():  ShutdownNicely()");  

	//tell the thread to exit
	m_ExitEvent.EventSet();
	Debug::trace("CLACommManager", "~CLACommManager():  m_ExitEvent.EventSet()");  

	//wait for the thread to exit before deleting all the resources!
	if ( pClaCommManagerThread )
	{
		while ( pClaCommManagerThread->IsRunning() )
		{
			PAL_absclass::PAL_sleep(1);
		}

		pClaCommManagerThread = NULL;
	}
	Debug::trace("CLACommManager", "~CLACommManager():  pClaCommManagerThread = NULL");  

	if(pMyLabelApplier)
	{
		delete pMyLabelApplier;
		pMyLabelApplier = NULL;
	}
	Debug::trace("CLACommManager", "~CLACommManager():  pMyLabelApplier = NULL");  

	if(pCodeUpdater)
	{
		delete pCodeUpdater;
		pCodeUpdater = NULL;
	}
	Debug::trace("CLACommManager", "~CLACommManager():  pCodeUpdater = NULL");  

	if (flashImgBuffer_ != 0)
	{
		delete[] flashImgBuffer_;
		flashImgBuffer_ = NULL;
	}
	Debug::trace("CLACommManager", "~CLACommManager():  flashImgBuffer_ = NULL");  

	/*cleanup lists*/
	processingList_.clear();
	pendingList_.clear();
	successfullList_.clear();
	failedList_.clear();
	popedDialogs_.clear();

	Debug::trace("CLACommManager", "~CLACommManager(): Exit");
}


void CLACommManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	try
	{
		//Util::CLockCriticalSection lock( EvolWrapperCommManagerCS );
		Util::CLockCriticalSection lock( CLACommManagerCS );

		switch (msg->getDeviceType())
		{
			case LabelApplier:			handleLabelApplierEvent((LaEvent) msg->getEventType(), msg->Data());
										break;
			case LabelApplierSystem:	handleLabelApplierSysEvent((SysEvent) msg->getEventType(), msg->Data());
										break;
			default:					Debug::trace("CLACommManager.Error", "***** Unknown message, device type " +
														 inttostring(msg->getDeviceType()) + " event number " +
														 inttostring(msg->getEventType()));
										break;
		}
	}
	catch (...)
	{
		Debug::trace("CLACommManager.Message.Out", "CLACommManager::processEvent. caught exception");
	}
}

USBD_NODE_ID CLACommManager::getNodeID()
{
	return nodeId;
}

void CLACommManager::CLASystemReset()
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	CompactLaGeneric genMsg;
	memset((char *) &genMsg, 0, sizeof(genMsg));

	genMsg.function_code = LaReqSystemReset;
	usbEvolHdr.setSourcePhysicalAddress(0x0000);
	usbEvolHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
	usbEvolHdr.setDestinationPhysicalAddress(0x8888);
	usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
	usbEvolHdr.setMessageLength(sizeof(genMsg));
	usbEvolHdr.Serialize( str );

	memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
	sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

	if ( result != OK ) //error
	{
		Debug::trace("CLACommManager.Error", "Failed to hard reset the CLA!");
		printUSBError( result, is_custom_code );
	}
}

void CLACommManager::claWakeUp()
{
	Debug::trace("CLACommManager", "claWakeUp()");
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	CompactLaGeneric genMsg;
	memset((char *) &genMsg, 0, sizeof(genMsg));

	genMsg.function_code = LaWakeup;
	usbEvolHdr.setSourcePhysicalAddress(0x0000);
	usbEvolHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
	usbEvolHdr.setDestinationPhysicalAddress(0x8888);
	usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
	usbEvolHdr.setMessageLength(sizeof(genMsg));
	usbEvolHdr.Serialize( str );

	memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
	sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );

	if ( result != OK ) //error
	{
		Debug::trace("CLACommManager.Error", "Failed to send Request wakeup!");
		printUSBError( result, is_custom_code );
	}
}


bool CLACommManager::handleLabelApplierEvent(LaEvent laEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (laEventType)
	{
		case LaConfig:
		{
			Debug::trace("CLACommManager.Message.Out", "LaConfig");	
			CompactLabelApplierConfiguration laConfig(buffer);

			CompactLaConfig compactLaConfig;

			compactLaConfig.function_code = LaConfig;
			compactLaConfig.configType = (UINT8) laConfig.getConfigType();

			compactLaConfig.config.has_weigher = laConfig.isWeigherStationAttached();
			compactLaConfig.config.has_primary_printer = laConfig.isPrimaryStationAttached();
			compactLaConfig.config.has_secondary_printer = laConfig.isSecondaryStationAttached();
			compactLaConfig.config.left_to_right = laConfig.isLeftToRight();
			compactLaConfig.config.front_discharge = laConfig.isFrontDischarge();
			compactLaConfig.config.primary_printer_label_offset = laConfig.getPrimaryPrinterLabelOffset();
			compactLaConfig.config.secondary_printer_label_offset = laConfig.getSecondaryPrinterLabelOffset();
			compactLaConfig.config.label_placement = laConfig.getLabelPlacement();
			compactLaConfig.config.overdrive = laConfig.getOverdrive();//Alex Zavala 11/7 new config parameter

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(compactLaConfig));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &compactLaConfig, sizeof(compactLaConfig));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(compactLaConfig) + CLA_SIZE, result, is_custom_code );

//			sendPeripheralMessage = true;
		}
		break;

		case LaReqSystemReset:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReqSystemReset");	
			CLASystemReset();
			break;
		}
		
		case LaSysInfoWrite:
		{
			Debug::trace("CLACommManager.Message.Out", "LaSysInfoWrite");
			CLASystemInfoWrite sysInfoWriteMsg(buffer);
			CompactLaSysInfoWrite sysInfoWrite;

			sysInfoWrite.function_code = LaSysInfoWrite;

			//Assign the new values
			std::string awsSerial = sysInfoWriteMsg.getAwsSerialNumber();
			std::string awsDateOfInstall = sysInfoWriteMsg.getAwsDateOfInstall();
			std::string awsDateOfManufacture = sysInfoWriteMsg.getAwsDateOfManufacture();
			for(unsigned int i = 0; i < AWS_SERIAL_NUMBER_SIZE; i++)
			{
				sysInfoWrite.awsSerialNumber[i] = awsSerial[i];
			}
			for(unsigned int i = 0; i < DATE_SIZE; i++)
			{
				sysInfoWrite.dateOfInstall[i] = awsDateOfInstall[i];
				sysInfoWrite.dateOfManufacture[i] = awsDateOfManufacture[i];
			}

			sysInfoWrite.awsModelNumber = (UINT16)sysInfoWriteMsg.getModelNumber();
			sysInfoWrite.awsSectionChecksum = sysInfoWriteMsg.getAwsSectionChecksum();
			sysInfoWrite.cycleCount = sysInfoWriteMsg.getCycleCount();

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(sysInfoWrite));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &sysInfoWrite, sizeof(sysInfoWrite));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysInfoWrite) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaSysInfoErase:
		{
			Debug::trace("CLACommManager.Message.Out", "LaSysInfoErase");
			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaSysInfoErase;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaMode:
		{
			Debug::trace("CLACommManager.Message.Out", "LaMode");

			// Constuct the Evol Message Buffer!!
			LabelApplierMode laMode(buffer);
			CompactLaMode claMsg;

			claMsg.function_code = LaMode;
			claMsg.mode = (UINT8)laMode.getMode();

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(claMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &claMsg, sizeof(claMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(claMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaPackageReady:
		{
			Debug::trace("CLACommManager.Message.Out", "LaPackageReady");	

			// Constuct the Evol Message Buffer!!
			LabelApplierPackageReady laPackageReady(buffer);
			CompactLaPackageReady claMsg;

			claMsg.function_code = LaPackageReady;
			claMsg.package_id = laPackageReady.getPackageId();

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(claMsg));
			usbEvolHdr.Serialize( str );
			
			memcpy( &str[CLA_SIZE], (char *) &claMsg, sizeof(claMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(claMsg) + CLA_SIZE, result, is_custom_code );

//			sendPeripheralMessage = true;
		}
		break;

		case LaLabelReady:
		{
			Debug::trace("CLACommManager.Message.Out", "LaLabelReady");

			// Constuct the Evol Message Buffer!!
			LabelApplierLabelReady laLabelReady(buffer);

			CompactLaLabelReady claMsg;
			memset((char *) &claMsg, 0, sizeof(claMsg));

			claMsg.function_code = LaLabelReady;
			claMsg.station = (UINT8) laLabelReady.getStationId();
			claMsg.package_id = laLabelReady.getPackageId();

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(claMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &claMsg, sizeof(claMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(claMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaLabelTaken:
		{
			Debug::trace("CLACommManager.Message.Out", "LaLabelTaken");

			// Constuct the Evol Message Buffer!!
			LabelApplierLabelTaken laLabelTaken(buffer);

			CompactLaLabelTaken claMsg;
			memset((char *) &claMsg, 0, sizeof(claMsg));

			claMsg.function_code = LaLabelTaken;
			claMsg.station = (UINT8) laLabelTaken.getStationId();
			claMsg.package_id = laLabelTaken.getPackageId();

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(claMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &claMsg, sizeof(claMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(claMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaSetup:
		{
			Debug::trace("CLACommManager.Message.Out", "LaSetup");

			LabelApplierSetup laSetup(buffer);

			CompactLaSetup claMsg;
			memset((char *) &claMsg, 0, sizeof(claMsg));

			claMsg.function_code = LaSetup;
			claMsg.pri_label_size = laSetup.getPrimaryLabelSize();
			claMsg.pri_label_rotation = (UINT16) laSetup.getPrimaryLabelRotation();
//			claMsg.priLabelLocation = laSetup.getPrimaryLabelPlacement();
			claMsg.sec_label_size = laSetup.getSecondaryLabelSize();
			claMsg.sec_label_rotation = (UINT16) laSetup.getSecondaryLabelRotation();
//			claMsg.secLabelLocation = laSetup.getSecondaryLabelPlacement();

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(claMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &claMsg, sizeof(claMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(claMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaReqInquire:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReqInquire");

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaInquire;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaReqWakeup:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReqWakeup");	

			LabelApplierRequestWakeup laRequestWakeup(buffer);

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaWakeup;

			UINT16 physicalAddress = laRequestWakeup.getPhysicalAddress();

//			if (evolMgrMap.find(physicalAddress) == evolMgrMap.end())
//			{
				usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
//			}

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );

//			sendPeripheralMessage = true;
		}
		break;

		case LaReqOff:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReqOff");

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaOff;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaOn:
		{
			Debug::trace("CLACommManager.Message.Out", "LaOn");

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaOn;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaReqStatus:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReqStatus");

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaStatus;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaTest:
		{
			Debug::trace("CLACommManager.Message.Out", "LaTest");	
			LabelApplierTest laTest(buffer); // use default constructor

			CompactLaTest testMsg;
			memset((char *) &testMsg, 0, sizeof(testMsg));

			testMsg.function_code = LaTest;

			testMsg.station = (UINT8)laTest.getStationId();
			testMsg.test = (UINT8)laTest.getTestId();
			testMsg.result = htons(laTest.getResult());
			testMsg.parm1 = htons(laTest.getParm1());
			testMsg.parm2 = htons(laTest.getParm2());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(testMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &testMsg, sizeof(testMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(testMsg) + CLA_SIZE, result, is_custom_code );

//			sendPeripheralMessage = true;
		}
		break;

		case LaReqCloseWindow:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReqCloseWindow");	

			// Constuct the Evol Message Buffer!!
			LabelApplierRequestCloseWindow requestCloseWindow(buffer);

			CompactLaCloseWindow closeWindowMsg;
			memset((char *) &closeWindowMsg, 0, sizeof(closeWindowMsg));

			closeWindowMsg.function_code = LaCloseWindow;
			closeWindowMsg.package_id = requestCloseWindow.getPackageId();

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(closeWindowMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &closeWindowMsg, sizeof(closeWindowMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(closeWindowMsg) + CLA_SIZE, result, is_custom_code );

//			sendPeripheralMessage = true;
		}
		break;

		//Alex Zavala 9/6
		case LaReqSysInfo:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReqSysInfo");	

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaSysInfo;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaReqSensors:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReqSensors");	

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaSensors;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );

//			sendPeripheralMessage = true;
		}
		break;

		case LaEnable:
		{
			Debug::trace("CLACommManager.Message.Out", "LaEnable");

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaEnable;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaDisable:
		{
			Debug::trace("CLACommManager.Message.Out", "LaDisable");

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaDisable;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case LaReset:
		{
			Debug::trace("CLACommManager.Message.Out", "LaReset");	

			CompactLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaReset;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );

//			sendPeripheralMessage = true;
		}
		break;

		default:
		{
			Debug::trace("CLACommManager.Error", HobString("Unknown LabelApplier message type: 0x") +
												 HobString(Hex::toString(laEventType)));
		}
		break;
	}

	if ( result != OK ) //error
	{
		printUSBError( result, is_custom_code );
	}

	return true;
}

bool CLACommManager::handleLabelApplierSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

	Debug::trace("CLACommManager.SystemMessage.Out", "handleLabelApplierSysEvent");

	switch (sysEventType)
	{
		case SysReqWakeup:
		{
			Debug::trace("CLACommManager.SystemMessage.Out", HobString("SysReqWakeup"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysWakeup;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );
		}
		break;

		case SysConfig:
		{
			Debug::trace("CLACommManager.SystemMessage.Out", HobString("SysConfig"));
			LabelApplierSystemConfiguration laSysConfig(buffer);

			SysConfigMsg sysConfig;

			sysConfig.function_code = SysConfig;
			sysConfig.configType = htons((UINT16)laSysConfig.getSysConfigType());
			laSysConfig.getDeviceSignature((UINT8 *) &sysConfig.system_config.device_signature);
			sysConfig.system_config.station_address =  (DeviceAddress) htons(laSysConfig.getStationAddress());
			sysConfig.system_config.controller_address = (DeviceAddress) htons(laSysConfig.getControllerAddress());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setMessageLength(sizeof(sysConfig));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &sysConfig, sizeof(sysConfig));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysConfig) + CLA_SIZE, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReqStatus:
		{
			Debug::trace("CLACommManager.SystemMessage.Out", HobString("SysReqStatus"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = LaSysStatus;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);
			usbEvolHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReqInquire:
		{
			Debug::trace("CLACommManager.SystemMessage.Out", HobString("SysReqInquire"));
			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysInquire;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		case SysReset:
		{	
			Debug::trace("CLACommManager.SystemMessage.Out", HobString("SysReset"));

			SysGenericMsg genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.function_code = SysReset;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setSourceLogicalAddress(USBEvolHeader::SYSTEM);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + CLA_SIZE, result, is_custom_code );

			sendPeripheralMessage = true;
		}
		break;

		default:
		{
			Debug::trace("CLACommManager.Error", HobString("Some Other System Event: ") +
													  HobString(Integer::toString((UINT16) sysEventType)));
		}
		break;
	}
	return sendPeripheralMessage;
}

void CLACommManager::processUsbMessage( std::string & msg, CUsbPipe::EvolPipeType UNUSED(pipe_type) )
{
	UNREFERENCED_PARAMETER(pipe_type);

	const char * ptr = msg.c_str();

	USBEvolHeader usbEvolHeader(ptr, false);

	UINT16 physicalAddress = usbEvolHeader.getSourcePhysicalAddress();
	UINT16 logicalAddress = usbEvolHeader.getSourceLogicalAddress();
//	UINT16 msgSize = usbEvolHeader.getMessageLength();

	Debug::trace("CLACommManager", HobString("processUsbMessage():  msg = [")  + Hex::toString( ptr, (int) msg.length(), true ) + ")");
	Debug::trace("CLACommManager", HobString("processUsbMessage():  physicalAddress = ")  + Hex::toString( physicalAddress ));
	Debug::trace("CLACommManager", HobString("processUsbMessage():  logicalAddress = ")  + Hex::toString( logicalAddress ));

	ptr+= CLA_SIZE;

	if (logicalAddress == USBEvolHeader::SYSTEM)
	{
		receivedClaSysMessage((SysGenericMsg *) ptr);
	}
	else if (logicalAddress == USBEvolHeader::CLA)
	{
		receivedClaDevMessage((CompactLaGeneric *) ptr, physicalAddress);	//, msgSize);
	}
	else
	{
		ptr-= CLA_SIZE;
		Debug::trace("CLACommManager.Error", HobString("Bad Header!!!:")  + Hex::toString( ptr, CLA_SIZE, true ));
		ptr += CLA_SIZE; //index past the header and print the message code.
		Debug::trace("CLACommManager.Error", HobString("body: ") + Hex::toString( ptr, 2, true ));
	}
}

void CLACommManager::receivedClaSysMessage(SysGenericMsg *genMsg) 
{
	Debug::trace("CLACommManager", std::string("receivedClaSysMessage():  function_code = ") + Integer::toString(genMsg->function_code));
	switch (genMsg->function_code)
	{
		case SysWakeup:
		{	
			//cout << "Ultima CLA -- SysWakeup: " << endl;
			Debug::trace("CLACommManager.SystemMessage.In", HobString("Ultima CLA -- SysWakeup"));
			SysWakeupMsg *sysWakeupMsg = (SysWakeupMsg *) genMsg;

		 	LabelApplierSystemWakeup laswu(this, (SysCfgStatus) sysWakeupMsg->sysCfgStatus);

			const char *pLaswuMsg = laswu.serialize();
			int mLength = laswu.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLaswuMsg, mLength);
		}
		break;

		case SysInquire:
		{	
//			//cout << "Ultima CLA -- SysInquire: " << endl;
			Debug::trace("CLACommManager.SystemMessage.In", HobString("Ultima CLA -- SysInquire"));
			SysInquireMsg *systemInquireMsg = (SysInquireMsg *) genMsg;

		 	WeigherSystemInquire wsi(this,
		 							 (SysConfigType) systemInquireMsg->configType,
		 							 (UINT8 *) &systemInquireMsg->system_config.device_signature,
		 							 (DeviceAddress) systemInquireMsg->system_config.station_address,
		 							 (DeviceAddress) systemInquireMsg->system_config.controller_address);

			const char *pWsiMsg = wsi.serialize();
			int mLength = wsi.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pWsiMsg, mLength);
		}
		break;

		default:
		Debug::trace("CLACommManager.Error", HobString("Ultima CLA -- Received Some Other System Event: ") +
											 HobString(Integer::toString((UINT16) (genMsg->function_code))));
		break;
	}
}

void CLACommManager::receivedClaDevMessage(CompactLaGeneric *genMsg, UINT16 physicalAddress)	//, INT16 msgLen)
{
	Debug::trace("CLACommManager", std::string("receivedClaDevMessage():  function_code = ") + Integer::toString(genMsg->function_code));
	switch (genMsg->function_code)
	{
		case LaWakeup:
		{	
			Debug::trace("CLACommManager.Message.LaWakeup", "LaWakeup");	

			CompactLaWakeup *wakeupMsg = (CompactLaWakeup *) genMsg;

			if (pMyLabelApplier == NULL)
			{
				Debug::trace("CLACommManager.LaWakeup", "USB CLA -- LaWakeup ************************************************");
				pMyLabelApplier = dynamic_cast<LabelApplierManager *> (RealTimeController::getInstance()->NewUSBCompactLabelApplier(nodeId, this, true, physicalAddress));
				usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
			}
			else
			{
				LabelApplierWakeup laWakeup(this, ((wakeupMsg->ready_to_run != 0) ? true : false));

				const char *pLawuMsg = laWakeup.serialize();
				int mLength = laWakeup.getSerializedLength();

				pMyLabelApplier->send(pLawuMsg, mLength);							
			}
		}
		break;

		case LaStatus:
		{	
			Debug::trace("CLACommManager.Message.In", "LaStatus");	
			CompactLaStatus *statusMsg = (CompactLaStatus *) genMsg;

			LabelApplierStatus las(this); // use default constructor
			las.setStatus(statusMsg->status);

			const char *pLasMsg = las.serialize();
			int mLength = las.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLasMsg, mLength);
		}
		break;

		case LaInquire:
		{	
			Debug::trace("CLACommManager.Message.In", "LaInquire");	
			CompactLaInquire *inquireMsg = (CompactLaInquire *) genMsg;

			CompactLabelApplierInquire lai(this); // use default constructor

			lai.setConfigType((LaConfigType) LaPermanent);

			lai.setWeigherStationAttached( ((inquireMsg->config.has_weigher != 0) ? true : false) );
			lai.setPrimaryStationAttached( ((inquireMsg->config.has_primary_printer != 0) ? true : false) );
			lai.setSecondaryStationAttached( ((inquireMsg->config.has_secondary_printer != 0) ? true : false) );
			lai.setLeftToRight( ((inquireMsg->config.left_to_right != 0) ? true : false) );
			lai.setFrontDischarge( ((inquireMsg->config.front_discharge != 0) ? true : false) );

			lai.setPrimaryPrinterLabelOffset(inquireMsg->config.primary_printer_label_offset);
			lai.setSecondaryPrinterLabelOffset(inquireMsg->config.secondary_printer_label_offset);
			lai.setLabelPlacement(inquireMsg->config.label_placement);
			lai.setOverdrive(inquireMsg->config.overdrive);//Alex Zavala 11/7

			const char *pWiMsg = lai.serialize();
			int mLength = lai.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pWiMsg, mLength);
		}
		break;

		case LaSensors:
		{
			Debug::trace("CLACommManager.Message.In", "LaSensors");
			CompactLaSensors *getSensorMsg = (CompactLaSensors *) genMsg;

			//Set up the label applier sensor msg
			CompactLabelApplierSensors clas(this, getSensorMsg->switches_and_sensors.generic_switches);//grab the sensor switches

			//Serialize the msg and send it to the label applier manager
			const char *pLagsMsg = clas.serialize();
			int mLength = clas.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLagsMsg, mLength);
		}
		break;

		case LaSysStatus:
		{
			Debug::trace("CLACommManager.SystemMessage.In", "LaSysStatus");
			CompactLaSysStatus *systemStatusMsg = (CompactLaSysStatus *) genMsg;
			CLASystemStatus *pCLASystemStatus = new CLASystemStatus(this);

			pCLASystemStatus->setConfigurationChecksum(systemStatusMsg->system_information.configuration_checksum);

			pCLASystemStatus->setAppFlashMajor(systemStatusMsg->system_information.nodeVersion.appFlash.major);
			pCLASystemStatus->setAppFlashMinor(systemStatusMsg->system_information.nodeVersion.appFlash.minor);

			pCLASystemStatus->setDataFlashMajor(systemStatusMsg->system_information.nodeVersion.dataFlash.major);
			pCLASystemStatus->setDataFlashMinor(systemStatusMsg->system_information.nodeVersion.dataFlash.minor);

			pCLASystemStatus->setBootFlashMajor(systemStatusMsg->system_information.nodeVersion.bootFlash.major);
			pCLASystemStatus->setBootFlashMinor(systemStatusMsg->system_information.nodeVersion.bootFlash.minor);

			if (pCodeUpdater)
			{
				if (!pCodeUpdater->isUpdaterBusy())				
				{
					Debug::trace("CLACommManager.Message.In", "LaSysStatus - Calling pCodeUpdater->requestUpdate(pCLASystemStatus)");
					pCodeUpdater->requestUpdate(*pCLASystemStatus);
				}
			}

			const char *pLassMsg = pCLASystemStatus->serialize();
			int mLength = pCLASystemStatus->getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLassMsg, mLength);

			//free our memory
			delete pCLASystemStatus;
		}
		break;

		case LaSysInfo:
		{
			Debug::trace("CLACommManager.SystemMessage.In", "LaSysInfo");	
			CompactLaSysInfo *sysInfoMsg = (CompactLaSysInfo *) genMsg;
			CLASystemInfo *pCLASystemInfo = new CLASystemInfo(this);

			/*	EPP93C56Board	*/
			pCLASystemInfo->setPartNumber(HobString((const char *) &sysInfoMsg->eep_information.boardInfo.partNumber[0], PART_NUMBER_SIZE));
			pCLASystemInfo->setBoardRev(HobString((const char *) &sysInfoMsg->eep_information.boardInfo.boardRev[0], REVISION_SIZE));
			pCLASystemInfo->setDateOfManufacture(HobString((const char *) &sysInfoMsg->eep_information.boardInfo.dateOfManufacture[0], DATE_SIZE));
			pCLASystemInfo->setSerialNumber(HobString((const char *) &sysInfoMsg->eep_information.boardInfo.serialNumber[0], SERIAL_NUMBER_SIZE));
			pCLASystemInfo->setBoardSectionChecksum(sysInfoMsg->eep_information.boardInfo.boardSectionChecksum);

			/*	EPP93C56AWS	*/
			pCLASystemInfo->setAwsSerialNumber(HobString((const char *) &sysInfoMsg->eep_information.awsInfo.awsSerialNumber[0], AWS_SERIAL_NUMBER_SIZE));
			pCLASystemInfo->setAwsDateOfManufacture(HobString((const char *) &sysInfoMsg->eep_information.awsInfo.dateOfManufacture[0], DATE_SIZE));
			pCLASystemInfo->setAwsDateOfInstall(HobString((const char *) &sysInfoMsg->eep_information.awsInfo.dateOfInstall[0], DATE_SIZE));
			pCLASystemInfo->setModelNumber(sysInfoMsg->eep_information.awsInfo.awsModelNumber);
			pCLASystemInfo->setCycleCount(sysInfoMsg->eep_information.awsInfo.cycleCount);
			pCLASystemInfo->setAwsSectionChecksum(sysInfoMsg->eep_information.awsInfo.awsSectionChecksum);
			
			//Check if we need to update
			if(pCodeUpdater)
			{
				if(!pCodeUpdater->isUpdaterBusy())
				{
					Debug::trace("CLACommManager.Message.In", "LaSysInfo  -  sending update request!!!!");
					pCodeUpdater->sendUpdateRequest();
				}
				else
				{
					Debug::trace("CLACommManager.Message.In", "LaSysInfo  -  Updater is Busy!!!!");
				}
			}
			else
			{
				Debug::trace("CLACommManager.Message", "LaSysInfo is NULL");
			}
			
			const char *pLassMsg = pCLASystemInfo->serialize();
			int mLength = pCLASystemInfo->getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLassMsg, mLength);

			//free our memory
			delete pCLASystemInfo;
		}
		break;

		case LaTestComplete:
		{
			Debug::trace("CLACommManager.Message.In", "LaTestComplete");
			CompactLaTestComplete *testCompleteMsg = (CompactLaTestComplete *) genMsg;
			CLATestComplete *pCLATestComplete = new CLATestComplete(this);

			//Set the parameters for the message
			pCLATestComplete->setTest(testCompleteMsg->test);
			pCLATestComplete->setData1(testCompleteMsg->parameter.data_short.data1);
			pCLATestComplete->setData2(testCompleteMsg->parameter.data_short.data2);
			pCLATestComplete->setResult(testCompleteMsg->result);

			const char *pLassMsg = pCLATestComplete->serialize();
			int mLength = pCLATestComplete->getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLassMsg, mLength);

			//free our memory
			delete pCLATestComplete;
		}
		break;

		case LaLoaderStatus:
		{
			HobString msgDebug;

			Debug::trace("CLACommManager.burn", msgDebug + "LaLoaderStatus");

			CompactLaLoaderStatus *pMsg = (CompactLaLoaderStatus *) genMsg;
			UINT16 nodeId = pMsg->nodeId;
			LaSlaveLoaderState status = (LaSlaveLoaderState)pMsg->status;
			UINT32 addr = ntohl(pMsg->addr);
			LaSlaveLoaderFaults fault = (LaSlaveLoaderFaults)pMsg->fault;
			UINT16 updateFailureCount = pMsg->updateFailureCount;
			LaSoftwareComponent softComponent = (LaSoftwareComponent)pMsg->softComponent;

			if ((bytesSentSoFar == sizeof(CompactLaUpdate)) && (totalBytesToSend == 0))
			{
				bytesSentSoFar = 0;
				Debug::trace("CLACommManager", msgDebug + "Set bytesSentSoFar to 0.");
			}

			Debug::trace("CLACommManager.burn", msgDebug + "nodeId = " + Integer::toString(nodeId) + ",  softComponent = " + Integer::toString(softComponent));
			Debug::trace("CLACommManager.burn", msgDebug + "status = " + Integer::toString(status));
			Debug::trace("CLACommManager.burn", msgDebug + "addr = " + Hex::toString(addr));
			Debug::trace("CLACommManager.burn", msgDebug + "fault = " + Integer::toString(fault));
			Debug::trace("CLACommManager.burn", msgDebug + "updateFailureCount = " + Integer::toString(updateFailureCount));

			switch(status)
			{
				case laSlaveBootInit:
					Debug::trace("CLACommManager", msgDebug + "status = laSlaveBootInit");
					break;

				case laSlaveBootReady:
					{

						Debug::trace("CLACommManager.burn", msgDebug + "status = laSlaveBootReady");

						//crank up our run thread!!!
						sleepTime = 4;	  //only 4mS of sleep time.

						nodeListIterator i;
						HobVersion::VersionType vt = getSoftwareType(nodeId, softComponent);

						Debug::trace("CLACommManager.burn", msgDebug + "vt: " + Integer::toString(vt) + "  pendingList_.size() = " + Integer::toString((int)pendingList_.size()));
						if (pendingList_.size() > 0)
						{
							Debug::trace("CLACommManager.burn", msgDebug + "*****  Showing the pendingList_  *****");
							for (i = pendingList_.begin(); (i != pendingList_.end()); i++)
							{
								HobString vtStr = Integer::toString(i->first);
								util_filesystem::File tmpSrc(i->second);
								Debug::trace("CLACommManager.burn", msgDebug + vtStr + ",  [" + tmpSrc.getFullPath() + "]");
							}
							Debug::trace("CLACommManager.burn", msgDebug + "*****  End pendingList_  *****");
						}

						if (fault == laSlaveNone)
						{
							i = pendingList_.find(vt);
							if (i != pendingList_.end())
							{
								Debug::trace("CLACommManager.burn", msgDebug + "Found the versionType in the pending list. Copy the tar file and extract the source.bin.");

								updateUserInterface(vt, MT_START);

								checkUpgradeProgress(true);		 //	  Update the final count before resetTransferCounters() is called.
								resetTransferCounters();

								switch (nodeId)
								{
									case 1:
										{
											usbEvolHdr.setDestinationLogicalAddress(0x001E);
											break;
										}
									case 2:
										{
											usbEvolHdr.setDestinationLogicalAddress(0x0022);
											break;
										}
									case 3:
										{
											usbEvolHdr.setDestinationLogicalAddress(0x001F);
											break;
										}
									case 4:
										{
											usbEvolHdr.setDestinationLogicalAddress(0x0020);
											break;
										}
									case 5:
										{
											usbEvolHdr.setDestinationLogicalAddress(0x0021);
											break;
										}
									case 6:
										{
											usbEvolHdr.setDestinationLogicalAddress(0x0023);
											break;
										}
									default:
										break;
								}
								Debug::trace("CLACommManager.burn", "added component to process list");

								processingList_.insert(make_pair(i->first, i->second));
								pendingList_.erase(i->first);

								if (processingList_.size() > 0)
								{
									Debug::trace("CLACommManager.burn", msgDebug + "*****  Showing the processingList_  *****");
									for (i = processingList_.begin(); (i != processingList_.end()); i++)
									{
										HobString vtStr = Integer::toString(i->first);
										util_filesystem::File tmpSrc(i->second);
										Debug::trace("CLACommManager.burn", msgDebug + vtStr + ",  [" + tmpSrc.getFullPath() + "]");
									}
									Debug::trace("CLACommManager.burn", msgDebug + "*****  End processingList_  *****");
								}
							}
							else
							{
								
								Debug::trace("CLACommManager.burn", msgDebug + "Did not find the node in the pending list. Don't know what to do here, so we are done. Transition to ALL_DOWNLOADS_COMPLETE.");
								codeUpdaterStateMachine_ = ALL_DOWNLOADS_COMPLETE;
								//  Notify the UI that the leftover nodes will not be upgraded.
								nodeListIterator i;

								for (i = pendingList_.begin(); (i != pendingList_.end()); i++)
								{
									updateUserInterface(i->first, MT_ABORTED);
								}
							}
						}
						else
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("Fault!  codeUpdaterStateMachine_ = ");
							if (codeUpdaterStateMachine_ == DOWNLOADING_CODE)
								msgDebug2.append("DOWNLOADING_CODE");
							else
								msgDebug2.append(Integer::toString(codeUpdaterStateMachine_));
							Debug::trace("CLACommManager.burn", msgDebug2);

							if (codeUpdaterStateMachine_ == DOWNLOADING_CODE)
							{
							
									Debug::trace("CLACommManager.burn", msgDebug + "Current state is Downloading code.");

								i = processingList_.find(vt);

								if (i != processingList_.end())
								{
									updateUserInterface(vt, MT_FAILED);

									Debug::trace("CLACommManager.burn", msgDebug + "Found the node in the processing list.");

									if ((fault != laSlaveCheckSum) && (fault != laSlaveUnknownRecordType))
									{
										Debug::trace("CLACommManager.burn", msgDebug + "Retry the flash.");

										/*leave job in pending list as we will retry it after a delay*/
										codeUpdaterStateMachine_ = PAUSE_BEFORE_RETRY;
									}
									else
									{
										Debug::trace("CLACommManager.burn", msgDebug + "Not retrying the flash because image is corrupted.");

										failedList_.insert(make_pair(i->first, fault));

										util_filesystem::File tmpData(i->second);

										if (tmpData.exists())
										{
											Debug::trace("CLACommManager", msgDebug + "Removing the temporary file on the ramdrive path = " + tmpData.getFullPath());
											tmpData.remove();
										}
										processingList_.erase(i->first);

										codeUpdaterStateMachine_ = DOWNLOAD_COMPLETE;
									}
								}
							}
						}
						break;
					}
				case laSlaveBootFault:
					{
						nodeListIterator i;
						HobVersion::VersionType vt = getSoftwareType(nodeId, softComponent);

						Debug::trace("CLACommManager.burn", msgDebug + "status = laSlaveBootFault");

						/*  It might be possible to get two fault messages, so this allows us to only handle the first one. */
						if (codeUpdaterStateMachine_ == DOWNLOADING_CODE)
						{
							Debug::trace("CLACommManager.burn", msgDebug + "Current state is Downloading code.");

							i = processingList_.find(vt);

							if (i != processingList_.end())
							{
								updateUserInterface(vt, MT_FAILED);

								Debug::trace("CLACommManager.burn", msgDebug + "Found the node in the processing list.");

								if ((fault != laSlaveCheckSum) && (fault != laSlaveUnknownRecordType))
								{
									Debug::trace("CLACommManager.burn", msgDebug + "Retry the flash.");

									/*leave job in pending list as we will retry it after a delay*/
									codeUpdaterStateMachine_ = PAUSE_BEFORE_RETRY;
								}
								else
								{
									Debug::trace("CLACommManager.burn", msgDebug + "Not retrying the flash because image is corrupted.");


									failedList_.insert(make_pair(i->first, fault));

									util_filesystem::File tmpData(i->second);

									if (tmpData.exists())
									{
										Debug::trace("CLACommManager.burn", msgDebug + "Removing the temporary file on the ramdrive path = " + tmpData.getFullPath());
										tmpData.remove();
									}

									processingList_.erase(i->first);

									codeUpdaterStateMachine_ = DOWNLOAD_COMPLETE;
								}
							}
						}
						break;
					}
				case laSlaveBootDone:
					{
						nodeListIterator i;
						HobVersion::VersionType vt = getSoftwareType(nodeId, softComponent);
						util_filesystem::Directory tmpPeripheralDir("");
						HobComplexVersion::getExtractionDir(vt, tmpPeripheralDir, (USBD_NODE_ID) getNodeID());									  

						Debug::trace("CLACommManager.burn", msgDebug + "status = laSlaveBootDone");
						Debug::trace("CLACommManager.burn", msgDebug + "tmpPeripheralDir = [" + tmpPeripheralDir.getFullPath() + "] - We need to delete this directory since we are done.");

						i = processingList_.find(vt);

						if (i != processingList_.end())
						{
							updateUserInterface(vt, MT_SUCCESS);

							Debug::trace("CLACommManager", msgDebug + "Found the node in the processing list.");

							successfullList_.insert(make_pair(i->first, fault));

							util_filesystem::File tmpData(i->second);
							if (tmpData.exists())
							{
								Debug::trace("CLACommManager", msgDebug + "Removing the temporary file on the ramdrive path = " + tmpData.getFullPath());
								tmpData.remove();
							}
							processingList_.erase(i->first);
						}

						bytesSentSoFar = totalBytesToSend;
						checkUpgradeProgress(true);

						codeUpdaterStateMachine_ = DOWNLOAD_COMPLETE;
						break;
					}
				default:
					Debug::trace("CLACommManager", "receivedCLADevMessage:unknown event");
					break;
				}
		}
		break;

		case LaOff:
		{
			Debug::trace("CLACommManager.Message.In", "LaOff");	

			LabelApplierOff lao(this); // use default constructor

			const char *pLaoMsg = lao.serialize();
			int mLength = lao.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLaoMsg, mLength);
		}
		break;

		case LaPrintPackage:
		{
			Debug::trace("CLACommManager.Message.In", "LaPrintPackage");	

			CompactLaPrint *printMsg = (CompactLaPrint *) genMsg;

			LabelApplierPrint lap(this); // use default constructor

			lap.setPackageId(printMsg->package_id);

			const char *pLapMsg = lap.serialize();
			int mLength = lap.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLapMsg, mLength);
		}
		break;

		case LaUltimaAutoMode:
		{
			Debug::trace("CLACommManager.Message.In", "LaUltimaAutoMode");	

			CompactLaUltimaAuto *ultimaAutoMsg = (CompactLaUltimaAuto *) genMsg;

			LabelApplierUltimaAutoMode lauam(this); // use default constructor

			lauam.setPackageId(ultimaAutoMsg->package_id);

			const char *pLauamMsg = lauam.serialize();
			int mLength = lauam.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pLauamMsg, mLength);
		}
		break;

		case LaOpenWindow:
		{
			Debug::trace("CLACommManager.Message.In", "LaOpenWindow");	

			// Constuct the Evol Message Buffer!!
			CompactLaOpenWindow *openWindowMsg = (CompactLaOpenWindow *) genMsg;

			LabelApplierOpenWindow laOpenWindow(this); // use default constructor

			laOpenWindow.setPackageId(openWindowMsg->package_id);

			const char *pOpenWindowMsg = laOpenWindow.serialize();
			int mLength = laOpenWindow.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pOpenWindowMsg, mLength);
		}
		break;

		case LaCloseWindow:
		{
			Debug::trace("CLACommManager.Message.In", "LaCloseWindow");	

			// Constuct the Evol Message Buffer!!
			CompactLaCloseWindow *closeWindowMsg = (CompactLaCloseWindow *) genMsg;

			LabelApplierCloseWindow laCloseWindow(this); // use default constructor

			laCloseWindow.setPackageId(closeWindowMsg->package_id);

			const char *pCloseWindowMsg = laCloseWindow.serialize();
			int mLength = laCloseWindow.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pCloseWindowMsg, mLength);
		}
		break;
		//Alex Zavala 1/3/14
		case LaPackageDischarge:
		{
			Debug::trace("CLACommManager.Message.In", "LaPackageComplete");	

			// Constuct the Evol Message Buffer!!
			CompactLaPackageDischarge *packageDischargeMsg = (CompactLaPackageDischarge *) genMsg;

			CLAPackageDischarge laPackageDischarge(this); // use default constructor

			laPackageDischarge.setPackageId(packageDischargeMsg->package_id);

			const char *pPackageDischargeMsg = laPackageDischarge.serialize();
			int mLength = laPackageDischarge.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pPackageDischargeMsg, mLength);
		}
		break;

		case LaILACycle:
		{
			Debug::trace("CLACommManager.Message.In", "LaILACycle");	

			ILACycle ilaCycle;

			const char *pILACycle = ilaCycle.serialize();
			int mLength = ilaCycle.getSerializedLength();

			if(pMyLabelApplier)
				pMyLabelApplier->send(pILACycle, mLength);
		}
		break;

		default:
			Debug::trace("CLACommManager.Error", HobString("Ultima LabelApplier -- Received Some Other Event: ") +
												 HobString(Integer::toString((UINT16) (genMsg->function_code))));
		break;
	}
}

static const unsigned long maxIntXferSize = 64;
void CLACommManager::setupCodeFlash(nodeList &burnList)
{
	int node2List = 0;

	if ((codeUpdaterStateMachine_ != WAITING_TO_SEND_UPDATE) || xmitting_)
	{
		Debug::trace("CLACommManager.burn", HobString("setupCodeFlash:flashing is currently in progress "));
		return;
	}
	Debug::trace("CLACommManager.burn", HobString("setupCodeFlash:starting burn process list size = ") + Integer::toString((int)burnList.size()));

	nodeListIterator i;

	processingList_.clear();
	if (burnList.size() > 0)
	{
		pendingList_.clear();
		for (i = burnList.begin(); (i != burnList.end()); i++)
		{
			pendingList_.insert(make_pair(i->first, i->second));
			updateUserInterface(i->first, MT_PENDING);
		}
	}

	Debug::trace("CLACommManager.burn", HobString("setupCodeFlash:  pendingList_.size() = ") + Integer::toString((int)pendingList_.size()));
	if (pendingList_.size() > 0)
	{
		for (i = pendingList_.begin(); (i != pendingList_.end()); i++)
		{
			if (i != pendingList_.end())
			{
				Debug::trace("CLACommManager.burn", HobString("setupCodeFlash:found component in pending list type = ") + Integer::toString(i->first));

				util_filesystem::File tmpSrc(i->second);

				if (tmpSrc.exists())
				{
					Debug::trace("CLACommManager.burn", HobString("setupCodeFlash:file exists path = ") + i->second);
					switch (i->first)
					{
						case HobVersion::CLADataSW:
							Debug::trace("CLACommManager.burn", HobString("setupCodeFlash:  CLADataSW"));
							node2List |= UPDATE_DATA_MASK;
							break;
						case HobVersion::CLABootSW:
							Debug::trace("CLACommManager.burn", HobString("setupCodeFlash:  CLABootSW"));
							node2List |= UPDATE_BOOT_MASK;
							break;
						case HobVersion::CLAAppSW:
							Debug::trace("CLACommManager.burn", HobString("setupCodeFlash:  CLAAppSW"));
							node2List |= UPDATE_APP_MASK;
							break;
						default:
							break;
					}
				}
			}
		}

		//CompactLaUpdateCLA outCmd;
		//outCmd.function_code = LaUpdate;

		//outCmd.needUpdate = (node2List > 0);
		//outCmd.update = (UINT16) determineEnumForUpdateMask(node2List);

		//OLD STYLE wrapper function for CLA
		CompactLaUpdate outCmd;
		outCmd.function_code = LaUpdate;
		outCmd.node2 = (node2List > 0);
		outCmd.update2 = (UINT16) determineEnumForUpdateMask(node2List);

		//Empty out the other fields
		outCmd.node1 = 0;
		outCmd.node3 = 0;
		outCmd.node4 = 0;
		outCmd.node5 = 0;
		outCmd.node6 = 0;

		outCmd.update1 = 0;
		outCmd.update3 = 0;
		outCmd.update4 = 0;
		outCmd.update5 = 0;
		outCmd.update6 = 0;

		//Debug::trace("CLACommManager.burn", HobString("outCmd node2 = ") + Integer::toString(outCmd.needUpdate));
		//Debug::trace("CLACommManager.burn", HobString("outCmd node2 update enum = ") + Integer::toString(outCmd.update));
		Debug::trace("CLACommManager.burn", HobString("outCmd node2 = ") + Integer::toString(outCmd.node2));
		Debug::trace("CLACommManager.burn", HobString("outCmd node2 update enum = ") + Integer::toString(outCmd.update2));

		//usbEvolHdr.setSourcePhysicalAddress(0x0000);
		//usbEvolHdr.setSourceLogicalAddress(0x0009);
		//usbEvolHdr.setDestinationPhysicalAddress(0x8181);
		//usbEvolHdr.setDestinationLogicalAddress(0x001D);
		usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::CLA);

		if (flashImgBuffer_ != 0)
		{
			delete[] flashImgBuffer_;
			flashImgBuffer_ = 0;
		}

		intDataLength_ = 0;
		pIntData_ = NULL;

		flashImgBuffer_ =(UINT8 *) new char[sizeof(outCmd)];

		if (flashImgBuffer_)
		{
			memcpy(flashImgBuffer_, (char *) &outCmd, sizeof(outCmd));

			sizeModeSRecord_ = false;
			intDataLength_ = sizeof(outCmd);
			pIntData_ = flashImgBuffer_;

			Debug::trace("CLACommManager.burn", HobString("length of outCmd: ") + Integer::toString(intDataLength_));

			Debug::trace("CLACommManager.burn", HobString("outCmd data: ") +
					HobString(Hex::toString((const char *) (flashImgBuffer_), intDataLength_)));		

			codeUpdaterStateMachine_ = SENDING_UPDATE;
		}
	}  
}

LaComponentUpdate CLACommManager::determineEnumForUpdateMask(int updateMask)
{
	Debug::trace("CLACommManager.burn", "LaSysInfo  -  determingEnumForUpdate");
    if ( (updateMask & UPDATE_DATA_MASK) && (updateMask & UPDATE_BOOT_MASK) && (updateMask & UPDATE_APP_MASK) )
    {
		Debug::trace("CLACommManager.burn", "enum = laAll");
        return laAll;
    }
    else
    {
        if ( (updateMask & UPDATE_DATA_MASK) && (updateMask & UPDATE_BOOT_MASK) )
        {
			Debug::trace("CLACommManager.burn", "enum = laDataBoot");
            return laDataBoot;
        }
        else
        {
            if ( (updateMask & UPDATE_BOOT_MASK) && (updateMask & UPDATE_APP_MASK) )
            {
				Debug::trace("CLACommManager.burn", "enum = laAppBoot");
                return laAppBoot;
            }
            else
            {
                if ( (updateMask & UPDATE_DATA_MASK) && (updateMask & UPDATE_APP_MASK) )
                {
					Debug::trace("CLACommManager.burn", "enum = laAppData");
                    return laAppData;
                }
                else
                {
                    if (updateMask & UPDATE_DATA_MASK)
                    {
						Debug::trace("CLACommManager.burn", "enum = laData");
                        return laData;
                    }
                    else
                    {
                        if (updateMask & UPDATE_APP_MASK)
                        {
							Debug::trace("CLACommManager.burn", "enum = laApp");
                            return laApp;
                        }
                        else
                        {
							Debug::trace("CLACommManager.burn", "enum = laBoot");
                            return laBoot;
                        }
                    }
                }
            }
        }

    }
}

void CLACommManager::sendUpgradeBurnList()
{
	Debug::trace("CLACommManager.Message.In", "Sending upgrade burnlist");
	UINT32 size = 0;
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	xmitting_ = true;

	size = intDataLength_;
	if (maxIntXferSize < size)
		size = maxIntXferSize;

	usbEvolHdr.setMessageLength((UINT16)size);
	usbEvolHdr.Serialize( str );

	memcpy( &str[CLA_SIZE], (char *) &pIntData_[0], size);
	sendUsbInterruptMessage( ( const char * ) str,  size + CLA_SIZE, result, is_custom_code );

	intDataLength_ = 0;
	pIntData_ = NULL;
	xmitting_ = false;
}

void CLACommManager::sendRecord()
{
	UINT32 size = 0;
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	xmitting_ = true;
	lastSRecord_ = false;

	//this is for transmitting firmware in s record format.
	//each packet will contain exactly one s record.
	//thus s records cannot be larger than 64 bytes.
	//the size of the packet is the size of the current s record.
	//the size of the s record is in the s record data.
	if (sizeModeSRecord_)
	{
		if (pIntData_[0] != 'S')
		{
			Debug::trace("CLACommManager.burn", "***In Srecord Mode but not Srecord***");
			Debug::trace("CLACommManager.burn", HobString("Pointer Addr pIntData_:") + Hex::toString((long long)pIntData_ ));
			Debug::trace("CLACommManager.burn", HobString("Bad Pointer pIntData_:") + Hex::toString((const char *)pIntData_, 128, true ));
			pIntData_ = 0;
			intDataLength_ = 0;

			xmitting_ = false;
			return;	 
		}

		//get the size of the record from the record size field.
		//size field accounts for address field and data fields only.
		sscanf((const char *)(pIntData_ + 2), "%02X", &size);		
		size *= 2;	
		#if defined (_WIN32)
		size += 5;	//offset for data not included in the size field. 
					//(record type field (1) + size field (1) + checksum field (1) + (2) cariage return and line feed 
		#else
		size += 6;	//offset for data not included in the size field. 
					//(record type field (1) + size field (1) + checksum field (1) + (2) cariage return and line feed 
		#endif
		//clip the s record to 64 bytes.
		if (size > maxIntXferSize)
			size = maxIntXferSize;

		if ((pIntData_[0] == 'S') && (pIntData_[1] == '7'))
		{
			Debug::trace("CLACommManager.burn", "***S7 found***");
			codeUpdaterStateMachine_ = LAST_SRECORD_SENT;
			lastSRecord_ = true;
		}
	}
	else
	{
		//this should not happen, sending of the				
		size = intDataLength_;
		if (maxIntXferSize < size)
			size = maxIntXferSize;
	}

	usbEvolHdr.setMessageLength((UINT16)size);
	usbEvolHdr.Serialize( str );

	memcpy( &str[CLA_SIZE], (char *) &pIntData_[0], size);
	Debug::trace("CLACommManager.burn", "ABOUT TO SEND SRECORD");
	sendUsbInterruptMessage( ( const char * ) str,  size + CLA_SIZE, result, is_custom_code );
	Debug::trace("CLACommManager.burn", "SENT USB INTERRUPT FOR SRECORD");
	//if this is the last record
	if(codeUpdaterStateMachine_ == LAST_SRECORD_SENT)
	{
		intDataLength_ = 0;
		pIntData_ = NULL;
	}
	else
	{
		//index to the next record.
		pIntData_ += size;

		//keep track of the bytes sent for browser update
		bytesSentSoFar += size;

		//deduct what we have sent!
		intDataLength_ -= size;
	}
	xmitting_ = false;
}

void CLACommManager::sendUpgrade()
{
	char* str  = new char[CLA_SIZE + intDataLength_];
	DWORD result = OK;
	bool is_custom_code = false;
	xmitting_ = true;
	unsigned int size = MAX_PAGE_SIZE;
	int msgSize = 0;
	int idx = 0;//index to current place in the binary img
	char dummyMsg_[] = {'0','0','0','0','0','0'};
	int dummyMsgLength_ = 6;

	//Create header to send to the CLA notify it of the upgrade
	Debug::trace( "CLACommManager.Message.Out", "Sending binary img to the CLA" );

	while( intDataLength_ > 0 )//Break up the binary img into pages that can fit in memory on the CLA
	{
		if( intDataLength_ > size )
		{
			msgSize = size;
		}
		else
		{
			msgSize = intDataLength_;
		}

		intDataLength_ -= msgSize;
		if(intDataLength_ <= 0)
		{
			//When the last page is sent, wait for the CLA to send a response that it was sent
			codeUpdaterStateMachine_ = LAST_SRECORD_SENT;
			lastSRecord_ = true;
		}
		Debug::trace("CLACommManager.Message.Out","Upgrade Page being prepared");
		usbEvolHdr.setMessageLength( (UINT16) msgSize );
		usbEvolHdr.Serialize( str );

		memcpy( &str[CLA_SIZE], (char *) &flashImgBuffer_[idx], msgSize );
		sendUsbInterruptMessage( ( const char * ) str,  msgSize + CLA_SIZE, result, is_custom_code );

		Debug::trace("CLACommManager.Message.Out","Upgrade Page Sent idx:" );
		idx += msgSize;
		//intDataLength_ -= msgSize;
		bytesSentSoFar += msgSize;
		uiUpdateCounter++;

		//only update the ui for every 10,000 bytes transfered or on the last record. 150
		UpgradeTransferStatus statusMsg(bytesSentSoFar, totalBytesToSend);
		dispatchEvent(&statusMsg);
		Debug::trace("CLACommManager.burn", HobString("SM:DOWNLOADING_CODE:send update page msg"));

		//Send a dummy message after the last msg is sent to notify the CLA
		if(lastSRecord_)
		{
			usbEvolHdr.setMessageLength( (UINT16) dummyMsgLength_ );
			usbEvolHdr.Serialize( str );

			memcpy( &str[CLA_SIZE], (char *) &dummyMsg_[0], dummyMsgLength_ );
			sendUsbInterruptMessage( ( const char * ) str,  dummyMsgLength_ + CLA_SIZE, result, is_custom_code );
		}
	}

	Debug::trace("CLACommManager.Message.Out","Last Upgrade Page Sent");
	
	pIntData_ = NULL;
	xmitting_ = false;
	
	/*
	usbEvolHdr.setMessageLength( (UINT16) intDataLength_ );
	usbEvolHdr.Serialize( str );

	memcpy( &str[CLA_SIZE], (char *) &flashImgBuffer_[0], intDataLength_ );
	sendUsbInterruptMessage( ( const char * ) str,  intDataLength_ + CLA_SIZE, result, is_custom_code );
	
	//Wait for the CLA to send a response that the last record was sent
	codeUpdaterStateMachine_ = LAST_SRECORD_SENT;
	lastSRecord_ = true;

	bytesSentSoFar = intDataLength_;
	intDataLength_ = 0;
	pIntData_ = NULL;
	

	xmitting_ = false;
	*/
}

void CLACommManager::receiveErrorCallBack( CUsbPipe::EvolPipeType pipe_type, DWORD error, bool user_defined_error )
{
	printUSBError( error, user_defined_error );

	Debug::trace("CLACommManager.Error", std::string("***receiveErrorCallBack***::error = ") + Integer::toString(error) +
										 std::string("pipe type: ") + Integer::toString(pipe_type));
}

DWORD CLACommManager::Run()
{
	Debug::trace("CLACommManager", HobString("Run()"));
	try
	{
		registerEvent(VersionManagerDev, 18);	//CancelUpgrade

		while (!m_ExitEvent.IsEventSet())
		{
			CLACommManagerCS.Enter();

			switch (codeUpdaterStateMachine_)
			{
				case WAITING_TO_SEND_UPDATE:
				{
					Debug::trace("CLACommManager.burn", HobString("SM:WAITING_TO_SEND_UPDATE:waiting"));
					attemptToSendUpdateCntr_ = 0;
				}
				break;

				case SENDING_UPDATE:
				{
					if ((intDataLength_ > 0) && (!xmitting_))
					{
						Debug::trace("CLACommManager.burn", HobString("SM:SENDING_UPDATE:send out update message"));

						codeUpdaterStateMachine_ = WAITING_FOR_STATUS_START;

						sendUpgradeBurnList();
					}
					else
					{
						attemptToSendUpdateCntr_++;
						Debug::trace("CLACommManager.burn", HobString("SM:SENDING_UPDATE:cannot send update message"));

						if (attemptToSendUpdateCntr_ > 100)
						{
							Debug::trace("CLACommManager", HobString("SM:SENDING_UPDATE:failure count exceeded. cancel sending update message"));
							codeUpdaterStateMachine_ = WAITING_TO_SEND_UPDATE;
						}
					}

				}
				break;

				case WAITING_FOR_STATUS_START:
				{
					/*processingList will be empty until we receive a loader status ready message from the CLA.*/
					if (processingList_.size() > 0)
					{
						Debug::trace("CLACommManager.burn", HobString("SM:WAITING_FOR_STATUS_START:start download"));
						nodeListIterator i;

						i = processingList_.begin();

						if (i != processingList_.end())
						{
							Debug::trace("CLACommManager.burn", HobString("SM:WAITING_FOR_STATUS_START:found the file in processing list"));
							//this will read in the file and send down the first s record.
							downloadCode(i->second.c_str());
							codeUpdaterStateMachine_ = DOWNLOADING_CODE;
						}
					}
				}
				break;

				case DOWNLOADING_CODE:
				{
					//if we still have more s records to send. and we are not currently sending one.
					//then send the next s record.
					//this is two steps. first the header then the s record data.
					if ((intDataLength_ > 0) && (!xmitting_))
					{
						Debug::trace("CLACommManager.burn", HobString("SM:DOWNLOADING_CODE:sending record"));
						sendRecord();
						//sendUpgrade();
						Debug::trace("CLACommManager.burn", HobString("SM:DOWNLOADING_CODE:sent record"));
						if(intDataLength_ > 0)
						{
							sleepTime = 4;
						}

						uiUpdateCounter++;
						//only update the ui for every 10,000 bytes transfered or on the last record. 150
						if((uiUpdateCounter >= 10) || (lastSRecord_))
						{
							uiUpdateCounter = 0;

							UpgradeTransferStatus statusMsg(bytesSentSoFar, totalBytesToSend);
							dispatchEvent(&statusMsg);
							Debug::trace("CLACommManager.burn", HobString("SM:DOWNLOADING_CODE:send update page msg"));
						}
					}
					else
					{
						Debug::trace("CLACommManager.burn", HobString("SM:DOWNLOADING_CODE: cannot send record"));
						Debug::trace("CLACommManager.burn",HobString("intDataLength_") + HobString(Integer::toString(intDataLength_)) + HobString(Bool::toString(xmitting_)));
					}
				}
				break;

				case LAST_SRECORD_SENT:
				{
					//wait for response from CLA
					Debug::trace("CLACommManager.burn", HobString("SM:LAST_SRECORD_SENT"));
				}
				break;

				case PAUSE_BEFORE_RETRY:
				{
					Debug::trace("CLACommManager.burn", HobString("SM:PAUSE_BEFORE_RETRY:pause complete"));
					PAL_absclass::PAL_sleep(30000);

					codeUpdaterStateMachine_ = WAITING_FOR_STATUS_START;
				}
				break;

				case DOWNLOAD_COMPLETE:
				{
					//if there is nothing currently being processed and there is
					//nothing remaining to be processed then this flash is complete.
					if ((processingList_.size() == 0) && (pendingList_.size() == 0))
					{
						Debug::trace("CLACommManager.burn", HobString("SM:DOWNLOAD_COMPLETE:all nodes flashed. transition to ALL_DOWNLOADS_COMPLETE"));
						codeUpdaterStateMachine_ = ALL_DOWNLOADS_COMPLETE;
					}
					else
					{
						Debug::trace("CLACommManager.burn", HobString("SM:DOWNLOAD_COMPLETE:last node flash complete. still more to flash. transition to WAITING_FOR_STATUS_START"));
						//this node is flashed. now wait for the next node to be ready
						//for flashing.
						codeUpdaterStateMachine_ = WAITING_FOR_STATUS_START;
					}
				}
				break;

				case ALL_DOWNLOADS_COMPLETE:
				{
					Debug::trace("CLACommManager.burn", HobString("SM:ALL_DOWNLOADS_COMPLETE:all node flashs complete. wait to send next update message if necessary. transition to WAITING_TO_SEND_UPDATE"));
					UpgradePeripheral msg(UpgradeComplete, LabelApplier, CompactLabelApplier_EPSAC2);
					dispatchEvent(&msg);
					codeUpdaterStateMachine_ = WAITING_TO_SEND_UPDATE;
				}
				break;

				case WAIT_FOR_SREC_COMPLETE:
				case UNKNOWN:
				{
				}
				break;
			}
			CLACommManagerCS.Leave();
			Debug::trace("CLAManager.ProbeCLA", " Exit Comm Run");
			PAL_absclass::PAL_waitForSingleObj(m_ExitEvent, sleepTime);				
		}
		Debug::trace("CLACommManager.burn", HobString("m_ExitEvent has been set"));
	}
	catch (...)
	{
		LogCatch;
	}
	return 0;
}

bool CLACommManager::createCommLink()
{
	//start the run thread
	if (pClaCommManagerThread)
	{
		pClaCommManagerThread->RestartThread(); 
		return true;
	}
	else
	{
		return false;
	}
}


bool CLACommManager::initializePeripheral()
{
	/*setup the int out transfer to initiate the flash update*/
	return true;
}

bool CLACommManager::downloadCode(const char * pCode)
{
	bool traceOn = Debug::isTracing("CLACommManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "downloadCode():  ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("initiate flashing code through interrupt pipe file = ").append(pCode);
		Debug::trace("CLACommManager", msgDebug2);
	}
	Debug::trace("CLACommManager", HobString("initiate flashing code through interrupt pipe file = ") + HobString(pCode));

	int blockSize = 0;
	int fileSize = 0; 

	if (PAL_absclass::PAL_getFileSize(pCode, &fileSize))
	{
		Debug::trace("CLACommManager", "have file size!");
		blockSize = 1024;

		PAL_absclass::PAL_sleep(10);
	}
	else
	{
		Debug::trace("CLACommManager", "failed to get file size.");
		return false;
	}

	int bytesRead = 0;
	UINT8* tmpPtr;

	Debug::trace("CLACommManager.burn", HobString("file size = ") + Integer::toString(fileSize));

	if ((fileSize > 0) && (blockSize > 0))
	{
		totalBytesToSend = fileSize;

		if (flashImgBuffer_ != 0)
		{
			delete[] flashImgBuffer_;
			flashImgBuffer_ = 0;			
		}

		tmpPtr  = flashImgBuffer_ = (UINT8 *) new char[fileSize];
		pIntData_ = tmpPtr;
		if(tmpPtr != 0)
		{
			std::ifstream myFile(pCode, std::ios::in);

			do
			{
				myFile.read(( char * )tmpPtr, blockSize);
				bytesRead = myFile.gcount();

				tmpPtr+= bytesRead;
			}
			while (bytesRead == blockSize);

			myFile.close();

			intDataLength_ = fileSize;

			Debug::trace("CLACommManager.burn", HobString("Pointer  pIntData_:") + Hex::toString((long long)pIntData_ ));
			Debug::trace("CLACommManager.burn", HobString("Pointer flashImgBuffer_:") + Hex::toString((long long)flashImgBuffer_ ));
			
			sizeModeSRecord_ = true;
		}
	}
	return true;
}

bool CLACommManager::finalizePeripheral()
{
	return true;
}

bool CLACommManager::rebootPeripheral()
{
	Debug::trace("CLACommManager.burn", HobString("Rebooting peripheral"));
	claWakeUp();
	return true;
}


bool CLACommManager::deleteCommLink()
{
	return true;
}


bool CLACommManager::areTransactionsComplete()
{
	if ((pendingList_.size() == 0) && (processingList_.size() == 0))
	{
		Debug::trace("CLACommManager.burn", HobString("transactions complete"));
		return true;
	}
	else
		return false;
	//return false;
}

void CLACommManager::resetTransferCounters()
{
	uiUpdateCounter = 0;
	bytesSentSoFar = 0;
	totalBytesToSend = 0;
	updateSessionCounter = 0;
}

void CLACommManager::updateUserInterface(HobVersion::VersionType softVersionType, MessageTypes messageType)
{
	HobString msgDebug("updateUserInterface:");
	msgDebug.append("  softVersionType = ").append(Integer::toString(softVersionType));
	msgDebug.append("  messageType = ").append(Integer::toString(messageType));
	Debug::trace("CLACommManager", msgDebug);

	CUCLAStatus status((UINT16) softVersionType, (UINT16) messageType);
	status.show();
	Debug::trace("CLACommManager", "updateUserInterface:  end of show *****");
	dispatchEvent(&status);
}

void CLACommManager::checkUpgradeProgress(bool updateAnyway)
{
	if (updateAnyway || (++updateSessionCounter == 25))
	{
		UpgradeTransferStatus statusMsg(bytesSentSoFar, totalBytesToSend);
		dispatchEvent(&statusMsg);

		HobString msgDebug("checkUpgradeProgress():  ");
		msgDebug.append("  bytesSentSoFar = ").append(Long::toString(bytesSentSoFar));
		msgDebug.append("  totalBytesToSend = ").append(Long::toString(totalBytesToSend));
		Debug::trace("CLACommManager.updateProgress", msgDebug);
		updateSessionCounter = 0;
	}
}

HobVersion::VersionType CLACommManager::getSoftwareType(UINT16 nodeID, LaSoftwareComponent type)
{

    switch (nodeID)
    {
        case 2://CLA
            {
                switch (type)
                {
                    case claData:
                        return HobVersion::CLADataSW;
                        break;
                    case claBoot:
                        return HobVersion::CLABootSW;
                        break;
                    case claApp:
                        return HobVersion::CLAAppSW;
                        break;
                    default:
                        return HobVersion::CLAAppSW;
                        break;
                }
            }
            break;
        default:
            break;
    }

    return HobVersion::UnknownComponent;
}
