
#include "WrapperEvent.h"
#include "Wrapper.h"
#include "WrapperManager.h"
#include "Configuration.h"
#include "EncoderPositionRange.h"

#include "WrapperInquire.h"
#include "WrapperWakeup.h"
#include "WrapperConfiguration.h"
#include "WrapperGeneric.h"
#include "WrapperMode.h"
#include "WrapperOpen.h"
#include "WrapperSensors.h"
#include "WrapperRequestInquire.h"
#include "WrapperRequestSensors.h"
#include "WrapperRequestStatus.h"
#include "WrapperRequestWakeup.h"
#include "WrapperSystemStatus.h"
#include "WrapperSystemConfiguration.h"
#include "WrapperSystemInquire.h"
#include "WrapperSystemRequestStatus.h"
#include "WrapperSystemRequestInquire.h"
#include "WrapperSystemRequestWakeup.h"
#include "WrapperRequestErrorHistory.h"
#include "WrapperStatusTracking.h"
#include "WrapperSensorsTracking.h"
#include "WrapperErrorHistoryTracking.h"
#include "WrapperTest.h"
#include "WrapperLoad.h"
#include "WrapperWrapperKind.h"

#include "WrapperJog.h"
#include "WrapperCycle.h"
#include "WrapperWrap.h"
#include "WrapperControl.h"

#include "EvolWrapperWakeup.h"
#include "EvolWrapperStatus.h"
#include "EvolWrapperControl.h"
#include "EvolWrapperLoad.h"
#include "EvolWrapperWrap.h"
#include "EvolWrapperInquire.h"
#include "EvolWrapperConfiguration.h"
#include "EvolWrapperPackageTimings.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "EvolWrapperTest.h"
#include "EvolWrapperTestComplete.h"
#include "EvolWrapperCycle.h"
#include "EvolWrapperPackageTimingsPointer.h"
#include "EvolWrapperResetDefaultPackageTimings.h"
#include "EvolWrapperUnlockPackageTimings.h"
#include "EvolWrapperTrayNames.h"
#include "EvolWrapperCameraScan.h"
#include "EvolWrapperSystemStatus.h"
#include "EvolWrapperErrorHistory.h"
#ifdef USE_CYCLE_COUNTER
#include "EvolWrapperCycleCount.h"
#include "EvolWrapperSaveCycleCount.h"
#include "EvolWrapperReqCycleCount.h"
#endif
#include "EvolWrapperCameraCalibration.h"
#include "EvolWrapperCameraThreshold.h"
#include "EvolWrapperWriteCameraThreshold.h"
#include "EvolWrapperSystemInfo.h"
#include "EvolWrapperSystemInfoWrite.h"
#include "EvolWrapperReqSystemReset.h"
#include "EvolWrapperMotorProfile.h"
#include "EvolWrapperDiagnosticsTest.h"
#include "EvolWrapperDiagnosticsTestResult.h"

#if 0
#include "EvolWrapperPackageHistory.h"
#endif

#include "EvolWrapperCameraHeightScan.h"
#include "EvolWrapperCamerasTracking.h"
#include "WeigherStatus.h"
#include "UpgradePeripheral.h"
#include "EvolWrapperEventCapture.h"
#include "EvolWrapperPackageDischarge.h"
#include "EvolWrapperReqUIModel.h"
#include "EvolWrapperUIModel.h"
#include "HobDateTime.h"
#include "FpmManager.h"
#include "MtdNGWWrapperData.h"
#include "NGWRequestVersion.h"
#include "WrapBoxSize.h"
#include "NGWEngInfo.h"
#include "NGWEngException.h"
#include "NGWEngInfo.h"
#include "NGWEngException.h"

#include "WrPPPWakeup.h"
#include "WrPPPConfig.h"
#include "WrPPPDist.h"
#include "WrPPPCalibration.h"
#include "WrPPPStatus.h"
#include "WrPPPVersion.h"
#include "WrPPPRequest.h"
#include "NGWWrapperPause.h"
#include "NGWWrapperResume.h"
#include "WrPPPTest.h"
#include "NGWDimensionOverrides.h"

#include <HfcAccessKeyActivation.h>
#include <AccessKeys.h>

#include "LabelApplierLabelReady.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Long.h"
#include "Bool.h"
#include "HobString.h"

#include "PAL_platformheader.h"

int WrapperManager::ignoreWakeups = 0;

#include <chrono>
#include <thread>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperManager::dataType = 0;
std::string WrapperManager::dataTypeName("WrapperManager"); 
#endif

WrapperManager::WrapperManager(Hobart::CEvolMsgReceiver *pMyWrapper,
		OpModeEvent currentOpMode,
		UINT16 physicalAddress,
		WrapperKind wrKind)
{
	Debug::trace("WrapperManager", "WrapperManager()");

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pMyWrapper = pMyWrapper;
	this->physicalAddress = physicalAddress;
	this->currentOpMode = currentOpMode;
	this->wrKind = wrKind;

	pMyTimings_ = NULL;

	if (currentOpMode == OpModeService)
	{
		probeTimerOn = false;
	}
	else
	{
		probeTimerOn = true;
	}

	pConfig = Configuration::getInstance();

	ticksPerSecond = 1000;
	
	probeTimeoutCntr_ = 0;
	
	wrapperProbeTime_ = pConfig->getWrapperProbeTime() * ticksPerSecond;
	probeWrapper = true;
//	lostContact = false;

	requestSensorTime = ticksPerSecond / 8;
//	requestSensorReading = false;
	trackSensors = false;

	trackCameras = false;

	sendGripperPWMTimings_ = false;
	requestedErrorHistory_ = false;
	wrTrayNames_ = 0;
	requestedPkgTimings_ = false;

	upgradeInProgress = false;

	requestWrapperModelUI_ = false;

	savingWrapperTimings_ = false;
	waitingToCycleWrapper_ = false;
	holdTestMsg_ = NULL;

	pollTimerStopped_ = false;
	pppTestTimerStart_ = false;
    isManfTestKeyPresent_ = false;

	previousCycleCount_ = evolWrapperCycleCount_ = 0;

	dimensionErrors_ = 0;
	msgCntr_ = 0;

    //get my type of system
    FpmManager *pSerialFlash = FpmManager::getInstance();

	if( pSerialFlash != NULL )
	{
	    style = pSerialFlash->getMFGSystemStyle();

		MtdNGWWrapperData *pData = pSerialFlash->readWrapperData();
		if( pData != NULL )
		{
			previousCycleCount_ = evolWrapperCycleCount_ = pData->getCycleCounts();
			Debug::trace("WrapperManager.Timer", "initialized evolWrapperCycleCount_: " + Integer::toString(evolWrapperCycleCount_));

			createTimer(( 10 * 1000 * 60 ), cycleCountTimerExpired, CHobartTimer::PERIODIC_TIMER); // every 10 min
		}
	}
	
	if( ( style == "NGW3" ) || ( style == "NGW2" ) )
	{
		partialCan_ = fullBrightCan_ = true;

		//quicken our response time to wrapper disconnects
		wrapperProbeTime_ = wrapperProbeTime_ / 2;

		registerEvent(Wrapper, WrNGWEngInfoN1);
		registerEvent(Wrapper, WrNGWEngInfoN2);
		registerEvent(Wrapper, WrNGWEngInfoN3);
		registerEvent(Wrapper, WrNGWEngInfoN4);
		registerEvent(Wrapper, WrNGWEngInfoN5);
		registerEvent(Wrapper, WrNGWEngInfoN6);
		registerEvent(Wrapper, WrNGWENGException);
		registerEvent(Wrapper, WrPPPResume);
	}
	else if ( ( style == "AWS3" ) || ( style == "NGW1" ) )
	{
		partialCan_ = true;
		fullBrightCan_ = false;		
	}
	//SOF_4216 Message applies to all wrapper models
	registerEvent(Wrapper, WrEvolReqSystemReset);

	createTimer(wrapperProbeTime_, wrapperProbe, CHobartTimer::PERIODIC_TIMER);

	//SOF-4638: two printer systems have Printer Plate Position sensors
	if( pSerialFlash->isWrapperTwoPrinter() ) {
		Debug::trace("WrapperManager.PPPTimer", "Start PPP probe timer: " + Integer::toString( ticksPerSecond * 10 ));
		createTimer(( ticksPerSecond * 10 ), pppProbeTimer, CHobartTimer::PERIODIC_TIMER);
		pppPollStart_ = true;
	}

	registerOperationalEvents();
	
	registerEvent(SessionDev, SnKeyActivation);


	sendWrapperKind();	
	
	//try sending here
	requestInquireMsg();

	//get the wrap box timings
	if( pMyWrapper )
	{
		EvolWrapperRequestPackageTimings rtm(EvolWrapperRequestPackageTimings::CurrentTimings);

		if( fullBrightCan_ )  {
			Debug::trace("WrapperManager", "WrapperManager():  dispatch: EvolWrapperRequestPackageTimings.");
			dispatchEvent(&rtm);
			requestedPkgTimings_ = true;
		} else {
			const char *pCC = rtm.serialize();
			pMyWrapper->send(pCC, rtm.getSerializedLength());
			requestedPkgTimings_ = true;
		}
	}
	else
		Debug::trace("WrapperManager", "Can't send EvolWrapperRequestPackageTimings. pMyWrapper is 0!");

	Debug::trace("WrapperManager", "WrapperManager():  dispatch: WrReqEvolSystemInfo.");
	if( !fullBrightCan_ )  {
		WrapperGeneric msg(WrReqEvolSystemInfo, this);
		dispatchEvent(&msg);
	} else {
		WrapperGeneric msg(WrReqEvolSystemInfo, this);
		const char *pCC = msg.serialize();
		pMyWrapper->send(pCC, msg.getSerializedLength());
	}

	PPPRequest req( this, PPPRequest::PPPReq::_Set_Rate, 800 );	
	dispatchEvent( &req );	

	//fix for SOF-4970
	PPPRequest wakeup_req( this, PPPRequest::PPPReq::_Wake );	
	dispatchEvent( &wakeup_req );	

	Debug::trace("WrapperManager", "WrapperManager():  Constructor is done.");
}

WrapperManager::~WrapperManager()
{
	Debug::trace("WrapperManager","~WrapperManager(): Entry");

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	cancelTimer(wrapperProbe);
	cancelTimer(sensorTimerExpired);

	//only for AWS3 and NGW1 systems
	if( partialCan_ ) {
		WrapperGeneric msg( WrNGWWrapperDetached, this );
		dispatchEvent( &msg );
	}

	deregisterAllEvents();

	if (wrKind == WrUltimaWrapper)
	{
		WrapperStatus lastWrapperStatus;

		lastWrapperStatus.setManagerDeleted();
		dispatchEvent(&lastWrapperStatus);
	}
	else
	{
		lastEvolWrapperStatus = EvolWrapperStatus();

		lastEvolWrapperStatus.setManagerDeleted();
		dispatchEvent(&lastEvolWrapperStatus);
	}

	if(wrTrayNames_)
	{
		delete wrTrayNames_;
	}
	//shutdown the msg thread
	ShutdownNicely();
	Debug::trace("WrapperManager","~WrapperManager(): Exit");
}



void WrapperManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	if ( !msg.get() )
		return;

	switch ( msg->getDeviceType() )
	{
	case SessionDev:
		handleSessionDevMessage( (SessionEvent) msg->getEventType(), msg->Data() );
		break;
	case Wrapper:
		handleWrapperEvent( (WrEvent) msg->getEventType(), msg->Data() );
		break;
	case WrapperSystem:
		handleWrapperSysEvent( (SysEvent) msg->getEventType(), msg->Data() );
		break;
	case Weigher:
		handleWeigherMessage( (WgEvent) msg->getEventType(), msg->Data() );
		break;
	default:
		{
			Debug::trace("WrapperManager.Error", "***** Unknown message, Device Type = " +
					 inttostring(msg->getDeviceType()) + ", EventType = " + inttostring(msg->getEventType()));
			break;
		}
	}
}


bool WrapperManager::isWrapperEvolWrapper()
{
	bool result = false;

	if(wrKind == WrEvolWrapper)
	{
		result = true;
	}

	return result;
}

void WrapperManager::handleWrapperEvent(WrEvent wrEvent, const char *recvBuffer)
{
	switch (wrEvent)
	{
		case WrWakeup:
		{
			Debug::trace("WrapperManager.ProbeWrapper", " WrWakeup");

			WrapperWakeup wrWakeup(recvBuffer);
			wrWakeup.setSourceObject(this);

			dispatchEvent(&wrWakeup);

			receivedWrapperFeedback();

			break;
		}
		case WrEvolReqSystemReset:
		{
			Debug::trace("WrapperManager.Message", "WrEvolReqSystemReset");
			requestEvolWrapperSystemReset();
			receivedWrapperFeedback();
			break;
		}
		case WrEvolWakeup:
		{
			if (WrapperManager::ignoreWakeups == 0)
			{
				Debug::trace("WrapperManager.ProbeWrapper", " WrEvolWakeup");

				EvolWrapperWakeup wrWakeup(recvBuffer);
				wrWakeup.setSourceObject(this);

				dispatchEvent(&wrWakeup);

				receivedWrapperFeedback();
			}
			else
				Debug::trace("WrapperManager.ProbeWrapper", " ignoring WrEvolWakeup received!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			break;
		}
		case WrInquire:
		{
			Debug::trace("WrapperManager.Message", "WrInquire");

			WrapperInquire wi(recvBuffer);
			wi.setSourceObject(this);

			if ( sendGripperPWMTimings_ )
			{
				Debug::trace("WrapperManager.PWMTimings", "sendGripperPWMTimings_ is true");

				// Change the gripper PWM timings
				WrapperTime time(0, 60, 255);
				WrapperGripperTiming& timing0 = wi.getWrapperGripperTiming0();
				timing0.setSetback(time);
				timing0.setPwm074();

				WrapperGripperTiming& timing1 = wi.getWrapperGripperTiming1();
				time.setValue(100);
				timing1.setSetback(time);
				timing1.setPwm074();

				WrapperGripperTiming& timing2 = wi.getWrapperGripperTiming2();
				time.setValue(140);
				timing2.setSetback(time);
				timing2.setPwm074();

				WrapperGripperTiming& timing3 = wi.getWrapperGripperTiming3();
				time.setValue(225);
				timing3.setSetback(time);
				timing3.setPwm038();

				// Now make it a config message and send it down to the wrapper
				wi.show();
				wi.setEventType(WrConfig);
				const char *pCC = wi.serialize();
				pMyWrapper->send(pCC, wi.getSerializedLength());

				// Send a ReqInquire so that with the reply we report the new settings
				WrapperRequestInquire wri(this);
				pCC = wri.serialize();
				pMyWrapper->send(pCC, wri.getSerializedLength());

				Debug::trace("WrapperManager.PWMTimings", "setting sendGripperPWMTimings_ to false");
				sendGripperPWMTimings_ = false;
			}
			else
			{
				dispatchEvent(&wi);

				if (Debug::isTracing("WrapperManager.WrapperInquire") == true)
				{
					wi.show();
				}
			}

			receivedWrapperFeedback();

			break;
		}
		case WrEvolInquire:
		{
			Debug::trace("WrapperManager.Message", "WrEvolInquire");

			EvolWrapperInquire wi(recvBuffer);
			wi.setSourceObject(this);

			if (Debug::isTracing("WrapperManager.Message") == true)
			{
				HobString m(recvBuffer);
				HobString h = HobString("EvolWrapperInquire message received: ") + HobString(Hex::toString(m.data(), sizeof(EvolWrapperInquire)));
				Debug::trace("WrapperManager.Message", h);

				Debug::trace("WrapperManager.Message", "wi.show()  ------------------------------------------- START");
				wi.show();
				Debug::trace("WrapperManager.Message", "wi.show()  ------------------------------------------- END");
			}
			dispatchEvent(&wi);

			receivedWrapperFeedback();

			break;
		}
		case WrOpen:
		{
			Debug::trace("WrapperManager.Message", "WrOpen");
			WrapperOpen wrOpen(recvBuffer);
			wrOpen.setSourceObject(this);
			dispatchEvent(&wrOpen);

			// test code for new evolution wrapper
			if (wrKind == WrEvolWrapper)
			{
				wrOpen.show();
			}
			receivedWrapperFeedback();
			break;
		}
		case WrReqStatus:
		{
			Debug::trace("WrapperManager.Message", "WrReqStatus");

			WrapperRequestStatus wrs(recvBuffer);
			wrs.setSourceObject(this);

			const char *pCC = wrs.serialize();
			pMyWrapper->send(pCC, wrs.getSerializedLength());

			if (wrKind == WrUltimaWrapper)
			{
				lastWrapperStatus = WrapperStatus();
			}
			else
			{
				EvolWrapperStatus tmp;
				lastEvolWrapperStatus = tmp;
			}
			break;
		} 
		case WrStatusTracking:
		{
			Debug::trace("WrapperManager.Message", "WrStatusTracking -- warning this code has been removed");

			break;
		}
		case WrStatus:
		{
			Debug::trace("WrapperManager.Message", "WrStatus");

			WrapperStatus statusMsg(recvBuffer);

			if (statusMsg.equals(lastWrapperStatus) == false)
			{
				//	Retain the current state of lastWrapperStatus.isPeripheralComm()
				//	for when we call receivedWrapperFeedback() at the end.
				bool isComm = lastWrapperStatus.isPeripheralComm();

				Debug::trace("WrapperManager.Message", "WrStatus - Status is different.");
				if (Debug::isTracing("WrapperManager.WrapperStatus") == true)
				{
					statusMsg.show();
				}

				WrapperStateInfo thisStateInfo(statusMsg.getStatusInfo().getStateInfo());
				WrapperStateInfo lastStateInfo(lastWrapperStatus.getStatusInfo().getStateInfo());

				if ((thisStateInfo.isIdle() == true) && (lastStateInfo.isIdle() == false))
				{
					Debug::trace("WrapperManager.Error", "Display Idle Wrapper Error");
				}

				// Maintain package history out of packages that go through the
				// elevator (which appears to be the strategy used in Ultima)
				WrapperPackageInfo& packageInfo = statusMsg.getPackageInfo(ElevatorPosition);
				if ( packageInfo.isPresent() )
				{
					addPackageToHistory(packageInfo.getPackCode(), packageInfo.getWidth(), packageInfo.getLength());
				}


				statusMsg.setSourceObject(this);

				if (isComm)
					dispatchEvent(&statusMsg);

				lastWrapperStatus = statusMsg;
				lastWrapperStatus.setPeripheralComm(isComm);
			}
			receivedWrapperFeedback();
			break;
		}
		case WrEvolStatus:
		{
			Debug::trace("WrapperManager.Message", "WrEvolStatus");

			EvolWrapperStatus statusMsg(recvBuffer);

			if (statusMsg.equals(lastEvolWrapperStatus) == false)
			{
				//SOF-1263 track package dimension errors
				EvolWrapperErrorInfo wrError = statusMsg.getCurrErrorInfo();

				int device = Wrapper;
				int deviceExt = wrError.getEvolWrapperErrorSource();
				int errorCode = wrError.getEvolWrapperError( (EvolWrapperErrorInfo::EvolWrapperErrorSource) deviceExt );

				#if 0 /* removed for Albany store */
				//Debug::trace("WrapperManager.Recover", "Error: " + Integer::toString(errorCode) );
				if( errorCode  == EvolWrapperErrorInfo::PackageDimensionError ) {
					dimensionErrors_++;

					if( dimensionErrors_ == 6 ) {
						int result = system("/usr/bin/python /usr/local/hobart/python/displayWaitMsg.py");
						if( result == -1 )
							Debug::trace("WrapperManager.Recover", "Failed to display wait dialog.");

						result = system("/sbin/reboot");
						if( result == -1 )
							Debug::trace("WrapperManager.Recover", "Failed to reboot the system.");
					}
				}
				#endif

				//	Retain the current state of lastEvolWrapperStatus.isPeripheralComm()
				//	for when we call receivedWrapperFeedback() at the end.
				bool isComm = lastEvolWrapperStatus.isPeripheralComm();
				if (Debug::isTracing("WrapperManager.WrapperStatus") == true)
				{
					statusMsg.show();
				}

				statusMsg.setSourceObject(this);
				if (isComm)
					dispatchEvent(&statusMsg);

				lastEvolWrapperStatus = statusMsg;
				lastEvolWrapperStatus.setPeripheralComm(isComm);
			}
			statusMsg.show();
			receivedWrapperFeedback();
			break;
		}
		case WrConfig:
		{
			Debug::trace("WrapperManager.Message", "WrConfig");
			if( !fullBrightCan_ )  {
				WrapperConfiguration wc(recvBuffer);
				wc.setSourceObject(this);
				const char *pCC = wc.serialize();
				pMyWrapper->send(pCC, wc.getSerializedLength());
				wc.show();
			}
			break;
		}
		case WrEvolConfig:
		{
			Debug::trace("WrapperManager.Message", "WrEvolConfig");

			if( !fullBrightCan_ )  {
				EvolWrapperConfiguration wc(recvBuffer);
				wc.setSourceObject(this);
				const char *pCC = wc.serialize();
				pMyWrapper->send(pCC, wc.getSerializedLength());
				wc.show();
			}
			break;
		}
		case WrAbort:
		{
			Debug::trace("WrapperManager.Message", "WrAbort");

			sendMessageToCommMgr(recvBuffer);

			break;
		}
		case WrEnable:
		{
			Debug::trace("WrapperManager.Message", "WrEnable");

			sendMessageToCommMgr(recvBuffer);

			break;
		}
		case WrDisable:
		{
			Debug::trace("WrapperManager.Message", "WrDisable");

			sendMessageToCommMgr(recvBuffer);

			break;
		}
		case WrReset:
		{
			Debug::trace("WrapperManager.Message", "WrReset");

			sendMessageToCommMgr(recvBuffer);

			break;
		}
		case WrResume:
		{
			Debug::trace("WrapperManager.Message", "WrResume");

			sendMessageToCommMgr(recvBuffer);

			break;
		}
		case WrPause:
		{
			Debug::trace("WrapperManager.Message", "WrPause");

			sendMessageToCommMgr(recvBuffer);

			break;
		}
		case WrReqErrorHistory:
		{
			Debug::trace("WrapperManager.Message", "WrReqErrorHistory");

			sendMessageToCommMgr(recvBuffer);

			lastErrorHistoryMsg = WrapperErrorHistory();
			requestedErrorHistory_ = true;

			break;
		}
		case WrMode:			
		{
			Debug::trace("WrapperManager.Message", "WrMode");
			WrapperMode wm(recvBuffer);
			wm.setSourceObject(this);

			//SOF-1142
			if( fullBrightCan_ ) {
				if( wm.getWrOpMode() == WrServiceMode ) {
					cancelTimer(wrapperProbe);
					pollTimerStopped_ = true;
				}
				if( ( wm.getWrOpMode() == WrOperateMode ) && pollTimerStopped_ ) {
					probeTimeoutCntr_ = 0;
					createTimer( wrapperProbeTime_, wrapperProbe, CHobartTimer::PERIODIC_TIMER );
					pollTimerStopped_ = false;
				}
			}

			if( ( !fullBrightCan_ ) && ( !partialCan_ ) ) {
				const char *pCC = wm.serialize();
				pMyWrapper->send(pCC, wm.getSerializedLength());
			} else {
				dispatchEvent(&wm);
			}
			break;
		}
		case WrTest:			
		{
			Debug::trace("WrapperManager.Message", "WrTest");

			WrapperTest wt(recvBuffer);
			wt.setSourceObject(this);

			const char *pCC = wt.serialize();
			pMyWrapper->send(pCC, wt.getSerializedLength());

			if (Debug::isTracing("WrapperManager.WrapperTest") == true)
			{
				wt.show();
			}

			break;
		}
		case WrReqSensors:
		{
			Debug::trace("WrapperManager.Message", "WrReqSensors");
			if( !fullBrightCan_ )  {
				sendMessageToCommMgr(recvBuffer);
			}
			lastSenseMsg = WrapperSensors();

			break;
		}
		case WrSensorsTracking:		 
		{
			Debug::trace("WrapperManager.Message", "WrSensorsTracking");

			WrapperSensorsTracking wrTracking(recvBuffer);

			trackSensors = wrTracking.getSensorsTracking();

			if (trackSensors == true)
			{
				lastSenseMsg = WrapperSensors();

				createTimer(requestSensorTime*10, sensorTimerExpired, CHobartTimer::PERIODIC_TIMER);

				requestSensorMsg();
			}
			else
			{
				cancelTimer( sensorTimerExpired );
			}
			break;
		}
		case WrSensors: 
		{
			Debug::trace("WrapperManager.Message", "WrSensors");

			WrapperSensors wrapperSense(recvBuffer);
			wrapperSense.setSourceObject(this);

			if (wrapperSense.equals(lastSenseMsg) == false)
			{
				if ((currentOpMode == OpModeService) &&
						(wrapperSense.getEncoderCount() == MarkPosition))
				{
					Debug::trace("WrapperManager.Error", "Send Beep Message");
				}

				lastSenseMsg = wrapperSense;
				dispatchEvent(&wrapperSense);
			}

			receivedWrapperFeedback();
			break;
		}
		case WrErrorHistoryTracking:			
		{
			Debug::trace("WrapperManager.Message", "WrErrorHistoryTracking -- warning this code has been removed");

			WrapperErrorHistoryTracking wrErrorHistoryTracking(recvBuffer);

			break;
		}
		case WrErrorHistory:	
		{
			Debug::trace("WrapperManager.Message", "WrErrorHistory");

			WrapperErrorHistory wrapperErrorHistory(recvBuffer);
			wrapperErrorHistory.setSourceObject(this);

			if (wrapperErrorHistory.equals(lastErrorHistoryMsg) == false || requestedErrorHistory_ )
			{
				if (Debug::isTracing("WrapperManager.ErrorHistory") == true)
				{
					wrapperErrorHistory.show();
				}

				lastErrorHistoryMsg = wrapperErrorHistory;
				dispatchEvent(&wrapperErrorHistory);
				requestedErrorHistory_ = false;
			}
			receivedWrapperFeedback();
			break;
		}						
		case WrReqInquire:
		{
			Debug::trace("WrapperManager.Message", "WrReqInquire");

			if( !fullBrightCan_ )  {
				requestInquireMsg();

			}
			break;
		}
		case WrReqWakeup:
		{
			if (WrapperManager::ignoreWakeups == 0)
			{
				Debug::trace("WrapperManager.ProbeWrapper", "WrReqWakeup");

				WrapperRequestWakeup wrw(recvBuffer);
				wrw.setSourceObject(this);
				wrw.setPhysicalAddress(physicalAddress);

				if( !fullBrightCan_ ) {
					const char *pCC = wrw.serialize();
					pMyWrapper->send(pCC, wrw.getSerializedLength());
				}
			}
			else
			{
				//lostContact = true;
				Debug::trace("WrapperManager.ProbeWrapper", "Set for ignoring WrReqWakeup");
			}
			
			break;
		}
		case WrReqKind:
		{
			Debug::trace("WrapperManager.Message", "WrReqKind");

			sendWrapperKind();

			break;
		}
		case WrWrap:
		{
			Debug::trace("WrapperManager.Message", "WrWrap");

			WrapperWrap ww(recvBuffer);
			ww.setSourceObject(this);

			const char *pCC = ww.serialize();
			pMyWrapper->send(pCC, ww.getSerializedLength());

			ww.show();
			break;
		}
		case WrCycle:
		{
			Debug::trace("WrapperManager.Message", "WrCycle");

			WrapperCycle wc(recvBuffer);
			wc.setSourceObject(this);

			const char *pCC = wc.serialize();
			pMyWrapper->send(pCC, wc.getSerializedLength());

			if (Debug::isTracing("WrapperManager.WrapperCycle") == true)
			{
				wc.show();
			}

			break;
		}
		case WrJog:
		{
			Debug::trace("WrapperManager.Message", "WrJog");

			WrapperJog wj(recvBuffer);
			wj.setSourceObject(this);

			const char *pCC = wj.serialize();
			pMyWrapper->send(pCC, wj.getSerializedLength());
			
			break;
		}
		case WrControl:
		{
			Debug::trace("WrapperManager.Message", "WrControl");

			WrapperControl wrControl(recvBuffer);
			wrControl.setSourceObject(this);

			const char *pCC = wrControl.serialize();
			pMyWrapper->send(pCC, wrControl.getSerializedLength());
	
			break;
		}
		case WrLoad:
		{
			Debug::trace("WrapperManager.Message", "WrLoad");
			if( !fullBrightCan_ )  {

				WrapperLoad wrLoad(recvBuffer);
				wrLoad.setSourceObject(this);

				const char *pCC = wrLoad.serialize();
				pMyWrapper->send(pCC, wrLoad.getSerializedLength());
			}
			break;
		}

		case WrEvolControl:
		{
			Debug::trace("WrapperManager.Message", "WrEvolControl");
			if( !fullBrightCan_ )  {

				EvolWrapperControl wc(recvBuffer);
				wc.setSourceObject(this);
				const char *pCC = wc.serialize();
				pMyWrapper->send(pCC, wc.getSerializedLength());

				if (Debug::isTracing("EvolWrapperControl") == true)
					wc.show();
			}
			break;
		}
		case WrEvolLoad:
		{
			Debug::trace("WrapperManager.Message", "WrEvolLoad");
			if( !fullBrightCan_ )  {
				EvolWrapperLoad wrLoad(recvBuffer);
				wrLoad.setSourceObject(this);

				const char *pCC = wrLoad.serialize();
				pMyWrapper->send(pCC, wrLoad.getSerializedLength());
			}
			break;
		}
		case WrEvolWrap:
		{
			Debug::trace("WrapperManager.Message", "WrEvolWrap");

			// SOF-1105
			if ( !Configuration::getInstance()->isScaleModelWrapOnly() )
			{
				evolWrapperCycleCount_++;
				Debug::trace("WrapperManager.Timer", "WrEvolWrap:  evolWrapperCycleCount_ = " + Integer::toString(evolWrapperCycleCount_));
			}

			if( ( !fullBrightCan_ ) && ( !partialCan_ ) ) {
				EvolWrapperWrap ww(recvBuffer);
				ww.setSourceObject(this);
				const char *pCC = ww.serialize();
				pMyWrapper->send(pCC, ww.getSerializedLength());

				ww.show();
			}
			break;
		}
		case WrSetGripperPWMTimings:
		{
			Debug::trace("WrapperManager.Message", "WrSetGripperPWMTimings");

			// Send a ReqInquire so that with the reply we send the Gripper PWM Timings
			Debug::trace("WrapperManager.PWMTimings", "Sending a request for inquire");
			WrapperRequestInquire wri(this);
			const char *pCC = wri.serialize();
			pMyWrapper->send(pCC, wri.getSerializedLength());

			Debug::trace("WrapperManager.PWMTimings", "Setting sendGripperPWMTimings_ to true");
			sendGripperPWMTimings_ = true;

			break;
		}
		case WrReqPackageHistory:
		{
			Debug::trace("WrapperManager.Message", "WrReqPackageHistory");
			if (wrKind == WrUltimaWrapper)
				dispatchEvent(&wrapperPackageHistory_);
			else if (wrKind == WrEvolWrapper)
			{				
				evolWrapperPackageHistory_.setSourceObject(this);
				dispatchEvent(&evolWrapperPackageHistory_);
			}	
			break;
		}
		
		case WrEvolRequestPackageTimings:
		{
			Debug::trace("WrapperManager.Message", "WrEvolRequestPackageTimings");
			if( !fullBrightCan_ )  {
				EvolWrapperRequestPackageTimings wrTimings(recvBuffer);
				wrTimings.setSourceObject(this);

				const char *pCC = wrTimings.serialize();
				pMyWrapper->send(pCC, wrTimings.getSerializedLength());
			}
			break;
		}

		case WrEvolCamerasTracking:		
		{
			Debug::trace("WrapperManager.Message", "WrEvolCamerasTracking");

			EvolWrapperCamerasTracking wrTracking(recvBuffer);

			if ( !(trackCameras = wrTracking.getCamerasTracking()) )
				cancelTimer( cameraTracking );
			else
			{
				createTimer( CAMERA_TRACKING_TIMER, cameraTracking, CHobartTimer::PERIODIC_TIMER );
				requestCamerasScanMsg( trackCameraNumber = EvolWrapperTest::LeftCameraScanAnalog );
			}

			break;
		}

		case WrCameraScan:	
		{
			Debug::trace("WrapperManager.Message", "WrCameraScan");

			EvolWrapperCameraScan cameraScan(recvBuffer);
			cameraScan.setSourceObject(this);

			if ((cameraScan.getCamera() == LeftCamera && !cameraScan.equals(lastLeftScan)) ||
				 (cameraScan.getCamera() == RightCamera && !cameraScan.equals(lastRightScan)))
			{
				if (cameraScan.getCamera() == LeftCamera)
					lastLeftScan = cameraScan;
				else
					lastRightScan = cameraScan;
				dispatchEvent(&cameraScan);
			}

			receivedWrapperFeedback();
			break;
		}
		case WrCameraCalibration:
		{
			Debug::trace("WrapperManager.Message", "WrCameraCalibration");
			EvolWrapperCameraCalibration cameraCal(recvBuffer);
			cameraCal.setSourceObject(this);
			cameraCal.show();
			
			dispatchEvent(&cameraCal);
			
			receivedWrapperFeedback();
			break;
		}

		case WrEvolDiagnosticsTest:
		{
			Debug::trace("WrapperManager.Message", "WrEvolDiagnosticsTest");
			if( !fullBrightCan_ )  {
				EvolWrapperDiagnosticsTest wrDiag(recvBuffer);
				wrDiag.setSourceObject(this);
				const char *pCC = wrDiag.serialize();

				pMyWrapper->send(pCC, wrDiag.getSerializedLength());
			}
			break;
		}

		case WrEvolDiagnosticsTestResult:
		{
			Debug::trace("WrapperManager.Message", "WrEvolDiagnosticsTestResult");
			EvolWrapperDiagnosticsTestResult wrDiag(recvBuffer);
			wrDiag.setSourceObject(this);

			dispatchEvent(&wrDiag);
			break;
		}

		case WrEvolTest:		
		{
			Debug::trace("WrapperManager.Message", "WrEvolTest");

			EvolWrapperTest *wt = new EvolWrapperTest(recvBuffer);
			if (wt)
			{
				wt->setSourceObject(this);
				sendEvolWrapperTest(wt);
			}

			//We are fakeing the label ready message only during a WrapperCycleTest since
			//the Printer Manager requires a valid transaction before sending. The wrapper cycle button
			//in service mode wrapper timings does not require a PLU.
			HobString test;
			wt->getTestName(wt->getTestCode(), test);
			Debug::trace("WrapperManager.LabelReady", test);
			if( test == "WrapperCycleTest" ) {
				//fake out the label ready message for the primary printer.
				LabelApplierLabelReady ready( this, PrimaryPrinterStation, 1 );
				pMyWrapper->send( ready.serialize(), ready.getSerializedLength() );
			}
			break;
		}
		case WrEvolTestComplete:		
		{
			Debug::trace("WrapperManager.Message", "WrEvolTestComplete");

			EvolWrapperTestComplete tcMsg(recvBuffer);
			tcMsg.setSourceObject(this);
			tcMsg.show();

			dispatchEvent(&tcMsg);

			receivedWrapperFeedback();
			break;
		}
		case WrEvolSensors:	
		{
			Debug::trace("WrapperManager.Message", "WrEvolSensors");

			EvolWrapperSensors wrapperSensors(recvBuffer);
			wrapperSensors.setSourceObject(this);

			dispatchEvent(&wrapperSensors);

			receivedWrapperFeedback();
			break;
		}
		case WrEvolCycle:
		{
			Debug::trace("WrapperManager.Message", "WrEvolCycle");

			EvolWrapperCycle wc(recvBuffer);
			wc.setSourceObject(this);

			const char *pCC = wc.serialize();
			pMyWrapper->send(pCC, wc.getSerializedLength());

			if (Debug::isTracing("WrapperManager.WrapperCycle") == true)
			{
				wc.show();
			}
			break;
		}
		case WrEvolPackageDesc:	
		{
			Debug::trace("WrapperManager.Message", "WrEvolPackageDesc");

			// SOF-1105
			// no WrEvolWrap command for wrap only model (no weigher), so use WrEvolPackageDesc
			// for cycle counter instead
			if ( Configuration::getInstance()->isScaleModelWrapOnly() )
			{
				evolWrapperCycleCount_++;
				Debug::trace("WrapperManager.Timer", "WrEvolWrap:  evolWrapperCycleCount_ = " + Integer::toString(evolWrapperCycleCount_));
			}

			EvolWrapperPackageDesc wrPackageDesc(recvBuffer);
			wrPackageDesc.setSourceObject(this);

			string trayName;
			if (wrTrayNames_)
				wrTrayNames_->getTrayName(wrPackageDesc.getWrapBoxIndex(), trayName);
			else
				Debug::trace("WrapperManager", "WrEvolPackageDesc:  wrTrayNames_ is 0!");
			Debug::trace("WrapperManager", "WrEvolPackageDesc:  trayName is " + trayName);
			
			//	Add the tray name to the wrPackageDesc msg also.
			wrPackageDesc.setTrayName(trayName);

			//	Add this wrPackageDesc to the history before we send it to the browser.
			addPackageToEvolHistory(wrPackageDesc);

			dispatchEvent(&wrPackageDesc);

			receivedWrapperFeedback();
			break;
		}
		case WrEvolPackageTimings:
		{
			Debug::trace("WrapperManager.Message", "WrEvolPackageTimings");

			EvolWrapperPackageTimingsPointer wrTimingsPtr(recvBuffer);
			wrTimingsPtr.setSourceObject(this);

			//	Set up the tray names every time we receive this message, provided it is not the Factory defaults.
			EvolWrapperPackageTimings *wrapperTimings = wrTimingsPtr.getTimingsPointer();

			if (wrapperTimings)
			{
				EvolWrapperTrayNames *trayNames = new EvolWrapperTrayNames(wrapperTimings);
				if (trayNames)
				{
					Debug::trace("WrapperManager.Message", "WrEvolPackageTimings:  Setting the new tray names.");
					setEvolWrapperTrayNames(trayNames);
				}
			}

			if (requestedPkgTimings_)
			{
				// this creates a small memory leak but allows more than one manager to
				// process this message (and prevents a crash)
				//if (wrapperTimings)
				//{
				//	delete wrapperTimings;
				//}
				requestedPkgTimings_ = false;
			}
			else
			{
				dispatchEvent(&wrTimingsPtr);
			}

			receivedWrapperFeedback();
			break;
		}
		case WrEvolSavePackageTimings:
		{
			Debug::trace("WrapperManager.Message", "WrEvolSavePackageTimings");
			if( !fullBrightCan_ )  {
				EvolWrapperPackageTimingsPointer wrTimingsPtr(recvBuffer);
				wrTimingsPtr.setSourceObject(this);

				EvolWrapperPackageTimings *wrapperTimings = wrTimingsPtr.getTimingsPointer();
				if (wrapperTimings && (wrapperTimings->getDisposition() == WRITE_PERMANENT || wrapperTimings->getDisposition() == WRITE_TEMPORARY))
				{
					Debug::trace("WrapperManager", "WrEvolSavePackageTimings:  Getting the tray names from the message.  Disposition = " + HobString(Integer::toString(wrapperTimings->getDisposition())));
					EvolWrapperTrayNames *trayNames = new EvolWrapperTrayNames(wrapperTimings);
					if (trayNames)
					{
						Debug::trace("WrapperManager", "WrEvolSavePackageTimings:  Setting the new tray names.");
						setEvolWrapperTrayNames(trayNames);
					}
					const char *pCC = wrTimingsPtr.serialize();
					if (pCC)
					{
						pMyWrapper->send(pCC, wrTimingsPtr.getSerializedLength());
						savingWrapperTimings_ = true;
					}
					else
					{
						Debug::trace("WrapperManager", "WrEvolSavePackageTimings:  Unable to send msg. pCC is NULL!");
					}
				}
			} else {
				//send the size of the wrap box data and wait for ack.
				EvolWrapperPackageTimingsPointer wrTimingsPtr(recvBuffer);
				EvolWrapperPackageTimings *pTimings = pMyTimings_= wrTimingsPtr.getTimingsPointer();

				Debug::trace("WrapperManager", "WrEvolSavePackageTimings:  Getting the tray names from the message");
				EvolWrapperTrayNames *trayNames = new EvolWrapperTrayNames( pTimings );
				if (trayNames)
				{
					Debug::trace("WrapperManager", "WrEvolSavePackageTimings:  Setting the new tray names.");
					setEvolWrapperTrayNames(trayNames);
				}

				Debug::trace("EvolWrapperPackageTimings", "Saving:  pMyTimings_ = " + Hex::toString((void*) pMyTimings_));
				if( pTimings != NULL ) {
					int numWrapBoxes = pTimings->getNumberWrapBoxes();
					int numHeightBoxes = 0;
					int totalSize = 0;

					for ( int i = 0; i < numWrapBoxes; ++i )
					{
						numHeightBoxes += pTimings->getWrapBox(i)->getNumberHeightBoxes();
					}
					totalSize = WrapBox::_WrapBoxOffset_ + ( numWrapBoxes * WrapBox::_WrapBoxSizeCan_) + ( numHeightBoxes * WrapBox::_HeightBoxSizeCan_ );
					WrapBoxSize msg( this, totalSize );
					dispatchEvent(&msg);
					savingWrapperTimings_ = true;
				} else {
					Debug::trace("WrapperManager", "WrEvolSavePackageTimings:  Unable to send msg. EvolWrapperPackageTimings is NULL!");
				}
			}
			//SOF-???
			receivedWrapperFeedback();
			break;
		}
		case WrWrapBoxAck:
		{
			Debug::trace("WrapperManager.Message", "WrWrapBoxAck");
			if( pMyTimings_ ) {
				Debug::trace("EvolWrapperPackageTimings", "Sending:  pMyTimings_ = " + Hex::toString((void*) pMyTimings_));
    			EvolWrapperPackageTimingsPointer timings( WrEvolPackageTimingsWrite, pMyTimings_ );
    			dispatchEvent( &timings );
			}
			break;
		}
		case WrWrapBoxTransferComplete:
		{
			pMyTimings_ = NULL;

			Debug::trace("WrapperManager.Message", "WrWrapBoxTransferComplete");

			WrapperGeneric wsptc(WrEvolSavePackageTimingsComplete, this);
			dispatchEvent(&wsptc);

			receivedWrapperFeedback();

			savingWrapperTimings_ = false;
			if (waitingToCycleWrapper_)
			{
				sendEvolWrapperTest();
				waitingToCycleWrapper_ = false;
			}
			break;
		}
		case WrEvolResetDefaultPackageTimings:
		{
			Debug::trace("WrapperManager.Message", "WrEvolResetDefaultPackageTimings");
			if( !fullBrightCan_ )  {
				EvolWrapperResetDefaultPackageTimings resetTimings(recvBuffer);
				resetTimings.setSourceObject(this);

				const char *pCC = resetTimings.serialize();
				pMyWrapper->send(pCC, resetTimings.getSerializedLength());
			}
			break;
		}
		case WrEvolUnlockPackageTimings:
		{
			Debug::trace("WrapperManager.Message", "WrEvolUnlockPackageTimings");
			if (!fullBrightCan_) {
				EvolWrapperUnlockPackageTimings unlockTimings(recvBuffer);
				unlockTimings.setSourceObject(this);

				const char* pCC = unlockTimings.serialize();
				pMyWrapper->send(pCC, unlockTimings.getSerializedLength());
			}
			break;
		}
		case WrEvolSavePackageTimingsComplete:
		{
			Debug::trace("WrapperManager.Message", "WrEvolSavePackageTimingsComplete");
			WrapperGeneric wsptc(WrEvolSavePackageTimingsComplete, this);

			dispatchEvent(&wsptc);

			receivedWrapperFeedback();

			savingWrapperTimings_ = false;
			if (waitingToCycleWrapper_)
			{
				sendEvolWrapperTest();
				waitingToCycleWrapper_ = false;
			}
			break;
		}
		case WrEvolSendDateTime:
		{
			break;
		}
		case WrEvolCameraHeightScanTracking:		
		{
			Debug::trace("WrapperManager.Message", "WrEvolCameraHeightScanTracking");

			EvolWrapperCamerasTracking wrTracking(recvBuffer);

			if ( !(trackCameras = wrTracking.getCamerasTracking()) )
				cancelTimer( heightScanTracking );
			else
			{
				createTimer( HEIGHT_SCAN_TRACKING_TIMER, heightScanTracking, CHobartTimer::PERIODIC_TIMER );
				requestCamerasScanMsg( EvolWrapperTest::CameraHeight );
			}
			break;
		}
		case WrCameraHeightScan:
		{
			Debug::trace("WrapperManager.Message", "WrCameraHeightScan");
			EvolWrapperCameraHeightScan heightScan(recvBuffer);
			heightScan.setSourceObject(this);

			dispatchEvent(&heightScan);

			receivedWrapperFeedback();
			break;
		}
		case WrEvolUpgradeInProgress:
		{
			Debug::trace("WrapperManager.Message", "WrEvolUpgradeInProgress");
			
			//bool might not be needed since we can cancel the probe timer here! -- ats
			upgradeInProgress = true;
			
			cancelTimer(wrapperProbe);

			break;
		}
		case WrEvolEventCapture:
		{
			Debug::trace("WrapperManager.Message", "WrEvolEventCapture");
			EvolWrapperEventCapture msg(recvBuffer);
			
			Debug::trace("WrapperManager.Message", HobString("event capture type = ") + Integer::toString(msg.getEventCaptureType()));
			Debug::trace("WrapperManager.Message", HobString("event pkg id = ") + Integer::toString(msg.getPackageId()));
			Debug::trace("WrapperManager.Message", HobString("event value = ") + Long::toString(msg.getValue()));
			Debug::trace("WrapperManager.Message", HobString("event error num = ") + Bool::toString(msg.getErrorNumber()));


			//get the current date and time to stamp this knifeTime event
			HobDateTime dt;

			msg.setEventMonth(dt.getMonth());
			msg.setEventDay(dt.getDay());
			msg.setEventYear(dt.getYear());
			msg.setEventHour(dt.getHour());
			msg.setEventMinutes(dt.getMinute());
			msg.setEventSeconds(dt.getSecond());
			
			msg.setSourceObject(this);
			dispatchEvent(&msg);

			receivedWrapperFeedback();
			break;
		}
		case WrEvolErrorHistory:
		{
			EvolWrapperErrorHistory evolWrapperErrorHistory(recvBuffer);
			evolWrapperErrorHistory.setSourceObject(this);
			
			//do not log any error message from the wrapper with an error code of NoWrapperError (0).
			if(evolWrapperErrorHistory.getWrapperError() != (UINT16)EvolWrapperErrorInfo::NoWrapperError) {
				dispatchEvent(&evolWrapperErrorHistory);
			}
			else
				Debug::trace("WrapperManager.Message", "received error message with a code of zero from Wrapper");
			receivedWrapperFeedback();

			break;
		}
#ifdef USE_CYCLE_COUNTER
		case WrEvolReqCycleCount:
		{
			Debug::trace("WrapperManager.Message", "WrEvolReqCycleCount:  evolWrapperCycleCount_ = " + inttostring(evolWrapperCycleCount_));
			EvolWrapperCycleCount evolWrapperCycleCount(this, evolWrapperCycleCount_);
			
			dispatchEvent(&evolWrapperCycleCount);
			break;
		}
		case WrEvolCycleCount:
		{
			Debug::trace("WrapperManager.Message", "WrEvolCycleCount");
			EvolWrapperCycleCount evolWrapperCycleCount(recvBuffer);
			evolWrapperCycleCount.setSourceObject(this);
			
			evolWrapperCycleCount_ = evolWrapperCycleCount.getWrapperCycleCount();
			
			dispatchEvent(&evolWrapperCycleCount);
			
			receivedWrapperFeedback();
			break;
		}
		case WrEvolSaveCycleCount:
		{
			Debug::trace("WrapperManager.Message", "WrEvolSaveCycleCount");
			EvolWrapperSaveCycleCount evolWrapperSaveCycleCount(recvBuffer);
			evolWrapperSaveCycleCount.setWrapperCycleCount(evolWrapperCycleCount_);
			
			evolWrapperSaveCycleCount.setSourceObject(this);

			const char *pCC = evolWrapperSaveCycleCount.serialize();
			pMyWrapper->send(pCC, evolWrapperSaveCycleCount.getSerializedLength());

			break;
		}
#endif
		case WrEvolStartMfgRunin:
		{
			Debug::trace("WrapperManager.Message", "WrEvolStartMfgRunin");
			if( !fullBrightCan_ )  {
				WrapperGeneric msg(WrEvolStartMfgRunin, this);

				const char *pCC = msg.serialize();
				pMyWrapper->send(pCC, msg.getSerializedLength());
			}
			break;
		}
		case WrEvolPkgDetectorStatus:
		{
			EvolWrapperPkgDetectorStatus evolWrapperPkgDetectorStatus(recvBuffer);
			
			if(evolWrapperPkgDetectorStatus.isSensorLocationElevator())
			{
				elevatorDetectorStatus = evolWrapperPkgDetectorStatus;
			}
			else if(evolWrapperPkgDetectorStatus.isSensorLocationSealer())
			{
				sealerDetectorStatus = evolWrapperPkgDetectorStatus;
			}

			receivedWrapperFeedback();
			break;
		}
		case WrEvolReqElevatorPkgDetectorStatus:
		{
			elevatorDetectorStatus.setSourceObject(this);
			
			dispatchEvent(&elevatorDetectorStatus);

			break;
		}
		case WrEvolReqSealerPkgDetectorStatus:
		{
			sealerDetectorStatus.setSourceObject(this);

			dispatchEvent(&sealerDetectorStatus);
			break;
		}
		case WrEvolRequestCameraThreshold:
		{
			Debug::trace("WrapperManager.Message", "WrEvolRequestCameraThreshold");
			sendMessageToCommMgr(recvBuffer);

			break;
		}
		case WrEvolCameraThreshold:
		{
			Debug::trace("WrapperManager.Message", "WrEvolCameraThreshold");
			EvolWrapperCameraThreshold evolWrapperCameraThreshold(recvBuffer);
			
			evolWrapperCameraThreshold.setSourceObject(this);
			
			dispatchEvent(&evolWrapperCameraThreshold);

			receivedWrapperFeedback();
			break;
		}
		case WrEvolWriteCameraThreshold:
		{
			Debug::trace("WrapperManager.Message", "WrEvolWriteCameraThreshold");
			if ( !fullBrightCan_)
			{
				EvolWrapperWriteCameraThreshold evolWrapperWriteCameraThreshold(recvBuffer);
				evolWrapperWriteCameraThreshold.setSourceObject(this);

				const char *pCC = evolWrapperWriteCameraThreshold.serialize();
				pMyWrapper->send(pCC, evolWrapperWriteCameraThreshold.getSerializedLength());
				
				//Camera board needs delay after a write of threshold before starting scan again.
				PAL_absclass::PAL_sleep(300);
			}
			break;
		}
		case WrEvolSystemInfo:
		{
			Debug::trace("WrapperManager.Message", "WrEvolSystemInfo");

			EvolWrapperSystemInfo ewsi(recvBuffer);			
			ewsi.setSourceObject(this);
			ewsi.setAwsCycleCount(evolWrapperCycleCount_);

			dispatchEvent(&ewsi);

			receivedWrapperFeedback();

			break;
		}
		case WrReqEvolSystemInfo:
		{
			Debug::trace("WrapperManager.Message", "WrReqEvolSystemInfo");
			if( !fullBrightCan_ )  {
				sendMessageToCommMgr(recvBuffer);
			}

			break;
		}
		case WrEvolSystemInfoErase:
		{
			Debug::trace("WrapperManager.Message", "WrEvolSystemInfoErase");
			if( !fullBrightCan_ )  {
				sendMessageToCommMgr(recvBuffer);
			}
			break;		
		}
		case WrEvolSystemInfoWrite:
		{
			Debug::trace("WrapperManager.Message", "WrEvolSystemInfoWrite  ***** Start *****");
			if (!pMyWrapper)
				Debug::trace("WrapperManager", "WrEvolSystemInfoWrite:  Unable to send msg. pMyWrapper is NULL!");
			else
			{
				EvolWrapperSystemInfoWrite ewsiw(recvBuffer);
				ewsiw.setSourceObject(this);

				const char *pCC = ewsiw.serialize();
				if (pCC) {
					if( !fullBrightCan_ )  {
						pMyWrapper->send(pCC, ewsiw.getSerializedLength());
					}
				} else {
					Debug::trace("WrapperManager", "WrEvolSystemInfoWrite:  Unable to send msg. pCC is NULL!");
				}
				
				// SOF-2402
				// The intent of allowing cycle count to be changed is for new boards only. For
				// example, a service tech replaces the main board on an existing wrapper and
				// wishes to update the cycle counter to actual.
				//
				// Rules:
				// 1) do not allow cycles to be zeroed out
				// 2) do not allow cycles to be reduced if we are over 1000 cycles (1000 give some leeway for new boards)
				// 3) increasing cycle count is allowed no matter existing cycle counts
				// 4) when manufacturing key present allow anything
				if ( ewsiw.getAwsCycleCount() )
					if (( evolWrapperCycleCount_ < 1000 ) || ( evolWrapperCycleCount_ < ewsiw.getAwsCycleCount() ) || ( isManfTestKeyPresent_ ))
					{
						evolWrapperCycleCount_ = ewsiw.getAwsCycleCount();

						std::thread t( &WrapperManager::writeCycleCountsToFlashThread, this );
						t.detach();
					}
			}
			break;	
		}
		case WrEvolManufacturingRunInNotice:
		{

			Debug::trace("WrapperManager.Message", "WrEvolManufacturingRunInNotice");
			WrapperGeneric	evolManufacturingRunInNotice(recvBuffer);			
			evolManufacturingRunInNotice.setSourceObject(this);
			
			dispatchEvent(&evolManufacturingRunInNotice);

			receivedWrapperFeedback();

			break;
		}
		case WrEvolMotorProfile:
		{
			Debug::trace("WrapperManager.Message", "WrEvolMotorProfile");
			EvolWrapperMotorProfile evolWrapperMotorProfile(recvBuffer);
			evolWrapperMotorProfile.setSourceObject(this);

			dispatchEvent(&evolWrapperMotorProfile);

			receivedWrapperFeedback();

			break;
		}
		case WrEvolPackageDischarge:
		{
			Debug::trace("WrapperManager.Message", "WrEvolPackageDischarge");
			EvolWrapperPackageDischarge ewpd(recvBuffer);
			ewpd.setSourceObject(this);
			ewpd.show();

			dispatchEvent(&ewpd);

			receivedWrapperFeedback();
			break;
		}
		case WrReqUIWrapperModel:
		{
			Debug::trace("WrapperManager.Message", "WrReqUIWrapperModel:  requestWrapperModelUI_ = " + booltostring(requestWrapperModelUI_));

			EvolWrapperReqUIModel evolWrapperReqUIModel(recvBuffer);
			evolWrapperReqUIModel.setSourceObject(this);
			
			//send this request only once per powerup.
			if(!requestWrapperModelUI_)
			{
				requestWrapperModelUI_ = true;			
				dispatchEvent(&evolWrapperReqUIModel);
			}
			break;
		}
		case WrUIWrapperModel:
		{
			Debug::trace("WrapperManager.Message", "WrUIWrapperModel");

			EvolWrapperUIModel evolWrapperUIModel(recvBuffer);
			evolWrapperUIModel.setSourceObject(this);

			const char *pCC = evolWrapperUIModel.serialize();
			pMyWrapper->send(pCC, evolWrapperUIModel.getSerializedLength());
			
			break;	
		}
		case WrNGWEngInfoN1:
		case WrNGWEngInfoN2:
		case WrNGWEngInfoN3:
		case WrNGWEngInfoN4:
		case WrNGWEngInfoN5:
		case WrNGWEngInfoN6:
		{
			NGWEngInfo info(recvBuffer);
			Debug::trace("NGWEngInfo", info.getEngInfo() );
			break;
		}
		case WrNGWENGException:
		{
			NGWEngException exception(recvBuffer);
			exception.show();
			break;
		}
		case WrWrapBoxSize:
		{
			WrapperGeneric msg(WrWrapBoxAck, this);
			dispatchEvent( &msg );
			break;
		}
		case WrPPPRequest: 
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPRequest"); 
			PPPRequest req( recvBuffer );		
			const char *pCC = req.serialize();
			pMyWrapper->send( pCC, req.getSerializedLength() );
			break;
		}
		case WrPPPWakeup: 
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPWakeup");
			PPPWakeup wake( recvBuffer );
			wake.show();
			dispatchEvent( &wake );
			
			if( pppPollStart_ ) {
				Debug::trace("WrapperManager.PPPTimer", "Cancel PPP probe timer");
				cancelTimer(pppProbeTimer);
				pppPollStart_ = false;
			}
			break;
		}
		case WrPPPConfig: 
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPConfig");
			PPPConfig pppConfig( recvBuffer );
			pppConfig.show();
			dispatchEvent( &pppConfig );
			break;
		}
		case WrPPPDist: 
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPDist");
			PPPDist dist( recvBuffer );
			dist.show();
			dispatchEvent( &dist );

			if( pppPollStart_ ) {
				Debug::trace("WrapperManager.PPPTimer", "Cancel PPP probe timer");
				cancelTimer(pppProbeTimer);
				pppPollStart_ = false;
			}
			break;
		}
		case WrPPPCalibration: 
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPCalibration");
			PPPCalib cal( recvBuffer );
			cal.show();
			dispatchEvent( &cal );
			break;
		}
		case WrPPPVersion: 
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPVersion");
            PPPVersion ver( recvBuffer );
            ver.show();
			dispatchEvent( &ver );
			break;
		}
		case WrPPPStatus:
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPStatus");
			PPPStatus status( recvBuffer );			
			status.show();
			dispatchEvent( &status );
			break;
		}
		case WrWrapperPause:
		{
			// SOF-5459 Printer Plate Adjust Message - Do not need this for a single sliding 2nd printer so can it all
			Debug::trace("WrapperManager.PPPMsg", "handleWrapperEvent() case WrWrapperPause: prolly should not do this anymore");
			//NGWWrapperPause msg( recvBuffer );
			//msg.show();
			//dispatchEvent( &msg );
			break;
		}
		case WrPPPTest: 
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPTest");
			PPPTest test( recvBuffer );
			if( test.getStartTest() ) {
				if( !pppTestTimerStart_ ) {
					pppTestTimerStart_ = true;
					createTimer( ( (ticksPerSecond / 8) * 10 ), pppTestTimer, CHobartTimer::PERIODIC_TIMER);
				}
			} else {
				pppTestTimerStart_ = false;
				cancelTimer( pppTestTimer );
			}
			break;
		}
		case WrPPPResume: 
		{
			Debug::trace("WrapperManager.PPPMsg", "WrPPPResume");
			NGWWrapperResume resume( recvBuffer );

			NGWWrapperPause pause( this );
			pause.setPackageId( resume.getPkgId() );
			pause.setState( (NGWWrapperPause::NGWState)resume.getState() );			
			
			const char *pCC = pause.serialize();
			pMyWrapper->send( pCC, pause.getSerializedLength() );
			break;
		}
		case WrNGWDimensionOverrides:
		{
			Debug::trace("WrapperManager.Message", "WrNGWDimensionOverrides");

			NGWDimensionOverrides overrides(recvBuffer);
			overrides.setSourceObject(this);

			const char *pCC = overrides.serialize();
			pMyWrapper->send( pCC, overrides.getSerializedLength() );
			break;
		}
		default:
		{
			Debug::trace("WrapperManager.Message", HobString("handleWrapperEvent -- unknown event: 0x") +
						HobString(Hex::toString((UINT32) wrEvent)));													
			break;
		}
	}
}


