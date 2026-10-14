
#include "PAL_platformheader.h"
//#include <winsock2.h>	  /* for htons, ntohs, etc. */ // replaced by PAL header


#include "RealTimeController.h"
#include "UsbManager.h"
#include "WrapperManager.h"
#include "LabelApplierManager.h"
#include "EvolWrapperCommManager.h"
#include "USBEvolHeader.h"
#include "Wrapper.h"
#include "ErrorHandling.h"

#include "EvolWrapperWakeup.h"
#include "EvolWrapperStatus.h"
#include "EvolWrapperControl.h"
#include "EvolWrapperStatusInfo.h"
#include "EvolWrapperErrorInfo.h"
#include "EvolWrapperTemperatureInfo.h"
#include "EvolWrapperLoad.h"
#include "EvolWrapperConfiguration.h"
#include "EvolWrapperInquire.h"
#include "EvolWrapperSensors.h"
#include "EvolWrapperTest.h"
#include "EvolWrapperTestComplete.h"
#include "EvolWrapperCycle.h"
#include "EvolWrapperPackageTimings.h"
#include "EvolWrapperPackageDesc.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "EvolWrapperPackageTimingsPointer.h"
#include "EvolWrapperCameraScan.h"
#include "EvolWrapperCameraHeightScan.h"
#include "EvolWrapperCameraCalibration.h"
#include "EvolWrapperEventCapture.h"
#include "EvolWrapperErrorHistory.h"
#include "EvolWrapperCycleCount.h"
#include "EvolWrapperSaveCycleCount.h"

#include "EvolWrapperCameraThreshold.h"
#include "EvolWrapperWriteCameraThreshold.h"
#include "EvolWrapperPackageDischarge.h"
#include "EvolWrapperMotorProfile.h"
#include "EvolWrapperDiagnosticsTest.h"
#include "EvolWrapperDiagnosticsTestResult.h"

#include "LabelApplierWakeup.h"
#include "LabelApplierLabelReady.h"
#include "LabelApplierLabelTaken.h"
#include "LabelApplierSetup.h"
#include "LabelApplierMode.h"

#include "WrapperOpen.h"
#include "EvolWrapperWrap.h"
#include "WrapperCycle.h"
#include "WrapperJog.h"
#include "WrapperMode.h"
#include "WrapperTest.h"
#include "EvolWrapperSystemStatus.h"
#include "EvolWrapperNode.h"
#include "EvolWrapperSystemInfo.h"
#include "EvolWrapperSystemInfoWrite.h"

#include "EvolWrapperReqUIModel.h"
#include "CLASystemStatus.h"
#include "NGWDimensionOverrides.h"

#include "HobDateTime.h"
#include "UsbAddComponent.h"
#include "UsbRemoveComponent.h"

#include "UsbWrapperCodeUpdater.h"
#include "CUEvolWrapperStatus.h"
#include "UpgradeTransferStatus.h"
#include "UpgradePeripheral.h"
#include "FpmManager.h"

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

int EvolWrapperCommManager::dataType = 0;
std::string EvolWrapperCommManager::dataTypeName("EvolWrapperCommManager"); 
#endif


EvolWrapperCommManager::EvolWrapperCommManager(USBD_NODE_ID node_id, SUsbDescriptors * descs ): CUsbCommBase( descs, node_id ), nodeId( node_id )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("EvolWrapperCommManager", "EvolWrapperCommManager");   

	this->nodeId = nodeId;

	pMyWrapper = NULL;
	pMyLabelApplier = NULL;
	pCodeUpdater = new UsbWrapperCodeUpdater(node_id, 0, this);

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
	deviceType_ = 0;//Defaults to AWS Alex Zavala 3/7/14
	resetTransferCounters();

	/*intialize the lists*/
	processingList_.clear();
	pendingList_.clear();
	successfullList_.clear();
	failedList_.clear();
	popedDialogs_.clear();

    //get my type of system
    FpmManager *pSerialFlash = FpmManager::getInstance();
    mySystemType = pSerialFlash->getMFGSystemStyle();

	sleepTime = 10000;  //inital run thread sleep time
	//create run thread for code updates, but do not start till needed! -- ats
	pEvolWrapperCommManagerThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "EvolWrapperCommManager Thread", false, false, false );

	//PAL_absclass::PAL_sleep( 10000 );
    Debug::trace("EvolWrapperCommManager", "Create Wrapper Pipes:");
	createPipes();
    Debug::trace("EvolWrapperCommManager", "Start Wrapper Pipes:");
	startPipes();

#if defined ( __linux__ )
    Debug::trace("EvolWrapperCommManager", HobString("PipeHandle =") + Hex::toString(getMyPipeHandle()));
#endif

    Debug::trace("EvolWrapperCommManager", "Request Wrapper wakeup.");
	wrapperWakeUp();
	Debug::trace("EvolWrapperCommManager", "EvolWrapperCommManager:  Constructor is done.");  
}

EvolWrapperCommManager::~EvolWrapperCommManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager");  


	UsbRemoveComponent removeMe(nodeId, this);
	dispatchEvent(&removeMe);

	deregisterAllEvents();
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  deregisterAllEvents()");  

	//shutdown the msg thread
	ShutdownNicely();
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  ShutdownNicely()");  

	//tell the thread to exit
	m_ExitEvent.EventSet();
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  m_ExitEvent.EventSet()");  

	//wait for the thread to exit before deleting all the resources!
	if ( pEvolWrapperCommManagerThread )
	{
		while ( pEvolWrapperCommManagerThread->IsRunning() )
		{
			Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  The thread is still active!");  
			PAL_absclass::PAL_sleep(1);
		}

		pEvolWrapperCommManagerThread = NULL;
	}
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  pClaCommManagerThread = NULL");  

	if(pMyLabelApplier)
	{
		delete pMyLabelApplier;
		pMyLabelApplier = NULL;
	}
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  pMyLabelApplier = NULL");  

	if(pMyWrapper)
	{
		delete pMyWrapper;
		pMyWrapper = NULL;
	}
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  pMyWrapper = NULL");  

	if(pCodeUpdater)
	{
		delete pCodeUpdater;
		pCodeUpdater = NULL;
	}
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  pCodeUpdater = NULL");  

	if (flashImgBuffer_ != 0)
	{
		delete[] flashImgBuffer_;
		flashImgBuffer_ = NULL;
	}
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager():  flashImgBuffer_ = NULL");  

	/*cleanup lists*/
	processingList_.clear();
	pendingList_.clear();
	successfullList_.clear();
	failedList_.clear();
	popedDialogs_.clear();
	Debug::trace("EvolWrapperCommManager", "~EvolWrapperCommManager(): Exit");
}


void EvolWrapperCommManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	try
	{
		Util::CLockCriticalSection lock( EvolWrapperCommManagerCS );

		switch ( msg->getDeviceType() )
		{
		case Wrapper:
			handleWrapperEvent((WrEvent) msg->getEventType(), msg->Data());
			break;

		case WrapperSystem:
			handleWrapperSysEvent((SysEvent) msg->getEventType(), msg->Data());
			break;

		case LabelApplier:
			handleLabelApplierEvent((LaEvent) msg->getEventType(), msg->Data());
			break;

		default:
			Debug::trace("EvolWrapperCommManager.Error", "***** Unknown message, device type " +
				inttostring(msg->getDeviceType()) + " event number " + inttostring(msg->getEventType()));
			break;
		}
	}
	catch ( ... )
	{
		Debug::trace("EvolWrapperCommManager.Message.Out", "EvolWrapperCommManager::processEvent. caught exception");
	}
}

USBD_NODE_ID EvolWrapperCommManager::getNodeID()
{
	return nodeId;
}

void EvolWrapperCommManager::evolWrapperSystemReset()
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	EvolWrGeneric genMsg;
	memset((char *) &genMsg, 0, sizeof(genMsg));

	genMsg.wrEvent = htons((UINT16) WR_REQ_HARD_SYSTEM_RESET);
	usbEvolHdr.setSourcePhysicalAddress(0x0000);
	usbEvolHdr.setSourceLogicalAddress(0x0009);
	usbEvolHdr.setDestinationPhysicalAddress(0x8181);
	usbEvolHdr.setDestinationLogicalAddress(0x001D);
	usbEvolHdr.setMessageLength(sizeof(genMsg));
	usbEvolHdr.Serialize( str );

	memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
	sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

	if ( result != OK ) //error
	{
		Debug::trace("EvolWrapperCommManager.Error", "Failed to hard reset the Wrapper!");
		printUSBError( result, is_custom_code );
	}
}

void EvolWrapperCommManager::wrapperWakeUp()
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	EvolWrGeneric genMsg;
	memset((char *) &genMsg, 0, sizeof(genMsg));

	genMsg.wrEvent = htons((UINT16) WR_WAKEUP);
	usbEvolHdr.setSourcePhysicalAddress(0x0000);
	usbEvolHdr.setSourceLogicalAddress(0x0009);
	usbEvolHdr.setDestinationPhysicalAddress(0x8181);
	usbEvolHdr.setDestinationLogicalAddress(0x001D);
	usbEvolHdr.setMessageLength(sizeof(genMsg));
	usbEvolHdr.Serialize( str );

	memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
	sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

	if ( result != OK ) //error
	{
		Debug::trace("EvolWrapperCommManager.Error", "Failed to send Request wakeup!");
		printUSBError( result, is_custom_code );
	}
}

