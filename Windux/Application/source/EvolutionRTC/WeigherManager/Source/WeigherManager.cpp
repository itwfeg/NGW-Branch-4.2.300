#include "MetSig.h"
#include "PAL_platformheader.h" 
#include "Configuration.h"
#include "WeigherManager.h"
#include "WeigherStatus.h"
#include "WeigherCachedStatus.h"
#include "WeigherInquire.h"
#include "WeigherWakeup.h"
#include "WeigherConfiguration.h"
#include "WeigherMode.h"
#include "WeigherGeneric.h"
#include "WeigherWeigherKind.h"

#include "WeigherRequestStatus.h"
#include "WeigherRequestInquire.h"
#include "WeigherRequestWakeup.h"

#include "WeigherSystemStatus.h"
#include "UsbWeigherSystemStatus.h"
#include "WeigherSystemInfo.h"
#include "WeigherSystemConfiguration.h"
#include "WeigherSystemInquire.h"
#include "WeigherSystemRequestWakeup.h"
#include "WeigherSystemWakeup.h"
#include "WeigherSystemRequestStatus.h"
#include "WeigherSystemRequestInquire.h"
#include "WeigherSystemCalibrationSwitch.h"
#include "WeigherSystemReset.h"
#include "WeigherColdReset.h"
//Cat3
#include "WeigherCat3DateTime.h"
#include "WeigherCat3PageRequest.h"
#include "WeigherCat3Statistics.h"
#include "WeigherWriteCat3Record.h"
#include "WeigherCat3RecordStatus.h"
#include "WeigherCat3AuditRecords.h"
//Value Max
#include "WeigherReqValueMax.h"
#include "WeigherValueMaxAssembly.h"
#include "WeigherValueMaxWriteAssembly.h"
#include "WeigherValueMaxHwVersion.h"
#include "WeigherValueMaxSerialNumber.h"
#include "WeigherValueMaxDateOfMan.h"
#include "WeigherValueMaxXGainFactor.h"
#include "WeigherValueMaxYGainFactor.h"
#include "WeigherValueMaxZGainFactor.h"
#include "WeigherValueMaxWriteHwVersion.h"
#include "WeigherValueMaxWriteSerialNumber.h"
#include "WeigherValueMaxWriteDateOfMan.h"
#include "WeigherValueMaxWriteXGainFactor.h"
#include "WeigherValueMaxWriteYGainFactor.h"
#include "WeigherValueMaxWriteZGainFactor.h"
#include "WeigherValueMaxAllGains.h"
#include "WeigherSystemRequestValueMax.h"
#include "WeigherValueMaxFitParams.h"
#include "FpmManager.h"
#include "WeigherFactoryDefaultsComplete.h"
#include "WeigherKpcError.h"
#include "KpcSystemErrors.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"
#include "ULong.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherManager::dataType = 0;
std::string WeigherManager::dataTypeName("WeigherManager"); 
#endif

WeigherManager::WeigherManager(Hobart::CEvolMsgReceiver *pMyWeigher, 
							   UINT16 physicalAddress,
							   WeigherKind wgKind) : Hobart::CEvolMsgReceiver("WeigherManager")
{
	Debug::trace("WeigherManager", string("Constructor WeigherManager:") + util_string::ptrtostring(this) + "  pMyWeigher:" + util_string::ptrtostring(pMyWeigher ) + " - started");

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->pMyWeigher = pMyWeigher;
	this->physicalAddress = physicalAddress;
	this->wgKind = wgKind;


	pConfig = Configuration::getInstance();

    //get my type of system
    FpmManager *pSerialFlash = FpmManager::getInstance();
	if( pSerialFlash != NULL ) {
	    style = pSerialFlash->getMFGSystemStyle();
	}

	weigherProbeTime = pConfig->getWeigherProbeTime() * 1000; 
	Debug::trace("WeigherManager.WeigherProbe", HobString("probe interval = ") + Integer::toString(weigherProbeTime));

#ifdef _UNIT_TEST
	weigherTestMsgTime  = (15 * 100 ) + 1000;	/* added for CAT3 unit testing  ats -- 07012016 */
	testCounter = 0;
	testMsgCount = 0;
#endif

#ifdef WG_DEFAULTS_TEST
	weigherTestMsgTime  = (10 * 1000 ) + 500;
	testCounter = 0;
	testMsgCount = 0;
	failedErases = 0;
#endif

	weigherLocked = false;
	largeMotionWhileLocked = false;
	weigherCalibSwitch = false;

	scaleFactor = 0;
	weigherConfigOutOfRange = false;
	reqConfigResendCounter_ = 0;


	registerEvent(Weigher, WgReqStatus);
	registerEvent(Weigher, WgReqInquire);
	registerEvent(Weigher, WgReqConfig );
	registerEvent(Weigher, WgReqWakeup);
	registerEvent(Weigher, WgReqKind);
	registerEvent(Weigher, WgReqValueMax);

	registerEvent(Weigher, WgEnable);
	registerEvent(Weigher, WgDisable);
	registerEvent(Weigher, WgRezero);
	registerEvent(Weigher, WgReset);
	registerEvent(Weigher, WgColdReset);
	registerEvent(Weigher, WgConfig);
	registerEvent(Weigher, WgMode);
	registerEvent(Weigher, WgNormal);
	registerEvent(Weigher, WgLock);
	registerEvent(Weigher, WgControl);

	registerEvent(Weigher, WgCat3ReqStatistics);
	registerEvent(Weigher, WgCat3ReqPage);
	registerEvent(Weigher, WgCat3ReqNextRecords);
	registerEvent(Weigher, WgCat3RecordStatus);
	registerEvent(Weigher, WgCat3AuditStatistics);
	registerEvent(Weigher, WgCat3Records);
	registerEvent(Weigher, WgCat3WriteRecord);
	registerEvent(Weigher, WgCat3ReqEraseLog);
	registerEvent(Weigher, WgCat3LogEraseComplete);
	registerEvent(Weigher, WgCat3Error);
	registerEvent(Weigher, WgResetFactoryDefaults);

	registerEvent(WeigherSystem, SysConfig);
	registerEvent(WeigherSystem, SysReqWakeup);
	registerEvent(WeigherSystem, SysReqStatus);
	registerEvent(WeigherSystem, SysReqInquire);
	registerEvent(WeigherSystem, SysReset);
	//value max
	registerEvent(Weigher, WgValueMaxAssembly);
	registerEvent(Weigher, WgValueMaxHwVersion);
	registerEvent(Weigher, WgValueMaxSerialNumber);
	registerEvent(Weigher, WgValueMaxDateOfManufacture);
	registerEvent(Weigher, WgValueMaxXGain);
	registerEvent(Weigher, WgValueMaxYGain);
	registerEvent(Weigher, WgValueMaxZGain);
	registerEvent(Weigher, WgValueMaxAllGains);
	registerEvent(Weigher, WgValueMaxFitParams);


	if( this->wgKind == WgCanKpc) {
		registerEvent(Weigher, WgSystemInfo);
		registerEvent(Weigher, WgKPCError);
	}

	if( this->wgKind == WgCanPrepack ) {
		registerEvent(Weigher, WgSystemInfo);
		registerEvent(Weigher, WgKPCError);
		registerEvent(KPCSystem, KpcError);
	}

	sendWeigherMode(ScaleManualMode);
	requestWeigherStatus();
	requestWeigherSystemStatus();

	receivedWeigherFeedback();

	sendWeigherKind();

	requestWeigherInquire();

	probeTimeoutCntr_ = 0;

	currentWeighMode = WeighMode_Avoir;

	createTimer(weigherProbeTime, weigherProbe, CHobartTimer::PERIODIC_TIMER);

#ifdef CAT3_UNIT_TEST
	/* added for unit testing  ats -- 07012016 */
	createTimer(weigherTestMsgTime, WeigherTestCat3, CHobartTimer::PERIODIC_TIMER);
#endif
#ifdef WG_DEFAULTS_TEST
	createTimer(weigherTestMsgTime, WeigherTest, CHobartTimer::PERIODIC_TIMER);
#endif
#ifdef VALUE_MAX_UNIT_TEST
	/* added for unit testing  ats -- 09202017 */
	createTimer(weigherTestMsgTime, WeigherTestValueMax, CHobartTimer::PERIODIC_TIMER);
#endif

	Debug::trace("WeigherManager", string("Constructor WeigherManager:") + util_string::ptrtostring(this) + " - ended");
}