void WrapperManager::sendMessageToCommMgr(const char* buffer)
{
	bool msgSent = false;
	WrapperGeneric wg(buffer);

	if (pMyWrapper)
	{
		wg.setSourceObject(this);

		const char *pCC = wg.serialize();
		if (pCC)
		{
			pMyWrapper->send(pCC, wg.getSerializedLength());
			msgSent = true;
		}
	}
	if (!msgSent)
		Debug::trace("WrapperManager.Message", "sendMessageToCommMgr():  Unable to send (" + HobString(Integer::toString(wg.getDeviceType())) + ", " + HobString(Integer::toString(wg.getEventType())) + ") message!!!");
}


void WrapperManager::handleWrapperSysEvent(SysEvent sysEvent, const char *recvBuffer)
{
	switch (sysEvent)
	{
		case SysWakeup:
			{
				Debug::trace("WrapperManager.SystemMessage", "SysWakeup");

				receivedWrapperFeedback();
			}
			break;
		case SysConfig:
			{
				Debug::trace("WrapperManager.Message", "SysConfig");

				WrapperSystemConfiguration wsc(recvBuffer);
				wsc.setSourceObject(this);

				const char *pCC = wsc.serialize();
				pMyWrapper->send(pCC, wsc.getSerializedLength());
			}
			break;
		case SysInquire:
			{
				Debug::trace("WrapperManager.Message", "SysInquire");

				WrapperSystemInquire wsi(recvBuffer);
				wsi.setSourceObject(this);
				dispatchEvent(&wsi);

				receivedWrapperFeedback();
			}
			break;
		case SysStatus:
			{
				Debug::trace("WrapperManager.Message", "SysStatus");

				WrapperSystemStatus wss(recvBuffer);
				wss.setSourceObject(this);
				dispatchEvent(&wss);

				receivedWrapperFeedback();
			}
			break;
		case SysReqStatus:
			{
				Debug::trace("WrapperManager.Message", "SysReqStatus");
				if( !fullBrightCan_ )  {
					WrapperSystemRequestStatus wsrs(recvBuffer);
					wsrs.setSourceObject(this);

					const char *pCC = wsrs.serialize();
					pMyWrapper->send(pCC, wsrs.getSerializedLength());
				} else {
					NGWRequestVersion req(this);
					dispatchEvent(&req);
				}
			}
			break;
		case SysReqInquire:
			{
				Debug::trace("WrapperManager.Message", "SysReqInquire");

				WrapperSystemRequestInquire wsri(recvBuffer);
				wsri.setSourceObject(this);

				const char *pCC = wsri.serialize();
				pMyWrapper->send(pCC, wsri.getSerializedLength());
			}
			break;

		case SysUsbWrStatus:
		{
			Debug::trace("WrapperManager.Message", "SysUsbWrStatus");

			EvolWrapperSystemStatus wss(recvBuffer);
			wss.setSourceObject(this);
			dispatchEvent(&wss);

			//	PTR 1565:	Each time we receive a WR_SYS_STATUS message, request the WR_SYSTEM_INFO message.
			//				This should ensure a check for upgrade for older Mfg. images.
			requestEvolWrapperSystemInfo();

			receivedWrapperFeedback();
			break;
		}

		default:
			{
				Debug::trace("WrapperManager.Error", HobString("handleWrapperEvent -- Some Other SySEvent: 0x") +
						HobString(Hex::toString((UINT32) sysEvent)));
			}
			break;
	}
}