bool EvolWrapperCommManager::handleWrapperEvent(WrEvent wrEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (wrEventType)
	{
		case WrReqWakeup:
		{
//				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrReqWakeup");	  
			Debug::trace("WrapperManager.ProbeWrapper", "Comm Mgr -- WrReqWakeup");
			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_WAKEUP);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
			if(result != OK)
			{
				Debug::trace("WrapperManager.ProbeWrapper", "Comm Mgr -- WrReqWakeup FAILED TO SEND!!!!!!");

				Debug::trace("EvolWrapperCommManager", "WrReqWakeup FAILED TO SEND!!!!!!");
			}
		}
		break;

		case WrEnable:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrEnable");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_ENABLE);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		case WrDisable:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrDisable");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_DISABLE);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		case WrReset:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrReset");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_RESET);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		case WrReqStatus:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrReqStatus");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_STATUS);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		case WrReqInquire:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrReqInquire");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_INQUIRE);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		case WrEvolConfig:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrEvolConfig");

			EvolWrapperConfiguration wrapperConfig(buffer);

			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "********************** case WR_CONFIG start ******************************************************");
			wrapperConfig.show();
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "********************** case WR_CONFIG end ********************************************************");

			EvolWrConfig configMsg;
			memset((char *) &configMsg, 0, sizeof(configMsg));

			configMsg.functionCode = htons((UINT16) WR_CONFIG);
			configMsg.disposition = htons((UINT16) WrPermanent);

			//	  EvolWrapperHeaterConfig
			EvolWrapperHeaterConfig heaterCfg = wrapperConfig.getHeaterConfig();
			configMsg.config.heaterConfig.checksum = htonl(heaterCfg.getChecksum());

			HeaterConfiguration filmHeaterCfg = heaterCfg.getFilmHeaterConfig();
			configMsg.config.heaterConfig.filmHeater.isUsed						 = htons(filmHeaterCfg.isItUsed());
			configMsg.config.heaterConfig.filmHeater.maxTemp						= htons(filmHeaterCfg.getMaxTemp());
			configMsg.config.heaterConfig.filmHeater.minTemp						= htons(filmHeaterCfg.getMinTemp());
			configMsg.config.heaterConfig.filmHeater.defaultTempSetting	 = htons(filmHeaterCfg.getDefaultTempSetting());

			HeaterConfiguration sealerHeaterCfg = heaterCfg.getSealerHeaterConfig();
			configMsg.config.heaterConfig.sealerHeater.isUsed				= htons(sealerHeaterCfg.isItUsed());
			configMsg.config.heaterConfig.sealerHeater.maxTemp				= htons(sealerHeaterCfg.getMaxTemp());
			configMsg.config.heaterConfig.sealerHeater.minTemp				= htons(sealerHeaterCfg.getMinTemp());
			configMsg.config.heaterConfig.sealerHeater.defaultTempSetting	= htons(sealerHeaterCfg.getDefaultTempSetting());

			//	  EvolWrapperGeneralConfig
			EvolWrapperGeneralConfig wrGenCfg = wrapperConfig.getWrapperConfig();
			configMsg.config.wrapperGenConfig.checksum							  = htonl(wrGenCfg.getChecksum());

			WrapperFilmWidthDimensions topFilm = wrGenCfg.getTopFilmRollDimensions();
			configMsg.config.wrapperGenConfig.topFilmRoll.rollWidth				 = htons(topFilm.getRollWidth());
			configMsg.config.wrapperGenConfig.topFilmRoll.rollDiameter			  = htons(topFilm.getRollDiameter());
			configMsg.config.wrapperGenConfig.topFilmRoll.remainingOnRoll	= htons(topFilm.getRemainingOnRoll());

			WrapperFilmWidthDimensions bottomFilm = wrGenCfg.getBottomFilmRollDimensions();
			configMsg.config.wrapperGenConfig.bottomFilmRoll.rollWidth					  = htons(bottomFilm.getRollWidth());
			configMsg.config.wrapperGenConfig.bottomFilmRoll.rollDiameter		= htons(bottomFilm.getRollDiameter());
			configMsg.config.wrapperGenConfig.bottomFilmRoll.remainingOnRoll		= htons(bottomFilm.getRemainingOnRoll());

			configMsg.config.wrapperGenConfig.wrapperOperation					  = htons((u_short)wrGenCfg.getWrapperOperation());
			configMsg.config.wrapperGenConfig.filmTensionNetDefault		 = htons(wrGenCfg.getFilmTensionNetDefault());
			configMsg.config.wrapperGenConfig.addOnHardware						 = htons((u_short)wrGenCfg.getHardwareAddOn());
			configMsg.config.wrapperGenConfig.filmTensionServiceAdjust	  = htons(wrGenCfg.getFilmTensionServiceAdjust());
			configMsg.config.wrapperGenConfig.maxPackageWeight					  = htons(wrGenCfg.getMaxPackageWeight());
			configMsg.config.wrapperGenConfig.minPackageWeight					  = htons(wrGenCfg.getMinPackageWeight());
			configMsg.config.wrapperGenConfig.maxPackageLength					  = htons(wrGenCfg.getMaxPackageLength());
			configMsg.config.wrapperGenConfig.minPackageLength					  = htons(wrGenCfg.getMinPackageLength());
			configMsg.config.wrapperGenConfig.maxPackageWidth					= htons(wrGenCfg.getMaxPackageWidth());
			configMsg.config.wrapperGenConfig.lowerFilmPullTension         = htons(wrGenCfg.getLowerFilmPullTension());
			configMsg.config.wrapperGenConfig.upperFilmPullTension         = htons(wrGenCfg.getUpperFilmPullTension());
			configMsg.config.wrapperGenConfig.sealerBeltPackageSensorType  = htons(wrGenCfg.getSealerBeltPackageSensorType());
			configMsg.config.wrapperGenConfig.packageGrandTotal					 = htonl(wrGenCfg.getPackageGrandTotal());
			configMsg.config.wrapperGenConfig.wrapperCycleTotal					 = htonl(wrGenCfg.getWrapperCycleTotal());
			configMsg.config.wrapperGenConfig.filmTensionFrontToBack		= htons(wrGenCfg.getFilmTensionFrontToBack());
			configMsg.config.wrapperGenConfig.filmTensionRightToLeft		= htons(wrGenCfg.getFilmTensionRightToLeft());

			//	  EvolLabelApplierConfiguration
			EvolLabelApplierConfiguration applierCfg = wrapperConfig.getLabelApplierConfig();
			configMsg.config.labelApplierConfig.checksum							= htonl(applierCfg.getChecksum());
			configMsg.config.labelApplierConfig.autoRotation				= htons(applierCfg.doesHaveAutoRotation());
			configMsg.config.labelApplierConfig.priLabelSizeDefault		 = htons(applierCfg.getPriLabelSizeDefault());
			configMsg.config.labelApplierConfig.priLabelWidth					= htons((u_short)applierCfg.getPriLabelWidth());								
			configMsg.config.labelApplierConfig.priLabelRotationDefault					 = htons((u_short)applierCfg.getPriLabelRotationDefault());
			configMsg.config.labelApplierConfig.priLabelLocationDefault					 = htons((u_short)applierCfg.getPriLabelLocationDefault());
			configMsg.config.labelApplierConfig.priLabelOffsetOnPkg		 = htons(applierCfg.getPriLabelOffsetOnPkg());
			configMsg.config.labelApplierConfig.priRotationCalibrationOffset		= htons(applierCfg.getPriRotationCalibrationOffset());
			configMsg.config.labelApplierConfig.priWandCalibrationOffset		= htons(applierCfg.getPriWandCalibrationOffset());

			configMsg.config.labelApplierConfig.secLabelSizeDefault		 = htons(applierCfg.getSecLabelSizeDefault());
			configMsg.config.labelApplierConfig.secLabelWidth									= htons((u_short)applierCfg.getSecLabelWidth());		
			configMsg.config.labelApplierConfig.secLabelRotationDefault					 = htons((u_short)applierCfg.getSecLabelRotationDefault());
			configMsg.config.labelApplierConfig.secLabelLocationDefault					 = htons((u_short)applierCfg.getSecLabelLocationDefault());
			configMsg.config.labelApplierConfig.secLabelOffsetOnPkg		 = htons(applierCfg.getSecLabelOffsetOnPkg());
			configMsg.config.labelApplierConfig.secRotationCalibrationOffset		= htons(applierCfg.getSecRotationCalibrationOffset());
			configMsg.config.labelApplierConfig.secWandCalibrationOffset		= htons(applierCfg.getSecWandCalibrationOffset());

			configMsg.config.labelApplierConfig.secondCLAAttached		= htons(applierCfg.isSecondCLAAttached());
			configMsg.config.labelApplierConfig.sideDischargeType		= htons((u_short)applierCfg.getSideDischargeType());

			configMsg.config.labelApplierConfig.priRotationCalibrationOffset_90Deg		= htons(applierCfg.getPriRotationCalibrationOffset_90Deg());
			configMsg.config.labelApplierConfig.priRotationCalibrationOffset_180Deg		= htons(applierCfg.getPriRotationCalibrationOffset_180Deg());
			configMsg.config.labelApplierConfig.priRotationCalibrationOffset_270Deg		= htons(applierCfg.getPriRotationCalibrationOffset_270Deg());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(configMsg));

			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &configMsg, sizeof(configMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(configMsg) + 10, result, is_custom_code );
		}
		break;

		case WrAbort:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrAbort");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_ABORT);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		case WrPause:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrPause");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_PAUSE);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		case WrResume:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrResume");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_RESUME);
			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		case WrEvolWrap:
		{
			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrEvolWrap");

				EvolWrapperWrap ww(buffer);

				EvolWrWrap wrapMsg;
				memset((char *) &wrapMsg, 0, sizeof(wrapMsg));

				wrapMsg.wrEvent = htons((UINT16) WR_WRAP);
				wrapMsg.intakeMode = htons((u_short)ww.getIntakeMode());
				wrapMsg.packageId = htons(ww.getPackageId());
				wrapMsg.pullFilm = htons(ww.isPullFilm());
				wrapMsg.packageWeight = htons(ww.getPackageWeight());
				wrapMsg.pluNumber = htonl(ww.getPluNumber());

				usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
				usbEvolHdr.setMessageLength(sizeof(wrapMsg));
				usbEvolHdr.Serialize( str );

				memcpy( &str[10], (char *) &wrapMsg, sizeof(wrapMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(wrapMsg) + 10, result, is_custom_code );
			}
			else {
				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrEvolWrap: System type AWS3! Do not send USB message.");
			}

		}
		break;

		case WrEvolCycle:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolCycle");

			EvolWrapperCycle wc(buffer);

			EvolWrCycle cycleMsg;
			memset((char *) &cycleMsg, 0, sizeof(cycleMsg));

			cycleMsg.wrEvent = htons((UINT16) WR_CYCLE);

			if (wc.isPullFilm() == true)
			{
				cycleMsg.pullFilm = htons(1);
			}

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(cycleMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &cycleMsg, sizeof(cycleMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(cycleMsg) + 10, result, is_custom_code );
				break;
		}


		case WrJog:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrJog");

			WrapperJog wj(buffer);

			EvolWrJog jogMsg;
			memset((char *) &jogMsg, 0, sizeof(jogMsg));

			jogMsg.wrEvent = htons((UINT16) WR_JOG);
			jogMsg.packageId = htons(wj.getPackageId());

			if (wj.isPullFilm() == true)
			{
				jogMsg.pullFilm = htons(1);
			}

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(jogMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &jogMsg, sizeof(jogMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(jogMsg) + 10, result, is_custom_code );
		}
		break;

		case WrMode:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrMode");
			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				WrapperMode wrMode(buffer);

				EvolWrMode modeMsg;

				modeMsg.wrEvent = htons((UINT16) WR_MODE);
				modeMsg.mode = htons((UINT16) wrMode.getWrOpMode());

				usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
				usbEvolHdr.setMessageLength(sizeof(modeMsg));
				usbEvolHdr.Serialize( str );

				memcpy( &str[10], (char *) &modeMsg, sizeof(modeMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(modeMsg) + 10, result, is_custom_code );
			}
			else {
				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrMode: System type AWS3! Do not send USB message.");
			}
		}
		break;

		case WrEvolControl:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrEvolControl");
			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				EvolWrapperControl wrControl(buffer);

				EvolWrControl controlMsg;
				memset((char *) &controlMsg, 0, sizeof(controlMsg));

				controlMsg.wrEvent = htons((UINT16) WR_CONTROL);

				controlMsg.sealerBeltTempSetting = htons(wrControl.getSealerBeltTempSetting());
				controlMsg.sealerBeltHeaterEnabled = htons(wrControl.getSealerBeltHeaterEnabled());
				controlMsg.filmSupplyTempSetting = htons(wrControl.getFilmSupplyTempSetting());
				controlMsg.filmSupplyHeaterEnabled = htons(wrControl.getFilmSupplyHeaterEnabled());
				controlMsg.filmTension = htons((u_short)wrControl.getFilmTension());
				controlMsg.filmWidthSelection = htons((u_short)wrControl.getFilmWidthSelection());
				controlMsg.filmTensionFrontToBack = htons(wrControl.getFilmTensionFrontToBack());
				controlMsg.filmTensionRightToLeft = htons(wrControl.getFilmTensionRightToLeft());
				controlMsg.intakeMode = htons((u_short)wrControl.getIntakeMode());

				usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
				usbEvolHdr.setMessageLength(sizeof(controlMsg));
				usbEvolHdr.Serialize( str );

				memcpy( &str[10], (char *) &controlMsg, sizeof(controlMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(controlMsg) + 10, result, is_custom_code );
			}
			else {
				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrEvolControl: System type AWS3! Do not send USB message.");
			}
		}
		break;

		case WrLoad:
		case WrEvolLoad:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrEvolLoad");

			EvolWrapperLoad wrLoad(buffer);

			EvolWrLoad loadMsg;
			memset((char *) &loadMsg, 0, sizeof(loadMsg));

			loadMsg.wrEvent = htons((UINT16) WR_LOAD);
			loadMsg.filmWidthSelection = htons((u_short)wrLoad.getFilmWidthSelection());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(loadMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &loadMsg, sizeof(loadMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(loadMsg) + 10, result, is_custom_code );

		}
		break;

		case WrEvolRequestPackageTimings:
		{
			Debug::trace("EvolWrapperCommManager.Message", "EvolWrapCommMgr::handleWrapperEvent() case WrEvolRequestPackageTimings(227)");

			EvolWrapperRequestPackageTimings wrReqTimings(buffer);
			wrReqTimings.show();

			EvolWrGenericOneParam rtMsg;
			memset((char *) &rtMsg, 0, sizeof(rtMsg));

			rtMsg.wrEvent = htons((UINT16) WR_PKG_SETTINGS_READ);
			rtMsg.param = htons((UINT16) wrReqTimings.getTimingsType());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(rtMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &rtMsg, sizeof(rtMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(rtMsg) + 10, result, is_custom_code );
			break;
		}

		case WrEvolSavePackageTimings:
		{
			Debug::trace("EvolWrapperCommManager.Message", "START SAVE @case WrEvolSavePackageTimings");

			EvolWrapperPackageTimingsPointer wptp(buffer);
			EvolWrapperPackageTimings *wrapperTimings = wptp.getTimingsPointer();
				
			if (wrapperTimings)
			{
				// Determine the size of the buffer to send to the wrapper
				int numWrapBoxes = wrapperTimings->getNumberWrapBoxes();
				int numHeightBoxes = 0;

				for ( int i = 0; i < numWrapBoxes; ++i )
				{
					numHeightBoxes += wrapperTimings->getWrapBox(i)->getNumberHeightBoxes();
				}

				//Debug::trace("EWCMTimings.Joe", "SAVE #1 handleWrapperEvent - sizeOf EvolWrHeightBox1P (using): " + Integer::toString(sizeof(EvolWrHeightBox1P)));
				//Debug::trace("EWCMTimings.Joe", "SAVE #2.1 handleWrapperEvent - numWrapBoxes: " + Integer::toString(numWrapBoxes));
				//Debug::trace("EWCMTimings.Joe", "SAVE #2.1 handleWrapperEvent - numHeightBoxes: " + Integer::toString(numHeightBoxes));

                int bufferSize = sizeof(EvolWrPackageTimingsWrite) + numWrapBoxes*sizeof(UINT16) +
					numWrapBoxes*sizeof(EvolWrWrapBox) + numHeightBoxes*sizeof(EvolWrHeightBox1P);

				//Debug::trace("EWCMTimings.Joe", "SAVE #3 handleWrapperEvent - valueOf bufferSize: " + Integer::toString(bufferSize));

				// Prepare the buffer that will hold the whole message
				char *pBufferMsg = (char *)calloc(1, (bufferSize + 10));
				char *pBufferStart = pBufferMsg;

				pBufferMsg += 10;	//save room for the header

				// Write the header
				EvolWrPackageTimingsWrite timingsMsg;
				memset((char*)&timingsMsg, 0, sizeof(EvolWrPackageTimingsWrite));

				timingsMsg.wrEvent = htons((UINT16)WR_PKG_SETTINGS_WRITE);
				timingsMsg.disposition = htons((UINT16) wrapperTimings->getDisposition());
				timingsMsg.wrapBoxes = htons((UINT16)numWrapBoxes);

				memcpy(pBufferMsg, (char*)&timingsMsg, sizeof(EvolWrPackageTimingsWrite));

				// Write the number of height boxes per wrap box
				int bufCounter = sizeof(EvolWrPackageTimingsWrite);		// Skip event and # wrap Boxes
				//Debug::trace("EWCMTimings.Joe", "SAVE #4 handleWrapperEvent - valueOf bufCounter (6): "    + Integer::toString(bufCounter));

				for ( int i = 0; i < numWrapBoxes; ++i )
				{
					*(UINT16*)(pBufferMsg + bufCounter) = htons((UINT16)wrapperTimings->getWrapBox(i)->getNumberHeightBoxes());
					bufCounter += sizeof(UINT16);
				}

                // Write the wrap boxes
				for ( int i = 0; i < numWrapBoxes; ++i )
				{
					const WrapBox* wrapBox = wrapperTimings->getWrapBox(i);
					EvolWrWrapBox wrBox;
					memset((char*)&wrBox, 0, sizeof(EvolWrWrapBox));

					// Convert the name from a char-based string to a UINT16-based array.
					{
						const string& name = wrapBox->getName();
						int j = 0, characters = (int)name.size();
						for ( ; j < 10 && j < characters; ++j )
							wrBox.name[j] = htons((UINT16)name[j]);
						for ( int k = j; k < 10; ++k )
							wrBox.name[k] = htons((UINT16)' ');
					}
					wrBox.pluNumHigh = htons((UINT16) (wrapBox->getPLUException() >> 16));
					wrBox.pluNumLow = htons((UINT16) (wrapBox->getPLUException() & 0x0FFFF));

					wrBox.lengthLow = htons((UINT16)wrapBox->getLengthLow());
					wrBox.lengthHigh = htons((UINT16)wrapBox->getLengthHigh());
					wrBox.widthLow = htons((UINT16)wrapBox->getWidthLow());
					wrBox.widthHigh = htons((UINT16)wrapBox->getWidthHigh());
					wrBox.lipHeight = htons((UINT16)wrapBox->getLip());

					memcpy(pBufferMsg + bufCounter, (char*)&wrBox, sizeof(EvolWrWrapBox));
					bufCounter += sizeof(EvolWrWrapBox);
				}

				//Debug::trace("EWCMTimings.Joe", "SAVE #5 handleWrapperEvent - valueOf bufCounter: " + Integer::toString(bufCounter));

				// Write the height boxes
				for ( int i = 0; i < numWrapBoxes; ++i )
				{
					const WrapBox* wrapBox = wrapperTimings->getWrapBox(i);
					WrapBox::const_iterator iter = wrapBox->begin();
					while ( iter != wrapBox->end() )
					{
						const WrapBox::HeightBox* heightBox = &*(iter++);
						EvolWrHeightBox1P wrHeightBox1P;
						memset((char*)&wrHeightBox1P, 0, sizeof(EvolWrHeightBox1P));

						wrHeightBox1P.height						= htons((UINT16)heightBox->getHeight());
						wrHeightBox1P.percentStretch				= htons((UINT16)heightBox->getParameter(0));
						wrHeightBox1P.knifeFireHeight				= htons((UINT16)heightBox->getParameter(1));
						wrHeightBox1P.clampOpenHeight				= htons((UINT16)heightBox->getParameter(2));
						wrHeightBox1P.elevatorPlacement				= htons((UINT16)heightBox->getParameter(3));
						wrHeightBox1P.addTrayLength					= htons((UINT16)heightBox->getParameter(4));
						wrHeightBox1P.gripperPullAdjustment			= htons((UINT16)heightBox->getParameter(5));
						wrHeightBox1P.frontUnderfolderAdjustment	= htons((UINT16)heightBox->getParameter(6));
						wrHeightBox1P.wrapSpeed						= htons((UINT16)heightBox->getParameter(7));
						wrHeightBox1P.dischargeSpeed				= htons((UINT16)heightBox->getParameter(8));
						wrHeightBox1P.sideUnderfolderAdjustment		= htons((UINT16)heightBox->getParameter(9));
						wrHeightBox1P.heightRestriction				= htons((UINT16)heightBox->getParameter(10));
						wrHeightBox1P.sideUnderfolderReturnHome		= htons((UINT16)heightBox->getParameter(11));
						wrHeightBox1P.gripperAllowShortPull			= htons((UINT16)heightBox->getParameter(12));
						wrHeightBox1P.sideClampGap					= htons((UINT16)heightBox->getParameter(13));
						wrHeightBox1P.filmRollToUse					= htons((UINT16)heightBox->getParameter(14));
						wrHeightBox1P.rearUnderfolderTripDelay		= htons((UINT16)heightBox->getParameter(15));
						wrHeightBox1P.brakeTiming					= htons((UINT16)heightBox->getParameter(16));
						wrHeightBox1P.outsideGripperRelease			= htons((UINT16)heightBox->getParameter(17));
						wrHeightBox1P.insideGripperRelease			= htons((UINT16)heightBox->getParameter(18));
						/*
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box height: "							+ Integer::toString(heightBox->getHeight() ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[0] percentStretch: "				+ Integer::toString(heightBox->getParameter(0) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[1] knifeFireHeight: "				+ Integer::toString(heightBox->getParameter(1) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[2] clampOpenHeight: "				+ Integer::toString(heightBox->getParameter(2) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[3] elevatorPlacement: "				+ Integer::toString(heightBox->getParameter(3) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[4] addTrayLength: "					+ Integer::toString(heightBox->getParameter(4) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[5] gripperPullAdjustment: "			+ Integer::toString(heightBox->getParameter(5) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[6] frontUnderfolderAdjustment: "	+ Integer::toString(heightBox->getParameter(6) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[7] wrapSpeed: "						+ Integer::toString(heightBox->getParameter(7) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[8] dischargeSpeed: "				+ Integer::toString(heightBox->getParameter(8) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[9] sideUnderfolderAdjustment: "		+ Integer::toString(heightBox->getParameter(9) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[10] heightRestriction: "			+ Integer::toString(heightBox->getParameter(10) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[11] sideUnderfolderReturnHome: "	+ Integer::toString(heightBox->getParameter(11) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[12] gripperAllowShortPull: "		+ Integer::toString(heightBox->getParameter(12) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[13] sideClampGap: "					+ Integer::toString(heightBox->getParameter(13) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[14] filmRollToUse: "				+ Integer::toString(heightBox->getParameter(14) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[15] rearUnderfolderTripDelay: "		+ Integer::toString(heightBox->getParameter(15) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[16] brakeTiming: "					+ Integer::toString(heightBox->getParameter(16) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[17] outsideGripperRelease: "		+ Integer::toString(heightBox->getParameter(17) ) );
						Debug::trace("EWCMTimings.Joe", "handleWrapperEvent() case WrEvolSavePackageTimings box[18] insideGripperRelease: "			+ Integer::toString(heightBox->getParameter(18) ) );

						Debug::trace("EWCMTimings.Joe", "SAVE #7.1 handleWrapperEvent - inside for loop - sizeOf pBufferMsg: "    + Integer::toString(sizeof(pBufferMsg)));
						Debug::trace("EWCMTimings.Joe", "SAVE #7.1 handleWrapperEvent - inside for loop - sizeOf wrHeightBox1P: " + Integer::toString(sizeof(wrHeightBox1P)));
						Debug::trace("EWCMTimings.Joe", "SAVE #7.2 handleWrapperEvent - inside for loop - valueOf bufCounter: "   + Integer::toString(bufCounter) );		// SOF-6077 not the sizeOf, but the valueOf now
						*/

						memcpy(pBufferMsg + bufCounter, (char*)&wrHeightBox1P, sizeof(EvolWrHeightBox1P));

						if ((i == (numWrapBoxes - 1)))
						{
							HobString msgDebug;
							msgDebug.append("Copied to address ").append(Hex::toString((long long) (pBufferMsg + bufCounter)));
							msgDebug.append(", Height box ").append(Integer::toString(heightBox->getHeight()));
							Debug::trace("EWCMTimings.Joe", msgDebug);
							HobString msgDebug2 = HobString(Hex::toString((pBufferMsg + bufCounter), sizeof(EvolWrHeightBox1P)) );			// SOF-6077 not the sizeOf, but the valueOf now
							Debug::trace("EWCMTimings.Joe", msgDebug2);
						}
						bufCounter += sizeof(EvolWrHeightBox1P);
					}
				}

				usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
				usbEvolHdr.setMessageLength((UINT16)bufferSize);
				usbEvolHdr.Serialize( str );

				//Debug::trace("EWCMTimings.Joe", "SAVE #8 handleWrapperEvent - send buffer, bufferSize + 10: " + Integer::toString(bufferSize + 10 ));

				memcpy(pBufferStart, &str[0], 10); //copy the header
				sendUsbInterruptMessage( ( const char * ) pBufferStart,  bufferSize + 10, result, is_custom_code );

				// this creates a small memory leak but allows more than one manager to process
				// this message (and prevents a crash)
				//delete wrapperTimings;

				free(pBufferStart);
			}
			else
				Debug::trace("EvolWrapperCommManager.Message", "EvolWrapperPackageTimings pointer for WrEvolSavePackageTimings is NULL!");

			//Debug::trace("EWCMTimings.Joe", "END EvolWRAPCommMgr::handleWrapperEvent() case WrEvolSavePackageTimings: ");

			break;
		}

		case WrEvolResetDefaultPackageTimings:
		{
			Debug::trace("WrapperManager.Message", "WrEvolResetDefaultPackageTimings");

			EvolWrPackageTimingsWrite resetTimingsMsg;
			memset((char*)&resetTimingsMsg, 0, sizeof(EvolWrPackageTimingsWrite));

			resetTimingsMsg.wrEvent = htons((UINT16)WR_PKG_SETTINGS_WRITE);
			resetTimingsMsg.disposition = htons((UINT16)WRITE_DEFAULT);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(EvolWrPackageTimingsWrite));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &resetTimingsMsg, sizeof(resetTimingsMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(resetTimingsMsg) + 10, result, is_custom_code );
			break;
		}

		case WrEvolUnlockPackageTimings:
		{
			Debug::trace("WrapperManager.Message", "WrEvolUnlockPackageTimings");
			break;
		}

		case WrTest:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "WrTest");

			WrapperTest wrTest(buffer);

			EvolWrTest testMsg;
			memset((char *) &testMsg, 0, sizeof(testMsg));

			testMsg.wrEvent = htons((UINT16) WR_TEST);
			testMsg.testNum = htons((UINT16)wrTest.getWrTestCode());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(testMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &testMsg, sizeof(testMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(testMsg) + 10, result, is_custom_code );
			break;
		}

		case WrEvolTest:
		{
			HobString msgDebug = "WrEvolTest";
			bool traceOn = Debug::isTracing("EvolWrapperCommManager.Message");
			if (traceOn)
				Debug::trace("EvolWrapperCommManager.Message", "WrEvolTest");

			EvolWrapperTest wrTest(buffer);
			wrTest.show();

			EvolWrTest testMsg;
			memset((char *) &testMsg, 0, sizeof(testMsg));

			testMsg.wrEvent = htons((UINT16) WR_TEST);
			testMsg.testNum = htons((UINT16)wrTest.getTestCode());
			if (traceOn)
				msgDebug.append(": testMsg.testNum = ").append(Integer::toString(testMsg.testNum));

			if (!wrTest.doUseIntParams())
			{
				testMsg.parameter.dataLong = htonl((UINT32) wrTest.getDataLong());
				if (traceOn)
					msgDebug.append("  testMsg.parameter.dataLong = ").append(Integer::toString(testMsg.parameter.dataLong));
			}
			else
			{
				testMsg.parameter.dataInt.data1 = htons((INT16) wrTest.getDataInt1());
				testMsg.parameter.dataInt.data2 = htons((INT16) wrTest.getDataInt2());
				if (traceOn)
				{
					msgDebug.append("  testMsg.parameter.dataInt.data1 = ").append(Integer::toString(testMsg.parameter.dataInt.data1));
					msgDebug.append("  testMsg.parameter.dataInt.data2 = ").append(Integer::toString(testMsg.parameter.dataInt.data2));
				}
			}
			if (traceOn)
				Debug::trace("EvolWrapperCommManager.Message", msgDebug);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(testMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &testMsg, sizeof(testMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(testMsg) + 10, result, is_custom_code );
			break;
		}

		case WrReqSensors:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrReqSensors");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_SENSE);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );  
			break;  
		}

		case WrEvolSendDateTime:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolSendDateTime");

			HobDateTime dt;
			EvolWrDateTime dateMsg;
			memset((char *) &dateMsg, 0, sizeof(dateMsg));

			dateMsg.wrEvent = htons((UINT16) WR_DATE_TIME);
			dateMsg.month = htons(dt.getMonth());
			dateMsg.day = htons(dt.getDay());
			dateMsg.year = htons(dt.getYear4());
			dateMsg.hours = htons(dt.getHour24());
			dateMsg.minutes = htons(dt.getMinute());
			dateMsg.seconds = htons(dt.getSecond());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(dateMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &dateMsg, sizeof(dateMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(dateMsg) + 10, result, is_custom_code );				 
			break;				  
		}

		case WrEvolReqCycleCount:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolReqCycleCount");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_WRAPPER_REQ_CYCLE_COUNT);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );  
			break;
		}
		case WrEvolSaveCycleCount:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolSaveCycleCount");

			EvolWrapperSaveCycleCount evolWrapperSaveCycleCount(buffer);

			EvolWrCycleCount evolWrSaveCycleCount;
			memset((char *) &evolWrSaveCycleCount, 0, sizeof(evolWrSaveCycleCount));

			evolWrSaveCycleCount.wrEvent = htons((UINT16) WR_WRAPPER_SAVE_CYCLE_COUNT);
			evolWrSaveCycleCount.cycleCount = htonl((UINT32) evolWrapperSaveCycleCount.getWrapperCycleCount());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(evolWrSaveCycleCount));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &evolWrSaveCycleCount, sizeof(evolWrSaveCycleCount));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(evolWrSaveCycleCount) + 10, result, is_custom_code );	
			break;
		}

		case WrEvolRequestCameraThreshold:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolRequestCameraThreshold");	
			EvolWrCameraThreshold evolWrCameraThreshold;
			evolWrCameraThreshold.wrEvent = htons((UINT16) WR_REQ_CAMERA_THRESHOLD);
			evolWrCameraThreshold.cameraThreshold = 0;

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(evolWrCameraThreshold));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &evolWrCameraThreshold, sizeof(evolWrCameraThreshold));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(evolWrCameraThreshold) + 10, result, is_custom_code );
			break;
		}
		case WrEvolWriteCameraThreshold:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolWriteCameraThreshold");	
			EvolWrapperWriteCameraThreshold evolWrapperWriteCameraThreshold(buffer);

			EvolWrCameraThreshold evolWrCameraThreshold;
			evolWrCameraThreshold.wrEvent = htons((UINT16) WR_WRITE_CAMERA_THRESHOLD);
			evolWrCameraThreshold.cameraThreshold = htons((UINT16)evolWrapperWriteCameraThreshold.getWrapperWriteCameraThreshold());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(evolWrCameraThreshold));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &evolWrCameraThreshold, sizeof(evolWrCameraThreshold));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(evolWrCameraThreshold) + 10, result, is_custom_code );
			break;
		}

		case WrEvolDiagnosticsTest:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolDiagnosticsTest");

			EvolWrapperDiagnosticsTest wrDiag(buffer);
			wrDiag.show();

			EvolWrDiagnosticsTest rtMsg;
			memset((char *) &rtMsg, 0, sizeof(rtMsg));

			rtMsg.wrEvent = htons((UINT16) WR_DIAGNOSTICS_TEST);
			rtMsg.test = htons((UINT16) wrDiag.getTest());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(rtMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &rtMsg, sizeof(rtMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(rtMsg) + 10, result, is_custom_code );
			break;
		}

		case WrEvolStartMfgRunin:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolStartMfgRunin");

			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_START_MFG_RUNIN);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
			break;
		}

		case WrReqEvolSystemInfo:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrReqEvolSystemInfo");	
			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_REQ_SYSTEM_INFO);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );  
			break;
		}
		case WrEvolSystemInfoErase:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolSystemInfoErase");	
			EvolWrGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.wrEvent = htons((UINT16) WR_SYSTEM_INFO_ERASE);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );  
			break;
		}
		case WrEvolSystemInfoWrite:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolSystemInfoWrite");	
			EvolWrapperSystemInfoWrite	  evolWrapperSystemInfoWrite(buffer);
			EvolWrSystemInfoWrite evolWrSystemInfoWrite;

			evolWrSystemInfoWrite.wrEvent = htons((UINT16) WR_SYSTEM_INFO_WRITE);
			evolWrSystemInfoWrite.awsModelNumber = htons((UINT16)evolWrapperSystemInfoWrite.getModelNumber());
			evolWrSystemInfoWrite.awsCycleCount = htonl((UINT32)evolWrapperSystemInfoWrite.getAwsCycleCount());

			const string& serialNumber = evolWrapperSystemInfoWrite.getAwsSerialNumber();
			for (unsigned int i = 0; i < (EvolWrapperSystemInfo::_SerialNumbSize_ + 1); i += 2)
			{
				evolWrSystemInfoWrite.awsSerialNumber[i] = serialNumber[i + 1];
				evolWrSystemInfoWrite.awsSerialNumber[i + 1] = serialNumber[i];
			}
			evolWrSystemInfoWrite.awsSerialNumber[10] = '0';
			evolWrSystemInfoWrite.awsSerialNumber[11] = serialNumber[EvolWrapperSystemInfo::_SerialNumbSize_ + 1];

			const string& dateMan = evolWrapperSystemInfoWrite.getAwsDateOfManufacture();
			for (unsigned int i = 0; i < EvolWrapperSystemInfo::_DateSize_; i += 2)
			{
				evolWrSystemInfoWrite.awsDateOfManufacture[i] = dateMan[i + 1];
				evolWrSystemInfoWrite.awsDateOfManufacture[i + 1] = dateMan[i];
			}

			const string& dateIns = evolWrapperSystemInfoWrite.getAwsDateOfInstall();
			for (unsigned int i = 0; i < EvolWrapperSystemInfo::_DateSize_; i += 2)
			{
				evolWrSystemInfoWrite.awsDateOfInstall[i] = dateIns[i + 1];
				evolWrSystemInfoWrite.awsDateOfInstall[i + 1] = dateIns[i];
			}

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(evolWrSystemInfoWrite));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &evolWrSystemInfoWrite, sizeof(evolWrSystemInfoWrite));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(evolWrSystemInfoWrite) + 10, result, is_custom_code );

			// As per Carlos, wrapper needs time to init eeprom and write data. Apparently during
			// this time USB is completely disabled.  So delay a bit to prevent too many messages
			// being sent at once.
			PAL_absclass::PAL_sleep( 5000 );

			break;
		}
		case WrEvolReqSystemReset:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrEvolReqSystemReset");	
			evolWrapperSystemReset();
			break;
		}
		case WrUIWrapperModel:
		{
			Debug::trace("EvolWrapperCommManager.Message", "WrUIWrapperModel");
			EvolWrapperUIModel evolWrapperUIModel(buffer);
			
			EWSWrapperType_ = evolWrapperUIModel.isModelSelectionEWS(); 
			deviceType_ = evolWrapperUIModel.getModelSelection();
			Debug::trace("EvolWrapperCommManager.Message", "WrUIWrapperModel:  Selection Value = " + Integer::toString(evolWrapperUIModel.getModelSelection()));

			//if(evolWrapperUIModel.isModelSelectionCLA())
			//{
				/*if(pCLACodeUpdater)
				{
					if(!pCLACodeUpdater->isUpdaterBusy())
					{
						CLASystemStatus *BLANK_CLA_STATUS = new CLASystemStatus(this);
						BLANK_CLA_STATUS->setConfigurationChecksum(0);
						BLANK_CLA_STATUS->setAppFlashMajor(0);
						BLANK_CLA_STATUS->setAppFlashMinor(0);
						BLANK_CLA_STATUS->setBootFlashMajor(0);
						BLANK_CLA_STATUS->setBootFlashMinor(0);
						BLANK_CLA_STATUS->setDataFlashMajor(0);
						BLANK_CLA_STATUS->setDataFlashMinor(0);
						pCLACodeUpdater->requestUpdate(*BLANK_CLA_STATUS);
						pCLACodeUpdater->sendUpdateRequest();
						delete BLANK_CLA_STATUS;
					}
					else
					{
						Debug::trace("EvolWrapperCommManager.Message", "UI: CLA Updater is Busy!!!!");
					}
				}
				else
				{
					Debug::trace("EvolWrapperCommManager.Message", "UI: CLA Updater is NULL");
				}*/
			if(pCodeUpdater)
			{
				if(!pCodeUpdater->isUpdaterBusy())
				{
					Debug::trace("EvolWrapperCommManager.Message", "UI: Went into Code Updater");
					pCodeUpdater->sendUpdateRequest(evolWrapperUIModel.getModelSelection());
				}
				else
				{
					Debug::trace("EvolWrapperCommManager.Message", "UI: Wrapper Updater is Busy!!!!");
				}
			}
			else
			{
				Debug::trace("EvolWrapperCommManager.Message", "UI: CLA Updater is NULL");
			}
			//}
			/*else
			{
				if(pCodeUpdater)
				{
					if(!pCodeUpdater->isUpdaterBusy())
					{
						pCodeUpdater->sendUpdateRequest(EWSWrapperType_);
					}
					else
					{
						Debug::trace("EvolWrapperCommManager.Message", "UI: Wrapper Updater is Busy!!!!");
					}
				}
				else
				{
						Debug::trace("EvolWrapperCommManager.Message", "UI: Wrapper Updater is NULL");
				}
			}*/
			break;
		}
		case WrNGWDimensionOverrides: 
		{
			NGWDimensionOverrides overrides( buffer );
			EvolWrDimensionOverrides ovMsg;
			
			ovMsg.wrEvent = htons( (UINT16)WR_PKG_SIZE_OVERRIDE );
			ovMsg.length = htons( (UINT16)overrides.getOverrideLength() );
			ovMsg.width = htons( (UINT16)overrides.getOverrideWidth() );
			ovMsg.lip_height = htons( (UINT16)overrides.getOverrideLipHeight() );

			usbEvolHdr.setDestinationLogicalAddress( USBEvolHeader::WRAPPER );
			usbEvolHdr.setMessageLength( sizeof(ovMsg) );
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &ovMsg, sizeof(ovMsg) );
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(ovMsg) + 10, result, is_custom_code );
			break;
		}			
		default:
		{
			Debug::trace("EvolWrapperCommManager.Error", HobString("Unknown Wrapper message type: 0x") +
															HobString(Hex::toString(wrEventType)));
		}
		break;
	}

	if ( result != OK ) //error
	{
		/*The has message failed! there is no mechanism in place to deal with this situation!
		  for now we will print some debug, but a mechanism needs to be in place to retry! -- ats*/
		Debug::trace("EvolWrapperCommManager.Error", std::string("***SEND TIMEOUT***::error = ") + Integer::toString(result));

		printUSBError( result, is_custom_code );
	}

	return  true;
}

