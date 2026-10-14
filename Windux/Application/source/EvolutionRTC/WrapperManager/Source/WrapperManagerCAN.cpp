/**
 * @file WrapperManagerCAN.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief Manager for AWS3 and NGW Wrappers
 * Created on: Sept 5, 2017
 * @addtogroup CAN
 * @{
 */
#include "WrapperManagerCAN.h"
#include "WrapperGeneric.h"
#include "WrapperWrapperKind.h"
#include "WrapperConfiguration.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "WrapperSensorsTracking.h"
#include "EvolWrapperCamerasTracking.h"
#include "EvolWrapperErrorInfo.h"
#include "EvolWrapperPackageTimingsPointer.h"
#include "EvolWrapperPackageDesc.h"
#include "WrapBoxSize.h"
#include "Configuration.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperManagerCAN::WrapperManagerCAN( Hobart::CEvolMsgReceiver *pMsgHandler, WrapperKind wrKind )
{
	pMsgHandler_ = pMsgHandler;
	kind_ = wrKind;

	//set our mode to operate.
	mode_ = OpModeOperate;

	//initialize our status.
	EvolWrapperStatus status;
	lastStatus_ = status;

	registerMyEvents();

	//determine our poll timer rate.
	Configuration *pConfig_ = Configuration::getInstance();
	if( pConfig_ != NULL )
		pingTime_ = ticksPerSec_ * pConfig_->getWrapperProbeTime();

	//clear our retry counter.
	pingRetry_ = 0;

	pTrayNames_ = NULL;

	camera_ = EvolWrapperTest::WrEvolCameraTestParam_t::RightCameraScanAnalog;

	//create free running ping timer
	createTimer( pingTime_, PingTimer, CHobartTimer::PERIODIC_TIMER );

	//notify the system a wrapper is attached
	sendKind();

	requestWrapperCfg();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperManagerCAN::registerMyEvents()
{
	if( ( kind_ == WrAWS3Wrapper ) || ( kind_ == WrNGW1Wrapper ) ) {

		registerEvent( Wrapper, WrReqWakeup );
		registerEvent( Wrapper, WrEvolWakeup );
		registerEvent( Wrapper, WrEvolPackageDesc );
		registerEvent( Wrapper, WrOpen );
		registerEvent( Wrapper, WrReqInquire );
		registerEvent( Wrapper, WrReqStatus );
		registerEvent( Wrapper, WrEvolStatus );
		registerEvent( Wrapper, WrReqSensors );
		registerEvent( Wrapper, WrEvolSensors );
		registerEvent( Wrapper, WrEvolConfig );
		registerEvent( Wrapper, WrEvolLoad );
		registerEvent( Wrapper, WrEvolWrap );
		registerEvent( Wrapper, WrEvolEventCapture );
		registerEvent( Wrapper, WrEvolPackageDischarge );

		registerEvent( Wrapper, WrReqEvolSystemInfo );
		registerEvent( Wrapper, WrEvolSystemInfoErase );
		registerEvent( Wrapper, WrEvolSystemInfoWrite );
		registerEvent( WrapperSystem, 21 ); //SysUsbWrStatus

		registerEvent( Wrapper, WrCameraScan );
		registerEvent( Wrapper, WrCameraCalibration );
		registerEvent( Wrapper, WrEvolCamerasTracking );
		registerEvent( Wrapper, WrEvolCameraHeightScanTracking );
		registerEvent( Wrapper, WrEvolRequestCameraThreshold );
		registerEvent( Wrapper, WrEvolCameraThreshold );
		registerEvent( Wrapper, WrEvolWriteCameraThreshold );

		registerEvent( Wrapper, WrEvolTest );
		registerEvent( Wrapper, WrEvolTestComplete );
		registerEvent( Wrapper, WrEvolErrorHistory );
		registerEvent( Wrapper, WrEvolDiagnosticsTest );
		registerEvent( Wrapper, WrEvolStartMfgRunin );
		registerEvent( Wrapper, WrEvolManufacturingRunInNotice );
		registerEvent( Wrapper, WrEvolDiagnosticsTestResult );
		registerEvent( Wrapper, WrWrapBoxSize );
		registerEvent( Wrapper, WrWrapBoxTransferComplete );
		registerEvent( Wrapper, WrEvolMotorProfile );
		registerEvent( Wrapper, WrReqErrorHistory );
		registerEvent( Wrapper, WrSensorsTracking );
		registerEvent( Wrapper, WrStatusTracking );
		registerEvent( Wrapper, WrErrorHistoryTracking );

	}

}

void WrapperManagerCAN::timeoutProcess( __int32 id )
{
	switch( id )
	{
		case PingTimer: {
			if( pingRetry_ == 0 ) {
				pingRetry_++;
			} else if( pingRetry_ == 1 ) {
				pingRetry_++;
				requestPing();
			} else if( pingRetry_ == 2 ) {
				pingRetry_++;
				requestPing();
			} else {

				pingRetry_ = 0;

				//announce that the wrapper has disconnected
				EvolWrapperErrorInfo error = lastStatus_.getCurrErrorInfo();
				error.setGeneralWrapper( WRAPPER_DISCONNTECTED );
				lastStatus_.setCurrErrorInfo( error );
				lastStatus_.clearPeripheralComm();
				dispatchEvent( &lastStatus_ );

				//inform the RTC that we have detached
				WrapperGeneric msg( WrNGWWrapperDetached, this );
				dispatchEvent( &msg );

				Debug::trace( "WrapperManager.Error", "Wrapper has not responded to ping!" );
			}
			break;
		}
		case SensorPollTimer: {
			requestSensors();
			break;
		}
		case CameraPollTimer: {
			if( camera_ ==  EvolWrapperTest::WrEvolCameraTestParam_t::RightCameraScanAnalog )
				camera_ =  EvolWrapperTest::WrEvolCameraTestParam_t::LeftCameraScanAnalog;
			else
				camera_ =  EvolWrapperTest::WrEvolCameraTestParam_t::RightCameraScanAnalog;

			EvolWrapperTest test( this, EvolWrapperTest::CameraTest );
			test.setDataInt1( camera_ );
			dispatchEvent(&test);
			break;
		}
		case IntakeHeightPollTimer: {
			EvolWrapperTest test( this, EvolWrapperTest::CameraTest );
			test.setDataInt1( EvolWrapperTest::CameraHeight );
			dispatchEvent(&test);
			break;
		}
		case UnitTestTimer: {
			break;
		}
		default: {
			break;
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperManagerCAN::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	switch (msg->getDeviceType())
	{
		case Wrapper: {
			handleWrapperEvent((WrEvent) msg->getEventType(), msg->Data());
			break;
		}
		default: {
			Debug::trace( "WrapperManager.Error", "Unknown message, deviceType = " + Integer::toString( msg->getDeviceType() ) +
					", eventType = " + Integer::toString( msg->getEventType() ) );
			break;

		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperManagerCAN::handleWrapperEvent(WrEvent wrEvent, const char *recvBuffer)
{
	switch (wrEvent)
	{
		case WrEvolWakeup: {
			Debug::trace("WrapperManager.Message", "WrEvolWakeup");
			clearPingCount();
			break;
		}
		case WrInquire:
		case WrEvolInquire: {
			Debug::trace("WrapperManager.Message", "WrEvolInquire");
			break;
		}
		case WrEvolStatus: {
			Debug::trace("WrapperManager.Message", "WrEvolStatus");

			EvolWrapperStatus msg(recvBuffer);
			//save the status if new
			if( !msg.equals( lastStatus_ ) ) {
				lastStatus_ = msg;
				lastStatus_.show();
			}
			break;
		}
		case WrEvolConfig:
		case WrConfig: {
			Debug::trace("WrapperManager.Message", "WrConfig");

			WrapperConfiguration config(recvBuffer);
			config.show();
			break;
		}
		case WrMode: {
			Debug::trace("WrapperManager.Message", "WrMode");
			break;
		}
		case WrOpen: {
			Debug::trace("WrapperManager.Message", "WrOpen");
			break;
		}
		case WrReqStatus: {
			Debug::trace("WrapperManager.Message", "WrReqStatus");
			break;
		}
		case WrEvolLoad: {
			Debug::trace("WrapperManager.Message", "WrEvolLoad");
			break;
		}
		case WrEvolWrap: {
			Debug::trace("WrapperManager.Message", "WrEvolWrap");
			break;
		}
		case WrEvolPackageDischarge:{
			Debug::trace("WrapperManager.Message", "WrEvolPackageDischarge");
			break;
		}
		case WrEvolSettings: {
			Debug::trace("WrapperManager.Message", "WrEvolSettings");
			break;
		}
		case WrEvolTest: {
			Debug::trace("WrapperManager.Message", "WrEvolTest");
			break;
		}
		case WrEvolTestComplete: {
			Debug::trace("WrapperManager.Message", "WrEvolTestComplete");
			break;
		}
		case WrEvolCycle: {
			Debug::trace("WrapperManager.Message", "WrEvolCycle");
			break;
		}
		case WrEvolSensors: {
			Debug::trace("WrapperManager.Message", "WrEvolSensors");
			break;
		}
		case WrSensorsTracking: {
			Debug::trace("WrapperManager.Message", "WrSensorsTracking");

			WrapperSensorsTracking track( recvBuffer );
			if( track.getSensorsTracking() ) {
				requestSensors();
				createTimer( sensorPollTime_, SensorPollTimer, CHobartTimer::PERIODIC_TIMER );
			} else {
				cancelTimer( SensorPollTimer );
			}
			break;
		}
		case WrEvolPackageDesc: {
			Debug::trace("WrapperManager.Message", "WrEvolPackageDesc");

			EvolWrapperPackageDesc desc(recvBuffer);
			//get the tray name
			string name;
			if( pTrayNames_ )
				pTrayNames_->getTrayName( desc.getWrapBoxIndex(), name );
			desc.setTrayName( name );

			//add the package to our list
			pkgHistory_.addPackage( desc.getId(), desc.getWidth(), desc.getLength(), desc.getHeight(), desc.getLip(),
					                desc.getFilmWidthSelection(), desc.getTrayName(), desc.hasUsedWrapBoxBestFit(),
					                desc.hasModifiedNetStretch() );
			break;
		}
		case WrEvolRequestPackageDesc: {
			Debug::trace("WrapperManager.Message", "WrEvolRequestPackageDesc");
			break;
		}
		case WrLabelReady: {
			Debug::trace("WrapperManager.Message", "WrLabelReady");
			break;
		}
		case WrLabelSetup: {
			Debug::trace("WrapperManager.Message", "WrLabelSetup");
			break;
		}
		case WrLabelTaken: {
			Debug::trace("WrapperManager.Message", "WrLabelTaken");
			break;
		}
		case WrEvolControl: {
			Debug::trace("WrapperManager.Message", "WrEvolControl");
			break;
		}
		case WrCameraScan: {
			Debug::trace("WrapperManager.Message", "WrCameraScan");
			break;
		}
		case WrCameraHeightScan: {
			Debug::trace("WrapperManager.Message", "WrCameraHeightScan");
			break;
		}
		case WrEvolCameraHeightScanTracking: {
			Debug::trace("WrapperManager.Message", "WrEvolCameraHeightScanTracking");

			EvolWrapperCamerasTracking track( recvBuffer );
			if( track.getCamerasTracking() ) {
				createTimer( cameraPollTime_, IntakeHeightPollTimer, CHobartTimer::PERIODIC_TIMER );
			} else {
				cancelTimer( IntakeHeightPollTimer );
			}
			break;
		}
		case WrCameraCalibration: {
			Debug::trace("WrapperManager.Message", "WrCameraCalibration");
			break;
		}
		case WrEvolCamerasTracking: {
			Debug::trace("WrapperManager.Message", "WrEvolCamerasTracking");

			EvolWrapperCamerasTracking track( recvBuffer );
			if( track.getCamerasTracking() ) {
				createTimer( cameraPollTime_, CameraPollTimer, CHobartTimer::PERIODIC_TIMER );
			} else {
				cancelTimer( CameraPollTimer );
			}
			break;
		}
		case WrEvolRequestCameraThreshold: {
			Debug::trace("WrapperManager.Message", "WrEvolRequestCameraThreshold");
			break;
		}
		case WrEvolCameraThreshold: {
			Debug::trace("WrapperManager.Message", "WrEvolCameraThreshold");
			break;
		}
		case WrEvolWriteCameraThreshold: {
			Debug::trace("WrapperManager.Message", "WrEvolWriteCameraThreshold");
			break;
		}
		case WrReqEvolSystemInfo: {
			Debug::trace("WrapperManager.Message", "WrReqEvolSystemInfo");
			break;
		}
		case WrEvolSystemInfo: {
			Debug::trace("WrapperManager.Message", "WrEvolSystemInfo");
			break;
		}
		case WrEvolSystemInfoErase: {
			Debug::trace("WrapperManager.Message", "WrEvolSystemInfoErase");
			break;
		}
		case WrEvolSystemInfoWrite: {
			Debug::trace("WrapperManager.Message", "WrEvolSystemInfoWrite");
			break;
		}
		case WrRequestWrapBoxAll: {
			Debug::trace("WrapperManager.Message", "WrRequestWrapBoxAll");
			break;
		}
		case WrRequestWrapBox: {
			Debug::trace("WrapperManager.Message", "WrRequestWrapBox");
			break;
		}
		case WrWrapBox: {
			Debug::trace("WrapperManager.Message", "WrWrapBox");
			break;
		}
		case WrWrapBoxSize: {
			Debug::trace("WrapperManager.Message", "WrWrapBoxSize");
			break;
		}
		case WrEvolPackageTimings: {
			Debug::trace("WrapperManager.Message", "WrEvolPackageTimings");
			EvolWrapperPackageTimingsPointer timingsPtr(recvBuffer);
			EvolWrapperPackageTimings *pTimings = timingsPtr.getTimingsPointer();

			if( pTrayNames_ )
				delete pTrayNames_;
			//create a new list of tray names
			if( pTimings )
				pTrayNames_ = new EvolWrapperTrayNames( pTimings );
			break;
		}
		case WrEvolSavePackageTimings: {
			Debug::trace("WrapperManager.Message", "WrEvolSavePackageTimings");

			EvolWrapperPackageTimingsPointer timingsPtr(recvBuffer);
			EvolWrapperPackageTimings *pTimings = timingsPtr.getTimingsPointer();

			if( pTrayNames_ )
				delete pTrayNames_;

			if( pTimings )
				pTrayNames_ = new EvolWrapperTrayNames( pTimings );

			//save our timing until we have an ack from the wrapper.
			pMyTimings_ = pTimings;

			//determine the overall size of the wrap box information.
			if( pTimings != NULL ) {

				int numWrapBoxes = pTimings->getNumberWrapBoxes();
				int numHeightBoxes = 0;
				int totalSize = 0;

				for ( int i = 0; i < numWrapBoxes; ++i )
				{
					numHeightBoxes += pTimings->getWrapBox(i)->getNumberHeightBoxes();
				}
				totalSize = WrapBox::_WrapBoxOffset_ + ( numWrapBoxes * WrapBox::_WrapBoxSizeCan_) + ( numHeightBoxes * WrapBox::_HeightBoxSizeCan_ );

				//inform the wrapper of the total size of the wrap box information we are about to send.
				WrapBoxSize msg( this, totalSize );
				dispatchEvent(&msg);

			} else {
				Debug::trace("WrapperManager", "WrEvolSavePackageTimings:  Unable to send msg. EvolWrapperPackageTimings is NULL!");
			}
			break;
		}
		case WrWrapBoxAck: {
			Debug::trace("WrapperManager.Message", "WrWrapBoxAck");
			if( pMyTimings_ ) {
    			EvolWrapperPackageTimingsPointer timings( WrEvolPackageTimingsWrite, pMyTimings_ );
    			dispatchEvent( &timings );
			}
			break;
		}
		case WrEvolRequestPackageTimings: {
			Debug::trace("WrapperManager.Message", "WrEvolRequestPackageTimings");
			break;
		}
		case WrWrapBoxComplete: {
			Debug::trace("WrapperManager.Message", "WrWrapBoxComplete");
			break;
		}
		case WrRequestHeightBox: {
			Debug::trace("WrapperManager.Message", "WrRequestHeightBox");
			break;
		}
		case WrWrapBoxTransferComplete: {
			Debug::trace("WrapperManager.Message", "WrWrapBoxTransferComplete");
			pMyTimings_ = NULL;
			break;
		}
		case WrEvolResetDefaultPackageTimings: {
			Debug::trace("WrapperManager.Message", "WrEvolResetDefaultPackageTimings");
			break;
		}
		case WrEvolSavePackageTimingsComplete: {
			Debug::trace("WrapperManager.Message", "WrEvolSavePackageTimingsComplete");
			break;
		}
		case WrReqPackageHistory: {
			Debug::trace("WrapperManager.Message", "WrReqPackageHistory");

			pkgHistory_.setSourceObject(this);
			dispatchEvent( &pkgHistory_ );
			break;
		}
		case WrEvolDiagnosticsTest: {
			Debug::trace("WrapperManager.Message", "WrEvolDiagnosticsTest");
			break;
		}
		case WrEvolDiagnosticsTestResult: {
			Debug::trace("WrapperManager.Message", "WrEvolDiagnosticsTestResult");
			break;
		}
		case WrEvolErrorHistory: {
			Debug::trace("WrapperManager.Message", "WrEvolErrorHistory");
			break;
		}
		case WrEvolStartMfgRunin: {
			Debug::trace("WrapperManager.Message", "WrEvolStartMfgRunin");
			break;
		}
		case WrEvolManufacturingRunInNotice: {
			Debug::trace("WrapperManager.Message", "WrEvolManufacturingRunInNotice");
			break;
		}
		case WrEvolMotorProfile: {
			Debug::trace("WrapperManager.Message", "WrEvolMotorProfile");
			break;
		}
		default: {
			Debug::trace("WrapperManager.Error", "Unsupported event received: " + Integer::toString( wrEvent ) );
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperManagerCAN::sendKind()
{
	WrapperWrapperKind kind( this, kind_ );
	dispatchEvent( &kind );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperManagerCAN::requestPing()
{
	WrapperGeneric msg( WrReqWakeup, this );
	dispatchEvent( &msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperManagerCAN::requestSensors()
{
	WrapperGeneric msg( WrReqSensors, this );
	dispatchEvent( &msg );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperManagerCAN::requestWrapperCfg ()
{
	WrapperGeneric msg( WrReqInquire, this );
	dispatchEvent( &msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperManagerCAN::requestWrapperWrapBoxTimings()
{
	EvolWrapperRequestPackageTimings timings( EvolWrapperRequestPackageTimings::CurrentTimings, this );
	dispatchEvent( &timings );
}