void WrapperManager::handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer)
{
	switch (wgEvent)
	{
		case WgStatus:
			{
				WeigherStatus wgStatus(recvBuffer);

				if (wgStatus.isPositiveNonZeroStableWeight() == true)
				{
					Debug::trace("WrapperManager", "Positive, NonZero, Stable Weight");
				}
			}
			break;
		default:
			{
				break;
			}
	}
}


void WrapperManager::handleVersionManagerMessage(VersionManagerEvent UNUSED(vmEvent), char *UNUSED(recvBuffer))
{
	UNREFERENCED_PARAMETER(recvBuffer);
	UNREFERENCED_PARAMETER(vmEvent);
}

void WrapperManager::handleSessionDevMessage( SessionEvent e, const char *recvBuffer )
{
	switch ( e )
	{
    case SnKeyActivation:
		{
			const HfcAccessKeyActivation haka( recvBuffer );
			const AccessKeys accessKeys( haka.getKeys() );

			if ( accessKeys.isMFGTestKeyPresent() )
			{
				Debug::trace("WrapperManager", "MANF TEST KEY Present");
				isManfTestKeyPresent_ = true;
			}
			else
			{
				Debug::trace("WrapperManager", "MANF TEST KEY Removed");
				isManfTestKeyPresent_ = false;
			}

			break;
		}

	default:
		break;
	}
}