bool EvolWrapperCommManager::handleWrapperSysEvent(SysEvent sysEventType, const char *UNUSED(buffer))
{
	UNREFERENCED_PARAMETER(buffer);

	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (sysEventType)
	{
		case SysReqStatus:
			{
				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "SysReqStatus");

				EvolWrGeneric genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.wrEvent = htons((UINT16) WR_SYS_STATUS);

				usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
				usbEvolHdr.setMessageLength(sizeof(genMsg));
				usbEvolHdr.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

			}
			break;
		default				 :
			{
				Debug::trace("EvolWrapperCommManager.Error", HobString("Unknown Wrapper System message type: 0x") +
						HobString(Hex::toString(sysEventType)));
			}
			break;
	}

	if ( result != OK ) //error
	{
		printUSBError( result, is_custom_code );
	}

	return true;
}

bool EvolWrapperCommManager::handleLabelApplierEvent(LaEvent laEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (laEventType)
	{
		case LaReqWakeup:
		{
			// Just return this message!!!
			// Since this is part of the wrapper, we'll just use
			// wrapper wakeups to determine loss of communication!
			//Debug::trace("EvolWrapperCommManager.ProbeWrapper", "LaReqWakeup");
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaReqWakeup");

			LabelApplierWakeup laWakeup(this, true);

			const char *pLawuMsg = laWakeup.serialize();
			int mLength = laWakeup.getSerializedLength();

			if (pMyWrapper)
				pMyWrapper->send(pLawuMsg, mLength);														
		}
		break;

/*
		case LaMode:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaMode");

			// Constuct the Evol Message Buffer!!
			LabelApplierMode laMode(buffer);
			EvolLaMode modeMsg;

			modeMsg.laEvent = htons(LA_MODE);
			modeMsg.mode = htons((u_short)laMode.getMode());

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(modeMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &modeMsg, sizeof(modeMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(modeMsg) + 10, result, is_custom_code );
		}
		break;
*/
		case LaLabelReady:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaLabelReady");

			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				LabelApplierLabelReady laLabelReady(buffer);
				EvolLaLabelReady evolLabelReady;

				memset((char *) &evolLabelReady, 0, sizeof(evolLabelReady));

				evolLabelReady.laEvent = htons(LA_LABEL_READY);
				evolLabelReady.station = htons((u_short)laLabelReady.getStationId());
				evolLabelReady.packageId = htons(laLabelReady.getPackageId());

				usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
				usbEvolHdr.setMessageLength(sizeof(evolLabelReady));
				usbEvolHdr.Serialize( str );

				memcpy( &str[10], (char *) &evolLabelReady, sizeof(evolLabelReady));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(evolLabelReady) + 10, result, is_custom_code );
			}
			else {
				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaLabelReady: System type AWS3! Do not send USB message.");
			}
		}
		break;

		case LaLabelTaken:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaLabelTaken");

			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				LabelApplierLabelTaken laLabelTaken(buffer);
				EvolLaLabelTaken evolLabelTaken;

				memset((char *) &evolLabelTaken, 0, sizeof(evolLabelTaken));

				evolLabelTaken.laEvent = htons(LA_LABEL_TAKEN);
				evolLabelTaken.station = htons((u_short)laLabelTaken.getStationId());
				evolLabelTaken.packageId = htons(laLabelTaken.getPackageId());

				usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
				usbEvolHdr.setMessageLength(sizeof(evolLabelTaken));
				usbEvolHdr.Serialize( str );

				memcpy( &str[10], (char *) &evolLabelTaken, sizeof(evolLabelTaken));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(evolLabelTaken) + 10, result, is_custom_code );
			}
			else {
				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaLabelTaken: System type AWS3! Do not send USB message.");
			}
		}
		break;

		case LaSetup:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaSetup");
			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				LabelApplierSetup laSetup(buffer);

				EvolLaSetup setupMsg;
				memset((char *) &setupMsg, 0, sizeof(setupMsg));

				setupMsg.laEvent = htons(LA_SETUP);
				setupMsg.priLabelSize = htons(laSetup.getPrimaryLabelSize());
				setupMsg.priLabelRotation = htons((UINT16) laSetup.getPrimaryLabelRotation());
				setupMsg.priLabelLocation = htons((UINT16) laSetup.getPrimaryLabelPlacement());
				setupMsg.secLabelSize = htons(laSetup.getSecondaryLabelSize());
				setupMsg.secLabelRotation = htons((UINT16) laSetup.getSecondaryLabelRotation());
				setupMsg.secLabelLocation = htons((UINT16) laSetup.getSecondaryLabelPlacement());


				usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
				usbEvolHdr.setMessageLength(sizeof(setupMsg));
				usbEvolHdr.Serialize( str );

				memcpy( &str[10], (char *) &setupMsg, sizeof(setupMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(setupMsg) + 10, result, is_custom_code );
			}
			else {
				Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaSetup: System type AWS3! Do not send USB message.");
			}

		}
		break;