WeigherManager::~WeigherManager()
{
	Debug::trace("WeigherManager", string("Destructor WeigherManager:") + util_string::ptrtostring(this) + "  pMyWeigher:" + util_string::ptrtostring(pMyWeigher ) + " - started");

	deregisterAllEvents();

	cancelTimer(weigherProbe);

#ifdef CAT3_UNIT_TEST
	cancelTimer(WeigherTestCat3);
#endif
#ifdef VALUE_MAX_UNIT_TEST
	cancelTimer(WeigherTestValueMax);
#endif
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	lastStatusMsg.setManagerDeleted();
	dispatchEvent(&lastStatusMsg);

#ifndef CHECK_FT
	shutdownTimerThreads();
#endif
	ShutdownNicely();

	Debug::trace("WeigherManager", string("Destructor WeigherManager:") + util_string::ptrtostring(this) + " - ended");
}


void WeigherManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
 {

	if ( msg.get() != NULL )
	{
		switch (msg->getDeviceType())
		{
			case Weigher		:	handleWeigherEvent((WgEvent) msg->getEventType(), msg->Data() );
									break;
			case KPCSystem		:	handleKpcSystemError( (KpcSystemEvent)msg->getEventType(), msg->Data() );
									break;
			case WeigherSystem	:	handleWeigherSysEvent((SysEvent) msg->getEventType(), msg->Data() );
									break;
			default				:
									break;
		}
	}
}