void WrapperManager::addPackageToHistory(WrPackCode packCode, UINT16 width, UINT16 length)
{
	Debug::trace("WrapperManager.PackageHistory", "New package wrapped: packcode " +
			inttostring(packCode) + ", width = " + uinttostring(width) + ", length = " + uinttostring(length));

	if ( PackCode0 <= packCode && packCode <= PackCode11 )
		wrapperPackageHistory_.addPackage(packCode, width, length);
	else
		Debug::trace("WrapperManager.PackageHistory", "Discarded package information: packcode out of range");
}


void WrapperManager::requestWakeupMsg()
{
	WrapperRequestWakeup wrw(this, physicalAddress);
	const char *pCC = wrw.serialize();
	if( !fullBrightCan_ )
	{
		pMyWrapper->send(pCC, wrw.getSerializedLength());
	}
	else
	{
	    if( pMyWrapper != NULL )
	    {
			Debug::trace( "WrapperManager.Timer","fullbright requestWakeupMsg(): send");
			pMyWrapper->send(pCC, wrw.getSerializedLength());
		}
		else
		{
			Debug::trace( "WrapperManager.Timer","fullbright requestWakeupMsg(): dispatchEvent");
		    dispatchEvent(&wrw);
	    }
    }
}

void WrapperManager::requestSystemWakeupMsg()
{
	WrapperSystemRequestWakeup wsrw(this);

	const char *pCC = wsrw.serialize();
	pMyWrapper->send(pCC, wsrw.getSerializedLength());
}