/*
		case LaEnable:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaEnable");

			EvolLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.laEvent = htons(LA_ENABLE);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

		}
		break;

		case LaDisable:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaDisable");

			EvolLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.laEvent = htons(LA_DISABLE);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

		}
		break;

		case LaReqOff:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaReqOff");

			EvolLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.laEvent = htons(LA_OFF);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;
*/
		case LaOn:
		{
			Debug::trace("EvolWrapperCommManager.Wrapper.Message.Out", "LaOn");

			EvolLaGeneric genMsg;
			memset((char *) &genMsg, 0, sizeof(genMsg));

			genMsg.laEvent = htons(LA_ON);

			usbEvolHdr.setDestinationLogicalAddress(USBEvolHeader::WRAPPER);
			usbEvolHdr.setMessageLength(sizeof(genMsg));
			usbEvolHdr.Serialize( str );

			memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
			sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
		}
		break;

		default:
		{
			Debug::trace("EvolWrapperCommManager.Error", HobString("Unknown LabelApplier message type: 0x") +
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

bool EvolWrapperCommManager::isValidHeader( const char *header ) const
{
	USBEvolHeader usbEvolHeader( header, false );

	const UINT16 physicalAddress( usbEvolHeader.getSourcePhysicalAddress() );

	if ( 0x8181 == physicalAddress )
	{
		const UINT16 logicalAddress( usbEvolHeader.getSourceLogicalAddress() );

		return (( USBEvolHeader::SYSTEM == logicalAddress ) || ( USBEvolHeader::WRAPPER == logicalAddress ));
	}

	return false;
}

bool EvolWrapperCommManager::locateNextHeader( const char *msg, unsigned int msgLen, unsigned int &address ) const
{
	if ( 10 < msgLen )
	{
		for ( address = 1; address <= (msgLen - 10); ++address )
			if ( isValidHeader( &msg[address] ) )
				return true;
	}

	return false;
}

void EvolWrapperCommManager::processUsbMessage( std::string & msg, CUsbPipe::EvolPipeType UNUSED(pipe_type) )
{
	UNREFERENCED_PARAMETER(pipe_type);

	const char *ptr( msg.c_str() );

	USBEvolHeader usbEvolHeader(ptr, false);

	const UINT16 logicalAddress( usbEvolHeader.getSourceLogicalAddress() );

	ptr += 10;

	if ( USBEvolHeader::SYSTEM == logicalAddress )
	{
		receivedWrapperSysMessage((SysGenericMsg *) ptr); //, physicalAddress);
	}
	else if ( USBEvolHeader::WRAPPER == logicalAddress )
	{
		const UINT16 physicalAddress( usbEvolHeader.getSourcePhysicalAddress() );
		const UINT16 msgSize( usbEvolHeader.getMessageLength() );

		receivedWrapperDevMessage((EvolWrGeneric *) ptr, physicalAddress, msgSize); //, logicalAddress);
	}
	else
	{
		ptr-= 10;
		Debug::trace("EvolWrapperCommManager.Error", HobString("Bad Header!!!:")  + Hex::toString( ptr, 10, true ));

		ptr += 10; //index past the header and print the message code.
		Debug::trace("EvolWrapperCommManager.Error", HobString("body: ") + Hex::toString( ptr, 2, true ));
	}
}

void EvolWrapperCommManager::receivedWrapperSysMessage(SysGenericMsg *genMsg) 
{
	UINT16 functionCode = ntohs(genMsg->function_code);

	switch (functionCode)
	{
		case SysStatus:
		{
			Debug::trace("WrapperManager.WrapperSystemReset", HobString("Ultima Wrapper -- SysStatus"));
		}
		break;
		default:
		{
			Debug::trace("EvolWrapperCommManager.Error", HobString("Received Unknown Wrapper System message type: 0x") +
					HobString(Hex::toString(functionCode)));
		}
		break;
	}
}

void EvolWrapperCommManager::receivedWrapperDevMessage(EvolWrGeneric *genMsg, UINT16 physicalAddress, INT16 msgLen)
{
	//var's are now used!!
	bool traceOn = Debug::isTracing("EvolWrapperCommManager");
	UINT16 wrEvent = ntohs(genMsg->wrEvent);
	int value1, value2;

	switch (wrEvent)
	{
		case WR_WAKEUP:
		{
			Debug::trace("EvolWrapperCommManager.WrWakeup", "USB Wrapper: WrEvolWakeup -- entered");
			EvolWrWakeup *wakeupMsg = (EvolWrWakeup *) genMsg;

			if (pMyWrapper == NULL)
			{
				Debug::trace("EvolWrapperCommManager.WrWakeup", "USB Wrapper -- WrWakeup ************************************************");

				// Create a new thread
				pMyWrapper = dynamic_cast<WrapperManager *> (RealTimeController::getInstance()->NewUSBWrapper(nodeId, this, physicalAddress));

				pMyLabelApplier = dynamic_cast<LabelApplierManager *> (RealTimeController::getInstance()->NewUSBLabelApplier(nodeId, this, true, physicalAddress));

				usbEvolHdr.setDestinationPhysicalAddress(physicalAddress);
			}
			else
			{
				bool result = false;

				if (ntohs(wakeupMsg->allNodeAvailable) > 0)
					result = true;

				EvolWrapperWakeup ww(this, (WrCfgStatus) ntohs(wakeupMsg->wrCfgStatus), result);

				const char *pWwMsg = ww.serialize();
				int mLength = ww.getSerializedLength();

				pMyWrapper->send(pWwMsg, mLength);
			}
		} 
		Debug::trace("EvolWrapperCommManager.WrWakeup", "USB Wrapper: WrEvolWakeup -- Completed");
		break;

		case WR_STATUS:
		{
			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: WrEvolStatus");

				EvolWrStatus *statusMsg = (EvolWrStatus *) genMsg;

				EvolWrapperStatus wrapperStatus(this);

				//EvolWrapperStatusInfo statusInfo;
				EvolWrapperStatusInfo statusInfo( wrapperStatus.getStatusInfo() );
				statusInfo.setReportStatus( ((ntohs((UINT16)statusMsg->statusInfo.reportStatus) != 0) ? true : false) );
				statusInfo.setOpenWindow( ((ntohs((UINT16)statusMsg->statusInfo.openWindow) != 0) ? true : false) );
				statusInfo.setLimpHome( ((ntohs((UINT16)statusMsg->statusInfo.limpHome) != 0) ? true : false) );
				statusInfo.setWrapperStopped( ((ntohs((UINT16)statusMsg->statusInfo.wrapperStopped) != 0) ? true : false));
				statusInfo.setMainPower( ((ntohs((UINT16)statusMsg->statusInfo.mainPower) != 0) ? true : false));
				statusInfo.setMainMotor( ((ntohs((UINT16)statusMsg->statusInfo.mainMotor) != 0) ? true : false));
				statusInfo.setWrapperState((EvolWrapperStatusInfo::EvolWrapperState) ntohs((UINT16)statusMsg->statusInfo.wrapperState));
				wrapperStatus.setStatusInfo(statusInfo);

				//EvolWrapperErrorInfo currErrorInfo;
				EvolWrapperErrorInfo currErrorInfo( wrapperStatus.getCurrErrorInfo() );
				currErrorInfo.setPrimaryConveyor((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.primaryConveyor));
				currErrorInfo.setSecondaryConveyor((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.secondaryConveyor));
				currErrorInfo.setSecondaryShift((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.secondaryShift));
				currErrorInfo.setElevator((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.elevator));
				currErrorInfo.setFilmSupply((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.filmSupply));
				currErrorInfo.setGripper((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.gripper));
				currErrorInfo.setKnife((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.knife));
				currErrorInfo.setSideClamp((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.sideClamp));
				currErrorInfo.setSideFolder((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.sideFolder));
				currErrorInfo.setRearFolder((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.rearFolder));
				currErrorInfo.setFrontFolder((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.frontFolder));
				currErrorInfo.setPusher((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.pusher));
				currErrorInfo.setSealerBelt((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.sealerBelt));
				currErrorInfo.setLabelApplier((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.labelApplier));
				currErrorInfo.setGeneralWrapper((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->currErrorInfo.generalWrapper));
				currErrorInfo.setDiagnosticsSupport((UINT16) ntohs((UINT16)statusMsg->currErrorInfo.diagnosticsSupport));
				wrapperStatus.setCurrErrorInfo(currErrorInfo);

				//EvolWrapperErrorInfo prevErrorInfo;
				EvolWrapperErrorInfo prevErrorInfo( wrapperStatus.getPrevErrorInfo() );
				prevErrorInfo.setPrimaryConveyor((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.primaryConveyor));
				prevErrorInfo.setSecondaryConveyor((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.secondaryConveyor));
				prevErrorInfo.setSecondaryShift((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.secondaryShift));
				prevErrorInfo.setElevator((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.elevator));
				prevErrorInfo.setFilmSupply((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.filmSupply));
				prevErrorInfo.setGripper((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.gripper));
				prevErrorInfo.setKnife((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.knife));
				prevErrorInfo.setSideClamp((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.sideClamp));
				prevErrorInfo.setSideFolder((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.sideFolder));
				prevErrorInfo.setRearFolder((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.rearFolder));
				prevErrorInfo.setFrontFolder((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.frontFolder));
				prevErrorInfo.setPusher((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.pusher));
				prevErrorInfo.setSealerBelt((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs((UINT16)statusMsg->prevErrorInfo.sealerBelt));
				prevErrorInfo.setLabelApplier((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs(statusMsg->prevErrorInfo.labelApplier));
				prevErrorInfo.setGeneralWrapper((UINT16)(EvolWrapperErrorInfo::EvolWrapperError) ntohs(statusMsg->prevErrorInfo.generalWrapper));
				prevErrorInfo.setDiagnosticsSupport((UINT16) ntohs((UINT16)statusMsg->prevErrorInfo.diagnosticsSupport));
				wrapperStatus.setPrevErrorInfo(prevErrorInfo);

				//EvolWrapperTemperatureInfo temperatureInfo;
				EvolWrapperTemperatureInfo temperatureInfo( wrapperStatus.getTemperatureInfo() );
				temperatureInfo.setSealerBeltTemp(ntohs(statusMsg->temperatureInfo.sealerBeltTemperature));
				temperatureInfo.setSealerBeltSetting(ntohs(statusMsg->temperatureInfo.sealerBeltSetting));
				temperatureInfo.setSealerBeltStatusBits(ntohs(statusMsg->temperatureInfo.sealerBeltStatusBits));
				temperatureInfo.setSealerBeltHeaterEnabled( ((ntohs(statusMsg->temperatureInfo.sealerBeltHeaterEnabled) != 0) ? true : false) );
				temperatureInfo.setFilmTemp(ntohs(statusMsg->temperatureInfo.filmTemperature));
				temperatureInfo.setFilmSetting(ntohs(statusMsg->temperatureInfo.filmSetting));
				temperatureInfo.setFilmStatusBits(ntohs(statusMsg->temperatureInfo.filmStatusBits));
				temperatureInfo.setFilmTempEnabled( ((ntohs(statusMsg->temperatureInfo.filmTempEnabled) != 0) ? true : false) );
				temperatureInfo.setAmbientTemp(ntohs(statusMsg->temperatureInfo.ambientTemperature));
				temperatureInfo.setAmbientTempStatusBits(ntohs(statusMsg->temperatureInfo.ambientTemperatureStatusBits));

				for (UINT16 i = MaxNodeBoards; i > 0; i--)
				{
					temperatureInfo.setNodeBoardTemp(ntohs(statusMsg->temperatureInfo.nodeBoardTemperature[i-1].nodeId),
							ntohs(statusMsg->temperatureInfo.nodeBoardTemperature[i-1].temp),
							ntohs(statusMsg->temperatureInfo.nodeBoardTemperature[i-1].statusBits));
				}

				temperatureInfo.setInternalTempFailed( ((ntohs(statusMsg->temperatureInfo.internalTemperatureFailed) != 0) ? true : false) );
				wrapperStatus.setTemperatureInfo(temperatureInfo);

				wrapperStatus.setWrapperMode((WrOpMode) ntohs(statusMsg->wrapperMode));

				if (pMyWrapper)
				{
					const char *pWwMsg = wrapperStatus.serialize();
					int mLength = wrapperStatus.getSerializedLength();

					pMyWrapper->send(pWwMsg, mLength);
				}
				else
					Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: WrEvolStatus:  Don't have a WrapperManager yet!");
			}
			else {
				Debug::trace("EvolWrapperCommManager.Message", "EvolWrStatus: AWS3 system type! Do not process this USB message." );
			}
		}
		break;

		case WR_OPEN:
		{
			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: WrOpen");

				WrapperOpen openMsg(this); // use default constructor

				if (pMyWrapper)
				{
					const char *pOpenMsg = openMsg.serialize();
					int mLength = openMsg.getSerializedLength();

					pMyWrapper->send(pOpenMsg, mLength);
				}
			}
			else {
				Debug::trace("EvolWrapperCommManager.Message", "WrapperOpen: AWS3 system type! Do not process this USB message." );
			}
		}
		break;

		case WR_INQUIRE:				
		{
			EvolWrInquire *inquireMsg = (EvolWrInquire *) genMsg;

			EvolWrapperInquire wrapperInquire(this); // use default constructor

			//	EvolWrapperHeaterConfig
			EvolWrapperHeaterConfig &heaterCfg = wrapperInquire.getHeaterConfig();
			heaterCfg.setChecksum(ntohl(inquireMsg->config.heaterConfig.checksum));

			HeaterConfiguration &filmHeaterCfg = heaterCfg.getFilmHeaterConfig();
			filmHeaterCfg.setIsUsed( ((ntohs(inquireMsg->config.heaterConfig.filmHeater.isUsed) != 0) ? true : false) );
			filmHeaterCfg.setMaxTemp(ntohs(inquireMsg->config.heaterConfig.filmHeater.maxTemp));
			filmHeaterCfg.setMinTemp(ntohs(inquireMsg->config.heaterConfig.filmHeater.minTemp));
			filmHeaterCfg.setDefaultTempSetting(ntohs(inquireMsg->config.heaterConfig.filmHeater.defaultTempSetting));

			HeaterConfiguration &sealerHeaterCfg = heaterCfg.getSealerHeaterConfig();
			sealerHeaterCfg.setIsUsed( ((ntohs(inquireMsg->config.heaterConfig.sealerHeater.isUsed) != 0) ? true : false) );
			sealerHeaterCfg.setMaxTemp(ntohs(inquireMsg->config.heaterConfig.sealerHeater.maxTemp));
			sealerHeaterCfg.setMinTemp(ntohs(inquireMsg->config.heaterConfig.sealerHeater.minTemp));
			sealerHeaterCfg.setDefaultTempSetting(ntohs(inquireMsg->config.heaterConfig.sealerHeater.defaultTempSetting));


			//	EvolWrapperGeneralConfig
			EvolWrapperGeneralConfig &wrGenCfg = wrapperInquire.getWrapperConfig();
			wrGenCfg.setChecksum(ntohl(inquireMsg->config.wrapperGenConfig.checksum));

			WrapperFilmWidthDimensions &topFilm = wrGenCfg.getTopFilmRollDimensions();
			topFilm.setRollWidth(ntohs(inquireMsg->config.wrapperGenConfig.topFilmRoll.rollWidth));
			topFilm.setRollDiameter(ntohs(inquireMsg->config.wrapperGenConfig.topFilmRoll.rollDiameter));
			topFilm.setRemainingOnRoll(ntohs(inquireMsg->config.wrapperGenConfig.topFilmRoll.remainingOnRoll));

			WrapperFilmWidthDimensions &bottomFilm = wrGenCfg.getBottomFilmRollDimensions();
			bottomFilm.setRollWidth(ntohs(inquireMsg->config.wrapperGenConfig.bottomFilmRoll.rollWidth));
			bottomFilm.setRollDiameter(ntohs(inquireMsg->config.wrapperGenConfig.bottomFilmRoll.rollDiameter));
			bottomFilm.setRemainingOnRoll(ntohs(inquireMsg->config.wrapperGenConfig.bottomFilmRoll.remainingOnRoll));

			wrGenCfg.setWrapperOperation((WrOperation) ntohs(inquireMsg->config.wrapperGenConfig.wrapperOperation));
			wrGenCfg.setFilmTensionNetDefault(ntohs(inquireMsg->config.wrapperGenConfig.filmTensionNetDefault));
			wrGenCfg.setHardwareAddOn((WrHardwareAddOn)ntohs(inquireMsg->config.wrapperGenConfig.addOnHardware));
			wrGenCfg.setFilmTensionServiceAdjust(ntohs(inquireMsg->config.wrapperGenConfig.filmTensionServiceAdjust));
			wrGenCfg.setMaxPackageWeight(ntohs(inquireMsg->config.wrapperGenConfig.maxPackageWeight));
			wrGenCfg.setMinPackageWeight(ntohs(inquireMsg->config.wrapperGenConfig.minPackageWeight));
			wrGenCfg.setMaxPackageLength(ntohs(inquireMsg->config.wrapperGenConfig.maxPackageLength));
			wrGenCfg.setMinPackageLength(ntohs(inquireMsg->config.wrapperGenConfig.minPackageLength));
			wrGenCfg.setMaxPackageWidth(ntohs(inquireMsg->config.wrapperGenConfig.maxPackageWidth));
			wrGenCfg.setLowerFilmPullTension(ntohs(inquireMsg->config.wrapperGenConfig.lowerFilmPullTension));
			wrGenCfg.setUpperFilmPullTension(ntohs(inquireMsg->config.wrapperGenConfig.upperFilmPullTension));
			wrGenCfg.setSealerBeltPackageSensorType(ntohs(inquireMsg->config.wrapperGenConfig.sealerBeltPackageSensorType));
			wrGenCfg.setPackageGrandTotal(ntohl(inquireMsg->config.wrapperGenConfig.packageGrandTotal));
			wrGenCfg.setWrapperCycleTotal(ntohl(inquireMsg->config.wrapperGenConfig.wrapperCycleTotal));
			wrGenCfg.setFilmTensionFrontToBack(ntohs(inquireMsg->config.wrapperGenConfig.filmTensionFrontToBack));
			wrGenCfg.setFilmTensionRightToLeft(ntohs(inquireMsg->config.wrapperGenConfig.filmTensionRightToLeft));

			//	  EvolLabelApplierConfiguration
			EvolLabelApplierConfiguration &applierCfg = wrapperInquire.getLabelApplierConfig();
			applierCfg.setChecksum(ntohl(inquireMsg->config.labelApplierConfig.checksum));
			applierCfg.setHasAutoRotation( (ntohs(inquireMsg->config.labelApplierConfig.autoRotation) != 0) ? true : false);

			applierCfg.setPriLabelSizeDefault(ntohs(inquireMsg->config.labelApplierConfig.priLabelSizeDefault));
			applierCfg.setPriLabelWidth((LabelWidth) ntohs(inquireMsg->config.labelApplierConfig.priLabelWidth));
			applierCfg.setPriLabelRotationDefault((LaRotation) ntohs(inquireMsg->config.labelApplierConfig.priLabelRotationDefault));
			applierCfg.setPriLabelLocationDefault((LaPosLoc) ntohs(inquireMsg->config.labelApplierConfig.priLabelLocationDefault));
			applierCfg.setPriLabelOffsetOnPkg(ntohs(inquireMsg->config.labelApplierConfig.priLabelOffsetOnPkg));
			applierCfg.setPriRotationCalibrationOffset(ntohs(inquireMsg->config.labelApplierConfig.priRotationCalibrationOffset));
			applierCfg.setPriWandCalibrationOffset(ntohs(inquireMsg->config.labelApplierConfig.priWandCalibrationOffset));

			applierCfg.setSecLabelSizeDefault(ntohs(inquireMsg->config.labelApplierConfig.secLabelSizeDefault));
			applierCfg.setSecLabelWidth((LabelWidth) ntohs(inquireMsg->config.labelApplierConfig.secLabelWidth));
			applierCfg.setSecLabelRotationDefault((LaRotation) ntohs(inquireMsg->config.labelApplierConfig.secLabelRotationDefault));
			applierCfg.setSecLabelLocationDefault((LaPosLoc) ntohs(inquireMsg->config.labelApplierConfig.secLabelLocationDefault));
			applierCfg.setSecLabelOffsetOnPkg(ntohs(inquireMsg->config.labelApplierConfig.secLabelOffsetOnPkg));
			applierCfg.setSecRotationCalibrationOffset(ntohs(inquireMsg->config.labelApplierConfig.secRotationCalibrationOffset));
			applierCfg.setSecWandCalibrationOffset(ntohs(inquireMsg->config.labelApplierConfig.secWandCalibrationOffset));

			applierCfg.setIsSecondCLAAttached( ((ntohs(inquireMsg->config.labelApplierConfig.secondCLAAttached) != 0) ? true : false) );
			applierCfg.setSideDischargeType((LaSideDischarge) ntohs(inquireMsg->config.labelApplierConfig.sideDischargeType));

			applierCfg.setPriRotationCalibrationOffset_90Deg(ntohs(inquireMsg->config.labelApplierConfig.priRotationCalibrationOffset_90Deg));
			applierCfg.setPriRotationCalibrationOffset_180Deg(ntohs(inquireMsg->config.labelApplierConfig.priRotationCalibrationOffset_180Deg));
			applierCfg.setPriRotationCalibrationOffset_270Deg(ntohs(inquireMsg->config.labelApplierConfig.priRotationCalibrationOffset_270Deg));

			if (pMyWrapper)
			{
				const char *pInquireMsg = wrapperInquire.serialize();
				int mLength = wrapperInquire.getSerializedLength();

				pMyWrapper->send(pInquireMsg, mLength);
			}


			Debug::trace("EvolWrapperCommManager.Message", "********************** case WR_INQUIRE start ******************************************************");
			wrapperInquire.show();
			Debug::trace("EvolWrapperCommManager.Message", "********************** case WR_INQUIRE end ********************************************************");
		}
		break;
		case WR_SYS_STATUS:
		{
			EvolWrapperSystemStatus wrSysStatus(this);
			unsigned int i = 0;
			WrSysStatus *pSysStatusMsg = 0;
			WrSysStatus_Old *pSysStatusMsg_Old = 0;
			bool useOld = false;

			if (msgLen == sizeof(WrSysStatus_Old))
				useOld = true;

			if (useOld)
			{
				pSysStatusMsg_Old = (WrSysStatus_Old *) genMsg;

				wrSysStatus.setReportSysError(ntohs(pSysStatusMsg_Old->report_sys_error));

				for (i = 0; i < EvolWrapperSystemStatus::_maxBoards_; ++i)
				{
					pSysStatusMsg_Old->systems_status[i].nodeid = ntohs(pSysStatusMsg_Old->systems_status[i].nodeid);
					pSysStatusMsg_Old->systems_status[i].bit_error = ntohs(pSysStatusMsg_Old->systems_status[i].bit_error);
					pSysStatusMsg_Old->systems_status[i].device_init_flags = ntohs(pSysStatusMsg_Old->systems_status[i].device_init_flags);
					pSysStatusMsg_Old->systems_status[i].nodeVersion.appFlash.major = ntohs(pSysStatusMsg_Old->systems_status[i].nodeVersion.appFlash.major);
					pSysStatusMsg_Old->systems_status[i].nodeVersion.appFlash.minor = ntohs(pSysStatusMsg_Old->systems_status[i].nodeVersion.appFlash.minor);
					pSysStatusMsg_Old->systems_status[i].nodeVersion.dataFlash.major = ntohs(pSysStatusMsg_Old->systems_status[i].nodeVersion.dataFlash.major);
					pSysStatusMsg_Old->systems_status[i].nodeVersion.dataFlash.minor = ntohs(pSysStatusMsg_Old->systems_status[i].nodeVersion.dataFlash.minor);
					pSysStatusMsg_Old->systems_status[i].nodeVersion.bootFlash.major = ntohs(pSysStatusMsg_Old->systems_status[i].nodeVersion.bootFlash.major);
					pSysStatusMsg_Old->systems_status[i].nodeVersion.bootFlash.minor = ntohs(pSysStatusMsg_Old->systems_status[i].nodeVersion.bootFlash.minor);
					pSysStatusMsg_Old->systems_status[i].error_flags = ntohl(pSysStatusMsg_Old->systems_status[i].error_flags);

					EvolWrapperNode node(pSysStatusMsg_Old->systems_status[i]);

					wrSysStatus.setNode(node, i);
				}
			}
			else
			{
				pSysStatusMsg = (WrSysStatus*) genMsg;

				Debug::trace("EvolWrapperCommManager.WrSysStatus","data: " + Hex::toString((char*)pSysStatusMsg, sizeof(WrSysStatus), true));

				wrSysStatus.setReportSysError(ntohs(pSysStatusMsg->report_sys_error));

				for (i = 0; i < EvolWrapperSystemStatus::_maxBoards_; ++i)
				{
					pSysStatusMsg->systems_status[i].nodeid = ntohs(pSysStatusMsg->systems_status[i].nodeid);
					HobString nodeIdStr = Integer::toString(pSysStatusMsg->systems_status[i].nodeid);

					pSysStatusMsg->systems_status[i].config_version = ntohs(pSysStatusMsg->systems_status[i].config_version);
					pSysStatusMsg->systems_status[i].device_init_flags = ntohs(pSysStatusMsg->systems_status[i].device_init_flags);
					pSysStatusMsg->systems_status[i].nodeVersion.appFlash.major = ntohs(pSysStatusMsg->systems_status[i].nodeVersion.appFlash.major);
					if (pSysStatusMsg->systems_status[i].nodeVersion.appFlash.major == 0xFFFF)
					{
						pSysStatusMsg->systems_status[i].nodeVersion.appFlash.major = 0;
						Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper:  WR_SYS_STATUS - Node " + nodeIdStr + " appFlash.major set to 0!");
					}
					pSysStatusMsg->systems_status[i].nodeVersion.appFlash.minor = ntohs(pSysStatusMsg->systems_status[i].nodeVersion.appFlash.minor);
					if (pSysStatusMsg->systems_status[i].nodeVersion.appFlash.minor == 0xFFFF)
					{
						pSysStatusMsg->systems_status[i].nodeVersion.appFlash.minor = 0;
						//Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper:  WR_SYS_STATUS - Node " + nodeIdStr + " appFlash.minor set to 0!");
					}
					pSysStatusMsg->systems_status[i].nodeVersion.dataFlash.major = ntohs(pSysStatusMsg->systems_status[i].nodeVersion.dataFlash.major);
					if (pSysStatusMsg->systems_status[i].nodeVersion.dataFlash.major == 0xFFFF)
					{
						pSysStatusMsg->systems_status[i].nodeVersion.dataFlash.major = 0;
						//Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper:  WR_SYS_STATUS - Node " + nodeIdStr + " dataFlash.major set to 0!");
					}
					pSysStatusMsg->systems_status[i].nodeVersion.dataFlash.minor = ntohs(pSysStatusMsg->systems_status[i].nodeVersion.dataFlash.minor);
					if (pSysStatusMsg->systems_status[i].nodeVersion.dataFlash.minor == 0xFFFF)
					{
						pSysStatusMsg->systems_status[i].nodeVersion.dataFlash.minor = 0;
						//Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper:  WR_SYS_STATUS - Node " + nodeIdStr + " dataFlash.minor set to 0!");
					}
					pSysStatusMsg->systems_status[i].nodeVersion.bootFlash.major = ntohs(pSysStatusMsg->systems_status[i].nodeVersion.bootFlash.major);
					if (pSysStatusMsg->systems_status[i].nodeVersion.bootFlash.major == 0xFFFF)
					{
						pSysStatusMsg->systems_status[i].nodeVersion.bootFlash.major = 0;
						//Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper:  WR_SYS_STATUS - Node " + nodeIdStr + " bootFlash.major set to 0!");
					}
					pSysStatusMsg->systems_status[i].nodeVersion.bootFlash.minor = ntohs(pSysStatusMsg->systems_status[i].nodeVersion.bootFlash.minor);
					if (pSysStatusMsg->systems_status[i].nodeVersion.bootFlash.minor == 0xFFFF)
					{
						pSysStatusMsg->systems_status[i].nodeVersion.bootFlash.minor = 0;
						//Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper:  WR_SYS_STATUS - Node " + nodeIdStr + " bootFlash.minor set to 0!");
					}
					pSysStatusMsg->systems_status[i].error_flags = ntohl(pSysStatusMsg->systems_status[i].error_flags);
					pSysStatusMsg->systems_status[i].wrap_box_checksum = ntohl(pSysStatusMsg->systems_status[i].wrap_box_checksum);

					EvolWrapperNode node(pSysStatusMsg->systems_status[i]);

					wrSysStatus.setNode(node, i);
				}
			}

			if (pCodeUpdater)
			{
				if (!pCodeUpdater->isUpdaterBusy())				
				{
					//Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper:  WR_SYS_STATUS - Calling pCodeUpdater->requestUpdate(wrSysStatus)");
					pCodeUpdater->requestUpdate(wrSysStatus);
				}
			}

			if (pMyWrapper)
			{
				const char *wSysStatusMsg = wrSysStatus.serialize();
				int mLength = wrSysStatus.getSerializedLength();

				HobString h = HobString("Serialized EvolWrapperSystemStatus: ") + HobString(Hex::toString(wSysStatusMsg, mLength));
				Debug::trace("EvolWrapperCommManager.Message", h);

				pMyWrapper->send(wSysStatusMsg, mLength);
			}
		}
		break;
		case WR_PKG_SETTINGS_READ:
		{
			Debug::trace("EvolWrapperCommManager.Message", "START EvolWrapCommMgr USB Wrapper: case WR_PKG_SETTINGS_READ");

			bool traceOnTimings = Debug::isTracing("EvolWrapperCommManager.PackageTimings");
			EvolWrPackageTimingsRead* timingsMessage = (EvolWrPackageTimingsRead*) genMsg;

			EvolWrapperPackageTimings *wrapperTimings = new EvolWrapperPackageTimings(this);

			FpmManager* pSerialFlash = FpmManager::getInstance();		// jhc added from 4.2.0_5.2.0
			
			UINT16 heightBoxMemSize = USE_1P_STRUCTURE_SIZE;
			if (pSerialFlash->isWrapperTwoPrinter())
				heightBoxMemSize = USE_2P_STRUCTURE_SIZE;

			if (!wrapperTimings)
			{
					Debug::trace("EvolWrapperCommManager.Message", "wrapperTimings is null!!  Exiting!");
				break;
			}

			int numWrapBoxes = ntohs(timingsMessage->wrapBoxes);
			int bufCounter = sizeof(EvolWrPackageTimingsRead);		// Skip event and # wrap Boxes

			//Debug::trace("EvolWrapperCommManager.Message", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ numWrapBoxes: " + Integer::toString( numWrapBoxes ) );
			//Debug::trace("EvolWrapperCommManager.Message", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ bufCounter: " + Integer::toString( bufCounter ) );

			wrapperTimings->setDisposition(ntohs(timingsMessage->disposition));

			// Read the number of height boxes for each wrap box
			std::vector<int> wrapBoxHeights(numWrapBoxes);
			for ( int i = 0; i < numWrapBoxes; ++i )
			{
				wrapBoxHeights[i] = ntohs(*(UINT16 *)((char*)genMsg + bufCounter));
				//Debug::trace("EvolWrapperCommManager.Message", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ wrapBoxHeights[i]: " + Integer::toString(wrapBoxHeights[i]) );

				bufCounter += sizeof(UINT16);
			}

			// Read each wrap box
			std::vector<WrapBox> wrapBoxes(numWrapBoxes);
			for ( int i = 0; i < numWrapBoxes; ++i )
			{
				char boxName[WrapBox::_MaxNameLength_ + 1];
				EvolWrWrapBox* wrapBox = (EvolWrWrapBox*)((char*)genMsg + bufCounter);

				// Copy name
				unsigned int j = 0;
				for (; j < WrapBox::_MaxNameLength_; ++j)
					boxName[j] = (char)ntohs(wrapBox->name[j]);

				// Remove trailing spaces
				int x = WrapBox::_MaxNameLength_ - 1;
				for ( ; x >= 0; --x )
					if ( boxName[x] != ' ' )
						break;
				boxName[x + 1] = '\0';

				// Add the wrap box to the container
				wrapBoxes[i] = WrapBox(boxName, ((ntohs(wrapBox->pluNumHigh) << 16) | ntohs(wrapBox->pluNumLow)),
						ntohs(wrapBox->lengthLow), ntohs(wrapBox->lengthHigh),
						ntohs(wrapBox->widthLow), ntohs(wrapBox->widthHigh), ntohs(wrapBox->lipHeight));

				if (traceOnTimings)
					wrapBoxes[i].show("EWCMTimings.Joe");

				bufCounter += sizeof(EvolWrWrapBox);
			}
			
			//Debug::trace("EvolWrapperCommManager.Message", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ numWrapBoxes: " + Integer::toString(numWrapBoxes));

			bool traceForClare = Debug::isTracing("ClareTest");
			HobString debugClare;

			// Read each height box
			for ( int i = 0; i < numWrapBoxes; ++i )
			{
				WrapBox& wrapBox = wrapBoxes[i];

				//Debug::trace("EvolWrapperCommManager.Message", "READ #4 receivedWrapperDevMessage - inside outer for loop - sizeOf EvolWrHeightBox: " + Integer::toString(sizeof(EvolWrHeightBox)));
				//Debug::trace("EvolWrapperCommManager.Message", "READ #4 receivedWrapperDevMessage - inside outer for loop - valueOf heightBoxMemSize: " + Integer::toString( heightBoxMemSize ) );			// SOF-6077 not the sizeOf, but the valueOf now

				// For each height box in each wrap box ...
				for ( int j = 0; j < wrapBoxHeights[i]; ++j )
				{
					EvolWrHeightBox* wrapperHeightBox = (EvolWrHeightBox*)((char*)genMsg + bufCounter);
					WrapBox::HeightBox heightBox (ntohs(wrapperHeightBox->height));

					heightBox.setParameter( 0, ntohs(wrapperHeightBox->percentStretch));
					heightBox.setParameter( 1, ntohs(wrapperHeightBox->knifeFireHeight));
					heightBox.setParameter( 2, ntohs(wrapperHeightBox->clampOpenHeight));
					heightBox.setParameter( 3, ntohs(wrapperHeightBox->elevatorPlacement));
					heightBox.setParameter( 4, ntohs(wrapperHeightBox->addTrayLength));
					/*
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ heightBox " + Integer::toString(i)  + ": " + Integer::toString(ntohs(wrapperHeightBox->height)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[0] percentStretch: " + Integer::toString(ntohs(wrapperHeightBox->percentStretch)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[1] knifeFireHeight: " + Integer::toString(ntohs(wrapperHeightBox->knifeFireHeight)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[2] clampOpenHeight: " + Integer::toString(ntohs(wrapperHeightBox->clampOpenHeight)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[3] elevatorPlacement: " + Integer::toString(ntohs(wrapperHeightBox->elevatorPlacement)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[4] addTrayLength: " + Integer::toString(ntohs(wrapperHeightBox->addTrayLength)));
					*/
					value1 = ntohs(wrapperHeightBox->gripperPullAdjustment);
					value2 = value1;
					if ((value1 & 0x08000) == 0x08000)
						value2 = -((value1 ^ 0x0FFFF) + 1);
					if (traceForClare)
						Debug::trace("ClareTest", HobString("gripperPullAdjustment = ") + Integer::toString(value2));
					heightBox.setParameter( 5, value2);
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[5] gripperPullAdjustment: " + Integer::toString(value1));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[5] gripperPullAdjustment: " + Integer::toString(value2));

					value1 = ntohs(wrapperHeightBox->frontUnderfolderAdjustment);
					value2 = value1;
					if ((value1 & 0x08000))
						value2 = -((value1 ^ 0x0FFFF) + 1);

					if (traceForClare)
						Debug::trace("ClareTest", HobString("frontUnderfolderAdjustment = ") + Integer::toString(value2));
					
                    heightBox.setParameter( 6, value2);
					heightBox.setParameter( 7, ntohs(wrapperHeightBox->wrapSpeed));
					heightBox.setParameter( 8, ntohs(wrapperHeightBox->dischargeSpeed));
					
					value1 = ntohs(wrapperHeightBox->sideUnderfolderAdjustment);
					value2 = value1;
					if ((value1 & 0x08000))
						value2 = -((value1 ^ 0x0FFFF) + 1);

					if (traceForClare)
						Debug::trace("ClareTest", HobString("sideUnderfolderAdjustment = ") + Integer::toString(value2));

					heightBox.setParameter( 9, value2);
					//Debug::trace("EvolWrapperCommManager.Message", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[9] sideUnderfolderAdjustment: " + Integer::toString(value1));
					//Debug::trace("EvolWrapperCommManager.Message", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[9] sideUnderfolderAdjustment: " + Integer::toString(value2));

					heightBox.setParameter(10, ntohs(wrapperHeightBox->heightRestriction));
					heightBox.setParameter(11, ntohs(wrapperHeightBox->sideUnderfolderReturnHome));
					heightBox.setParameter(12, ntohs(wrapperHeightBox->gripperAllowShortPull));
					heightBox.setParameter(13, ntohs(wrapperHeightBox->sideClampGap));
					heightBox.setParameter(14, ntohs(wrapperHeightBox->filmRollToUse));
					heightBox.setParameter(15, ntohs(wrapperHeightBox->rearUnderfolderTripDelay));
					heightBox.setParameter(16, ntohs(wrapperHeightBox->brakeTiming));
					heightBox.setParameter(17, ntohs(wrapperHeightBox->outsideGripperRelease));
					heightBox.setParameter(18, ntohs(wrapperHeightBox->insideGripperRelease));
					
                    /*
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[10] heightRestriction: " + Integer::toString(ntohs(wrapperHeightBox->heightRestriction)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[11] sideUnderfolderReturnHome: " + Integer::toString(ntohs(wrapperHeightBox->sideUnderfolderReturnHome)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[12] gripperAllowShortPull: " + Integer::toString(ntohs(wrapperHeightBox->gripperAllowShortPull)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[13] sideClampGap: " + Integer::toString(ntohs(wrapperHeightBox->sideClampGap)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[14] filmRollToUse: " + Integer::toString(ntohs(wrapperHeightBox->filmRollToUse)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[15] rearUnderfolderTripDelay: " + Integer::toString(ntohs(wrapperHeightBox->rearUnderfolderTripDelay)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[16] brakeTiming: " + Integer::toString(ntohs(wrapperHeightBox->brakeTiming)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[17] outsideGripperRelease: " + Integer::toString(ntohs(wrapperHeightBox->outsideGripperRelease)));
					Debug::trace("EWCMTimings.Joe", "receiveWrapperDevMessage() case WR_PKG_SETTINGS_READ box[18] insideGripperRelease: " + Integer::toString(ntohs(wrapperHeightBox->insideGripperRelease)));

					Debug::trace("EWCMTimings.Joe", "WRAPCommMgr::receivedWrapperDevMessage() case WR_PKG_SETTINGS_READ skipped Params 19 and 20. Bug?  Wouldnt this fail on a 2P? ");
                    */

					if (traceOnTimings)
						heightBox.show("EWCMTimings.Joe");

					wrapBox.insertHeightBox(heightBox);

					//Debug::trace("EvolWrapperCommManager.Message", "READ #5 receivedWrapperDevMessage - inside inner for loop - sizeOf EvolWrHeightBox: " + Integer::toString(sizeof(EvolWrHeightBox)));
					//Debug::trace("EvolWrapperCommManager.Message", "READ #5 receivedWrapperDevMessage - inside inner for loop - valueOf heightBoxMemSize: " + Integer::toString( heightBoxMemSize) );	// SOF-6077 not the sizeOf, but the valueOf now
																// SOF-6077 not the sizeOf anymore, but the valueOf now
					bufCounter += heightBoxMemSize ;			// SOF 6077 - bufCounter += sizeof(EvolWrHeightBox) - see EvolWrHeightBox structure to see why this was done;
				}
			}

			Debug::trace("EvolWrapperCommManager.Message", "Going to call addWrapBox() for each wrap box.");

			// Store each wrap box
			for ( int i = 0; i < numWrapBoxes; ++i )
				wrapperTimings->addWrapBox(wrapBoxes[i]);

				Debug::trace("EvolWrapperCommManager.Message", "Ready to send the EvolWrapperPackageTimings pointer to the WrapperManager.");
			if (pMyWrapper)
			{
				EvolWrapperPackageTimingsPointer wptp(WrEvolPackageTimings, wrapperTimings);
				const char *pMsg = wptp.serialize();
				int mLength = wptp.getSerializedLength();

				pMyWrapper->send(pMsg, mLength);

					Debug::trace("EvolWrapperCommManager.Message", "WrEvolPackageTimings msg was sent");
			}
			else
			{
					Debug::trace("EvolWrapperCommManager.Message", "Can't send the EvolWrapperPackageTimings msg.  pMyWrapper is NULL!!!");
			}

			Debug::trace("EvolWrapperCommManager.Message", "END CASE Params EvolWRAPCommMgr::receivedWrapperDevMessage() case WR_PKG_SETTINGS_READ(164): ");

		}
		break;

		case WR_PKG_SETTINGS_WRITE_COMPLETE:
		{
			Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: WrEvolSavePackageTimingsComplete");
			if (pMyWrapper)
			{
				WrapperGeneric wsptc(WrEvolSavePackageTimingsComplete, this);
				const char *pMsg = wsptc.serialize();
				int mLength = wsptc.getSerializedLength();

				pMyWrapper->send(pMsg, mLength);

				Debug::trace("EvolWrapperCommManager.Message", "WrEvolSavePackageTimingsComplete msg was sent");
			}
			else
			{
				Debug::trace("EvolWrapperCommManager.Message", "Can't send the WrEvolSavePackageTimingsComplete msg.  pMyWrapper is NULL!!!");
			}
		}
		break;

		case WR_TEST_COMPLETE:		  
		{
			Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: WrEvolTestComplete");

			EvolWrTestComplete *testMsg = (EvolWrTestComplete *) genMsg;
			if (traceOn)
			{
				HobString m((const char *) genMsg, sizeof(EvolWrTestComplete));
				HobString h = HobString("EvolWrTestComplete message received: ") + HobString(Hex::toString(m.data(), (int)m.size()));
				Debug::trace("EvolWrapperCommManager.Message", h);
			}

			EvolWrapperTestComplete wrTestComplete(this); // use default constructor
			wrTestComplete.setTestCode((EvolWrapperTest::WrEvolTestCode) ntohs(testMsg->testNum));
			wrTestComplete.setData1(ntohs(testMsg->parameter.dataInt.data1));
			wrTestComplete.setData2(ntohs(testMsg->parameter.dataInt.data2));
			wrTestComplete.setTestPassed( ((ntohs((UINT16)testMsg->passed) != 0) ? true : false) );

			if (pMyWrapper)
			{
				const char *pTestMsg = wrTestComplete.serialize();
				int mLength = wrTestComplete.getSerializedLength();

				pMyWrapper->send(pTestMsg, mLength);
			}
			break;
		}

		case WR_SENSE:
		{
			Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: WrEvolSensors");
			EvolWrSensors *sensorMsg = (EvolWrSensors *) genMsg;

			EvolWrapperSensors wrSensors(this);


			EvolWrapperGeneralSensors &genSensors = wrSensors.getGeneralSensors();
			genSensors.setStatus(ntohl(sensorMsg->switchesSensors.field));

			//	EvolWrapperInterlockSensors
			EvolWrapperInterlockSensors &interlockSensors = wrSensors.getInterlockSensors();
			interlockSensors.setStatus(ntohs(sensorMsg->interlockSwitches.field));

			EvolWrapperGeneralSensors_2 &genSensors_2 = wrSensors.getGeneralSensors_2();
			genSensors_2.setStatus(ntohs(sensorMsg->switchesSensors_2.field));

			Debug::trace("EvolWrapperCommManager.Message", "Before the Serialize:");
			wrSensors.show();

			if (pMyWrapper)
			{
				const char *wSensorsMsg = wrSensors.serialize();
				int mLength = wrSensors.getSerializedLength();

				HobString h = HobString("Serialized EvolWrapperSensors: ") + HobString(Hex::toString(wSensorsMsg, mLength));
				Debug::trace("EvolWrapperCommManager.Message", h);

				pMyWrapper->send(wSensorsMsg, mLength);
			}

			Debug::trace("EvolWrapperCommManager.Message", "********************** case WR_SENSE start ******************************************************");
			wrSensors.show();
			Debug::trace("EvolWrapperCommManager.Message", "********************** case WR_SENSE end ********************************************************");
			break;
		}

		case WR_PACKAGE_DESC:
		{
			Debug::trace("EvolWrapperCommManager.Message", "START EvolWrapCommMgr::receivedWrapperDevMessage() case WR_PACKAGE_DESC ");
			Debug::trace("EvolWrapperCommManager.Message", "case WR_PACKAGE_DESC, mySystemType: " + mySystemType);

			if( ( mySystemType != "AWS3" ) && (  mySystemType != "NGW1" ) && (  mySystemType != "NGW2" ) ) {
				Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: case WR_PACKAGE_DESC NOT AWS3, NOT NGW1, NOT NGW2");
				EvolWrPackageDesc *pkgDescMsg = (EvolWrPackageDesc *) genMsg;

				EvolWrapperPackageDesc wrPkgDesc(this);
				wrPkgDesc.setId(ntohs(pkgDescMsg->id));
				wrPkgDesc.setLength(ntohs(pkgDescMsg->length));
				wrPkgDesc.setWidth(ntohs(pkgDescMsg->width));
				wrPkgDesc.setHeight(ntohs(pkgDescMsg->height));
				wrPkgDesc.setLip(ntohs(pkgDescMsg->lip));

				Debug::trace("EvolWrapperCommManager.Message", "case WR_PACKAGE_DESC, Id: " + inttostring(wrPkgDesc.getId() ) );
				Debug::trace("EvolWrapperCommManager.Message", "case WR_PACKAGE_DESC, Length: " + inttostring(wrPkgDesc.getLength()));
				Debug::trace("EvolWrapperCommManager.Message", "case WR_PACKAGE_DESC, Width: " + inttostring(wrPkgDesc.getWidth()));
				Debug::trace("EvolWrapperCommManager.Message", "case WR_PACKAGE_DESC, Height: " + inttostring(wrPkgDesc.getId()));
				Debug::trace("EvolWrapperCommManager.Message", "case WR_PACKAGE_DESC, Lip: " + inttostring(wrPkgDesc.getId()));

	#ifdef NEW_FILM_WIDTH_VAR
				wrPkgDesc.setFilmWidthSelection((WrFilmWidthSelection) ntohs(pkgDescMsg->filmWidthUsedToWrap));
	#else
				wrPkgDesc.setFilmWidthUsedToWrap(ntohs(pkgDescMsg->filmWidthUsedToWrap));
	#endif

				wrPkgDesc.setWrapBoxIndex(ntohs(pkgDescMsg->wrapBoxIndex));
				wrPkgDesc.setHeightBoxIndex(ntohs(pkgDescMsg->heightBoxIndex));
				wrPkgDesc.setUsedWrapBoxBestFit( ((ntohs(pkgDescMsg->usedWrapBoxBestFit) != 0) ? true : false) );
				wrPkgDesc.setModifiedNetStretch( ((ntohs(pkgDescMsg->modifiedNetStretch)  != 0) ? true : false) );

				if (pMyWrapper)
				{
					const char *wPkgDescMsg = wrPkgDesc.serialize();
					int mLength = wrPkgDesc.getSerializedLength();

					HobString h = HobString("Serialized EvolWrapperPackageDesc: ") + HobString(Hex::toString(wPkgDescMsg, mLength));
					Debug::trace("EvolWrapperCommManager.Message", h);

					pMyWrapper->send(wPkgDescMsg, mLength);
				}
			}
			else {
				Debug::trace("EvolWrapperCommManager.Message", "WrapperOpen: AWS3 system type! Do not process this USB message." );
			}
			break;
		}

		case CAMERA_SCAN:
		{
			Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: EvolWrCameraScan");

			EvolWrCameraScan *cameraMsg = (EvolWrCameraScan *) genMsg;
			HobString m((const char *) genMsg, sizeof(EvolWrCameraScan));
			HobString h = HobString("EvolWrCameraScan message received: ") + HobString(Hex::toString(m.data(), (int)m.size()));
			Debug::trace("EvolWrapperCommManager.Message", h);

			EvolWrapperCameraScan wrCameraScan(this); // use default constructor

			EvolWrapperTest::WrEvolCameraTestParam whichCamera = (EvolWrapperTest::WrEvolCameraTestParam)  ntohs(cameraMsg->camera);
			if (whichCamera == EvolWrapperTest::RightCameraScanDigital)
			{
				wrCameraScan.setCamera((RightCamera));
				wrCameraScan.setImageType(DigitalImage);
			}
			else if (whichCamera == EvolWrapperTest::RightCameraScanAnalog)
			{
				wrCameraScan.setCamera((RightCamera));
				wrCameraScan.setImageType(AnalogImage);
			}
			else if (whichCamera == EvolWrapperTest::LeftCameraScanDigital)
			{
				wrCameraScan.setCamera((LeftCamera));
				wrCameraScan.setImageType(DigitalImage);
			}
			else if (whichCamera == EvolWrapperTest::LeftCameraScanAnalog)
			{
				wrCameraScan.setCamera((LeftCamera));
				wrCameraScan.setImageType(AnalogImage);
			}

			wrCameraScan.setCameraImage(cameraMsg->cameraImage);

			if (pMyWrapper)
			{
				const char *pCameraScanMsg = wrCameraScan.serialize();
				int mLength = wrCameraScan.getSerializedLength();

				pMyWrapper->send(pCameraScanMsg, mLength);
			}

			Debug::trace("EvolWrapperCommManager.Message", "********************** case CAMERA_SCAN start *********************************");
			wrCameraScan.show();
			Debug::trace("EvolWrapperCommManager.Message", "********************** case CAMERA_SCAN end ***********************************");
			break;
		}

		case CAMERA_HEIGHT_SCAN:				
		{

			Debug::trace("EvolWrapperCommManager.Message", "USB Wrapper: EvolWrCameraHeightScan");

			EvolWrCameraHeightScan *cameraMsg = (EvolWrCameraHeightScan *) genMsg;

			EvolWrapperCameraHeightScan wrCameraHeightScan(this);
			wrCameraHeightScan.setScan(ntohs(cameraMsg->cameraHeightScan));

			if (pMyWrapper)
			{
				const char *wrScanMsg = wrCameraHeightScan.serialize();
				int mLength = wrCameraHeightScan.getSerializedLength();
				pMyWrapper->send(wrScanMsg, mLength);
			}
			break;
		}
		case CAMERA_CALIBRATION:				
		{
			EvolWrCameraCal *cameraMsg = (EvolWrCameraCal *) genMsg;

			EvolWrapperCameraCalibration wrCameraCalibration(this);
			wrCameraCalibration.setCamera((WrapperCamera) ntohs(cameraMsg->camera));
			wrCameraCalibration.setResult( ((ntohs(cameraMsg->calResult) != 0) ? true : false) );
			wrCameraCalibration.setScaleFactor(ntohs(cameraMsg->scaleFactor));
			wrCameraCalibration.setFirstPixel(ntohs(cameraMsg->firstPixel));
			wrCameraCalibration.setLastPixel(ntohs(cameraMsg->lastPixel));

			wrCameraCalibration.show();

			if (pMyWrapper)
			{
				const char *wrCalMsg = wrCameraCalibration.serialize();
				int mLength = wrCameraCalibration.getSerializedLength();

				pMyWrapper->send(wrCalMsg, mLength);
			}
			break;
		}

		case WR_DIAGNOSTICS_TEST_RESULT:
		{
			EvolWrDiagnosticsTestResult *pEvolMsg( (EvolWrDiagnosticsTestResult *) genMsg );

			EvolWrapperDiagnosticsTestResult diagTestResult( this );
			diagTestResult.setTest( ntohs( pEvolMsg->test ) );
			diagTestResult.setTestResult( ntohs( pEvolMsg->testResult ) ? true : false );
			diagTestResult.setOpenLoopOverride(	ntohs( pEvolMsg->openLoopOverride ) ? true : false );
			diagTestResult.setDiag( ntohs( pEvolMsg->diagRoutine ) );
			diagTestResult.setDiagExpectedResult( ntohs( pEvolMsg->diagRoutineExpectedResult ) );
			diagTestResult.setDiagActualResult( ntohl( pEvolMsg->diagRoutineActualResult ) );
			diagTestResult.setErrorCode( ntohs( pEvolMsg->errorCode ) );

			if (pMyWrapper)
			{
				const char *pMsg( diagTestResult.serialize() );
				int mLength( diagTestResult.getSerializedLength() );

				pMyWrapper->send(pMsg, mLength);
			}

			break;
		}

		case WR_EVENT_CAPTURE:
		{
			Debug::trace("EvolWrapperCommManager.Message", "Event Capture Received");
			const EvolWrEventCapture *rawMsg( (EvolWrEventCapture*) genMsg );

			EvolWrapperEventCapture msg(this);
			msg.setEventCaptureType((WrEventCaptureType) ntohs(rawMsg->eventCaptureType));
			msg.setPackageId(ntohs(rawMsg->packageId));
			msg.setValue(ntohl(rawMsg->eventValue));
			msg.setErrorNumber(ntohs(rawMsg->errorNum));

			Debug::trace("EvolWrapperCommManager.Message", HobString("event capture type = ") + Integer::toString(msg.getEventCaptureType()));
			Debug::trace("EvolWrapperCommManager.Message", HobString("event pkg id = ") + Integer::toString(msg.getPackageId()));
			Debug::trace("EvolWrapperCommManager.Message", HobString("event value = ") + Long::toString(msg.getValue()));
			Debug::trace("EvolWrapperCommManager.Message", HobString("event error num = ") + Long::toString(msg.getErrorNumber()));

			if (pMyWrapper)
			{
				const char *pEvolKnifeTimeMsg( msg.serialize() );
				int mLength( msg.getSerializedLength() );

				pMyWrapper->send(pEvolKnifeTimeMsg, mLength);
			}
			break;
		}
		case WR_ERR_HIS:
		{
			EvolWrErrorHistory *pEvolWrErrorHistory = (EvolWrErrorHistory *) genMsg;

			EvolWrapperErrorHistory wrErrorHistory(this);

			wrErrorHistory.setWrapperError(ntohs(pEvolWrErrorHistory->wrError));
			wrErrorHistory.setPackageLength(ntohs(pEvolWrErrorHistory->packageLength));
			wrErrorHistory.setPackageWidth(ntohs(pEvolWrErrorHistory->packageWidth));
			wrErrorHistory.setPackageHeight(ntohs(pEvolWrErrorHistory->packageHeight));
			wrErrorHistory.setFilmUsed((WrFilmWidthSelection) ntohs(pEvolWrErrorHistory->filmRollUsed));
			wrErrorHistory.setUsedBestFit( ((ntohs(pEvolWrErrorHistory->usedWrapBoxBestFit) != 0) ? true : false) );
			wrErrorHistory.setStretchModified( ((ntohs(pEvolWrErrorHistory->modifiedNetStretch) != 0) ? true : false) );

			std::string Trayname = "";					  
			for(unsigned int i = 1; i < ((EvolWrapperErrorHistory::_MaxNameLength_ ) *2); i += 2)
			{
				Trayname.append(1, pEvolWrErrorHistory->trayName[i]);
			}
			wrErrorHistory.setTrayName(Trayname);


			if (pMyWrapper)
			{
				const char *pErrorHistoryMsg = wrErrorHistory.serialize();
				int mLength = wrErrorHistory.getSerializedLength();

				HobString h = HobString("Serialized EvolWrapperErrorHistory: ") + HobString(Hex::toString(pErrorHistoryMsg, mLength));
				Debug::trace("EvolWrapperCommManager.Message", h);

				pMyWrapper->send(pErrorHistoryMsg, mLength);
			}
			break;
		} 
		case WR_PKG_SENSOR_STATUS: 
		{
#if 0	//add back when detector firmware changes for reporting status is done. -- ats 
			EvolWrPkgSensorStatus *pEvolWrPkgSensorStatus = (EvolWrPkgSensorStatus *) genMsg;

			EvolWrapperPkgDetectorStatus wrPkgDetectorStatus(this);

			wrPkgDetectorStatus.setSensorLocation(ntohs(pEvolWrPkgSensorStatus->sensorType));
			wrPkgDetectorStatus.setDetectorType(ntohs(pEvolWrPkgSensorStatus->detectorType));
			wrPkgDetectorStatus.setEmitterPower(ntohs(pEvolWrPkgSensorStatus->emitterPowerLevel));
			wrPkgDetectorStatus.setIsOldDetector( ((ntohs(pEvolWrPkgSensorStatus->isOldRev) != 0) ? true : false) );
			wrPkgDetectorStatus.setSensorOperational( ((ntohs(pEvolWrPkgSensorStatus->sensorInError) != 0) ? true : false));						

			if (pMyWrapper)
			{
				const char *pPkgDetectorStatus = wrPkgDetectorStatus.serialize();
				int mLength = wrPkgDetectorStatus.getSerializedLength();

				HobString h = HobString("Serialized EvolWrapperPkgDetectorStatus: ") + HobString(Hex::toString(pPkgDetectorStatus, mLength));
				Debug::trace("EvolWrapperCommManager.Message", h);

				pMyWrapper->send(pPkgDetectorStatus, mLength);
			}
#endif

			break;
		}
		case WR_WRAPPER_CYCLE_COUNT:
		{
			EvolWrCycleCount *pEvolWrCycleCount = (EvolWrCycleCount *) genMsg;
			EvolWrapperCycleCount evolWrapperCycleCount(this, ntohl(pEvolWrCycleCount->cycleCount));

			const char *pEvolMsg = evolWrapperCycleCount.serialize();
			int mLength = evolWrapperCycleCount.getSerializedLength();

			if (pMyWrapper)
			{
				pMyWrapper->send(pEvolMsg, mLength);
			}

			break;
		}
		case WR_CAMERA_THRESHOLD:
		{
			Debug::trace("EvolWrapperCommManager.Message", "Wrapper Camera Threshold message");

			EvolWrCameraThreshold *pEvolWrCameraThreshold = (EvolWrCameraThreshold *) genMsg;
			UCHAR threshold = (UCHAR)(pEvolWrCameraThreshold->cameraThreshold >> 8);

			EvolWrapperCameraThreshold evolWrapperCameraThreshold(this, threshold);

			const char *pEvolMsg = evolWrapperCameraThreshold.serialize();
			int mLength = evolWrapperCameraThreshold.getSerializedLength();

			if (pMyWrapper)
			{
				pMyWrapper->send(pEvolMsg, mLength);
			}

			break;
		}
		case WR_TRANSFER_STATION_DISCHARGING:
		{
			EvolWrGenericOneParam *pkgDischargeMsg = (EvolWrGenericOneParam *) genMsg;

			EvolWrapperPackageDischarge ewpd(this);
			ewpd.setPackId(ntohs(pkgDischargeMsg->param));

			if (pMyWrapper)
			{
				const char *pEvolMsg = ewpd.serialize();
				int mLength = ewpd.getSerializedLength();

				pMyWrapper->send(pEvolMsg, mLength);
			}

			break;
		}
		case WR_SYSTEM_INFO:
		{
				Debug::trace("EvolWrapperCommManager.Message", "Wrapper System Info message");
			EvolWrSystemInfo *pEvolWrSystemInfo = (EvolWrSystemInfo *) genMsg;
			EvolWrapperSystemInfo	evolWrapperSystemInfo(this);

			evolWrapperSystemInfo.setBoardChecksumStatus( ((ntohs(pEvolWrSystemInfo->validBoardChecksum)  != 0) ? true : false) );
			evolWrapperSystemInfo.setAwsChecksumStatus( ((ntohs(pEvolWrSystemInfo->validAwsChecksum)  != 0) ? true : false) );
			evolWrapperSystemInfo.setModelNumber(ntohs(pEvolWrSystemInfo->awsModelNumber));
			evolWrapperSystemInfo.setAwsCycleCount(ntohl(pEvolWrSystemInfo->awsCycleCount));

			std::string partNumber = "";					
			for(unsigned int i = 0; i < EvolWrapperSystemInfo::_PartNumbSize_ ; i += 2)
			{
				partNumber.append(1, pEvolWrSystemInfo->motorBoardPartNumber[i + 1]);
				partNumber.append(1, pEvolWrSystemInfo->motorBoardPartNumber[i]);
			}

			partNumber.append(1, pEvolWrSystemInfo->motorBoardPartNumber[EvolWrapperSystemInfo::_PartNumbSize_ + 2]);

			evolWrapperSystemInfo.setMotorBoardPartNumber(partNumber);

			std::string revision = "";					  
			for(unsigned int i = 0; i < EvolWrapperSystemInfo::_RevisionSize_; i += 2)
			{
				revision.append(1, pEvolWrSystemInfo->motorBoardRev[i + 1]);
				revision.append(1, pEvolWrSystemInfo->motorBoardRev[i]);
			}

			evolWrapperSystemInfo.setMotorBoardRevision(revision);

			std::string date = "";				  
			for(unsigned int i = 0; i < EvolWrapperSystemInfo::_DateSize_; i += 2)
			{
				date.append(1, pEvolWrSystemInfo->motorBoardDateOfManufacture[i + 1]);
				date.append(1, pEvolWrSystemInfo->motorBoardDateOfManufacture[i]);
			}

			evolWrapperSystemInfo.setMotorBoardDateOfManufacture(date);

			std::string serialNumber = "";				  
			for(unsigned int i = 0; i < EvolWrapperSystemInfo::_SerialNumbSize_; i += 2)
			{
				serialNumber.append(1, pEvolWrSystemInfo->awsSerialNumber[i + 1]);
				serialNumber.append(1, pEvolWrSystemInfo->awsSerialNumber[i]);
			}
			serialNumber.append(1, pEvolWrSystemInfo->awsSerialNumber[EvolWrapperSystemInfo::_SerialNumbSize_ + 2]);

			evolWrapperSystemInfo.setAwsSerialNumber(serialNumber);

			std::string awsDate = "";
			for(unsigned int i = 0; i < EvolWrapperSystemInfo::_DateSize_; i += 2)
			{
				awsDate.append(1, pEvolWrSystemInfo->awsDateOfManufacture[i + 1]);
				awsDate.append(1, pEvolWrSystemInfo->awsDateOfManufacture[i]);
			}

			evolWrapperSystemInfo.setAwsDateOfManufacture(awsDate);

			std::string dateInstall = "";
			for(unsigned int i = 0; i < EvolWrapperSystemInfo::_DateSize_; i += 2)
			{
				dateInstall.append(1, pEvolWrSystemInfo->awsDateOfInstall[i + 1]);
				dateInstall.append(1, pEvolWrSystemInfo->awsDateOfInstall[i]);
			}

			evolWrapperSystemInfo.setAwsDateOfInstall(dateInstall);

			Debug::trace("EvolWrapperCommManager.Message", "WR_SYSTEM_INFO  -  board revision: " + revision);
			Debug::trace("EvolWrapperCommManager.Message", "WR_SYSTEM_INFO  -  modelNumber blank?: " +  HobString(Bool::toString(evolWrapperSystemInfo.isModelNumberBlank())));
			//if the board is a combo board and the model field is null then request the tech to supply the model type.
			if((evolWrapperSystemInfo.isBoardCombo()) && (evolWrapperSystemInfo.isModelNumberBlank()))
			{
				Debug::trace("EvolWrapperCommManager.Message", "WR_SYSTEM_INFO  -  Combo board with blank model number found!!!");
				EvolWrapperReqUIModel evolWrapperReqUIModel;
				
				const char *pEvolReqMsg = evolWrapperReqUIModel.serialize();
				int length = evolWrapperReqUIModel.getSerializedLength();

				if (pMyWrapper)
				{
					pMyWrapper->send(pEvolReqMsg, length);
				}
				else
				{
					Debug::trace("EvolWrapperCommManager.Error", "WR_SYSTEM_INFO  -  Don't have a WrapperManager yet to request the model!!!");
				}
			}
			else
			{
				if(pCodeUpdater)
				{
					if(!pCodeUpdater->isUpdaterBusy())
					{
						EWSWrapperType_ = evolWrapperSystemInfo.isEWSModelType();

						if(EWSWrapperType_)
						{
							Debug::trace("EvolWrapperCommManager.Message", "WR_SYSTEM_INFO  -  sendUpdateRequest():  EWS wrapper Model type!!");
							pCodeUpdater->sendUpdateRequest(1);
						}
						else
						{
							Debug::trace("EvolWrapperCommManager.Message", "WR_SYSTEM_INFO  -  sendUpdateRequest():  AWS wrapper Model type!!");
							pCodeUpdater->sendUpdateRequest(0);
						}

						//pCodeUpdater->sendUpdateRequest(EWSWrapperType_);
					}
					else
					{
						Debug::trace("EvolWrapperCommManager.Message", "WR_SYSTEM_INFO  -  Updater is Busy!!!!");
					}
				}
				else
				{
					Debug::trace("EvolWrapperCommManager.Message", "WR_SYSTEM_INFO is NULL");
				}
			}

			const char *pEvolMsg = evolWrapperSystemInfo.serialize();
			int mLength = evolWrapperSystemInfo.getSerializedLength();

			if (pMyWrapper)
			{
				pMyWrapper->send(pEvolMsg, mLength);
			}
			else
			{
				Debug::trace("EvolWrapperCommManager.Error", "WR_SYSTEM_INFO  -  Don't have a WrapperManager yet to send the WrapperSystemInfo!!!");
			}
			break;
		}
		case WR_MANUFACTURING_RUNIN_NOTICE:
		{
			Debug::trace("EvolWrapperCommManager.Message", "Wrapper Manufacturing Run In Notice");
			WrapperGeneric mfgRunInNotice(WrEvolManufacturingRunInNotice, this);

			const char *pEvolMsg = mfgRunInNotice.serialize();
			int mLength = mfgRunInNotice.getSerializedLength();

			if (pMyWrapper)
			{
				pMyWrapper->send(pEvolMsg, mLength);
				Debug::trace("EvolWrapperCommManager.Message", "WrManufacturingRunInNotice message sent");
			}

			break;
		}
		case WR_MOTOR_PROFILE:
		{
			Debug::trace("EvolWrapperCommManager.Message", "Wrapper Motor Profile Message");
			EvolWrMotorProfile *pEvolWrMotorProfile = (EvolWrMotorProfile *) genMsg;
			EvolWrapperMotorProfile evolWrapperMotorProfile(this);

			evolWrapperMotorProfile.setBldcMechanism(ntohs(pEvolWrMotorProfile->bldcMechanism));
			evolWrapperMotorProfile.setBldcMotorStatus(ntohs(pEvolWrMotorProfile->motorStatus));
			evolWrapperMotorProfile.setBldcMotorCapacityUsage(ntohs(pEvolWrMotorProfile->motorCapacityUsage));

			const char *pEvolMsg = evolWrapperMotorProfile.serialize();
			int mLength = evolWrapperMotorProfile.getSerializedLength();

			if (pMyWrapper)
			{
				pMyWrapper->send(pEvolMsg, mLength);
				Debug::trace("EvolWrapperCommManager.Message", "WrMotorProfile message sent");
			}

			break;
		}
		case WR_LOADER_STATUS:
		{
			HobString msgDebug;

			Debug::trace("EvolWrapperCommManager.burn", msgDebug + "WrLoaderStatus");

			EvolWrLoaderStatus *pMsg = (EvolWrLoaderStatus *) genMsg;
			UINT16 nodeId = ntohs(pMsg->nodeId);
			WrSlaveLoaderState status = (WrSlaveLoaderState)ntohs(pMsg->status);
			UINT32 addr = ntohl(pMsg->addr);
			WrSlaveLoaderFaults fault = (WrSlaveLoaderFaults)ntohs(pMsg->fault);
			UINT16 updateFailureCount = ntohs(pMsg->updateFailureCount);
			WrSlaveSoftwareComponent softComponent = (WrSlaveSoftwareComponent)ntohs(pMsg->softComponent);

			if ((bytesSentSoFar == sizeof(WrUpdateWrapper)) && (totalBytesToSend == 0))
			{
				bytesSentSoFar = 0;
				Debug::trace("EvolWrapperCommManager", msgDebug + "Set bytesSentSoFar to 0.");
			}

			Debug::trace("EvolWrapperCommManager.burn", msgDebug + "nodeId = " + Integer::toString(nodeId) + ",  softComponent = " + Integer::toString(softComponent));
			Debug::trace("EvolWrapperCommManager.burn", msgDebug + "status = " + Integer::toString(status));
			Debug::trace("EvolWrapperCommManager.burn", msgDebug + "addr = " + Hex::toString(addr));
			Debug::trace("EvolWrapperCommManager.burn", msgDebug + "fault = " + Integer::toString(fault));
			Debug::trace("EvolWrapperCommManager.burn", msgDebug + "updateFailureCount = " + Integer::toString(updateFailureCount));

			switch(status)
			{
				case wrSlaveBootInit:
					Debug::trace("EvolWrapperCommManager", msgDebug + "status = wrSlaveBootInit");
					break;

				case wrSlaveBootReady:
					{

						Debug::trace("EvolWrapperCommManager.burn", msgDebug + "status = wrSlaveBootReady");

						//crank up our run thread!!!
						sleepTime = 4;	  //only 4mS of sleep time.

						nodeListIterator i;
						HobVersion::VersionType vt = getSoftwareType(nodeId, softComponent);

						if (traceOn)
						{
							Debug::trace("EvolWrapperCommManager.burn", msgDebug + "vt: " + Integer::toString(vt) + "  pendingList_.size() = " + Integer::toString((int)pendingList_.size()));
							if (pendingList_.size() > 0)
							{
								Debug::trace("EvolWrapperCommManager.burn", msgDebug + "*****  Showing the pendingList_  *****");
								for (i = pendingList_.begin(); (i != pendingList_.end()); i++)
								{
									HobString vtStr = Integer::toString(i->first);
									util_filesystem::File tmpSrc(i->second);
									Debug::trace("EvolWrapperCommManager.burn", msgDebug + vtStr + ",  [" + tmpSrc.getFullPath() + "]");
								}
								Debug::trace("EvolWrapperCommManager.burn", msgDebug + "*****  End pendingList_  *****");
							}
						}

						if (fault == wrSlaveNone)
						{
							i = pendingList_.find(vt);
							if (i != pendingList_.end())
							{
								if (traceOn)
									Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Found the versionType in the pending list. Copy the tar file and extract the source.bin.");

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
								Debug::trace("EvolWrapperCommManager.burn", "added component to process list");

								processingList_.insert(make_pair(i->first, i->second));
								pendingList_.erase(i->first);

								if (traceOn)
								{
									if (processingList_.size() > 0)
									{
										Debug::trace("EvolWrapperCommManager.burn", msgDebug + "*****  Showing the processingList_  *****");
										for (i = processingList_.begin(); (i != processingList_.end()); i++)
										{
											HobString vtStr = Integer::toString(i->first);
											util_filesystem::File tmpSrc(i->second);
											Debug::trace("EvolWrapperCommManager.burn", msgDebug + vtStr + ",  [" + tmpSrc.getFullPath() + "]");
										}
										Debug::trace("EvolWrapperCommManager.burn", msgDebug + "*****  End processingList_  *****");
									}
								}
							}
							else
							{
								
									Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Did not find the node in the pending list. Don't know what to do here, so we are done. Transition to ALL_DOWNLOADS_COMPLETE.");
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
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append("Fault!  codeUpdaterStateMachine_ = ");
								if (codeUpdaterStateMachine_ == DOWNLOADING_CODE)
									msgDebug2.append("DOWNLOADING_CODE");
								else
									msgDebug2.append(Integer::toString(codeUpdaterStateMachine_));
								Debug::trace("EvolWrapperCommManager.burn", msgDebug2);
							}

							if (codeUpdaterStateMachine_ == DOWNLOADING_CODE)
							{
							
									Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Current state is Downloading code.");

								i = processingList_.find(vt);

								if (i != processingList_.end())
								{
									updateUserInterface(vt, MT_FAILED);

									Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Found the node in the processing list.");

									if ((fault != wrSlaveCheckSum) && (fault != wrSlaveUnknownRecordType))
									{
										Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Retry the flash.");

										/*leave job in pending list as we will retry it after a delay*/
										codeUpdaterStateMachine_ = PAUSE_BEFORE_RETRY;
									}
									else
									{
										Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Not retrying the flash because image is corrupted.");

										failedList_.insert(make_pair(i->first, fault));

										util_filesystem::File tmpData(i->second);

										if (tmpData.exists())
										{
											if (traceOn)
												Debug::trace("EvolWrapperCommManager", msgDebug + "Removing the temporary file on the ramdrive path = " + tmpData.getFullPath());
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
				case wrSlaveBootFault:
					{
						nodeListIterator i;
						HobVersion::VersionType vt = getSoftwareType(nodeId, softComponent);

						Debug::trace("EvolWrapperCommManager.burn", msgDebug + "status = wrSlaveBootFault");

						/*  It might be possible to get two fault messages, so this allows us to only handle the first one. */
						if (codeUpdaterStateMachine_ == DOWNLOADING_CODE)
						{
							Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Current state is Downloading code.");

							i = processingList_.find(vt);

							if (i != processingList_.end())
							{
								updateUserInterface(vt, MT_FAILED);

								Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Found the node in the processing list.");

								if ((fault != wrSlaveCheckSum) && (fault != wrSlaveUnknownRecordType))
								{
									Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Retry the flash.");

									/*leave job in pending list as we will retry it after a delay*/
									codeUpdaterStateMachine_ = PAUSE_BEFORE_RETRY;
								}
								else
								{
									Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Not retrying the flash because image is corrupted.");


									failedList_.insert(make_pair(i->first, fault));

									util_filesystem::File tmpData(i->second);

									if (tmpData.exists())
									{
										Debug::trace("EvolWrapperCommManager.burn", msgDebug + "Removing the temporary file on the ramdrive path = " + tmpData.getFullPath());
										tmpData.remove();
									}

									processingList_.erase(i->first);

									codeUpdaterStateMachine_ = DOWNLOAD_COMPLETE;
								}
							}
						}
						break;
					}
				case wrSlaveBootDone:
					{
						nodeListIterator i;
						HobVersion::VersionType vt = getSoftwareType(nodeId, softComponent);
						util_filesystem::Directory tmpPeripheralDir("");
						HobComplexVersion::getExtractionDir(vt, tmpPeripheralDir, (USBD_NODE_ID) getNodeID());									  

						Debug::trace("EvolWrapperCommManager.burn", msgDebug + "status = wrSlaveBootDone");
						Debug::trace("EvolWrapperCommManager.burn", msgDebug + "tmpPeripheralDir = [" + tmpPeripheralDir.getFullPath() + "] - We need to delete this directory since we are done.");

						i = processingList_.find(vt);

						if (i != processingList_.end())
						{
							updateUserInterface(vt, MT_SUCCESS);

							if (traceOn)
								Debug::trace("EvolWrapperCommManager", msgDebug + "Found the node in the processing list.");

							successfullList_.insert(make_pair(i->first, fault));

							util_filesystem::File tmpData(i->second);
							if (tmpData.exists())
							{
								if (traceOn)
									Debug::trace("EvolWrapperCommManager", msgDebug + "Removing the temporary file on the ramdrive path = " + tmpData.getFullPath());
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
					Debug::trace("EvolWrapperCommManager", "receivedWrapperDevMessage:unknown event");
					break;
			}
			break;
		}
		default:
			Debug::trace("EvolWrapperCommManager.Error", HobString("Received Unknown Wrapper message: ") +
				Hex::toString( (char*) genMsg, msgLen, true ) );
			break;
	}
}

static const unsigned long maxIntXferSize = 64;
void EvolWrapperCommManager::sendUpgradeBurnList()
{
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

	memcpy( &str[10], (char *) &pIntData_[0], size);
	sendUsbInterruptMessage( ( const char * ) str,  size + 10, result, is_custom_code );

	intDataLength_ = 0;
	pIntData_ = NULL;
	xmitting_ = false;
}

void EvolWrapperCommManager::sendRecord()
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
			Debug::trace("EvolWrapperCommManager.burn", "***In Srecord Mode but not Srecord***");
			Debug::trace("EvolWrapperCommManager.burn", HobString("Pointer Addr pIntData_:") + Hex::toString((long long)pIntData_ ));
			Debug::trace("EvolWrapperCommManager.burn", HobString("Bad Pointer pIntData_:") + Hex::toString((const char *)pIntData_, 128, true ));
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
			Debug::trace("EvolWrapperCommManager.burn", "***S7 found***");
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

	memcpy( &str[10], (char *) &pIntData_[0], size);
	sendUsbInterruptMessage( ( const char * ) str,  size + 10, result, is_custom_code );
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

void EvolWrapperCommManager::receiveErrorCallBack( CUsbPipe::EvolPipeType pipe_type, DWORD error, bool user_defined_error )
{
	printUSBError( error, user_defined_error );

	Debug::trace("EvolWrapperCommManager.Error", std::string("***  receiveErrorCallBack  ***:  error = ") + Integer::toString(error) +
			std::string(",  pipe type: ") + Integer::toString(pipe_type));
}

DWORD EvolWrapperCommManager::Run()
{
	try
	{
		registerEvent(VersionManagerDev, 18);	//CancelUpgrade

		while (!m_ExitEvent.IsEventSet())
		{
			EvolWrapperCommManagerCS.Enter();

			switch (codeUpdaterStateMachine_)
			{
				case WAITING_TO_SEND_UPDATE:
				{
					attemptToSendUpdateCntr_ = 0;
				}
				break;

				case SENDING_UPDATE:
				{
					if ((intDataLength_ > 0) && (!xmitting_))
					{
						Debug::trace("EvolWrapperCommManager.burn", HobString("SM:SENDING_UPDATE:send out update message"));

						codeUpdaterStateMachine_ = WAITING_FOR_STATUS_START;

						sendUpgradeBurnList();
					}
					else
					{
						attemptToSendUpdateCntr_++;
						Debug::trace("EvolWrapperCommManager.burn", HobString("SM:SENDING_UPDATE:cannot send update message"));

						if (attemptToSendUpdateCntr_ > 100)
						{
							Debug::trace("EvolWrapperCommManager", HobString("SM:SENDING_UPDATE:filure count exceeded. cancel sending update message"));
							codeUpdaterStateMachine_ = WAITING_TO_SEND_UPDATE;
						}
					}

				}
				break;

				case WAITING_FOR_STATUS_START:
				{
					/*processingList will be empty until we receive a loader status ready message from the wrapper.*/
					if (processingList_.size() > 0)
					{
						Debug::trace("EvolWrapperCommManager.burn", HobString("SM:WAITING_FOR_STATUS_START:start download"));
						nodeListIterator i;

						i = processingList_.begin();

						if (i != processingList_.end())
						{
							Debug::trace("EvolWrapperCommManager.burn", HobString("SM:WAITING_FOR_STATUS_START:found the file in processing list"));
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
						sendRecord();
						
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
						}
					}
					else
					{
						Debug::trace("EvolWrapperCommManager.burn", HobString("SM:DOWNLOADING_CODE: cannot send record"));
						Debug::trace("EvolWrapperCommManager.burn",HobString("intDataLength_") + HobString(Integer::toString(intDataLength_)) + HobString(Bool::toString(xmitting_)));
					}
				}
				break;

				case LAST_SRECORD_SENT:
				{
					//wait for response from wrapper
				}
				break;

				case PAUSE_BEFORE_RETRY:
				{
					Debug::trace("EvolWrapperCommManager.burn", HobString("SM:PAUSE_BEFORE_RETRY:pause complete"));
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
						Debug::trace("EvolWrapperCommManager.burn", HobString("SM:DOWNLOAD_COMPLETE:all nodes flashed. transition to ALL_DOWNLOADS_COMPLETE"));
						codeUpdaterStateMachine_ = ALL_DOWNLOADS_COMPLETE;
					}
					else
					{
						Debug::trace("EvolWrapperCommManager.burn", HobString("SM:DOWNLOAD_COMPLETE:last node flash complete. still more to flash. transition to WAITING_FOR_STATUS_START"));
						//this node is flashed. now wait for the next node to be ready
						//for flashing.
						codeUpdaterStateMachine_ = WAITING_FOR_STATUS_START;
					}
				}
				break;

				case ALL_DOWNLOADS_COMPLETE:
				{
					Debug::trace("EvolWrapperCommManager.burn", HobString("SM:ALL_DOWNLOADS_COMPLETE:all node flashs complete. wait to send next update message if necessary. transition to WAITING_TO_SEND_UPDATE"));
					UpgradePeripheral msg(UpgradeComplete, Wrapper, WrEvolWrapper);
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
			EvolWrapperCommManagerCS.Leave();
			Debug::trace("WrapperManager.ProbeWrapper", " Exiting Comm Run");
			PAL_absclass::PAL_waitForSingleObj(m_ExitEvent, sleepTime);				
			Debug::trace("WrapperManager.ProbeWrapper", " Comm Run is done");
		}
	}
	catch (...)
	{
		LogCatch;
		Debug::trace("EvolWrapperCommManager.Error", std::string("Error caught in Run()!!"));
	}
	return 0;
}

void EvolWrapperCommManager::setupCodeFlash (nodeList& burnList)
{
	int node1List = 0, node2List = 0, node3List = 0, node4List = 0, node5List = 0, node6List = 0;

	if ((codeUpdaterStateMachine_ != WAITING_TO_SEND_UPDATE) || xmitting_)
	{
		Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:flashing is currently in progress "));
		return;
	}
	Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:starting burn process list size = ") + Integer::toString((int)burnList.size()));

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

	Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  pendingList_.size() = ") + Integer::toString((int)pendingList_.size()));
	if (pendingList_.size() > 0)
	{
		for (i = pendingList_.begin(); (i != pendingList_.end()); i++)
		{
			if (i != pendingList_.end())
			{
				Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:found component in pending list type = ") + Integer::toString(i->first));

				util_filesystem::File tmpSrc(i->second);

				if (tmpSrc.exists())
				{
					Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:file exists path = ") + i->second);
					switch (i->first)
					{
						case HobVersion::WRMasterNVDataSW:
						case HobVersion::WREMasterNVDataSW:
						case HobVersion::CLADataSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRMasterNVDataSW"));
							node2List |= UPDATE_DATA_MASK;
							break;
						case HobVersion::WRMasterBootSW:
						case HobVersion::WREMasterBootSW:
						case HobVersion::CLABootSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRMasterBootSW"));
							node2List |= UPDATE_BOOT_MASK;
							break;
						case HobVersion::WRMasterAppSW:
						case HobVersion::WREMasterAppSW:
						case HobVersion::CLAAppSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRMasterAppSW"));
							node2List |= UPDATE_APP_MASK;
							break;
						case HobVersion::WRSlave1NVDataSW:
						case HobVersion::WRESlave1NVDataSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave1NVDataSW"));
							node1List |= UPDATE_DATA_MASK;
							break;
						case HobVersion::WRSlave1BootSW:
						case HobVersion::WRESlave1BootSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave1BootSW"));
							node1List |= UPDATE_BOOT_MASK;
							break;
						case HobVersion::WRSlave1AppSW:
						case HobVersion::WRESlave1AppSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave1AppSW"));
							node1List |= UPDATE_APP_MASK;
							break;
						case HobVersion::WRSlave3NVDataSW:
						case HobVersion::WRESlave3NVDataSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave3NVDataSW"));
							node3List |= UPDATE_DATA_MASK;
							break;
						case HobVersion::WRSlave3BootSW:
						case HobVersion::WRESlave3BootSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave3BootSW"));
							node3List |= UPDATE_BOOT_MASK;
							break;
						case HobVersion::WRSlave3AppSW:
						case HobVersion::WRESlave3AppSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave3AppSW"));
							node3List |= UPDATE_APP_MASK;
							break;
						case HobVersion::WRSlave4NVDataSW:
						case HobVersion::WRESlave4NVDataSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave4NVDataSW"));
							node4List |= UPDATE_DATA_MASK;
							break;
						case HobVersion::WRSlave4BootSW:
						case HobVersion::WRESlave4BootSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave4BootSW"));
							node4List |= UPDATE_BOOT_MASK;
							break;
						case HobVersion::WRSlave4AppSW:
						case HobVersion::WRESlave4AppSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave4AppSW"));
							node4List |= UPDATE_APP_MASK;
							break;
						case HobVersion::WRSlave5NVDataSW:
						case HobVersion::WRESlave5NVDataSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave5NVDataSW"));
							node5List |= UPDATE_DATA_MASK;
							break;
						case HobVersion::WRSlave5BootSW:
						case HobVersion::WRESlave5BootSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave5BootSW"));
							node5List |= UPDATE_BOOT_MASK;
							break;
						case HobVersion::WRSlave5AppSW:
						case HobVersion::WRESlave5AppSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave5AppSW"));
							node5List |= UPDATE_APP_MASK;
							break;
						case HobVersion::WRSlave6NVDataSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave6NVDataSW"));
							node6List |= UPDATE_DATA_MASK;
							break;
						case HobVersion::WRSlave6BootSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave6BootSW"));
							node6List |= UPDATE_BOOT_MASK;
							break;
						case HobVersion::WRSlave6AppSW:
							Debug::trace("EvolWrapperCommManager.burn", HobString("setupCodeFlash:  WRSlave6AppSW"));
							node6List |= UPDATE_APP_MASK;
							break;
						default:
							break;
					}
				}
			}
		}

		WrUpdateWrapper outCmd;

		outCmd.wrEvent = htons(WrLoaderUpdate);

		outCmd.node1 = htons((node1List > 0));
		outCmd.update1 = htons((UINT16) determineEnumForUpdateMask(node1List));
		outCmd.node3 = htons((node3List > 0));
		outCmd.update3 = htons((UINT16) determineEnumForUpdateMask(node3List));
		outCmd.node4 = htons((node4List > 0));
		outCmd.update4 = htons((UINT16) determineEnumForUpdateMask(node4List));
		outCmd.node5 = htons((node5List > 0));
		outCmd.update5 = htons((UINT16) determineEnumForUpdateMask(node5List));
		outCmd.node6 = htons((node6List > 0));
		outCmd.update6 = htons((UINT16) determineEnumForUpdateMask(node6List));
		outCmd.node2 = htons((node2List > 0));
		outCmd.update2 = htons((UINT16) determineEnumForUpdateMask(node2List));

		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node1 = ") + Integer::toString(outCmd.node1));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node1 update enum = ") + Integer::toString(outCmd.update1));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node3 = ") + Integer::toString(outCmd.node3));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node3 update enum = ") + Integer::toString(outCmd.update3));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node4 = ") + Integer::toString(outCmd.node4));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node4 update enum = ") + Integer::toString(outCmd.update4));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node5 = ") + Integer::toString(outCmd.node5));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node5 update enum = ") + Integer::toString(outCmd.update5));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node6 = ") + Integer::toString(outCmd.node6));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node6 update enum = ") + Integer::toString(outCmd.update6));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node2 = ") + Integer::toString(outCmd.node2));
		Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd node2 update enum = ") + Integer::toString(outCmd.update2));

		usbEvolHdr.setSourcePhysicalAddress(0x0000);
		usbEvolHdr.setSourceLogicalAddress(0x0009);
		usbEvolHdr.setDestinationPhysicalAddress(0x8181);
		usbEvolHdr.setDestinationLogicalAddress(0x001D);

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

			Debug::trace("EvolWrapperCommManager.burn", HobString("length of outCmd: ") + Integer::toString(intDataLength_));

			Debug::trace("EvolWrapperCommManager.burn", HobString("outCmd data: ") +
					HobString(Hex::toString((const char *) (flashImgBuffer_), intDataLength_)));		

			codeUpdaterStateMachine_ = SENDING_UPDATE;
		}
	}  
}

