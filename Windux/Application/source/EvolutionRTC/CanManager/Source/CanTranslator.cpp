/**
 * @file CanTranslator.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Responsible for translating the CAN message into device messages that the System controller can use
 * Created on: Jun 24, 2015
 */
#include "CanTranslator.h"
#include "CanManager.h"
#include "FpmManager.h"
#include "WeigherManager.h"
#include "WeigherWakeup.h"
#include "WeigherMode.h"
#include "WeigherInquire.h"
#include "WeigherStatus.h"
#include "WeigherConfiguration.h"
#include "WeigherVersion.h"
#include "WeigherSerialNumber.h"
#include "WeigherSystemInfo.h"
#include "WeigherCat3DateTime.h"
#include "WeigherCat3PageRequest.h"
#include "WeigherWriteCat3Record.h"
#include "WeigherCat3RecordStatus.h"
#include "WeigherCat3AuditRecords.h"
#include "WeigherCat3Statistics.h"
#include "WeigherFactoryDefaultsComplete.h"
#include "WeigherReqValueMax.h"
#include "WeigherValueMaxFitParams.h"
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
#include "WeigherKpcError.h"

#include "PrinterManager.h"
#include "Printer.h"
#include "PrinterGeneric.h"
#include "PrinterMode.h"
#include "PrinterConfiguration.h"
#include "PrinterCommand.h"
#include "PrinterRequestPrintheadPower.h"
#include "PrinterRequestLabelTransfer.h"
#include "PrinterRequestCalibrate.h"
#include "PrinterRam.h"
#include "PrinterTeach.h"
#include "CommandOperations.h"
#include "CommandOperationList.h"
#include "PrinterTest.h"
#include "PrinterMask.h"
#include "PrinterNomSize.h"
#include "PrinterWakeup.h"
#include "PrinterInquire.h"
#include "PrinterStatus.h"
#include "PrinterSensors.h"
#include "PrinterLabelTransferReady.h"
#include "PrinterLabelTransferStatus.h"
#include "PrinterLabelTransferComplete.h"
#include "PrinterVersion.h"
#include "PrinterCutterStatus.h"
#include "PrinterHeadWear.h"
#include "PrinterFactoryDefaultsComplete.h"
#include "PrinterRS485FlowControl.h"
#include "PrinterNewPrinterType.h"
#include "PrinterKpcError.h"
#include "PrinterLabelImageSize.h"
#include "KpcSystemEvents.h"
#include "KpcSystemErrors.h"
#include "KpcBootEvent.h"
#include "KPCBootAck.h"
#include "KPCBootAppVersion.h"
#include "KPCBootBulkMsg.h"
#include "KPCBootCmd.h"
#include "KPCBootGeneric.h"
#include "KPCBootReady.h"
#include "KPCBootStatus.h"
#include "KPCBootVersion.h"
#include "KPCBootFrame.h"
#include "KPCBootFrameHeader.h"
#include "PrinterRequestHeadDotStatus.h"
#include "KpcBootUpgraderEvent.h"
#include "KPCBootUpgraderAck.h"
#include "KPCBootUpgraderBulkMsg.h"
#include "KPCBootUpgraderCmd.h"
#include "KPCBootUpgraderGeneric.h"
#include "KPCBootUpgraderStatus.h"
#include "KPCBootUpgraderFrame.h"
#include "KPCBootUpgraderFrameHeader.h"
#include "PrinterRequestHeadDotWear.h"
#include "PrinterHeadDotStatus.h"
#include "PrinterHeadDotAck.h"
#include "PrinterHeadType.h"
#include "PrinterGapCalStatus.h"

#include "RealTimeController.h"
#include "WrapperManager.h"
#include "WrapperEvent.h"
#include "EvolWrapperWakeup.h"
#include "EvolWrapperConfiguration.h"
#include "EvolWrapperControl.h"
#include "WrapperMode.h"
#include "WrapperOpen.h"
#include "EvolWrapperInquire.h"
#include "EvolWrapperWrap.h"
#include "EvolWrapperLoad.h"
#include "EvolWrapperCycle.h"
#include "EvolWrapperCycleCount.h"
#include "EvolWrapperTest.h"
#include "EvolWrapperSaveCycleCount.h"
#include "EvolWrapperSystemInfoWrite.h"
#include "EvolWrapperTest.h"
#include "EvolWrapperTestComplete.h"
#include "EvolWrapperEventCapture.h"
#include "EvolWrapperErrorHistory.h"
#include "EvolWrapperPackageDesc.h"
#include "EvolWrapperPackageDischarge.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "EvolWrapperSystemInfo.h"
#include "EvolWrapperCameraHeightScan.h"
#include "EvolWrapperCameraCalibration.h"
#include "EvolWrapperCameraScan.h"
#include "EvolWrapperDiagnosticsTest.h"
#include "EvolWrapperCameraThreshold.h"
#include "EvolWrapperWriteCameraThreshold.h"
#include "EvolWrapperSystemStatus.h"
#include "EvolWrapperNode.h"
#include "EvolWrapperMotorProfile.h"
#include "EvolWrapperDiagnosticsTestResult.h"
#include "EvolWrapperPackageTimingsPointer.h"
#include "EvolWrapperPackageTimings.h"

#include "WrapBox.h"
#include "WrapBoxSize.h"
#include "NGWLabelReady.h"
#include "NGWLabelTaken.h"
#include "NGWLabelSetup.h"
#include "NGWCycleCount.h"
#include "NGWWakeup.h"
#include "NGWVersion.h"
#include "NGWError.h"
#include "NGWReqBootStatus.h"
#include "NGWRequestBootVersion.h"
#include "NGWBootFrame.h"
#include "NGWBootNodeCMD.h"
#include "NGWBootStatus.h"
#include "NGWBootVersion.h"
#include "NGWAck.h"
#include "NGWFlush.h"
#include "NGWBulkTransfer.h"
#include "NGWEngInfo.h"
#include "NGWEngException.h"
#include "WrapperOpen.h"
#include "LabelApplierSetup.h"
#include "LabelApplierLabelReady.h"
#include "LabelApplierLabelTaken.h"
#include "WrPPPWakeup.h"
#include "WrPPPDist.h"
#include "WrPPPConfig.h"
#include "WrPPPRequest.h"
#include "WrPPPCalibration.h"
#include "WrPPPVersion.h"
#include "WrPPPStatus.h"
#include "NGWWrapperPause.h"
#include "NGWEvent.h"
#include "NGWDimensionOverrides.h"


#include "KPCBootManager.h"
#include "KPCBootUpgrader.h"
#include "KPCBootReady.h"

#include "BoardSiteTestManager.h"
#include "BoardSiteTestStatus.h"
#include "BoardSiteTestComplete.h"
#include "BoardSiteTest.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Hex.h"