void WrapperManager::requestStatusMsg()
{
	if (pMyWrapper != NULL)
	{
		WrapperRequestStatus reqStatus(this);

		const char *pCC = reqStatus.serialize();
		pMyWrapper->send(pCC, reqStatus.getSerializedLength());
	}
}

void WrapperManager::requestSensorMsg()
{
	if (pMyWrapper != NULL)
	{
		WrapperRequestSensors reqSense(this);

		const char *pCC = reqSense.serialize();
		pMyWrapper->send(pCC, reqSense.getSerializedLength());
	}
}

void WrapperManager::requestErrorHistoryMsg()
{
	if (pMyWrapper != NULL)
	{
		WrapperRequestErrorHistory reqErrorHistory(this);

		const char *pCC = reqErrorHistory.serialize();
		pMyWrapper->send(pCC, reqErrorHistory.getSerializedLength());
	}
}
#ifdef USE_CYCLE_COUNTER
void WrapperManager::requestEvolWrapperCycleCount()
{
	if (pMyWrapper != NULL)
	{
		EvolWrapperReqCycleCount evolWrapperReqCycleCount(this);

		const char *pCC = evolWrapperReqCycleCount.serialize();
		pMyWrapper->send(pCC, evolWrapperReqCycleCount.getSerializedLength());
	}
}
#endif