void WeigherManager::handleWeigherEvent(WgEvent wgEvent, const char *recvBuffer)
{
	switch (wgEvent)
	{
		case WgWakeup:		
		{
			Debug::trace("WeigherManager.Message.Probe", HobString("WgWakeup"));
			receivedWeigherFeedback();
		}
		break;
		case WgInquire:
		{
			Debug::trace("WeigherManager.InquireTimer", "Stopping Timer");
			cancelTimer(WeigherReqConfig);
			reqConfigResendCounter_= 0;
			
			Debug::trace("WeigherManager.Message", HobString("WgInquire"));

			WeigherInquire wi(recvBuffer);
			wi.setSourceObject(this);

			if ( MetSig::isWeigherDualRangeCapable( wi.getWeigherModel() ) )
				weigherConfigOutOfRange = wi.isUserConfigOutOfRange();

			currentWeighMode = wi.getScaleWeighMode();

			dispatchEvent(&wi);

			receivedWeigherFeedback();

			// Set the WeigherManager's internal scaleFactor from the Configuration just received from the weigher.
			scaleFactor = wi.getScaleFactor();

			if (Debug::isTracing("WeigherManager.WeigherInquire") == true)
			{
				wi.show();
			}
		}
		break;
		case WgConfig:		
		{
			Debug::trace("WeigherManager.Message", HobString("WgConfig"));

			WeigherConfiguration wc(recvBuffer);
			wc.setSourceObject(this);

			//if ( (wc.getConfigType() & 0x7FFF) == 0 ) 	// It's WgPermanent
			// For Category III, have to send the current date/time to the Weigher Manager before permanently setting any configuration or calibration values.
			sendDateTime();

			// If the weigher's scaleFactor has changed, reset the WeigherManager's
			// internal scaleFactor before passing the Configuration to the weigher.
			if (scaleFactor != wc.getScaleFactor())
				scaleFactor = wc.getScaleFactor();

			if ( MetSig::isWeigherDualRangeCapable( wc.getWeigherModel() ) )
				weigherConfigOutOfRange = wc.isUserConfigOutOfRange();

			const char *pCC = wc.serialize();
			pMyWeigher->send(pCC, wc.getSerializedLength());

			if (Debug::isTracing("WeigherManager.WeigherConfig") == true)
			{
				wc.show();
			}

			//	Tom Fink added a cold reset for the Atmel-based SAM-BA device.
			//	He said that we should send a WeigherColdReset when the weigh mode changes [CCD Dec 2011].
			if (currentWeighMode != wc.getScaleWeighMode())
			{
				currentWeighMode = wc.getScaleWeighMode();
				if (wgKind == WgUsbAWS)
				{
					WeigherColdReset wcr;
					wcr.setSourceObject(this);

					const char *pCC = wcr.serialize();
					pMyWeigher->send(pCC, wcr.getSerializedLength());
					Debug::trace("WeigherManager.Message", HobString("WgConfig - WeighMode changed. Sent a WeigherColdReset to the weigher."));
				}
			}
		}
		break;
		// This message comes from the corresponding Weigher Comm Manager
		// (WeigherCommManager in case of USB weigher or LSMNCommManager in case
		// of Ultima weigher) which, in turn, got it from the physical device
		// itself. We always keep track of the last status message received but
		// we will relay this message through the system only if the weigher is
		// not "locked".
		case WgStatus:		
		{
			Debug::trace("WeigherManager.Message", HobString("WgStatus"));

			WeigherStatus weigherStatus( recvBuffer );
			weigherStatus.setSourceObject(this);

			// Set the scaleFactor in the weigherStatus message before dispatching it.
			weigherStatus.setScaleFactor(scaleFactor);

			// WeigherManager keeps track if any user configurable configuration value is out of range.
			// Indicate this to the rest of the system via the WeigherStatus.
			weigherStatus.setWeigherConfigOutOfRange(weigherConfigOutOfRange);

			bool curKeyPressed = weigherStatus.isKeyPressed();
			if (curKeyPressed != weigherCalibSwitch)
			{
				WeigherSystemCalibrationSwitch calibSwitch( this, curKeyPressed );
				dispatchEvent(&calibSwitch);
				weigherCalibSwitch = curKeyPressed;

				if (Debug::isTracing("WeigherManager.WeigherSystemCalibrationSwitch") == true)
				{
					calibSwitch.show();
				}
			}

			if ( weigherStatus.equals(lastStatusMsg) == false)
			{
				if (weigherLocked == false)
				{
					if ( (lastStatusMsg.isNegativeGrossWeight()) && (!weigherStatus.isNegativeGrossWeight()) && (weigherStatus.isLargeMotion()) &&
						(!weigherStatus.isPositiveNonZeroStableWeight()) && (!weigherStatus.isZeroStableWeight()) && 
						(Configuration::getInstance()->isServiceCounterScale())	)
					{					
						Debug::trace("WeigherManager.ThrowOutNegative", "Hey, we are throwing out crap values here");	
					}
					else
					{
						dispatchEvent(&weigherStatus);
						if (Debug::isTracing("WeigherManager.WeigherStatus") == true)
						{
							weigherStatus.show();
						}
					}
				}
				else
				{
					if ( weigherStatus.isLargeMotion() )
						largeMotionWhileLocked = true;

					Debug::trace("WeigherManager.Message", HobString("** Weigher ** LOCKED **"));
					if (Debug::isTracing("WeigherManager.WeigherStatus") == true)
					{
						weigherStatus.show();
					}
				}
				lastStatusMsg = weigherStatus;
			}
			receivedWeigherFeedback();
			break;
		}
		case WgControl:		Debug::trace("WeigherManager.Message.WgControl", HobString("WgControl"));
		case WgEnable:		Debug::trace("WeigherManager.Message.WgEnable",	 HobString("WgEnable"));
		case WgDisable:		Debug::trace("WeigherManager.Message.WgDisable", HobString("WgDisable"));
		case WgReset:		Debug::trace("WeigherManager.Message.WgReset",	 HobString("WgReset"));
		case WgColdReset:	Debug::trace("WeigherManager.Message.WgReset",	 HobString("WgColdReset"));
		case WgRezero:		Debug::trace("WeigherManager.Message.WgRezero",	 HobString("WgRezero"));
		{
			WeigherGeneric wg(recvBuffer);
			wg.setSourceObject(this);

			const char *pCC = wg.serialize();
			pMyWeigher->send(pCC, wg.getSerializedLength());
		}
		break;
		case WgMode:		
		{
			Debug::trace("WeigherManager.Message", HobString("WgMode"));

			WeigherMode wm(recvBuffer);
			wm.setSourceObject(this);

			if (Debug::isTracing("WeigherManager.WeigherMode") == true)
			{
				wm.show();
			}

			const char *pCC = wm.serialize();
			pMyWeigher->send(pCC, wm.getSerializedLength());
		}
		break;
		case WgValueMaxFitParams:
		{
			Debug::trace("WeigherManager.Message", HobString("WgValueMaxFitParams"));

			WeigherValueMaxFitParams fp(recvBuffer);
			fp.setSourceObject(this);

			if (Debug::isTracing("WeigherManager.Message") == true)
			{
				fp.show();
			}

			const char *pCC = fp.serialize();
			pMyWeigher->send(pCC, fp.getSerializedLength());
		}
		break;
		// This message is sent by the Operating Modes to enable the distribution
		// of weigher status messages through the system as they arrive. Also,
		// since the last status is always kept in memory, even if not distributed
		// through the system, it could be distributed now if necessary.
		case WgNormal:
		{
			Debug::trace("WeigherManager.Message", HobString("WgNormal"));

			if ( weigherLocked )
			{
				Debug::trace("WeigherManager.Message", HobString("weigherWasLocked"));

				// If during the time the "weigher window was closed" we got a
				// large motion we want to notify it to the Operating Mode
				if ( largeMotionWhileLocked )
				{
					WeigherCachedStatus wcs ( this );

					Debug::trace("WeigherManager.Message", HobString("dispatching a WeigherCachedStatus with large motion"));

					wcs.setLargeMotion(true);
					dispatchEvent(&wcs);

					largeMotionWhileLocked = false;
				}

				// If we had a last status with a zero stable weight, lets notify
				// it so we don't miss the last zero stable weight.
				if (lastStatusMsg.isZeroStableWeight() == true)
					dispatchEvent(&lastStatusMsg);

				// If we had a last status with a positive stable weight, lets request
				// a new status so we don't miss the last stable weight.
				if ( lastStatusMsg.isPositiveNonZeroStableWeight() )
				{
					WeigherRequestStatus wrs(this);

					const char *pCC = wrs.serialize();
					pMyWeigher->send(pCC, wrs.getSerializedLength());
				}

				weigherLocked = false;
			}

			break;
		}
		// This message is sent by the Operating Modes to disable the distribution
		// of weigher status messages through the system. (The WeigherManager will
		// keep track of the last status message even if it does not distribute it.)
		case WgLock:
		{
			Debug::trace("WeigherManager.Message", HobString("WgLock"));

			weigherLocked = true;

			break;
		}
		case WgReqStatus:	
		{
			Debug::trace("WeigherManager.Message", HobString("WgReqStatus"));

			WeigherRequestStatus wrs(recvBuffer);
			wrs.setSourceObject(this);

			const char *pCC = wrs.serialize();
			pMyWeigher->send(pCC, wrs.getSerializedLength());

			lastStatusMsg = WeigherStatus();
		}
		break;
		case WgReqInquire:
		{
			Debug::trace("WeigherManager.Message", HobString("WgReqInquire"));

			requestWeigherInquire();
		}
		break;
		case WgReqConfig:
		{
			if(reqConfigResendCounter_ <= 8)
			{
				Debug::trace("CANWeigherManager.Message", HobString("WgReqConfig"));
				Debug::trace("WeigherManager.InquireTimer", "Starting Timer");
				createTimer(1000, WeigherReqConfig, false);
				requestWeigherInquire();
				reqConfigResendCounter_++;
			}
			else
			{
				reqConfigResendCounter_=0;
				Debug::trace("CANWeigherManager.Message", HobString("Too many retries canceling timer"));
			}
		}
		break;
		case WgReqWakeup:
		{
			Debug::trace("WeigherManager.Message", HobString("WgReqWakeup"));

			WeigherRequestWakeup wrw(recvBuffer);
			wrw.setSourceObject(this);
			wrw.setPhysicalAddress(physicalAddress);

			const char *pCC = wrw.serialize();
			pMyWeigher->send(pCC, wrw.getSerializedLength());
		}
		case WgReqValueMax:
		{
			Debug::trace("WeigherManager.Message", HobString("WgReqValueMax"));

			WeigherSystemRequestValueMax wrw(recvBuffer);
			wrw.setSourceObject(this);

			const char *pCC = wrw.serialize();
			pMyWeigher->send(pCC, wrw.getSerializedLength());
		}
		break;
		case WgReqKind:
		{
			sendWeigherKind();
		}
		break;
		case WgCat3ReqStatistics:
		{
			Debug::trace("WeigherManager.cat3", HobString("WgCat3ReqStatistics"));

			WeigherGeneric reqStats(WgCat3ReqStatistics, this);
			const char *pCC = reqStats.serialize();
			pMyWeigher->send(pCC, reqStats.getSerializedLength());
		}
		break;
		case WgCat3ReqPage:
		{
			Debug::trace("WeigherManager.cat3", HobString("WgCat3ReqPage"));

			WeigherCat3PageRequest wc3pr(recvBuffer);
			const char *pCC = wc3pr.serialize();
			pMyWeigher->send(pCC, wc3pr.getSerializedLength());
		}
		break;
		case WgCat3ReqNextRecords:
		{
			Debug::trace("WeigherManager.cat3", HobString("WgCat3ReqNextRecords"));

			WeigherGeneric reqNext(WgCat3ReqNextRecords, this);
			const char *pCC = reqNext.serialize();
			pMyWeigher->send(pCC, reqNext.getSerializedLength());
		}
		break;
		case WgCat3RecordStatus:
		{
			Debug::trace("WeigherManager.cat3", "WeigherCat3RecordStatus");

			WeigherCat3RecordStatus status(recvBuffer);
			status.setSourceObject(this);
			dispatchEvent(&status);
			Debug::trace("WeigherManager.cat3", std::string("WeigherCat3RecordStatus: Was record saved: ") + Bool::toString(status.getStatus()) );
		}
		break;
		case WgCat3AuditStatistics:
		{
			Debug::trace("WeigherManager.cat3", "WgCat3AuditStatistics");

			WeigherCat3Statistics stats(recvBuffer);
			stats.setSourceObject(this);
			//stats.show();
			dispatchEvent(&stats);
		}
		break;
		case WgCat3Records:
		{
			Debug::trace("WeigherManager.cat3", "WgCat3Records");

			WeigherCat3AuditRecords recs(recvBuffer);
			recs.setSourceObject(this);
			//recs.show();
			dispatchEvent(&recs);
		}
		break;
		case WgCat3WriteRecord:
		{
			Debug::trace("WeigherManager.cat3", "WgCat3WriteRecord");

			WeigherWriteCat3Record rec(recvBuffer);
			const char *pCC = rec.serialize();
			pMyWeigher->send(pCC, rec.getSerializedLength());
		}
		break;
		case WgCat3ReqEraseLog:
		{
			Debug::trace("WeigherManager.cat3", "WgCat3ReqEraseLog");

			WeigherGeneric cat3AuditClear( WgCat3ReqEraseLog, this );
			const char *pCC = cat3AuditClear.serialize();
			pMyWeigher->send(pCC, cat3AuditClear.getSerializedLength());
		}
		break;
		case WgCat3LogEraseComplete:
		{
			Debug::trace("WeigherManager.cat3", "WgCat3LogEraseComplete");

			WeigherGeneric cat3AuditCleared( WgCat3LogEraseComplete, this );
			dispatchEvent(&cat3AuditCleared);
		}
		break;
		case WgCat3Error:
		{
			Debug::trace("WeigherManager.cat3", "WgCat3Error");
		}
		break;
		case WgResetFactoryDefaults:
		{
			Debug::trace("WeigherManager.Message", "WgResetFactoryDefaults");

			// SOF-1760
			// For Category III, have to send the current date/time to the Weigher Manager before resetting to factory defaults.
			sendDateTime();

			WeigherGeneric msg(recvBuffer);
			msg.setSourceObject(this);
			const char *pCC = msg.serialize();
			pMyWeigher->send( pCC, msg.getSerializedLength() );
		}
		break;
		case WgResetFactoryDefaultsComplete:
		{
			Debug::trace("WeigherManager.Message", "WgResetFactoryDefaultsComplete");

			#ifdef WG_DEFAULTS_TEST
			testMsgCount++;
			testCounter = 0;
			#endif

			WeigherFactoryDefaultsComplete cmpltMsg( recvBuffer );
			if( !cmpltMsg.getSavedStatus() ) {
				Debug::trace("WeigherManager.Message", "Weigher factory defaults corruption!" + Integer::toString(cmpltMsg.getSavedStatus()));

				#ifdef WG_DEFAULTS_TEST
				failedErases++;
				#endif

			} else {
				Debug::trace("WeigherManager.Message", "Weigher factory defaults saved: " + Integer::toString(cmpltMsg.getSavedStatus()));
			}

			#ifdef WG_DEFAULTS_TEST
			Debug::trace("WeigherManager.Message", "Total test messages: " + Integer::toString(testMsgCount) + " Total failures: " + Integer::toString(failedErases));
			#endif

			cmpltMsg.setSourceObject(this);
			dispatchEvent( &cmpltMsg );
			receivedWeigherFeedback();
		}
		break;

		case WgValueMaxAssembly:
		{
			WeigherValueMaxAssembly assembly(recvBuffer);
			Debug::trace("WeigherManager.valueMax", "assembly number: " + assembly.getAssemblyNumber() );
		}
		break;
		case WgValueMaxHwVersion:
		{
			WeigherValueMaxHwVersion hxVersion(recvBuffer);
			Debug::trace("WeigherManager.valueMax", "hardware version: " + hxVersion.getHardwareVersion() );
		}
		break;
		case WgValueMaxSerialNumber:
		{
			WeigherValueMaxSerialNumber serial(recvBuffer);
			Debug::trace("WeigherManager.valueMax", "serial number: " + serial.getSerialNumber() );
		}
		break;
		case WgValueMaxDateOfManufacture:
		{
			WeigherValueMaxDateOfMan date(recvBuffer);
			Debug::trace("WeigherManager.valueMax", "epoch date: " + Integer::toString( date.getDate() ) );
		}
		break;
		case WgValueMaxXGain:
		{
			WeigherValueMaxXGainFactor x(recvBuffer);
			char value[16];
			sprintf( value, "%f", x.getGain() );
			Debug::trace("WeigherManager.valueMax", "X gain: " + std::string(value) );
		}
		break;
		case WgValueMaxYGain:
		{
			WeigherValueMaxYGainFactor y(recvBuffer);
			char value[16];
			sprintf( value, "%f", y.getGain() );
			Debug::trace("WeigherManager.valueMax", "Y gain: " + std::string(value) );
		}
		break;
		case WgValueMaxZGain:
		{
			WeigherValueMaxZGainFactor z(recvBuffer);
			char value[16];
			sprintf( value, "%f", z.getGain() );
			Debug::trace("WeigherManager.valueMax", "Z gain: " + std::string(value) );
		}
		break;
		case WgValueMaxAllGains:
		{
			WeigherValueMaxAllGains all(recvBuffer);
			char value[16];
			sprintf( value, "%f", all.getXGain() );
			Debug::trace("WeigherManager.valueMax", "All gains X gain: " + std::string(value) );
			sprintf( value, "%f", all.getYGain() );
			Debug::trace("WeigherManager.valueMax", "All gains Y gain: " + std::string(value) );
			sprintf( value, "%f", all.getZGain() );
			Debug::trace("WeigherManager.valueMax", "All gains Z gain: " + std::string(value) );
		}
		break;
		case WgSystemInfo:
		{
			Debug::trace("WeigherManager.Message", "WgSystemInfo" );


			WeigherSystemInfo info(recvBuffer);

			info.show();

			// convert new format into old format
			UsbWeigherSystemStatus oldInfo(this, info.getWeigherPosition(), WgCanKpc);
			oldInfo.setHardwareVersion( info.getHardwareVersion() );
			oldInfo.setSoftwareVersion( info.getSoftwareVersion() );
			oldInfo.setSerialNumber(0); //TO DO

			if( info.isEepromValid() ) {
				oldInfo.clearEepromError();
			} else {
				oldInfo.setFlags( EepromError );
				oldInfo.setEepromError();
			}
			if( info.isSerialFlashValid() ) {
				oldInfo.clearSystemError();
				oldInfo.clearWeigherError();
			} else {
				oldInfo.setSystemError();
				oldInfo.setFlags( SystemError | WeigherError );
				oldInfo.setWeigherError();
			}

			oldInfo.show();

			dispatchEvent( &oldInfo );
		}
		break;
		case WgKPCError: {
			WeigherKpcError error( recvBuffer );
			std::string str = "KPC Frimware: " + error.getErrorDescription();
			Debug::trace("WeigherManager.Error", str);
		}
		break;
		default:
		{
			Debug::trace("WeigherManager.Error", HobString("Some Other Event: ") + HobString(Integer::toString((UINT16) wgEvent)));
		}
		break;
	}
}