WrComponentUpdate EvolWrapperCommManager::determineEnumForUpdateMask(int updateMask)
{
	if ( (updateMask & UPDATE_DATA_MASK) && (updateMask & UPDATE_BOOT_MASK) && (updateMask & UPDATE_APP_MASK) )
	{
		return wrAll;
	}
	else
	{
		if ( (updateMask & UPDATE_DATA_MASK) && (updateMask & UPDATE_BOOT_MASK) )
		{
			return wrDataBoot;
		}
		else
		{
			if ( (updateMask & UPDATE_BOOT_MASK) && (updateMask & UPDATE_APP_MASK) )					
			{
				return wrAppBoot;
			}
			else
			{
				if ( (updateMask & UPDATE_DATA_MASK) && (updateMask & UPDATE_APP_MASK) )						
				{
					return wrAppData;
				}
				else
				{
					if (updateMask & UPDATE_DATA_MASK)
					{
						return wrData;
					}
					else
					{
						if (updateMask & UPDATE_APP_MASK)
						{
							return wrApp;
						}
						else
						{
							return wrBoot;
						}
					}
				}
			}
		}
	}
}

void EvolWrapperCommManager::requestANodeUpdate(HobVersion::VersionType softCompType, const string& codeLocation)
{
	nodeListIterator i;

	Debug::trace("EvolWrapperCommManager", HobString("requestANodeUpdate:adding soft type = ") + Integer::toString(softCompType) + HobString(" path = ") + codeLocation);

	i = pendingList_.find(softCompType);

	if (i != pendingList_.end())
	{
		Debug::trace("EvolWrapperCommManager", "requestANodeUpdate:nodeId found will delete");

		pendingList_.erase(softCompType);
	}
	pendingList_.insert(make_pair(softCompType, codeLocation));
}