void WrapperManager::requestEvolWrapperSystemInfo()
{
	// SOF-2768 This is causing continuous requests for system info.
	// We should only need this once! PTR 1565 is the problem.
	// Wrapper settings page does not update (Hybrid wrapper only) if this is not sent based on SysUsbWrStatus message.
	// Wrapper settings page continuously requests SysUsbWrStatus ( request based on QTimer ).
	if( msgCntr_ <=  3 ) {
		msgCntr_++;
		if( !fullBrightCan_ ) {
			if (pMyWrapper != NULL)
			{
				WrapperGeneric wg(WrReqEvolSystemInfo, this);

				const char *pCC = wg.serialize();
				pMyWrapper->send(pCC, wg.getSerializedLength());
			}
		}
	}
}

void WrapperManager::requestEvolWrapperSystemReset()
{
	if(pMyWrapper != NULL)
	{
		Debug::trace("WrapperManager","Requesting wrapper hard system reset");
		WrapperGeneric wg(WrEvolReqSystemReset, this);

		const char *pCC = wg.serialize();
		pMyWrapper->send(pCC, wg.getSerializedLength());
	}
}

void WrapperManager::requestCamerasScanMsg(EvolWrapperTest::WrEvolCameraTestParam whichCamera)
{
	// We can only request one camera's data points at a time.
	// The choices are listed in EvolWrapperTest.
	// When we get the WrEvolTestComplete message for it, we will request the other side.
	EvolWrapperTest ewt(this, EvolWrapperTest::CameraTest);
	ewt.setDataInt1((INT16)whichCamera);

	if ( fullBrightCan_ )
		dispatchEvent(&ewt);
	else if ( pMyWrapper )
	{
		const char *pCC = ewt.serialize();
		pMyWrapper->send( pCC, ewt.getSerializedLength() );
	}
}


void WrapperManager::requestInquireMsg()
{
	if (pMyWrapper != NULL)
	{
		WrapperRequestInquire wsri(this);

		const char *pCC = wsri.serialize();
		pMyWrapper->send(pCC, wsri.getSerializedLength());
	}
}

void WrapperManager::receivedWrapperFeedback()
{
	if ((wrKind == WrUltimaWrapper) && (lastWrapperStatus.isPeripheralComm() == false))
	{
		lastWrapperStatus.setPeripheralComm();
		dispatchEvent(&lastWrapperStatus);
		sendWrapperKind();
	}
	else if ((wrKind == WrEvolWrapper) && (lastEvolWrapperStatus.isPeripheralComm() == false || lastEvolWrapperStatus.getCurrErrorInfo().getGeneralWrapper()-501 == 201))
	{
		Debug::trace("WrapperManager.ProbeWrapper", "receivedWrapperFeedback - lastEvolStatus comm was false or was 201");
		lastEvolWrapperStatus.setPeripheralComm();

		EvolWrapperErrorInfo ewei = lastEvolWrapperStatus.getCurrErrorInfo();
		if (ewei.getGeneralWrapper()-501 == 201)
		{
			Debug::trace("WrapperManager.ProbeWrapper", "receivedWrapperFeedback - lastEvolStatus resetting gen error from 201 to 0");
			ewei.setGeneralWrapper(0);
			lastEvolWrapperStatus.setCurrErrorInfo(ewei);
		}
		dispatchEvent(&lastEvolWrapperStatus);
		sendWrapperKind();
	}
	probeTimeoutCntr_ = 0;
	Debug::trace("WrapperManager.WrapperProbe", HobString("Clear probeTimeoutCounter!!! ") + Integer::toString(probeTimeoutCntr_));

}

void WrapperManager::timeoutProcess( __int32 timer_id )
{
	Debug::trace("WrapperManager.Timer", "timeoutProcess(): " + Integer::toString(timer_id));

	if( timer_id == wrapperProbe )
	{

		if(( style == "NGW2" ) || ( style == "NGW3" ) ) {
			Debug::trace("WrapperManager.Timer", "probeTimeoutCntr_: " + Integer::toString(probeTimeoutCntr_));
			switch (probeTimeoutCntr_)
			{
				case 0: {
					Debug::trace("WrapperManager.WrapperProbe", HobString("Probe Timeout requesting wakeup ") + Integer::toString(probeTimeoutCntr_) );
					requestWakeupMsg();
					break;
				}
				case 1: {
					Debug::trace("WrapperManager.WrapperProbe", HobString("Probe Timeout requesting wakeup ") + Integer::toString(probeTimeoutCntr_) );
					requestWakeupMsg();
					break;
				}
				case 2: {
					if( lastWrapperStatus.isPeripheralComm() == true ) {
						lastWrapperStatus = WrapperStatus();

						lastWrapperStatus.clearPeripheralComm();
						dispatchEvent(&lastWrapperStatus);

						EvolWrapperErrorInfo ewei = lastEvolWrapperStatus.getCurrErrorInfo();
						//ewei.setGeneralWrapper(WrapperProbeNoResponse);
						ewei.setGeneralWrapper(201);
						lastEvolWrapperStatus.setCurrErrorInfo(ewei);
						Debug::trace("WrapperManager.ProbeWrapper", "lostContact == true - setting gen error to 201");

						dispatchEvent(&lastEvolWrapperStatus);

						if( fullBrightCan_ )  {
							WrapperGeneric msg( WrNGWWrapperDetached, this );
							dispatchEvent( &msg );
						}


					}
				}
			}
		} else {

			switch (probeTimeoutCntr_)
			{
				case 0:
				case 1://have received some messages from the wrapper since last timeout.
				case 2:
					break;
				case 3:
				case 4:
				case 5: {
					Debug::trace("WrapperManager.WrapperProbe", HobString("Probe Timeout requesting wakeup ") + Integer::toString(probeTimeoutCntr_) );
					requestWakeupMsg();
					break;
				}
				case 6: {
					Debug::trace("WrapperManager.ProbeWrapper", " WrapperManager has lost contact " + Integer::toString(probeTimeoutCntr_));
					if ((wrKind == WrUltimaWrapper) && (lastWrapperStatus.isPeripheralComm() == true)) {
						lastWrapperStatus = WrapperStatus();

						lastWrapperStatus.clearPeripheralComm();
						dispatchEvent(&lastWrapperStatus);
					} else if ((wrKind == WrEvolWrapper) && (lastEvolWrapperStatus.isPeripheralComm() == true)) {

						if (lastEvolWrapperStatus.getCurrErrorInfo().getGeneralWrapper()-501 == 201) {
							Debug::trace("WrapperManager.ProbeWrapper", "lostContact == true - gen error was already 201 so setting comm to false");
							lastEvolWrapperStatus = EvolWrapperStatus();
							lastEvolWrapperStatus.clearPeripheralComm();
							dispatchEvent(&lastEvolWrapperStatus);
						} else {
							lastEvolWrapperStatus = EvolWrapperStatus();

							EvolWrapperErrorInfo ewei = lastEvolWrapperStatus.getCurrErrorInfo();
							//ewei.setGeneralWrapper(WrapperProbeNoResponse);
							ewei.setGeneralWrapper(201);
							lastEvolWrapperStatus.setCurrErrorInfo(ewei);
							Debug::trace("WrapperManager.ProbeWrapper", "lostContact == true - setting gen error to 201");

							dispatchEvent(&lastEvolWrapperStatus);

						}

						//inform the RTC that we have detached
						if( fullBrightCan_ )  {
							WrapperGeneric msg( WrNGWWrapperDetached, this );
							dispatchEvent( &msg );
						}
					}
					probeTimeoutCntr_ = 0;
					break;
				}
			}
		}
		probeTimeoutCntr_++;
	}
	else if ( timer_id == sensorTimerExpired )
	{
		if (trackSensors == true)
		{
			requestSensorMsg();	
		}
	}
	else if(timer_id == cycleCountTimerExpired)
	{
		Debug::trace("WrapperManager.Timer", "cycleCountTimerExpired: " + Integer::toString(evolWrapperCycleCount_));

		if (( evolWrapperCycleCount_ > 0 ) && ( evolWrapperCycleCount_ != previousCycleCount_ ))
		{
			// SOF-2402
			std::thread t( &WrapperManager::writeCycleCountsToFlashThread, this );
			t.detach();
		}
	}
	else if(timer_id == wrapperTestTimerExpired)
	{
		EvolWrapperReqSystemReset evolWrapperReqSystemReset(this);

		const char *pCC = evolWrapperReqSystemReset.serialize();

		pMyWrapper->send(pCC, evolWrapperReqSystemReset.getSerializedLength());	
	}
	else if(timer_id == cameraTracking)
	{
		if ( trackCameras )
		{
			if ( trackCameraNumber == EvolWrapperTest::LeftCameraScanAnalog )
				requestCamerasScanMsg( trackCameraNumber = EvolWrapperTest::RightCameraScanAnalog );
			else if ( trackCameraNumber == EvolWrapperTest::RightCameraScanAnalog )
				requestCamerasScanMsg( trackCameraNumber = EvolWrapperTest::LeftCameraScanAnalog );
		}
	}
	else if(timer_id == heightScanTracking)
	{
		if ( trackCameras )
		{
			requestCamerasScanMsg( EvolWrapperTest::CameraHeight );
		}
	}
	else if(timer_id == pppTestTimer)
	{
		PPPRequest req( this, PPPRequest::_Status );
		const char *pCC = req.serialize();
		pMyWrapper->send( pCC, req.getSerializedLength() );
	}
	else if(timer_id == pppProbeTimer) 
	{
		Debug::trace("WrapperManager.PPPTimer", "Expired: send WrPPPBootError");
		WrapperGeneric msg( WrPPPBootError, this );
		dispatchEvent( &msg );	
	}
}