void WeigherManager::handleKpcSystemError(KpcSystemEvent event, const char *recvBuffer)
{
	switch (event)
	{
		case KpcError: {
			KpcSystemErrors error( recvBuffer );
			std::string str = "KPC System: " + error.getErrorDescription();
			Debug::trace("CANPrinterManager.Error", str);
			break;
		}
	}
}

void WeigherManager::handleWeigherSysEvent(SysEvent sysEvent, const char *recvBuffer)
{
	switch (sysEvent)
	{
		case SysWakeup:
		{
			Debug::trace("WeigherManager.Message", HobString("SysWakeup"));

			receivedWeigherFeedback();
		}
		break;
		case SysConfig:
		{
			Debug::trace("WeigherManager.Message", HobString("SysConfig"));

			WeigherSystemConfiguration wsc(recvBuffer);
			wsc.setSourceObject(this);

			const char *pCC = wsc.serialize();
			pMyWeigher->send(pCC, wsc.getSerializedLength());

			if (Debug::isTracing("WeigherManager.WeigherSystemConfig") == true)
			{
				wsc.show();
			}
		}
		break;
		case SysInquire:
		{
			Debug::trace("WeigherManager.Message", HobString("SysInquire"));

			WeigherSystemInquire wsi( recvBuffer );
			wsi.setSourceObject(this);
			dispatchEvent(&wsi);
			receivedWeigherFeedback();

			if (Debug::isTracing("WeigherManager.WeigherSystemInquire") == true)
			{
				wsi.show();
			}
		}
		break;
		case SysStatus:
		{
			Debug::trace("WeigherManager.Message", HobString("SysStatus"));

			WeigherSystemStatus wss( recvBuffer);
			wss.setSourceObject(this);
			dispatchEvent(&wss);
			receivedWeigherFeedback();

			if (Debug::isTracing("WeigherManager.WeigherSystemStatus") == true)
			{
				wss.show();
			}
		}
		break;
		case SysUsbWgStatus:
		{
			Debug::trace("WeigherManager.Message", HobString("SysUsbWgStatus"));

			UsbWeigherSystemStatus wss (recvBuffer);
			wss.setSourceObject(this);
			dispatchEvent(&wss);
			receivedWeigherFeedback();

			if (Debug::isTracing("WeigherManager.WeigherSystemStatus") == true)
			{
				wss.show();
			}
		}
		break;
		case SysReqWakeup:
		{
			Debug::trace("WeigherManager.Message", HobString("SysReqWakeup"));

			WeigherSystemRequestWakeup wsrwu(recvBuffer);
			wsrwu.setSourceObject(this);

			const char *pCC = wsrwu.serialize();
			pMyWeigher->send(pCC, wsrwu.getSerializedLength());
		}
		break;
		case SysReqStatus:
		{
			Debug::trace("WeigherManager.Message", HobString("SysReqStatus"));
			if( ( style != "HTK" ) && ( style != "FSS" ) ) {
				WeigherSystemRequestStatus wsrs(recvBuffer);
				wsrs.setSourceObject(this);

				const char *pCC = wsrs.serialize();
				pMyWeigher->send(pCC, wsrs.getSerializedLength());
			} else {
				WeigherGeneric msg( WgReqSystemInfo, this );
				msg.setSourceObject(this);
				const char *pCC = msg.serialize();
				pMyWeigher->send(pCC, msg.getSerializedLength());
			}
		}
		break;
		case SysReqInquire:
		{
			Debug::trace("WeigherManager.Message", HobString("SysReqInquire"));
			if( ( style != "HTK" ) && ( style != "FSS" ) ){			
				WeigherSystemRequestInquire wsri(recvBuffer);
				wsri.setSourceObject(this);

				const char *pCC = wsri.serialize();
				pMyWeigher->send(pCC, wsri.getSerializedLength());
			} else {
				//KPC weigher does not support
				unsigned char fakeSignature[5] = { 'W','G','\0','\0','\0'};

				WeigherSystemInquire inquire( this, SysPermanent, &fakeSignature[0], 0, 0 );
				dispatchEvent( &inquire );
			}
		}
		break;
		case SysCalibSwitch:
		{
			Debug::trace("WeigherManager.Message", HobString("SysCalibSwitch"));

			WeigherSystemCalibrationSwitch calibSwitch(recvBuffer);
			calibSwitch.setSourceObject(this);
			dispatchEvent(&calibSwitch);
			receivedWeigherFeedback();
		}
		break;
		case SysReset:
		{
			Debug::trace("WeigherManager.Message", HobString("SysReset"));

			WeigherSystemReset wgSysReset(recvBuffer);
			wgSysReset.setSourceObject(this);

			const char *pCC = wgSysReset.serialize();
			pMyWeigher->send(pCC, wgSysReset.getSerializedLength());
		}
		break;
		default:
		{
			Debug::trace("WeigherManager.Error", HobString("Some Other System Event: ") + HobString(Integer::toString((UINT16) sysEvent)));
		}
		break;
	}
}


