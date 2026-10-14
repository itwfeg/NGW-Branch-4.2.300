/**
 * @file CANWeigherManager.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief
 * Created on: Jun 24, 2015
 */
#include "MetSig.h"
#include "CANWeigherManager.h"
#include "FpmManager.h"
#include "Configuration.h"
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
#include "WeigherSystemRequestValueMax.h"
#include "WeigherValueMaxFitParams.h"

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
#include "WeigherFactoryDefaultsComplete.h"
#include "WeigherKpcError.h"
#include "KpcSystemErrors.h"
#include "BoardSiteGeneric.h"

#include "Debug.h"
#include "Hex.h"
#include "Integer.h"
#include "ULong.h"


CANWeigherManager::CANWeigherManager( Hobart::CEvolMsgReceiver *pTranslator_,  WeigherKind wgKind ) :
									  Hobart::CEvolMsgReceiver("CANWeigherManager")
{
	Debug::trace("CANWeigherManager", "Constructor");

	wgKind_ = wgKind;

	if( pTranslator_ != NULL )
		this->pTranslator_ = pTranslator_;

	style_ = FpmManager::getInstance()->getMFGSystemStyle();

	registerWeighEvents();

	weigherLocked_ 		= false;
	largeMotionLocked_ 	= false;
	calSwitch_ 			= false;
	cfgOutOfRange_ 		= false;
#if defined ( __linux__ )
	inquireOnce_ 		= false;
#endif	
	vmErrorSent_ 		= false;	//SOF-3958

	noResponseCntr_ 	= 0;
	currentWeighMode_ 	= WeighMode_Avoir;
	scaleFactor_		= 0;
	reqConfigResendCounter_ = 0;


	createTimer( Configuration::getInstance()->getWeigherProbeTime() * 1000, weigherProbe, CHobartTimer::PERIODIC_TIMER);
	//queue up a request for inquire to start things off
	WeigherGeneric wri(WgReqConfig,this);
	send(&wri);

	sendWeigherKind();

#if defined ( __linux__ )
	util_filesystem::File  boardSiteFile(BOARD_SITE_FILE_NAME);
	// check if we are running board site to enable valuemax testing
	if ( boardSiteFile.exists() )  {	
		boardSiteTest_ = true;
	} else {
		boardSiteTest_ = false;
	}
#endif

	#ifdef _UNIT_TEST
	testTime_ = (15 * 100 ) + 1000;
	#endif

	#ifdef CAT3_UNIT_TEST
	createTimer( testTime_, WeigherTestCat3, CHobartTimer::PERIODIC_TIMER );
	#endif

	#ifdef WG_DEFAULTS_TEST
	createTimer( testTime_, WeigherTestDefaults, CHobartTimer::PERIODIC_TIMER );
	#endif

	#ifdef VALUE_MAX_UNIT_TEST	
	Debug::trace("WeigherManager.Test", "Create value max test timer.");
	//createTimer( testTime_, WeigherTestValueMax, CHobartTimer::PERIODIC_TIMER );
	#endif
	
}

CANWeigherManager::~CANWeigherManager()
{
	Debug::trace("CANWeigherManager", "Destructor");

	cancelTimer( weigherProbe );

	#ifdef CAT3_UNIT_TEST
	cancelTimer( WeigherTestCat3 );
	#endif
	#ifdef VALUE_MAX_UNIT_TEST
	cancelTimer( WeigherTestValueMax );
	#endif
	#ifdef WG_DEFAULTS_TEST
	cancelTimer( WeigherTestDefaults );
	#endif

	lastStatusMsg_.setManagerDeleted();
	dispatchEvent( &lastStatusMsg_ );

	ShutdownNicely();
}


void CANWeigherManager::registerWeighEvents()
{
	//weigh events
	registerEvent( Weigher, WgReqStatus );
	registerEvent( Weigher, WgReqInquire );
	registerEvent( Weigher, WgReqConfig );
	registerEvent( Weigher, WgReqWakeup );
	registerEvent( Weigher, WgReqKind );
	registerEvent( Weigher, WgReqValueMax);

	registerEvent( Weigher, WgEnable );
	registerEvent( Weigher, WgDisable );
	registerEvent( Weigher, WgRezero );
	registerEvent( Weigher, WgReset );
	registerEvent( Weigher, WgColdReset );
	registerEvent( Weigher, WgConfig );
	registerEvent( Weigher, WgMode );
	registerEvent( Weigher, WgNormal );
	registerEvent( Weigher, WgLock );
	registerEvent( Weigher, WgControl );

	//cat3 events
	registerEvent( Weigher, WgCat3ReqStatistics );
	registerEvent( Weigher, WgCat3ReqPage );
	registerEvent( Weigher, WgCat3ReqNextRecords );
	registerEvent( Weigher, WgCat3RecordStatus );
	registerEvent( Weigher, WgCat3AuditStatistics );
	registerEvent( Weigher, WgCat3Records );
	registerEvent( Weigher, WgCat3WriteRecord );
	registerEvent( Weigher, WgCat3ReqEraseLog );
	registerEvent( Weigher, WgCat3LogEraseComplete );
	registerEvent( Weigher, WgCat3Error );
	registerEvent( Weigher, WgResetFactoryDefaults );

	//value max
	registerEvent( Weigher, WgValueMaxAssembly );
	registerEvent( Weigher, WgValueMaxHwVersion );
	registerEvent( Weigher, WgValueMaxSerialNumber );
	registerEvent( Weigher, WgValueMaxDateOfManufacture );
	registerEvent( Weigher, WgValueMaxXGain );
	registerEvent( Weigher, WgValueMaxYGain );
	registerEvent( Weigher, WgValueMaxZGain );
	registerEvent( Weigher, WgValueMaxAllGains );
	registerEvent( Weigher, WgKPCError );
	registerEvent( Weigher, WgSystemInfo );
	registerEvent( Weigher, WgValueMaxFitParams );

	registerEvent(WeigherSystem, SysReqStatus);

	//someone needs to listen to kpc system error messages.
	if( wgKind_ == WgCanPrepack ) {
		registerEvent( KPCSystem, KpcError );
	}
}

void CANWeigherManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
 {

	if ( msg.get() != NULL )
	{
		switch (msg->getDeviceType())
		{
			case Weigher		:	handleWeigherEvent((WgEvent) msg->getEventType(), msg->Data() );
									break;
			case WeigherSystem	:	handleWeigherSysEvent((SysEvent) msg->getEventType(), msg->Data() );
									break;
			case KPCSystem		: 	handleKpcSystemError(  (KpcSystemEvent)msg->getEventType(), msg->Data() );
			default				:
									break;
		}
	}
}

void CANWeigherManager::timeoutProcess( __int32 timer_id )
{
	if ( timer_id == weigherProbe )
	{
		noResponseCntr_++;
		switch (noResponseCntr_)
		{
			case 0:
			case 1://have received some messages from the weigher since last timeout.
			if(( wgKind_ == WgCanKpc ) || ( wgKind_ == WgCanPrepack )) {
				Debug::trace("CANWeigherManager", HobString("Probe Timeout(1) requesting wakeup"));
				requestWeigherWakeup();
			}
			break;

			case 2:{
				Debug::trace("CANWeigherManager", HobString("Probe Timeout requesting wakeup"));
				requestWeigherWakeup();
			}
			break;

			case 3:{
				Debug::trace("CANWeigherManager", HobString("****Weigher Lost Contact****"));

				noResponseCntr_ = 0;

				WeigherGeneric msg( WgKPCDetached, this );
				dispatchEvent( &msg );

				if (lastStatusMsg_.isPeripheralComm() == true)
				{
					Debug::trace("CANWeigherManager", HobString("Dispatch Lost Comm message"));

					lastStatusMsg_.clearPeripheralComm();
					lastStatusMsg_.setManagerDeleted();
					dispatchEvent(&lastStatusMsg_);
				}
			}
			break;
		}
	}
	if( timer_id == WeigherReqConfig ){
		Debug::trace("CANWeigherManager.InquireTimer", "Processing Config Timer timeout");
		WeigherGeneric wri(WgReqConfig,this);
		send(&wri);
	}
	//SOF-3958
	if( timer_id == VMErrorTimer ) {
		//allow another valuemax error to be sent to ui.
		vmErrorSent_ = false;
		Debug::trace("CANWeigherManager.VMError", "timeout");
	}

	#ifdef WG_DEFAULTS_TEST
	if( timer_id == WeigherTestDefaults ) {
		if( pTranslator_ != NULL ) {
			testCounter_++;
			switch ( testCounter_ )
			{
				case 1: {
					Debug::trace("WeigherManager.Test", "Sending WgCat3ReqStatistics");

					WeigherGeneric msg(WgResetFactoryDefaults);
					msg.setSourceObject(this);
					const char *pCC = msg.serialize();
					pTranslator_->send( pCC, msg.getSerializedLength() );
					break;
				}
			}
		}
	}
	#endif

	#ifdef CAT3_UNIT_TEST
	if( timer_id == WeigherTestCat3 ) {
		if( pTranslator_ != NULL ) {
			testCounter_++;
			if( testMsgCount_ == 0 ) {
				switch( testCounter_ )
				{
					case 1: {
						Debug::trace("WeigherManager.Test", "Sending WgCat3ReqStatistics");

						//Request Cat3 statistics from weigher
						WeigherGeneric wMsg(WgCat3ReqStatistics, this);
						const char *pCC = wMsg.serialize();
						pTranslator_->send(pCC, wMsg.getSerializedLength());
						break;
					}
					case 2: {
						Debug::trace("WeigherManager.Test", "Sending WgCat3DateTime");

						//Set Cat3 date and time for weigher
						WeigherCat3DateTime wCat3DateTime( this );
						wCat3DateTime.setEpochTime(0x12345678);
						const char *pCC = wCat3DateTime.serialize();
						pTranslator_->send(pCC, wCat3DateTime.getSerializedLength());
						break;
					}
					case 3: {
						Debug::trace("WeigherManager.Test", "Sending WeigherCat3PageRequest");

						//Request Cat3 page from weigher
						WeigherCat3PageRequest wCat3Req( this );
						wCat3Req.setPageNumber(0);
						wCat3Req.setPageIndex(0);
						const char *pCC = wCat3Req.serialize();
						pTranslator_->send(pCC, wCat3Req.getSerializedLength());
						break;
					}
					case 4: {
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
						pTranslator_->send(pCC, wCat3Record.getSerializedLength());
						break;

					}
					case 5: {
						Debug::trace( "WeigherManager.Test", "Sending WgInquire" );
						WeigherGeneric wMsg( WgInquire, this );


						const char *pCC = wMsg.serialize();
						pTranslator_->send( pCC, wMsg.getSerializedLength() );
						break;
					}
					case 6: {
						Debug::trace( "WeigherManager.Test", "Sending WgReqWakeup" );
						WeigherGeneric wMsg( WgReqWakeup, this );


						const char *pCC = wMsg.serialize();
						pTranslator_->send( pCC, wMsg.getSerializedLength() );
						break;
					}
					case 7: {
						Debug::trace( "WeigherManager.Test", "Sending WgReqStatus" );
						WeigherRequestStatus wMsg( this );

						const char *pCC = wMsg.serialize();
						pTranslator_->send( pCC, wMsg.getSerializedLength() );
						break;
					}
					case 8: {
		                Debug::trace( "WeigherManager.Test", "Sending WeigherEnable" );
		                WeigherGeneric wg( WgEnable, this );
		                wg.setDeviceType( Weigher );

						const char *pCC = wg.serialize();
						pTranslator_->send( pCC, wg.getSerializedLength() );
						break;
					}
					case 9: {
		                Debug::trace( "WeigherManager.Test", "Sending WeigherMode" );
		                WeigherMode wm( this, ScalePrepackMode );

						const char *pCC = wm.serialize();
						pTranslator_->send( pCC, wm.getSerializedLength() );
						break;
					}
					case 10: {
		                Debug::trace( "WeigherManager.Test", "Sending WeigherDisable" );
		                WeigherGeneric wg( WgDisable, this );
		                wg.setDeviceType( Weigher );

						const char *pCC = wg.serialize();
						pTranslator_->send( pCC, wg.getSerializedLength() );
						break;
					}
					case 11: {
		                Debug::trace( "WeigherManager.Test", "Sending WeigherRezero" );
		                WeigherGeneric wg( WgRezero, this );
		                wg.setDeviceType( Weigher );

						const char *pCC = wg.serialize();
						pTranslator_->send( pCC, wg.getSerializedLength() );
		                break;
					}
					case 12: {
		                Debug::trace( "WeigherManager.Test", "Sending WeigherReset" );
		                WeigherGeneric wg( WgReset, this );
		                wg.setDeviceType( Weigher );

						const char *pCC = wg.serialize();
						pTranslator_->send( pCC, wg.getSerializedLength() );
						break;
					}
					case 13: {
		                Debug::trace( "WeigherManager.Test", "Sending WeigherColdReset" );
		                WeigherGeneric wg( WgColdReset, this );

						const char *pCC = wg.serialize();
						pTranslator_->send( pCC, wg.getSerializedLength() );
						break;
					}
					case 14: {
						Debug::trace( "WeigherManager.Test", "Sending WeigherConfiguration" );
						WeigherConfiguration config(this);
						const char *pCC = config.serialize();
						pTranslator_->send( pCC, config.getSerializedLength() );
						break;
					}
					case 15: {
						break;
					}
					case 16: {
						/*
						Debug::trace("WeigherManager.Test", "Sending WgCat3ReqEraseLog");
						WeigherGeneric wMsg(WgCat3ReqEraseLog, this);
						const char *pCC = wMsg.serialize();
						pTranslator_->send(pCC, wMsg.getSerializedLength());

						Debug::trace("WeigherManager.Test", "Sending WeigherCat3PageRequest");

						//Request Cat3 page from weigher
						WeigherCat3PageRequest wCat3Req( this );
						wCat3Req.setPageNumber(0);
						wCat3Req.setPageIndex(0);

						const char *pCC = wCat3Req.serialize();
						pTranslator_->send(pCC, wCat3Req.getSerializedLength());
						*/
						break;
					}
					case 17: {
						/*
						Debug::trace("WeigherManager.Test", "Sending WeigherCat3ReqNextRecords");

						WeigherGeneric wMsg(WgCat3ReqNextRecords, this);

						const char *pCC = wMsg.serialize();
						pTranslator_->send(pCC, wMsg.getSerializedLength());
						*/
						break;
					}
					case 18: {
						Debug::trace("WeigherManager.Test", "Enable Write CAT3 Record Test!");
						testMsgCount_ = 1;
						break;
					}
				}
			} else {
				if( testMsgCount_ > 0 ) {
					Debug::trace("WeigherManager.Test", "Test Message Count: " + Integer::toString(testMsgCount_));
					if( testMsgCount_ == 1 ) {
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
						pTranslator_->send(pCC, wCat3Record.getSerializedLength());

					}
					if( ( testMsgCount_ == 2 ) || ( testMsgCount_ == 4 ) || ( testMsgCount_ == 6 )  ) {
						Debug::trace("WeigherManager.Test", "Sending WgCat3ReqStatistics");

						//Request Cat3 statistics from weigher
						WeigherGeneric wMsg(WgCat3ReqStatistics, this);

						const char *pCC = wMsg.serialize();
						pTranslator_->send(pCC, wMsg.getSerializedLength());

					}

					if( testMsgCount_ == 3 ) {
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
						pTranslator_->send(pCC, wCat3Record.getSerializedLength());
					}

					if( testMsgCount_ == 5 ) {
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
						pTranslator_->send(pCC, wCat3Record.getSerializedLength());
					}

					testMsgCount_++;
					if( testMsgCount_ >= 7 )
					{
						testCounter_ = 0;
						testMsgCount_ = 0;
					}
				}
			}
		}
	}
	#endif
	#ifdef VALUE_MAX_UNIT_TEST
	if( timer_id == WeigherTestValueMax ) {
		testCounter_++;
		if( testMsgCount_ == 0 ) {
			switch( testCounter_ )
			{
				case 1: {
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: Assembly number ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_ASSEMBLY, this );

					const char *pCC = req.serialize();
					pTranslator_->send(pCC, req.getSerializedLength());

					break;
				}
				case 2: {
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: hardware version ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_HWVERSION, this );

					const char *pCC = req.serialize();
					pTranslator_->send(pCC, req.getSerializedLength());
					break;
				}
				case 3: {
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: serial number ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_SERIALNUM, this );

					const char *pCC = req.serialize();
					pTranslator_->send(pCC, req.getSerializedLength());
					break;
				}
				case 4: {
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: date of manufacture ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_DATE, this );

					const char *pCC = req.serialize();
					pTranslator_->send(pCC, req.getSerializedLength());
					break;
				}
				case 5: {
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: X gain ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_XGAINFACTOR, this );

					const char *pCC = req.serialize();
					pTranslator_->send(pCC, req.getSerializedLength());
					break;
				}
				case 6: {
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: Y gain ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_YGAINFACTOR, this );

					const char *pCC = req.serialize();
					pTranslator_->send(pCC, req.getSerializedLength());
					break;
				}
				case 7: {
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: Z gain ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_ZGAINFACTOR, this );

					const char *pCC = req.serialize();
					pTranslator_->send(pCC, req.getSerializedLength());
					break;
				}
				case 8: {
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteAssembly: Assembly number: 12-345678 ");
					WeigherValueMaxWriteAssembly assembly( "12-345678", this );

					const char *pCC = assembly.serialize();
					pTranslator_->send(pCC, assembly.getSerializedLength());
					break;
				}
				case 9: {
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteHwVersion: hardware version: B3 ");
					WeigherValueMaxWriteHwVersion hw( "B3", this );

					const char *pCC = hw.serialize();
					pTranslator_->send(pCC, hw.getSerializedLength());
					break;
				}
				case 10: {
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteSerialNumber: serial number: fedcba987654321");
					WeigherValueMaxWriteSerialNumber num( "fedcba987654321", this );
					const char *pCC = num.serialize();
					pTranslator_->send(pCC, num.getSerializedLength());
					break;
				}
				case 11: {
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteDateOfManufacture: date of manufacture: 631228187 = 2/21/2018 21:4:32 GMT ");
					WeigherValueMaxWriteDateOfMan date( 631228187, this );
					const char *pCC = date.serialize();
					pTranslator_->send(pCC, date.getSerializedLength());
					break;
				}
				case 12: {
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteGain: X gain: 3.141592");
					WeigherValueMaxWriteXGainFactor gain( 3.141592, this );
					const char *pCC = gain.serialize();
					pTranslator_->send(pCC, gain.getSerializedLength());
					break;
				}
				case 13: {
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteGain: Y gain: 1.414");
					WeigherValueMaxWriteYGainFactor gain( 1.414, this );
					const char *pCC = gain.serialize();
					pTranslator_->send(pCC, gain.getSerializedLength());
					break;
				}
				case 14: {
					Debug::trace("WeigherManager.Test", "Sending WgValueMaxWriteGain: Z gain:  2.123 ");
					WeigherValueMaxWriteZGainFactor gain( 2.123, this );
					const char *pCC = gain.serialize();
					pTranslator_->send(pCC, gain.getSerializedLength());
					break;
				}
				case 15: {
					Debug::trace("WeigherManager.Test", "Sending WgReqValueMax: All current gains ");
					WeigherReqValueMax req( WeigherReqValueMax::VMREQ_CURRENTFACTORS, this );
					const char *pCC = req.serialize();
					pTranslator_->send(pCC, req.getSerializedLength());
				}
				if( testMsgCount_ == 15 ) {
					testCounter_ = 1;
					testMsgCount_ = 0;
				}

			}
		}
	}
	#endif
}