void WrapperManager::writeCycleCountsToFlashThread()
{
	FpmManager *pSerialFlash( FpmManager::getInstance() );

	if ( !pSerialFlash )
	{
		Debug::trace("WrapperManager.Timer", "FpmManager *pSerialFlash is NULL.");
		return;
	}

#if defined (__linux__)
	sched_param params;
	params.sched_priority = 0;

	// lower thread priority
	if ( 0 != pthread_setschedparam( pthread_self(), SCHED_IDLE, &params ) )
		Debug::trace( "WrapperManager.Timer", "failed to set priority!" );
#endif

	MtdNGWWrapperData *pData( pSerialFlash->readWrapperData() );

	if ( pData )
	{
		pData->setCycleCounter( previousCycleCount_ = evolWrapperCycleCount_ );

		if(pSerialFlash->writeNGWWrapperData() != 0)
			Debug::trace("WrapperManager.Timer", "Cycle counter saved count: " + Integer::toString(evolWrapperCycleCount_));
		else
			Debug::trace("WrapperManager.Timer", "Failed to save cycle counter!");
	}
	else
	{
		Debug::trace("WrapperManager.Timer", "MtdNGWWrapperData *pData is NULL.");
	}
}

void WrapperManager::sendWrapperKind()
{
	WrapperWrapperKind wrapperKind(this, wrKind);
	dispatchEvent(&wrapperKind);
}

void WrapperManager::addPackageToEvolHistory(EvolWrapperPackageDesc &wrPackageDesc)
{
	evolWrapperPackageHistory_.addPackage(wrPackageDesc.getId(), wrPackageDesc.getWidth(), wrPackageDesc.getLength(), wrPackageDesc.getHeight(),
											wrPackageDesc.getLip(), wrPackageDesc.getFilmWidthSelection(), wrPackageDesc.getTrayName(),
											wrPackageDesc.hasUsedWrapBoxBestFit(), wrPackageDesc.hasModifiedNetStretch());
}

void WrapperManager::setEvolWrapperTrayNames(EvolWrapperTrayNames *trayNames)
{
	//	Save the current values, but release any current data first.
	if (wrTrayNames_)
	{
		if( wrTrayNames_ != trayNames ) {
			Debug::trace("WrapperManager", "setEvolWrapperTrayNames():  Deleting wrTrayNames_ = " + HobString(Hex::toString((void*) wrTrayNames_)));
			delete wrTrayNames_;
		}
	}
	wrTrayNames_ = trayNames;
	Debug::trace("WrapperManager", "setEvolWrapperTrayNames():  wrTrayNames_ = " + HobString(Hex::toString((void*) wrTrayNames_)));
}

void WrapperManager::registerOperationalEvents()
{
	// Register for all the OperationMode messages
	registerEvent(Wrapper, WrReqWakeup);
	registerEvent(Wrapper, WrReqInquire);
	registerEvent(Wrapper, WrReqStatus);
	registerEvent(Wrapper, WrReqSensors);
	registerEvent(Wrapper, WrReqErrorHistory);
	registerEvent(Wrapper, WrSensorsTracking);
	registerEvent(Wrapper, WrStatusTracking);
	registerEvent(Wrapper, WrErrorHistoryTracking);

	registerEvent(Wrapper, WrEvolCamerasTracking);
	registerEvent(Wrapper, WrEvolCameraHeightScanTracking);

	registerEvent(Wrapper, WrAbort);
	registerEvent(Wrapper, WrEnable);
	registerEvent(Wrapper, WrDisable);
	registerEvent(Wrapper, WrReset);
	registerEvent(Wrapper, WrConfig);
	registerEvent(Wrapper, WrMode);
	registerEvent(Wrapper, WrWrap);
	registerEvent(Wrapper, WrPause);
	registerEvent(Wrapper, WrResume);
	registerEvent(Wrapper, WrLoad);
	registerEvent(Wrapper, WrKind);

	registerEvent(Wrapper, WrSetGripperPWMTimings);
	registerEvent(Wrapper, WrReqPackageHistory);
	registerEvent(Wrapper, WrEvolRequestPackageTimings);
	registerEvent(Wrapper, WrEvolSavePackageTimings);
	registerEvent(Wrapper, WrEvolResetDefaultPackageTimings);
	registerEvent(Wrapper, WrEvolUnlockPackageTimings);

	registerEvent(WrapperSystem, SysConfig);
	registerEvent(WrapperSystem, SysReqStatus);
	registerEvent(WrapperSystem, SysReqInquire);

	registerEvent(Wrapper, WrEvolLoad);
	registerEvent(Wrapper, WrEvolWrap);
	registerEvent(Wrapper, WrEvolConfig);
	registerEvent(Wrapper, WrEvolTest);
//	registerEvent(Wrapper, WrEvolSendDateTime);
	registerEvent(Wrapper, WrEvolUpgradeInProgress);

	//new for cycle count -- ats
	registerEvent(Wrapper, WrEvolCycleCount);
	registerEvent(Wrapper, WrEvolReqCycleCount);
	registerEvent(Wrapper, WrEvolSaveCycleCount);

	registerEvent(Wrapper, WrEvolStartMfgRunin);

	//new for camera scan
	registerEvent(Wrapper, WrCameraCalibration);
	registerEvent(Wrapper, WrEvolRequestCameraThreshold);
	registerEvent(Wrapper, WrEvolCameraThreshold);
	registerEvent(Wrapper, WrEvolWriteCameraThreshold);

	registerEvent(Wrapper, WrEvolDiagnosticsTest);

	registerEvent(Wrapper, WrReqEvolSystemInfo);
	registerEvent(Wrapper, WrEvolSystemInfoErase);
	registerEvent(Wrapper, WrEvolSystemInfoWrite);

	registerEvent(Wrapper, WrUIWrapperModel);
	registerEvent(Wrapper, WrEvolReqSystemReset);
	registerEvent(Wrapper, WrNGWDimensionOverrides);
	
	//Can Translator dispatches these messages
	if( partialCan_ )
	{
		registerEvent(Wrapper, WrOpen);
		registerEvent(Wrapper, WrEvolStatus);
		registerEvent(Wrapper, WrEvolPackageDesc);
	}
	if( fullBrightCan_ )
	{
		//Full bright CAN implementation
		registerEvent(Wrapper, WrEvolWakeup);
		registerEvent(Wrapper, WrEvolEventCapture);
		registerEvent(Wrapper, WrEvolSensors);
		registerEvent(Wrapper, WrEvolErrorHistory);
		registerEvent(Wrapper, WrCameraScan);
		registerEvent(Wrapper, WrEvolTestComplete);
		registerEvent(Wrapper, WrEvolPackageDischarge);
		registerEvent(WrapperSystem, SysUsbWrStatus);
		registerEvent(Wrapper, WrEvolMotorProfile);
		registerEvent(Wrapper, WrEvolManufacturingRunInNotice);
		registerEvent(Wrapper, WrEvolDiagnosticsTestResult);
		registerEvent(Wrapper, WrEvolPackageTimings);
		registerEvent(Wrapper, WrWrapBoxSize);
		registerEvent(Wrapper, WrWrapBoxTransferComplete);
		registerEvent(Wrapper, WrWrapBoxAck);
	}
	registerEvent(Weigher, WgStatus);
}

void WrapperManager::sendEvolWrapperTest(EvolWrapperTest *testMsg)
{
	Debug::trace("WrapperManager.Message", "sendEvolWrapperTest():  Begin");
	if (savingWrapperTimings_)
	{
		//	HA-81: Wait until we get the WrEvolSavePackageTimingsComplete back
		//	before sending any test messages - especially CycleWrapper.
		//	Just save the message for now.
		if (testMsg)
		{
			holdTestMsg_ = testMsg;
			waitingToCycleWrapper_ = true;
			Debug::trace("WrapperManager.Message", "sendEvolWrapperTest():  Hold the test message. waitingToCycleWrapper_ = true.");
		}
	}
	else
	{
		EvolWrapperTest *tempTestMsg = NULL;

		if (!testMsg && holdTestMsg_)
		{
			tempTestMsg = holdTestMsg_;
			holdTestMsg_ = NULL;
			Debug::trace("WrapperManager.Message", "sendEvolWrapperTest():  Using holdTestMsg_");
		}
		else if (testMsg && !holdTestMsg_)
		{
			tempTestMsg = testMsg;
			Debug::trace("WrapperManager.Message", "sendEvolWrapperTest():  Using msg passed in.");
		}
		else
		{
			Debug::trace("WrapperManager.Message", "sendEvolWrapperTest():  Problem! We have no test message!!!");
		}
		
		if (tempTestMsg)
		{
			if( !fullBrightCan_ )  {

				const char *pCC = tempTestMsg->serialize();
				pMyWrapper->send(pCC, tempTestMsg->getSerializedLength());
				Debug::trace("WrapperManager.Message", "sendEvolWrapperTest():  Test msg sent to wrapper.");

				if (Debug::isTracing("WrapperManager.WrapperTest") == true)
				{
					tempTestMsg->show();
				}
			} else {
				Debug::trace("WrapperManager.Message", "sendEvolWrapperTest():  Test msg sent to wrapper.");
				dispatchEvent(tempTestMsg);
			}
			delete tempTestMsg;
		}
	}
}