void WeigherManager::timeoutProcess( __int32 timer_id )
{
	if ( timer_id == weigherProbe )
	{
		probeTimeoutCntr_++;
		switch (probeTimeoutCntr_)
		{
			case 0:
			case 1://have received some messages from the weigher since last timeout.

			if( ( style == "HTK" ) || ( style == "FSS" ) || ( style == "HT" ) ) {	// NEW FSSS Code
				Debug::trace("WeigherManager", HobString("Probe Timeout(1) requesting wakeup"));
				requestWeigherWakeup();
			}						 							// ORIG trunk code has this commented out
			break;

			case 2:
			{
				Debug::trace("WeigherManager.WeigherProbe", HobString("Probe Timeout(2) requesting wakeup"));

				requestWeigherWakeup();

			}
			break;

			case 3:
			{
				Debug::trace("WeigherManager", HobString("****Weigher Lost Contact****"));

				probeTimeoutCntr_ = 0;
//#if 1 			/* SOF-5557 just log the problem, do not delete the managers */
				WeigherGeneric msg( WgKPCDetached, this );
				dispatchEvent( &msg );

				if (lastStatusMsg.isPeripheralComm() == true)
				{
					Debug::trace("WeigherManager", HobString("Dispatch Lost Comm message"));

					lastStatusMsg.clearPeripheralComm();
					lastStatusMsg.setManagerDeleted();
					dispatchEvent(&lastStatusMsg);
				}
//#endif				
			}
			break;
		}
	}
	if(timer_id == WeigherReqConfig){
		Debug::trace("WeigherManager.InquireTimer", "Processing Config Timer timeout");
		WeigherGeneric wri(WgReqConfig,this);
		send(&wri);
	}
	#ifdef WG_DEFAULTS_TEST
	if(timer_id == WeigherTest)
	{
		if( pMyWeigher != NULL )
		{
			testCounter++;
			switch (testCounter)
			{
				case 1:
				{
					Debug::trace("WeigherManager.Test", "Sending WgCat3ReqStatistics");

					WeigherGeneric msg(WgResetFactoryDefaults);
					msg.setSourceObject(this);
					const char *pCC = msg.serialize();
					pMyWeigher->send( pCC, msg.getSerializedLength() );
					break;
				}
			}
		}
	}
	#endif
	#ifdef CAT3_UNIT_TEST
	if(timer_id == WeigherTestCat3)
	{
		if( pMyWeigher != NULL )
		{
			testCounter++;
			if( testMsgCount == 0 )
			{
				switch (testCounter)
				{
					case 1:
					{
						Debug::trace("WeigherManager.Test", "Sending WgCat3ReqStatistics");

						//Request Cat3 statistics from weigher
						WeigherGeneric wMsg(WgCat3ReqStatistics, this);
						const char *pCC = wMsg.serialize();
						pMyWeigher->send(pCC, wMsg.getSerializedLength());
						break;
					}
					case 2:
					{
						Debug::trace("WeigherManager.Test", "Sending WgCat3DateTime");

						//Set Cat3 date and time for weigher
						WeigherCat3DateTime wCat3DateTime( this );
						wCat3DateTime.setEpochTime(0x12345678);
						const char *pCC = wCat3DateTime.serialize();
						pMyWeigher->send(pCC, wCat3DateTime.getSerializedLength());
						break;
					}
					case 3:
					{
						Debug::trace("WeigherManager.Test", "Sending WeigherCat3PageRequest");

						//Request Cat3 page from weigher
						WeigherCat3PageRequest wCat3Req( this );
						wCat3Req.setPageNumber(0);
						wCat3Req.setPageIndex(0);
						const char *pCC = wCat3Req.serialize();
						pMyWeigher->send(pCC, wCat3Req.getSerializedLength());
						break;
					}
					case 4:
					{
						Debug::trace("WeigherManager.Test", "Sending WeigherWriteCat3Record");

						//Save audit record to weigher audit trail
						WeigherWriteCat3Record wCat3Record( this );
						wCat3Record.setEvent( (AuditEvent) CONFIGURATION  );
						wCat3Record.setAuditType( (AuditType) REMOTE_EVENT );
						wCat3Record.setEpochTime(0x87654321);
						wCat3Record.setParamId( ( SealableParamID) FRACTIONAL_PRICING );
						wCat3Record.setNewValue(2);
						wCat3Record.setPreviousValue(1);

						const char *pCC = wCat3Record.serialize();
						pMyWeigher->send(pCC, wCat3Record.getSerializedLength());
						break;

					}
					case 5:
					{
						Debug::trace( "WeigherManager.Test", "Sending WgInquire" );
						WeigherGeneric wMsg( WgInquire, this );


						const char *pCC = wMsg.serialize();
						pMyWeigher->send( pCC, wMsg.getSerializedLength() );
						break;
					}
					case 6:
					{
						Debug::trace( "WeigherManager.Test", "Sending WgReqWakeup" );
						WeigherGeneric wMsg( WgReqWakeup, this );


						const char *pCC = wMsg.serialize();
						pMyWeigher->send( pCC, wMsg.getSerializedLength() );
						break;
					}
					case 7:
					{
						Debug::trace( "WeigherManager.Test", "Sending WgReqStatus" );
						WeigherRequestStatus wMsg( this );

						const char *pCC = wMsg.serialize();
						pMyWeigher->send( pCC, wMsg.getSerializedLength() );
						break;
					}
					case 8:
					{
		                Debug::trace( "WeigherManager.Test", "Sending WeigherEnable" );
		                WeigherGeneric wg( WgEnable, this );
		                wg.setDeviceType( Weigher );

						if( style != "NGS1" ) {
							const char *pCC = wg.serialize();
							pMyWeigher->send( pCC, wg.getSerializedLength() );
						} else {
							dispatchEvent( &wg );
						}
						break;
					}
					case 9:
					{
		                Debug::trace( "WeigherManager.Test", "Sending WeigherMode" );
		                WeigherMode wm( this, ScalePrepackMode );

						const char *pCC = wm.serialize();
						pMyWeigher->send( pCC, wm.getSerializedLength() );
						break;
					}
					case 10:
					{
		                Debug::trace( "WeigherManager.Test", "Sending WeigherDisable" );
		                WeigherGeneric wg( WgDisable, this );
		                wg.setDeviceType( Weigher );

						const char *pCC = wg.serialize();
						pMyWeigher->send( pCC, wg.getSerializedLength() );
						break;
					}
					case 11:
					{
		                Debug::trace( "WeigherManager.Test", "Sending WeigherRezero" );
		                WeigherGeneric wg( WgRezero, this );
		                wg.setDeviceType( Weigher );

						const char *pCC = wg.serialize();
						pMyWeigher->send( pCC, wg.getSerializedLength() );
		                break;
					}
					case 12:
					{
		                Debug::trace( "WeigherManager.Test", "Sending WeigherReset" );
		                WeigherGeneric wg( WgReset, this );
		                wg.setDeviceType( Weigher );

						const char *pCC = wg.serialize();
						pMyWeigher->send( pCC, wg.getSerializedLength() );
						break;
					}
					case 13:
					{
		                Debug::trace( "WeigherManager.Test", "Sending WeigherColdReset" );
		                WeigherGeneric wg( WgColdReset, this );

						const char *pCC = wg.serialize();
						pMyWeigher->send( pCC, wg.getSerializedLength() );
						break;
					}
					case 14:
					{
						Debug::trace( "WeigherManager.Test", "Sending WeigherConfiguration" );
						WeigherConfiguration config(this);
						const char *pCC = config.serialize();
						pMyWeigher->send( pCC, config.getSerializedLength() );
						break;
					}
					case 15:
					{
						break;
					}
					case 16:
					{
						/*
						Debug::trace("WeigherManager.Test", "Sending WgCat3ReqEraseLog");
						WeigherGeneric wMsg(WgCat3ReqEraseLog, this);
						const char *pCC = wMsg.serialize();
						pMyWeigher->send(pCC, wMsg.getSerializedLength());

						Debug::trace("WeigherManager.Test", "Sending WeigherCat3PageRequest");

						//Request Cat3 page from weigher
						WeigherCat3PageRequest wCat3Req( this );
						wCat3Req.setPageNumber(0);
						wCat3Req.setPageIndex(0);

						const char *pCC = wCat3Req.serialize();
						pMyWeigher->send(pCC, wCat3Req.getSerializedLength());
						*/
						break;
					}
					case 17:
					{
						/*
						Debug::trace("WeigherManager.Test", "Sending WeigherCat3ReqNextRecords");

						WeigherGeneric wMsg(WgCat3ReqNextRecords, this);

						const char *pCC = wMsg.serialize();
						pMyWeigher->send(pCC, wMsg.getSerializedLength());
						*/
						break;
					}

					case 18:
					{
						Debug::trace("WeigherManager.Test", "Enable Write CAT3 Record Test!");
						testMsgCount = 1;
						break;
					}

				}
			}
			else
			{
				if( testMsgCount > 0 )
				{
					Debug::trace("WeigherManager.Test", "Test Message Count: " + Integer::toString(testMsgCount));
					if( testMsgCount == 1 )
					{
						Debug::trace("WeigherManager.Test", "Sending WeigherWriteCat3Record: FRACTIONAL_PRICING ");
						//Save audit record to weigher audit trail
						WeigherWriteCat3Record wCat3Record( this );
						wCat3Record.setEvent( (AuditEvent)CONFIGURATION );
						wCat3Record.setAuditType((AuditType)REMOTE_EVENT );
						wCat3Record.setEpochTime(0x09abcdef);
						wCat3Record.setParamId((SealableParamID)FRACTIONAL_PRICING);
						wCat3Record.setNewValue(1);
						wCat3Record.setPreviousValue(0);

						const char *pCC = wCat3Record.serialize();
						pMyWeigher->send(pCC, wCat3Record.getSerializedLength());

					}
					if( ( testMsgCount == 2 ) || ( testMsgCount == 4 ) || ( testMsgCount == 6 )  )
					{
						Debug::trace("WeigherManager.Test", "Sending WgCat3ReqStatistics");

						//Request Cat3 statistics from weigher
						WeigherGeneric wMsg(WgCat3ReqStatistics, this);

						const char *pCC = wMsg.serialize();
						pMyWeigher->send(pCC, wMsg.getSerializedLength());

					}

					if( testMsgCount == 3 )
					{
						Debug::trace("WeigherManager.Test", "Sending WeigherWriteCat3Record: POUNDS_FOR_MULTIPLIER ");
						//Save audit record to weigher audit trail
						WeigherWriteCat3Record wCat3Record( this );
						wCat3Record.setEvent((AuditEvent)CONFIGURATION );
						wCat3Record.setAuditType((AuditType)LOCAL_EVENT);
						wCat3Record.setEpochTime(0x01234567);
						wCat3Record.setParamId((SealableParamID)POUNDS_FOR_MULTIPLIER);
						wCat3Record.setNewValue(0);
						wCat3Record.setPreviousValue(1);

						const char *pCC = wCat3Record.serialize();
						pMyWeigher->send(pCC, wCat3Record.getSerializedLength());
					}

					if( testMsgCount == 5 )
					{
						Debug::trace("WeigherManager.Test", "Sending WeigherWriteCat3Record: POUNDS_FOR_MULTIPLIER ");
						//Save audit record to weigher audit trail
						WeigherWriteCat3Record wCat3Record( this );
						wCat3Record.setEvent((AuditEvent)CONFIGURATION);
						wCat3Record.setAuditType((AuditType)REMOTE_EVENT );
						wCat3Record.setEpochTime(0x01234567);
						wCat3Record.setParamId((SealableParamID)SOFTWARE_UPDATE);
						wCat3Record.setNewValue(0x00010200);
						wCat3Record.setPreviousValue(0x00010100);

						const char *pCC = wCat3Record.serialize();
						pMyWeigher->send(pCC, wCat3Record.getSerializedLength());
					}

					testMsgCount++;
					if( testMsgCount >= 7 )
					{
						testCounter = 0;
						testMsgCount = 0;
					}
				}
			}
		}
	}
	#endif
	#ifdef VALUE_MAX_UNIT_TEST
	if(timer_id == WeigherTestValueMax)
	{
		testCounter++;
		if( testMsgCount == 0 )
		{
			switch (testCounter)
			{
				case 1:
				{
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: Assembly number ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_ASSEMBLY, this );

					const char *pCC = req.serialize();
					pMyWeigher->send(pCC, req.getSerializedLength());

					break;
				}
				case 2:
				{
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: hardware version ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_HWVERSION, this );

					const char *pCC = req.serialize();
					pMyWeigher->send(pCC, req.getSerializedLength());
					break;
				}
				case 3:
				{
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: serial number ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_SERIALNUM, this );

					const char *pCC = req.serialize();
					pMyWeigher->send(pCC, req.getSerializedLength());
					break;
				}
				case 4:
				{
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: date of manufacture ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_DATE, this );

					const char *pCC = req.serialize();
					pMyWeigher->send(pCC, req.getSerializedLength());
					break;
				}
				case 5:
				{
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: X gain ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_XGAINFACTOR, this );

					const char *pCC = req.serialize();
					pMyWeigher->send(pCC, req.getSerializedLength());
					break;
				}
				case 6:
				{
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: Y gain ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_YGAINFACTOR, this );

					const char *pCC = req.serialize();
					pMyWeigher->send(pCC, req.getSerializedLength());
					break;
				}
				case 7:
				{
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: Z gain ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_ZGAINFACTOR, this );

					const char *pCC = req.serialize();
					pMyWeigher->send(pCC, req.getSerializedLength());
					break;
				}
				case 8:
				{
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteAssembly: Assembly number: 12-345678 ");
					WeigherValueMaxWriteAssembly assembly( "12-345678", this );

					const char *pCC = assembly.serialize();
					pMyWeigher->send(pCC, assembly.getSerializedLength());
					break;
				}
				case 9:
				{
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteHwVersion: hardware version: B3 ");
					WeigherValueMaxWriteHwVersion hw( "B3", this );

					const char *pCC = hw.serialize();
					pMyWeigher->send(pCC, hw.getSerializedLength());
					break;
				}
				case 10:
				{
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteSerialNumber: serial number: fedcba987654321");
					WeigherValueMaxWriteSerialNumber num( "fedcba987654321", this );
					const char *pCC = num.serialize();
					pMyWeigher->send(pCC, num.getSerializedLength());
					break;
				}
				case 11:
				{
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteDateOfManufacture: date of manufacture: 631228187 = 2/21/2018 21:4:32 GMT ");
					WeigherValueMaxWriteDateOfMan date( 631228187, this );
					const char *pCC = date.serialize();
					pMyWeigher->send(pCC, date.getSerializedLength());
					break;
				}
				case 12:
				{
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteGain: X gain: 3.141592");
					WeigherValueMaxWriteXGainFactor gain( 3.141592, this );
					const char *pCC = gain.serialize();
					pMyWeigher->send(pCC, gain.getSerializedLength());
					break;
				}
				case 13:
				{
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteGain: Y gain: 1.414");
					WeigherValueMaxWriteYGainFactor gain( 1.414, this );
					const char *pCC = gain.serialize();
					pMyWeigher->send(pCC, gain.getSerializedLength());
					break;
				}
				case 14:
				{
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteGain: Z gain:  2.123 ");
					WeigherValueMaxWriteZGainFactor gain( 2.123, this );
					const char *pCC = gain.serialize();
					pMyWeigher->send(pCC, gain.getSerializedLength());
					break;
				}
				case 15:
				{
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: All current gains ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_CURRENTFACTORS, this );
					const char *pCC = req.serialize();
					pMyWeigher->send(pCC, req.getSerializedLength());
				}
				if( testMsgCount == 15 ) {
					testCounter = 1;
					testMsgCount = 0;
				}

			}
		}
	}
	#endif
}