bool EvolWrapperCommManager::createCommLink()
{
	//start the run thread
	if (pEvolWrapperCommManagerThread)
	{
		pEvolWrapperCommManagerThread->RestartThread(); 
		return true;
	}
	else
	{
		return false;
	}
}


bool EvolWrapperCommManager::initializePeripheral()
{
	/*setup the int out transfer to initiate the flash update*/
	return true;
}

bool EvolWrapperCommManager::downloadCode(const char * pCode)
{
	bool traceOn = Debug::isTracing("EvolWrapperCommManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "downloadCode():  ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("initiate flashing code through interupt pipe file = ").append(pCode);
		Debug::trace("EvolWrapperCommManager", msgDebug2);
	}
	Debug::trace("EvolWrapperCommManager", HobString("initiate flashing code through interupt pipe file = ") + HobString(pCode));

	int blockSize = 0;
	int fileSize = 0; 

	if (PAL_absclass::PAL_getFileSize(pCode, &fileSize))
	{
		Debug::trace("EvolWrapperCommManager", "have file size!");
		blockSize = 1024;

		PAL_absclass::PAL_sleep(10);
	}
	else
	{
		Debug::trace("EvolWrapperCommManager", "failed to get file size.");
		return false;
	}

	int bytesRead = 0;
	UINT8* tmpPtr;

	Debug::trace("EvolWrapperCommManager.burn", HobString("file size = ") + Integer::toString(fileSize));

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

			Debug::trace("EvolWrapperCommManager.burn", HobString("Pointer  pIntData_:") + Hex::toString((long long)pIntData_ ));
			Debug::trace("EvolWrapperCommManager.burn", HobString("Pointer flashImgBuffer_:") + Hex::toString((long long)flashImgBuffer_ ));
			
			sizeModeSRecord_ = true;
		}
	}
	return true;
}