#include <CfgInternal.h>
#include <Configuration.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
CanTranslator::CanTranslator(): Hobart::CEvolMsgReceiver("CanTranslator") {

    Debug::trace( "CanTranslator", "CanTranslator()");

#if defined ( __linux__ )

    pPrinter1Manager = pPrinter2Manager = pPrinter3Manager = pWeigherManager = pWrapperManager = pLabelApplier = pKPCBootManager = pKPCBootUpgrader =NULL;

    //get my type of system
    pSerialFlash = FpmManager::getInstance();
    mySystemType = pSerialFlash->getMFGSystemStyle();

    pCanManager = NULL;
    NGWScanFinished_ = false;

	contiguousMessages.push_back( CanMsgRange( 0x596, 0x59C ) ); //Wrapper status message breaks convention
	contiguousMessages.push_back( CanMsgRange( 0x534, 0x536 ) ); //Wrapper package descriptor
	contiguousMessages.push_back( CanMsgRange( 0x5b0, 0x5B6 ) ); //Wrapper system info
	contiguousMessages.push_back( CanMsgRange( 0x588, 0x58d ) ); //Wrapper config
	contiguousMessages.push_back( CanMsgRange( 0x5bf, 0x5c7 ) ); //Wrapper camera scan
	contiguousMessages.push_back( CanMsgRange( 0x5ca, 0x5cc ) ); //Wrapper error history
	contiguousMessages.push_back( CanMsgRange( 0x5d0, 0x5db ) ); //Wrapper version
    
    if (FpmManager::getInstance()->isWrapperTwoPrinter())
        contiguousMessages.push_back(CanMsgRange(0x5e1, 0x5ec)); //Wrapper wrapbox - bumped up to 0x5ec for new vertical offsets
    else
        contiguousMessages.push_back(CanMsgRange(0x5e1, 0x5eb)); //Wrapper wrapbox - Original 1P range
	
    contiguousMessages.push_back( CanMsgRange( 0x71a, 0x71b ) ); //Wrapper engineering info

	contiguousMessages.push_back( CanMsgRange( 0x402, 0x405 ) ); //Weigher status
	contiguousMessages.push_back( CanMsgRange( 0x411, 0x41b ) ); //Weigher config
	contiguousMessages.push_back( CanMsgRange( 0x490, 0x4b8 ) ); //Weigher cat3 records
	contiguousMessages.push_back( CanMsgRange( 0x440, 0x444 ) ); //Weigher cat3 statistics
	contiguousMessages.push_back( CanMsgRange( 0x44f, 0x450 ) ); //Weigher cat3 record status
	contiguousMessages.push_back( CanMsgRange( 0x421, 0x422 ) ); //Weigher value max assembly number
	contiguousMessages.push_back( CanMsgRange( 0x427, 0x428 ) ); //Weigher value max serial number
	//contiguousMessages.push_back( CanMsgRange( 0x436, 0x437 ) ); //Weigher value max current gains
	contiguousMessages.push_back( CanMsgRange( 0x430, 0x43c ) ); //Weigher FIT Params

	contiguousMessages.push_back( CanMsgRange( 0x100, 0x101 ) ); //Printer wakeup
	contiguousMessages.push_back( CanMsgRange( 0x119, 0x11e ) ); //Printer config
	contiguousMessages.push_back( CanMsgRange( 0x107, 0x108 ) ); //Printer status
	contiguousMessages.push_back( CanMsgRange( 0x109, 0x10c ) ); //Printer sensors
	contiguousMessages.push_back( CanMsgRange( 0x10e, 0x10f ) ); //Printer version
	contiguousMessages.push_back( CanMsgRange( 0x175, 0x191 ) ); //Printer head dot wear

    contiguousMessages.push_back( CanMsgRange( 0x610, 0x611 ) ); //PPP wakeup
    contiguousMessages.push_back( CanMsgRange( 0x61b, 0x61e ) ); //PPP config
    contiguousMessages.push_back( CanMsgRange( 0x616, 0x617 ) ); //PPP calibration
    /* contiguousMessages.push_back( CanMsgRange( 0x1A0, 0x1A2 ) ); */ //Printer gap cal status     
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
CanTranslator::~CanTranslator() {

#if defined ( __linux__ )

	deregisterAllEvents();
	ShutdownNicely();

    Debug::trace("CanTranslator", "~CanTranslator(): --Entry");
    if( pPrinter1Manager )
        RealTimeController::getInstance()->CANDeviceDetached( RealTimeController::_FirstCANPrinter );
    if( pPrinter2Manager )
        RealTimeController::getInstance()->CANDeviceDetached( RealTimeController::_SecondCANPrinter );
    if( pPrinter3Manager )
        RealTimeController::getInstance()->CANDeviceDetached( RealTimeController::_ThirdCANPrinter );
    if( pWeigherManager )
        RealTimeController::getInstance()->CANDeviceDetached( RealTimeController::_CANWeigher );
    if( pWrapperManager )
        RealTimeController::getInstance()->CANDeviceDetached( RealTimeController::_CANWrapper );
    delete pCanLibrary;
    Debug::trace("CanTranslator", "~CanTranslator(): --Exit");

#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined ( __linux__ )           // added #defines to indiv sections in case we need to bring something back later
void CanTranslator::assignCanManager(CanManager *pManager)
{

    if ( pManager )
    {
    	pCanManager = pManager;

    	registerWeigherEvents();
        registerPrinterEvents();
        registerWrapperEvents();
        registerKPCBootEvents();
        registerKPCBootUpgraderEvents();

        if ( (pCanLibrary = new CanLibrary()) )
            Debug::trace( "CanTranslator", "CanTranslator(): Successfully created the Can Library");
        else
            Debug::trace( "CanTranslator.Error", "CanTranslator(): Failed to create Can Library!");
    }
    else
    {
        Debug::trace( "CanTranslator.Error", "CanTranslator(): Failed to get instance of the CanManager!");
    }

}
#endif // __linux__

#if defined ( __linux__ )           // added #defines to indiv sections in case we need to bring something back later

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    if ( !msg.get() )
    {
        Debug::trace("CanTranslator.Error", "processEvent(): message received is NULL! Ignoring");
        return;
    }

    switch (msg->getDeviceType())
    {
	case Weigher:
		Debug::trace("CanTranslator.Message", "processEvent(): Received Weigher msg, WgEvent: " + Integer::toString(msg->getEventType()));
		translateWeigherEvent( (WgEvent) msg->getEventType(), msg->Data() );
		break;
	case Printer:
	case PrinterSystem:
		Debug::trace("CanTranslator.Message", "processEvent(): Received Printer msg, PrEvent: " + Integer::toString(msg->getEventType()));
		translatePrinterEvent( (PrEvent) msg->getEventType(), msg->Data(), msg->Length());
		break;
	case Wrapper:
		Debug::trace("CanTranslator.Message", "processEvent(): Received Wrapper msg, WrEvent: " + Integer::toString(msg->getEventType()));
		translateWrapperEvent( (WrEvent) msg->getEventType(), msg->Data() );
		break;
	case LabelApplier:
		Debug::trace("CanTranslator.Message", "processEvent(): Received LabelApplier msg, WrEvent: " + Integer::toString(msg->getEventType()));
		translateLabelApplierEvent( (LaEvent) msg->getEventType(), msg->Data() );
		break;
    case KPCBootloader:
        Debug::trace("CanTranslator.Message", "processEvent(): Received KPC Boot msg, KpcBootEvent: " + Integer::toString(msg->getEventType()));
        translateKPCBootEvent((KPCBootEvent)msg->getEventType(), msg->Data());
        break;
    case KPCBootUpdater:
        Debug::trace("CanTranslator.Message", "processEvent(): Received KPC Boot Upgrader msg, KpcBootEvent: " + Integer::toString(msg->getEventType()));
        translateKPCBootUpgraderEvent((KPCBootUpgraderEvent)msg->getEventType(), msg->Data());
        break;
    case BoardSite:
        Debug::trace("CanTranslator.Message", "processEvent(): Received board site msg, BoardSiteEvent: " + Integer::toString(msg->getEventType()));
        translateBoardSiteEvent((BoardSiteEvent)msg->getEventType(), msg->Data());
        break;
    case NGWrapper:
        Debug::trace("CanTranslator.Message", "processEvent(): Received NGWrapper Device");
        if( msg->getEventType() == _WrStartKPC ) {
            Debug::trace("CanTranslator.Message", "processEvent(): Received _WrStartKPC Event");
            NGWScanFinished_ = true;
        } 
        break;

	default:
		Debug::trace("CanTranslator.Error", "processEvent(): Received message from unsupported device: " + HobString(Integer::toString(msg->getDeviceType())));
		break;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateWeigherEvent(WgEvent wgEvent, const char *recvBuffer) {

    Debug::trace( "CanTranslator", "translateWeigherEvent() WgEvent:" + Integer::toString(wgEvent) );

    can_frame canMsg;
    memset( &canMsg, 0, sizeof(can_frame) );

    //translate downstream (out going) weigher messages
    switch (wgEvent) {
        case WgRezero:
        case WgColdReset:
        case WgReset:
        case WgDisable:
        case WgEnable:
        case WgControl:
        case WgReqStatus:
        case WgReqInquire:
        case WgReqWakeup:
        case WgReqKind:
        case WgReqVersion:
        case WgReqSerialNum:
        case WgReqSystemInfo:
        case WgResetFactoryDefaults: {
            canMsg.can_id = pCanLibrary->getWeigherCobID(wgEvent);
            canMsg.can_dlc = 1;
            canMsg.data[0] = 0;

            Debug::trace( "CanTranslator.Translate", "translateWeigherEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id) );

            if(canMsg.can_id != 0) {
                pCanManager->send(&canMsg);
            } else {
                Debug::trace("CanTranslator.Error", "translateWeigherEvent(): - WgReqs/WgCommands: Failed to get COB id for next frame: " + HobString(Integer::toString(canMsg.can_id)));
            }
            break;
        }
        case WgReqValueMax: {
        	WeigherReqValueMax req(recvBuffer);
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgReqValueMax);
            canMsg.can_dlc = 1;
            canMsg.data[0] = req.getRequest();
            Debug::trace( "CanTranslator.Translate", "WgReqValueMax - translateWeigherEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id) );
            if(canMsg.can_id != 0) {
				pCanManager->send(&canMsg);
			} else {
				Debug::trace("CanTranslator.Error", "WgReqValueMax - translateWeigherEvent(): - WgMode: Failed to get COB id for next frame: " + HobString(Integer::toString(canMsg.can_id)));
			}
        	break;
        }
		case WgMode: {
            WeigherMode wm(recvBuffer);

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgMode);
            canMsg.can_dlc = 1;
            canMsg.data[0] = wm.getMode();
            if(canMsg.can_id != 0) {
                pCanManager->send(&canMsg);
            } else {
                Debug::trace("CanTranslator.Error", "translateWeigherEvent(): - WgMode: Failed to get COB id for next frame: " + HobString(Integer::toString(canMsg.can_id)));
            }
            break;
        }
        case WgConfig: {
            WeigherConfiguration wc(recvBuffer);
            int cobIdIndex = 0, nBytes = 0;

#if 0 	 	// implement Carlos's Weigher Configuration changes
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig);
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( wc.getConfigType() );
            canMsg.data[1] = getLsbFromShort( wc.getConfigType() );
            canMsg.data[2] = getMsbFromLong( (unsigned long)wc.getCenterOfMaintenanceZone() );
            canMsg.data[3] = getCharFromLong( (unsigned long)wc.getCenterOfMaintenanceZone(), 2 );
            canMsg.data[4] = getCharFromLong( (unsigned long)wc.getCenterOfMaintenanceZone(), 1 );
            canMsg.data[5] = getLsbFromLong( (unsigned long)wc.getCenterOfMaintenanceZone() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getRateOptionTime() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getRateOptionTime() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
#else
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig);
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( wc.getConfigType() );
            canMsg.data[1] = getLsbFromShort( wc.getConfigType() );
            canMsg.data[2] = getMsbFromLong( (unsigned long)wc.getCenterOfMaintenanceZone() );
            canMsg.data[3] = getCharFromLong( (unsigned long)wc.getCenterOfMaintenanceZone(), 2 );
            canMsg.data[4] = getCharFromLong( (unsigned long)wc.getCenterOfMaintenanceZone(), 1 );
            canMsg.data[5] = getLsbFromLong( (unsigned long)wc.getCenterOfMaintenanceZone() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getPrepackMotionCount() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getPrepackMotionCount() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

#endif
            cobIdIndex++;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 1;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromLong( (unsigned long)wc.getScaleFactor() );
            canMsg.data[1] = getCharFromLong( (unsigned long)wc.getScaleFactor(), 2 );
            canMsg.data[2] = getCharFromLong( (unsigned long)wc.getScaleFactor(), 1 );
            canMsg.data[3] = getLsbFromLong( (unsigned long)wc.getScaleFactor() );
            canMsg.data[4] = getMsbFromLong( (unsigned long)wc.getMaxWeight() );
            canMsg.data[5] = getCharFromLong( (unsigned long)wc.getMaxWeight(), 2 );
            canMsg.data[6] = getCharFromLong( (unsigned long)wc.getMaxWeight(), 1 );
            canMsg.data[7] = getLsbFromLong( (unsigned long)wc.getMaxWeight() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            cobIdIndex++;
#if 0 	 	// implement Carlos's Weigher Configuration changes
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 2;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromLong( (unsigned long)wc.getGainFactor() );
            canMsg.data[1] = getCharFromLong( (unsigned long)wc.getGainFactor(), 2 );
            canMsg.data[2] = getCharFromLong( (unsigned long)wc.getGainFactor(), 1 );
            canMsg.data[3] = getLsbFromLong( (unsigned long)wc.getGainFactor() );
            canMsg.data[4] = getMsbFromLong( (unsigned long)wc.getZeroReference() );
            canMsg.data[5] = getCharFromLong( (unsigned long)wc.getZeroReference(), 2 );
            canMsg.data[6] = getCharFromLong( (unsigned long)wc.getZeroReference(), 1 );
            canMsg.data[7] = getLsbFromLong( (unsigned long)wc.getZeroReference() );
#else
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 2;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromLong( (unsigned long)wc.getGainFactor() );
            canMsg.data[1] = getCharFromLong( (unsigned long)wc.getGainFactor(), 2 );
            canMsg.data[2] = getCharFromLong( (unsigned long)wc.getGainFactor(), 1 );
            canMsg.data[3] = getLsbFromLong( (unsigned long)wc.getGainFactor() );
            canMsg.data[4] = getMsbFromShort( (unsigned short)wc.getInitializeZeroTime() );
            canMsg.data[5] = getLsbFromShort( (unsigned short)wc.getInitializeZeroTime() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getSmallMotionLimit() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getSmallMotionLimit() );
#endif
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            cobIdIndex++;
#if 0 	 	// implement Carlos's Weigher Configuration changes
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 3;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromLong( (unsigned long)wc.getCalibrationReference() );
            canMsg.data[1] = getCharFromLong( (unsigned long)wc.getCalibrationReference(), 2 );
            canMsg.data[2] = getCharFromLong( (unsigned long)wc.getCalibrationReference(), 1 );
            canMsg.data[3] = getLsbFromLong( (unsigned long)wc.getCalibrationReference() );
            canMsg.data[4] = getMsbFromShort( (unsigned short)wc.getAutoZeroMaintenanceLimit() );
            canMsg.data[5] = getLsbFromShort( (unsigned short)wc.getAutoZeroMaintenanceLimit() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getPrepackMotionCount() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getPrepackMotionCount() );
#else
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 3;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( (unsigned short)wc.getLargeMotionLimit() );
            canMsg.data[1] = getLsbFromShort( (unsigned short)wc.getLargeMotionLimit() );
            canMsg.data[2] = getMsbFromShort( (unsigned short)wc.getLargeMotionCount() );
            canMsg.data[3] = getLsbFromShort( (unsigned short)wc.getLargeMotionCount() );
            canMsg.data[4] = getMsbFromShort( (unsigned short)wc.getSmallMotionCount() );
            canMsg.data[5] = getLsbFromShort( (unsigned short)wc.getSmallMotionCount() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getNoMotionCount() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getNoMotionCount() );
#endif
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            cobIdIndex++;
#if 0 	 	// implement Carlos's Weigher Configuration changes
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 4;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( (unsigned short)wc.getInitializeZeroTime() );
            canMsg.data[1] = getLsbFromShort( (unsigned short)wc.getInitializeZeroTime() );
            canMsg.data[2] = getMsbFromShort( (unsigned short)wc.getAutoZeroTime() );
            canMsg.data[3] = getLsbFromShort( (unsigned short)wc.getAutoZeroTime() );
            canMsg.data[4] = getMsbFromShort( (unsigned short)wc.getRateOptionLimit() );
            canMsg.data[5] = getLsbFromShort( (unsigned short)wc.getRateOptionLimit() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getZeroReferenceLimit() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getZeroReferenceLimit() );
#else
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 4;
            canMsg.can_dlc = 8;
            canMsg.data[0] = (unsigned char)wc.getFilterSpeed();
            canMsg.data[1] = (unsigned char)wc.getWeigherType();
            canMsg.data[2] = wc.getFlags();
            canMsg.data[3] = (unsigned char)wc.getWeigherModel();
            canMsg.data[4] = getMsbFromLong( (unsigned long)wc.getMinWeightToPrint());
            canMsg.data[5] = getCharFromLong( (unsigned long)wc.getMinWeightToPrint(), 2 );
            canMsg.data[6] = getCharFromLong( (unsigned long)wc.getMinWeightToPrint(), 1 );
            canMsg.data[7] = getLsbFromLong( (unsigned long)wc.getMinWeightToPrint() );
#endif
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            cobIdIndex++;
#if 0 	 	// implement Carlos's Weigher Configuration changes
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 5;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( (unsigned short)wc.getCalibrationReferenceLimit() );
            canMsg.data[1] = getLsbFromShort( (unsigned short)wc.getCalibrationReferenceLimit() );
            canMsg.data[2] = getMsbFromShort( (unsigned short)wc.getSmallMotionLimit() );
            canMsg.data[3] = getLsbFromShort( (unsigned short)wc.getSmallMotionLimit() );
            canMsg.data[4] = getMsbFromShort( (unsigned short)wc.getLargeMotionLimit() );
            canMsg.data[5] = getLsbFromShort( (unsigned short)wc.getLargeMotionLimit() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getLargeMotionCount() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getLargeMotionCount() );
#else
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 5;
            canMsg.can_dlc = 8;

            // SOF-5593 if we are a free standing self service scale without value max!
            if( pSerialFlash->isFreeStandingSelfService() ) {

                if (pSerialFlash->isValueMaxModelFSS() )
                {
                    Debug::trace("CanTranslator.ValueMax", "Turning value max on for scale model " + pSerialFlash->getFullMFGSystemStyle() );
                }
                
                // and check the state of the checkbox in service weigher config page 
                wc.setValueMaxEnabled( ( pSerialFlash->isValueMaxModelFSS() &&  Configuration::getInstance()->toBoolCfg(fieldOffsetCfgValueMaxEnabled)
                    ) || ( !pSerialFlash->isValueMaxModelFSS() && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgValueMaxEnabled) ) );
                
            }
			canMsg.data[0] = (unsigned char)wc.getValueMaxEnabled();
            Debug::trace( "SOF-5593", "wc.getValueMaxEnabled(): " + Integer::toString( canMsg.data[0] ) ); 
			canMsg.data[1] = getMsbFromLong( (unsigned long)wc.getLastCalibrationDate() );
            canMsg.data[2] = getCharFromLong( (unsigned long)wc.getLastCalibrationDate(), 2 );
            canMsg.data[3] = getCharFromLong( (unsigned long)wc.getLastCalibrationDate(), 1 );
            canMsg.data[4] = getLsbFromLong( (unsigned long)wc.getLastCalibrationDate());
            canMsg.data[5] = getMsbFromShort( (unsigned short)wc.getNumberOfCalibrations() );
            canMsg.data[6] = getLsbFromShort( (unsigned short)wc.getNumberOfCalibrations() );
            canMsg.data[7] = (unsigned char)wc.getAzsmMotionLimit();
#endif
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            cobIdIndex++;
#if 0 	 	// implement Carlos's Weigher Configuration changes
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 6;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( (unsigned short)wc.getSmallMotionCount() );
            canMsg.data[1] = getLsbFromShort( (unsigned short)wc.getSmallMotionCount() );
            canMsg.data[2] = getMsbFromShort( (unsigned short)wc.getNoMotionCount() );
            canMsg.data[3] = getLsbFromShort( (unsigned short)wc.getNoMotionCount() );
            canMsg.data[4] = getMsbFromShort( (unsigned short)wc.getZeroTrackingOption() );
            canMsg.data[5] = getLsbFromShort( (unsigned short)wc.getZeroTrackingOption() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getNegativeWeightOption() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getNegativeWeightOption() );
#else
            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 6;
            canMsg.can_dlc = 6;
            canMsg.data[0] = getMsbFromLong( (unsigned long)wc.getLastConfigurationDate() );
            canMsg.data[1] = getCharFromLong( (unsigned long)wc.getLastConfigurationDate(), 2 );
            canMsg.data[2] = getCharFromLong( (unsigned long)wc.getLastConfigurationDate(), 1 );
            canMsg.data[3] = getLsbFromLong( (unsigned long)wc.getLastConfigurationDate());
            canMsg.data[4] = getMsbFromShort( (unsigned short)wc.getNumberOfConfigurations() );
            canMsg.data[5] = getLsbFromShort( (unsigned short)wc.getNumberOfConfigurations() );
#endif
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
#if 0 	 	// implement Carlos's Weigher Configuration changes
            cobIdIndex++;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 7;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( (unsigned short)wc.getPowerupMaintenanceZoneLimit() );
            canMsg.data[1] = getLsbFromShort( (unsigned short)wc.getPowerupMaintenanceZoneLimit() );
            canMsg.data[2] = getMsbFromShort( (unsigned short)wc.getOperateMaintenanceZoneLimit() );
            canMsg.data[3] = getLsbFromShort( (unsigned short)wc.getOperateMaintenanceZoneLimit() );
            canMsg.data[4] = getMsbFromShort( (unsigned short)wc.getNumberOfCalibrations() );
            canMsg.data[5] = getLsbFromShort( (unsigned short)wc.getNumberOfCalibrations() );
            canMsg.data[6] = getMsbFromShort( (unsigned short)wc.getNumberOfCalibrations() );
            canMsg.data[7] = getLsbFromShort( (unsigned short)wc.getNumberOfCalibrations() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            cobIdIndex++;


            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 8;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromLong( (unsigned long)wc.getLastCalibrationDate() );
            canMsg.data[1] = getCharFromLong( (unsigned long)wc.getLastCalibrationDate(), 2 );
            canMsg.data[2] = getCharFromLong( (unsigned long)wc.getLastCalibrationDate(), 1 );
            canMsg.data[3] = getLsbFromLong( (unsigned long)wc.getLastCalibrationDate());
            canMsg.data[4] = (unsigned char)wc.getFilterSpeed();
            canMsg.data[5] = (unsigned char)wc.getWeigherType();
            canMsg.data[6] = wc.getFlags();
            canMsg.data[7] = (unsigned char)wc.getWeigherModel();
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            cobIdIndex++;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgConfig) + 9;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromLong( (unsigned long)wc.getLastConfigurationDate() );
            canMsg.data[1] = getCharFromLong( (unsigned long)wc.getLastConfigurationDate(), 2 );
            canMsg.data[2] = getCharFromLong( (unsigned long)wc.getLastConfigurationDate(), 1 );
            canMsg.data[3] = getLsbFromLong( (unsigned long)wc.getLastConfigurationDate());
            canMsg.data[4] = getMsbFromLong( (unsigned short)wc.getMinWeightToPrint());
            canMsg.data[5] = getCharFromLong( (unsigned short)wc.getMinWeightToPrint(), 2 );
            canMsg.data[6] = getCharFromLong( (unsigned short)wc.getMinWeightToPrint(), 1 );
            canMsg.data[7] = getLsbFromLong( (unsigned short)wc.getMinWeightToPrint() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
#endif
            break;
        }
        case WgCat3ReqStatistics : {
        	int nBytes = 0;

        	canMsg.can_id = pCanLibrary->getWeigherCobID(WgCat3ReqStatistics);
            canMsg.can_dlc = 1;
            canMsg.data[0] = 0;

            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgCat3ReqStatistics. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgCat3ReqPage: {
        	WeigherCat3PageRequest req(recvBuffer);
        	int nBytes = 0;

        	canMsg.can_id = pCanLibrary->getWeigherCobID(WgCat3ReqPage);
            canMsg.can_dlc = 2;
            canMsg.data[0] = req.getPageNumber();
            canMsg.data[1] = req.getPageIndex();
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgCat3ReqPage. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgCat3WriteRecord: {
        	WeigherWriteCat3Record record(recvBuffer);
        	int nBytes = 0;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgCat3WriteRecord);
            canMsg.can_dlc = 7;
            canMsg.data[0] = record.getEvent();
            canMsg.data[1] = record.getAuditType();
            canMsg.data[2] = record.getParamId();
            canMsg.data[3] = getMsbFromLong( record.getEpochTime() );
            canMsg.data[4] = getCharFromLong( record.getEpochTime(), 2 );
            canMsg.data[5] = getCharFromLong( record.getEpochTime(), 1 );
            canMsg.data[6] = getLsbFromLong( record.getEpochTime() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgCat3WriteRecord. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgCat3WriteRecord) + 1;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromLong( record.getPreviousValue() );
            canMsg.data[1] = getCharFromLong( record.getPreviousValue(), 2 );
            canMsg.data[2] = getCharFromLong( record.getPreviousValue(), 1 );
            canMsg.data[3] = getLsbFromLong( record.getPreviousValue() );
            canMsg.data[4] = getMsbFromLong( record.getNewValue() );
            canMsg.data[5] = getCharFromLong( record.getNewValue(), 2 );
            canMsg.data[6] = getCharFromLong( record.getNewValue(), 1 );
            canMsg.data[7] = getLsbFromLong( record.getNewValue() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgCat3WriteRecord. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

        	break;
        }
        case WgCat3ReqNextRecords: {
        	int nBytes = 0;

        	canMsg.can_id = pCanLibrary->getWeigherCobID(WgCat3ReqNextRecords);
            canMsg.can_dlc = 1;
            canMsg.data[0] = 0;
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgCat3ReqNextRecords. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgCat3ReqEraseLog: {
        	int nBytes = 0;

        	canMsg.can_id = pCanLibrary->getWeigherCobID(WgCat3ReqEraseLog);
            canMsg.can_dlc = 1;
            canMsg.data[0] = 0;
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgCat3ReqEraseLog. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgCat3DateTime: {
        	WeigherCat3DateTime date(recvBuffer);
        	int nBytes = 0;

        	canMsg.can_id = pCanLibrary->getWeigherCobID(WgCat3DateTime);
            canMsg.can_dlc = 4;
            canMsg.data[0] = getMsbFromLong( date.getEpochTime() );
            canMsg.data[1] = getCharFromLong( date.getEpochTime(), 2 );
            canMsg.data[2] = getCharFromLong( date.getEpochTime(), 1 );
            canMsg.data[3] = getLsbFromLong( date.getEpochTime() );

            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteAssembly. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgValueMaxWriteAssembly: {
        	WeigherValueMaxWriteAssembly assembly(recvBuffer);
            std::string str = assembly.getAssemblyNumber();
        	int nBytes = 0;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteAssembly);
            canMsg.can_dlc = 8;
            canMsg.data[0] = str[0];
            canMsg.data[1] = str[1];
            canMsg.data[2] = str[2];
            canMsg.data[3] = str[3];
            canMsg.data[4] = str[4];
            canMsg.data[5] = str[5];
            canMsg.data[6] = str[6];
            canMsg.data[7] = str[7];
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteAssembly. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteAssembly) + 1;
            canMsg.can_dlc = 2;
            canMsg.data[0] = str[8];
            canMsg.data[1] = str[9];
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteAssembly. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

        	break;
        }
        case WgValueMaxWriteHwVersion: {
        	WeigherValueMaxWriteHwVersion hwVersion(recvBuffer);
            std::string str = hwVersion.getHardwareVersion();
        	int nBytes = 0;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteHwVersion);
            canMsg.can_dlc = 2;
            canMsg.data[0] = str[0];
            canMsg.data[1] = str[1];
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteHwVersion. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

        	break;
        }
        case WgValueMaxWriteSerialNumber: {
        	WeigherValueMaxWriteSerialNumber serialNumber(recvBuffer);
            std::string str = serialNumber.getSerialNumber();
        	int nBytes = 0;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteSerialNumber);
            canMsg.can_dlc = 8;
            canMsg.data[0] = str[0];
            canMsg.data[1] = str[1];
            canMsg.data[2] = str[2];
            canMsg.data[3] = str[3];
            canMsg.data[4] = str[4];
            canMsg.data[5] = str[5];
            canMsg.data[6] = str[6];
            canMsg.data[7] = str[7];
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteSerialNumber. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteSerialNumber) + 1;
            canMsg.can_dlc = 8;
            canMsg.data[0] = str[8];
            canMsg.data[1] = str[9];
            canMsg.data[2] = str[10];
            canMsg.data[3] = str[11];
            canMsg.data[4] = str[12];
            canMsg.data[5] = str[13];
            canMsg.data[6] = str[14];
            canMsg.data[7] = str[15];
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteSerialNumber. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgValueMaxWriteDateOfManufacture: {
        	WeigherValueMaxWriteDateOfMan date(recvBuffer);

        	int nBytes = 0;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteDateOfManufacture);
            canMsg.can_dlc = 4;
            canMsg.data[0] = getMsbFromLong( date.getDate() );
            canMsg.data[1] = getCharFromLong( date.getDate(), 2 );
            canMsg.data[2] = getCharFromLong( date.getDate(), 1 );
            canMsg.data[3] = getLsbFromLong( date.getDate() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteDateOfManufacture. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgValueMaxWriteXGain: {
        	WeigherValueMaxWriteXGainFactor x(recvBuffer);
        	int nBytes = 0;
        	float xValue = x.getGain();
        	unsigned char *pValue = (unsigned char *)&xValue;
        	canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteXGain);
            canMsg.can_dlc = 4;
            canMsg.data[0] = *pValue++;
            canMsg.data[1] = *pValue++;
            canMsg.data[2] = *pValue++;
            canMsg.data[3] = *pValue;
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteXGain. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgValueMaxWriteYGain: {
        	WeigherValueMaxWriteYGainFactor y(recvBuffer);
        	int nBytes = 0;
        	float yValue = y.getGain();
        	unsigned char *pValue = (unsigned char *)&yValue;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteYGain);
            canMsg.can_dlc = 4;
            canMsg.data[0] = *pValue++;
            canMsg.data[1] = *pValue++;
            canMsg.data[2] = *pValue++;
            canMsg.data[3] = *pValue++;
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteYGain. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
        case WgValueMaxWriteZGain: {
        	WeigherValueMaxWriteZGainFactor z(recvBuffer);
        	int nBytes = 0;
        	float zValue = z.getGain();
        	unsigned char *pValue = (unsigned char *)&zValue;

            canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxWriteZGain);
            canMsg.can_dlc = 4;
            canMsg.data[0] = *pValue++;
            canMsg.data[1] = *pValue++;
            canMsg.data[2] = *pValue++;
            canMsg.data[3] = *pValue++;
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);

                if( nBytes != sizeof(canMsg)) {
                    Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxWriteZGain. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            }
            else {
                Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent) );
            }
        	break;
        }
		case WgValueMaxFitParams:
		{
			WeigherValueMaxFitParams fp(recvBuffer);
			int nBytes = 0;

			//--------
			// Frame 0
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams);
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getGravity());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getGravity(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getGravity(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getGravity());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getConvFactor());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getConvFactor(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getConvFactor(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getConvFactor());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 0): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 0): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 1
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 1;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getZeroValue());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getZeroValue(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getZeroValue(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getZeroValue());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getSpanValue());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getSpanValue(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getSpanValue(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getSpanValue());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 1): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 1): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 2
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 2;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getZ0ElectronicOffset());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getZ0ElectronicOffset(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getZ0ElectronicOffset(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getZ0ElectronicOffset());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getDeadWeight());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getDeadWeight(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getDeadWeight(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getDeadWeight());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 2): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 2): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 3
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 3;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getX2());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getX2(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getX2(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getX2());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getX1());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getX1(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getX1(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getX1());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 3): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 3): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 4
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 4;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getX0());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getX0(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getX0(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getX0());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getY2());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getY2(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getY2(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getY2());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 4): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 4): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 5
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 5;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getY1());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getY1(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getY1(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getY1());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getY0());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getY0(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getY0(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getY0());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 5): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 5): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 6
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 6;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getX_Rot_0());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getX_Rot_0(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getX_Rot_0(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getX_Rot_0());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getX_Rot_1());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getX_Rot_1(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getX_Rot_1(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getX_Rot_1());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 6): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 6): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 7
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 7;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getX_Rot_2());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getX_Rot_2(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getX_Rot_2(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getX_Rot_2());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getY_Rot_0());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getY_Rot_0(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getY_Rot_0(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getY_Rot_0());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 7): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 7): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 8
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 8;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getY_Rot_1());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getY_Rot_1(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getY_Rot_1(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getY_Rot_1());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getY_Rot_2());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getY_Rot_2(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getY_Rot_2(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getY_Rot_2());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 8): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 8): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//--------
			// Frame 9
			//--------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 9;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getZ_Rot_0());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getZ_Rot_0(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getZ_Rot_0(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getZ_Rot_0());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getZ_Rot_1());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getZ_Rot_1(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getZ_Rot_1(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getZ_Rot_1());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 9): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 9): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//---------
			// Frame 10
			//---------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 10;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getZ_Rot_2());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getZ_Rot_2(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getZ_Rot_2(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getZ_Rot_2());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getX_Gain());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getX_Gain(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getX_Gain(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getX_Gain());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 10): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 10): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//---------
			// Frame 11
			//---------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 11;
			canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getY_Gain());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getY_Gain(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getY_Gain(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getY_Gain());
			canMsg.data[4] = getMsbFromLong((unsigned long)fp.getG_Coeff());
			canMsg.data[5] = getCharFromLong((unsigned long)fp.getG_Coeff(), 2);
			canMsg.data[6] = getCharFromLong((unsigned long)fp.getG_Coeff(), 1);
			canMsg.data[7] = getLsbFromLong((unsigned long)fp.getG_Coeff());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 11): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 11): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			//---------
			// Frame 12
			//---------
			canMsg.can_id = pCanLibrary->getWeigherCobID(WgValueMaxFitParams) + 12;
			canMsg.can_dlc = 4;
			canMsg.data[0] = getMsbFromLong((unsigned long)fp.getCheckSum());
			canMsg.data[1] = getCharFromLong((unsigned long)fp.getCheckSum(), 2);
			canMsg.data[2] = getCharFromLong((unsigned long)fp.getCheckSum(), 1);
			canMsg.data[3] = getLsbFromLong((unsigned long)fp.getCheckSum());
			if (canMsg.can_id != 0)
			{
				nBytes = pCanManager->send(&canMsg);
				if (nBytes != sizeof(canMsg))
				{
					Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to send WgValueMaxFitParams. Sent (Frame 12): " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
				}
			}
			else
			{
				Debug::trace("CanTranslator.Error", "translateWeigherEvent(): Failed to get COB id for next frame (Frame 12): " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wgEvent));
			}

			break;
		}

		case WgKPCDetached: {
        	pWeigherManager = NULL;
        if (!pKPCBootManager)
        {
            Debug::trace("CanTranslator.info", "translateWeigherEvent(): Creating Boot Manager Instance ");
            pKPCBootManager = KPCBootManager::getInstance();
        }
        	break;
        }
        default: {
            Debug::trace( "CanTranslator.Error", "translateWeigherEvent(): Unknown Weigher event: " + Integer::toString(wgEvent) );
            break;
        }

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translatePrinterEvent(PrEvent prEvent, const char *recvBuffer, int bufLen) {

    Debug::trace( "CanTranslator.Translate", "translatePrinterEvent() PrEvent:" + Integer::toString(prEvent) );
	Debug::trace("CanTranslator.Translate", std::string("translatePrinterEvent recvBuffer: ") + HobString(Hex::toString(recvBuffer, bufLen) ));

    //determine which printer the message is intended (all printer messages must define a type!)
    can_frame canMsg;
    memset( &canMsg, 0, sizeof(can_frame) );

    PrinterGeneric printerGeneric(recvBuffer);
    PrinterType type = printerGeneric.getPrinterType();
	Debug::trace("CanTranslator.Translate", "translatePrinterEvent() PrinterType: " + HobString(Integer::toString(type)));

	//PrinterType type = FirstPrinter;

    if( type != FirstPrinter && type != SecondPrinter && type != ThirdPrinter )
	{
        Debug::trace("CanTranslator.Error", "translatePrinterEvent(): Asked to translate unsupported printerNum: " + HobString(Integer::toString(type)));
		Debug::trace("CanTranslator.Error", "translatePrinterEvent(): unsupported printerNum so return " );
		return;
    }
	else
	{
		Debug::trace("CanTranslator.Translate", "translatePrinterEvent(): printerNum so process the printNum " + HobString(Integer::toString(type)));
	}

    switch (prEvent) {
    	case PrReqWakeup:
    	case PrReqInquire:
    	case PrReqStatus:
    	case PrReqSensors:
    	case PrEnableTakeup:
    	//case PrFlush:
    	case PrEnable:
    	case PrDisable:
    	case PrReset:
    	case PrReqVersion:
    	case PrCutterHome:
    	case PrCutterCut:
    	case PrReqCutterStatus:
    	case PrResetFactoryDefaults: 
        case PrStartGapCalibration: 
        case PrGapCalNext:
        case PrUseContinuous:           //SOF-5124
        case PrCutterBitDflt: {         //SOF-3887
            canMsg.can_id = pCanLibrary->getPrinterCobID( prEvent, type );
            canMsg.can_dlc = 1;
            canMsg.data[0] = 0;
            if(canMsg.can_id != 0)
			{
				Debug::trace("CanTranslator.Translate", "translatePrinterEvent() PrEvent:" + Integer::toString(prEvent));
            	pCanManager->send( &canMsg );
            }
			else
			{
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrEvent:" + Integer::toString(prEvent));
            }
    		break;
    	}
        case PrFlush: {
            // not implemented on either HTM or FSSS scales. Not used in KPC firmware so no need to send.
            break;
        }
    	case PrReqCalibrate: {
    		PrinterRequestCalibrate cal( recvBuffer );

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type );
            canMsg.can_dlc = 1;
            canMsg.data[0] = cal.getPrinterCalibrateType();

            if(canMsg.can_id != 0)
			{
            	pCanManager->send( &canMsg );
            }
			else
			{
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrReqCalibrate!" );
            }
    		break;
    	}
    	case PrCommand : {
    		PrinterCommand cmd( recvBuffer );

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type );
            canMsg.can_dlc = 6;
			canMsg.data[0] = cmd.getPrinterCommand();
			canMsg.data[1] = cmd.getPrinterCommandOptions();
			canMsg.data[2] = getMsbFromShort( (unsigned short)cmd.getPrinterIndirection() );
			canMsg.data[3] = getLsbFromShort( (unsigned short)cmd.getPrinterIndirection() );
			canMsg.data[4] = getMsbFromShort( (unsigned short)cmd.getValue() );
			canMsg.data[5] = getLsbFromShort( (unsigned short)cmd.getValue() );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrCommand!" );
            }
    		break;
    	}
    	case PrRam: {
    		PrinterRam ram( recvBuffer );

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type );
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 0 ) );
			canMsg.data[1] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 0 ) );
			canMsg.data[2] = getMsbFromShort( (unsigned short)ram.getRamValue( 0 ) );
			canMsg.data[3] = getLsbFromShort( (unsigned short)ram.getRamValue( 0 ) );
			canMsg.data[4] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 1 ) );
			canMsg.data[5] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 1 ) );
			canMsg.data[6] = getMsbFromShort( (unsigned short)ram.getRamValue( 1 ) );
			canMsg.data[7] = getLsbFromShort( (unsigned short)ram.getRamValue( 1 ) );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrRam!" );
            }

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type ) + 1;
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 2 ) );
			canMsg.data[1] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 2 ) );
			canMsg.data[2] = getMsbFromShort( (unsigned short)ram.getRamValue( 2 ) );
			canMsg.data[3] = getLsbFromShort( (unsigned short)ram.getRamValue( 2 ) );
			canMsg.data[4] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 3 ) );
			canMsg.data[5] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 3 ) );
			canMsg.data[6] = getMsbFromShort( (unsigned short)ram.getRamValue( 3 ) );
			canMsg.data[7] = getLsbFromShort( (unsigned short)ram.getRamValue( 3 ) );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrRam!" );
            }

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type ) + 2;
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 4 ) );
			canMsg.data[1] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 4 ) );
			canMsg.data[2] = getMsbFromShort( (unsigned short)ram.getRamValue( 4 ) );
			canMsg.data[3] = getLsbFromShort( (unsigned short)ram.getRamValue( 4 ) );
			canMsg.data[4] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 5 ) );
			canMsg.data[5] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 5 ) );
			canMsg.data[6] = getMsbFromShort( (unsigned short)ram.getRamValue( 5 ) );
			canMsg.data[7] = getLsbFromShort( (unsigned short)ram.getRamValue( 5 ) );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrRam!" );
            }

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type ) + 3;
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 6 ) );
			canMsg.data[1] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 6 ) );
			canMsg.data[2] = getMsbFromShort( (unsigned short)ram.getRamValue( 6 ) );
			canMsg.data[3] = getLsbFromShort( (unsigned short)ram.getRamValue( 6 ) );
			canMsg.data[4] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 7 ) );
			canMsg.data[5] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 7 ) );
			canMsg.data[6] = getMsbFromShort( (unsigned short)ram.getRamValue( 7 ) );
			canMsg.data[7] = getLsbFromShort( (unsigned short)ram.getRamValue( 7 ) );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrRam!" );
            }

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type ) + 4;
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 8 ) );
			canMsg.data[1] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 8 ) );
			canMsg.data[2] = getMsbFromShort( (unsigned short)ram.getRamValue( 8 ) );
			canMsg.data[3] = getLsbFromShort( (unsigned short)ram.getRamValue( 8 ) );
			canMsg.data[4] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 9 ) );
			canMsg.data[5] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 9 ) );
			canMsg.data[6] = getMsbFromShort( (unsigned short)ram.getRamValue( 9 ) );
			canMsg.data[7] = getLsbFromShort( (unsigned short)ram.getRamValue( 9 ) );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrRam!" );
            }

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type ) + 5;
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 10 ) );
			canMsg.data[1] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 10 ) );
			canMsg.data[2] = getMsbFromShort( (unsigned short)ram.getRamValue( 10 ) );
			canMsg.data[3] = getLsbFromShort( (unsigned short)ram.getRamValue( 10 ) );
			canMsg.data[4] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 11 ) );
			canMsg.data[5] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 11 ) );
			canMsg.data[6] = getMsbFromShort( (unsigned short)ram.getRamValue( 11 ) );
			canMsg.data[7] = getLsbFromShort( (unsigned short)ram.getRamValue( 11 ) );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrRam!" );
            }

            canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type ) + 6;
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 12 ) );
			canMsg.data[1] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 12 ) );
			canMsg.data[2] = getMsbFromShort( (unsigned short)ram.getRamValue( 12 ) );
			canMsg.data[3] = getLsbFromShort( (unsigned short)ram.getRamValue( 12 ) );
			canMsg.data[4] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 13 ) );
			canMsg.data[5] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 13 ) );
			canMsg.data[6] = getMsbFromShort( (unsigned short)ram.getRamValue( 13 ) );
			canMsg.data[7] = getLsbFromShort( (unsigned short)ram.getRamValue( 13 ) );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrRam!" );
            }

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type ) + 7;
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 14 ) );
			canMsg.data[1] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 14 ) );
			canMsg.data[2] = getMsbFromShort( (unsigned short)ram.getRamValue( 14 ) );
			canMsg.data[3] = getLsbFromShort( (unsigned short)ram.getRamValue( 14 ) );
			canMsg.data[4] = getMsbFromShort( (unsigned short)ram.getRamIndirection( 15 ) );
			canMsg.data[5] = getLsbFromShort( (unsigned short)ram.getRamIndirection( 15 ) );
			canMsg.data[6] = getMsbFromShort( (unsigned short)ram.getRamValue( 15 ) );
			canMsg.data[7] = getLsbFromShort( (unsigned short)ram.getRamValue( 15 ) );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrRam!" );
            }
    		break;
    	}
    	case PrMask: {
    		PrinterMask mask( recvBuffer );

    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type );
            canMsg.can_dlc = 3;
			canMsg.data[0] = mask.getSensorMask();
			canMsg.data[1] = mask.getSensor2Mask();
			canMsg.data[2] = mask.getUserMask();
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrMask!" );
            }
    		break;
    	}
    	case PrMode: {
    		PrinterMode mode( recvBuffer );
    		canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type );
            canMsg.can_dlc = 1;
			canMsg.data[0] = mode.getPrinterMode();
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrMode!" );
            }

    		break;
    	}
    	case PrTeach: {
    		PrinterTeach teach( recvBuffer );
            Debug::trace("CanTranslator.cutter","Teach command: " + Integer::toString(teach.getPrinterCommandType()) + "data:" + Integer::toString(teach.getNumEntries()) );
    		PrTeachMsg cmdList;

    		memset( &cmdList.operation, (UINT8) 0x00, sizeof( cmdList.operation ) );
    		CommandOperationList *pList = teach.getCommandOperationList();
    		if( pList != NULL ) {

    			unsigned short size = (unsigned short)pList->getCommandBlock( (CommandOperationMsg *)&cmdList.operation, true );

    			canMsg.can_id  = pCanLibrary->getPrinterCobID( prEvent, type );
				canMsg.can_dlc = 3;
				canMsg.data[0] = teach.getPrinterCommandType();
				canMsg.data[1] = getMsbFromShort( size );
				canMsg.data[2] = getLsbFromShort( size );
				if(canMsg.can_id != 0) {
					pCanManager->send( &canMsg );
				} else {
					Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrTeach!" );
				}

				int id =  pCanLibrary->getPrinterCobID( prEvent, type ) + 1;

				for( int i = 0; i < size; i ++ ) {
					canMsg.can_id  = id++;
					canMsg.can_dlc = 8;
					canMsg.data[0] = cmdList.operation[i].generic.commandDirective;
					canMsg.data[1] = cmdList.operation[i].generic.pad[0];
					canMsg.data[2] = cmdList.operation[i].generic.pad[2];
					canMsg.data[3] = cmdList.operation[i].generic.pad[1];
					canMsg.data[4] = cmdList.operation[i].generic.pad[3];
					canMsg.data[5] = cmdList.operation[i].generic.pad[4];
					canMsg.data[6] = cmdList.operation[i].generic.pad[5];
					canMsg.data[7] = cmdList.operation[i].generic.pad[6];
					if(canMsg.can_id != 0) {
						pCanManager->send( &canMsg );
					} else {
						Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrTeach!" );
					}
                    // 2mS sleep to eliminate transmit error.
                    PAL_absclass::PAL_sleep( 20 );

				}
                #if 0  //SOF-3165
				// device is expecting the maximum size even if unused.
				int remaining = MaxTeachTableEntries - size;

				for( int i = 0; i < remaining; i++ ) {
					canMsg.can_id  = id++;
					canMsg.can_dlc = 8;
					canMsg.data[0] = 0;
					canMsg.data[1] = 0;
					canMsg.data[2] = 0;
					canMsg.data[3] = 0;
					canMsg.data[4] = 0;
					canMsg.data[5] = 0;
					canMsg.data[6] = 0;
					canMsg.data[7] = 0;
					if(canMsg.can_id != 0) {
						pCanManager->send( &canMsg );
					} else {
						Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrTeach!" );
					}
				}
                #endif
    		} else {
    			Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Command list is NULL for PrTeach!" );
    		}
    		break;
    	}
    	case PrTest: {
            PrinterTest test( recvBuffer );

            canMsg.can_id = pCanLibrary->getPrinterCobID( prEvent, type );
            canMsg.can_dlc = 1;
            canMsg.data[0] = test.getData();
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrTest!" );
            }
            break;
        }
        case PrNomSize: {
            Debug::trace("CanTranslator.Translate", "translatePrinterEvent(): PrNomSize");
            PrinterNomSize printerSize(recvBuffer);
            
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 4;
            canMsg.data[0] = getMsbFromShort( (unsigned short)printerSize.getActualSize() );
            canMsg.data[1] = getLsbFromShort( (unsigned short)printerSize.getActualSize() );
            canMsg.data[2] = getMsbFromShort( (unsigned short)printerSize.getMeasuredSize() );
            canMsg.data[3] = getLsbFromShort( (unsigned short)printerSize.getMeasuredSize() );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrNomSize!" );
            }
            break;
        }
        case PrConfig: {
            //Debug::trace("CanTranslator.PrConfig", "CanTranslator::translatePrinterEvent() PrConfig (7,1):  Got a PrConfig message in CanTranslator");

            PrinterConfiguration prConfig(recvBuffer);
            
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort((unsigned short)prConfig.getConfigType());
            canMsg.data[1] = getLsbFromShort((unsigned short)prConfig.getConfigType());
            canMsg.data[2] = prConfig.getPrinterType();
            canMsg.data[3] = prConfig.getLabelWidth();
            canMsg.data[4] = prConfig.getMediaSensorAdjustment();
            canMsg.data[5] = prConfig.getOutOfMediaCount();
            canMsg.data[6] = getMsbFromShort((unsigned short)prConfig.getPrintContrast());
            canMsg.data[7] = getLsbFromShort((unsigned short)prConfig.getPrintContrast());

            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigType(): " + Integer::toString(prConfig.getConfigType()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getPrinterType(): " + Integer::toString(prConfig.getPrinterType()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getLabelWidth(): " + Integer::toString(prConfig.getLabelWidth()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getMediaSensorAdjustment(): " + Integer::toString(prConfig.getMediaSensorAdjustment()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getOutOfMediaCount()): " + Integer::toString(prConfig.getOutOfMediaCount()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getPrintContrast(): " + Integer::toString(prConfig.getPrintContrast()));

            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrConfig!" );
            }
            
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type) + 1;
            canMsg.can_dlc = 8;
			canMsg.data[0] = getMsbFromShort((unsigned short)prConfig.getConfigWord0());
			canMsg.data[1] = getLsbFromShort((unsigned short)prConfig.getConfigWord0());
            canMsg.data[2] = getMsbFromShort((unsigned short)prConfig.getConfigWord1());
            canMsg.data[3] = getLsbFromShort((unsigned short)prConfig.getConfigWord1());
            canMsg.data[4] = getMsbFromShort((unsigned short)prConfig.getConfigWord2());
            canMsg.data[5] = getLsbFromShort((unsigned short)prConfig.getConfigWord2());
            canMsg.data[6] = getMsbFromShort((unsigned short)prConfig.getConfigWord3());
            canMsg.data[7] = getLsbFromShort((unsigned short)prConfig.getConfigWord3());

            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord0(): " + Integer::toString(prConfig.getConfigWord0()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord1(): " + Integer::toString(prConfig.getConfigWord1()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord2(): " + Integer::toString(prConfig.getConfigWord2()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord3(): " + Integer::toString(prConfig.getConfigWord3()));

            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrConfig!" );
            }
            
			canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type) + 2;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort((unsigned short)prConfig.getConfigWord4());
            canMsg.data[1] = getLsbFromShort((unsigned short)prConfig.getConfigWord4());
            canMsg.data[2] = getMsbFromShort((unsigned short)prConfig.getConfigWord5());
            canMsg.data[3] = getLsbFromShort((unsigned short)prConfig.getConfigWord5());
            canMsg.data[4] = getMsbFromShort((unsigned short)prConfig.getConfigWord6());
            canMsg.data[5] = getLsbFromShort((unsigned short)prConfig.getConfigWord6());
            canMsg.data[6] = getMsbFromShort((unsigned short)prConfig.getConfigWord7());
            canMsg.data[7] = getLsbFromShort((unsigned short)prConfig.getConfigWord7());
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord4(): " + Integer::toString(prConfig.getConfigWord4()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord5(): " + Integer::toString(prConfig.getConfigWord5()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord6(): " + Integer::toString(prConfig.getConfigWord6()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord7(): " + Integer::toString(prConfig.getConfigWord7()));


            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrConfig!" );
            }
            
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type) + 3;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort((unsigned short)prConfig.getConfigWord8());
            canMsg.data[1] = getLsbFromShort((unsigned short)prConfig.getConfigWord8());
            canMsg.data[2] = getMsbFromShort((unsigned short)prConfig.getConfigWord9());
            canMsg.data[3] = getLsbFromShort((unsigned short)prConfig.getConfigWord9());
            canMsg.data[4] = getMsbFromShort((unsigned short)prConfig.getConfigWord10());
            canMsg.data[5] = getLsbFromShort((unsigned short)prConfig.getConfigWord10());
            canMsg.data[6] = getMsbFromShort((unsigned short)prConfig.getConfigWord11());
            canMsg.data[7] = getLsbFromShort((unsigned short)prConfig.getConfigWord11());
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord8(): " + Integer::toString(prConfig.getConfigWord8() ));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord9(): " + Integer::toString(prConfig.getConfigWord9()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord10(): " + Integer::toString(prConfig.getConfigWord10()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord11(): " + Integer::toString(prConfig.getConfigWord11()));
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrConfig!" );
            }
            
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type) + 4;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort((unsigned short)prConfig.getConfigWord12());
            canMsg.data[1] = getLsbFromShort((unsigned short)prConfig.getConfigWord12());
            canMsg.data[2] = getMsbFromShort((unsigned short)prConfig.getConfigWord13());
            canMsg.data[3] = getLsbFromShort((unsigned short)prConfig.getConfigWord13());
            canMsg.data[4] = getMsbFromShort((unsigned short)prConfig.getMediaSensorType());
            canMsg.data[5] = getLsbFromShort((unsigned short)prConfig.getMediaSensorType());
            canMsg.data[6] = getMsbFromShort((unsigned short)prConfig.getSLAEjectLine());
            canMsg.data[7] = getLsbFromShort((unsigned short)prConfig.getSLAEjectLine());

            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord12(): " + Integer::toString(prConfig.getConfigWord12()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getConfigWord13(): " + Integer::toString(prConfig.getConfigWord13()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getMediaSensorType(): " + Integer::toString(prConfig.getSLAEjectLine()));
            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getSLAEjectLine(): " + Integer::toString(prConfig.getSLAEjectLine()));

            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrConfig!" );
            }
            
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type) + 5;
            canMsg.can_dlc = 2;
            canMsg.data[0] = getMsbFromShort((unsigned short)prConfig.getPrintPositionAbsolute());
            canMsg.data[1] = getLsbFromShort((unsigned short)prConfig.getPrintPositionAbsolute());

            //Debug::trace("CanTranslator.PrConfig", "translatePrinterEvent() prConfig.getPrintPositionAbsolute(): " + Integer::toString(prConfig.getPrintPositionAbsolute()));

            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrConfig!" );
            }
            break;
        }
        case PrReqLabelTransfer: {
        	Debug::trace("CanTranslator.Translate", "translatePrinterEvent(): PrReqLabelTransfer");
        	PrinterRequestLabelTransfer transfer( recvBuffer );

        	canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 4;
            canMsg.data[0] = getMsbFromShort((unsigned short)transfer.getTotalDataLength());
            canMsg.data[1] = getLsbFromShort((unsigned short)transfer.getTotalDataLength());
            canMsg.data[2] = getMsbFromShort((unsigned short)transfer.getActualDataLength());
            canMsg.data[3] = getLsbFromShort((unsigned short)transfer.getActualDataLength());

            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrReqLabelTransfer!" );
            }
        	break;
        }
        case PrReqPrintheadPower: {
        	PrinterRequestPrintheadPower reqPower( recvBuffer );
        	canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 1;
            canMsg.data[0] = reqPower.getPowerState();

            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrReqPrintheadPower!" );
            }
        	break;
        }
        case PrKPCDetached: {
        	pPrinter1Manager = NULL;
        if (!pKPCBootManager)
        {
            Debug::trace("CanTranslator.info", "translatePrinterEvent(): Creating Boot Manager Instance ");
            pKPCBootManager = KPCBootManager::getInstance();
        }
        	break;
        }
        case PrNewOrder: {
        	PrinterNewPrinterType stationOrder( recvBuffer );
        	canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 1;
            canMsg.data[0] = stationOrder.getNewPrinterType();

            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrNewOrder!" );
            }
        	break;
        }
        case PrLabelImageSize: {
        	Debug::trace("CanTranslator.Translate", "translatePrinterEvent(): PrLabelImageSize");
        	PrinterLabelImageSize size( recvBuffer );

        	canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 4;
            canMsg.data[0] = getMsbFromLong( size.getImageSize() );
            canMsg.data[1] = getCharFromLong( size.getImageSize(), 2 );
            canMsg.data[2] = getCharFromLong( size.getImageSize(), 1 );
            canMsg.data[3] = getLsbFromLong( size.getImageSize() );

            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            } else {
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrLabelImageSize!" );
            }
        	break;
        }
        case PrReqHeadDotWear: {
            Debug::trace("CanTranslator.HeadDot", "translatePrinterEvent() case PrReqHeadDotWear:  Requesting Head Dot Wear!" );
            PrinterRequestHeadDotWear wear( recvBuffer );
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 4;
            canMsg.data[0] = ( wear.getSource() << 8);
            canMsg.data[1] = ( wear.getSource());
            canMsg.data[2] = ( wear.getDestination() << 8);
            canMsg.data[3] = ( wear.getDestination() );
            if(canMsg.can_id != 0)
			{
				Debug::trace("CanTranslator.HeadDot", "translatePrinterEvent() case PrReqHeadDotWear PrEvent: " + Integer::toString(prEvent));
            	pCanManager->send( &canMsg );
            }
			else
			{
            	Debug::trace("CanTranslator.HeadDot", "translatePrinterEvent() case PrReqHeadDotWear:  Failed to get COB id for event: PrReqHeadDotWear!" );
            }
        	break;
        }
        case PrReqHeadDotStatus: {
            Debug::trace("CanTranslator.HeadDot", "translatePrinterEvent() case PrReqHeadDotStatus:  Requesting Head Dot Status!" );
            PrinterRequestHeadDotStatus status( recvBuffer );
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 4;
            canMsg.data[0] = ( status.getSource() << 8);
            canMsg.data[1] = ( status.getSource());
            canMsg.data[2] = ( status.getDestination() << 8);
            canMsg.data[3] = ( status.getDestination() );
            if(canMsg.can_id != 0) 
			{
				Debug::trace("CanTranslator.HeadDot", "translatePrinterEvent() PrReqHeadDotStatus PrEvent: " + Integer::toString(prEvent));
            	pCanManager->send( &canMsg );
            }
			else
			{
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent() PrReqHeadDotStatus:  Failed to get COB id for event: PrReqHeadDotStatus!" );
            }
        	break;
        }
        case PrHeadDotAck: {
            Debug::trace("CanTranslator.HeadDot", "translatePrinterEvent(): case PrHeadDotAck -  Requesting Head Dot Status!" );
            PrinterHeadDotAck ack( recvBuffer );
            canMsg.can_id = pCanLibrary->getPrinterCobID(prEvent, type);
            canMsg.can_dlc = 6;
            canMsg.data[0] = ( ack.getSource() << 8);
            canMsg.data[1] = ( ack.getSource());
            canMsg.data[2] = ( ack.getDestination() << 8);
            canMsg.data[3] = ( ack.getDestination() );
            canMsg.data[4] = ( ack.getDotCount() << 8);
            canMsg.data[5] = ( ack.getDotCount() );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            }
			else 
			{
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrReqHeadDotStatus!" );
            }
        	break;
        }
        default: {
            Debug::trace( "CanTranslator.Error", "translatePrinterEvent(): Unknown Printer event: " + Integer::toString(prEvent) );
			Debug::trace("CanTranslator.Error", "translatePrinterEvent(): Fell to default case so break out");
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateWrapperEvent(WrEvent wrEvent, const char *recvBuffer) {

    int nBytes = 0;

	can_frame canMsg;
	memset( &canMsg, 0, sizeof(can_frame) );

    Debug::trace( "CanTranslator", "translateWrapperEvent() WrEvent:" + Integer::toString(wrEvent) );

    //translate downstream (out going) wrapper messages
    switch (wrEvent) {
		case WrReqStatus:
		case WrReqSensors:
		case WrReqEvolSystemInfo:
        case WrReqInquire:
        case WrEvolReqCycleCount:
        case WrEvolReqSystemReset:
        case WrEvolRequestCameraThreshold:
    	case WrReqWakeup:
    	case WrNgwReqVersion:
    	case WrEvolStartMfgRunin:
    	case WrEvolResetDefaultPackageTimings:
    	case WrEvolSavePackageTimings: {
        	sendWrapperRequest( wrEvent );
        	break;
        }
    	case WrEvolRequestPackageTimings: {
    		const EvolWrapperRequestPackageTimings request(recvBuffer);

    		canMsg.can_dlc = 2;
    		canMsg.can_id = pCanLibrary->getWrapperGenericReqCobID();
    		if( request.getTimingsType() == EvolWrapperRequestPackageTimings::CurrentTimings ) {
    			canMsg.data[0] = (unsigned char)pCanLibrary->getTimingsCurrentRequest();
    			canMsg.data[1] = 0;
    		}
    		if( request.getTimingsType() == EvolWrapperRequestPackageTimings::DefaultTimings ) {
    			canMsg.data[0] = (unsigned char)pCanLibrary->getTimingsDefaultRequest();
    			canMsg.data[1] = 0;
    		}
    		if( request.getTimingsType() == EvolWrapperRequestPackageTimings::TemporaryTimings ) {
    			canMsg.data[0] = (unsigned char)pCanLibrary->getTimingsTemporaryRequest();
    			canMsg.data[1] = 0;
    		}

            if(canMsg.can_id != 0) {
                pCanManager->send(&canMsg);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperEvent(): can_id is NULL. Ignoring wrEvent:" + HobString(Integer::toString(wrEvent)));
            }
    		break;
    	}
        case WrEvolSystemInfoErase: {

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 1;
            canMsg.data[0] = 0;

            Debug::trace( "CanTranslator.Translate", "translateWrapperEvent(): Send CAN Frame: " + Hex::toString(canMsg.can_id) );

            if(canMsg.can_id != 0) {
                pCanManager->send(&canMsg);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperEvent(): can_id is NULL. Ignoring wrEvent:" + HobString(Integer::toString(wrEvent)));
            }
            break;
        }
        case WrLoad: {
            const EvolWrapperLoad wrapperload(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 1;
            canMsg.data[0] = getLsbFromShort( wrapperload.getFilmWidthSelection() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolLoad. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolLoad: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrEvolConfig: {
            const EvolWrapperConfiguration wrapperConfig(recvBuffer);

            const EvolWrapperGeneralConfig wrGenCfg( wrapperConfig.getWrapperConfig() );
            const EvolWrapperHeaterConfig heaterCfg( wrapperConfig.getHeaterConfig() );
            const HeaterConfiguration filmHeaterCfg( heaterCfg.getFilmHeaterConfig() );
            const HeaterConfiguration sealerHeaterCfg( heaterCfg.getSealerHeaterConfig() );
            const EvolLabelApplierConfiguration applierCfg(wrapperConfig.getLabelApplierConfig());
            const WrapperFilmWidthDimensions topFilm( wrGenCfg.getTopFilmRollDimensions() );
            const WrapperFilmWidthDimensions bottomFilm( wrGenCfg.getBottomFilmRollDimensions() );

            Debug::trace( "CanTranslator.WrEvolConfig", "sealerHeaterCfg.getMaxTemp(): " + Integer::toString(sealerHeaterCfg.getMaxTemp() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "sealerHeaterCfg.getMinTemp(): " + Integer::toString(sealerHeaterCfg.getMinTemp() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "sealerHeaterCfg.isItUsed(): " + Integer::toString(sealerHeaterCfg.isItUsed() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "filmHeaterCfg.isItUsed(): " + Integer::toString(filmHeaterCfg.isItUsed() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getSideDischargeType(): " + Integer::toString(applierCfg.getSideDischargeType() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "sealerHeaterCfg.getDefaultTempSetting()): " + Integer::toString(sealerHeaterCfg.getDefaultTempSetting() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "filmHeaterCfg.getMaxTemp(): " + Integer::toString(filmHeaterCfg.getMaxTemp() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "filmHeaterCfg.getMinTemp(): " + Integer::toString(filmHeaterCfg.getMinTemp()) );
            Debug::trace( "CanTranslator.WrEvolConfig", "filmHeaterCfg.getDefaultTempSetting(): " + Integer::toString(filmHeaterCfg.getDefaultTempSetting() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "topFilm.getRollWidth(): " + Integer::toString(topFilm.getRollWidth() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "bottomFilm.getRollWidth(): " + Integer::toString(bottomFilm.getRollWidth() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.doesHaveAutoRotation(): " + Integer::toString(applierCfg.doesHaveAutoRotation() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriLabelOffsetOnPkg(): " + Integer::toString(applierCfg.getPriLabelOffsetOnPkg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getSecLabelOffsetOnPkg(): " + Integer::toString(applierCfg.getSecLabelOffsetOnPkg() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriLabelRotationDefault(): " + Integer::toString(applierCfg.getPriLabelRotationDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriLabelSizeDefault(): " + Integer::toString(applierCfg.getPriLabelSizeDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriLabelWidth(): " + Integer::toString(applierCfg.getPriLabelWidth() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriLabelLocationDefault(): " + Integer::toString(applierCfg.getPriLabelLocationDefault()) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriRotationCalibrationOffset(): " + Integer::toString(applierCfg.getPriRotationCalibrationOffset() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriWandCalibrationOffset(): " + Integer::toString(applierCfg.getPriWandCalibrationOffset() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "wrGenCfg.getSealerBeltPackageSensorType(): " + Integer::toString(wrGenCfg.getSealerBeltPackageSensorType() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getSecLabelRotationDefault(): " + Integer::toString(applierCfg.getSecLabelRotationDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getSecLabelSizeDefault(): " + Integer::toString(applierCfg.getSecLabelSizeDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getSecLabelWidth() : " + Integer::toString(applierCfg.getSecLabelWidth()  ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getSecLabelOffsetOnPkg(): " + Integer::toString(applierCfg.getSecLabelOffsetOnPkg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getSecWandCalibrationOffset(): " + Integer::toString(applierCfg.getSecWandCalibrationOffset() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriRotationCalibrationOffset_90Deg(): " + Integer::toString(applierCfg.getPriRotationCalibrationOffset_90Deg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriRotationCalibrationOffset_180Deg(): " + Integer::toString(applierCfg.getPriRotationCalibrationOffset_180Deg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriRotationCalibrationOffset_270Deg(): " + Integer::toString(applierCfg.getPriRotationCalibrationOffset_270Deg() ) );
            
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getPriApplierHandAdjust(): " + Integer::toString(applierCfg.getPriApplierHandAdjust() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "applierCfg.getSecApplierHandAdjust(): " + Integer::toString(applierCfg.getSecApplierHandAdjust() ) );

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 8;
            canMsg.data[0] = (unsigned char)(WrPermanent);
            canMsg.data[1] = getLsbFromShort( sealerHeaterCfg.getMaxTemp() );
            canMsg.data[2] = getMsbFromShort( sealerHeaterCfg.getMaxTemp() );
            canMsg.data[3] = getLsbFromShort( sealerHeaterCfg.getMinTemp() );
            canMsg.data[4] = sealerHeaterCfg.isItUsed();
            canMsg.data[5] = filmHeaterCfg.isItUsed();
            canMsg.data[6] = applierCfg.getSideDischargeType();
            canMsg.data[7] = getLsbFromShort( wrGenCfg.getUpperFilmPullTension() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolConfig: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent) + 1;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getLsbFromShort( sealerHeaterCfg.getDefaultTempSetting() );
            canMsg.data[1] = getMsbFromShort( sealerHeaterCfg.getDefaultTempSetting() );
            canMsg.data[2] = getLsbFromShort( filmHeaterCfg.getMaxTemp() );
            canMsg.data[3] = getMsbFromShort( filmHeaterCfg.getMaxTemp() );
            canMsg.data[4] = getLsbFromShort( filmHeaterCfg.getMinTemp() );
            canMsg.data[5] = getLsbFromShort( filmHeaterCfg.getDefaultTempSetting() );
            canMsg.data[6] = getMsbFromShort( filmHeaterCfg.getDefaultTempSetting() );
            canMsg.data[7] = getLsbFromShort( wrGenCfg.getLowerFilmPullTension() );

            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolConfig: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent) + 2;
            canMsg.can_dlc = 8;
            canMsg.data[0] = getLsbFromShort( topFilm.getRollWidth() );
            canMsg.data[1] = getMsbFromShort( topFilm.getRollWidth() );
            canMsg.data[2] = getLsbFromShort( bottomFilm.getRollWidth() );
            canMsg.data[3] = getMsbFromShort( bottomFilm.getRollWidth() );
            canMsg.data[4] = applierCfg.doesHaveAutoRotation();
            canMsg.data[5] = (unsigned char)wrGenCfg.getHardwareAddOn();
            canMsg.data[6] = (unsigned char)applierCfg.getPriLabelOffsetOnPkg();
            canMsg.data[7] = (unsigned char)applierCfg.getSecLabelOffsetOnPkg();
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolConfig: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent) + 3;
            canMsg.can_dlc = 8;
            canMsg.data[0] = (unsigned char)applierCfg.getPriLabelRotationDefault();
            canMsg.data[1] = getLsbFromShort( applierCfg.getPriLabelSizeDefault() );
            canMsg.data[2] = getMsbFromShort( applierCfg.getPriLabelSizeDefault() );
            canMsg.data[3] = getLsbFromShort((LabelWidth)applierCfg.getPriLabelWidth());
            canMsg.data[4] = getLsbFromShort( applierCfg.getPriLabelLocationDefault() );
            canMsg.data[5] = (unsigned char)applierCfg.getPriRotationCalibrationOffset();
            canMsg.data[6] = (unsigned char)applierCfg.getPriWandCalibrationOffset();
            canMsg.data[7] = wrGenCfg.getSealerBeltPackageSensorType();
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolConfig: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent) + 4;
            canMsg.can_dlc = 7;
            canMsg.data[0] = (unsigned char)applierCfg.getSecLabelRotationDefault();
            canMsg.data[1] = getLsbFromShort( applierCfg.getSecLabelSizeDefault() );
            canMsg.data[2] = getMsbFromShort( applierCfg.getSecLabelSizeDefault() );
            canMsg.data[3] = getLsbFromShort( (LabelWidth)applierCfg.getSecLabelWidth() );
            canMsg.data[4] = getLsbFromShort( applierCfg.getSecLabelLocationDefault()  );
            canMsg.data[5] = (unsigned char)applierCfg.getSecRotationCalibrationOffset();
            canMsg.data[6] = (unsigned char)applierCfg.getSecWandCalibrationOffset();
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolConfig: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent) + 5;
            canMsg.can_dlc = 8;
            canMsg.data[0] = (unsigned char)applierCfg.getPriRotationCalibrationOffset_90Deg();
            canMsg.data[1] = (unsigned char)applierCfg.getPriRotationCalibrationOffset_180Deg();
            canMsg.data[2] = (unsigned char)applierCfg.getPriRotationCalibrationOffset_270Deg();
            canMsg.data[3] = (unsigned char)applierCfg.getSecRotationCalibrationOffset_90Deg();
            canMsg.data[4] = (unsigned char)applierCfg.getSecRotationCalibrationOffset_180Deg();
            canMsg.data[5] = (unsigned char)applierCfg.getSecRotationCalibrationOffset_270Deg();
            canMsg.data[6] = (unsigned char)applierCfg.getPriApplierHandAdjust();
            canMsg.data[7] = (unsigned char)applierCfg.getSecApplierHandAdjust();
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolConfig. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolConfig: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;

        }
        case WrEvolWrap: {
        	Debug::trace( "CanTranslator", "Send CAN Message: WrEvolWrap" );
            const EvolWrapperWrap ww(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( ww.getIntakeMode()  );
            canMsg.data[1] = getLsbFromShort( ww.getIntakeMode() );
            canMsg.data[2] = getMsbFromShort( ww.getPackageId() );
            canMsg.data[3] = getLsbFromShort( ww.getPackageId() );
            canMsg.data[4] = getMsbFromShort( ww.isPullFilm() );
            canMsg.data[5] = getLsbFromShort( ww.isPullFilm() );
            canMsg.data[6] = getMsbFromShort( ww.getPackageWeight() );
            canMsg.data[7] = getLsbFromShort( ww.getPackageWeight() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolWrap. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                } else {
                	//AWS3 uses two frame id's for this single message.
                	if( ( mySystemType == "AWS3" ) || ( mySystemType == "NGW1" ) ||
                		( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" )) {
						canMsg.can_id = 0x531;
						canMsg.can_dlc = 4;
						canMsg.data[0] = getMsbFromLong( ww.getPluNumber()  );
						canMsg.data[1] = getCharFromLong( ww.getPluNumber(), 2 );
						canMsg.data[2] = getCharFromLong( ww.getPluNumber(), 1 );
						canMsg.data[3] = getLsbFromLong( ww.getPluNumber()  );
						nBytes = pCanManager->send(&canMsg);
						if(nBytes != sizeof(canMsg) ) {
							Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolWrap. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
						}
                	}
                	else {
                        canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
						canMsg.can_dlc = 4;
						canMsg.data[0] = getMsbFromLong( ww.getPluNumber()  );
						canMsg.data[1] = getCharFromLong( ww.getPluNumber(), 2 );
						canMsg.data[2] = getCharFromLong( ww.getPluNumber(), 1 );
						canMsg.data[3] = getLsbFromLong( ww.getPluNumber()  );
						nBytes = pCanManager->send(&canMsg);
						if(nBytes != sizeof(canMsg) ) {
							Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolWrap. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
						}
                	}
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolWrap: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrEvolCycle: {
            const EvolWrapperCycle wc(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 2;
            if (wc.isPullFilm() == true) {
                canMsg.data[0] = getMsbFromShort( 1 );
                canMsg.data[1] = getLsbFromShort( 1 );
            } else {
                canMsg.data[0] = getMsbFromShort( 0 );
                canMsg.data[1] = getLsbFromShort( 0 );
            }

            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolCycle. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolCycle: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrMode: {
        	Debug::trace( "CanTranslator", "Send CAN Message: WrMode" );
            const WrapperMode wrMode(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 1;
            canMsg.data[0] =  (unsigned char)wrMode.getWrOpMode();
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrMode. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrMode: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrEvolControl: {
        	Debug::trace( "CanTranslator", "Send CAN Message: WrEvolControl" );

            const EvolWrapperControl wrControl(recvBuffer);


            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( wrControl.getSealerBeltTempSetting() );
            canMsg.data[1] = getLsbFromShort( wrControl.getSealerBeltTempSetting() );
            canMsg.data[2] = getMsbFromShort( wrControl.getSealerBeltHeaterEnabled() );
            canMsg.data[3] = getLsbFromShort( wrControl.getSealerBeltHeaterEnabled() );
            canMsg.data[4] = getMsbFromShort( wrControl.getFilmWidthSelection() );
            canMsg.data[5] = getLsbFromShort( wrControl.getFilmWidthSelection() );
            canMsg.data[6] = getMsbFromShort( wrControl.getFilmTension() );
            canMsg.data[7] = getLsbFromShort( wrControl.getFilmTension() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolControl. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolControl: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
        	//AWS3 uses two frame id's for this single message.
        	if( ( mySystemType == "AWS3" ) || ( mySystemType == "NGW1" ) ||
        	    ( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" ) ) {
        		canMsg.can_id = 0x53B;
        	} else {
        		canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
        	}
            canMsg.can_dlc = 6;
            canMsg.data[0] = getMsbFromShort( wrControl.getFilmTensionFrontToBack() );
            canMsg.data[1] = getLsbFromShort( wrControl.getFilmTensionFrontToBack() );
            canMsg.data[2] = getMsbFromShort( wrControl.getFilmTensionRightToLeft() );
            canMsg.data[3] = getLsbFromShort( wrControl.getFilmTensionRightToLeft() );
            canMsg.data[4] = getMsbFromShort( wrControl.getIntakeMode() );
            canMsg.data[5] = getLsbFromShort( wrControl.getIntakeMode() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolControl. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolControl: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrEvolTest: {
            const EvolWrapperTest wrTest(recvBuffer);

        	Debug::trace( "CanTranslator", "Send CAN Message: EvolWrapperTest " + Integer::toString( wrTest.getTestCode() ) + " " +
        			Integer::toString( wrTest.getDataInt1() ) + " " + Integer::toString( wrTest.getDataInt2() ) );

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 5;
            canMsg.data[0] = wrTest.getTestCode();
            canMsg.data[1] = getLsbFromShort( wrTest.getDataInt1()  );
            canMsg.data[2] = getMsbFromShort( wrTest.getDataInt1() );
            canMsg.data[3] = getLsbFromShort( wrTest.getDataInt2() );
            canMsg.data[4] = getMsbFromShort( wrTest.getDataInt2()  );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolTest. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolTest: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrEvolSaveCycleCount: {
            const EvolWrapperSaveCycleCount saveCycleCount(recvBuffer);


            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 4;
            canMsg.data[0] = getMsbFromLong( saveCycleCount.getWrapperCycleCount() );
            canMsg.data[1] = getCharFromLong( saveCycleCount.getWrapperCycleCount(), 2 );
            canMsg.data[2] = getCharFromLong( saveCycleCount.getWrapperCycleCount(), 1 );
            canMsg.data[3] = getLsbFromLong( saveCycleCount.getWrapperCycleCount() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolSaveCycleCount. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolSaveCycleCount: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrEvolSystemInfoWrite: {

        	Debug::trace( "CanTranslator.Translate", "translateWrapperEvent(): EvolWrapperSystemInfoWrite:" );


            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);

            if ( !canMsg.can_id )
            {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolSystemInfoWrite: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
                break;
            }

			const EvolWrapperSystemInfoWrite systemInfoWrite( recvBuffer );

			Debug::trace( "CanTranslator.Translate", "translateWrapperEvent(): EvolWrapperSystemInfoWrite getAwsSerialNumber(): " + systemInfoWrite.getAwsSerialNumber() );
			Debug::trace( "CanTranslator.Translate", "translateWrapperEvent(): EvolWrapperSystemInfoWrite getAwsDateOfManufacture(): " + systemInfoWrite.getAwsDateOfManufacture() );
			Debug::trace( "CanTranslator.Translate", "translateWrapperEvent(): EvolWrapperSystemInfoWrite getAwsDateOfInstall(): " + systemInfoWrite.getAwsDateOfInstall() );
			Debug::trace( "CanTranslator.Translate", "translateWrapperEvent(): EvolWrapperSystemInfoWrite getModelNumber(): " + Integer::toString(systemInfoWrite.getModelNumber()));

			char serial[32];
			memset( serial, 0, sizeof(serial) );
			strncpy( serial, systemInfoWrite.getAwsSerialNumber().c_str(), sizeof(serial) );

			char dateOfMan[32];
			memset( dateOfMan, 0, sizeof(dateOfMan) );
			strncpy( dateOfMan, systemInfoWrite.getAwsDateOfManufacture().c_str(), sizeof(dateOfMan) );

			char dateOfIns[32];
			memset( dateOfIns, 0, sizeof(dateOfIns) );
			strncpy( dateOfIns, systemInfoWrite.getAwsDateOfInstall().c_str(), sizeof(dateOfIns) );

			ULONG cycleCount( systemInfoWrite.getAwsCycleCount() );

			// ---- //

			canMsg.can_dlc = 8;
			canMsg.data[0] = serial[0];
			canMsg.data[1] = serial[1];
			canMsg.data[2] = serial[2];
			canMsg.data[3] = serial[3];
			canMsg.data[4] = serial[4];
			canMsg.data[5] = serial[5];
			canMsg.data[6] = serial[6];
			canMsg.data[7] = serial[7];

			if ( sizeof(canMsg) != (nBytes = pCanManager->send(&canMsg)) )
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolSystemInfoWrite. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );

			if ( ( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" ) )
				++canMsg.can_id;

			// ---- //

			canMsg.can_dlc = 8;
			canMsg.data[0] = serial[8];
			canMsg.data[1] = serial[9];
			canMsg.data[2] = serial[10];
			canMsg.data[3] = serial[11];
			canMsg.data[4] = dateOfMan[0];
			canMsg.data[5] = dateOfMan[1];
			canMsg.data[6] = dateOfMan[2];
			canMsg.data[7] = dateOfMan[3];

			if ( sizeof(canMsg) != (nBytes = pCanManager->send(&canMsg)) )
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolSystemInfoWrite. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );

			if ( ( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" ) )
				++canMsg.can_id;

			// ---- //

			canMsg.can_dlc = 8;
			canMsg.data[0] = dateOfMan[4];
			canMsg.data[1] = dateOfMan[5];
			canMsg.data[2] = dateOfIns[0];
			canMsg.data[3] = dateOfIns[1];
			canMsg.data[4] = dateOfIns[2];
			canMsg.data[5] = dateOfIns[3];
			canMsg.data[6] = dateOfIns[4];
			canMsg.data[7] = dateOfIns[5];

			if ( sizeof(canMsg) != (nBytes = pCanManager->send(&canMsg)) )
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolSystemInfoWrite. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );

			if ( ( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" ) )
				++canMsg.can_id;

			// ---- //

			canMsg.can_dlc = 6;
			canMsg.data[0] = getLsbFromShort( systemInfoWrite.getModelNumber() );
			canMsg.data[1] = getMsbFromShort( systemInfoWrite.getModelNumber() );

			canMsg.data[5] = cycleCount & 0xFF;
			cycleCount >>= 8;

			canMsg.data[4] = cycleCount & 0xFF;
			cycleCount >>= 8;

			canMsg.data[3] = cycleCount & 0xFF;
			cycleCount >>= 8;

			canMsg.data[2] = cycleCount & 0xFF;
			cycleCount >>= 8;

			if ( sizeof(canMsg) != (nBytes = pCanManager->send(&canMsg)) )
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolSystemInfoWrite. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );

			if ( ( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" ) )
				++canMsg.can_id;

            break;
        }
        case WrLabelReady: {
        	Debug::trace( "CanTranslator", "Send CAN Message: WrLabelReady" );
            const NGWLabelReady lr(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 4;
            canMsg.data[0] = getMsbFromShort( lr.getStationId() );
            canMsg.data[1] = getLsbFromShort( lr.getStationId() );
            canMsg.data[2] = getMsbFromShort( lr.getPackageId() );
            canMsg.data[3] = getLsbFromShort( lr.getPackageId() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message NGWLabelReady. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for NGWLabelReady: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrLabelTaken: {
        	Debug::trace( "CanTranslator", "Send CAN Message: WrLabelTaken" );
        	const NGWLabelTaken lt(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 4;
            canMsg.data[0] = getMsbFromShort( lt.getStationId() );
            canMsg.data[1] = getLsbFromShort( lt.getStationId() );
            canMsg.data[2] = getMsbFromShort( lt.getPackageId() );
            canMsg.data[3] = getLsbFromShort( lt.getPackageId() );
            if(canMsg.can_id != 0) {
                nBytes = pCanManager->send(&canMsg);
                if(nBytes != sizeof(canMsg) ) {
                    Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message NGWLabelTaken. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                }
            } else {
                Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for NGWLabelTaken: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
            }
            break;
        }
        case WrLabelSetup: {
        	//AWS3 uses two frame id's for this single message.
        	if( ( mySystemType == "AWS3" ) || (  mySystemType == "NGW1"  )) {
        		Debug::trace( "CanTranslator", "Send CAN Message: WrLabelSetup" );
				const NGWLabelSetup ls(recvBuffer);

				canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
				canMsg.can_dlc = 8;
				canMsg.data[0] = getMsbFromShort( ls.getStationId() );
				canMsg.data[1] = getLsbFromShort( ls.getStationId() );
				canMsg.data[2] = getMsbFromShort( ls.getLabelSize() );
				canMsg.data[3] = getLsbFromShort( ls.getLabelSize() );
				canMsg.data[4] = getMsbFromShort( ls.getLabelRotation() );
				canMsg.data[5] = getLsbFromShort( ls.getLabelRotation() );
				canMsg.data[6] = getMsbFromShort( ls.getLabelLocation() );
				canMsg.data[7] = getLsbFromShort( ls.getLabelLocation() );

				if(canMsg.can_id != 0) {
					nBytes = pCanManager->send(&canMsg);
					if(nBytes != sizeof(canMsg) ) {
						Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrLabelSetup. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
					}
				} else {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrLabelSetup: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
				}
           	}
            break;
        }
        case WrEvolWriteCameraThreshold: {
        	Debug::trace( "CanTranslator", "Send CAN Message: EvolWrapperWriteCameraThreshold" );
        	const EvolWrapperWriteCameraThreshold threshold(recvBuffer);

			canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
			canMsg.can_dlc = 2;
			canMsg.data[0] = 0;
			canMsg.data[1] = threshold.getWrapperWriteCameraThreshold();
			if(canMsg.can_id != 0) {
				nBytes = pCanManager->send(&canMsg);
				if(nBytes != sizeof(canMsg) ) {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolWriteCameraThreshold. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
				}
			} else {
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolWriteCameraThreshold: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
			}
        	break;
        }
        case WrEvolDiagnosticsTest: {
        	Debug::trace( "CanTranslator", "Send CAN Message: WrEvolDiagnosticsTest" );
        	const EvolWrapperDiagnosticsTest test(recvBuffer);

			canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
			canMsg.can_dlc = 2;
			canMsg.data[0] = getLsbFromShort( test.getTest() );
			canMsg.data[1] = getMsbFromShort( test.getTest() );
			if(canMsg.can_id != 0) {
				nBytes = pCanManager->send(&canMsg);
				if(nBytes != sizeof(canMsg) ) {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolDiagnosticsTest. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
				}
			} else {
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolDiagnosticsTest: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
			}
			break;
        }
        case WrWrapBoxAck: {
        	Debug::trace( "CanTranslator", "Send CAN Message: WrWrapBoxAck" );

			canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
			canMsg.can_dlc = 1;
			canMsg.data[0] = true;
			if(canMsg.can_id != 0) {
				nBytes = pCanManager->send(&canMsg);
				if(nBytes != sizeof(canMsg) ) {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrWrapBoxAck. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
				}
			} else {
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrWrapBoxAck: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
			}
        	break;
        }
        case WrWrapBoxSizeSave: {
        	Debug::trace( "CanTranslator", "Send CAN Message: WrWrapBoxSizeSave" );
        	const WrapBoxSize msgSize(recvBuffer);


			canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
			canMsg.can_dlc = 2;
			canMsg.data[0] = getMsbFromShort( msgSize.getSize() );
			canMsg.data[1] = getLsbFromShort( msgSize.getSize() );
			if(canMsg.can_id != 0) {
				nBytes = pCanManager->send(&canMsg);
				if(nBytes != sizeof(canMsg) ) {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrWrapBoxSizeSave. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
				}
			} else {
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrWrapBoxSizeSave: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
			}
        	break;
        }
        case WrEvolPackageTimingsWrite: {
        	Debug::trace( "CanTranslator.Timings", "Send CAN Message: translateWrapperEvent() case WrEvolPackageTimingsWrite" );
			const EvolWrapperPackageTimingsPointer wrTimingsPtr(recvBuffer);
			const EvolWrapperPackageTimings *pTimings( wrTimingsPtr.getTimingsPointer() );

			if( pTimings != NULL ) {
				//get the number of wrapboxes we are sending.
				int numWrapBoxes = pTimings->getNumberWrapBoxes();
				int numHeightBoxes = 0;
				for ( int i = 0; i < numWrapBoxes; ++i )
				{
					numHeightBoxes += pTimings->getWrapBox(i)->getNumberHeightBoxes();
				}

				Debug::trace( "CanTranslator.Timings", "translateWrapperEvent() case WrEvolPackageTimingsWrite numHeightBoxes: " + Integer::toString(numHeightBoxes) );

				// write the start of the wrapboxes

				canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
				canMsg.can_dlc = 8;
				canMsg.data[0] = getLsbFromShort( EvolWrapperPackageTimings::_WrSettingsWrite_ );
				canMsg.data[1] = getLsbFromShort( (unsigned short)pTimings->getDisposition() );
				canMsg.data[2] = getLsbFromShort( numWrapBoxes );

				Debug::trace( "CanTranslator", "Write Start of wrapbox data" );
				if( numWrapBoxes >= 1 ) {
					canMsg.data[3] = getLsbFromShort( pTimings->getWrapBox(0)->getNumberHeightBoxes() );
				} else {
					canMsg.data[3] = 0;
				}
				if( numWrapBoxes >= 2 ) {
					canMsg.data[4] = getLsbFromShort( pTimings->getWrapBox(1)->getNumberHeightBoxes() );
				} else {
					canMsg.data[4] = 0;
				}
				if( numWrapBoxes >= 3 ) {
					canMsg.data[5] = getLsbFromShort( pTimings->getWrapBox(2)->getNumberHeightBoxes() );
				} else {
					canMsg.data[5] = 0;
				}
				if( numWrapBoxes >= 4 ) {
					canMsg.data[6] = getLsbFromShort( pTimings->getWrapBox(3)->getNumberHeightBoxes() );
				} else {
					canMsg.data[6] = 0;
				}
				if( numWrapBoxes >= 5 ) {
					canMsg.data[7] = getLsbFromShort( pTimings->getWrapBox(4)->getNumberHeightBoxes() );
				} else {
					canMsg.data[7] = 0;
				}
				// 10mS sleep to eliminate transmit error.
				PAL_absclass::PAL_sleep( 10 );

				if(canMsg.can_id != 0) {
					nBytes = pCanManager->send(&canMsg);
					if(nBytes != sizeof(canMsg) ) {
						Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. " + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + " bytes of: " + Integer::toString( sizeof(canMsg)) );
					}
				} else {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolPackageTimingsWrite: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
				}

				canMsg.can_id = ( pCanLibrary->getWrapperCobID(wrEvent) + 1 );
				if( numWrapBoxes >= 6 ) {
					canMsg.data[0] = getLsbFromShort( pTimings->getWrapBox(5)->getNumberHeightBoxes() );
				} else {
					canMsg.data[0] = 0 ;
				}
				if( numWrapBoxes >= 7 ) {
					canMsg.data[1] = getLsbFromShort( pTimings->getWrapBox(6)->getNumberHeightBoxes() );
				} else {
					canMsg.data[1] = 0;
				}
				if( numWrapBoxes >= 8 ) {
					canMsg.data[2] = getLsbFromShort( pTimings->getWrapBox(7)->getNumberHeightBoxes() );
				} else {
					canMsg.data[2] = 0;
				}
				if( numWrapBoxes >= 9 ) {
					canMsg.data[3] = getLsbFromShort( pTimings->getWrapBox(8)->getNumberHeightBoxes() );
				} else {
					canMsg.data[3] = 0;
				}
				if( numWrapBoxes >= 10 ) {
					canMsg.data[4] = getLsbFromShort( pTimings->getWrapBox(9)->getNumberHeightBoxes() );
				} else {
					canMsg.data[4] = 0;
				}
				if( numWrapBoxes >= 11 ) {
					canMsg.data[5] = getLsbFromShort( pTimings->getWrapBox(10)->getNumberHeightBoxes() );
				} else {
					canMsg.data[5] = 0;
				}
				if( numWrapBoxes >= 12 ) {
					canMsg.data[6] = getLsbFromShort( pTimings->getWrapBox(11)->getNumberHeightBoxes() );
				} else {
					canMsg.data[6] = 0;
				}
				if( numWrapBoxes >= 13 ) {
					canMsg.data[7] = getLsbFromShort( pTimings->getWrapBox(12)->getNumberHeightBoxes() );
				} else {
					canMsg.data[7] = 0;
				}

				// 10mS sleep to eliminate transmit error.
				PAL_absclass::PAL_sleep( 10 );

				if(canMsg.can_id != 0) {
					nBytes = pCanManager->send(&canMsg);
					if(nBytes != sizeof(canMsg) ) {
						Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite."  + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
					}
				} else {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolPackageTimingsWrite: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
				}


				canMsg.can_id = ( pCanLibrary->getWrapperCobID(wrEvent) + 2 );
				if( numWrapBoxes >= 14 ) {
					canMsg.data[0] = getLsbFromShort( pTimings->getWrapBox(13)->getNumberHeightBoxes() );
				} else {
					canMsg.data[0] = 0;
				}
				if( numWrapBoxes >= 15 ) {
					canMsg.data[1] =  getLsbFromShort( pTimings->getWrapBox(14)->getNumberHeightBoxes() );
				} else {
					canMsg.data[1] = 0;
				}
				if( numWrapBoxes >= 16 ) {
					canMsg.data[2] = getLsbFromShort( pTimings->getWrapBox(15)->getNumberHeightBoxes() );
				} else {
					canMsg.data[2] = 0;
				}
				if( numWrapBoxes >= 17 ) {
					canMsg.data[3] = getLsbFromShort( pTimings->getWrapBox(16)->getNumberHeightBoxes() );
				} else {
					canMsg.data[3] = 0;
				}
				if( numWrapBoxes >= 18 ) {
					canMsg.data[4] = getLsbFromShort( pTimings->getWrapBox(17)->getNumberHeightBoxes() );
				} else {
					canMsg.data[4] = 0;
				}
				if( numWrapBoxes >= 19 ) {
					canMsg.data[5] = getLsbFromShort( pTimings->getWrapBox(18)->getNumberHeightBoxes() );
				} else {
					canMsg.data[5] = 0;
				}
				if( numWrapBoxes >= 20 ) {
					canMsg.data[6] = getLsbFromShort( pTimings->getWrapBox(19)->getNumberHeightBoxes() );
				} else {
					canMsg.data[6] = 0;
				}
				if( numWrapBoxes >= 21 ) {
					canMsg.data[7] = getLsbFromShort( pTimings->getWrapBox(20)->getNumberHeightBoxes() );
				} else {
					canMsg.data[7] = 0;
				}

				// 10mS sleep to eliminate transmit error.
				PAL_absclass::PAL_sleep( 10 );

				if(canMsg.can_id != 0) {
					nBytes = pCanManager->send(&canMsg);
					if(nBytes != sizeof(canMsg) ) {
						Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. "  + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
					}
				} else {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolPackageTimingsWrite: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
				}


				canMsg.can_id = ( pCanLibrary->getWrapperCobID(wrEvent) + 3 );
				canMsg.can_dlc = 4;
				if( numWrapBoxes >= 22 ) {
					canMsg.data[0] = getLsbFromShort( pTimings->getWrapBox(21)->getNumberHeightBoxes() );
				} else {
					canMsg.data[0] = 0;
				}
				if( numWrapBoxes >= 23 ) {
					canMsg.data[1] =  getLsbFromShort( pTimings->getWrapBox(22)->getNumberHeightBoxes() );
				} else {
					canMsg.data[1] = 0;
				}
				if( numWrapBoxes >= 24 ) {
					canMsg.data[2] = getLsbFromShort( pTimings->getWrapBox(23)->getNumberHeightBoxes() );
				} else {
					canMsg.data[2] = 0;
				}
				if( numWrapBoxes == 25 ) {
					canMsg.data[3] = getLsbFromShort( pTimings->getWrapBox(24)->getNumberHeightBoxes() );
				} else {
					canMsg.data[3] = 0;
				}

				// 10mS sleep to eliminate transmit error.
				PAL_absclass::PAL_sleep( 10 );

				if(canMsg.can_id != 0) {
					nBytes = pCanManager->send(&canMsg);
					if(nBytes != sizeof(canMsg) ) {
						Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. "  + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
					}
				} else {
					Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrEvolPackageTimingsWrite: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
				}

				Debug::trace( "CanTranslator", "Send Wrap Box data:  " + Integer::toString(numWrapBoxes) );
				//send all wrapbox data
				for( int i = 0; i < numWrapBoxes; i++ ) {

					const WrapBox *pBox( pTimings->getWrapBox(i) );

					canMsg.can_id = 0x5f7;
					canMsg.can_dlc = 8;

					if( pBox != NULL ){
						const std::string name = pBox->getName();
						if(name.size() >= 1 )
							canMsg.data[0] = name[0];
						else
							canMsg.data[0] = ' ';
						if(name.size() >= 2 )
							canMsg.data[1] = name[1];
						else
							canMsg.data[1] = ' ';
						if(name.size() >= 3 )
							canMsg.data[2] = name[2];
						else
							canMsg.data[2] = ' ';
						if(name.size() >= 4 )
							canMsg.data[3] = name[3];
						else
							canMsg.data[3] = ' ';
						if(name.size() >= 5 )
							canMsg.data[4] = name[4];
						else
							canMsg.data[4] = ' ';
						if(name.size() >= 6 )
							canMsg.data[5] = name[5];
						else
							canMsg.data[5] = ' ';
						if( name.size() >= 7 )
							canMsg.data[6] = name[6];
						else
							canMsg.data[6] = ' ';
						if( name.size() >= 8 )
							canMsg.data[7] = name[7];
						else
							canMsg.data[7] = ' ';

						// 10mS sleep to eliminate transmit error.
						PAL_absclass::PAL_sleep( 10 );

						nBytes = pCanManager->send(&canMsg);
						if(nBytes != sizeof(canMsg) ) {
							Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. " + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
						}
					} else {
						Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. pBox is NULL! sent  " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
					}

					if( pBox != NULL ){
						const std::string name = pBox->getName();
						canMsg.can_id = 0x5f8;
						canMsg.can_dlc = 7;

						if( name.size() >= 9 )
							canMsg.data[0] = name[8];
						else
							canMsg.data[0] = ' ';
						if( name.size() >= 10 )
							canMsg.data[1] = name[9];
						else
							canMsg.data[1] = ' ';

						canMsg.data[2] = (unsigned char )pBox->getLengthLow();
						canMsg.data[3] = (unsigned char )pBox->getLengthHigh();
						canMsg.data[4] = (unsigned char )pBox->getWidthLow();
						canMsg.data[5] = (unsigned char )pBox->getWidthHigh();
						canMsg.data[6] = (unsigned char )pBox->getLip();

						// 10mS sleep to eliminate transmit error.
						PAL_absclass::PAL_sleep( 10 );

						nBytes = pCanManager->send(&canMsg);
						if(nBytes != sizeof(canMsg) ) {
							Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. " + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
						}
					} else {
						Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. pBox is NULL! sent  " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
					}

					canMsg.can_id = 0x5f9;
					canMsg.can_dlc = 4;
					canMsg.data[0] = getMsbFromLong( pBox->getPLUException() );
					canMsg.data[1] = getCharFromLong( pBox->getPLUException(), 2 );
					canMsg.data[2] = getCharFromLong( pBox->getPLUException(), 1 );
					canMsg.data[3] = getLsbFromLong( pBox->getPLUException() );

					// 10mS sleep to eliminate transmit error.
					PAL_absclass::PAL_sleep( 10 );

					nBytes = pCanManager->send(&canMsg);
					if(nBytes != sizeof(canMsg) ) {
						Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. " + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
					}

				}

				Debug::trace( "CanTranslator.Timings", "Send Height Box data (THIS IS WHAT WE WANT TO SEE #1) numWrapBoxes:  " + Integer::toString(numWrapBoxes) );
				//finally send all height box data
				for( int i = 0; i < numWrapBoxes; i++ ) {

					int numHeightBoxes = pTimings->getWrapBox(i)->getNumberHeightBoxes();
					//Debug::trace("CanTranslator.Timings", "Send Height Box data (THIS IS WHAT WE WANT TO SEE #2) numHeightBoxes:  " + Integer::toString(numHeightBoxes));

					for( int j = 0; j < numHeightBoxes; j++  ) {

						const WrapBox::HeightBox *pHeightBox( pTimings->getWrapBox(i)->getHeightBoxNumber(j) );

						if( pHeightBox != NULL ) {
							canMsg.can_id = 0x5fa;
							canMsg.can_dlc = 8;
							canMsg.data[0] = pHeightBox->getHeight();
							canMsg.data[1] = pHeightBox->getParameter( WrapBox::PERCENT_STRETCH_PARAMETER );
							canMsg.data[2] = pHeightBox->getParameter( WrapBox::CLAMP_HEIGHT_PARAMETER );
							canMsg.data[3] = pHeightBox->getParameter( WrapBox::ELEVATOR_PLACEMENT_PARAMETER );
							canMsg.data[4] = pHeightBox->getParameter( WrapBox::GRIPPER_ADJUST_PARAMETER );
							canMsg.data[5] = pHeightBox->getParameter( WrapBox::WRAP_SPEED_PARAMETER );
							canMsg.data[6] = pHeightBox->getParameter( WrapBox::DISCHARGE_SPEED_PARAMETER );
							canMsg.data[7] = pHeightBox->getParameter( WrapBox::SUF_ADJUST_PARAMETER );
                            //Debug::trace("CanTranslator.Timings", "Height Box data[" + Integer::toString(i) + "] with id: 0x5fa");
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fa canMsg.data[0]:  " + Integer::toString(canMsg.data[0]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fa canMsg.data[1]:  " + Integer::toString(canMsg.data[1]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fa canMsg.data[2]:  " + Integer::toString(canMsg.data[2]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fa canMsg.data[3]:  " + Integer::toString(canMsg.data[3]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fa canMsg.data[4]:  " + Integer::toString(canMsg.data[4]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fa canMsg.data[5]:  " + Integer::toString(canMsg.data[5]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fa canMsg.data[6]:  " + Integer::toString(canMsg.data[6]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fa canMsg.data[7]:  " + Integer::toString(canMsg.data[7]));

							// 10mS sleep to eliminate transmit error.
							PAL_absclass::PAL_sleep( 10 );

							nBytes = pCanManager->send(&canMsg);
							if(nBytes != sizeof(canMsg) ) {
								Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. " + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
							}

							canMsg.can_id = 0x5fb;
                            if (FpmManager::getInstance()->isWrapperTwoPrinter())
                                canMsg.can_dlc = 8;                 // 2P system
                            else
                                canMsg.can_dlc = 6;                 // 1P system

							canMsg.data[0] = pHeightBox->getParameter(  WrapBox::HEIGHT_RESTRICT_PARAMETER );
							canMsg.data[1] = pHeightBox->getParameter( WrapBox::FILM_ROLL_USED_PARAMETER );
							canMsg.data[2] = pHeightBox->getParameter( WrapBox::RUF_TRIP_DELAY_PARAMETER );
							canMsg.data[3] = pHeightBox->getParameter( WrapBox::BRAKE_TIMING_PARAMETER );
							canMsg.data[4] = pHeightBox->getParameter( WrapBox::KNIFE_HEIGHT_PARAMETER );
							canMsg.data[5] = pHeightBox->getParameter( WrapBox::FUF_ADJUST_PARAMETER );
                            //Debug::trace("CanTranslator.Timings", "Height Box data[" + Integer::toString(i) + "] with id: 0x5fb");
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fb canMsg.data[0]:  " + Integer::toString(canMsg.data[0]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fb canMsg.data[1]:  " + Integer::toString(canMsg.data[1]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fb canMsg.data[2]:  " + Integer::toString(canMsg.data[2]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fb canMsg.data[3]:  " + Integer::toString(canMsg.data[3]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fb canMsg.data[4]:  " + Integer::toString(canMsg.data[4]));
                            //Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fb canMsg.data[5]:  " + Integer::toString(canMsg.data[5]));


                            if (FpmManager::getInstance()->isWrapperTwoPrinter())
                            {
								//Debug::trace("CanTranslator.Timings", " ========== START 2P Section ========== " );
                                canMsg.data[6] = pHeightBox->getParameter(WrapBox::ADDED_LENGTH_PARAMETER);
                                canMsg.data[7] = pHeightBox->getParameter(WrapBox::PRIMARY_LABEL_VERT_OFFSET);
								//Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fb canMsg.data[6]:  " + Integer::toString(canMsg.data[6]));
								//Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fb canMsg.data[7]:  " + Integer::toString(canMsg.data[7]));
                                // 10mS sleep to eliminate transmit error.
                                PAL_absclass::PAL_sleep(10);
                                // THIS NEEDS FIXED for 4.2.300 - FIXED 01-10-2023 - USB WILL NEVER call CAN Messages so we better NEVER see this
                                nBytes = pCanManager->send(&canMsg);
                                if (nBytes != sizeof(canMsg)) {
								    Debug::trace("CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. " + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
                                }

                                canMsg.can_id = 0x5fc;			// send to carlos
                                canMsg.can_dlc = 1;
                                canMsg.data[0] = pHeightBox->getParameter(WrapBox::SECONDARY_LABEL_VERT_OFFSET);
								//Debug::trace("CanTranslator.Timings", "Height Box data[" + Integer::toString(i) + "] with id: 0x5fc");
								//Debug::trace("CanTranslator.Timings", "Height Box data id: 0x5fc canMsg.data[0]:  " + Integer::toString(canMsg.data[0]));
                            }
                     
							// 10mS sleep to eliminate transmit error.
							PAL_absclass::PAL_sleep( 10 );

							nBytes = pCanManager->send(&canMsg);
							if(nBytes != sizeof(canMsg) ) {
								Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. " + Hex::toString(canMsg.can_id) + " Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
							}
						} else {
							Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrEvolPackageTimingsWrite. pHeightBox NULL! Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
						}
					}
				}

			} else {
				Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): pTimings is NULL!" );
			}
        	break;
        }
        case WrNGWReqBootStatus:
        {
        	const NGWRequestBootStatus bs(recvBuffer);


            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 1;
            canMsg.data[0] = bs.getNodeId();
        	canMsg.data[1] = 0;
        	canMsg.data[2] = 0;
        	canMsg.data[3] = 0;
        	canMsg.data[4] = 0;
        	canMsg.data[5] = 0;
        	canMsg.data[6] = 0;
        	canMsg.data[7] = 0;

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWReqBootStatus. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWReqBootStatus: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }

        	break;
        }
        case WrNGWBootCMD:
        {
        	const NGWBootNodeCMD cmd(recvBuffer);


        	canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
        	canMsg.can_dlc = 3;
        	canMsg.data[0] = cmd.getNodeId();
        	canMsg.data[1] = cmd.getCommand();
        	canMsg.data[2] = cmd.getAction();
        	canMsg.data[3] = 0;
        	canMsg.data[4] = 0;
        	canMsg.data[5] = 0;
        	canMsg.data[6] = 0;
        	canMsg.data[7] = 0;

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWBootCMD. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWBootCMD: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }
        	break;
        }
        case WrNGWBootWriteAppFlash:
        {

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);

        	break;
        }
        case WrNGWRequestBootVersion:
        {
            const NGWRequestBootVersion bv(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);
            canMsg.can_dlc = 8;
            canMsg.data[0] = bv.getCommand();
            canMsg.data[1] = bv.getSourceId();
            canMsg.data[2] = bv.getNodeId();
            canMsg.data[3] = 0;
            canMsg.data[4] = 0;
            canMsg.data[5] = 0;
            canMsg.data[6] = 0;
            canMsg.data[7] = 0;

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWRequestBootVersion. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWRequestBootVersion: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }
        	break;
        }
        case WrNGWBootFrame:
        {
        	const NGWBootFrame bf(recvBuffer);

        	canMsg.can_dlc = bf.getPayloadLength();

        	switch( bf.getFrameNumber() )
        	{
        		case NGWBootFrame::_CanHeaderFrame:
        		{
        			Debug::trace( "CanTranslator", "Sending header.....");
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame);
        			break;
        		}
        		case NGWBootFrame::_CanFrame1:
        		{
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame1);
        			break;
        		}
        		case NGWBootFrame::_CanFrame2:
        		{
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame2);
        			break;
        		}
        		case NGWBootFrame::_CanFrame3:
        		{
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame3);
        			break;
        		}
        		case NGWBootFrame::_CanFrame4:
        		{
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame4);
        			break;
        		}
        		case NGWBootFrame::_CanFrame5:
        		{
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame5);
        			break;
        		}
        		case NGWBootFrame::_CanFrame6:
        		{
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame6);
        			break;
        		}
        		case NGWBootFrame::_CanFrame7:
        		{
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame7);
        			break;
        		}
        		case NGWBootFrame::_CanFrame8:
        		{
        			canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame8);
        			break;
        		}
        	}

        	canMsg.data[0] = bf.getFrameData(0);
			canMsg.data[1] = bf.getFrameData(1);
			canMsg.data[2] = bf.getFrameData(2);
			canMsg.data[3] = bf.getFrameData(3);
			canMsg.data[4] = bf.getFrameData(4);
			canMsg.data[5] = bf.getFrameData(5);
			canMsg.data[6] = bf.getFrameData(6);
			canMsg.data[7] = bf.getFrameData(7);

			if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWBootFrame. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWBootFrame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }
        	break;
        }
        case WrNGWBulkFrame:
        {
        	const NGWBulkTransfer bulkMsg(recvBuffer);


        	/* send the header frame first */
        	canMsg.can_id = pCanLibrary->getWrapperCobID(WrNGWBootFrame);
        	canMsg.can_dlc = 8;
        	canMsg.data[0] = bulkMsg.getCommand();
        	canMsg.data[1] = bulkMsg.getSourceId();
        	canMsg.data[2] = bulkMsg.getNodeId();
        	canMsg.data[3] = bulkMsg.getNumFrames();
        	canMsg.data[4] = bulkMsg.getLength();
			canMsg.data[5] = 0;
			canMsg.data[6] = 0;
			canMsg.data[7] = 0;
			if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWBulkFrame Header. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWBulkFrame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }

			int k = 0;
        	for( int i = 0; i < bulkMsg.getNumFrames(); i++ )
        	{
        		canMsg.can_id = pCanLibrary->getWrapperCobID( static_cast< WrEvent >(WrNGWBootFrame + (i + 1)) );
        		for( int j = 0; j < 8; j++)
        		{
        			canMsg.data[j] = bulkMsg.getData(k++);
        		}

        		if(canMsg.can_id != 0) {
                     nBytes = pCanManager->send(&canMsg);
                     if(nBytes != sizeof(canMsg) ) {
                         Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWBulkFrame. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                     }
                 } else {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWBulkFrame: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
                 }
        	}
        	break;
        }
        case WrNGWAck:
        {
           	const NGWAck ack(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);

            canMsg.can_dlc = 2;
            canMsg.data[0] = ack.getNodeId();
            canMsg.data[1] = ack.getSourceId();
            canMsg.data[2] = 0;
            canMsg.data[3] = 0;
            canMsg.data[4] = 0;
            canMsg.data[5] = 0;
            canMsg.data[6] = 0;
            canMsg.data[7] = 0;

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWAck. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWAck: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }
        	break;
        }
        case WrNGWFlush:
        {
           	const NGWFlush flush(recvBuffer);

            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);

            canMsg.can_dlc = 2;
            canMsg.data[0] = flush.getNodeId();
            canMsg.data[1] = flush.getSourceId();
            canMsg.data[2] = 0;
            canMsg.data[3] = 0;
            canMsg.data[4] = 0;
            canMsg.data[5] = 0;
            canMsg.data[6] = 0;
            canMsg.data[7] = 0;

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWFlush. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWFlush: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }
        	break;
        }
        case WrNGWWrapperDetached: {
        	pWrapperManager = NULL;
        	pLabelApplier = NULL;
            /*
            if (!pKPCBootManager)
            {
                Debug::trace("CanTranslator.info", "translateWeigherEvent(): Creating Boot Manager Instance ");
                pKPCBootManager = KPCBootManager::getInstance();
            }
            */
        	break;
        }
        case WrPPPRequest: {
            Debug::trace( "CanTranslator", "Sending WrPPPRequest!");
            PPPRequest req( recvBuffer );
            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);

            canMsg.can_dlc = 3;
            canMsg.data[0] = req.getRequest();
            canMsg.data[1] = getMsbFromShort( req.getData() );
            canMsg.data[2] = getLsbFromShort( req.getData() );

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrPPPRequests. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrPPPRequests: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }
            break;
        }
        case WrWrapperPause: {
			// SOF-5459 Printer Plate Adjust Message - Do not need this for a single sliding 2nd printer so can it all
			Debug::trace("CanTranslator.Error", "translateWrapperEvent(): case WrWrapperPause: probably should not be getting this event any more " );
            /*
            Debug::trace( "CanTranslator", "Sending WrWrapperPause!");
            NGWWrapperPause msg( recvBuffer );
            canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);

            canMsg.can_dlc = 6;
            canMsg.data[0] = msg.getState();
            canMsg.data[1] = msg.getPackageId();
            canMsg.data[2] = msg.getPrimaryPlatePosition();
            canMsg.data[3] = msg.getRequiredPrimaryPlatePosition();
            canMsg.data[4] = msg.getSecondaryPlatePosition();
            canMsg.data[5] = msg.getRequiredSecondaryPlatePosition();

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrWrapperPause. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrWrapperPause: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }
            */
        	break;
        }
        case WrNGWDimensionOverrides:
        {
           
           NGWDimensionOverrides overrides(recvBuffer);
           
           Debug::trace( "CanTranslator", "Sending WrNGWDimensionOverrides!" + HobString(Integer::toString(overrides.getOverrideLength())) +
           HobString(Integer::toString(overrides.getOverrideWidth())) + HobString(Integer::toString(overrides.getOverrideLipHeight())) ); 

           canMsg.can_id = pCanLibrary->getWrapperCobID(wrEvent);

           canMsg.can_dlc = 3;
           canMsg.data[0] = overrides.getOverrideLength();
           canMsg.data[1] = overrides.getOverrideWidth();
           canMsg.data[2] = overrides.getOverrideLipHeight();
            

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to send Wrapper message WrNGWDimensionOverrides. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWDimensionOverrides: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(wrEvent) );
             }
           break;
        }
        default: {
            Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Unknown Wrapper event: " + Integer::toString(wrEvent) );
            break;
        }
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateLabelApplierEvent(LaEvent laEvent, const char *recvBuffer)
{
    int nBytes = 0;

    can_frame canMsg;
    memset( &canMsg, 0, sizeof(can_frame) );

    Debug::trace( "CanTranslator", "translateLabelApplierEvent() laEvent:" + Integer::toString(laEvent) );

    //translate downstream (out going) Applier messages
    switch (laEvent) {
		case LaSetup: {
			Debug::trace( "CanTranslator", "Send Can Message: LaSetup");
			LabelApplierSetup setup(recvBuffer);

			//Debug::trace( "CanTranslator", "PrimaryLabelSize: " + Integer::toString(setup.getPrimaryLabelSize()) );
			//Debug::trace( "CanTranslator", "PrimaryLabelRotation: " + Integer::toString(setup.getPrimaryLabelRotation()) );
			//Debug::trace( "CanTranslator", "PrimaryLabelPlacement: " + Integer::toString(setup.getPrimaryLabelPlacement()) );
			//Debug::trace( "CanTranslator", "SecondaryLabelSize: " + Integer::toString(setup.getSecondaryLabelSize()) );
			//Debug::trace( "CanTranslator", "SecondaryLabelRotation: " + Integer::toString(setup.getSecondaryLabelRotation()) );
			//Debug::trace( "CanTranslator", "SecondaryLabelPlacement: " + Integer::toString(setup.getSecondaryLabelPlacement()) );
			//Debug::trace( "CanTranslator", "getRotateSecondaryPrintImage: "		+ Integer::toString(setup.getRotateSecondaryPrintImage()));

            canMsg.can_id = 0x53c;		//Not going to add Applier to library for 3 messages!

            canMsg.can_dlc = 8;
            canMsg.data[0] = getMsbFromShort( setup.getPrimaryLabelSize() );
            canMsg.data[1] = getLsbFromShort( setup.getPrimaryLabelSize() );
            canMsg.data[2] = getMsbFromShort( setup.getPrimaryLabelRotation() );
            canMsg.data[3] = getLsbFromShort( setup.getPrimaryLabelRotation() );
            canMsg.data[4] = getMsbFromShort( setup.getPrimaryLabelPlacement() );
            canMsg.data[5] = getLsbFromShort( setup.getPrimaryLabelPlacement() );
            canMsg.data[6] = getMsbFromShort( setup.getSecondaryLabelSize() );
            canMsg.data[7] = getLsbFromShort( setup.getSecondaryLabelSize() );

			//Debug::trace("CanTranslator", "CAN DATA[0] getPrimaryLabelSize: " + Integer::toString(canMsg.data[0]));
			//Debug::trace("CanTranslator", "CAN DATA[1] getPrimaryLabelSize: " + Integer::toString(canMsg.data[1]));
			//Debug::trace("CanTranslator", "CAN DATA[2] getPrimaryLabelRotation: " + Integer::toString(canMsg.data[2]));
			//Debug::trace("CanTranslator", "CAN DATA[3] getPrimaryLabelRotation: " + Integer::toString(canMsg.data[3]));
			//Debug::trace("CanTranslator", "CAN DATA[4] getPrimaryLabelPlacement: " + Integer::toString(canMsg.data[4]));
			//Debug::trace("CanTranslator", "CAN DATA[5] getPrimaryLabelPlacement: " + Integer::toString(canMsg.data[5]));
			//Debug::trace("CanTranslator", "CAN DATA[6] getSecondaryLabelSize: " + Integer::toString(canMsg.data[6]));
			//Debug::trace("CanTranslator", "CAN DATA[7] getSecondaryLabelSize: " + Integer::toString(canMsg.data[7]));

            if(canMsg.can_id != 0) {
                 nBytes = pCanManager->send(&canMsg);
                 if(nBytes != sizeof(canMsg) ) {
                     Debug::trace( "CanTranslator.Error", "translateLabelApplierEvent(): Failed to send Applier message LaSetup. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
                 } else {


					canMsg.can_id = 0x53d;	//Not going to add Applier to library for 3 messages!

					canMsg.can_dlc = 5;											// was 4 but I added getRotateSecondaryPrintImage
					canMsg.data[0] = getMsbFromShort( setup.getSecondaryLabelRotation() );
					canMsg.data[1] = getLsbFromShort( setup.getSecondaryLabelRotation() );
					canMsg.data[2] = getMsbFromShort( setup.getSecondaryLabelPlacement() );
					canMsg.data[3] = getLsbFromShort( setup.getSecondaryLabelPlacement() );
					canMsg.data[4] = setup.getRotateSecondaryPrintImage();      // right now I do not send the RotatePrimaryPrintImage value, BUT if I do in the future, make RotatePrimaryPrintImage [5] and move secondary to [6]

					//Debug::trace("CanTranslator", "CAN DATA[0] getSecondaryLabelRotation: " + Integer::toString(canMsg.data[0]));
					//Debug::trace("CanTranslator", "CAN DATA[1] getSecondaryLabelRotation: " + Integer::toString(canMsg.data[1]));
					//Debug::trace("CanTranslator", "CAN DATA[2] getSecondaryLabelPlacement: " + Integer::toString(canMsg.data[2]));
					//Debug::trace("CanTranslator", "CAN DATA[3] getSecondaryLabelPlacement: " + Integer::toString(canMsg.data[3]));
					//Debug::trace("CanTranslator", "CAN DATA[4] getRotateSecondaryPrintImage: " + Integer::toString(canMsg.data[4]));

	                 nBytes = pCanManager->send(&canMsg);
	                 if(nBytes != sizeof(canMsg) ) {
	                     Debug::trace( "CanTranslator.Error", "translateLabelApplierEvent(): Failed to send Applier message LaSetup. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
	                 }

                 }
             } else {
                 Debug::trace( "CanTranslator.Error", "translateWrapperEvent(): Failed to get COB id for WrNGWFlush: " + Integer::toString(canMsg.can_id) + " Event: " + Integer::toString(laEvent) );
             }

			break;
		}
		case LaLabelReady: {
			Debug::trace( "CanTranslator", "Send Can Message: LaLabelReady");
			LabelApplierLabelReady ready(recvBuffer);

			canMsg.can_id = 0x532;	//Not going to add Applier to library for 3 messages!

			canMsg.can_dlc = 4;
			canMsg.data[0] = getMsbFromShort( ready.getStationId() );
			canMsg.data[1] = getLsbFromShort( ready.getStationId() );
			canMsg.data[2] = getMsbFromShort( ready.getPackageId() );
			canMsg.data[3] = getLsbFromShort( ready.getPackageId() );
			nBytes = pCanManager->send(&canMsg);
			if(nBytes != sizeof(canMsg) ) {
				Debug::trace( "CanTranslator.Error", "translateLabelApplierEvent(): Failed to send Applier message LaLabelReady. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
			}
			break;
		}
		case LaLabelTaken: {
			Debug::trace( "CanTranslator", "Send Can Message: LaLabelTaken");
			LabelApplierLabelTaken taken(recvBuffer);

			canMsg.can_id = 0x533;	//Not going to add Applier to library for 3 messages!

			canMsg.can_dlc = 4;
			canMsg.data[0] = getMsbFromShort( taken.getStationId() );
			canMsg.data[1] = getLsbFromShort( taken.getStationId() );
			canMsg.data[2] = getMsbFromShort( taken.getPackageId() );
			canMsg.data[3] = getLsbFromShort( taken.getPackageId() );
			nBytes = pCanManager->send(&canMsg);
			if(nBytes != sizeof(canMsg) ) {
			 Debug::trace( "CanTranslator.Error", "translateLabelApplierEvent(): Failed to send Applier message LabelApplierLabelTaken. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
			}
			break;
		}
		default: {
			Debug::trace( "CanTranslator", "translateLabelApplierEvent(): Unsupported Applier message: " + Integer::toString(laEvent) );
			break;
		}

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateMessage(can_frame *pCanFrame)
{
    bool isContiguous( false );
    int offset( 0 );

	//
	// Wrapper breaks convention. It uses multiple frame id's for a single
	// message. The version message from the bootloader is only sent once and the id's
	// are 0x54c,0x543,0x544,0x545,0x546. This can be removed after phase 2 of NGW project.
	//
	if ((( 0x543 <= pCanFrame->can_id ) && ( pCanFrame->can_id <= 0x546 )) || ( 0x54c == pCanFrame->can_id ))
	{
		pCanFrame->can_id = 0x54c;
	}
	// search contiguous messages
	else
	{
		for ( CanMsgRangeList::const_iterator i( contiguousMessages.begin() ); i != contiguousMessages.end(); ++i )
			if (( i->first <= pCanFrame->can_id ) && ( pCanFrame->can_id <= i->second ))
			{
				isContiguous = true;
				offset = pCanFrame->can_id - i->first;

				pCanFrame->can_id = i->first;
				break;
			}
	}

    //what is the size of this message?
    const int msgSize( pCanLibrary->getMsgSizeForCobId( pCanFrame->can_id ) );

    //ignore any message that is not known to the CAN Library.
    if ( !msgSize )
    	return;

    //Ack dot wear frames
    if(pCanFrame->can_id >= 0x171 && pCanFrame->can_id <= 0x191)
    {
            PrinterHeadDotAck ack( this, FirstPrinter,0,KPCPrinter,0);
            can_frame canMsg;
            canMsg.can_id = pCanLibrary->getPrinterCobID(PrHeadDotAck, FirstPrinter);
            canMsg.can_dlc = 6;
            canMsg.data[0] = ( ack.getSource() << 8);
            canMsg.data[1] = ( ack.getSource());
            canMsg.data[2] = ( ack.getDestination() << 8);
            canMsg.data[3] = ( ack.getDestination() );
            canMsg.data[4] = ( ack.getDotCount() << 8);
            canMsg.data[5] = ( ack.getDotCount() );
            if(canMsg.can_id != 0) {
            	pCanManager->send( &canMsg );
            }
			else 
			{
            	Debug::trace("CanTranslator.Error", "translatePrinterEvent():  Failed to get COB id for event: PrReqHeadDotStatus!" );
            }
    }

	CanMsg canMsg;
    canMsg.msgSize = 0;
    canMsg.numBytesRcvd = 0;
    canMsg.msgComplete = false;

	//message fits in one CAN frame (< 8 bytes).
	if (msgSize <= pCanFrame->can_dlc)
	{
		Debug::trace("CanTranslator.Translate", "translateMessage(): Received canMsg so call convertFrameToCanMsg() 1.");
		convertFrameToCanMsg(pCanFrame, &canMsg, msgSize);
	}

	//
	//if the overall size of the message is greater than
	//the number of bytes received in this frame then message needs to be queued.
	//
	else
	{
		std::lock_guard<std::mutex> guard( mutex_ );

		//determine if message queued.
		Debug::trace( "CanTranslator", "translateMessage(): Find Message on Queue: 0x" + Hex::toString(pCanFrame->can_id+offset) );

		//find the message on the queue.
		CanMsgQueue::iterator i( rxMsgQueue.find( pCanFrame->can_id ) );

		if ( rxMsgQueue.end() != i )
		{
			// found message in queue and we got start of message cob?
			// prevent out of order CAN messages or missing CAN messages!!!
			if (( isContiguous ) && ( 0 == offset )) 
            {
                //remove old can frame 
                rxMsgQueue.erase( pCanFrame->can_id );                
            	Debug::trace( "CanTranslator.Translate", "translateMessage(): Found message on queue but frame start: 0x" + Hex::toString(canMsg.id) );
            }
            else
			{
				canMsg = i->second;
				Debug::trace( "CanTranslator.Translate", "translateMessage(): Message from Queue: 0x" + Hex::toString(canMsg.id) + " Message Total Size: " + Integer::toString(canMsg.msgSize) +
						" Total Bytes Received: " + Integer::toString(canMsg.numBytesRcvd));
			}
		}

		//if message complete then erase from queue
		if ( convertFrameToCanMsg( pCanFrame, &canMsg, msgSize ) )
		{
			Debug::trace("CanTranslator.Translate", "translateMessage(): Received canMsg so call convertFrameToCanMsg() 2.");
			Debug::trace( "CanTranslator.Translate", "translateMessage(): Message Complete, Erase from Queue: 0x" + Hex::toString(pCanFrame->can_id) );

			rxMsgQueue.erase( pCanFrame->can_id );
		}
		else
		{
			Debug::trace( "CanTranslator.Translate", "translateMessage(): Push Message on Queue: 0x" + Hex::toString(pCanFrame->can_id) + " Message size: " + Integer::toString(msgSize) +
					" Frame Message Size: " + Integer::toString(canMsg.numBytesRcvd));

			rxMsgQueue[pCanFrame->can_id] = canMsg;
		}

    }

	if ( !canMsg.msgComplete )
		return;

	if (( pCanLibrary->getMinPrinterCobID(FirstPrinter) <= pCanFrame->can_id ) && ( pCanFrame->can_id <= pCanLibrary->getMaxPrinterCobID(ThirdPrinter) ))
	{
		Debug::trace( "CanTranslator.Translate", "translateMessage(): Translate Printer message.");
		translatePrinterCanMsg( &canMsg );
	}
	else if (( pCanLibrary->getMinWeigherCobID() <= pCanFrame->can_id ) && ( pCanFrame->can_id <= pCanLibrary->getMaxWeigherCobID() ))
	{
		Debug::trace( "CanTranslator.Translate", "translateMessage(): Translate Weigher message.");
		translateWeigherCanMsg( &canMsg );
	}
	else if (( pCanLibrary->getMinWrapperCobID() <= pCanFrame->can_id ) && ( pCanFrame->can_id <= pCanLibrary->getMaxWrapperCobID() ))
	{
		Debug::trace( "CanTranslator.Translate", "translateMessage(): Translate Wrapper message.");
		translateWrapperCanMsg( &canMsg );
	}
	else if ( pCanLibrary->getKpcSystemErrorCobID() == pCanFrame->can_id )
	{
		Debug::trace( "CanTranslator.Translate", "translateMessage(): Translate KPC system error message.");
	}
    else if ((pCanLibrary->getMinKPCBootCobID() <= pCanFrame->can_id) && (pCanFrame->can_id <= pCanLibrary->getMaxKPCBootCobID()))
    {
        Debug::trace("CanTranslator.Translate", "translateMessage(): Translate KPC Boot message.");
        translateKPCBootCanMsg(&canMsg);
    }
    else if ((pCanLibrary->getMinKPCBootUpgraderCobID() <= pCanFrame->can_id) && (pCanFrame->can_id <= pCanLibrary->getMaxKPCBootUpgraderCobID()))
    {
        Debug::trace("CanTranslator.Translate", "translateMessage(): Translate KPC Boot Upgrader message.");
        translateKPCBootUpgraderCanMsg(&canMsg);
    } else if ((pCanLibrary->getMinARKBoardSiteCobID() <= pCanFrame->can_id) && (pCanFrame->can_id <= pCanLibrary->getMaxARKBoardSiteCobID())) {
        Debug::trace("CanTranslator.Translate", "translateMessage(): Board Site testing message.");
        translateBoardSiteCanMsg( &canMsg );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanTranslator::convertFrameToCanMsg(can_frame *pCanFrame, CanMsg *pCanMsg, int size) {

    pCanMsg->id = pCanFrame->can_id;
    pCanMsg->msgSize = size;
    pCanMsg->numBytesRcvd += pCanFrame->can_dlc;

    if( pCanMsg->numBytesRcvd == pCanMsg->msgSize ) {
        pCanMsg->msgComplete = true;
    } else {
        if( pCanMsg->numBytesRcvd > pCanMsg->msgSize ) {//check for message overrun condition
            Debug::trace( "CanTranslator.Error", "convertFrameToCanMsg(): Message Overrun Id: " + Hex::toString(pCanMsg->id) +  " Message Size: " + Integer::toString(pCanMsg->msgSize) +  " Total Bytes Received: " + Integer::toString(pCanMsg->numBytesRcvd) );

            //dump the message from the Queue
            pCanMsg->msgComplete = true;
        } else {
            pCanMsg->msgComplete = false;
        }
    }
    Debug::trace( "CanTranslator", "convertFrameToCanMsg(): Received Can Message: " + Hex::toString(pCanMsg->id) + " Message Size:  " + Integer::toString(pCanMsg->msgSize) +
            " Frame Message Size: " + Integer::toString(pCanFrame->can_dlc) + " Total Bytes Received: " + Integer::toString(pCanMsg->numBytesRcvd) + " Is Message Complete: " + Integer::toString(pCanMsg->msgComplete));

    for( int i = 0; i < pCanFrame->can_dlc; i++ ) {
        pCanMsg->msgData.push_back( pCanFrame->data[i] );
    }
    return pCanMsg->msgComplete;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translatePrinterCanMsg(const CanMsg *pCanMsg) {

	unsigned short hobEvent = pCanLibrary->getEventForCobId( pCanMsg->id );
    PrinterType printerPosition = pCanLibrary->getPrinterPosition( pCanMsg->id );
    if( printerPosition != MaxPrinterTypes) {
		switch(hobEvent) {
             Debug::trace("CanTranslator.Translate", "translatePrinterCanMsg(): Translating Printer Message:" + Hex::toString( hobEvent ) );
			case PrWakeup: {
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrWakeup printerNum:" + HobString(Integer::toString(printerPosition)));
                int pid = charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] );

                PrinterWakeup wake;
                wake.setPrinterType((PrinterType)charToShort(pCanMsg->msgData[2], pCanMsg->msgData[3]) );
                wake.setBufferSize( charToShort(pCanMsg->msgData[4], pCanMsg->msgData[5]) );
                wake.setPrinterheadSize( charToShort(pCanMsg->msgData[6], pCanMsg->msgData[7]) );
                wake.setLabelWidth( (LabelWidth)pCanMsg->msgData[8] );
                unsigned long transferSize = charToShort(pCanMsg->msgData[9], pCanMsg->msgData[10]);

                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrWakeup id:" + Hex::toString( pid ) );
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrWakeup buffer size:" + Integer::toString( wake.getBufferSize() ) );
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrWakeup head size:" + Integer::toString( wake.getPrintheadSize() ) );
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrWakeup label width:" + Integer::toString( wake.getLabelWidth() ) );
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrWakeup transfer size:" + Integer::toString( transferSize ) );
                
                switch( printerPosition ) {
                    case FirstPrinter: {
                        if( pPrinter1Manager == NULL ) {
                        	if( pid == CAN_KPC_PRODUCT_ID ) {
                            Debug::trace("CanTranslator.Message", "RealTimeController::getInstance()->NewCANPrinter() KPCPrinter");    
                            pPrinter1Manager = RealTimeController::getInstance()->NewCANPrinter(this,
                                                    FirstPrinter,
                                                    wake.getLabelWidth(),
                                                    wake.getBufferSize(),
                                                    wake.getPrintheadSize() * 8,
                                                    transferSize,
                                                    KPCPrinter);
                        	} else if( pid == CAN_ARK_PRINTER_PRODUCT_ID ) {
                                Debug::trace("CanTranslator.Message", "RealTimeController::getInstance()->NewCANPrinter() ARKPrinterOnly");    
                                pPrinter1Manager = RealTimeController::getInstance()->NewCANPrinter(this,
                                                        FirstPrinter,
                                                        wake.getLabelWidth(),
                                                        wake.getBufferSize(),
                                                        wake.getPrintheadSize() * 8,
                                                        ARKPrinterOnly );
                        	} else if( pid == CAN_SS_KPC_PRODUCT_ID )  {
                                #if 1   //New freestanding scale project 
                                /* Remove comment when ready to use new device manager.
                                pPrinter1Manager = RealTimeController::getInstance()->NewSSPrinter(this,
                                                        FirstPrinter,
                                                        wake.getLabelWidth(),
                                                        wake.getBufferSize(),
                                                        wake.getPrintheadSize() * 8,
                                                        SSKPCPrinter );
                                                        */
                                Debug::trace("CanTranslator.Message", "RealTimeController::getInstance()->NewCANPrinter() SSKPCPrinter");    
                                pPrinter1Manager = RealTimeController::getInstance()->NewCANPrinter(this,
                                                    FirstPrinter,
                                                    wake.getLabelWidth(),
                                                    wake.getBufferSize(),
                                                    wake.getPrintheadSize() * 8,
                                                    transferSize,
                                                    SSKPCPrinter );

                                #else
                                pPrinter1Manager = RealTimeController::getInstance()->NewCANPrinter(this,
                                                    FirstPrinter,
                                                    wake.getLabelWidth(),
                                                    wake.getBufferSize(),
                                                    wake.getPrintheadSize() * 8,
                                                    transferSize,
                                                    KPCPrinter);
                                #endif
                        	}
                        } else {
                        	Debug::trace("CanTranslator.Message", "send printer Wakeup");

                        	PrinterWakeup printerWakeup( this, FirstPrinter, wake.getLabelWidth(),
                        			wake.getBufferSize(),
                        			wake.getPrintheadSize() * 8,
                                    PrinterConfigValid );

                        	sendPrinterMessage(&printerWakeup);
                        }
                        break;
                    }
                    case SecondPrinter: {
                        if( pPrinter2Manager == NULL ) {
                            pPrinter2Manager = RealTimeController::getInstance()->NewCANPrinter(this,
                                                    SecondPrinter,
                                                    wake.getLabelWidth(),
                                                    wake.getBufferSize(),
                                                    wake.getPrintheadSize() * 8,
                                                    ARKPrinterOnly );

                        } else {
                        	PrinterWakeup printerWakeup(this, SecondPrinter, wake.getLabelWidth(),
                        			wake.getBufferSize(),
                        			wake.getPrintheadSize() * 8, // wants to be in dots
                        			PrinterConfigValid );
                            
                        	sendPrinterMessage(&printerWakeup);
                        }
                        break;
                    }
                    case ThirdPrinter: {
                        if( pPrinter3Manager == NULL ) {
                            pPrinter3Manager = RealTimeController::getInstance()->NewCANPrinter(this,
                                                    ThirdPrinter,
                                                    wake.getLabelWidth(),
                                                    wake.getBufferSize(),
                                                    wake.getPrintheadSize() * 8,
                                                    ARKPrinterOnly );

                        } else {
                        	PrinterWakeup printerWakeup(this, ThirdPrinter, UFWLabel,
                        			wake.getBufferSize(),
                        			wake.getPrintheadSize() * 8,
                        			PrinterConfigValid );
                            

                        }
                        break;
                    }
                    case MaxPrinterTypes:
                    default: {
                        Debug::trace("CanTranslator.Error", "translatePrinterCanMsg(): Received Wakeup for unsupported PrinterType:"+Integer::toString(printerPosition));
                        break;
                    }
                }
                break;
            }
            case PrInquire: {
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrInquire");
                //Debug::trace("CanTranslator.Message", "CanTranslator::translatePrinterCanMsg() PrInquire (7,2):  Got a PrConfig message in CanTranslator");
                PrinterInquire prInquire(this, (PrinterType)printerPosition); // use default constructor

                prInquire.setConfigType( (PrConfigType)charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
                unsigned char sig[6] = {'P','R','\0','\0','\0','\0'};
                prInquire.setDeviceSignature( &sig[0] );			// no longer used

                prInquire.setPrinterType( (PrinterType) pCanMsg->msgData[2] );
                prInquire.setLabelWidth( (LabelWidth) pCanMsg->msgData[3] );
                prInquire.setMediaSensorAdjustment( pCanMsg->msgData[4] );
                prInquire.setOutOfMediaCount( pCanMsg->msgData[5] );                                    // Immutable!! Set to 200 on Ultima
                prInquire.setPrintContrast( charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7] ) );  // "Print Contrast" on Ultima Configuration.
                prInquire.setConfigWord0( charToShort( pCanMsg->msgData[8], pCanMsg->msgData[9] ) );    // Used on "European" labels
                prInquire.setConfigWord1( charToShort( pCanMsg->msgData[10], pCanMsg->msgData[11] ) );  // Used on "European" labels
                prInquire.setConfigWord2( charToShort( pCanMsg->msgData[12], pCanMsg->msgData[13] ) );  // Used on "European" labels
                prInquire.setConfigWord3( charToShort( pCanMsg->msgData[14], pCanMsg->msgData[15] ) );  // "Expel Position" on Ultima Configuration.
                prInquire.setConfigWord4( charToShort( pCanMsg->msgData[16], pCanMsg->msgData[17] ) );  // Used on "European" labels
                prInquire.setConfigWord5( charToShort( pCanMsg->msgData[18], pCanMsg->msgData[19] ) );
                prInquire.setConfigWord6( charToShort( pCanMsg->msgData[20], pCanMsg->msgData[21] ) );
                prInquire.setConfigWord7( charToShort( pCanMsg->msgData[22], pCanMsg->msgData[23] ) );
                prInquire.setConfigWord8( charToShort( pCanMsg->msgData[24], pCanMsg->msgData[25] ) );
                prInquire.setConfigWord9( charToShort( pCanMsg->msgData[26], pCanMsg->msgData[27] ) );
                prInquire.setConfigWord10( charToShort( pCanMsg->msgData[28], pCanMsg->msgData[29] ) );
                prInquire.setConfigWord11( charToShort( pCanMsg->msgData[30], pCanMsg->msgData[31] ) );
                prInquire.setConfigWord12( charToShort( pCanMsg->msgData[32], pCanMsg->msgData[33] ) );
                prInquire.setConfigWord13( charToShort( pCanMsg->msgData[34], pCanMsg->msgData[35] ) );
                prInquire.setMediaSensorType( (MediaSensorType)charToShort( pCanMsg->msgData[36], pCanMsg->msgData[37] ) );
                prInquire.setSLAEjectLine( charToShort( pCanMsg->msgData[38], pCanMsg->msgData[39] ) );
                prInquire.setPrintPositionAbsolute( charToShort( pCanMsg->msgData[40], pCanMsg->msgData[41] ) );

                //Debug::trace("CanTranslator.Message", "translatePrinterCanMsg() prInquire.getPritnerType(): " + Integer::toString((PrinterType)pCanMsg->msgData[2] ));
                //Debug::trace("CanTranslator.Message", "translatePrinterCanMsg() prInquire.getLabelWidth(): " + Integer::toString((LabelWidth)pCanMsg->msgData[3] ));
                //Debug::trace("CanTranslator.Message", "translatePrinterCanMsg() prInquire.getMediaSensorAdjustment(): " + Integer::toString(pCanMsg->msgData[4] ));
                //Debug::trace("CanTranslator.Message", "translatePrinterCanMsg() prInquire.getOutOfMediaCount(): " + Integer::toString(pCanMsg->msgData[5] ));
                //Debug::trace("CanTranslator.Message", "translatePrinterCanMsg() prInquire.getPrintContrast(): " + Integer::toString(prInquire.getPrintContrast() ));
                //Debug::trace("CanTranslator.Message", "translatePrinterCanMsg() prInquire.getMediaSensorType(): " + Integer::toString(prInquire.getMediaSensorType() ));
                //Debug::trace("CanTranslator.Message", "translatePrinterCanMsg() prInquire.getSLAEjectLine(): " + Integer::toString(prInquire.getSLAEjectLine() ));
                
                prInquire.show();
                sendPrinterMessage(&prInquire);
                break;
            }
            case PrStatus: {
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrStatus");
                PrinterStatus ps(this, (PrinterType)printerPosition); // use default constructor

                //Set the status information
                ps.setState(pCanMsg->msgData[0]);
                ps.setCommand( pCanMsg->msgData[1] );
                ps.setError( pCanMsg->msgData[2] );
                ps.setHistory( pCanMsg->msgData[3] );
                ps.setSensor( pCanMsg->msgData[4] );
                ps.setUser( pCanMsg->msgData[5] );
                ps.setCounter( charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7] ) );
                
                //Set the mask info
                ps.setSensorMask(  pCanMsg->msgData[8] );
                ps.setUserMask(  pCanMsg->msgData[9] );
                ps.setSensor2Mask(  pCanMsg->msgData[10] );

                //pCanMsg->msgData[11]  SLA not supported

                //Set the remaining bytes
                ps.setSensor2( pCanMsg->msgData[12]);
                sendPrinterMessage(&ps);
                break;
            }
            case PrSensors: {
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrSensors");
                PrinterSensors sensors(this, (PrinterType) printerPosition);
                
                int headUp = charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] );

                PrinterHeadUpType state = PrinterHeadEngaged;
				if( headUp >= 231) {	// 231 = 3v
					state = PrinterHeadDisengaged;
				} else {
					state = PrinterHeadEngaged;
				}
				sensors.setHeadUp( state );


				int labelReading = charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] );
Debug::trace("CanTranslator.Gap", "labelReading: " + Integer::toString(labelReading));
				LabelWidth labelWidth = UnknownLabel;

				if( state == PrinterHeadEngaged ) {
					if ((0 <= labelReading) && (labelReading <=  39 )) { // 0v - .5V = 39
						labelWidth = NarrowLabel;
					} else  {
						labelWidth = WideLabel;
					}
				}
				sensors.setLabelWidth( labelWidth );

				sensors.setRegulatorTemperature( charToShort( pCanMsg->msgData[4], pCanMsg->msgData[5] ) );

				unsigned short headTemperature = charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7] );
				sensors.setHeadTemperature( headTemperature );
				sensors.setHeadVoltageCounts( charToShort( pCanMsg->msgData[8], pCanMsg->msgData[9] ) );
				sensors.setHeadCurrent( charToShort( pCanMsg->msgData[10], pCanMsg->msgData[11] ) );
				sensors.setLabelHighAverage( charToShort( pCanMsg->msgData[12], pCanMsg->msgData[13] ) );
				sensors.setLabelLowAverage( charToShort( pCanMsg->msgData[14], pCanMsg->msgData[15] ) );
				sensors.setLabelThreshold( charToShort( pCanMsg->msgData[16], pCanMsg->msgData[17] ) );
				sensors.setLabel( charToShort( pCanMsg->msgData[18], pCanMsg->msgData[19] ) );
				sensors.setMediaHighAverage( charToShort( pCanMsg->msgData[20], pCanMsg->msgData[21] ) );
				sensors.setMediaLowAverage( charToShort( pCanMsg->msgData[22], pCanMsg->msgData[23] ) );
				sensors.setMediaThreshold( charToShort( pCanMsg->msgData[24], pCanMsg->msgData[25] ) );
				sensors.setMedia( charToShort( pCanMsg->msgData[26], pCanMsg->msgData[27] ) );

				sendPrinterMessage( &sensors );
                break;
            }
            case PrLabelTransferReady : {
            	Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrLabelTransferReady");

            	PrinterLabelTransferReady msg( this, true, (unsigned int)printerPosition );
            	dispatchEvent( &msg );
            	break;
            }
            case PrVersion: {
                Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrVersion");
                PrinterVersion version( this, (PrinterType)printerPosition );

    			std::string dbString = "";
    			for( int i = 0; i < 12; i++ ) {
    				dbString.append(Hex::toString(pCanMsg->msgData[i]));
    			}
    			Debug::trace("CanTranslator.Message", "rxData[]: " + dbString);

                int id = charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) ;
                Debug::trace("CanTranslator.Message", "PrVersion id: " + Hex::toString( id ) );

                version.setSoftwareMajor( charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] ) );
                version.setSoftwareMinor( charToShort( pCanMsg->msgData[4], pCanMsg->msgData[5] ) );
                version.setSoftwareBuild( charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7] ) );
                //sent as a short

                version.setHardwareMajor( pCanMsg->msgData[9] );
                version.setHardwareMinor( pCanMsg->msgData[11] );

                sendPrinterMessage( &version );
                break;
            }

            case PrHeadType: {
				Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrHeadType");

				PrinterHeadType head_type( this, (PrinterType)printerPosition );

				std::string dbString = "";
				for( int i = 0; i < 3; i++ ) {
					dbString.append(Hex::toString(pCanMsg->msgData[i]));
				}
				Debug::trace("CanTranslator.Message", "rxData[]: " + dbString);

				head_type.setHeadType( pCanMsg->msgData[0] );
				head_type.setHeadSize( charToShort(pCanMsg->msgData[1], pCanMsg->msgData[2]) );

				Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrHeadType "  + Integer::toString( head_type.getHeadType() ));
				Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrHeadSize "  + Integer::toString( head_type.getHeadSize() ));

				sendPrinterMessage( &head_type );
				break;
			}
            case PrCutterStatus: {
            	Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrCutterStatus");

            	PrinterCutterStatus status( this, (PrinterType)printerPosition, (bool)pCanMsg->msgData[0], (bool)pCanMsg->msgData[1],
                (bool)pCanMsg->msgData[2], (bool)pCanMsg->msgData[3] );
            	sendPrinterMessage( &status );
            	break;
            }
            case PrHeadDotWear: {
            	Debug::trace("CanTranslator.HeadDot", "translatePrinterCanMsg(): PrHeadDotWear "  + Integer::toString( pCanMsg->msgSize));

                std::string dbString = "";

    			for( int i = 0; i < pCanMsg->numBytesRcvd; i++ ) {
    				dbString.append(Hex::toString(pCanMsg->msgData[ i + 3 ]));
    			}
    			Debug::trace("CanTranslator.HeadDot", "pCanMsg->msgData[]: " + dbString);
                PrinterDotStatus dots[pCanMsg->numBytesRcvd / 2];
                uint8_t counts[pCanMsg->numBytesRcvd / 2];
                int j = 0;
                for(int i = 0; i < pCanMsg->numBytesRcvd; i+=8){
                    //this message is set up in blocks of 8 with the first 4 bytes being don status and the last 4 being dot counts
                    dots[j] = (PrinterDotStatus)pCanMsg->msgData[i];
                    counts[j++] = pCanMsg->msgData[i+4];
                    dots[j] = (PrinterDotStatus)pCanMsg->msgData[i+1];
                    counts[j++] = pCanMsg->msgData[i+5];
                    dots[j] = (PrinterDotStatus)pCanMsg->msgData[i+2];
                    counts[j++] = pCanMsg->msgData[i+6];
                    dots[j] = (PrinterDotStatus)pCanMsg->msgData[i+3];
                    counts[j++] = pCanMsg->msgData[i+7];
                }
            	PrinterHeadWear headWear( this, (PrinterType)printerPosition, (PrinterDotStatus *)dots, (uint8_t *)counts, (pCanMsg->msgSize / 2));
            	headWear.show();
                sendPrinterMessage( &headWear );
            	//sendPrinterMessage( headWear.serialize(), headWear.getSerializedLength(), (PrinterType)printerPosition );
            	break;
            }
            case PrResetFactoryDefaultsComplete: {
            	PrinterFactoryDefaultsComplete msg( this,  pCanMsg->msgData[0] );
            	sendPrinterMessage( &msg );
            	break;
            }
            case PrRS485FlowControl: {
            	Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrRS485FlowControl");
            	PrinterRS485FlowControl msg( this,  pCanMsg->msgData[0] );
        		dispatchEvent( &msg );
            	break;
            }
            case PrKPCError: {
            	Debug::trace("CanTranslator.Message", "translatePrinterCanMsg(): PrKPCError");
            	PrinterKpcError error( this, (PrinterKpcError::PrKpcWeigherErrors)charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ), FirstPrinter );
            	sendPrinterMessage( &error );
            	break;
            }
            case PrFlushCmplt: {
            	Debug::trace("CanTranslator.LabelTransfer", "translatePrinterCanMsg(): PrFlushCmplt");
            	PrinterLabelTransferComplete cmplt( this, (PrinterType)printerPosition, 0, 0, 0 );
            	sendPrinterMessage( &cmplt );
            	//dispatchEvent(&cmplt);
            	break;
            }
            case PrHeadDotStatus:  {
                Debug::trace("CanTranslator.HeadDot", "translatePrinterCanMsg(): case PrHeadDotStatus");
            	PrinterHeadDotStatus status(this, (PrinterType)printerPosition);
                // dont set source or destination so all subscribers can see this event
            	//status.setSource((uint16_t)(pCanMsg->msgData[0] << 8 | pCanMsg->msgData[1]));
                //status.setDestination((uint16_t)(pCanMsg->msgData[2] << 8 | pCanMsg->msgData[3]));
                status.setStatus((uint8_t)(pCanMsg->msgData[4]));
            	sendPrinterMessage(&status);
            	break;

            }
            case PrGapCalStatus: {
                PrinterGapCalStatus status( this, (PrinterType)printerPosition, (GapCalState)pCanMsg->msgData[0] );
                status.setDeflectionVoltage( (float)( (float)pCanMsg->msgData[1]  * .01294 ) );
                status.setDriveCurrent( (float)( (float)pCanMsg->msgData[2]  / 10.0 ) );
                status.setBackingVoltage( (float)( (float)pCanMsg->msgData[3] * .01294 ) );
                status.setlabelBackingVoltage( ( (float)pCanMsg->msgData[4] * .01294 ) );
                sendPrinterMessage( &status );
                break;
            }
            default:
            Debug::trace( "CanTranslator.Error", "translatePrinterCanMsg(): Unhandled Message Type " + Integer::toString(  pCanMsg->id ));
            break;
        }
    } else {
        Debug::trace( "CanTranslator.Error", "translatePrinterCanMsg(): Invalid printer position for cobID " + Integer::toString(  pCanMsg->id ));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateWeigherCanMsg(const CanMsg *pCanMsg) {
    
    unsigned short hobEvent = pCanLibrary->getEventForCobId( pCanMsg->id );

	Debug::trace("CanTranslator.Message", "translateWeigherCanMsg(): hobEvent=" + Integer::toString(hobEvent));

    switch(hobEvent) {
        case WgWakeup: {
            if( pWeigherManager == NULL ) {

            	int pid = charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] );

            	Debug::trace("CanTranslator.Message", "Create new Weigher Manager! Weigher product id: " + Hex::toString(pid));
            	if( ( pid == CAN_KPC_PRODUCT_ID ) || ( pid == CAN_SS_KPC_PRODUCT_ID ) ) {
            		pWeigherManager = RealTimeController::getInstance()->NewCANWeigher( this, WgCanKpc );
            	} else if ( pid == CAN_WEIGHER_PRODUCT_ID ) {
            		pWeigherManager = RealTimeController::getInstance()->NewCANWeigher( this, WgCanPrepack );
            	} else {
            		Debug::trace("CanTranslator.Message", "Unknown product id from Weigher wakeup: " + Hex::toString(pid));
            	}
            } else {
                Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgWakeup");
                WeigherWakeup ww(this, (WgCfgStatus)  pCanMsg->msgData[3] );
                sendWeigherMessage(&ww);
            }
            break;
        }
        case WgStatus: {
            Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgStatus");

            std::string dbString;

			for ( size_t i( 0 ); i < pCanMsg->numBytesRcvd; i++ )
				dbString.append( Hex::toString( pCanMsg->msgData[i] ) );

			Debug::trace("CanTranslator.WeigherStatus", "pCanMsg->msgData[]: " + dbString);

			const unsigned int counts( charToInt( &pCanMsg->msgData[10] ) );
			const unsigned int rawCounts( charToInt( &pCanMsg->msgData[6] ) );

			const unsigned short tiltX( charToShort( pCanMsg->msgData[26], pCanMsg->msgData[27] ) );
			const unsigned short tiltY( charToShort( pCanMsg->msgData[28], pCanMsg->msgData[29] ) );

            Debug::trace( "CanTranslator.WeigherStatus", "translateWeigherCanMsg(): TiltX : " + Integer::toString( tiltX ) );
            Debug::trace( "CanTranslator.WeigherStatus", "translateWeigherCanMsg(): TiltY : " + Integer::toString( tiltY ) );

            // create weigher status message
            WeigherStatus ws( this );
            ws.setStatusBits( charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
            ws.setCounts( counts );
            ws.setRawCounts( rawCounts );
            ws.setFilteredUncalibratedCounts( charToInt( &pCanMsg->msgData[2] ) );
            ws.setADCounts( charToInt( &pCanMsg->msgData[2] ) );

            ws.setAccelX( charToInt( &pCanMsg->msgData[14] ) );
            ws.setAccelY( charToInt( &pCanMsg->msgData[18] ) );
            ws.setAccelZ( charToInt( &pCanMsg->msgData[22] ) );
            ws.setTiltX( *((short*) &tiltX) );
            ws.setTiltY( *((short*) &tiltY) );

            Debug::trace( "CanTranslator.WeigherStatus", "translateWeigherCanMsg(): StatusBits 0: " + Hex::toString( pCanMsg->msgData[0] ));
            Debug::trace( "CanTranslator.WeigherStatus", "translateWeigherCanMsg(): StatusBits 1: " + Hex::toString( pCanMsg->msgData[1] ));
            Debug::trace( "CanTranslator.WeigherStatus", "translateWeigherCanMsg(): StatusBits : " + Integer::toString( ws.getStatusBits() ));
            ws.show();

            sendWeigherMessage(&ws);
            break;
        }
        case WgInquire:
        case WgConfig: {
            Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgConfig");
            WeigherInquire wi(this); // use default constructor

#if 0 	 	// implement Carlos's Weigher Configuration changes
            wi.setConfigType( (WgConfigType)charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
            wi.setCenterOfMaintenanceZone( charToInt( &pCanMsg->msgData[2] ) );
            wi.setRateOptionTime( charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7] ) );
            wi.setScaleFactor( charToInt( &pCanMsg->msgData[8] ) );
            wi.setMaxWeight( charToInt( &pCanMsg->msgData[12] ) );
            wi.setGainFactor( charToInt( &pCanMsg->msgData[16] ) );
            wi.setZeroReference( charToInt( &pCanMsg->msgData[20] ) );
            wi.setCalibrationReference( charToInt( &pCanMsg->msgData[24] ) );
            wi.setAutoZeroMaintenanceLimit( charToShort( pCanMsg->msgData[28], pCanMsg->msgData[29] ) );
            wi.setPrepackMotionCount( charToShort( pCanMsg->msgData[30], pCanMsg->msgData[31]) );
            wi.setInitializeZeroTime( charToShort( pCanMsg->msgData[32], pCanMsg->msgData[33]) );
            wi.setAutoZeroTime( charToShort( pCanMsg->msgData[34], pCanMsg->msgData[35] ) );
            wi.setRateOptionLimit(charToShort( pCanMsg->msgData[36], pCanMsg->msgData[37] ) );
            wi.setZeroReferenceLimit( charToShort( pCanMsg->msgData[38], pCanMsg->msgData[39] ) );
            wi.setCalibrationReferenceLimit( charToShort( pCanMsg->msgData[40], pCanMsg->msgData[41] ) );
            wi.setSmallMotionLimit( charToShort( pCanMsg->msgData[42], pCanMsg->msgData[43] ) );
            wi.setLargeMotionLimit( charToShort( pCanMsg->msgData[44], pCanMsg->msgData[45] ) );
            wi.setLargeMotionCount( charToShort( pCanMsg->msgData[46], pCanMsg->msgData[47] ) );
            wi.setSmallMotionCount( charToShort( pCanMsg->msgData[48], pCanMsg->msgData[49] ) );
            wi.setNoMotionCount( charToShort( pCanMsg->msgData[50], pCanMsg->msgData[51] ) );
            wi.setZeroTrackingOption( charToShort( pCanMsg->msgData[52], pCanMsg->msgData[53] ) );
            wi.setNegativeWeightOption( charToShort( pCanMsg->msgData[54], pCanMsg->msgData[55] ));
            wi.setPowerupMaintenanceZoneLimit( charToShort( pCanMsg->msgData[56], pCanMsg->msgData[57] ) );
            wi.setOperateMaintenanceZoneLimit( charToShort( pCanMsg->msgData[58], pCanMsg->msgData[59] ) );
            wi.setNumberOfCalibrations( charToShort( pCanMsg->msgData[60], pCanMsg->msgData[61] ) );
            wi.setNumberOfConfigurations( charToShort( pCanMsg->msgData[62], pCanMsg->msgData[63] ) );
            wi.setLastCalibrationDate( charToInt( &pCanMsg->msgData[64] ) );
            wi.setFilterSpeed( (WgFilterSpeed)pCanMsg->msgData[68] );
            wi.setWeigherType((WeigherType)pCanMsg->msgData[69] );
            wi.setFlags( pCanMsg->msgData[70] );
            wi.setWeigherModel( (WeigherModel)pCanMsg->msgData[71] );
            wi.setLastConfigurationDate( charToInt( &pCanMsg->msgData[72] ) );
            wi.setMinWeightToPrint( charToInt( &pCanMsg->msgData[76] ) );

            wi.setReferenceVoltage(0);	//no longer used
            unsigned char sig[6] = {'W','G','\0','\0','\0','\0'};
            wi.setDeviceSignature( &sig[0] ); //no longer used

            if (wi.getAutoZeroTime() != 3) {
                wi.setAutoZeroTime(3);
            }
#else
            wi.setConfigType( (WgConfigType)charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
            wi.setCenterOfMaintenanceZone( charToInt( &pCanMsg->msgData[2] ) );
            wi.setRateOptionTime( 0 );		//no longer used!
            wi.setPrepackMotionCount( charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7]) );
            wi.setScaleFactor( charToInt( &pCanMsg->msgData[8] ) );
            wi.setMaxWeight( charToInt( &pCanMsg->msgData[12] ) );
            wi.setGainFactor( charToInt( &pCanMsg->msgData[16] ) );
            wi.setZeroReference( 0 ); 	//no longer used!
            wi.setCalibrationReference( 0 ); //no longer used!
            wi.setAutoZeroMaintenanceLimit( 0 ); //no longer used!
            wi.setInitializeZeroTime( charToShort( pCanMsg->msgData[20], pCanMsg->msgData[21]) );
            wi.setAutoZeroTime( 3 );
            wi.setRateOptionLimit( 0 );
            wi.setZeroReferenceLimit( 0 );
            wi.setCalibrationReferenceLimit( 0 );
            wi.setSmallMotionLimit( charToShort( pCanMsg->msgData[22], pCanMsg->msgData[23] ) );
            wi.setLargeMotionLimit( charToShort( pCanMsg->msgData[24], pCanMsg->msgData[25] ) );
            wi.setLargeMotionCount( charToShort( pCanMsg->msgData[26], pCanMsg->msgData[27] ) );
            wi.setSmallMotionCount( charToShort( pCanMsg->msgData[28], pCanMsg->msgData[29] ) );
            wi.setNoMotionCount( charToShort( pCanMsg->msgData[30], pCanMsg->msgData[31] ) );
            wi.setZeroTrackingOption( 0 );
            wi.setNegativeWeightOption( 0 );
            wi.setPowerupMaintenanceZoneLimit( 0 );
            wi.setOperateMaintenanceZoneLimit( 0 );
            wi.setFilterSpeed( (WgFilterSpeed)pCanMsg->msgData[32] );
            wi.setWeigherType((WeigherType)pCanMsg->msgData[33] );
            wi.setFlags( pCanMsg->msgData[34] );
            wi.setWeigherModel( (WeigherModel)pCanMsg->msgData[35] );
            wi.setMinWeightToPrint( charToInt( &pCanMsg->msgData[36] ) );
            wi.setValueMaxEnabled( pCanMsg->msgData[40] );
			wi.setLastCalibrationDate( charToInt( &pCanMsg->msgData[41] ) );
            wi.setNumberOfCalibrations( charToShort( pCanMsg->msgData[45], pCanMsg->msgData[46] ) );

            wi.setAzsmMotionLimit(pCanMsg->msgData[47]);
            wi.setLastConfigurationDate( charToInt( &pCanMsg->msgData[48] ) );
            wi.setNumberOfConfigurations( charToShort( pCanMsg->msgData[52], pCanMsg->msgData[53] ) );
            wi.setReferenceVoltage(0);	//no longer used
            unsigned char sig[6] = {'W','G','\0','\0','\0','\0'};
            wi.setDeviceSignature( &sig[0] ); //no longer used
#endif
            wi.show();
            sendWeigherMessage(&wi);
            break;
        }
        case WgVersion: {
            Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgVersion");

            WeigherVersion weigherVersion(this);
            weigherVersion.setSoftwareMajor( charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ));
            weigherVersion.setSoftwareMinor( charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] ));
            weigherVersion.setHardwareMajor( pCanMsg->msgData[4] );
            weigherVersion.setHardwareMinor( pCanMsg->msgData[5] );

            weigherVersion.show();
            sendWeigherMessage(&weigherVersion);
            break;
        }
        case WgSerialNumber: {
            Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgSerialNumber");

            WeigherSerialNumber weigherSerialNumber(this);
            UINT8 temp[4];
            temp[0] = pCanMsg->msgData[0];
            temp[1] = pCanMsg->msgData[1];
            temp[2] = pCanMsg->msgData[2];
            temp[3] = pCanMsg->msgData[3];

            weigherSerialNumber.setSerialNumber( &temp[0] );

            weigherSerialNumber.show();
            sendWeigherMessage( &weigherSerialNumber );
            break;
        }
        case WgSystemInfo: {
        	Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgSystemInfo");

        	WeigherSystemInfo info( this );
        	info.setSerialFlashValid( pCanMsg->msgData[0] );
        	info.setEepromValid( pCanMsg->msgData[1] );
        	info.setSoftwareVersionType();
        	info.setSoftwareVersionMajor( pCanMsg->msgData[2] );
        	info.setSoftwareVersionMinor( pCanMsg->msgData[3] );
        	info.setSoftwareVersionBuild( pCanMsg->msgData[4] );
        	info.setHardwareVersionType();
        	info.setHardwareVersionMajor( pCanMsg->msgData[5] );
        	info.setHardwareVersionMinor( pCanMsg->msgData[6] );
        	info.setHardwareVersionBuild();
        	info.setWeigherPosition( (WeigherType)pCanMsg->msgData[7] );

        	info.show();

        	sendWeigherMessage( &info );
        	break;
        }
        case WgCat3LogEraseComplete: {
        	WeigherGeneric wMsg(WgCat3LogEraseComplete, this);
        	dispatchEvent( &wMsg );
        	break;
        }
        case WgCat3RecordStatus: {
        	WeigherCat3RecordStatus status(this);
        	status.setStatus( pCanMsg->msgData[0] );

        	sendWeigherMessage( &status );
        	break;
        }
        case WgCat3Records: {
        	WeigherCat3AuditRecords records(this);

        	records.setPageNumber( pCanMsg->msgData[0] );
        	records.setPageIndex( pCanMsg->msgData[1] );
        	records.setRecordBuffer( &pCanMsg->msgData[2] );

        	records.show();
        	sendWeigherMessage( &records );
        	break;
        }
        case WgCat3AuditStatistics: {
        	WeigherCat3Statistics stats(this);

            std::string dbString = "";

			for( int i = 0; i < pCanMsg->numBytesRcvd; i++ ) {
				dbString.append(Hex::toString(pCanMsg->msgData[i]));
			}
			Debug::trace("CanTranslator.WgCat3AuditStatistics", "pCanMsg->msgData[]: " + dbString);

        	stats.setTotalPages( pCanMsg->msgData[0] );
        	stats.setTotalRecordsPerPage( pCanMsg->msgData[1] );
        	stats.setCurrentPage( pCanMsg->msgData[2] );
        	stats.setTotalFreeRecords( charToShort( pCanMsg->msgData[3], pCanMsg->msgData[4] ) );
        	stats.setTotalRecordedEvents( charToShort( pCanMsg->msgData[5], pCanMsg->msgData[6] ) );
        	stats.setTotalCalibrationRecords( charToShort( pCanMsg->msgData[7], pCanMsg->msgData[8] ) );
        	stats.setTotalConfigurationRecords( charToShort( pCanMsg->msgData[9], pCanMsg->msgData[10] ) );
        	stats.setGainCoefficientCounter( charToShort( pCanMsg->msgData[11], pCanMsg->msgData[12] ) );
        	stats.setOffsetCoefficientCounter( charToShort( pCanMsg->msgData[13], pCanMsg->msgData[14] ) );
        	stats.setGainFactorCounter( charToShort( pCanMsg->msgData[15], pCanMsg->msgData[16] ) );
        	stats.setCenterOfMainCounter( charToShort( pCanMsg->msgData[17], pCanMsg->msgData[18] ) );
        	stats.setZeroReferenceCounter( charToShort( pCanMsg->msgData[19], pCanMsg->msgData[20] ) );
        	stats.setWeigherModelCounter( charToShort( pCanMsg->msgData[21], pCanMsg->msgData[22] ) );
        	stats.setMinWeightPrintCounter( charToShort( pCanMsg->msgData[23], pCanMsg->msgData[24] ) );
        	stats.setMaxWeightCounter( charToShort( pCanMsg->msgData[25], pCanMsg->msgData[26] ) );
        	stats.setFilterSpeedCounter( charToShort( pCanMsg->msgData[27], pCanMsg->msgData[28] ) );
        	stats.setDivisionSizeCounter( charToShort( pCanMsg->msgData[29], pCanMsg->msgData[30] ) );
        	stats.setWeigherModeCounter( charToShort( pCanMsg->msgData[31], pCanMsg->msgData[32] ) );

        	stats.show();
        	sendWeigherMessage( &stats );
        	break;
        }
        case WgValueMaxAssembly: {
        	Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgValueMaxAssembly");

        	std::string str = "";
        	str.append( 1, pCanMsg->msgData[0]  );
        	str.append( 1, pCanMsg->msgData[1] );
        	str.append( 1, pCanMsg->msgData[2] );
        	str.append( 1, pCanMsg->msgData[3] );
        	str.append( 1, pCanMsg->msgData[4] );
        	str.append( 1, pCanMsg->msgData[5] );
        	str.append( 1, pCanMsg->msgData[6] );
        	str.append( 1, pCanMsg->msgData[7] );
        	str.append( 1, pCanMsg->msgData[8] );
        	str.append( 1, pCanMsg->msgData[9] );

        	WeigherValueMaxAssembly assembly( str, this );
        	sendWeigherMessage( &assembly );
        	break;
        }
        case WgValueMaxHwVersion: {
        	Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgValueMaxHwVersion");

        	std::string str = "";
        	str.append(  1, pCanMsg->msgData[0]  );
        	str.append( 1, pCanMsg->msgData[1]  );

        	Debug::trace( "CanTranslator", "WgValueMaxHwVersion: " + str );

        	WeigherValueMaxHwVersion hwVersion( str, this );
        	sendWeigherMessage( &hwVersion );
        	break;
        }
        case WgValueMaxSerialNumber: {
        	Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgValueMaxSerialNumber");

        	std::string str = "";
        	str.append( 1, pCanMsg->msgData[0]  );
        	str.append( 1, pCanMsg->msgData[1] );
        	str.append( 1, pCanMsg->msgData[2] );
        	str.append( 1, pCanMsg->msgData[3] );
        	str.append( 1, pCanMsg->msgData[4] );
        	str.append( 1, pCanMsg->msgData[5] );
        	str.append( 1, pCanMsg->msgData[6] );
        	str.append( 1, pCanMsg->msgData[7] );
        	str.append( 1, pCanMsg->msgData[8] );
        	str.append( 1, pCanMsg->msgData[9] );
        	str.append( 1, pCanMsg->msgData[10] );
        	str.append( 1, pCanMsg->msgData[11] );
        	str.append( 1, pCanMsg->msgData[12] );
        	str.append( 1, pCanMsg->msgData[13] );
        	str.append( 1, pCanMsg->msgData[14] );
        	str.append( 1, pCanMsg->msgData[15] );
        	WeigherValueMaxSerialNumber serial( str, this );
        	sendWeigherMessage( &serial );
        	break;
        }
        case WgValueMaxDateOfManufacture: {
        	Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgValueMaxDateOfManufacture " + Integer::toString(pCanMsg->msgSize) );

        	WeigherValueMaxDateOfMan date( charToInt( &pCanMsg->msgData[0] ), this );
        	sendWeigherMessage( &date );
        	break;
        }
        case WgValueMaxXGain: {
        	Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgValueMaxXGain");

        	unsigned char value[4] = {0};
        	value[3] = pCanMsg->msgData[0];
        	value[2] = pCanMsg->msgData[1];
        	value[1] = pCanMsg->msgData[2];
        	value[0] = pCanMsg->msgData[3];

        	float x = 0;
        	memcpy(&x, &value[0], sizeof(float));

        	WeigherValueMaxXGainFactor xValue( x,  this );
        	sendWeigherMessage( &xValue );
        	break;
        }
        case WgValueMaxYGain: {
        	Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgValueMaxYGain");

        	unsigned char value[4] = {0};
        	value[3] = pCanMsg->msgData[0];
        	value[2] = pCanMsg->msgData[1];
        	value[1] = pCanMsg->msgData[2];
        	value[0] = pCanMsg->msgData[3];

        	float y = 0;
        	memcpy(&y, &value[0], sizeof(float));

        	WeigherValueMaxYGainFactor yValue( y,  this );
        	sendWeigherMessage( &yValue );
        	break;
        }
        case WgValueMaxZGain: {
        	Debug::trace( "CanTranslator", "translateWeigherCanMsg(): WgValueMaxZGain");

        	unsigned char value[4] = {0};
        	value[3] = pCanMsg->msgData[0];
        	value[2] = pCanMsg->msgData[1];
        	value[1] = pCanMsg->msgData[2];
        	value[0] = pCanMsg->msgData[3];

        	float z = 0;
        	memcpy(&z, &value[0], sizeof(float));

        	WeigherValueMaxZGainFactor zValue( z,  this );
        	sendWeigherMessage( &zValue );
        	break;
        }
        case WgValueMaxAllGains: {

        	float x = 0;
        	float y = 0;
        	float z = 0;
        	unsigned char value[4] = {0};

        	value[3] = pCanMsg->msgData[0];
        	value[2] = pCanMsg->msgData[1];
        	value[1] = pCanMsg->msgData[2];
        	value[0] = pCanMsg->msgData[3];

        	memcpy(&x, &value[0], sizeof(float));

        	value[3] = pCanMsg->msgData[4];
        	value[2] = pCanMsg->msgData[5];
        	value[1] = pCanMsg->msgData[6];
        	value[0] = pCanMsg->msgData[7];

        	memcpy(&y, &value[0], sizeof(float));

        	value[3] = pCanMsg->msgData[8];
        	value[2] = pCanMsg->msgData[9];
        	value[1] = pCanMsg->msgData[10];
        	value[0] = pCanMsg->msgData[11];

        	memcpy(&z, &value[0], sizeof(float));

        	WeigherValueMaxAllGains gains( x, y, z, this );
        	sendWeigherMessage( &gains );
        	break;
        }
        case WgKPCError: {
        	Debug::trace("CanTranslator.Message", "translateWeigherCanMsg(): WgKPCError");
        	WeigherKpcError error( this, (WeigherKpcError::WgKpcWeigherErrors)charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
        	sendWeigherMessage( &error );
        	break;
        }
		case WgValueMaxFitParams:
		{
			Debug::trace("CanTranslator", "translateWeigherCanMsg(): WgValueMaxFitParams");

			WeigherValueMaxFitParams fp(this);

			fp.setGravity(charToInt(&pCanMsg->msgData[0]));
			fp.setConvFactor(charToInt(&pCanMsg->msgData[4]));
			fp.setZeroValue(charToInt(&pCanMsg->msgData[8]));
			fp.setSpanValue(charToInt(&pCanMsg->msgData[12]));
			fp.setZ0ElectronicOffset(charToInt(&pCanMsg->msgData[16]));
			fp.setDeadWeight(charToInt(&pCanMsg->msgData[20]));
			fp.setX2(charToInt(&pCanMsg->msgData[24]));
			fp.setX1(charToInt(&pCanMsg->msgData[28]));
			fp.setX0(charToInt(&pCanMsg->msgData[32]));
			fp.setY2(charToInt(&pCanMsg->msgData[36]));
			fp.setY1(charToInt(&pCanMsg->msgData[40]));
			fp.setY0(charToInt(&pCanMsg->msgData[44]));
			fp.setX_Rot_0(charToInt(&pCanMsg->msgData[48]));
			fp.setX_Rot_1(charToInt(&pCanMsg->msgData[52]));
			fp.setX_Rot_2(charToInt(&pCanMsg->msgData[56]));
			fp.setY_Rot_0(charToInt(&pCanMsg->msgData[60]));
			fp.setY_Rot_1(charToInt(&pCanMsg->msgData[64]));
			fp.setY_Rot_2(charToInt(&pCanMsg->msgData[68]));
			fp.setZ_Rot_0(charToInt(&pCanMsg->msgData[72]));
			fp.setZ_Rot_1(charToInt(&pCanMsg->msgData[76]));
			fp.setZ_Rot_2(charToInt(&pCanMsg->msgData[80]));
			fp.setX_Gain(charToInt(&pCanMsg->msgData[84]));
			fp.setY_Gain(charToInt(&pCanMsg->msgData[88]));
			fp.setG_Coeff(charToInt(&pCanMsg->msgData[92]));
			fp.setCheckSum(charToInt(&pCanMsg->msgData[96]));

			fp.show();

			sendWeigherMessage(&fp);
			break;
		}
		case WgResetFactoryDefaultsComplete: {
        	Debug::trace("CanTranslator.Message", "translateWeigherCanMsg(): WgResetFactoryDefaultsComplete");
        	WeigherFactoryDefaultsComplete	cmplt( this, (bool)pCanMsg->msgData[0] );
        	sendWeigherMessage( &cmplt );
        	break;
        }
        default: {
            Debug::trace( "CanTranslator.Error", "translateWeigherCanMsg(): Unknown Weigher event: " + Integer::toString(hobEvent) );
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::handleWrStatusCanMsg(can_frame *pCanFrame)
{

	Debug::trace( "CanTranslator.EvolWrapperStatus", "handle frame id: " + Hex::toString( pCanFrame->can_id) );
	switch( pCanFrame->can_id ) {
		case 0x596: {
			statusBffr.clear();
			statusBffr.insert(statusBffr.begin(), pCanFrame->data[0]);
			statusBffr.insert(statusBffr.begin() + 1, pCanFrame->data[1]);
			statusBffr.insert(statusBffr.begin() + 2, pCanFrame->data[2]);
			statusBffr.insert(statusBffr.begin() + 3, pCanFrame->data[3]);
			statusBffr.insert(statusBffr.begin() + 4, pCanFrame->data[4]);
			statusBffr.insert(statusBffr.begin() + 5, pCanFrame->data[5]);
			statusBffr.insert(statusBffr.begin() + 6, pCanFrame->data[6]);
			break;
		}
		case 0x597: {
			statusBffr.insert(statusBffr.begin() + 7, pCanFrame->data[0]);
			statusBffr.insert(statusBffr.begin() + 8, pCanFrame->data[1]);
			statusBffr.insert(statusBffr.begin() + 9, pCanFrame->data[2]);
			statusBffr.insert(statusBffr.begin() + 10, pCanFrame->data[3]);
			statusBffr.insert(statusBffr.begin() + 11, pCanFrame->data[4]);
			statusBffr.insert(statusBffr.begin() + 12, pCanFrame->data[5]);
			statusBffr.insert(statusBffr.begin() + 13, pCanFrame->data[6]);
			statusBffr.insert(statusBffr.begin() + 14, pCanFrame->data[7]);
			break;
		}
		case 0x598: {
			statusBffr.insert(statusBffr.begin() + 15, pCanFrame->data[0]);
			statusBffr.insert(statusBffr.begin() + 16, pCanFrame->data[1]);
			statusBffr.insert(statusBffr.begin() + 17, pCanFrame->data[2]);
			statusBffr.insert(statusBffr.begin() + 18, pCanFrame->data[3]);
			statusBffr.insert(statusBffr.begin() + 19, pCanFrame->data[4]);
			statusBffr.insert(statusBffr.begin() + 20, pCanFrame->data[5]);
			statusBffr.insert(statusBffr.begin() + 21, pCanFrame->data[6]);
			statusBffr.insert(statusBffr.begin() + 22, pCanFrame->data[7]);
			break;
		}
		case 0x599: {
			statusBffr.insert(statusBffr.begin() + 23, pCanFrame->data[0]);
			statusBffr.insert(statusBffr.begin() + 24, pCanFrame->data[1]);
			statusBffr.insert(statusBffr.begin() + 25, pCanFrame->data[2]);
			statusBffr.insert(statusBffr.begin() + 26, pCanFrame->data[3]);
			statusBffr.insert(statusBffr.begin() + 27, pCanFrame->data[4]);
			statusBffr.insert(statusBffr.begin() + 28, pCanFrame->data[5]);
			statusBffr.insert(statusBffr.begin() + 29, pCanFrame->data[6]);
			statusBffr.insert(statusBffr.begin() + 30, pCanFrame->data[7]);
			break;
		}
		case 0x59a: {
			statusBffr.insert(statusBffr.begin() + 31, pCanFrame->data[0]);
			statusBffr.insert(statusBffr.begin() + 32, pCanFrame->data[1]);
			statusBffr.insert(statusBffr.begin() + 33, pCanFrame->data[2]);
			statusBffr.insert(statusBffr.begin() + 34, pCanFrame->data[3]);
			statusBffr.insert(statusBffr.begin() + 35, pCanFrame->data[4]);
			statusBffr.insert(statusBffr.begin() + 36, pCanFrame->data[5]);
			statusBffr.insert(statusBffr.begin() + 37, pCanFrame->data[6]);
			statusBffr.insert(statusBffr.begin() + 38, pCanFrame->data[7]);
			break;
		}
		case 0x59b: {
			statusBffr.insert(statusBffr.begin() + 39, pCanFrame->data[0]);
			statusBffr.insert(statusBffr.begin() + 40, pCanFrame->data[1]);
			statusBffr.insert(statusBffr.begin() + 41, pCanFrame->data[2]);
			statusBffr.insert(statusBffr.begin() + 42, pCanFrame->data[3]);
			statusBffr.insert(statusBffr.begin() + 43, pCanFrame->data[4]);
			statusBffr.insert(statusBffr.begin() + 44, pCanFrame->data[5]);
			statusBffr.insert(statusBffr.begin() + 45, pCanFrame->data[6]);
			statusBffr.insert(statusBffr.begin() + 46, pCanFrame->data[7]);
			break;
		}
		case 0x59c: {
			statusBffr.insert(statusBffr.begin() + 47, pCanFrame->data[0]);
			statusBffr.insert(statusBffr.begin() + 48, pCanFrame->data[1]);
			statusBffr.insert(statusBffr.begin() + 49, pCanFrame->data[2]);
			statusBffr.insert(statusBffr.begin() + 50, pCanFrame->data[3]);

			CanMsg canMsg;
			canMsg.id = 0x596;
			canMsg.msgSize = 51;
			canMsg.numBytesRcvd = 51;
			canMsg.msgData = statusBffr;
			translateWrapperCanMsg( &canMsg );

			//get ready for the next status message.
			//statusBffr.clear();
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateKpcSystemCanMsg(const CanMsg *pCanMsg)
{

	const unsigned short hobEvent( pCanLibrary->getEventForCobId( pCanMsg->id ) );
	Debug::trace( "CanTranslator", "translateKpcSystemCanMsg(): Translated COB ID: " + Hex::toString(pCanMsg->id) + " To Device Event: " + Integer::toString(hobEvent) );

	if( hobEvent == KpcError ) {
		KpcSystemErrors error( this, (KpcSystemErrors::KpcSystemError)charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ));
		dispatchEvent( &error );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateWrapperCanMsg(const CanMsg *pCanMsg)
{
    const unsigned short hobEvent( pCanLibrary->getEventForCobId( pCanMsg->id ) );

    Debug::trace( "CanTranslator", "translateWrapperCanMsg(): Translated COB ID: " + Hex::toString(pCanMsg->id) + " To Device Event: " + Integer::toString(hobEvent) );

    switch(hobEvent)
    {
        case WrEvolWakeup: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolWakeup" );

            if( !pWrapperManager ) {
            	Debug::trace("CanTranslator.Message", "Create new wrapper manager!" );
                pWrapperManager = RealTimeController::getInstance()->NewCANWrapper( this );
				if( pWrapperManager == NULL )
					Debug::trace("CanTranslator.Error", "Failed to create Wrapper Manager!" );

				pLabelApplier = RealTimeController::getInstance()->NewCANLabelApplier( this );
                if( pLabelApplier == NULL )
                	Debug::trace("CanTranslator.Error", "Failed to create Label Manager!" );
            }

            NGWWakeup ww(this, (NGWWakeup::WrapperModel)charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ), (NGWWakeup::WrapperOp)charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] ) );
            ww.show();

            sendWrapperMessage( &ww );
            break;
        }
        case WrEvolStatus: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolStatus");
            EvolWrapperStatus wrapperStatus(this);

            EvolWrapperStatusInfo statusInfo( wrapperStatus.getStatusInfo() );

            statusInfo.setReportStatus( pCanMsg->msgData[0] );
            statusInfo.setOpenWindow( pCanMsg->msgData[1] );
            statusInfo.setLimpHome( pCanMsg->msgData[2] );
            statusInfo.setWrapperStopped( pCanMsg->msgData[3] );
            statusInfo.setMainPower( pCanMsg->msgData[4] );
            statusInfo.setWrapperState( (EvolWrapperStatusInfo::EvolWrapperState_t)pCanMsg->msgData[5] );
            wrapperStatus.setWrapperMode( (WrOpMode)pCanMsg->msgData[6] );
            wrapperStatus.setStatusInfo( statusInfo );

            EvolWrapperErrorInfo error;
            error.setPrimaryConveyor( (UINT16)pCanMsg->msgData[7] );
			error.setSecondaryConveyor( (UINT16)pCanMsg->msgData[8] );
			error.setSecondaryShift( (UINT16)pCanMsg->msgData[9] );
			error.setElevator( (UINT16)pCanMsg->msgData[10] );
			error.setFilmSupply( (UINT16)pCanMsg->msgData[11] );
			error.setGripper( (UINT16)pCanMsg->msgData[12] );
			error.setKnife( (UINT16)pCanMsg->msgData[13] );
			error.setSideClamp( (UINT16)pCanMsg->msgData[14] );
			error.setSideFolder( (UINT16)pCanMsg->msgData[15] );
			error.setRearFolder( (UINT16)pCanMsg->msgData[16] );
			error.setFrontFolder( (UINT16)pCanMsg->msgData[17] );
			error.setPusher( (UINT16)pCanMsg->msgData[18] );
			error.setSealerBelt( (UINT16)pCanMsg->msgData[19] );
			error.setLabelApplier( (UINT16)pCanMsg->msgData[20] );
			error.setGeneralWrapper( (UINT16)pCanMsg->msgData[21] );
			error.setDiagnosticsSupport( (UINT16)pCanMsg->msgData[22] );
			wrapperStatus.setCurrErrorInfo(error);

			EvolWrapperErrorInfo prevError;
			prevError.setPrimaryConveyor( (UINT16)pCanMsg->msgData[23] );
			prevError.setSecondaryConveyor( (UINT16)pCanMsg->msgData[24] );
			prevError.setSecondaryShift( (UINT16)pCanMsg->msgData[25] );
			prevError.setElevator( (UINT16)pCanMsg->msgData[26] );
			prevError.setFilmSupply( (UINT16)pCanMsg->msgData[27] );
			prevError.setGripper( (UINT16)pCanMsg->msgData[28] );
			prevError.setKnife((UINT16)pCanMsg->msgData[29] );
			prevError.setSideClamp( (UINT16)pCanMsg->msgData[30] );
			prevError.setSideFolder( (UINT16)pCanMsg->msgData[31] );
			prevError.setRearFolder( (UINT16)pCanMsg->msgData[32] );
			prevError.setFrontFolder( (UINT16)pCanMsg->msgData[33] );
			prevError.setPusher( (UINT16)pCanMsg->msgData[34] );
			prevError.setSealerBelt( (UINT16)pCanMsg->msgData[35] );
			prevError.setLabelApplier( (UINT16)pCanMsg->msgData[36] );
			prevError.setGeneralWrapper( (UINT16)pCanMsg->msgData[37] );
			prevError.setDiagnosticsSupport( (UINT16)pCanMsg->msgData[38] );
			wrapperStatus.setPrevErrorInfo(prevError);

            EvolWrapperTemperatureInfo tmp_temperatureInfo = (const EvolWrapperTemperatureInfo)wrapperStatus.getTemperatureInfo();
            EvolWrapperTemperatureInfo &temperatureInfo = tmp_temperatureInfo;
            temperatureInfo.setSealerBeltTemp( charToShort( pCanMsg->msgData[40], pCanMsg->msgData[39] ) );
            temperatureInfo.setSealerBeltSetting( charToShort( pCanMsg->msgData[42], pCanMsg->msgData[41] ) );
            temperatureInfo.setSealerBeltStatusBits( charToShort( pCanMsg->msgData[44], pCanMsg->msgData[43] ) );
            temperatureInfo.setSealerBeltHeaterEnabled( charToShort( pCanMsg->msgData[46], pCanMsg->msgData[45] ) );
            temperatureInfo.setAmbientTemp( charToShort( pCanMsg->msgData[48], pCanMsg->msgData[47] ) );
            temperatureInfo.setAmbientTempStatusBits( charToShort( pCanMsg->msgData[50], pCanMsg->msgData[49] ) );

            wrapperStatus.setTemperatureInfo(temperatureInfo);

            wrapperStatus.show();

            sendWrapperMessage( &wrapperStatus );
            break;
        }
        case WrOpen: {
            Debug::trace( "CanTranslator.OpenMessage", "translateWrapperCanMsg(): WrOpen" );
            WrapperOpen openMsg(this);

            openMsg.show();

            sendWrapperMessage( &openMsg );
            break;
        }
        case WrEvolConfig: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolConfig");
            EvolWrapperInquire wrapperInquire(this);

            std::string dbString = "";

			for( int i = 0; i < pCanMsg->numBytesRcvd; i++ ) {
				dbString.append(Hex::toString(pCanMsg->msgData[i]));
			}
			Debug::trace("EvolWrapperConfig", "pCanMsg->msgData[]: " + dbString);

            EvolWrapperHeaterConfig &heaterCfg = wrapperInquire.getHeaterConfig();
            HeaterConfiguration &filmHeaterCfg = heaterCfg.getFilmHeaterConfig();
            HeaterConfiguration &sealerHeaterCfg = heaterCfg.getSealerHeaterConfig();
            EvolLabelApplierConfiguration &applierCfg = wrapperInquire.getLabelApplierConfig();
            EvolWrapperGeneralConfig &genConfig = wrapperInquire.getWrapperConfig();

            sealerHeaterCfg.setMaxTemp( charToShort( pCanMsg->msgData[1], pCanMsg->msgData[2] ) );

            sealerHeaterCfg.setMinTemp( pCanMsg->msgData[3] );
            sealerHeaterCfg.setIsUsed( pCanMsg->msgData[4] );

            filmHeaterCfg.setIsUsed( pCanMsg->msgData[5] );
            applierCfg.setIsSecondCLAAttached( false );
            applierCfg.setSideDischargeType( (LaSideDischarge)pCanMsg->msgData[6] );

            //SOF-2293
            genConfig.setUpperFilmPullTension( (UINT16)pCanMsg->msgData[7] );

            sealerHeaterCfg.setDefaultTempSetting( charToShort( pCanMsg->msgData[8], pCanMsg->msgData[9] ) );
            filmHeaterCfg.setMaxTemp( charToShort( pCanMsg->msgData[10], pCanMsg->msgData[11] ) );

            filmHeaterCfg.setMinTemp( pCanMsg->msgData[12] );
            filmHeaterCfg.setDefaultTempSetting( charToShort( pCanMsg->msgData[13], pCanMsg->msgData[14] ) );
            //SOF-2293
            genConfig.setLowerFilmPullTension(pCanMsg->msgData[15]);

            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): sealerHeaterCfg.setMaxTemp: " + Integer::toString( sealerHeaterCfg.getMaxTemp() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): sealerHeaterCfg.setMinTemp: " + Integer::toString( sealerHeaterCfg.getMinTemp() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): filmHeaterCfg.setIsUsed: " + Integer::toString( filmHeaterCfg.getIsUsed() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): filmHeaterCfg.setMaxTemp: " + Integer::toString( filmHeaterCfg.getMaxTemp() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): filmHeaterCfg.setMinTemp: " + Integer::toString( filmHeaterCfg.getMinTemp() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): filmHeaterCfg.setDefaultTempSetting: " + Integer::toString( filmHeaterCfg.getDefaultTempSetting() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): sealerHeaterCfg.setIsUsed: " + Integer::toString( sealerHeaterCfg.getIsUsed() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSideDischargeType: " + Integer::toString( applierCfg.getSideDischargeType() ) );

            EvolWrapperGeneralConfig &wrGenCfg = wrapperInquire.getWrapperConfig();

            WrapperFilmWidthDimensions &topFilm = wrGenCfg.getTopFilmRollDimensions();
            topFilm.setRollWidth( charToShort( pCanMsg->msgData[16], pCanMsg->msgData[17] )  );

            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): topFilm.setRollWidth: " + Integer::toString( topFilm.getRollWidth() ) );

            WrapperFilmWidthDimensions &bottomFilm = wrGenCfg.getBottomFilmRollDimensions();
            bottomFilm.setRollWidth( charToShort( pCanMsg->msgData[18], pCanMsg->msgData[19] )  );

            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): bottomFilm.setRollWidth: " + Integer::toString( bottomFilm.getRollWidth() ) );



            applierCfg.setHasAutoRotation( pCanMsg->msgData[20] );
            wrGenCfg.setWrapperOperation( (WrOperation)0 ); //not used
            wrGenCfg.setHardwareAddOn( (WrHardwareAddOn)pCanMsg->msgData[21] );
            applierCfg.setPriLabelOffsetOnPkg( (UINT16)pCanMsg->msgData[22] );
            applierCfg.setSecLabelOffsetOnPkg( (UINT16)pCanMsg->msgData[23] );

            applierCfg.setPriLabelRotationDefault( (LaRotation)pCanMsg->msgData[24] );
            applierCfg.setPriLabelSizeDefault( charToShort( pCanMsg->msgData[25], pCanMsg->msgData[26] ) );
            applierCfg.setPriLabelWidth( (LabelWidth)pCanMsg->msgData[27] );
            applierCfg.setPriLabelLocationDefault((LaPosLoc) pCanMsg->msgData[28] );
            applierCfg.setPriRotationCalibrationOffset( (UINT16)pCanMsg->msgData[29] );//0Deg
            applierCfg.setPriWandCalibrationOffset( (UINT16)pCanMsg->msgData[30] );
            wrGenCfg.setSealerBeltPackageSensorType( (UINT32)pCanMsg->msgData[31]  );


            applierCfg.setSecLabelRotationDefault( (LaRotation)pCanMsg->msgData[32] );
            applierCfg.setSecLabelSizeDefault( charToShort( pCanMsg->msgData[33], pCanMsg->msgData[34] ) );
			applierCfg.setSecLabelWidth( (LabelWidth)pCanMsg->msgData[35] );
			applierCfg.setSecLabelLocationDefault((LaPosLoc) pCanMsg->msgData[36] );
			applierCfg.setSecRotationCalibrationOffset( (UINT16)pCanMsg->msgData[37] );
			applierCfg.setSecWandCalibrationOffset( (UINT16)pCanMsg->msgData[38] );

            applierCfg.setPriRotationCalibrationOffset_90Deg( (UINT16)pCanMsg->msgData[39] );
            applierCfg.setPriRotationCalibrationOffset_180Deg( (UINT16)pCanMsg->msgData[40] );
            applierCfg.setPriRotationCalibrationOffset_270Deg( (UINT16)pCanMsg->msgData[41] );

            applierCfg.setSecRotationCalibrationOffset_90Deg( (UINT16)pCanMsg->msgData[42] );
            applierCfg.setSecRotationCalibrationOffset_180Deg( (UINT16)pCanMsg->msgData[43] );
            applierCfg.setSecRotationCalibrationOffset_270Deg( (UINT16)pCanMsg->msgData[44] );

            applierCfg.setPriApplierHandAdjust( (UINT16)pCanMsg->msgData[45] );
            applierCfg.setSecApplierHandAdjust( (UINT16)pCanMsg->msgData[46] );

			Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): wrGenCfg.getHardwareAddOn: " + Integer::toString( wrGenCfg.getHardwareAddOn() ) );
			Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setHasAutoRotation: " + Integer::toString( applierCfg.getHasAutoRotation() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriLabelSizeDefault: " + Integer::toString( applierCfg.getPriLabelSizeDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriLabelWidth: " + Integer::toString( applierCfg.getPriLabelWidth() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriLabelRotationDefault: " + Integer::toString( applierCfg.getPriLabelRotationDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriLabelLocationDefault: " + Integer::toString( applierCfg.getPriLabelLocationDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriLabelOffsetOnPkg: " + Integer::toString( applierCfg.getPriLabelOffsetOnPkg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriRotationCalibrationOffset: " + Integer::toString( applierCfg.getPriRotationCalibrationOffset() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriWandCalibrationOffset: " + Integer::toString( applierCfg.getPriWandCalibrationOffset() ) );


            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecLabelSizeDefault: " + Integer::toString( applierCfg.getSecLabelSizeDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecLabelWidth: " + Integer::toString( applierCfg.getSecLabelWidth() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecLabelRotationDefault: " + Integer::toString( applierCfg.getSecLabelRotationDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecLabelLocationDefault: " + Integer::toString( applierCfg.getSecLabelLocationDefault() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecLabelOffsetOnPkg: " + Integer::toString( applierCfg.getSecLabelOffsetOnPkg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecRotationCalibrationOffset: " + Integer::toString( applierCfg.getSecRotationCalibrationOffset() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecWandCalibrationOffset: " + Integer::toString( applierCfg.getSecWandCalibrationOffset() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): wrGenCfg.setBeltSensor: " + Integer::toString( wrGenCfg.getSealerBeltPackageSensorType() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): wrGenCfg.setUpperFilmPullTension: " + Integer::toString( wrGenCfg.getUpperFilmPullTension() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): wrGenCfg.setlowerFilmPullTension: " + Integer::toString( wrGenCfg.getLowerFilmPullTension() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriRotationCalibrationOffset_90Deg: " + Integer::toString( applierCfg.getPriRotationCalibrationOffset_90Deg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriRotationCalibrationOffset_180Deg: " + Integer::toString( applierCfg.getPriRotationCalibrationOffset_180Deg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriRotationCalibrationOffset_270Deg: " + Integer::toString( applierCfg.getPriRotationCalibrationOffset_270Deg() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecRotationCalibrationOffset_90Deg: " + Integer::toString( applierCfg.getSecRotationCalibrationOffset_90Deg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecRotationCalibrationOffset_180Deg: " + Integer::toString( applierCfg.getSecRotationCalibrationOffset_180Deg() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecRotationCalibrationOffset_270Deg: " + Integer::toString( applierCfg.getSecRotationCalibrationOffset_270Deg() ) );

            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setPriApplierHandAdjust: " + Integer::toString( applierCfg.getPriApplierHandAdjust() ) );
            Debug::trace( "CanTranslator.WrEvolConfig", "translateWrapperCanMsg(): applierCfg.setSecApplierHandAdjust: " + Integer::toString( applierCfg.getSecApplierHandAdjust() ) );

            sendWrapperMessage( &wrapperInquire );
            break;
        }
        case WrEvolTestComplete: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolTestComplete");
            EvolWrapperTestComplete wrTestComplete(this);
            wrTestComplete.setTestCode( (EvolWrapperTest::WrEvolTestCode)pCanMsg->msgData[0] );

            /* This message class is incorrect for what the wrapper is sending. This is why the skip in the data */
            wrTestComplete.setData1( charToShort(pCanMsg->msgData[3], pCanMsg->msgData[4] ) );
            wrTestComplete.setData2( 0 );	/* never used! */
            wrTestComplete.setTestPassed( pCanMsg->msgData[5] );

            wrTestComplete.show();

            sendWrapperMessage( &wrTestComplete );
            break;

        }
        case WrEvolSensors: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolSensors");
            EvolWrapperSensors wrSensors(this);
            EvolWrapperGeneralSensors &genSensors = wrSensors.getGeneralSensors();
            genSensors.setStatus( charToInt( &pCanMsg->msgData[0] ) );

            EvolWrapperInterlockSensors &interlockSensors = wrSensors.getInterlockSensors();
            interlockSensors.setStatus( charToShort( pCanMsg->msgData[4], pCanMsg->msgData[5] ) );

			EvolWrapperGeneralSensors_2 &genSensors_2 = wrSensors.getGeneralSensors_2();
			genSensors_2.setStatus( charToShort(pCanMsg->msgData[6], pCanMsg->msgData[7]) );

            wrSensors.show();
            sendWrapperMessage(&wrSensors);
            break;
        }
        case WrEvolEventCapture: {
            Debug::trace("CanTranslator.WrEvolEventCapture", "translateWrapperCanMsg(): WrEvolEventCapture");
            EvolWrapperEventCapture msg(this);
            msg.setEventCaptureType( (WrEventCaptureType)pCanMsg->msgData[0] );
            msg.setPackageId( pCanMsg->msgData[1]  );
            msg.setValue( charToInt( &pCanMsg->msgData[2] ) );
            msg.setErrorNumber( charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7] ) );

            msg.show();
            sendWrapperMessage(&msg);
            break;

        }
        case WrEvolSystemInfo: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolSystemInfo");

            if ( pCanMsg->numBytesRcvd < 52 )
            {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): WrEvolSystemInfo BAD SIZE!");
                break;
            }

            std::string dbString;

			for ( int i( 0 ); i < pCanMsg->numBytesRcvd; ++i )
				dbString.append( Hex::toString(pCanMsg->msgData[i]) );

			Debug::trace("CanTranslator.Message", "pCanMsg->msgData[]: " + dbString);

            EvolWrapperSystemInfo info(this);
            int offset( 0 );

            // checksum
            info.setBoardChecksumStatus( (bool) charToShort( pCanMsg->msgData[offset+1], pCanMsg->msgData[offset] ) );

            offset += 2;

            // part number
            std::string partNumber;
            for ( unsigned int i( 0 ); i < EvolWrapperSystemInfo::_PartNumbSize_; ++i )
            	partNumber.push_back( makeValidChar( pCanMsg->msgData[offset + i] ) );

            if ( !pCanMsg->msgData[offset + EvolWrapperSystemInfo::_PartNumbSize_ - 1])
            	partNumber[EvolWrapperSystemInfo::_PartNumbSize_ - 1] = '\0';

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo partNumber:" + partNumber );
            info.setMotorBoardPartNumber( partNumber );

            offset += EvolWrapperSystemInfo::_PartNumbSize_;

            // revision
            std::string revision;
            for( unsigned int i( 0 ); i < EvolWrapperSystemInfo::_RevisionSize_; ++i )
            	revision.push_back( makeValidChar( pCanMsg->msgData[offset + i] ) );

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo revision:" + revision );
            info.setMotorBoardRevision( revision );

            offset += EvolWrapperSystemInfo::_RevisionSize_;

            // date of manf
            std::string date;
            for( unsigned int i( 0 ); i < EvolWrapperSystemInfo::_DateSize_; ++i )
                date.push_back( makeValidChar( pCanMsg->msgData[offset + i] ) );

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo date:" + date );
            info.setMotorBoardDateOfManufacture( date );

            offset += EvolWrapperSystemInfo::_DateSize_;

            // checksum
            info.setAwsChecksumStatus( charToShort( pCanMsg->msgData[offset+1], pCanMsg->msgData[offset] ) );

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo checksum:" + Integer::toString( info.getAwsChecksumStatus() ) );

            offset += 2;

            // serial number
            std::string serialNumber;
            for( unsigned int i( 0 ); i < EvolWrapperSystemInfo::_SerialNumbSize_; ++i )
                serialNumber.push_back( makeValidChar( pCanMsg->msgData[offset + i] ) );

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo serialNumber:" + serialNumber );
            info.setAwsSerialNumber(serialNumber);

            offset += EvolWrapperSystemInfo::_SerialNumbSize_;

            // date
            std::string awsDate;
            for(unsigned int i( 0 ); i < EvolWrapperSystemInfo::_DateSize_; ++i )
                awsDate.push_back( makeValidChar( pCanMsg->msgData[offset + i] ) );

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo awsDate:" + awsDate );
            info.setAwsDateOfManufacture( awsDate );

            offset += EvolWrapperSystemInfo::_DateSize_;

            // date of instal
            std::string dateInstall;
            for( unsigned int i( 0 ); i < EvolWrapperSystemInfo::_DateSize_; ++i )
                dateInstall.push_back( makeValidChar( pCanMsg->msgData[offset + i] ) );

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo dateInstall:" + dateInstall );
            info.setAwsDateOfInstall(dateInstall);

            offset += EvolWrapperSystemInfo::_DateSize_;

            // model
            info.setModelNumber( charToShort( pCanMsg->msgData[offset], pCanMsg->msgData[offset+1] ) );
            offset += 2;

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo setModelNumber:" + Integer::toString( info.getModelNumber() ) );

            // cycle
            unsigned int cycleCount( 0 );
            for( unsigned int i( EvolWrapperSystemInfo::_CycleCountSize_ ); i--; )
            {
            	cycleCount <<= 8;
            	cycleCount |= pCanMsg->msgData[offset + i];
            }

            Debug::trace("CanTranslator.Message", "WrEvolSystemInfo cycleCount:" + Integer::toString( cycleCount ) );
            info.setAwsCycleCount( cycleCount );

            offset += EvolWrapperSystemInfo::_CycleCountSize_;

            info.show();
            sendWrapperMessage(&info);
            break;
        }
        case WrEvolErrorHistory: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolErrorHistory" );
            EvolWrapperErrorHistory wrErrorHistory(this);

            wrErrorHistory.setWrapperError(charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
            std::string Trayname = "";
            for(unsigned int i = 1; i < ((EvolWrapperErrorHistory::_MaxNameLength_ ) *2); i++ ) {
                Trayname.append(1, pCanMsg->msgData[i + 1] );
            }
            wrErrorHistory.setTrayName(Trayname);
            wrErrorHistory.setPackageLength( charToShort( pCanMsg->msgData[12], pCanMsg->msgData[13] ));
            wrErrorHistory.setPackageWidth(charToShort( pCanMsg->msgData[14], pCanMsg->msgData[15] ));
            wrErrorHistory.setPackageHeight(charToShort( pCanMsg->msgData[16], pCanMsg->msgData[17] ));
            wrErrorHistory.setUsedBestFit( pCanMsg->msgData[18] );
            wrErrorHistory.setFilmUsed( (WrFilmWidthSelection)pCanMsg->msgData[19] );
            wrErrorHistory.setStretchModified( pCanMsg->msgData[20] );

            wrErrorHistory.show();
            sendWrapperMessage(&wrErrorHistory);
            break;

        }
        case WrEvolCycleCount: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolCycleCount");
            EvolWrapperCycleCount evolWrapperCycleCount(this, charToInt( &pCanMsg->msgData[0] ) );

            evolWrapperCycleCount.show();
            sendWrapperMessage(&evolWrapperCycleCount);
            break;
        }
        case WrEvolPackageDesc: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolPackageDesc");
            EvolWrapperPackageDesc wrPkgDesc(this);
            wrPkgDesc.setId( charToShort( pCanMsg->msgData[1], pCanMsg->msgData[0] ) );
            wrPkgDesc.setLength( charToShort( pCanMsg->msgData[3], pCanMsg->msgData[2] ) );
            wrPkgDesc.setWidth( charToShort( pCanMsg->msgData[5], pCanMsg->msgData[4] ) );
            wrPkgDesc.setHeight( charToShort( pCanMsg->msgData[7], pCanMsg->msgData[6] ) );
            wrPkgDesc.setLip( charToShort( pCanMsg->msgData[9], pCanMsg->msgData[8] ) );
            wrPkgDesc.setFilmWidthSelection( ( WrFilmWidthSelection )pCanMsg->msgData[10] );

            wrPkgDesc.setWrapBoxIndex( charToShort( pCanMsg->msgData[13], pCanMsg->msgData[12] ) );
            wrPkgDesc.setHeightBoxIndex( charToShort( pCanMsg->msgData[15], pCanMsg->msgData[14] ) );
            wrPkgDesc.setUsedWrapBoxBestFit( ((charToShort( pCanMsg->msgData[17], pCanMsg->msgData[16] ) != 0) ? true : false) );
            wrPkgDesc.setModifiedNetStretch( ((charToShort( pCanMsg->msgData[19], pCanMsg->msgData[18] )  != 0) ? true : false) );

            wrPkgDesc.show();
            sendWrapperMessage(&wrPkgDesc);
            break;
        }
        case WrEvolPackageDischarge: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrEvolPackageDischarge");
            EvolWrapperPackageDischarge ewpd(this);
            ewpd.setPackId( (UINT16)pCanMsg->msgData[0] );

            ewpd.show();
            sendWrapperMessage(&ewpd);
            break;
        }
        case WrNgwCycleCount: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrNgwCycleCount");
            NGWCycleCount cycleCount(this);
            cycleCount.setCycleCount(charToInt( &pCanMsg->msgData[0] ) );

            cycleCount.show();
            sendWrapperMessage(&cycleCount);
            break;
        }
        case WrNgwVersion: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrNgwVersion");
            EvolWrapperSystemStatus version(this);
            WrSysStatus	node;

            int x = 0;
            for( int i = 0; i < EvolWrapperSystemStatus::_maxBoards_; i++ ) {

            	node.systems_status[i].nodeid = (unsigned short)pCanMsg->msgData[x++];
            	node.systems_status[i].config_version = (unsigned short)pCanMsg->msgData[x++];
            	//skip over model
            	x++;

            	node.systems_status[i].nodeVersion.appFlash.major = charToShort( pCanMsg->msgData[x], pCanMsg->msgData[x + 1]);
            	if( node.systems_status[i].nodeVersion.appFlash.major == 0xFFFF ) {
            		node.systems_status[i].nodeVersion.appFlash.major = 0;
            	}
            	x += sizeof(unsigned short);

				node.systems_status[i].nodeVersion.appFlash.minor = charToShort( pCanMsg->msgData[x], pCanMsg->msgData[x + 1]);
            	if( node.systems_status[i].nodeVersion.appFlash.minor == 0xFFFF ) {
            		node.systems_status[i].nodeVersion.appFlash.minor = 0;
            	}
				x += sizeof(unsigned short);


				node.systems_status[i].nodeVersion.bootFlash.major = charToShort( pCanMsg->msgData[x], pCanMsg->msgData[x + 1] );
				if( node.systems_status[i].nodeVersion.bootFlash.major == 0xFFFF ) {
            		node.systems_status[i].nodeVersion.bootFlash.major = 0;
            	}
				x += sizeof(unsigned short);

				node.systems_status[i].nodeVersion.bootFlash.minor = charToShort( pCanMsg->msgData[x], pCanMsg->msgData[x + 1]);
				if( node.systems_status[i].nodeVersion.bootFlash.minor == 0xFFFF ) {
            		node.systems_status[i].nodeVersion.bootFlash.minor = 0;
            	}
				x += sizeof(unsigned short);

				node.systems_status[i].wrap_box_checksum = charToInt( &pCanMsg->msgData[x] );
				x += sizeof(unsigned long);

				// no longer used
				node.systems_status[i].nodeVersion.dataFlash.major = 0;
				node.systems_status[i].nodeVersion.dataFlash.minor = 0;

				EvolWrapperNode ngwNode( node.systems_status[i] );

				version.setNode( ngwNode, i );

            }
            version.show();

            sendWrapperMessage(&version);
            break;
        }
        case WrNgwError: {
            Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrNgwError");
            NGWError error(this);

            error.setMechanism( charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
            error.setError( charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] ) );

            error.show();
            sendWrapperMessage(&error);
            break;
        }
        case WrCameraHeightScan: {
        	Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrCameraHeightScan");
        	EvolWrapperCameraHeightScan scan(this);
        	scan.setScan( charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );

        	sendWrapperMessage(&scan);
        	break;
        }

		case WrNGWDimensionOverrides:
		{
			Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrNGWDimensionOverrides");
			break;
		}

        case WrCameraCalibration: {
        	Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrCameraCalibration");
        	EvolWrapperCameraCalibration calib(this);
        	calib.setCamera( (WrapperCamera)pCanMsg->msgData[0] );
        	calib.setResult( pCanMsg->msgData[1] );
        	calib.setScaleFactor( charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] ) );
        	calib.setFirstPixel( charToShort( pCanMsg->msgData[4], pCanMsg->msgData[5] ) );
        	calib.setLastPixel( charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7] ) );

        	calib.show();
        	sendWrapperMessage(&calib);
        	break;
        }
        case WrCameraScan: {
        	Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrCameraScan");

        	EvolWrapperCameraScan wrCameraScan(this);
        	EvolWrapperTest::WrEvolCameraTestParam whichCamera = (EvolWrapperTest::WrEvolCameraTestParam)pCanMsg->msgData[0];

        	if( whichCamera == EvolWrapperTest::RightCameraScanDigital ) {
				wrCameraScan.setCamera( RightCamera );
				wrCameraScan.setImageType( DigitalImage );
			}
			else if( whichCamera == EvolWrapperTest::RightCameraScanAnalog ) {
				wrCameraScan.setCamera( RightCamera );
				wrCameraScan.setImageType( AnalogImage );
			}
			else if( whichCamera == EvolWrapperTest::LeftCameraScanDigital ) {
				wrCameraScan.setCamera( LeftCamera );
				wrCameraScan.setImageType( DigitalImage );
			}
			else if( whichCamera == EvolWrapperTest::LeftCameraScanAnalog ) {
				wrCameraScan.setCamera( LeftCamera );
				wrCameraScan.setImageType( AnalogImage) ;
			}
			else
			{
	        	Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrCameraScan BAD Camera!");
				break;
			}

        	wrCameraScan.setCameraImage( &pCanMsg->msgData[2]);
        	sendWrapperMessage( &wrCameraScan );
        	break;
        }
        case WrEvolCameraThreshold: {
        	EvolWrapperCameraThreshold threshold(this);
        	unsigned short thres = charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1]  );
        	threshold.setWrapperCameraThreshold( (const char *)&thres );
        	sendWrapperMessage( &threshold );
        	break;
        }
        case WrEvolMotorProfile: {
        	EvolWrapperMotorProfile profile(this);
        	profile.setBldcMechanism( (unsigned short)pCanMsg->msgData[0] );
        	profile.setBldcMotorStatus( (unsigned short)pCanMsg->msgData[1]  );
        	profile.setBldcMotorCapacityUsage( charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] ) );
        	sendWrapperMessage( &profile );
        	break;
        }
        case WrEvolManufacturingRunInNotice: {
        	WrapperGeneric notice(WrEvolManufacturingRunInNotice, this);
        	sendWrapperMessage( &notice );
        	break;
        }
        case WrEvolDiagnosticsTestResult: {
        	EvolWrapperDiagnosticsTestResult results(this);
        	results.setTest(charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
        	results.setTestResult( pCanMsg->msgData[2] );
        	results.setOpenLoopOverride( pCanMsg->msgData[3] );
        	results.setDiag( charToShort( pCanMsg->msgData[4], pCanMsg->msgData[5] ) );
        	results.setDiagExpectedResult( charToShort( pCanMsg->msgData[6], pCanMsg->msgData[7] ) );
        	results.setDiagActualResult( charToShort( pCanMsg->msgData[8], pCanMsg->msgData[9] ) );
        	results.setErrorCode(charToShort( pCanMsg->msgData[10], pCanMsg->msgData[11] ) );
        	sendWrapperMessage( &results );
        	break;
        }
        case WrWrapBoxSize: {
        	pCanLibrary->getEventForCobId( pCanMsg->id );

        	// get the size of the incoming wrap box transfer
        	int size = charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] );
			Debug::trace("CanTranslator.Translate", + "WrWrapBoxSize: " + Integer::toString( size ));

			// assign a new message size for the wrap box message
        	pCanLibrary->setMsgSizeForCobId( 0x5e1, size );

        	WrapperGeneric msg(WrWrapBoxSize, this);
			sendWrapperMessage( &msg );
        	break;
        }
        case WrEvolPackageTimings: {
        	EvolWrapperPackageTimings *pWrTimings = new EvolWrapperPackageTimings(this);
        	if( pWrTimings != NULL ) {
        		pWrTimings->setDisposition(pCanMsg->msgData[1]);

        		int numWrapBoxes = pCanMsg->msgData[2];
        		int numHeightBoxes = 0;
        		int index = 3;

        		int tHeightBoxCnt = 0;
        		//get the number of height boxes of each wrap box
        		std::vector<int> boxHeights(numWrapBoxes);
        		for( int i = 0; i < numWrapBoxes; i++ ){
        			tHeightBoxCnt += pCanMsg->msgData[ index ];
        			boxHeights[i] = pCanMsg->msgData[ index++ ];
        		}

                std::string dbString = "";

    			for( int i = 0; i < pCanMsg->numBytesRcvd; i++ ) {
    				dbString.append(Hex::toString(pCanMsg->msgData[i]));
    			}
    			Debug::trace("CanTranslator.WrEvolPackageTimings", "translateWrapperCanMsg() case WrEvolPackageTimings: pCanMsg->msgData[]: " + dbString);


        		Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: numWrapBoxes: " + Integer::toString(numWrapBoxes));
        		Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: numHeightBoxes: " + Integer::toString(tHeightBoxCnt));

        		//create our wrap boxes
        		std::vector<WrapBox> wrapBoxes(numWrapBoxes);



        		//skip to the wrap box information
        		int wbIndex = WrapBox::_WrapBoxOffset_; //+ ( tHeightBoxCnt * WrapBox::_HeightBoxSizeCan_ );

        		Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: wbIndex: " + Integer::toString( wbIndex ) );

        		//translate the wrap box information
				for ( int i = 0; i < numWrapBoxes; i++ ) {

					char boxName[WrapBox::_MaxNameLength_ + 1];
					for(int x = 0; x < WrapBox::_MaxNameLength_; x++ ) {
						boxName[x] = pCanMsg->msgData[ wbIndex++ ];
					}

					// remove trailing spaces
					int x = WrapBox::_MaxNameLength_ - 1;
					for ( ; x >= 0; --x )
						if ( boxName[x] != ' ' )
							break;
					boxName[x + 1] = '\0';
					EvolWrWrapBox wb;
					wb.lengthLow  	= pCanMsg->msgData[ wbIndex++ ];
					wb.lengthHigh 	= pCanMsg->msgData[ wbIndex++ ];
					wb.widthLow 	= pCanMsg->msgData[ wbIndex++ ];
					wb.widthHigh 	= pCanMsg->msgData[ wbIndex++ ];
					wb.lipHeight 	= pCanMsg->msgData[ wbIndex++ ];

					//Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: pCanMsg->msgData[ wbIndex ]: " + Integer::toString(pCanMsg->msgData[ wbIndex ]));
					//Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: pCanMsg->msgData[ wbIndex + 1 ]: " + Integer::toString(pCanMsg->msgData[ wbIndex+1 ]));

					wb.pluNumHigh =  charToShort( pCanMsg->msgData[ wbIndex ], pCanMsg->msgData[ wbIndex+1 ] );

					//Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: wb.pluNumHigh: " + Integer::toString(wb.pluNumHigh));

					wbIndex += sizeof(unsigned short);
                    //Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: pCanMsg->msgData[ wbIndex ]: " + Integer::toString(pCanMsg->msgData[ wbIndex ]));
					//Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: pCanMsg->msgData[ wbIndex+1 ]: " + Integer::toString(pCanMsg->msgData[ wbIndex+1 ]));
					wb.pluNumLow  = charToShort( pCanMsg->msgData[ wbIndex ], pCanMsg->msgData[ wbIndex+1 ] );

					//Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: wb.pluNumLow: " + Integer::toString(wb.pluNumLow));
					int plu = wb.pluNumHigh;
					plu <<= 16;
					plu |= wb.pluNumLow;
					wbIndex += sizeof(unsigned short);

					// add the wrap box to the container
					wrapBoxes[i] = WrapBox( boxName, plu,
											wb.lengthLow, wb.lengthHigh,
											wb.widthLow, wb.widthHigh, wb.lipHeight );
					wrapBoxes[i].show(" ");
					//Debug::trace("CanTranslator.WrEvolPackageTimings", + "translateWrapperCanMsg() case WrEvolPackageTimings: offset: " + Integer::toString(wbIndex));
				}
        		//move our index to the start of the height boxes
        		index = WrapBox::_WrapBoxOffset_ + ( WrapBox::_WrapBoxSizeCan_ * numWrapBoxes );


        		//translate and insert the number of height boxes into each wrap box
    			for ( int i = 0; i < numWrapBoxes; ++i ) {

    				WrapBox& wrapBox = wrapBoxes[i];

					// translate the height information
					for ( int j = 0; j < boxHeights[i]; ++j ) {
						WrapBox::HeightBox heightBox( pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::PERCENT_STRETCH_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::CLAMP_HEIGHT_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::ELEVATOR_PLACEMENT_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::GRIPPER_ADJUST_PARAMETER, pCanMsg->msgData[ index++ ] );

						heightBox.setParameter( WrapBox::WRAP_SPEED_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::DISCHARGE_SPEED_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::SUF_ADJUST_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::HEIGHT_RESTRICT_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::SUF_RETURN_PARAMETER, 0 );										//parameter no longer used
						heightBox.setParameter( WrapBox::GRIPPER_SHORT_PULL_PARAMETER, 0);								//parameter no longer used
						heightBox.setParameter( WrapBox::SIDE_CLAMP_GAP_PARAMETER, 0);									//parameter no longer used
						heightBox.setParameter( WrapBox::FILM_ROLL_USED_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::RUF_TRIP_DELAY_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::BRAKE_TIMING_PARAMETER, pCanMsg->msgData[ index++ ] );
						heightBox.setParameter( WrapBox::KNIFE_HEIGHT_PARAMETER, pCanMsg->msgData[ index++ ] );  		//parameter changed to MfgTrayWidthSpec
						heightBox.setParameter( WrapBox::FUF_ADJUST_PARAMETER, pCanMsg->msgData[ index++ ] );			//parameter changed to TrailingEdgLabelAdj
						heightBox.setParameter( WrapBox::GRIPPER_RELEASE_PARAMETER, 0); 								//parameter no longer used
						heightBox.setParameter( WrapBox::GRIPPER_RELAX_HOME_PARAMETER, 0); 								//parameter no longer used
                        // THIS NEEDS FIXED for 4.2.300 - FIXED 01-10-2023 - USB WILL NEVER call CAN Messages so we better NEVER see this
                        if (FpmManager::getInstance()->isWrapperTwoPrinter())
                        {
                            heightBox.setParameter(WrapBox::ADDED_LENGTH_PARAMETER, pCanMsg->msgData[index++]);			    //leading edge label adjust
                            heightBox.setParameter(WrapBox::PRIMARY_LABEL_VERT_OFFSET, pCanMsg->msgData[index++]);			// new primary label vertical offset
                            heightBox.setParameter(WrapBox::SECONDARY_LABEL_VERT_OFFSET, pCanMsg->msgData[index++]);		// new secondary label vertical offset
                        }

						heightBox.show(" ");
						//insert the height box into the wrap box
						wrapBox.insertHeightBox(heightBox);
					}
    			}

    			for( int i = 0; i < numWrapBoxes; i++ ) {
    				pWrTimings->addWrapBox( wrapBoxes[i] );
    			}

    			Debug::trace("CanTranslator.WrEvolPackageTimings", "translateWrapperCanMsg() case WrEvolPackageTimings: pWrTimings: " + Hex::toString((long) pWrTimings) );

    			EvolWrapperPackageTimingsPointer timings( WrEvolPackageTimings, pWrTimings );
    			sendWrapperMessage( &timings );

         	} else {
        		Debug::trace("CanTranslator.Error",  "translateWrapperCanMsg() case WrEvolPackageTimings: pWrTimings is NULL! Unable to translate: WrEvolPackageTimings");
        	}
        	break;
        }
        case WrWrapBoxAck: {
        	WrapperGeneric msg( WrWrapBoxAck, this );
        	sendWrapperMessage( &msg );
        	break;
        }
        case WrWrapBoxTransferComplete: {
        	WrapperGeneric msg( WrWrapBoxTransferComplete, this );
        	sendWrapperMessage( &msg );
        	break;
        }
        case WrNGWBootStatus: {
        	Debug::trace("CanTranslator.Message", "translateWrapperCanMsg(): WrNGWBootStatus");
        	NGWBootStatus status( this );
        	status.setNodeId( pCanMsg->msgData[0] );
        	status.setStatus( (NGWBootStatus::LoaderStatus) pCanMsg->msgData[1] );
        	status.setFaultAddress( charToInt( &pCanMsg->msgData[2] ) );
        	status.setFault( (NGWBootStatus::LoaderFaults) pCanMsg->msgData[6] );

        	const char *pEvolMsg = status.serialize();
            int mLength = status.getSerializedLength();

            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error",  "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWBootStatus");
            }
        	break;
        }
        case WrNGWBootVersion: {
        	NGWBootVersion bv(this);
        	bv.setNodeId( pCanMsg->msgData[1] );
        	bv.setAppMajorVersion( charToShort( pCanMsg->msgData[14], pCanMsg->msgData[15] ) );
        	bv.setAppMinorVersion( charToShort( pCanMsg->msgData[16], pCanMsg->msgData[17] ) );
        	bv.setDataMajorVersion( charToShort( pCanMsg->msgData[18], pCanMsg->msgData[19] ) );
        	bv.setDataMinorVersion( charToShort( pCanMsg->msgData[20], pCanMsg->msgData[21] ) );
        	bv.setBootMajorVersion( charToShort( pCanMsg->msgData[22], pCanMsg->msgData[23] ) );
        	bv.setBootMinorVersion( charToShort( pCanMsg->msgData[24], pCanMsg->msgData[25] ) );

        	const char *pEvolMsg = bv.serialize();
            int mLength = bv.getSerializedLength();

            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWBootVersion");
            }
        	break;
        }
        case WrNGWAck: {
        	NGWAck wrAck(this);
        	wrAck.setNodeId(pCanMsg->msgData[0]);
        	wrAck.setSourceId(pCanMsg->msgData[1]);

        	const char *pEvolMsg = wrAck.serialize();
            int mLength = wrAck.getSerializedLength();

            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWAck");
            }
        	break;
        }
        case WrNGWFlush: {

        	NGWFlush wrFlush(this);
        	wrFlush.setNodeId(pCanMsg->msgData[0]);
        	wrFlush.setSourceId(pCanMsg->msgData[1]);

            const char *pEvolMsg = wrFlush.serialize();
            int mLength = wrFlush.getSerializedLength();
            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWFlush");
            }
        	break;
        }
        case WrNGWEngInfoN1: {
        	NGWEngInfo info( this, (NGWEngInfo::EngInfoType)pCanMsg->msgData[0] );
        	info.setEngInfo( (unsigned char *)&pCanMsg->msgData[1], ( pCanMsg->numBytesRcvd - 1 ) );
        	info.setEventType(WrNGWEngInfoN1);
			
            const char *pEvolMsg = info.serialize();
            int mLength = info.getSerializedLength();
            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWEngInfoN1");
            }
        	break;
        }
        case WrNGWEngInfoN2: {
        	NGWEngInfo info( this, (NGWEngInfo::EngInfoType)pCanMsg->msgData[0] );
        	info.setEngInfo( (unsigned char *)&pCanMsg->msgData[1], ( pCanMsg->numBytesRcvd - 1 ) );
        	info.setEventType(WrNGWEngInfoN2);
            const char *pEvolMsg = info.serialize();
            int mLength = info.getSerializedLength();
            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWEngInfoN2");
            }
        	break;
        }
        case WrNGWEngInfoN3: {
        	NGWEngInfo info( this, (NGWEngInfo::EngInfoType) pCanMsg->msgData[0] );
        	info.setEngInfo( (unsigned char *)&pCanMsg->msgData[1], ( pCanMsg->numBytesRcvd - 1 ) );
        	info.setEventType(WrNGWEngInfoN3);
            const char *pEvolMsg = info.serialize();
            int mLength = info.getSerializedLength();
            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWEngInfoN3");
            }
        	break;
        }
        case WrNGWEngInfoN4: {
        	NGWEngInfo info( this, (NGWEngInfo::EngInfoType)pCanMsg->msgData[0] );
        	info.setEngInfo( (unsigned char *)&pCanMsg->msgData[1], ( pCanMsg->numBytesRcvd - 1 ) );
        	info.setEventType(WrNGWEngInfoN4);
            const char *pEvolMsg = info.serialize();
            int mLength = info.getSerializedLength();
            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWEngInfoN4");
            }
        	break;
        }
        case WrNGWEngInfoN5: {
        	NGWEngInfo info( this, (NGWEngInfo::EngInfoType)pCanMsg->msgData[0] );
        	info.setEngInfo( (unsigned char *)&pCanMsg->msgData[1], ( pCanMsg->numBytesRcvd - 1 ) );
        	info.setEventType(WrNGWEngInfoN5);
            const char *pEvolMsg = info.serialize();
            int mLength = info.getSerializedLength();
            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWEngInfoN5");
            }
        	break;
        }
        case WrNGWEngInfoN6: {
        	NGWEngInfo info( this, (NGWEngInfo::EngInfoType)pCanMsg->msgData[0] );
        	Debug::trace( "CanTranslator", "translateWrapperCanMsg(): WrNGWEngInfoN6 " + Integer::toString(pCanMsg->numBytesRcvd - 2));
        	info.setEngInfo( (unsigned char *)&pCanMsg->msgData[1], pCanMsg->numBytesRcvd - 1 );
        	info.setEventType(WrNGWEngInfoN6);
            const char *pEvolMsg = info.serialize();
            int mLength = info.getSerializedLength();
            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWEngInfoN6");
            }
        	break;
        }
        case WrNGWENGException: {
        	NGWEngException exception(this, (NGWEngException::MechanismType)pCanMsg->msgData[0], (NGWEngException::NGWException)pCanMsg->msgData[1] );
            const char *pEvolMsg = exception.serialize();
            int mLength = exception.getSerializedLength();
            if(pUpdateManager) {
            	pUpdateManager->send(pEvolMsg, mLength);
            } else {
                Debug::trace("CanTranslator.Error", "translateWrapperCanMsg(): pUpdateManager is NULL! Unable to translate: WrNGWENGException");
            }
        	break;
        }
        case WrPPPWakeup: {
            Debug::trace( "CanTranslator", "translateWrapperCanMsg(): WrPPPWakeup");

            PPPWakeup wake(this);
            wake.setProductId( charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
            wake.setRate( charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] ) );
            wake.setSoftwareMajor( pCanMsg->msgData[4] );
            wake.setSoftwareMinor( pCanMsg->msgData[5] );
            wake.setSoftwareBuild( pCanMsg->msgData[6] );
            wake.setHardwareMajor( pCanMsg->msgData[7] );
            wake.setHardwareMinor( pCanMsg->msgData[8] );
            wake.show();
            sendWrapperMessage( &wake );    
            break;
        }
        case WrPPPConfig: {
            Debug::trace( "CanTranslator", "translateWrapperCanMsg(): WrPPPConfig");

            PPPConfig config(this, (unsigned long)pCanMsg->msgData[0], (unsigned long)pCanMsg->msgData[1], charToShort( pCanMsg->msgData[2], pCanMsg->msgData[3] ) );
                                         
            config.setPrimarySlope( charToInt( &pCanMsg->msgData[4] ) );
            config.setSecondarySlope( charToInt( &pCanMsg->msgData[8] ) );
            config.setPrimaryPlateHomeDistance( charToShort( pCanMsg->msgData[12], pCanMsg->msgData[13] ) );
            config.setSecondaryPlateHomeDistance( charToShort( pCanMsg->msgData[14], pCanMsg->msgData[15] ) );
            config.setMechanicalHomeSpec( charToShort( pCanMsg->msgData[16], pCanMsg->msgData[17] ) );
            #if 0   /* VL53L1X */
            config.setXFieldOfView( pCanMsg->msgData[18] );
            config.setYFieldOfView( pCanMsg->msgData[19] );
            config.setCenterFieldOfView( pCanMsg->msgData[20] );            
            #endif
                        
            config.show();
            sendWrapperMessage( &config );
            break;
        }
        case WrPPPDist: {
            //Debug::trace( "CanTranslator", "translateWrapperCanMsg(): WrPPPDist");

            PPPDist distance(this, (PPPDist::PrPlate)pCanMsg->msgData[2], (unsigned long)charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ), 0 );
            distance.show();
            sendWrapperMessage( &distance );
            break;
        }
        case WrPPPCalibration: {
            Debug::trace( "CanTranslator", "translateWrapperCanMsg(): WrPPPCalibration");
            PPPCalib cal(this);
            cal.setCalPlate0Status( (PPPCalib::CalStatus)pCanMsg->msgData[0] );
            cal.setCalDistance0Millimeter( charToInt( &pCanMsg->msgData[1] ) );
            cal.setCalPlate1Status( (PPPCalib::CalStatus)pCanMsg->msgData[5] );
            cal.setCalDistance1Millimeter( charToInt( &pCanMsg->msgData[6] ) );
            cal.show();
            sendWrapperMessage( &cal );
            break;
        }
        case WrPPPVersion: {
            PPPVersion ver( this );
            ver.setProductId( charToShort( pCanMsg->msgData[0], pCanMsg->msgData[1] ) );
            ver.setSoftwareMajor( pCanMsg->msgData[2] );
            ver.setSoftwareMinor( pCanMsg->msgData[3] );
            ver.setSoftwareBuild( pCanMsg->msgData[4] );
            ver.setHardwareMajor( pCanMsg->msgData[5] );
            ver.setHardwareMinor( pCanMsg->msgData[6] );
            ver.show();
            sendWrapperMessage( &ver );
            break;
        }
        case WrPPPBootVersion: {
            PPPVersion ver( this );
            ver.setBootMajor( pCanMsg->msgData[0] );
            ver.setBootMinor( pCanMsg->msgData[1] );
            ver.setBootBuild( pCanMsg->msgData[2] );
            ver.show();
            sendWrapperMessage( &ver );
            break;
        }
        case WrPPPStatus: {
            Debug::trace( "CanTranslator", "translateWrapperCanMsg(): WrPPPStatus");
            PPPStatus status( this );
            status.setMode( (PPPStatus::PPPMode)pCanMsg->msgData[0] );
            status.setSensorStatus( (PPPStatus::SensorStatus)pCanMsg->msgData[1] );
            status.show();
            sendWrapperMessage( &status );
            break;
        }
        case WrWrapperPause: {
			// SOF-5459 Printer Plate Adjust Message - Do not need this for a single sliding 2nd printer so can it all
			Debug::trace("CanTranslator.Error", "translateWrapperEvent(): case translateWrapperCanMsg: probably should not be getting this event any more ");
			/*
            Debug::trace( "CanTranslator", "translateWrapperCanMsg(): WrWrapperPause");
            NGWWrapperPause msg( this );
            msg.setState( (NGWWrapperPause::NGWState)pCanMsg->msgData[0] );
            msg.setPackageId( pCanMsg->msgData[1] );
            msg.setPrimaryPlatePosition( pCanMsg->msgData[2] );
            msg.setRequiredPrimaryPlatePosition( pCanMsg->msgData[3] );
            msg.setRequiredSecondaryPlatePosition( pCanMsg->msgData[4] );
            msg.setRequiredSecondaryPlatePosition( pCanMsg->msgData[5] );
            msg.show();
            sendWrapperMessage( &msg );
			*/
            break;
        }
        default: {
            Debug::trace( "CanTranslator.Error", "translateWrapperCanMsg(): Invalid wrapper event " + Integer::toString(hobEvent) + " for CobId  " + Hex::toString(  pCanMsg->id ));
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendRequestWeigherWakeup() {
    sendWeigherRequest(WgReqWakeup);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendRequestAllPrinterWakeup() {
    sendPrinterRequest( PrReqWakeup, FirstPrinter );
    sendPrinterRequest( PrReqWakeup, SecondPrinter );
    sendPrinterRequest( PrReqWakeup, ThirdPrinter );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendRequestPrinterWakeup(PrinterType prType) {
    sendPrinterRequest( PrReqWakeup, prType );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendRequestWrapperWakeup() {
    sendWrapperRequest( WrReqWakeup );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendWeigherRequest(WgEvent wgEvent)
{
    int nBytes;

    can_frame canMsg;
    memset( &canMsg, 0, sizeof(can_frame) );

    Debug::trace( "CanTranslator","sendWeigherRequest()" );

    switch (wgEvent) {
        case WgRezero:
        case WgColdReset:
        case WgReset:
        case WgDisable:
        case WgEnable:
        case WgControl:
        case WgReqStatus:
        case WgReqInquire:
        case WgReqWakeup:
        case WgReqKind:
        case WgReqVersion:
        case WgReqSerialNum:
		case WgReqValueMax:
		{
            if( pCanLibrary != NULL ) {
                canMsg.can_id = pCanLibrary->getWeigherCobID(wgEvent);
                canMsg.can_dlc = 1;
                canMsg.data[0] = 0;

                Debug::trace( "CanTranslator", "sendWeigherRequest(): Sending CAN frame: " + Hex::toString(canMsg.can_id) );
                if(canMsg.can_id != 0) {
                    nBytes = pCanManager->send(&canMsg);
                    if( nBytes != sizeof(canMsg) ) {
                        Debug::trace( "CanTranslator.Error", "sendWeigherRequest(): Failed to send Weigher message. Sent: " + Integer::toString(nBytes) + " bytes of: " + Integer::toString( sizeof(canMsg)) );
                    }
                }
            } else {
                Debug::trace( "CanTranslator", "sendWeigherRequest(): Library not initialized!" );
            }
            break;
        }
        default:
            Debug::trace("CanTranslator.Error", "sendWeigherRequest(): Unsupported WgRequest:"+ HobString(Integer::toString(wgEvent)));
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendPrinterRequest(PrEvent prEvent, PrinterType prType) {

#if 0 //unsupported
    Debug::trace( "CanTranslator","sendPrinterRequest()" );
    PrRequestCode reqCode = REQUEST_WAKEUP;
    switch (prEvent) {
        case PrReqWakeup:
            reqCode = REQUEST_WAKEUP;
            break;
        case PrReqInquire:
            reqCode = REQUEST_CONFIGURATION;
            break;
        case PrReqStatus:
            reqCode = REQUEST_STATUS;
            break;
        case PrReqSensors:
            reqCode = REQUEST_SENSORS;
            break;
        case PrReqVersion:
            reqCode = REQUEST_VERSION;
            break;
        default:
            Debug::trace("CanTranslator.Error", "sendPrinterRequest(): Unsupported PrRequest: " + HobString(Integer::toString(prEvent)));
            return;
    }
    //Set up the CAN frame information
    can_frame canMsg;
    canMsg.can_id   = pCanLibrary->getPrinterCobID(PrRequest, prType);
    canMsg.can_dlc  = 1;
    canMsg.data[0]  = reqCode;
    Debug::trace("CanTranslator.Translate", "sendPrinterRequest(): can_id: 0x " + Hex::toString(canMsg.can_id) + ", reqCode:"+Integer::toString(reqCode));
    sendCanFrame( canMsg );
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendWrapperRequest(WrEvent wrEvent)
{
	if ( !pCanManager )
		return;

	can_frame canMsg;
	memset( &canMsg, 0, sizeof(can_frame) );

    switch (wrEvent)
    {
	case WrReqWakeup:
	case WrReqInquire:
	case WrReqStatus:
	case WrReqSensors:
	case WrReqEvolSystemInfo:
	case WrEvolSystemInfoErase:
	case WrEvolReqCycleCount:
	case WrEvolReqSystemReset:
	case WrEvolRequestCameraThreshold:
	case WrNgwReqVersion:
	case WrEvolStartMfgRunin:
	case WrEvolResetDefaultPackageTimings:
	case WrEvolSavePackageTimings:
	{
		canMsg.can_id = pCanLibrary->getWrapperGenericReqCobID();
		canMsg.can_dlc = 2;
		canMsg.data[0] = pCanLibrary->getWrapperRequest( wrEvent );
		canMsg.data[1] = 0;

		if(canMsg.can_id != 0)
		{
			const int nBytes( pCanManager->send(&canMsg) );

			if( nBytes != sizeof(canMsg) )
				Debug::trace( "CanTranslator.Error", "sendWrapperGenericRequest(): Failed to send Printer message. Sent: " + Integer::toString(nBytes) + " bytes of: " + Integer::toString( sizeof(canMsg)) );
		}

       	break;
	}

    default:
		Debug::trace("CanTranslator.Error", "sendWrapperRequest(): Unsupported WrRequest:" + HobString(Integer::toString(wrEvent)));
		break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendLargeCANMessage(const char* recvBuffer, unsigned int size, unsigned int cobId, bool littleEndian) {

    unsigned int bytesSentSoFar = 0;
    unsigned int canDataIter = 0;

    can_frame canMsg;
    memset( &canMsg, 0, sizeof(can_frame) );
    
    //First, we will do some error checking
    if(!recvBuffer) {//No message to send
        Debug::trace("CanTranslator.Error", "sendLargeCANMessage(): Message is NULL! Ignoring");
        return;
    }
    if(!size) {//Size is 0, don't need to send anything
        Debug::trace("CanTranslator.Error", "sendLargeCANMessage(): Size is 0! Ignoring");
        return;
    }
    if(!pCanLibrary->acceptedCobId(cobId)) {//We don't currently support this cobId
        Debug::trace("CanTranslator.Error", "sendLargeCANMessage(): Unsupported cobId:"+Integer::toString(cobId)+", ignoring");
        return;
    }
    //Break up the large message into 8 byte transfer chunks to transfer over CAN
    while(bytesSentSoFar<size) {
        //Set up the CAN frame for transfer
        canMsg.can_id   = cobId;    //CAN cobId
        
        canDataIter = 0;
        while(canDataIter < 8 && ((bytesSentSoFar+canDataIter)< size)) {//CAN Data
            if(littleEndian) {
                canMsg.data[canDataIter] = recvBuffer[size-bytesSentSoFar-canDataIter-1];//Send the message in reverse order (LittleEndian)
            } else {
                canMsg.data[canDataIter] = recvBuffer[bytesSentSoFar+canDataIter];
            }
            ++canDataIter;
        }
        canMsg.can_dlc  = canDataIter;        //CAN Data Size
        
        //Send the CAN frame
        sendCanFrame(canMsg);
        
        //Increment our send variables
        bytesSentSoFar += canDataIter;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanTranslator::isTranslatable(canid_t id) {
	bool translatable = false;
	if(pCanLibrary != NULL) {
		translatable = pCanLibrary->frameFilter(id);
	}
	return translatable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendCanFrame( can_frame& canMsg ) {

    unsigned int nBytes = 0;
    
    if(canMsg.can_id != 0) {
        nBytes = pCanManager->send( &canMsg );
        if( nBytes != sizeof(canMsg) ) {
            Debug::trace( "CanTranslator.Error", "sendCanFrame(): Failed to send Printer message. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString( sizeof(canMsg)) );
        }
    } else {
        Debug::trace( "CanTranslator.Error", "sendCanFrame(): Failed to get COB id for frame: " + Integer::toString(canMsg.can_id));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short CanTranslator::charToShort(unsigned char msbByte, unsigned char lsbByte) {
    unsigned short result;
    result = (unsigned short)msbByte;
    result <<= 8;
    result |= (unsigned short)lsbByte;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int  CanTranslator::charToInt(const unsigned char *pMsbByte) {
    unsigned int result;
    result = *pMsbByte++;
    result <<= 8;
    result |= *pMsbByte++;
    result <<= 8;
    result |= *pMsbByte++;
    result <<= 8;
    result |= *pMsbByte;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char CanTranslator::getMsbFromShort(unsigned short value) {
    std::vector<unsigned char> v;
    v.assign( reinterpret_cast<unsigned char *>( &value ), reinterpret_cast<unsigned char *>( &value ) + sizeof( value ) );
    std::vector<unsigned char>::iterator iter = ( v.end() - 1);
    return *iter;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char CanTranslator::getLsbFromShort(unsigned short value) {
    std::vector<unsigned char> v;
    v.assign( reinterpret_cast<unsigned char *>( &value ), reinterpret_cast<unsigned char *>( &value ) + sizeof( value ) );
    std::vector<unsigned char>::iterator iter = v.begin();
    return *iter;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char CanTranslator::getMsbFromLong(unsigned long value) {
    std::vector<unsigned char> v;
    v.assign( reinterpret_cast<unsigned char *>( &value ), reinterpret_cast<unsigned char *>( &value ) + sizeof( value ) );
    std::vector<unsigned char>::iterator iter = ( v.end() - 1);
    return *iter;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char CanTranslator::getCharFromLong(unsigned long value, int index) {
    std::vector<unsigned char> v;
    v.assign( reinterpret_cast<unsigned char *>( &value ), reinterpret_cast<unsigned char *>( &value ) + sizeof( value ) );
    if( index <= v.size() ) {
        return v[index];
    } else {
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char CanTranslator::getLsbFromLong(unsigned long value) {
    std::vector<unsigned char> v;
    v.assign( reinterpret_cast<unsigned char *>( &value ), reinterpret_cast<unsigned char *>( &value ) + sizeof( value ) );
    std::vector<unsigned char>::iterator iter = v.begin();
    return *iter;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::registerWeigherEvents() {

	Debug::trace("CanTranslator", "registerWeigherEvents(): " + mySystemType );

	if( ( mySystemType == "HTK" ) || ( mySystemType == "NGW2" ) || ( mySystemType == "FSS" ) )
	{
		registerEvent(Weigher, WgKPCDetached);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::registerPrinterEvents() {

	Debug::trace("CanTranslator", "registerPrinterEvents(): " + mySystemType );

	if( ( mySystemType == "HTK") || ( mySystemType == "FSS" ) )
	{
		Debug::trace("CanTranslator", "mySystemType IS HTK so register PrReqCutterStatus with mySystemType: " + mySystemType);
		Debug::trace("CanTranslator.HeadDot", "mySystemType IS HTK so register PrReqHeadDotStatus with mySystemType: " + mySystemType);
		Debug::trace("CanTranslator.HeadDot", "mySystemType IS HTK so register PrReqHeadDotWear with mySystemType: " + mySystemType);

		registerEvent( Printer, PrKPCDetached );
		registerEvent( Printer, PrReqLabelTransfer);
        registerEvent( Printer, PrReqHeadDotStatus);
        registerEvent( Printer, PrReqHeadDotWear);
        registerEvent( Printer, PrHeadDotAck);
        registerEvent( Printer, PrReqCutterStatus);
		registerEvent( Printer, PrCutterCut);
	}
	else
	{
		Debug::trace("CanTranslator.HeadDot", "mySystemType IS NOT HTK so MISSED register PrReqCutterStatus with mySystemType: " + mySystemType);
		Debug::trace("CanTranslator.HeadDot", "mySystemType IS NOT HTK so MISSED register PrReqHeadDotStatus with mySystemType: " + mySystemType);
		Debug::trace("CanTranslator.HeadDot", "mySystemType IS NOT HTK so MISSED register PrReqHeadDotWear with mySystemType: " + mySystemType);

	}
}
void CanTranslator::registerKPCBootEvents()
{
    Debug::trace("CanTranslator", "registerKPCBootEvents(): " + mySystemType);
    if ( ( mySystemType == "HTK" ) || ( mySystemType == "NGW2" ) || ( mySystemType == "FSS" ) ) // also want to register for 2nd label applier
    {
        registerEvent(KPCBootloader, KpcBootReqStatus);
        registerEvent(KPCBootloader, KpcBootCmd);
        registerEvent(KPCBootloader, KpcBootFrame);
        registerEvent(KPCBootloader, KpcBootFrame1);
        registerEvent(KPCBootloader, KpcBootFrame2);
        registerEvent(KPCBootloader, KpcBootFrame3);
        registerEvent(KPCBootloader, KpcBootFrame4);
        registerEvent(KPCBootloader, KpcBootFrame5);
        registerEvent(KPCBootloader, KpcBootFrame6);
        registerEvent(KPCBootloader, KpcBootFrame7);
        registerEvent(KPCBootloader, KpcBootFrame8);
        registerEvent(KPCBootloader, KpcBootFlush);
        registerEvent(KPCBootloader, KpcBootReqVersion);
        registerEvent(KPCBootloader, KpcBootBulkMsg);
        registerEvent(KPCBootloader, KpcBootDone);
        registerEvent(KPCBootloader, KpcBootReqAppVersion);
    }
}

void CanTranslator::registerKPCBootUpgraderEvents()
{

    Debug::trace("CanTranslator", "registerKPCBootEvents(): " + mySystemType);

    if ( ( mySystemType == "HTK" ) || ( mySystemType == "NGW2" ) || ( mySystemType == "FSS" ) )
    {
        registerEvent(KPCBootUpdater, KpcBootUpgraderReqStatus);
        registerEvent(KPCBootUpdater, KpcBootUpgraderCmd);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame1);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame2);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame3);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame4);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame5);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame6);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame7);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFrame8);
        registerEvent(KPCBootUpdater, KpcBootUpgraderFlush);
        registerEvent(KPCBootUpdater, KpcBootUpgraderReqVersion);
        registerEvent(KPCBootUpdater, KpcBootUpgraderBulkMsg);
        registerEvent(KPCBootUpdater, KpcBootUpgraderDone);
    }
}

void CanTranslator::deregisterKPCBootUpgraderEvents()
{

    Debug::trace("CanTranslator", "registerKPCBootEvents(): " + mySystemType);

    if ( ( mySystemType == "HTK" ) || ( mySystemType == "NGW2" ) || ( mySystemType == "FSS" ) )
    {
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderReqStatus);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderCmd);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame1);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame2);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame3);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame4);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame5);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame6);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame7);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFrame8);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderFlush);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderReqVersion);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderBulkMsg);
        deregisterEvent(KPCBootUpdater, KpcBootUpgraderDone);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::registerWrapperEvents() {

	// AWS3/NGW1 is hybrid CAN/USB system
	if( ( mySystemType == "AWS3" ) || ( mySystemType == "NGW1" ) )
	{
		Debug::trace( "CanTranslator", "registerWrapperEvents()" );

		// AWS3 and NGW1 only support CAN upgrade messages and a few operational messages
		registerEvent(Wrapper, WrNgwReqVersion);

		registerEvent(Wrapper, WrNGWReqBootStatus);
		registerEvent(Wrapper, WrNGWBootCMD);
		registerEvent(Wrapper, WrNGWRequestBootVersion);
		registerEvent(Wrapper, WrNGWBootFrame);
		registerEvent(Wrapper, WrNGWAck);
		registerEvent(Wrapper, WrNGWFlush);
		registerEvent(Wrapper, WrNGWBulkFrame);

		//add wrap messaging to the AWS3/NGW1
		registerEvent(Wrapper, WrEvolWrap);
		registerEvent(Wrapper, WrMode);
		registerEvent(Wrapper, WrLabelReady);	//Not support in AWS3
		registerEvent(Wrapper, WrLabelTaken);	//Not support in AWS3
		registerEvent(Wrapper, WrLabelSetup);	//Not support in AWS3
		registerEvent(Wrapper, WrEvolControl);
		registerEvent(LabelApplier, LaSetup);
		registerEvent(LabelApplier, LaLabelReady);
		registerEvent(LabelApplier, LaLabelTaken);

		registerEvent(Wrapper, WrNGWWrapperDetached);

	//NGW3/NGW2 are full bright CAN systems
	} else if( ( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" ) ) {

		registerEvent(Wrapper, WrNgwReqVersion);

		registerEvent(Wrapper, WrNGWReqBootStatus);
		registerEvent(Wrapper, WrNGWBootCMD);
		registerEvent(Wrapper, WrNGWRequestBootVersion);
		registerEvent(Wrapper, WrNGWBootFrame);
		registerEvent(Wrapper, WrNGWAck);
		registerEvent(Wrapper, WrNGWFlush);
		registerEvent(Wrapper, WrNGWBulkFrame);

		//add wrap messaging to the AWS3
		registerEvent(Wrapper, WrEvolWrap);
		registerEvent(Wrapper, WrMode);
		registerEvent(Wrapper, WrLabelReady);	//Not support in AWS3
		registerEvent(Wrapper, WrLabelTaken);	//Not support in AWS3
		registerEvent(Wrapper, WrLabelSetup);	//Not support in AWS3
		registerEvent(Wrapper, WrEvolControl);
		registerEvent(LabelApplier, LaSetup);
		registerEvent(LabelApplier, LaLabelReady);
		registerEvent(LabelApplier, LaLabelTaken);

		//registerEvent(Wrapper, WrEvolSystemInfo);
		registerEvent(Wrapper, WrReqEvolSystemInfo);
		registerEvent(Wrapper, WrEvolSystemInfoErase);
		registerEvent(Wrapper, WrEvolSystemInfoWrite);
		registerEvent(Wrapper, WrReqInquire);
		registerEvent(Wrapper, WrEvolConfig);
		registerEvent(Wrapper, WrReqSensors);
		registerEvent(Wrapper, WrReqWakeup);
		registerEvent(Wrapper, WrEvolTest);
		registerEvent(Wrapper, WrEvolWriteCameraThreshold);
		registerEvent(Wrapper, WrEvolDiagnosticsTest);
		registerEvent(Wrapper, WrNgwReqVersion);
		registerEvent(Wrapper, WrLoad);
		registerEvent(Wrapper, WrEvolStartMfgRunin);
		registerEvent(Wrapper, WrEvolDiagnosticsTest);
		registerEvent(Wrapper, WrEvolRequestPackageTimings);
		registerEvent(Wrapper, WrEvolResetDefaultPackageTimings);
		registerEvent(Wrapper, WrWrapBoxAck);
		registerEvent(Wrapper, WrWrapBoxSizeSave);
		registerEvent(Wrapper, WrEvolSavePackageTimings);
		registerEvent(Wrapper, WrEvolPackageTimingsWrite);
		registerEvent(Wrapper, WrNGWWrapperDetached);
		registerEvent(Wrapper, WrNGWDimensionOverrides);
        registerEvent(Wrapper, WrPPPRequest );

        registerEvent(NGWrapper, _WrStartKPC);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendPrinterMessage(const char* msg, int msgLength, PrinterType prType) {

	if( prType == FirstPrinter ) {
        if(pPrinter1Manager) {
            pPrinter1Manager->send(msg, msgLength);
        } else {
            Debug::trace("CanTranslator.Error", "sendPrinterMessage(): Printer manager is NULL for this position:" + Integer::toString((unsigned int)prType) + " Unable to send msg");
        }
    } else if( prType == SecondPrinter ) {
        if(pPrinter2Manager) {
            pPrinter2Manager->send(msg, msgLength);
        } else {
            Debug::trace("CanTranslator.Error", "sendPrinterMessage(): Printer manager is NULL for this position:" + Integer::toString((unsigned int)prType) + " Unable to send msg");
        }
    } else if( prType == ThirdPrinter ) {
        if(pPrinter3Manager) {
            pPrinter3Manager->send(msg, msgLength);
        } else {
            Debug::trace("CanTranslator.Error", "sendPrinterMessage(): Printer manager is NULL for this position:" + Integer::toString((unsigned int)prType) + " Unable to send msg");
        }
    } else {
        Debug::trace("CanTranslator.Error", "sendPrinterMessage(): printerPosition is unsupported! Cannot send msg for position:"+Integer::toString((unsigned int) prType));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendPrinterMessage(const EvolMessage *msg) {
	if ( !msg )
		return;

	const char *buffer( msg->serialize() );

	sendPrinterMessage( buffer, msg->getSerializedLength(), FirstPrinter );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendWeigherMessage(const char* msg, int msgLength) {

    if( pWeigherManager ) {
        pWeigherManager->send(msg, msgLength);
    } else {
        Debug::trace("CanTranslator.Error", "sendWeigherMessage(): pWeigherManager is NULL! Unable to send msg");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendWeigherMessage(const EvolMessage *msg) {
	if ( !msg ){
        Debug::trace("CanTranslator", "msg was null");
		return;
	}
	const char *buffer( msg->serialize() );
	sendWeigherMessage( buffer, msg->getSerializedLength() );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendWrapperMessage(const char* msg, int msgLength) {

    if( pWrapperManager ) {
        pWrapperManager->send(msg, msgLength);
    } else {
        Debug::trace("CanTranslator.Error", "sendWrapperMessage(): pWrapperManager is NULL! Unable to send msg");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendWrapperMessage(const EvolMessage *msg) {

	if ( !msg )
		return;

	const char *buffer( msg->serialize() );

	sendWrapperMessage( buffer, msg->getSerializedLength() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
char CanTranslator::makeValidChar( char c )
{
	if (( c < ' ' ) || ( '~' < c ))
		return '0';

	return c;
}

void CanTranslator::translateKPCBootEvent(KPCBootEvent KpcBootEvent, const char *recvBuffer)
{

    Debug::trace("CanTranslator", "translateKPCBootEvent() KpcBootEvent:" + Integer::toString(KpcBootEvent));

    can_frame canMsg;
    memset(&canMsg, 0, sizeof(can_frame));

    //translate downstream (out going) requests
    switch (KpcBootEvent)
    {
    case KpcBootReqStatus:
    {
        unsigned int nBytes = 0;
        canMsg.can_id = pCanLibrary->getKPCBootCobID(KpcBootEvent);
        canMsg.can_dlc = 0;

        Debug::trace("CanTranslator.Translate", "translateKpcBootEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KpcBootReqStatus. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }

        break;
    }
    case KpcBootReqVersion:
    {
        unsigned int nBytes = 0;
        canMsg.can_id = pCanLibrary->getKPCBootCobID(KpcBootEvent);
        canMsg.can_dlc = 0;

        Debug::trace("CanTranslator.Translate", "translateKpcBootEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KpcBootReqVersion. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }
    case KpcBootReqAppVersion:
    {
        unsigned int nBytes = 0;
        canMsg.can_id = pCanLibrary->getKPCBootCobID(KpcBootEvent);
        canMsg.can_dlc = 0;

        Debug::trace("CanTranslator.Translate", "translateKpcBootEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KpcBootReqAppVersion. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }
    case KpcBootFlush:
    {
        unsigned int nBytes = 0;
        canMsg.can_id = pCanLibrary->getKPCBootCobID(KpcBootEvent);
        canMsg.can_dlc = 1;
        canMsg.data[0] = 0;

        Debug::trace("CanTranslator.Translate", "translateKpcBootEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));

        if (canMsg.can_id != 0)
        {
            nBytes = pCanManager->send(&canMsg);
            Debug::trace("CanTranslator.FrameTime", "translateKpcBootEvent(): Sent Flush ");
            if (nBytes != sizeof(canMsg))
            {
                Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KpcBootFlush. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
            }
        }
        else
        {
            Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to get COB id for next frame: " + HobString(Integer::toString(canMsg.can_id)));
        }
        break;
    }
    case KpcBootCmd:
    {
        unsigned int nBytes = 0;
        KPCBootCmd cmdMsg(recvBuffer);
        canMsg.can_id = pCanLibrary->getKPCBootCobID(KpcBootEvent);
        canMsg.can_dlc = 7;
        canMsg.data[0] = cmdMsg.getSourceId();
        canMsg.data[1] = cmdMsg.getDestination();
        canMsg.data[2] = cmdMsg.getCommand();
        canMsg.data[3] = (uint8_t)(cmdMsg.getAddress() >> 24);
        canMsg.data[4] = (uint8_t)(cmdMsg.getAddress() >> 16);
        canMsg.data[5] = (uint8_t)(cmdMsg.getAddress() >> 8);
        canMsg.data[6] = (uint8_t)(cmdMsg.getAddress());
        cmdMsg.show();

        Debug::trace("CanTranslator.Translate", "translateKpcBootEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KpcBootCmd. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }
    case KpcBootFrame:
    {
        unsigned int nBytes = 0;
        KPCBootFrameHeader header(recvBuffer);
        canMsg.can_id = pCanLibrary->getKPCBootCobID(KpcBootEvent);
        canMsg.can_dlc = 8;
        canMsg.data[0] = __BlockWriteProgramFlash;
        canMsg.data[1] = KPCBootloader;
        canMsg.data[2] = 0x0B;
        canMsg.data[3] = header.getNumberOfFrames();
        canMsg.data[4] = header.getFinalFrameLength();
        Debug::trace("CanTranslator.Translate", "translateKpcBootEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KpcBootFrame. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }
    case KpcBootFrame1:
    case KpcBootFrame2:
    case KpcBootFrame3:
    case KpcBootFrame4:
    case KpcBootFrame5:
    case KpcBootFrame6:
    case KpcBootFrame7:
    case KpcBootFrame8:
    {
        unsigned int nBytes = 0;
        KPCBootFrame msg(recvBuffer);
        canMsg.can_id = pCanLibrary->getKPCBootCobID(KpcBootEvent);
        canMsg.can_dlc = 8;
        memcpy(&canMsg.data, msg.getFrameBuffer(), 8);
        Debug::trace("CanTranslator.Translate", "translateKpcBootEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KpcBootFrameN. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }

    case KpcBootBulkMsg:
    {
        const KPCBootBulkMsg bulkMsg(recvBuffer);
        unsigned int nBytes = 0;
        // bulkMsg.show();
        /* send the header frame first */
        canMsg.can_id = pCanLibrary->getKPCBootCobID(KpcBootFrame);
        canMsg.can_dlc = 8;
        canMsg.data[0] = bulkMsg.getCommand();
        canMsg.data[1] = bulkMsg.getSourceId();
        canMsg.data[2] = bulkMsg.getDestination();
        canMsg.data[3] = bulkMsg.getNumFrames();
        canMsg.data[4] = bulkMsg.getLength();
        canMsg.data[5] = 0;
        canMsg.data[6] = 0;
        canMsg.data[7] = 0;
        if (canMsg.can_id != 0)
        {
            nBytes = pCanManager->send(&canMsg);
            Debug::trace("CanTranslator.FrameTime", "translateKpcBootEvent(): Sent Header ");
            if (nBytes != sizeof(canMsg))
            {
                Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KpcBootFrame Header. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
            }
        }
        else
        {
            Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to get COB id for KpcBootFrame: " + Integer::toString(canMsg.can_id));
        }

        int k = 0;
        for (int i = 0; i < bulkMsg.getNumFrames(); i++)
        {
            canMsg.can_id = pCanLibrary->getKPCBootCobID(static_cast<KPCBootEvent>(KpcBootFrame + (i + 1)));
            for (int j = 0; j < 8; j++)
            {
                canMsg.data[j] = bulkMsg.getData(k++);
            }

            if (canMsg.can_id != 0)
            {
                nBytes = pCanManager->send(&canMsg);
                Debug::trace("CanTranslator.FrameTime", "translateKpcBootEvent(): Sent Frame ");
                if (nBytes != sizeof(canMsg))
                {
                    Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to send Kpc Boot message KPCBootFrame. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
                }
            }
            else
            {
                Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Failed to get COB id for KpcBootFrame: " + Integer::toString(canMsg.can_id));
            }
        }
        break;
    }
    case KpcBootDone:
    {
        Debug::trace("CanTranslator.info", "translateKPCBootCanMsg(): Deleting Boot Manager Pointer ");
        pKPCBootManager = NULL;
        // SOF-5607 We no longer want the upgrade of a bootloader to be and automatic process.
        // We run the chance of bricking boards from a core upgrade if this is an automatic process.
        //pKPCBootUpgrader = KPCBootUpgrader::getInstance();
        break;
    }

    default:
        Debug::trace("CanTranslator.Error", "translateKpcBootEvent(): Unknown Kpc event: " + Integer::toString(KpcBootEvent));
        break;
    }
}

void CanTranslator::translateKPCBootCanMsg(const CanMsg *pCanMsg)
{

    unsigned short hobEvent = pCanLibrary->getEventForCobId(pCanMsg->id);
    switch (hobEvent)
    {
    case KpcBootReady:
    {
        Debug::trace("CanTranslator.KPC", "Received Boot Ready: kpc = " + ptrtostring(pKPCBootManager) + " weigher = " + ptrtostring(pWeigherManager) + " printer = " + ptrtostring(pPrinter1Manager));

        if( FpmManager::getInstance()->isWrapperTwoPrinter() )
        {
            if (!pKPCBootManager && !pWeigherManager && !pPrinter1Manager && NGWScanFinished_)
            {
                Debug::trace("CanTranslator.KPC", "translateKPCBootCanMsg(): Creating Boot Manager Instance on a 2P System");
                pKPCBootManager = KPCBootManager::getInstance();

                NGWScanFinished_ = false;
            }
            else if( pKPCBootManager )
            {
                Debug::trace("CanTranslator.KPC", "translateKPCBootCanMsg(): KpcBootReady sent to KPC Bootloader");

                KPCBootReady bootReady(this);

                bootReady.setSource(pCanMsg->msgData[0] << 8 | pCanMsg->msgData[1]);
                bootReady.setDestination(pCanMsg->msgData[2] << 8 | pCanMsg->msgData[3]);

                bootReady.show();

                sendKPCBootMessage(&bootReady);
            }
            else
            {
                /*
                Debug::trace("CanTranslator.KPC", "translateKPCBootCanMsg(): KpcBootReady sent dispatched");
                KPCBootReady bootReady(this);

                bootReady.setSource(pCanMsg->msgData[0] << 8 | pCanMsg->msgData[1]);
                bootReady.setDestination(pCanMsg->msgData[2] << 8 | pCanMsg->msgData[3]);
                bootReady.setSourceObject(this);
                bootReady.show();
                dispatchEvent(&bootReady);
                */
            }
        }
        else
        {
            if (!pKPCBootManager && !pWeigherManager && !pPrinter1Manager)
            {
                Debug::trace("CanTranslator.KPC", "translateKPCBootCanMsg(): Creating Boot Manager Instance ");
                pKPCBootManager = KPCBootManager::getInstance();
            }
            else if( pKPCBootManager)
            {
                Debug::trace("CanTranslator.KPC", "translateKPCBootCanMsg(): KpcBootReady sent to KPC Bootloader");

                KPCBootReady bootReady(this);

                bootReady.setSource(pCanMsg->msgData[0] << 8 | pCanMsg->msgData[1]);
                bootReady.setDestination(pCanMsg->msgData[2] << 8 | pCanMsg->msgData[3]);

                bootReady.show();

                sendKPCBootMessage(&bootReady);
            }
            else
            {
                Debug::trace("CanTranslator.KPC", "translateKPCBootCanMsg(): KpcBootReady sent dispatched");
                KPCBootReady bootReady(this);

                bootReady.setSource(pCanMsg->msgData[0] << 8 | pCanMsg->msgData[1]);
                bootReady.setDestination(pCanMsg->msgData[2] << 8 | pCanMsg->msgData[3]);
                bootReady.setSourceObject(this);
                bootReady.show();
                dispatchEvent(&bootReady);
            }
        }
        break;
    }
    case KpcBootStatus:
    {
        KPCBootStatus status(this);
        status.setId(pCanMsg->msgData[0] << 8 | pCanMsg->msgData[1]);
        status.setStatusCode((KPCBootStatus::BootStatusCode)pCanMsg->msgData[2]);
        status.setBootloaderFaults((KPCBootStatus::BootloaderFaults)pCanMsg->msgData[3]);

        status.setFaultInfo(pCanMsg->msgData[4] << 24 | pCanMsg->msgData[5] << 16 | pCanMsg->msgData[6] << 8 | pCanMsg->msgData[7]);

        status.show();

        Debug::trace("CanTranslator.FrameTime", "translateKpcBootEvent(): Got Status");
        sendKPCBootMessage(&status);
        Debug::trace("CanTranslator.FrameTime", "translateKpcBootEvent(): After Got Status");
        break;
    }
    case KpcBootVersion:
    {
        KPCBootVersion ver(this);

        ver.setBootVerMajor(pCanMsg->msgData[0]);
        ver.setBootVerMinor(pCanMsg->msgData[1]);
        ver.setBootVerBuild(pCanMsg->msgData[2]);

        ver.show();
        sendKPCBootMessage(&ver);

        break;
    }
    case KpcBootAppVersion:
    {
        Debug::trace("CanTranslator.AppVer", "Received KpcBootAppVersion");
        KPCBootAppVersion ver(this);

        ver.setPrinterAppVerMajor(pCanMsg->msgData[0]);
        ver.setPrinterAppVerMinor(pCanMsg->msgData[1]);
        ver.setPrinterAppVerBuild(pCanMsg->msgData[2]);

        ver.setWeigherAppVerMajor(pCanMsg->msgData[4]);
        ver.setWeigherAppVerMinor(pCanMsg->msgData[5]);
        ver.setWeigherAppVerBuild(pCanMsg->msgData[6]);

        ver.show();
        sendKPCBootMessage(&ver);

        break;
    }
    case KpcBootAck:
    {
        Debug::trace("CanTranslator.FrameTime", "translateKpcBootEvent(): Ack");
        KPCBootAck ack(this);

        ack.setSource(pCanMsg->msgData[0]);
        ack.setDestination(pCanMsg->msgData[1]);
        uint16_t canId((pCanMsg->msgData[2] << 8) | pCanMsg->msgData[3]);
        Debug::trace("CanTranslator.info", "translateKPCBootCanMsg(): CanId: " + Hex::toString(canId));
        (KPCBootEvent) pCanLibrary->getEventForCobId(canId);
        ack.setAckedEvent((KPCBootEvent)pCanLibrary->getEventForCobId(canId));
        ack.show();

        sendKPCBootMessage(&ack);
        break;
    }
    default:
    {
        Debug::trace("CanTranslator.Error", "translateKPCBootCanMsg(): Unknown Kpc event: " + Integer::toString(hobEvent));
        break;
    }
    }
}

void CanTranslator::sendKPCBootMessage(const EvolMessage *msg)
{
    if (!msg)
        return;

    const char *buffer(msg->serialize());

    sendKPCBootMessage(buffer, msg->getSerializedLength());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendKPCBootMessage(const char *msg, int msgLength)
{

    if (pKPCBootManager)
    {
        pKPCBootManager->send(msg, msgLength);
    }
    else
    {
        Debug::trace("CanTranslator.Error", "sendKPCBootMessage(): sendKPCBootMessage is NULL! Unable to send msg");
    }
}

////////////////////////////////// UPGRADER ////////////////////////////////////////////////
void CanTranslator::sendKPCBootUpgraderMessage(const EvolMessage *msg)
{
    if (!msg)
        return;

    const char *buffer(msg->serialize());

    sendKPCBootUpgraderMessage(buffer, msg->getSerializedLength());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::sendKPCBootUpgraderMessage(const char *msg, int msgLength)
{

    if (pKPCBootUpgrader)
    {
        pKPCBootUpgrader->send(msg, msgLength);
    }
    else
    {
        Debug::trace("CanTranslator.Error", "sendKPCBootUpgraderMessage(): sendKPCBootMessage is NULL! Unable to send msg");
    }
}

void CanTranslator::translateKPCBootUpgraderEvent(KPCBootUpgraderEvent KpcBootEvent, const char *recvBuffer)
{

    Debug::trace("CanTranslator", "translateKPCBootUpgraderEvent() KpcBootEvent:" + Integer::toString(KpcBootEvent));

    can_frame canMsg;
    memset(&canMsg, 0, sizeof(can_frame));

    //translate downstream (out going) requests
    switch (KpcBootEvent)
    {
    case KpcBootUpgraderReqStatus:
    {
        unsigned int nBytes = 0;
        canMsg.can_id = pCanLibrary->getKPCBootUpgraderCobID(KpcBootEvent);
        canMsg.can_dlc = 0;

        Debug::trace("CanTranslator.Translate", "translateKPCBootUpgraderEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to send Kpc Boot message KpcBootReqStatus. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }

        break;
    }
    case KpcBootUpgraderReqVersion:
    {
        unsigned int nBytes = 0;
        canMsg.can_id = pCanLibrary->getKPCBootUpgraderCobID(KpcBootEvent);
        canMsg.can_dlc = 0;

        Debug::trace("CanTranslator.Translate", "translateKPCBootUpgraderEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to send Kpc Boot message KpcBootReqVersion. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }
    case KpcBootUpgraderFlush:
    {
        unsigned int nBytes = 0;
        canMsg.can_id = pCanLibrary->getKPCBootUpgraderCobID(KpcBootEvent);
        canMsg.can_dlc = 1;
        canMsg.data[0] = 0;

        Debug::trace("CanTranslator.Translate", "translateKPCBootUpgraderEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));

        if (canMsg.can_id != 0)
        {
            nBytes = pCanManager->send(&canMsg);
            Debug::trace("CanTranslator.FrameTime", "translateKPCBootUpgraderEvent(): Sent Flush ");
            if (nBytes != sizeof(canMsg))
            {
                Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to send Kpc Boot message KpcBootFlush. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
            }
        }
        else
        {
            Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to get COB id for next frame: " + HobString(Integer::toString(canMsg.can_id)));
        }
        break;
    }
    case KpcBootUpgraderCmd:
    {
        unsigned int nBytes = 0;
        KPCBootUpgraderCmd cmdMsg(recvBuffer);
        canMsg.can_id = pCanLibrary->getKPCBootUpgraderCobID(KpcBootEvent);
        canMsg.can_dlc = 3;
        canMsg.data[0] = cmdMsg.getSourceId();
        canMsg.data[1] = cmdMsg.getDestination();
        canMsg.data[2] = cmdMsg.getCommand();


        Debug::trace("CanTranslator.Translate", "translateKPCBootUpgraderEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to send Kpc Boot message KpcBootCmd. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }
    case KpcBootUpgraderFrame:
    {
        unsigned int nBytes = 0;
        KPCBootUpgraderFrameHeader header(recvBuffer);
        canMsg.can_id = pCanLibrary->getKPCBootUpgraderCobID(KpcBootEvent);
        canMsg.can_dlc = 8;
        canMsg.data[0] = __BlockWriteProgramFlash;
        canMsg.data[1] = KPCBootloader;
        canMsg.data[2] = 0x0B;
        canMsg.data[3] = header.getNumberOfFrames();
        canMsg.data[4] = header.getFinalFrameLength();
        Debug::trace("CanTranslator.Translate", "translateKPCBootUpgraderEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to send Kpc Boot message KpcBootFrame. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }
    case KpcBootUpgraderFrame1:
    case KpcBootUpgraderFrame2:
    case KpcBootUpgraderFrame3:
    case KpcBootUpgraderFrame4:
    case KpcBootUpgraderFrame5:
    case KpcBootUpgraderFrame6:
    case KpcBootUpgraderFrame7:
    case KpcBootUpgraderFrame8:
    {
        unsigned int nBytes = 0;
        KPCBootUpgraderFrame msg(recvBuffer);
        canMsg.can_id = pCanLibrary->getKPCBootUpgraderCobID(KpcBootEvent);
        canMsg.can_dlc = 8;
        memcpy(&canMsg.data, msg.getFrameBuffer(), 8);
        Debug::trace("CanTranslator.Translate", "translateKPCBootUpgraderEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
        nBytes = pCanManager->send(&canMsg);
        if (nBytes != sizeof(canMsg))
        {
            Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to send Kpc Boot message KpcBootFrameN. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
        }
        break;
    }

    case KpcBootUpgraderBulkMsg:
    {
        const KPCBootUpgraderBulkMsg bulkMsg(recvBuffer);
        unsigned int nBytes = 0;
        // bulkMsg.show();
        /* send the header frame first */
        canMsg.can_id = pCanLibrary->getKPCBootUpgraderCobID(KpcBootUpgraderFrame);
        canMsg.can_dlc = 8;
        canMsg.data[0] = bulkMsg.getCommand();
        canMsg.data[1] = bulkMsg.getSourceId();
        canMsg.data[2] = bulkMsg.getDestination();
        canMsg.data[3] = bulkMsg.getNumFrames();
        canMsg.data[4] = bulkMsg.getLength();
        canMsg.data[5] = 0;
        canMsg.data[6] = 0;
        canMsg.data[7] = 0;
        if (canMsg.can_id != 0)
        {
            nBytes = pCanManager->send(&canMsg);
            Debug::trace("CanTranslator.FrameTime", "translateKPCBootUpgraderEvent(): Sent Header ");
            if (nBytes != sizeof(canMsg))
            {
                Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to send Kpc Boot message KpcBootFrame Header. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
            }
        }
        else
        {
            Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to get COB id for KpcBootFrame: " + Integer::toString(canMsg.can_id));
        }

        int k = 0;
        for (int i = 0; i < bulkMsg.getNumFrames(); i++)
        {
            canMsg.can_id = pCanLibrary->getKPCBootUpgraderCobID(static_cast<KPCBootUpgraderEvent>(KpcBootUpgraderFrame + (i + 1)));
            for (int j = 0; j < 8; j++)
            {
                canMsg.data[j] = bulkMsg.getData(k++);
            }

            if (canMsg.can_id != 0)
            {
                nBytes = pCanManager->send(&canMsg);
                Debug::trace("CanTranslator.FrameTime", "translateKPCBootUpgraderEvent(): Sent Frame ");
                if (nBytes != sizeof(canMsg))
                {
                    Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to send Kpc Boot message KPCBootFrame. Sent: " + Integer::toString(nBytes) + "bytes of: " + Integer::toString(sizeof(canMsg)));
                }
            }
            else
            {
                Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Failed to get COB id for KpcBootFrame: " + Integer::toString(canMsg.can_id));
            }
        }
        break;
    }
    case KpcBootUpgraderDone:
    {
        Debug::trace("CanTranslator.info", "translateKPCBootUpgraderEvent(): Deleting Boot Manager Pointer ");
        pKPCBootUpgrader = NULL;
        break;
    }

    default:
        Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderEvent(): Unknown Kpc event: " + Integer::toString(KpcBootEvent));
        break;
    }
}

void CanTranslator::translateKPCBootUpgraderCanMsg(const CanMsg *pCanMsg)
{

    unsigned short hobEvent = pCanLibrary->getEventForCobId(pCanMsg->id);
    switch (hobEvent)
    {
    case KpcBootUpgraderStatus:
    {
        KPCBootUpgraderStatus status(this);
        status.setId(pCanMsg->msgData[0] << 8 | pCanMsg->msgData[1]);
        status.setStatusCode((KPCBootUpgraderStatus::BootUpgraderStatusCode)pCanMsg->msgData[2]);
        status.setBootUpgraderFaults((KPCBootUpgraderStatus::BootUpgraderFaults)pCanMsg->msgData[3]);

        status.setFaultInfo(pCanMsg->msgData[4] << 24 | pCanMsg->msgData[5] << 16 | pCanMsg->msgData[6] << 8 | pCanMsg->msgData[7]);

        status.show();

        Debug::trace("CanTranslator.FrameTime", "translateKPCBootUpgraderCanMsg(): Got Status");
        sendKPCBootUpgraderMessage(&status);
        Debug::trace("CanTranslator.FrameTime", "translateKPCBootUpgraderCanMsg(): After Got Status");
        break;
    }
    case KpcBootUpgraderVersion:
    {
        KPCBootVersion ver(KPCBootUpdater, this);

        ver.setBootVerMajor(pCanMsg->msgData[0]);
        ver.setBootVerMinor(pCanMsg->msgData[1]);
        ver.setBootVerBuild(pCanMsg->msgData[2]);

        ver.show();
        sendKPCBootUpgraderMessage(&ver);

        break;
    }
    case KpcBootUpgraderAppVersion:
    {
        KPCBootAppVersion ver(KPCBootUpdater, this);

        ver.setPrinterAppVerMajor(pCanMsg->msgData[0]);
        ver.setPrinterAppVerMinor(pCanMsg->msgData[1]);
        ver.setPrinterAppVerBuild(pCanMsg->msgData[2]);

        ver.setWeigherAppVerMajor(pCanMsg->msgData[4]);
        ver.setWeigherAppVerMinor(pCanMsg->msgData[5]);
        ver.setWeigherAppVerBuild(pCanMsg->msgData[6]);

        ver.show();
        sendKPCBootUpgraderMessage(&ver);

        break;
    }
    case KpcBootUpgraderAck:
    {
        Debug::trace("CanTranslator.FrameTime", "translateKPCBootUpgraderCanMsg(): Ack");
        KPCBootUpgraderAck ack(this);

        ack.setSource(pCanMsg->msgData[0]);
        ack.setDestination(pCanMsg->msgData[1]);
        uint16_t canId((pCanMsg->msgData[2] << 8) | pCanMsg->msgData[3]);
        Debug::trace("CanTranslator.info", "translateKPCBootUpgraderCanMsg(): CanId: " + Hex::toString(canId));
        (KPCBootEvent) pCanLibrary->getEventForCobId(canId);
        ack.setAckedEvent((KPCBootUpgraderEvent)pCanLibrary->getEventForCobId(canId));
        ack.show();

        sendKPCBootUpgraderMessage(&ack);
        break;
    }
    default:
    {
        Debug::trace("CanTranslator.Error", "translateKPCBootUpgraderCanMsg(): Unknown Kpc event: " + Integer::toString(hobEvent));
        break;
    }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateBoardSiteEvent(BoardSiteEvent event, const char *recvBuffer)
{
    Debug::trace("CanTranslator", "translateBoardSiteEvent() BoardSiteEvent:" + Integer::toString(event));

    can_frame canMsg;
    memset(&canMsg, 0, sizeof(can_frame));

    //translate downstream (out going) requests
    switch (event)
    {
        case BrdSiteTestStatus:
        {
            
            BoardSiteTestStatus msg(recvBuffer);

            canMsg.can_id = pCanLibrary->getBoardSiteCobID(event);
            canMsg.can_dlc = 1;
            canMsg.data[0] = msg.isReady();
            
            unsigned int nBytes = 0;
            Debug::trace("CanTranslator.Translate", "translateBoardSiteEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
            nBytes = pCanManager->send(&canMsg);
            if (nBytes != sizeof(canMsg))
            {
                Debug::trace("CanTranslator.Error", "translateBoardSiteEvent(): Failed to send Kpc Boot message KpcBootReqStatus. Sent: " + Integer::toString(0) + "bytes of: " + Integer::toString(sizeof(canMsg)));
            }

            break;
        }
        case BrdSiteBoardTestComplete:
        {
            
            BoardSiteTestComplete msg(recvBuffer);
            
            canMsg.can_id = pCanLibrary->getBoardSiteCobID(event);
            canMsg.can_dlc = 6;
            canMsg.data[0] = msg.getTestResult();
            canMsg.data[1] = (unsigned char)msg.getErrorCode();
            canMsg.data[2] = getMsbFromShort(msg.getExpected()); 
            canMsg.data[3] = getLsbFromShort(msg.getExpected());
            canMsg.data[4] = getMsbFromShort(msg.getResult()); 
            canMsg.data[5] = getLsbFromShort(msg.getResult());

            unsigned int nBytes = 0;
            Debug::trace("CanTranslator.Translate", "translateBoardSiteEvent(): Sending CAN frame: " + Hex::toString(canMsg.can_id));
            nBytes = pCanManager->send(&canMsg);
            if (nBytes != sizeof(canMsg))
            {
                Debug::trace("CanTranslator.Error", "translateBoardSiteEvent(): Failed to send Kpc Boot message KpcBootReqVersion. Sent: " + Integer::toString(0) + "bytes of: " + Integer::toString(sizeof(canMsg)));
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanTranslator::translateBoardSiteCanMsg(const CanMsg *pCanMsg)
{
	unsigned short hobEvent = pCanLibrary->getEventForCobId( pCanMsg->id );
    Debug::trace("CanTranslator.Translate", "translateBoardSiteCanMsg(): Board Site Message: 0X" + Hex::toString(  pCanMsg->id  ) + " " + Hex::toString( hobEvent ) );
    switch(hobEvent) {        
        case BrdSiteReqTestMode: {
            if(pCanMsg->msgData[0]) {
                //start of board site testing. remove all peripheral CAN managers.
                RealTimeController::getInstance()->CANDeviceDetached( RealTimeController::_FirstCANPrinter );
                RealTimeController::getInstance()->CANDeviceDetached( RealTimeController::_CANWeigher );


                registerEvent( BoardSite, BrdSiteTestStatus );
                registerEvent( BoardSite, BrdSiteBoardTestComplete );
                //support for phase 1 of ARK board site testing. no need to remove this since it's only created 
                //when testing the board at manufacture.                               
                pSiteManager = BoardSiteTestManager::getInstance( BoardSiteTestManager::_PHASE1 );
                if( pSiteManager == NULL ) {
                    Debug::trace("CanTranslator.Translate", "translateBoardSiteCanMsg() Failed to create BoardSiteTestManager!");   
                } else {
                    Debug::trace("CanTranslator.Translate", "BoardSiteTestManager should be running!");   
                }
            } 
            break;
        }
        case BrdSiteBoardTest: {
                BoardSiteTest msg(this, (BoardSiteTests)pCanMsg->msgData[0], pCanMsg->msgData[1], pCanMsg->msgData[2],
                                  pCanMsg->msgData[3], pCanMsg->msgData[4], pCanMsg->msgData[5], 
                                  pCanMsg->msgData[6], pCanMsg->msgData[7]);
                if( pSiteManager != NULL ) {                                  
                    pSiteManager->send(&msg);
                }
            break;
        }
        default: {
           Debug::trace("CanTranslator.Error", "translateBoardSiteCanMsg(): Unknown Event:"  + Integer::toString(hobEvent) + " for id:0X"  +  Hex::toString( pCanMsg->id ) );
        }
    }
}
#endif // __linux__