void WeigherManager::receivedWeigherFeedback()
{
	if (lastStatusMsg.isPeripheralComm() == false)
	{
		lastStatusMsg.setPeripheralComm();
		dispatchEvent(&lastStatusMsg);

		sendWeigherKind();
	}

	Debug::trace("WeigherManager.WeigherProbe", HobString("receivedWeigherFeedback"));

	probeTimeoutCntr_ = 0;
}

void WeigherManager::requestWeigherWakeup()
{
	Debug::trace("WeigherManager.WeigherProbe", HobString("requesting weigher wakeup"));

	WeigherRequestWakeup wrw( this, physicalAddress);

	const char *pCC = wrw.serialize();
	pMyWeigher->send(pCC, wrw.getSerializedLength());
}

// The Ultima Weigher does not respond to this message!!!
void WeigherManager::requestWeigherSystemWakeup()
{
	WeigherSystemRequestWakeup wswu( this);

	const char *pCC = wswu.serialize();
	pMyWeigher->send(pCC, wswu.getSerializedLength());
}

void WeigherManager::requestWeigherStatus()
{
	WeigherRequestStatus wrs(this);

	const char *pCC = wrs.serialize();
	pMyWeigher->send(pCC, wrs.getSerializedLength());
}

void WeigherManager::requestWeigherSystemStatus()
{
	WeigherSystemRequestStatus wrss(this);

	const char *pCC = wrss.serialize();
	pMyWeigher->send(pCC, wrss.getSerializedLength());
}