bool EvolWrapperCommManager::finalizePeripheral()
{
	return true;
}

bool EvolWrapperCommManager::rebootPeripheral()
{
	Debug::trace("EvolWrapperCommManager", HobString("rebootPeripheral()"));
	return false;
/*
	NOTE:
	According to Aaron and Carlos the WR_VERSION_MATCH is not used anywhere and is not even
	supported in the wrapper.  So I am removing this code for now.

	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	EvolWrGenericOneParam genMsg;
	memset((char *) &genMsg, 0, sizeof(genMsg));

	genMsg.wrEvent = htons((UINT16) WR_VERSION_MATCH);
	genMsg.param = htons((UINT16) true);
	usbEvolHdr.setSourcePhysicalAddress(0x0000);
	usbEvolHdr.setSourceLogicalAddress(0x0009);
	usbEvolHdr.setDestinationPhysicalAddress(0x8181);
	usbEvolHdr.setDestinationLogicalAddress(0x001D);
	usbEvolHdr.setMessageLength(sizeof(genMsg));
	usbEvolHdr.Serialize( str );

	memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
	sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

	if ( result != OK ) //error
	{
		Debug::trace("EvolWrapperCommManager.Error", "Failed to send WR_VERSION_MATCH msg!");
		printUSBError( result, is_custom_code );
		return false;
	}
	else
	{
		return true;
	}
*/
}