void CANWeigherManager::handleKpcSystemError( KpcSystemEvent event, const char *recvBuffer)
{
	switch (event)
	{
		case KpcError: {
			KpcSystemErrors error( recvBuffer );
			std::string str = "KPC System: " + error.getErrorDescription();
			Debug::trace("CANWeigherManager.Error", str);
			break;
		}
	}
}

void CANWeigherManager::handleWeigherSysEvent(SysEvent sysEvent, const char *recvBuffer)
{
	switch (sysEvent)
	{
		case SysReqStatus:
		{
			Debug::trace("CANWeigherManager.Message", HobString("SysReqStatus"));
			WeigherGeneric msg( WgReqSystemInfo, this );
			msg.setSourceObject(this);
			const char *pCC = msg.serialize();
			pTranslator_->send(pCC, msg.getSerializedLength());
		}
		break;
		default:
		{
			Debug::trace("CANWeigherManager", "Unsupported CAN Weigher system event: " + HobString(Integer::toString((UINT16) sysEvent)));
		}
		break;
	}
}

void CANWeigherManager::handleWeigherEvent(WgEvent wgEvent, const char *recvBuffer)
{
	Debug::trace("CANWeigherManager.Message", "Top of handleWeigherEvent():  wgEventType = " + Integer::toString(wgEvent));

	switch (wgEvent)
	{
		case WgWakeup:
		{
			Debug::trace("CANWeigherManager.Message.Probe", HobString("WgWakeup"));
			clearResponseCntr();
		}
		break;
		case WgInquire:
		{
			Debug::trace("CANWeigherManager.InquireTimer", "Stopping Timer");
			cancelTimer(WeigherReqConfig);
			reqConfigResendCounter_= 0;

			Debug::trace("CANWeigherManager.Message", HobString("WgInquire"));

			WeigherInquire wi(recvBuffer);
			wi.setSourceObject(this);

			if ( MetSig::isWeigherDualRangeCapable( wi.getWeigherModel() ) )
				cfgOutOfRange_ = wi.isUserConfigOutOfRange();

			currentWeighMode_ = wi.getScaleWeighMode();

			dispatchEvent(&wi);

			clearResponseCntr();

			// Set the WeigherManager's internal scaleFactor from the Configuration just received from the weigher.
			scaleFactor_ = wi.getScaleFactor();

			// If we are running board site then enable value max.
#if defined ( __linux__ )
			if( boardSiteTest_ ) {
					if( !inquireOnce_ ) {
						inquireOnce_ = true;
						Debug::trace("CANWeigherManager.Message", "Inquire BoardSite Config.");				
						wi.setValueMaxEnabled( true );					
						WeigherConfiguration wc( WgPermanent, wi );
						wc.setSourceObject(this);
						const char *pCC = wc.serialize();
						pTranslator_->send(pCC, (unsigned int)wc.getSerializedLength());					
					
						PAL_absclass::PAL_sleep( 500 );
						sendWeigherMode( ScaleManualMode );	
					}

			}
				
			
#endif

			if (Debug::isTracing("CANWeigherManager.WeigherInquire") == true) {
				wi.show();
			}
		}
		break;
		case WgConfig:
		{
			Debug::trace("CANWeigherManager.Message", HobString("WgConfig"));

			WeigherConfiguration wc(recvBuffer);
			wc.setSourceObject(this);

			//if ( (wc.getConfigType() & 0x7FFF) == 0 ) 	// It's WgPermanent
			// For Category III, have to send the current date/time to the Weigher Manager before permanently setting any configuration or calibration values.
			sendDateTime();

			// If the weigher's scaleFactor has changed, reset the WeigherManager's
			// internal scaleFactor before passing the Configuration to the weigher.
			if (scaleFactor_ != wc.getScaleFactor())
				scaleFactor_ = wc.getScaleFactor();

			if ( MetSig::isWeigherDualRangeCapable( wc.getWeigherModel() ) )
				cfgOutOfRange_ = wc.isUserConfigOutOfRange();

			const char *pCC = wc.serialize();
			pTranslator_->send(pCC, (unsigned int)wc.getSerializedLength());
			wc.show();

			//	Send a WeigherColdReset when the weigh mode.
			if (currentWeighMode_ != wc.getScaleWeighMode())
			{
				currentWeighMode_ = wc.getScaleWeighMode();
				if (wgKind_ == WgUsbAWS)
				{
					WeigherColdReset wcr;
					wcr.setSourceObject(this);

					const char *pCC = wcr.serialize();
					pTranslator_->send(pCC, (unsigned int)wcr.getSerializedLength());
					Debug::trace("CANWeigherManager.Message", HobString("WgConfig - WeighMode changed. Sent a WeigherColdReset to the weigher."));
				}
			}
		}
		break;
		// We always keep track of the last status message received but
		// we will relay this message through the system only if the weigher is
		// not "locked".
		case WgStatus: {
			Debug::trace("CANWeigherManager.Message", HobString("WgStatus"));

			WeigherStatus weigherStatus( recvBuffer );
			weigherStatus.setSourceObject( this );

#if defined ( __linux__ )
			if( boardSiteTest_ ) {					
                if( ( weigherStatus.getAccelX() != 0 ) ||  ( weigherStatus.getAccelY() != 0 ) ||  
                    ( weigherStatus.getAccelZ() != 0 ) ){
					BoardSiteGeneric msg( BrdSitePeripheralValuePass, this ); 
					dispatchEvent( &msg );
				}
			}
#endif

			//SOF-3958
			if( ( weigherStatus.isValMaxEepAcclCommFailure() ) || ( weigherStatus.isValMaxEepCommFailure() ) ||
			    ( weigherStatus.isValMaxEepChecksumFailure() ) ) {
					Debug::trace("CANWeigherManager.VMError", "Error: " + Hex::toString(weigherStatus.getStatusBits()));
					if( !vmErrorSent_ ) {
						Debug::trace("CANWeigherManager.VMError", "Sending VM Error!");
						//cancel previous timer even though it a one shot!
						cancelTimer( VMErrorTimer );
						vmErrorSent_ = true;

						//do not send another status with errors set until timer times out.
						createTimer( ( 30 * MILLISECTOSEC ), VMErrorTimer, false );	
					} else {
						Debug::trace("CANWeigherManager.VMError", "Clearing VM Error!");
						weigherStatus.clearValMaxEepCommFailure();
						weigherStatus.clearValMaxAcclCommFailure();
						weigherStatus.clearValMaxEepChecksumFailure();
					}	
				}


			// Set the scaleFactor in the weigherStatus message before dispatching it.
			weigherStatus.setScaleFactor( scaleFactor_ );

			// WeigherManager keeps track if any user configurable configuration value is out of range.
			// Indicate this to the rest of the system via the WeigherStatus.
			weigherStatus.setWeigherConfigOutOfRange( cfgOutOfRange_ );


			if ( weigherStatus.isKeyPressed() != calSwitch_ ) {
				calSwitch_ = weigherStatus.isKeyPressed();

				WeigherSystemCalibrationSwitch calibSwitch( this, calSwitch_ );
				dispatchEvent( &calibSwitch );
				calibSwitch.show();
			}

			if ( weigherStatus.equals(lastStatusMsg_) == false ) {
				if ( weigherLocked_ == false ) {
					if ( (lastStatusMsg_.isNegativeGrossWeight()) && (!weigherStatus.isNegativeGrossWeight()) && (weigherStatus.isLargeMotion()) &&
						(!weigherStatus.isPositiveNonZeroStableWeight()) && (!weigherStatus.isZeroStableWeight()) &&
						(Configuration::getInstance()->isServiceCounterScale())	) {
						Debug::trace("CANWeigherManager.Error", "Hey, we are throwing out crap values here");

#if defined ( __linux__ )
						if( boardSiteTest_ ) {
							dispatchEvent( &weigherStatus );
						}
#endif
					} else {
						dispatchEvent( &weigherStatus );
						weigherStatus.show();
					}
				} else {
					if ( weigherStatus.isLargeMotion() )
						largeMotionLocked_ = true;

#if defined ( __linux__ )					
					if( boardSiteTest_ ) {
						dispatchEvent( &weigherStatus );
					}
#endif

					Debug::trace("CANWeigherManager.Status", HobString("** Weigher ** LOCKED **"));
					weigherStatus.show();
				}
				lastStatusMsg_ = weigherStatus;
	
			} 
			clearResponseCntr();
			break;
		}
		case WgControl:		Debug::trace("CANWeigherManager.Message", HobString("WgControl"));
		case WgEnable:		Debug::trace("CANWeigherManager.Message", HobString("WgEnable"));
		case WgDisable:		Debug::trace("CANWeigherManager.Message", HobString("WgDisable"));
		case WgReset:		Debug::trace("CANWeigherManager.Message", HobString("WgReset"));
		case WgColdReset:	Debug::trace("CANWeigherManager.Message", HobString("WgColdReset"));
		case WgRezero:		Debug::trace("CANWeigherManager.Message", HobString("WgRezero")); {
			WeigherGeneric wg( recvBuffer );
			wg.setSourceObject( this );

			const char *pCC = wg.serialize();
			pTranslator_->send( pCC, (unsigned int)wg.getSerializedLength() );
		}
		break;
		case WgMode: {
			Debug::trace("CANWeigherManager.Message", HobString("WgMode"));

			WeigherMode wm( recvBuffer );
			wm.setSourceObject(this);

			wm.show();

			const char *pCC = wm.serialize();
			pTranslator_->send( pCC, (unsigned int)wm.getSerializedLength() );
		}
		break;
		// This message is sent by the Operating Modes to enable the distribution
		// of weigher status messages through the system as they arrive. Also,
		// since the last status is always kept in memory, even if not distributed
		// through the system, it could be distributed now if necessary.
		case WgNormal: {
			Debug::trace("CANWeigherManager.WgNormal", HobString("WgNormal"));

			if ( weigherLocked_ ) {
				Debug::trace("CANWeigherManager.WgNormal", HobString("weigherWasLocked"));

				// If during the time the "weigher window was closed" we got a
				// large motion we want to notify it to the Operating Mode
				if ( largeMotionLocked_ ) {
					WeigherCachedStatus wcs ( this );

					Debug::trace("CANWeigherManager.WgNormal", HobString("dispatching a WeigherCachedStatus with large motion"));

					wcs.setLargeMotion( true );
					dispatchEvent( &wcs );

					largeMotionLocked_ = false;
				}

				// If we had a last status with a zero stable weight, lets notify
				// it so we don't miss the last zero stable weight.
				if ( lastStatusMsg_.isZeroStableWeight() == true )
					dispatchEvent( &lastStatusMsg_ );

				// If we had a last status with a positive stable weight, lets request
				// a new status so we don't miss the last stable weight.
				if ( lastStatusMsg_.isPositiveNonZeroStableWeight() ) {
					WeigherRequestStatus wrs( this );

					const char *pCC = wrs.serialize();
					pTranslator_->send( pCC, (unsigned int)wrs.getSerializedLength() );
				}

				weigherLocked_ = false;
			}
		}
		break;
		// This message is sent by the Operating Modes to disable the distribution
		// of weigher status messages through the system. (The WeigherManager will
		// keep track of the last status message even if it does not distribute it.)
		case WgLock: {
			Debug::trace("CANWeigherManager.WgLock", HobString("WgLock"));
			weigherLocked_ = true;
		}
		break;
		case WgReqStatus: {
			Debug::trace("CANWeigherManager.Message", HobString("WgReqStatus"));

			WeigherRequestStatus wrs( recvBuffer );
			wrs.setSourceObject( this );

			const char *pCC = wrs.serialize();
			pTranslator_->send(pCC, (unsigned int)wrs.getSerializedLength());

			lastStatusMsg_ = WeigherStatus();
		}
		break;
		case WgReqInquire: {
			Debug::trace("CANWeigherManager.Message", HobString("WgReqInquire"));
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
		case WgReqWakeup: {
			Debug::trace("CANWeigherManager.Message", HobString("WgReqWakeup"));

			WeigherRequestWakeup wrw( recvBuffer );
			wrw.setSourceObject( this );

			const char *pCC = wrw.serialize();
			pTranslator_->send( pCC, (unsigned int)wrw.getSerializedLength() );
		}
		break;
		case WgReqValueMax: {
			Debug::trace("CANWeigherManager.Message", HobString("WgReqValueMax"));

			WeigherSystemRequestValueMax wrw(recvBuffer);
			wrw.setSourceObject(this);

			const char *pCC = wrw.serialize();
			pTranslator_->send(pCC, (unsigned int)wrw.getSerializedLength());
		}
		break;
		case WgValueMaxFitParams: {
			Debug::trace("CANWeigherManager.Message", HobString("WgValueMaxFitParams"));

			WeigherValueMaxFitParams fp(recvBuffer);
			fp.setSourceObject(this);

			dispatchEvent( &fp );
		}
		break;
		case WgReqKind: {
			sendWeigherKind();
		}
		break;
		case WgCat3ReqStatistics: {
			Debug::trace("CANWeigherManager.cat3", HobString("WgCat3ReqStatistics"));

			WeigherGeneric reqStats( WgCat3ReqStatistics, this );
			const char *pCC = reqStats.serialize();
			pTranslator_->send( pCC, (unsigned int)reqStats.getSerializedLength() );
		}
		break;
		case WgCat3ReqPage: {
			Debug::trace("CANWeigherManager.cat3", HobString("WgCat3ReqPage"));

			WeigherCat3PageRequest wc3pr( recvBuffer );
			const char *pCC = wc3pr.serialize();
			pTranslator_->send( pCC, (unsigned int)wc3pr.getSerializedLength() );
		}
		break;
		case WgCat3ReqNextRecords: {
			Debug::trace("CANWeigherManager.cat3", HobString("WgCat3ReqNextRecords"));

			WeigherGeneric reqNext( WgCat3ReqNextRecords, this );
			const char *pCC = reqNext.serialize();
			pTranslator_->send( pCC, (unsigned int)reqNext.getSerializedLength() );
		}
		break;
		case WgCat3RecordStatus: {
			Debug::trace("CANWeigherManager.cat3", "WeigherCat3RecordStatus");

			WeigherCat3RecordStatus status( recvBuffer );
			status.setSourceObject( this );
			dispatchEvent( &status );
			Debug::trace("CANWeigherManager.cat3", std::string("WeigherCat3RecordStatus: Was record saved: ") + Bool::toString(status.getStatus()) );
		}
		break;
		case WgCat3AuditStatistics: {
			Debug::trace("CANWeigherManager.cat3", "WgCat3AuditStatistics");

			WeigherCat3Statistics stats( recvBuffer );
			stats.setSourceObject( this );
			//stats.show();
			dispatchEvent( &stats );
		}
		break;
		case WgCat3Records: {
			Debug::trace("CANWeigherManager.cat3", "WgCat3Records");

			WeigherCat3AuditRecords recs( recvBuffer );
			recs.setSourceObject( this );
			//recs.show();
			dispatchEvent( &recs );
		}
		break;
		case WgCat3WriteRecord: {
			Debug::trace("CANWeigherManager.cat3", "WgCat3WriteRecord");

			WeigherWriteCat3Record rec( recvBuffer );
			const char *pCC = rec.serialize();
			pTranslator_->send( pCC, (unsigned int)rec.getSerializedLength() );
		}
		break;
		case WgCat3ReqEraseLog: {
			Debug::trace("CANWeigherManager.cat3", "WgCat3ReqEraseLog");

			WeigherGeneric cat3AuditClear( WgCat3ReqEraseLog, this );
			const char *pCC = cat3AuditClear.serialize();
			pTranslator_->send( pCC, (unsigned int)cat3AuditClear.getSerializedLength() );
		}
		break;
		case WgCat3LogEraseComplete: {
			Debug::trace("CANWeigherManager.cat3", "WgCat3LogEraseComplete");

			WeigherGeneric cat3AuditCleared( WgCat3LogEraseComplete, this );
			dispatchEvent( &cat3AuditCleared );
		}
		break;
		case WgCat3Error: {
			Debug::trace("CANWeigherManager.cat3", "WgCat3Error");
		}
		break;
		case WgResetFactoryDefaults: {
			Debug::trace("CANWeigherManager.Message", "WgResetFactoryDefaults");

			// SOF-1760
			// For Category III, have to send the current date/time to the Weigher Manager before resetting to factory defaults.
			sendDateTime();

			WeigherGeneric msg( recvBuffer );
			msg.setSourceObject( this );
			const char *pCC = msg.serialize();
			pTranslator_->send( pCC, (unsigned int)msg.getSerializedLength() );
		}
		break;
		case WgResetFactoryDefaultsComplete: {
			Debug::trace("CANWeigherManager.Message", "WgResetFactoryDefaultsComplete");

			WeigherFactoryDefaultsComplete cmpltMsg( recvBuffer );
			if( !cmpltMsg.getSavedStatus() ) {
				Debug::trace("CANWeigherManager.Message", "Weigher factory defaults corruption!" + Integer::toString(cmpltMsg.getSavedStatus()));
			} else {
				Debug::trace("CANWeigherManager.Message", "Weigher factory defaults saved: " + Integer::toString(cmpltMsg.getSavedStatus()));
			}
			cmpltMsg.setSourceObject( this );
			dispatchEvent( &cmpltMsg );
			clearResponseCntr();

			#ifdef WG_DEFAULTS_TEST
			testMsgCount_++;
			testCounter_ = 0;
			#endif
		}
		break;
		case WgValueMaxAssembly: {
			WeigherValueMaxAssembly assembly( recvBuffer );
			Debug::trace("CANWeigherManager.valueMax", "assembly number: " + assembly.getAssemblyNumber() );
		}
		break;
		case WgValueMaxHwVersion: {
			WeigherValueMaxHwVersion hxVersion( recvBuffer );
			Debug::trace("CANWeigherManager.valueMax", "hardware version: " + hxVersion.getHardwareVersion() );
		}
		break;
		case WgValueMaxSerialNumber: {
			WeigherValueMaxSerialNumber serial( recvBuffer );
			Debug::trace("CANWeigherManager.valueMax", "serial number: " + serial.getSerialNumber() );
		}
		break;
		case WgValueMaxDateOfManufacture: {
			WeigherValueMaxDateOfMan date( recvBuffer );
			Debug::trace("CANWeigherManager.valueMax", "epoch date: " + Integer::toString( date.getDate() ) );
		}
		break;
		case WgValueMaxXGain: {
			WeigherValueMaxXGainFactor x( recvBuffer );
			char value[16];
			sprintf( value, "%f", x.getGain() );
			Debug::trace("CANWeigherManager.valueMax", "X gain: " + std::string(value) );
		}
		break;
		case WgValueMaxYGain: {
			WeigherValueMaxYGainFactor y( recvBuffer );
			char value[16];
			sprintf( value, "%f", y.getGain() );
			Debug::trace("CANWeigherManager.valueMax", "Y gain: " + std::string(value) );
		}
		break;
		case WgValueMaxZGain: {
			WeigherValueMaxZGainFactor z( recvBuffer );
			char value[16];
			sprintf( value, "%f", z.getGain() );
			Debug::trace("CANWeigherManager.valueMax", "Z gain: " + std::string(value) );
		}
		break;
		case WgValueMaxAllGains: {
			WeigherValueMaxAllGains all( recvBuffer );
			char value[16];
			sprintf( value, "%f", all.getXGain() );
			Debug::trace("CANWeigherManager.valueMax", "All gains X gain: " + std::string(value) );
			sprintf( value, "%f", all.getYGain() );
			Debug::trace("CANWeigherManager.valueMax", "All gains Y gain: " + std::string(value) );
			sprintf( value, "%f", all.getZGain() );
			Debug::trace("CANWeigherManager.valueMax", "All gains Z gain: " + std::string(value) );
		}
		break;
		case WgSystemInfo: {
			Debug::trace("CANWeigherManager.Message", "WgSystemInfo" );

			WeigherSystemInfo info( recvBuffer );

			info.show();

			// convert new format into old format
			UsbWeigherSystemStatus oldInfo( this, info.getWeigherPosition(), WgCanKpc );
			oldInfo.setHardwareVersion( info.getHardwareVersion() );
			oldInfo.setSoftwareWeigherVersion( info.getSoftwareVersion() );
			oldInfo.setSerialNumber( 0 ); //TO DO

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

			Debug::trace("CANWeigherManager.Error", str);

			dispatchEvent( &error );
		}
		break;
		default: {
			Debug::trace("CANWeigherManager.Error", HobString("Some Other Event: ") + HobString(Integer::toString((UINT16) wgEvent)));
		}
		break;
	}
}

void CANWeigherManager::requestWeigherWakeup()
{
	Debug::trace("CANWeigherManager.WeigherProbe", HobString("requesting weigher wakeup"));

	WeigherRequestWakeup wrw( this, 0 );

	const char *pCC = wrw.serialize();
	pTranslator_->send(pCC, (unsigned int)wrw.getSerializedLength());
}

void CANWeigherManager::requestWeigherStatus()
{
	WeigherRequestStatus wrs(this);

	const char *pCC = wrs.serialize();
	pTranslator_->send(pCC, (unsigned int)wrs.getSerializedLength());
}


void CANWeigherManager::requestWeigherInquire()
{
	WeigherRequestInquire wri(this);

	const char *pCC = wri.serialize();
	pTranslator_->send(pCC, (unsigned int)wri.getSerializedLength());
}

void CANWeigherManager::sendWeigherMode(WgOperatingMode opMode)
{
	WeigherMode wm( this, opMode);

	const char *pCC = wm.serialize();
	pTranslator_->send(pCC,  (unsigned int)wm.getSerializedLength());
}

void CANWeigherManager::sendWeigherKind()
{
	WeigherWeigherKind wwk( this, wgKind_ );
	dispatchEvent(&wwk);
}

void CANWeigherManager::sendDateTime()
{
	// For Category III, have to send the current date/time to the Weigher Manager before permanently setting any configuration or calibration values.
	time_t now;
	time( &now );
	WeigherCat3DateTime wCat3DateTime( this, (UINT32) now );
	const char *pCC = wCat3DateTime.serialize();
	pTranslator_->send(pCC, (unsigned int)wCat3DateTime.getSerializedLength());

	struct tm * timeinfo = localtime (&now);
	char buffer [80];
	strftime (buffer,80,"%c",timeinfo);
	Debug::trace("WeigherManager.cat3", std::string("sent date time to FPC: ulong: ") + ULong::toString(now) + std::string("  string: ") + std::string(buffer));
}