void WeigherManager::requestWeigherInquire()
{
	WeigherRequestInquire wri(this);

	const char *pCC = wri.serialize();
	pMyWeigher->send(pCC, wri.getSerializedLength());
}

void WeigherManager::sendWeigherMode(WgOperatingMode opMode)
{
	WeigherMode wm( this, opMode);

	const char *pCC = wm.serialize();
	pMyWeigher->send(pCC,  wm.getSerializedLength());
}

void WeigherManager::sendWeigherKind()
{
	WeigherWeigherKind wwk( this, wgKind);
	dispatchEvent(&wwk);
}

void WeigherManager::sendDateTime()
{
	// For Category III, have to send the current date/time to the Weigher Manager before permanently setting any configuration or calibration values.
	time_t now;
	time(&now);
	WeigherCat3DateTime wCat3DateTime( this, (UINT32) now );
	const char *pCC = wCat3DateTime.serialize();
	pMyWeigher->send(pCC, wCat3DateTime.getSerializedLength());

	struct tm * timeinfo = localtime (&now);
	char buffer [80];
	strftime (buffer,80,"%c",timeinfo);
	Debug::trace("WeigherManager.cat3", std::string("sent date time to FPC: ulong: ") + ULong::toString(now) + std::string("  string: ") + std::string(buffer));
}