bool EvolWrapperCommManager::deleteCommLink()
{
	return true;
}


HobVersion::VersionType EvolWrapperCommManager::getSoftwareType(UINT16 nodeID, WrSlaveSoftwareComponent type)
{
	switch (nodeID)
	{
		case 1:
		{
			switch (type)
			{
				case wrSlaveNVData:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave1NVDataSW;
					else					
						return HobVersion::WRSlave1NVDataSW;
					break;
				}					
				case wrSlaveBootLoader:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave1BootSW;
					else						
						return HobVersion::WRSlave1BootSW;
					break;
				}					
				case wrSlaveProgramFlash:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave1AppSW;
					else						
						return HobVersion::WRSlave1AppSW;
					break;
				}
				default:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave1AppSW;						
					else					
						return HobVersion::WRSlave1AppSW;
					break;										  
				}
			}
		}
		break;
		case 2:
		{
			switch (type)
			{
				case wrSlaveNVData:
				{
					/*if(EWSWrapperType_)
						return HobVersion::WREMasterNVDataSW;
					else	
					return HobVersion::WRMasterNVDataSW;*/
					/*if(deviceType_ == 0)//AWS
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: WRMasterNVDataSW");
						return HobVersion::WRMasterNVDataSW;
					}
					else if(deviceType_ == 1)//EWS
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: WREMasterNVDataSW");
						return HobVersion::WREMasterNVDataSW;
					}
					else//CLA
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: CLADataSW");
						return HobVersion::CLADataSW;
					}*/
					if(deviceType_ == 2)//CLA
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: CLADataSW");
						return HobVersion::CLADataSW;
					}
					else//Original Code
					{
						if(EWSWrapperType_)
							return HobVersion::WREMasterNVDataSW;
						else	
							return HobVersion::WRMasterNVDataSW;
					}
				
					break;
				}					
				case wrSlaveBootLoader:
				{
					/*if(EWSWrapperType_)
						return HobVersion::WREMasterBootSW;
					else
					return HobVersion::WRMasterBootSW;*/
					/*if(deviceType_ == 0)//AWS
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: WRMasterBootSW");
						return HobVersion::WRMasterBootSW;
					}
					else if(deviceType_ == 1)//EWS
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: WREMasterBootSW");
						return HobVersion::WREMasterBootSW;
					}
					else//CLA
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: CLABootSW");
						return HobVersion::CLABootSW;
					}*/
					if(deviceType_ == 2)//CLA
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: CLABootSW");
						return HobVersion::CLABootSW;
					}
					else//Original Code
					{
						if(EWSWrapperType_)
							return HobVersion::WREMasterBootSW;
						else
							return HobVersion::WRMasterBootSW;
					}
					break;
				}
				case wrSlaveProgramFlash:
				{
					/*if(EWSWrapperType_)
						return HobVersion::WREMasterAppSW;
					else
					return HobVersion::WRMasterAppSW;*/
					/*if(deviceType_ == 0)//AWS
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: WRMasterAppSW");
						return HobVersion::WRMasterAppSW;
					}
					else if(deviceType_ == 1)//EWS
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: WREMasterAppSW");
						return HobVersion::WREMasterAppSW;
					}
					else//CLA
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: CLAAppSW");
						return HobVersion::CLAAppSW;
					}*/
					if(deviceType_ == 2)//CLA
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: CLAAppSW");
						return HobVersion::CLAAppSW;
					}
					else//original code
					{
						if(EWSWrapperType_)
							return HobVersion::WREMasterAppSW;
						else
							return HobVersion::WRMasterAppSW;
					}
					break;
				}
				default:
				{
					/*if(EWSWrapperType_)
						return HobVersion::WREMasterAppSW;
					else
					return HobVersion::WRMasterAppSW;*/
					/*if(deviceType_ == 0)//AWS
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: WRMasterAppSW");
						return HobVersion::WRMasterAppSW;
					}
					else if(deviceType_ == 1)//EWS
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: WREMasterAppSW");
						return HobVersion::WREMasterAppSW;
					}
					else//CLA
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: CLAAppSW");
						return HobVersion::CLAAppSW;
					}*/
					if(deviceType_ == 2)//CLA
					{
						Debug::trace("EvolWrapperCommManager.burn","getSoftwareType(): Node 2: CLAAppSW");
						return HobVersion::CLAAppSW;
					}
					else//original code
					{
						if(EWSWrapperType_)
							return HobVersion::WREMasterAppSW;
						else
							return HobVersion::WRMasterAppSW;
					}
					
					break;										  
				}
			}
		}
		break;
		case 3:
		{
			switch (type)
			{
				case wrSlaveNVData:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave3NVDataSW;
					else
						return HobVersion::WRSlave3NVDataSW;
					break;
				}
				case wrSlaveBootLoader:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave3BootSW;
					else
						return HobVersion::WRSlave3BootSW;
					break;
				}
				case wrSlaveProgramFlash:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave3AppSW;
					else					
						return HobVersion::WRSlave3AppSW;
					break;
				}					
				default:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave3AppSW;
					else
						return HobVersion::WRSlave3AppSW;
					break;										  
				}
			}
		}
		break;
		case 4:
		{
			switch (type)
			{
				case wrSlaveNVData:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave4NVDataSW;						
					else
						return HobVersion::WRSlave4NVDataSW;
					break;
				}
				case wrSlaveBootLoader:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave4BootSW;
					else
						return HobVersion::WRSlave4BootSW;
					break;
				}
				case wrSlaveProgramFlash:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave4AppSW;
					else
						return HobVersion::WRSlave4AppSW;
					break;
				}					
				default:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave4AppSW;
					else
						return HobVersion::WRSlave4AppSW;
					break;										  
				}
			}
		}
		break;
		case 5:
		{
			switch (type)
			{
				case wrSlaveNVData:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave5NVDataSW;
					else
						return HobVersion::WRSlave5NVDataSW;
					break;
				}
				case wrSlaveBootLoader:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave5BootSW;
					else
						return HobVersion::WRSlave5BootSW;
					break;
				}
				case wrSlaveProgramFlash:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave5AppSW;
					else
						return HobVersion::WRSlave5AppSW;
					break;
				}
				default:
				{
					if(EWSWrapperType_)
						return HobVersion::WRESlave5AppSW;
					else
						return HobVersion::WRSlave5AppSW;
					break;										  
				}
			}
		}
		break;
		case 6:
		{
			//camera system is the same on both wrapper models. 
			switch (type)
			{
				case wrSlaveNVData:
					return HobVersion::WRSlave6NVDataSW;
					break;
				case wrSlaveBootLoader:
					return HobVersion::WRSlave6BootSW;
					break;
				case wrSlaveProgramFlash:
					return HobVersion::WRSlave6AppSW;
					break;
				default:
					return HobVersion::WRSlave6AppSW;
					break;										  
			}
		}
		break;
		default:
			break;
	}
	return HobVersion::UnknownComponent;
}


bool EvolWrapperCommManager::areTransactionsComplete()
{
	if ((pendingList_.size() == 0) && (processingList_.size() == 0))
	{
		Debug::trace("EvolWrapperCommManager", HobString("areTransactionsComplete():  Returning true"));
		return true;
	}
	else
	{
		Debug::trace("EvolWrapperCommManager", HobString("areTransactionsComplete():  Returning false"));
		return false;
	}
}

void EvolWrapperCommManager::resetTransferCounters()
{
	uiUpdateCounter = 0;
	bytesSentSoFar = 0;
	totalBytesToSend = 0;
	updateSessionCounter = 0;
}

void EvolWrapperCommManager::updateUserInterface(HobVersion::VersionType softVersionType, MessageTypes messageType)
{
	HobString msgDebug("updateUserInterface:");
	msgDebug.append("  softVersionType = ").append(Integer::toString(softVersionType));
	msgDebug.append("  messageType = ").append(Integer::toString(messageType));
	Debug::trace("EvolWrapperCommManager", msgDebug);

	CUEvolWrapperStatus status((UINT16) softVersionType, (UINT16) messageType);
	status.show();
	Debug::trace("EvolWrapperCommManager", "updateUserInterface:  end of show *****");
	dispatchEvent(&status);
}

void EvolWrapperCommManager::checkUpgradeProgress(bool updateAnyway)
{
	if (updateAnyway || (++updateSessionCounter == 25))
	{
		UpgradeTransferStatus statusMsg(bytesSentSoFar, totalBytesToSend);
		dispatchEvent(&statusMsg);

		HobString msgDebug("checkUpgradeProgress():  ");
		msgDebug.append("  bytesSentSoFar = ").append(Long::toString(bytesSentSoFar));
		msgDebug.append("  totalBytesToSend = ").append(Long::toString(totalBytesToSend));
		Debug::trace("EvolWrapperCommManager.updateProgress", msgDebug);
		updateSessionCounter = 0;
	}
}