void WeigherManager::testWeigher(int i)
{
	if ((i % 0x100) == 0)
	{
		requestWeigherSystemStatus();
	}

	if ((i % 0x80) == 0)
	{
		requestWeigherWakeup();
	}

/*
	const char *pCC = NULL;

	if ((i % 0x500) == 0)
	{
		WeigherRequestInquire wi(this);

		pCC = wi.serialize();
		pMyWeigher->send(pCC, wi.getSerializedLength());
	}

	if (i == 0x800)
	{
		WeigherConfiguration wc(this);

		// Set config to This Scale!!
		// These are the only values that changed!!
		wc.setConfigType(WgPermanent);
		wc.setCenterOfMaintenanceZone(0);
		wc.setMaxWeight(900000);
		wc.setGainFactor(10000);
		wc.setZeroReference(0);
		wc.setCalibrationReference(0);
		wc.setSmallMotionLimit(15);

		pCC = wc.serialize();
		pMyWeigher->send(pCC, wc.getSerializedLength());
	}

	if ((i % 10000) == 0)		
	{
		sendWeigherMode(ScalePrepackMode);
	}
	else if ((i % 7500) == 0)
	{
		sendWeigherMode(ScaleWeightTrackingMode);
	}
	else if ((i % 5000) == 0)
	{
		sendWeigherMode(ScaleDumbMode);
	}
	else if ((i % 2500) == 0)
	{
		sendWeigherMode(ScaleManualMode);
	}*/
}
