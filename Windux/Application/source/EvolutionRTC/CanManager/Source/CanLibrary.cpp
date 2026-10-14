/**
 * @file CanLibrary.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Keeps maps and lists of the CAN peripheral messaging
 * Created on: Jun 29, 2015
 */
#include <algorithm>
#include "CanLibrary.h"
#include "FpmManager.h"
#include "HobString.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#if defined ( __linux__ )
// do not add these includes
#else
#include "KpcBootEvent.h"
#include "KpcBootUpgraderEvent.h"
#endif

//Printer message includes
#include "PrinterLabelTransfer.h"
#include "PrinterInquire.h"
#include "PrinterError.h"
#include "PrinterVersion.h"
#include "PrinterStatus.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//  Note: NGW1 = hybrid CAN / USB wrapper messaging BCM board
//	NGW2 = hybrid CAN / USB wrapper messaging ARA board
//	NGW3 = full CAN wrapper messaging ARA / ARK
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
CanLibrary::CanLibrary() {
    Debug::trace( "CanLibrary", "CanLibrary(): build device libraries.");

#if defined ( __linux__ )           // added #defines to indiv sections in case we need to bring something back later
    //get my type of system
    FpmManager *pSerialFlash = FpmManager::getInstance();
    mySystemType = pSerialFlash->getMFGSystemStyle();

    setupFrameFilter();

    buildWeigherLibrary();
    buildPrinterLibrary();
    buildWrapperLibrary();
    buildKPCBootLibrary();
	buildKPCBootUpgraderLibrary();
	buildBoardSiteLibrary();
	

#endif // __linux__
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CanLibrary::~CanLibrary() {
}

#if defined ( __linux__ )			// added #defines to indiv sections in case we need to bring something back later

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::buildWeigherLibrary() {
    Debug::trace( "CanLibrary", "buildWeigherLibrary()");

    //build map of possible out going weigher can messages.
    addWeigherMessage( 0x452, WgReqStatus, true, 0 );
    addWeigherMessage( 0x408, WgReqInquire, true, 0 );
    addWeigherMessage( 0x401, WgReqWakeup, true, 0 );
    addWeigherMessage( 0x42d, WgReqSystemInfo, true, 0 );
    addWeigherMessage( 0x451, WgMode, true, 0 );
    addWeigherMessage( 0x454, WgEnable, true, 0 );
    addWeigherMessage( 0x455, WgDisable, true, 0 );
    addWeigherMessage( 0x456, WgRezero, true, 0 );
    addWeigherMessage( 0x457, WgReset, true, 0 );
    addWeigherMessage( 0x458, WgColdReset, true, 0 );
    addWeigherMessage( 0x459, WgControl, true, 0 );
    addWeigherMessage( 0x41e, WgResetFactoryDefaults, true, 0 );
    //Cat3
    addWeigherMessage( 0x45c, WgCat3ReqStatistics, true, 0 );
    addWeigherMessage( 0x45d, WgCat3DateTime, true, 0 );
    addWeigherMessage( 0x45e, WgCat3ReqPage, true, 0 );
    addWeigherMessage( 0x45f, WgCat3ReqNextRecords, true, 0 );
    addWeigherMessage( 0x460, WgCat3ReqEraseLog, true, 0 );
    addWeigherMessage( 0x480, WgCat3WriteRecord, true, 0 );
    addWeigherMessage( 0x470, WgConfig, true, 0 );
    addWeigherMessage( 0x482, WgReqVersion, true, 0 );
    //Value Max
    addWeigherMessage( 0x420, WgReqValueMax, true, 0 );
    addWeigherMessage( 0x423, WgValueMaxWriteAssembly, true, 0 );
    addWeigherMessage( 0x426, WgValueMaxWriteHwVersion, true, 0 );
    addWeigherMessage( 0x429, WgValueMaxWriteSerialNumber, true, 0 );
    addWeigherMessage( 0x42c, WgValueMaxWriteDateOfManufacture, true, 0 );

    //build list of possible in coming weigher can messages
    addWeigherMessage( 0x400, WgWakeup, false, 4 );
    addWeigherMessage( 0x402, WgStatus, false, 30 );
    addWeigherMessage( 0x406, WgSystemInfo, false, 8 );
    addWeigherMessage( 0x407, WgKPCError, false, 8 );
#if 0 	 	// implement Carlos's Weigher Configuration changes
    addWeigherMessage( 0x411, WgInquire, false, 80 );
#else    
	addWeigherMessage( 0x411, WgInquire, false, 54 );
#endif
    addWeigherMessage( 0x440, WgCat3AuditStatistics, false, 33 );
    addWeigherMessage( 0x490, WgCat3Records, false, 322 );
    addWeigherMessage( 0x44f, WgCat3RecordStatus, false, 1 );
    addWeigherMessage( 0x450, WgCat3LogEraseComplete, false, 1 );
    addWeigherMessage( 0x41f, WgResetFactoryDefaultsComplete, false, 1 );

    addWeigherMessage( 0x421, WgValueMaxAssembly, false, 10 );
    addWeigherMessage( 0x425, WgValueMaxHwVersion, false, 2 );
    addWeigherMessage( 0x427, WgValueMaxSerialNumber, false, 16 );
    addWeigherMessage( 0x42b, WgValueMaxDateOfManufacture, false, 4 );
	addWeigherMessage( 0x430, WgValueMaxFitParams, false, 100 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::addWeigherMessage(unsigned int cobId, WgEvent wgEvent, bool outgoing, unsigned int canMsgSize) {

    if(outgoing) {
        Debug::trace("CanLibrary.addMessage", "addWeigherMessage(): Adding incoming weigher event:" + Integer::toString(wgEvent));
        std::vector<canid_t> wgCob;
        wgCob.push_back(cobId);
        wgCanMap.insert( std::make_pair(wgEvent, wgCob) );
    } else {
        Debug::trace("CanLibrary.addMessage", "addWeigherMessage(): Adding outgoing weigher event:"+Integer::toString(wgEvent));
        CanLibMessage wgCan;
        wgCan.eventType = wgEvent;
        wgCan.cobId = cobId;
        wgCan.msgSize = canMsgSize;
        wgList.push_back(wgCan);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canid_t CanLibrary::getWeigherCobID(WgEvent event) {

    Debug::trace( "CanLibrary", "getWeigherCobID() event: " + Integer::toString(event));

    WgCanMap::iterator pIter = wgCanMap.find(event);
    if( pIter != wgCanMap.end() ) {
        std::vector<canid_t> &ids = pIter->second;
        return ids.front();
    } else {
        Debug::trace( "CanLibrary.Error", "Failed to get cobid for Weigher event: " + Integer::toString(event));
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperRequest CanLibrary::getWrapperRequest( WrEvent wrEvent )
{
	WrapperRequest request = REQ_NONE;

	switch( wrEvent )
	{
		case WrReqWakeup: {
			request = REQ_WAKEUP;
			break;
		}
		case WrReqInquire: {
			request = REQ_CONFIG;
			break;
		}
		case WrReqStatus: {
			request = REQ_STATUS;
			break;
		}
		case WrReqSensors: {
			request = REQ_SENSORS;
			break;
		}
		case WrReqEvolSystemInfo: {
			request = REQ_SYSTEM_INFO;
			break;
		}
		case WrEvolReqCycleCount: {
			request = REQ_CYCLE_COUNT;
			break;
		}
		case WrEvolReqSystemReset: {
			request = REQ_SYSTEM_RESET;
			break;
		}
		case WrEvolRequestCameraThreshold: {
			request = REQ_CAMERA_THRESHOLD;
			break;
		}
		case WrNgwReqVersion: {
			request = REQ_VERSION;
			break;
		}
		case WrEvolStartMfgRunin: {
			request = REQ_MFG_RUNIN;
			break;
		}
		/*
		case WrEvolRequestPackageTimings: {
			request = REQ_WB_CURRENT_TIMINGS;
			break;
		} */
		case WrEvolResetDefaultPackageTimings:{
			request = REQ_WB_WRITE_DEFAULT_TIMINGS;
			break;
		}
		case WrEvolSavePackageTimings: {
			request = REQ_WB_WRITE_TIMINGS;
			break;
		}

	}
	return request;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanLibrary::acceptedCobId(unsigned int cob) {
    if( (( cob >= minPrinter1CobID ) && (cob <= maxPrinter1CobID )) ||
        (( cob >= minPrinter2CobID ) && (cob <= maxPrinter2CobID )) ||
        (( cob >= minPrinter3CobID ) && (cob <= maxPrinter3CobID )) ) {
        return acceptedPrinterCobId(cob);
    } else if( ( cob >= minWeigherCobID ) && (cob <= maxWeigherCobID ) ) {
        return acceptedWeigherCobId(cob);
    } else if( ( cob >= minWrapperCobID ) && (cob <= maxWrapperCobID ) ) {
        return acceptedWrapperCobId(cob);
    } else {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanLibrary::acceptedPrinterCobId(unsigned int cob) {

    PrCanMap::iterator pIter;
    PrCanMap::iterator pIterEnd;
    if( ( cob >= minPrinter1CobID ) && (cob <= maxPrinter1CobID ) ) {
        pIter = prCanMap.begin();
        pIterEnd = prCanMap.end();
    } else if( ( cob >= minPrinter2CobID ) && (cob <= maxPrinter2CobID )) {
        pIter = pr2CanMap.begin();
        pIterEnd = pr2CanMap.end();
    } else if( ( cob >= minPrinter3CobID ) && (cob <= maxPrinter3CobID )) {
        pIter = pr3CanMap.begin();
        pIterEnd = pr3CanMap.end();
    } else {//Cob isn't within the printer's cob range!
        Debug::trace("CanLibrary.Error", "acceptedPrinterCobId(): CobId: 0x" + Hex::toString(cob) + " is not within the printer cob range! Returning false");
        return false;
    }
    for(;pIter != prCanMap.end();pIter++) {
        if( std::find( (pIter->second).begin(), (pIter->second).end(), cob) != (pIter->second).end() ) {//Check if the cob exists in the vector
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanLibrary::acceptedWeigherCobId(unsigned int cob) {
    WgCanMap::iterator pIter = wgCanMap.begin();
    for(;pIter != wgCanMap.end(); pIter++) {
        if(std::find( (pIter->second).begin(), (pIter->second).end(), cob) != (pIter->second).end()) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanLibrary::acceptedWrapperCobId(unsigned int cob) {
    WrCanMap::iterator pIter = wrCanMap.begin();
    for(;pIter != wrCanMap.end(); pIter++) {
        if(std::find( (pIter->second).begin(), (pIter->second).end(), cob) != (pIter->second).end()) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::setupFrameFilter()
{
	FpmManager* pSerialFlash = FpmManager::getInstance();
	// setup filter for incoming supported CAN cob id's
	filter.clear();

	// AWS3 and NGW1 only support CAN upgrade messages
	if( ( mySystemType == "AWS3" ) || (mySystemType == "NGW1") ){
		filter.push_back( 0x54b );							//WrNGWBootStatus
		filter.push_back( 0x54c ); 							//WrNGWBootVersion
		filter.push_back( 0x54d );							//WrNGWAck

		filter.push_back( 0x59d );							//WrOpen

		filter.push_back( 0x596 );							//WrStatus
		filter.push_back( 0x597 );
		filter.push_back( 0x598 );
		filter.push_back( 0x599 );
		filter.push_back( 0x59A );
		filter.push_back( 0x59B );
		filter.push_back( 0x59C );

		filter.push_back( 0x534 );							//WrPackageDescriptor
		filter.push_back( 0x535 );
		filter.push_back( 0x536 );

		filter.push_back( 0x710 );							//Node 1 WrEngInfo
		filter.push_back( 0x711 );
		filter.push_back( 0x712 );							//Node 2 WrEngInfo
		filter.push_back( 0x713 );
		filter.push_back( 0x714 );							//Node 3 WrEngInfo
		filter.push_back( 0x715 );
		filter.push_back( 0x716 );							//Node 4 WrEngInfo
		filter.push_back( 0x717 );
		filter.push_back( 0x718 );							//Node 5 WrEngInfo
		filter.push_back( 0x719 );		
		filter.push_back( 0x71a );							//Node 6 WrEngInfo		
		filter.push_back( 0x71b );		

	} if( ( mySystemType == "NGS1" ) || (mySystemType == "NGW2") ) {
		filter.push_back( 0x54b );							//WrNGWBootStatus
		filter.push_back( 0x54c ); 							//WrNGWBootVersion
		filter.push_back( 0x54d );							//WrNGWAck

		filter.push_back( 0x59d );							//WrOpen

		filter.push_back( 0x596 );							//WrStatus
		filter.push_back( 0x597 );
		filter.push_back( 0x598 );
		filter.push_back( 0x599 );
		filter.push_back( 0x59A );
		filter.push_back( 0x59B );
		filter.push_back( 0x59C );

		filter.push_back( 0x534 );							//WrPackageDescriptor
		filter.push_back( 0x535 );
		filter.push_back( 0x536 );

		filter.push_back( 0x5B0 );							//WrSystemInfo
		filter.push_back( 0x5B1 );
		filter.push_back( 0x5B2 );
		filter.push_back( 0x5B3 );
		filter.push_back( 0x5B4 );
		filter.push_back( 0x5B5 );
		filter.push_back( 0x5B6 );

		filter.push_back( 0x588 );							//WrConfig
		filter.push_back( 0x589 );
		filter.push_back( 0x58a );
		filter.push_back( 0x58b );
		filter.push_back( 0x58c );
		filter.push_back( 0x58d );

		filter.push_back( 0x5d0 );							//WrVersion
		filter.push_back( 0x5d1 );
		filter.push_back( 0x5d2 );
		filter.push_back( 0x5d3 );
		filter.push_back( 0x5d4 );
		filter.push_back( 0x5d5 );
		filter.push_back( 0x5d6 );
		filter.push_back( 0x5d7 );
		filter.push_back( 0x5d8 );
		filter.push_back( 0x5d9 );
		filter.push_back( 0x5da );
		filter.push_back( 0x5db );

		filter.push_back( 0x5a7 );							//WrSensors
		filter.push_back( 0x586 );							//WrWakeup
		filter.push_back( 0x5a5 );							//WrTestComplete
		filter.push_back( 0x594 );							//WrEventCapture

		filter.push_back( 0x5bf );							//WrCameraScan
		filter.push_back( 0x5c0 );
		filter.push_back( 0x5c1 );
		filter.push_back( 0x5c2 );
		filter.push_back( 0x5c3 );
		filter.push_back( 0x5c4 );
		filter.push_back( 0x5c5 );
		filter.push_back( 0x5c6 );
		filter.push_back( 0x5c7 );

		filter.push_back( 0x5ba );							//WrCameraHeight
		filter.push_back( 0x5bb );							//WrCameraThreshold
		filter.push_back( 0x5b9 );							//WrCameraCalibration

		filter.push_back( 0x5ca );							//WrErrHistory
		filter.push_back( 0x5cb );
		filter.push_back( 0x5cc );

		filter.push_back( 0x5af );							//WrTransferDischarge
		filter.push_back( 0x5b8 );							//WrEvolMotorProfile
		filter.push_back( 0x5c8 );							//WrEvolManufacturingRunInNotice

		filter.push_back( 0x5bd );							//WrEvolDiagnosticsTestResult
		filter.push_back( 0x5be );

		filter.push_back( 0x5e0 );							//WrWrapBoxSize

		filter.push_back( 0x5e1 );							//WrWrapBoxStart
		filter.push_back( 0x5e2 );
		filter.push_back( 0x5e3 );
		filter.push_back( 0x5e4 );

		filter.push_back( 0x5e7 );							//WrWrapBox
		filter.push_back( 0x5e8 );
		filter.push_back( 0x5e9 );

		filter.push_back( 0x5ea );							//WrHeightBox
		filter.push_back( 0x5eb );
		if (pSerialFlash->isWrapperTwoPrinter())
			filter.push_back(0x5ec);						// added for vertical offsets - otherwise dont add it

		filter.push_back( 0x5f0 );							//WrWrapBoxAck
		filter.push_back( 0x5ee );							//WrWrapBoxTransferComplete
		
		filter.push_back( 0x610 );							//WrPPPWakeup
		//filter.push_back( 0x611 );						// repurposed for 

		filter.push_back( 0x61b );							//WrPPPConfig
		filter.push_back( 0x61c );
		filter.push_back( 0x61d );
		filter.push_back( 0x61e );

		filter.push_back( 0x615 );							//WrPPPDist

		filter.push_back( 0x616 );							//WrPPPCal
		filter.push_back( 0x617 );

		filter.push_back( 0x619 );							//WrPPPVersion
		filter.push_back( 0x61a );							//WrPPPStatus

		filter.push_back( 0x620 );							//WrWrapperPause	
		filter.push_back( 0x621 );

		filter.push_back( 0x710 );							//Node 1 WrEngInfo
		filter.push_back( 0x711 );
		filter.push_back( 0x712 );							//Node 2 WrEngInfo
		filter.push_back( 0x713 );
		filter.push_back( 0x714 );							//Node 3 WrEngInfo
		filter.push_back( 0x715 );
		filter.push_back( 0x716 );							//Node 4 WrEngInfo
		filter.push_back( 0x717 );
		filter.push_back( 0x718 );							//Node 5 WrEngInfo
		filter.push_back( 0x719 );		
		filter.push_back( 0x71a );							//Node 6 WrEngInfo		
		filter.push_back( 0x71b );		
				
		// KPC for TOF upgrades
		filter.push_back( 0x099 );							//KpcError
		//KPC Bootloader
		filter.push_back( 0x070 );
		filter.push_back( 0x071 );
		filter.push_back( 0x072 );
		filter.push_back( 0x073 );
		filter.push_back( 0x074 );

		//KPC BootUpgrader
		filter.push_back( 0x0D0 );
		filter.push_back( 0x0D1 );
		filter.push_back( 0x0D2 );
		filter.push_back( 0x0D3 );
		filter.push_back( 0x0D4 );
				
	} if( ( mySystemType == "HTK" ) || ( mySystemType == "FSS" ) || ( mySystemType == "FSS" ) /*|| mySystemType == "NGW2"*/ ) {
		filter.push_back( 0x099 );							//KpcError

		filter.push_back( 0x400 );							//WgWakeup

		filter.push_back( 0x402 );							//WgStatus
		filter.push_back( 0x403 );
		filter.push_back( 0x404 );
		filter.push_back( 0x405 );
		filter.push_back( 0x406 );							//WgSystemInfo
		filter.push_back( 0x407 );							//WgKPCError

		filter.push_back( 0x411 );							//WgConfig (Inquire)
		filter.push_back( 0x412 );
		filter.push_back( 0x413 );
		filter.push_back( 0x414 );
		filter.push_back( 0x415 );
		filter.push_back( 0x416 );
		filter.push_back( 0x417 );
#if 0 	// implement Carlos's Weigher Configuration changes
		filter.push_back( 0x418 );
		filter.push_back( 0x419 );
		filter.push_back( 0x41a );
		filter.push_back( 0x41b );
#endif
		filter.push_back( 0x405 );							//WgSerialNumber
		filter.push_back( 0x407 );							//WgVersion

		filter.push_back( 0x41f );							//WgResetFactoryDefaultsComplete

		filter.push_back( 0x421 );							//WgValueMaxAssembly
		filter.push_back( 0x422 );

		filter.push_back( 0x425 );							//WgValueMaxHardwareVersion
		filter.push_back( 0x427 );							//WgValueMaxSerialNumber
		filter.push_back( 0x428 );

		filter.push_back( 0x42b );							//WgValueMaxDate

		filter.push_back( 0x430 );							//WgValueMaxFitParams
		filter.push_back( 0x431 );
		filter.push_back( 0x432 );
		filter.push_back( 0x433 );
		filter.push_back( 0x434 );
		filter.push_back( 0x435 );
		filter.push_back( 0x436 );
		filter.push_back( 0x437 );
		filter.push_back( 0x438 );
		filter.push_back( 0x439 );
		filter.push_back( 0x43a );
		filter.push_back( 0x43b );
		filter.push_back( 0x43c );



		filter.push_back( 0x440 );							//WgCat3AuditStatistics
		filter.push_back( 0x441 );
		filter.push_back( 0x442 );
		filter.push_back( 0x443 );
		filter.push_back( 0x444 );

		for( int i = 0; i < 41; i++ ) {						//WgCat3Records
			filter.push_back( 0x490 + i );
		}

		filter.push_back( 0x44f );							//WgCat3RecordStatus
		filter.push_back( 0x450 );							//WgCat3LogEraseComplete

		 //add Printer id's
		 filter.push_back( 0x100 );							//PrWakeup
		 filter.push_back( 0x101 );

		 filter.push_back( 0x102 );							//PrKPCError
		 filter.push_back( 0x103 );							//PrFlushCmplt
		 filter.push_back( 0x119 );							//PrConfig
		 filter.push_back( 0x11A );
		 filter.push_back( 0x11B );
		 filter.push_back( 0x11C );
		 filter.push_back( 0x11D );
		 filter.push_back( 0x11E );

		 filter.push_back( 0x107 );							//PrStatus
		 filter.push_back( 0x108 );

		 filter.push_back( 0x109 );							//PrSensors
		 filter.push_back( 0x10a );
		 filter.push_back( 0x10b );
		 filter.push_back( 0x10c );

		 filter.push_back( 0x10e );							//PrVersion
		 filter.push_back( 0x10f );

		 filter.push_back( 0x112 );							//PrHeadType
		 filter.push_back( 0x115 );							//PrCutterStatus
		 filter.push_back( 0x116 );							//PrLabelTransferReady
		 filter.push_back( 0x117 );							//PrResetFactoryDefaultsComplete
		 filter.push_back( 0x118 );							//PrRS485FlowControl

		 for( int i = 0; i < 29; i++ ) {					//PrHeadDotWear
			filter.push_back( 0x175 + i );
		 }

		 filter.push_back( 0x194 );							//PrHeadDotStatus
		 filter.push_back( 0x195 );							//PrHeadDotStatus
		 
		 filter.push_back( 0x1A0 );							//PrGapCalStatus
		 
		 //KPC Bootloader
		 filter.push_back( 0x070 );
		 filter.push_back( 0x071 );
		 filter.push_back( 0x072 );
		 filter.push_back( 0x073 );
		 filter.push_back( 0x074 );

		 //KPC BootUpgrader
		 filter.push_back( 0x0D0 );
		 filter.push_back( 0x0D1 );
		 filter.push_back( 0x0D2 );
		 filter.push_back( 0x0D3 );
		 filter.push_back( 0x0D4 );

		 //Board Site
		 filter.push_back( 0x0F1 );
		 filter.push_back( 0x0F2 );
		 filter.push_back( 0x0F3 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanLibrary::frameFilter(canid_t cobID){

	std::vector<canid_t>::iterator iter;

	iter = std::find(filter.begin(), filter.end(), cobID);
	if (iter != filter.end()){
		return true;
	}
	else{
		return false;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::buildPrinterLibrary() {

	Debug::trace( "CanLibrary", "buildPrinterLibrary()");
	//build list of possible out going printer can messages.
	addPrinterMessage( 0x120, PrReqWakeup, true, FirstPrinter, 0 );
	addPrinterMessage( 0x121, PrReqInquire, true, FirstPrinter, 0 );
	addPrinterMessage( 0x122, PrReqStatus, true, FirstPrinter, 0 );
	addPrinterMessage( 0x123, PrReqSensors, true, FirstPrinter, 0 );
	addPrinterMessage( 0x124, PrReqCalibrate, true, FirstPrinter, 0 );
	addPrinterMessage( 0x125, PrReqLabelTransfer, true, FirstPrinter, 0 );
	addPrinterMessage( 0x126, PrReqPrintheadPower, true, FirstPrinter, 0 );
	addPrinterMessage( 0x127, PrReqVersion, true, FirstPrinter, 0 );
	addPrinterMessage( 0x128, PrMode, true, FirstPrinter, 0 );
	addPrinterMessage( 0x129, PrDisable, true, FirstPrinter, 0 );
	addPrinterMessage( 0x12a, PrEnable, true, FirstPrinter, 0 );
	addPrinterMessage( 0x12b, PrReset, true, FirstPrinter, 0 );

	addPrinterMessage( 0x12c, PrFlush, true, FirstPrinter, 0 );
	addPrinterMessage( 0x12d, PrCutterHome, true, FirstPrinter, 0 );
	addPrinterMessage( 0x12e, PrCutterCut, true, FirstPrinter, 0 );
	addPrinterMessage( 0x12f, PrResetFactoryDefaults, true, FirstPrinter, 0 );
	addPrinterMessage( 0x130, PrConfig, true, FirstPrinter, 0 );
	addPrinterMessage( 0x136, PrEnableTakeup, true, FirstPrinter, 0 );
	addPrinterMessage( 0x137, PrReqCutterStatus, true, FirstPrinter, 0 );
	addPrinterMessage( 0x138, PrReqHeadDotWear, true, FirstPrinter, 0 );
	addPrinterMessage( 0x140, PrTeach, true, FirstPrinter, 0 );
	addPrinterMessage( 0x160, PrMask, true, FirstPrinter, 0 );
	addPrinterMessage( 0x161, PrNomSize, true, FirstPrinter, 0 );
	addPrinterMessage( 0x162, PrTest, true, FirstPrinter, 0 );
	addPrinterMessage( 0x163, PrRam, true, FirstPrinter, 0 );
	addPrinterMessage( 0x16b, PrCommand, true, FirstPrinter, 0 );
	addPrinterMessage( 0x16c, PrPrintheadPower, true, FirstPrinter, 0 );
	addPrinterMessage( 0x172, PrLabelImageSize, true, FirstPrinter, 0 );
	addPrinterMessage( 0x192, PrNewOrder, true, FirstPrinter, 0 );
	addPrinterMessage( 0x193, PrHeadDotAck, true, FirstPrinter, 0 );
	addPrinterMessage( 0x195, PrReqHeadDotStatus, true, FirstPrinter, 0 );
	addPrinterMessage( 0x196, PrCutterBitDflt, true, FirstPrinter, 0 ); 		// SOF-3887
	addPrinterMessage( 0x1b0, PrStartGapCalibration, true, FirstPrinter, 0 );	// SOF-4727
	addPrinterMessage( 0x1b1, PrGapCalNext, true, FirstPrinter, 0 );
	addPrinterMessage( 0x1b2, PrUseContinuous, true, FirstPrinter, 0 );			//SOF-5124

	//build list of possible in coming printer can messages
	addPrinterMessage( 0x100, PrWakeup, false, FirstPrinter, 11 );
	addPrinterMessage( 0x102, PrKPCError, false, FirstPrinter, 8 );
	addPrinterMessage( 0x103, PrFlushCmplt, false, FirstPrinter, 1 );
	addPrinterMessage( 0x119, PrInquire, false, FirstPrinter, 42 );
	addPrinterMessage( 0x107, PrStatus, false, FirstPrinter, 13 );
	addPrinterMessage( 0x109, PrSensors, false, FirstPrinter, 28 );
	addPrinterMessage( 0x10e, PrVersion, false, FirstPrinter, 12 );
	addPrinterMessage( 0x112, PrHeadType, false, FirstPrinter, 3 );
	addPrinterMessage( 0x115, PrCutterStatus, false, FirstPrinter, 4 );
	addPrinterMessage( 0x116, PrLabelTransferReady, false, FirstPrinter, 1 );
	addPrinterMessage( 0x117, PrResetFactoryDefaultsComplete, false, FirstPrinter, 1 );
	addPrinterMessage( 0x118, PrRS485FlowControl, false, FirstPrinter, 1 );
	addPrinterMessage( 0x175, PrHeadDotWear, false, FirstPrinter, 896 );
	addPrinterMessage( 0x194, PrHeadDotStatus, false, FirstPrinter, 5 );
	addPrinterMessage( 0x1A0, PrGapCalStatus, false, FirstPrinter, 5); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::addPrinterMessage(unsigned int cobId, PrEvent prEvent, bool outgoing, PrinterType prNum, unsigned int canMsgSize) {

    if(outgoing) {
        addOutgoingPrinterMessage(cobId, prEvent, prNum);
    } else {
        addIncomingPrinterMessage(cobId, prEvent, prNum, canMsgSize);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::addOutgoingPrinterMessage(unsigned int& cobId, PrEvent& prEvent, PrinterType& prNum) {

    std::vector<canid_t> prCob;
    Debug::trace("CanLibrary.addMessage", "addOutgoingPrinterMessage(): Adding CAN outgoing message: 0x" + Hex::toString(cobId)+" to printer:" + Integer::toString(prNum));
    prCob.push_back(cobId);
    switch(prNum) {
        case FirstPrinter:
            prCanMap.insert( std::make_pair(prEvent, prCob) );
            break;
        case SecondPrinter:
            pr2CanMap.insert( std::make_pair(prEvent, prCob) );
            break;
        case ThirdPrinter:
            pr3CanMap.insert( std::make_pair(prEvent, prCob) );
            break;
        case MaxPrinterTypes:
        default:
            Debug::trace("CanLibrary.Error", "addOutgoingPrinterMessage(): Unable to add CAN outgoing message: 0x" + Hex::toString(cobId)+" to printerNum:"+Integer::toString(prNum));
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::addIncomingPrinterMessage(unsigned int& cobId, PrEvent& prEvent, PrinterType& prNum, unsigned int& canMsgSize) {

    CanLibMessage prCan;
    Debug::trace("CanLibrary.addMessage", "addIncomingPrinterMessage(): Adding CAN incoming message: 0x" + Hex::toString(cobId)+" to printer:"+Integer::toString(prNum));
    prCan.eventType = prEvent;
    prCan.cobId = cobId;
    prCan.msgSize = canMsgSize;
    switch(prNum) {
        case FirstPrinter:
            prList.push_back(prCan);
            break;
        case SecondPrinter:
            pr2List.push_back(prCan);
            break;
        case ThirdPrinter:
            pr3List.push_back(prCan);
            break;
        case MaxPrinterTypes:
        default:
            Debug::trace("CanLibrary.Error", "addIncomingPrinterMessage(): Unable to add CAN incoming message: " + Hex::toString(cobId)+" to printerNum:"+Integer::toString(prNum));
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canid_t CanLibrary::getPrinterCobID(PrEvent event, PrinterType prType) {

    PrCanMap::iterator pIter;
    PrCanMap::iterator pEndIter;
    
    if( prType == FirstPrinter ) {
        pIter = prCanMap.find(event);
        pEndIter = prCanMap.end();
    } else if( prType == SecondPrinter ) {
        pIter = pr2CanMap.find(event);
        pEndIter = pr2CanMap.end();
    } else if( prType == ThirdPrinter ) {
        pIter = pr3CanMap.find(event);
        pEndIter = pr3CanMap.end();
    } else {
        Debug::trace( "CanLibrary.Error", "getPrinterCobID(): Failed to find Printer: " + Integer::toString(prType) );
        return 0;
    }
    
    if( pIter != pEndIter ) {
        std::vector<canid_t> &ids = pIter->second;
        return ids.front();
    } else {
        Debug::trace( "CanLibrary.Error", "getPrinterCobID(): Failed to get cobid for Printer event: " + Integer::toString(event));
        return 0;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::buildWrapperLibrary() {
	FpmManager *pSerialFlash = FpmManager::getInstance();

    Debug::trace( "CanLibrary", "buildWrapperLibrary()");
	
	if( ( mySystemType == "AWS3" ) || ( mySystemType == "NGW1" ) ) {
		//build map of possible out going wrapper can messages.
		addWrapperMessage( 0x540, WrNGWReqBootStatus, true, 1 );
		addWrapperMessage( 0x541, WrNGWBootCMD, true, 1 );
		addWrapperMessage( 0x542, WrNGWBootFrame, true, 8 );
		addWrapperMessage( 0x543, WrNGWBootFrame1, true, 8 );
		addWrapperMessage( 0x544, WrNGWBootFrame2, true, 8 );
		addWrapperMessage( 0x545, WrNGWBootFrame3, true, 8 );
		addWrapperMessage( 0x546, WrNGWBootFrame4, true, 8 );
		addWrapperMessage( 0x547, WrNGWBootFrame5, true, 8 );
		addWrapperMessage( 0x548, WrNGWBootFrame6, true, 8 );
		addWrapperMessage( 0x549, WrNGWBootFrame7, true, 8 );
		addWrapperMessage( 0x54a, WrNGWBootFrame8, true, 8 );
		addWrapperMessage( 0x54e, WrNGWFlush, true, 2 );
		addWrapperMessage( 0x54c, WrNGWRequestBootVersion, true, 8 );

		addWrapperMessage( 0x532, WrLabelReady, true, 4 );
		addWrapperMessage( 0x533, WrLabelTaken, true, 4 );
		addWrapperMessage( 0x53c, WrLabelSetup, true, 12 );
		addWrapperMessage( 0x53a, WrEvolControl, true, 12 );

		addWrapperMessage( 0x530, WrEvolWrap, true, 14 );
		addWrapperMessage( 0x539, WrMode, true, 1 );

	} if( ( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" ) ) {
		//build map of possible out going wrapper can messages.
		addWrapperMessage( 0x540, WrNGWReqBootStatus, true, 1 );
		addWrapperMessage( 0x541, WrNGWBootCMD, true, 1 );
		addWrapperMessage( 0x542, WrNGWBootFrame, true, 8 );
		addWrapperMessage( 0x543, WrNGWBootFrame1, true, 8 );
		addWrapperMessage( 0x544, WrNGWBootFrame2, true, 8 );
		addWrapperMessage( 0x545, WrNGWBootFrame3, true, 8 );
		addWrapperMessage( 0x546, WrNGWBootFrame4, true, 8 );
		addWrapperMessage( 0x547, WrNGWBootFrame5, true, 8 );
		addWrapperMessage( 0x548, WrNGWBootFrame6, true, 8 );
		addWrapperMessage( 0x549, WrNGWBootFrame7, true, 8 );
		addWrapperMessage( 0x54a, WrNGWBootFrame8, true, 8 );
		addWrapperMessage( 0x54e, WrNGWFlush, true, 2 );
		addWrapperMessage( 0x54c, WrNGWRequestBootVersion, true, 8 );

		addWrapperMessage( 0x532, WrLabelReady, true, 4 );
		addWrapperMessage( 0x533, WrLabelTaken, true, 4 );
		addWrapperMessage( 0x53c, WrLabelSetup, true, 12 );
		addWrapperMessage( 0x53a, WrEvolControl, true, 12 );

		addWrapperMessage( 0x530, WrEvolWrap, true, 14 );
		addWrapperMessage( 0x539, WrMode, true, 1 );
	
	
		//new messages added for full bright CAN
		addWrapperMessage( 0x566, WrReqEvolSystemInfo, true, 0 );
		addWrapperMessage( 0x567, WrEvolSystemInfoErase, true, 0 );
		addWrapperMessage( 0x57c, WrEvolSystemInfoWrite, true, 0 );
		addWrapperMessage( 0x505, WrEvolInquire, true, 0 );
		addWrapperMessage( 0x501, WrReqWakeup, true, 0 );
		addWrapperMessage( 0x503, WrReqStatus, true, 0 );
		addWrapperMessage( 0x515, WrReqSensors, true, 0 );
		addWrapperMessage( 0x579, WrEvolTest, true, 0 );
		addWrapperMessage( 0x568, WrEvolConfig, true, 0 );
		addWrapperMessage( 0x5b7, WrEvolWriteCameraThreshold, true, 2);
		addWrapperMessage( 0x5bc, WrEvolDiagnosticsTest, true, 2 );
		addWrapperMessage( 0x511, WrLoad, true, 1 );
		addWrapperMessage( 0x5e0, WrWrapBoxSizeSave, true, 2);
		addWrapperMessage( 0x5f1, WrEvolPackageTimingsWrite, true, 30);
		addWrapperMessage(0x5f0, WrWrapBoxAck, true, 1);

		addWrapperMessage(0x618, WrPPPRequest, true, 1);
		addWrapperMessage(0x600, WrPPPWriteConfig, true, 10);
		addWrapperMessage(0x620, WrWrapperPause, true, 6);
		addWrapperMessage(0x611, WrNGWDimensionOverrides, true, 3);			// pulled from PPPWakeup  no longer used

	} if( ( mySystemType == "AWS3" ) || ( mySystemType == "NGW1" ) ) {
		//build list of possible in coming wrapper can messages
		addWrapperMessage(0x54b, WrNGWBootStatus, false, 7 );
		addWrapperMessage(0x54c, WrNGWBootVersion, false, 40 );
		addWrapperMessage(0x54d, WrNGWAck, false, 4 );

		addWrapperMessage(0x59d, WrOpen, false, 1 );
		addWrapperMessage(0x596, WrEvolStatus, false, 51 );
		addWrapperMessage(0x534, WrEvolPackageDesc, false, 20 );
		addWrapperMessage(0x594, WrEvolEventCapture, false, 8 );

		addWrapperMessage(0x710, WrNGWEngInfoN1, false, 16 );
		addWrapperMessage(0x712, WrNGWEngInfoN2, false, 16 );
		addWrapperMessage(0x714, WrNGWEngInfoN3, false, 16 );
		addWrapperMessage(0x716, WrNGWEngInfoN4, false, 16 );
		addWrapperMessage(0x718, WrNGWEngInfoN5, false, 16 );
		addWrapperMessage(0x71a, WrNGWEngInfoN6, false, 16 );

	} if( ( mySystemType == "NGW3" ) || ( mySystemType == "NGW2" ) ) {
		//build list of possible in coming wrapper can messages
		addWrapperMessage(0x54b, WrNGWBootStatus, false, 7 );
		addWrapperMessage(0x54c, WrNGWBootVersion, false, 40 );
		addWrapperMessage(0x54d, WrNGWAck, false, 4 );

		addWrapperMessage(0x59d, WrOpen, false, 1 );
		addWrapperMessage(0x596, WrEvolStatus, false, 51 );
		addWrapperMessage(0x534, WrEvolPackageDesc, false, 20 );
		addWrapperMessage(0x594, WrEvolEventCapture, false, 8 );
	
		addWrapperMessage(0x710, WrNGWEngInfoN1, false, 16 );
		addWrapperMessage(0x712, WrNGWEngInfoN2, false, 16 );
		addWrapperMessage(0x714, WrNGWEngInfoN3, false, 16 );
		addWrapperMessage(0x716, WrNGWEngInfoN4, false, 16 );
		addWrapperMessage(0x718, WrNGWEngInfoN5, false, 16 );
		addWrapperMessage(0x71a, WrNGWEngInfoN6, false, 16 );
	
		//new messages added for full bright CAN
		addWrapperMessage(0x5b0, WrEvolSystemInfo, false, 52 );

		// SOF-4775
		if (pSerialFlash->isWrapperTwoPrinter())
			addWrapperMessage(0x588, WrEvolConfig, false, 47);//increased msg size due to addition of Sec Rotation and Hand Applier timing
		else
			addWrapperMessage(0x588, WrEvolConfig, false, 42);
		

		addWrapperMessage(0x5a7, WrEvolSensors, false, 6 );
		addWrapperMessage(0x586, WrEvolWakeup, false, 4 );
		addWrapperMessage(0x5ca, WrEvolErrorHistory, false, 21 );
		addWrapperMessage(0x5a5, WrEvolTestComplete, false, 6);
		addWrapperMessage(0x5bf, WrCameraScan, false, 130);
		addWrapperMessage(0x5ba, WrCameraHeightScan, false, 2);
		addWrapperMessage(0x5b9, WrCameraCalibration, false, 8);
		addWrapperMessage(0x5af, WrEvolPackageDischarge, false, 1);
		addWrapperMessage(0x5bb, WrEvolCameraThreshold, false, 2);
		addWrapperMessage(0x5d0, WrNgwVersion, false, 90);
		addWrapperMessage(0x5b8, WrEvolMotorProfile, false, 4);
		addWrapperMessage(0x5bd, WrEvolDiagnosticsTestResult, false, 12);
		addWrapperMessage(0x5c8, WrEvolManufacturingRunInNotice, false, 1);

		addWrapperMessage(0x5e0, WrWrapBoxSize, false, 2);
		addWrapperMessage(0x5f0, WrWrapBoxAck, false, 1);
		addWrapperMessage(0x5e1, WrEvolPackageTimings,  false, 30);
		addWrapperMessage(0x5ee, WrWrapBoxTransferComplete, false, 1);

		//new printer plate position sensor
		addWrapperMessage(0x610, WrPPPWakeup, false, 9);
		#if 0	/* VL53L0X */
		addWrapperMessage(0x612, WrPPPConfig, false, 18);
		#else	/* VL53L1X */
		addWrapperMessage(0x61b, WrPPPConfig, false, 25);
		#endif
		addWrapperMessage(0x615, WrPPPDist, false, 3);
		addWrapperMessage(0x616, WrPPPCalibration, false, 10);
		addWrapperMessage(0x619, WrPPPVersion, false, 7);
		addWrapperMessage(0x61a, WrPPPStatus, false, 2 );
		addWrapperMessage(0x621, WrPPPBootVersion, false, 3 );
		addWrapperMessage(0x620, WrWrapperPause, false, 6);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::addWrapperMessage(unsigned int cobId, WrEvent wrEvent, bool outgoing, unsigned int canMsgSize) {

    if(outgoing) {
        Debug::trace("CanLibrary.addMessage", "Adding outgoing wrapper event: " + Integer::toString(wrEvent));
        std::vector<canid_t> wrCan;
        wrCan.push_back(cobId);
        wrCanMap.insert( std::make_pair( wrEvent, wrCan) );
    } else {
        Debug::trace("CanLibrary.addMessage", "Adding incoming wrapper event: " + Integer::toString(wrEvent));
        CanLibMessage WrCan;
        WrCan.eventType = wrEvent;
        WrCan.cobId = cobId;
        WrCan.msgSize = canMsgSize;
        wrList.push_back(WrCan);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canid_t CanLibrary::getWrapperCobID(WrEvent event) {

    WrCanMap::iterator pIter = wrCanMap.find(event);
    if( pIter != wrCanMap.end() ) {
        std::vector<canid_t> &ids = pIter->second;
        return ids.front();
    } else {
        Debug::trace( "CanLibrary.Error", "getWrapperCobID(): Failed to get cobid for Wrapper event: " + Integer::toString(event));
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short CanLibrary::getEventForCobId(canid_t cobID) {

    //find the matching device event for given cobID.
    unsigned short event = 0;
    CanLibMessage libMsg;
    MsgList::iterator pIter, pIterEnd;

    //determine which device list to use.
    if( ( cobID >= minPrinter1CobID ) && (cobID <= maxPrinter1CobID ) ) {
        pIter = prList.begin();
        pIterEnd = prList.end();
    } else if( ( cobID >= minPrinter2CobID ) && (cobID <= maxPrinter2CobID ) ) {
        pIter = pr2List.begin();
        pIterEnd = pr2List.end();
    } else if( ( cobID >= minPrinter3CobID ) && (cobID <= maxPrinter3CobID ) ) {
        pIter = pr3List.begin();
        pIterEnd = pr3List.end();
    } else if( ( cobID >= minWeigherCobID ) && (cobID <= maxWeigherCobID ) ) {
        pIter = wgList.begin();
        pIterEnd = wgList.end();
    } else if( ( cobID >= minWrapperCobID ) && (cobID <= maxWrapperCobID ) ) {
        pIter = wrList.begin();
        pIterEnd = wrList.end();
    } else if( cobID == kpcSystemErrCobID ) {
    	event = KpcError;
    } else if( ( cobID >= minKPCBootCobID ) && (cobID <= maxKPCBootCobID ) ) {
		pIter = kpcBootList.begin();
		pIterEnd = kpcBootList.end();
	} else if( ( cobID >= minKPCBootUpgraderCobID ) && (cobID <= maxKPCBootUpgraderCobID ) ) {
		pIter = kpcBootUpgraderList.begin();
		pIterEnd = kpcBootUpgraderList.end();
	} else if( ( cobID >= minARKBoardSiteCobID ) && (cobID <= maxARKBoardSiteCobID ) ) {
		pIter = brdSiteList.begin();
		pIterEnd = brdSiteList.end();
	} else {
        Debug::trace("CanLibrary.Error", "getEventForCobId(): Requested event for unsupported cobId: 0x" + Hex::toString(cobID));
        return event;//No point in iterating if we know we don't support this cobId
    }
    
    for( ; pIter !=  pIterEnd; pIter++ ) {
        libMsg = *pIter;
        if( libMsg.cobId == cobID ) {
            event = libMsg.eventType;
            break;
        }
    }

    if(pIter == pIterEnd) {
        Debug::trace( "CanLibrary.Error", "getEventForCobId(): Failed to find event " + Integer::toString(event) + " for CobID " + Hex::toString(cobID) );
    }
    return event;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CanLibrary::getMsgSizeForCobId(canid_t cobID) {

    //find the matching device event for given cobID.
    int size = 0;
    CanLibMessage libMsg;
    MsgList::iterator pIter, pIterEnd;

    //determine which device list to use.
    if( ( cobID >= minPrinter1CobID ) && (cobID <= maxPrinter1CobID ) ) {
        Debug::trace( "CanLibrary", "getMsgSizeForCobId(): CobID for First Printer Device.");

        pIter = prList.begin();
        pIterEnd = prList.end();
    } else if( ( cobID >= minPrinter2CobID ) && (cobID <= maxPrinter2CobID ) ) {
        pIter = pr2List.begin();
        pIterEnd = pr2List.end();
    } else if( ( cobID >= minPrinter3CobID ) && (cobID <= maxPrinter3CobID ) ) {
        pIter = pr3List.begin();
        pIterEnd = pr3List.end();
    } else if( ( cobID >= minWeigherCobID ) && (cobID <= maxWeigherCobID ) ) {
        pIter = wgList.begin();
        pIterEnd = wgList.end();
    } else if( ( cobID >= minWrapperCobID ) && (cobID <= maxWrapperCobID ) ) {
        pIter = wrList.begin();
        pIterEnd = wrList.end();
    } else if ( ( cobID >= minKPCBootCobID ) && (cobID <= maxKPCBootCobID ) ){
    	Debug::trace( "CanLibrary", "getMsgSizeForCobId(): CobID for KPC Bootloader");
		pIter = kpcBootList.begin();
    	pIterEnd = kpcBootList.end();
	} else if ( ( cobID >= minKPCBootUpgraderCobID ) && (cobID <= maxKPCBootUpgraderCobID ) ){
    	Debug::trace( "CanLibrary", "getMsgSizeForCobId(): CobID for KPC BootUpgrader");
		pIter = kpcBootUpgraderList.begin();
    	pIterEnd = kpcBootUpgraderList.end();	
	} else if( ( cobID >= minARKBoardSiteCobID ) && (cobID <= maxARKBoardSiteCobID ) ) {
		Debug::trace( "CanLibrary", "getMsgSizeForCobId(): CobID for BoardSite Testing");
		pIter = brdSiteList.begin();
    	pIterEnd = brdSiteList.end();	
    } else {
        Debug::trace("CanLibrary.Error", "getMsgSizeForCobId(): Requested message size for unsupported cobId: 0x" + Hex::toString(cobID));
        return size;//No point in iterating if we know we don't support this cobId
    }
    
    for( ; pIter !=  pIterEnd; pIter++ ) {
        libMsg = *pIter;
        if( libMsg.cobId == cobID ) {
            size = libMsg.msgSize;
            break;
        }
    }

    if(pIter == pIterEnd) {
        Debug::trace( "CanLibrary.Error", "getMsgSizeForCobId(): Failed to find message size " + Integer::toString(size) + " for CobID " + Hex::toString(cobID) );
    }
    Debug::trace( "CanLibrary", "getMsgSizeForCobId(): size " + Integer::toString(size));
    return size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::setMsgSizeForCobId(canid_t cobID, int size)
{
    //find the matching device list for given cobID.
    CanLibMessage libMsg;
    MsgList::iterator pIter, pIterEnd;
    Debug::trace("CanTranslator.Translate", + "setMsgSizeForCobId: " + Integer::toString( size ) + " For: " + Hex::toString(cobID) );
    //determine which device list to use.
    if( ( cobID >= minPrinter1CobID ) && (cobID <= maxPrinter1CobID ) ) {
        Debug::trace( "CanLibrary", "getMsgSizeForCobId(): CobID for First Printer Device.");

        pIter = prList.begin();
        pIterEnd = prList.end();
    } else if( ( cobID >= minPrinter2CobID ) && (cobID <= maxPrinter2CobID ) ) {
        pIter = pr2List.begin();
        pIterEnd = pr2List.end();
    } else if( ( cobID >= minPrinter3CobID ) && (cobID <= maxPrinter3CobID ) ) {
        pIter = pr3List.begin();
        pIterEnd = pr3List.end();
    } else if( ( cobID >= minWeigherCobID ) && (cobID <= maxWeigherCobID ) ) {
        pIter = wgList.begin();
        pIterEnd = wgList.end();
    } else if( ( cobID >= minWrapperCobID ) && (cobID <= maxWrapperCobID ) ) {
        pIter = wrList.begin();
        pIterEnd = wrList.end();
    } else {
        Debug::trace("CanLibrary.Error", "setMsgSizeForCobId(): unsupported cobId: 0x" + Hex::toString(cobID));
    }

    for( ; pIter !=  pIterEnd; pIter++ ) {
        libMsg = *pIter;
        if( libMsg.cobId == cobID ) {
        	Debug::trace("CanTranslator.Translate", + "size set!");
            libMsg.msgSize = size;
            *pIter = libMsg;
            break;
        }
    }

    if(pIter == pIterEnd) {
        Debug::trace( "CanLibrary.Error", "setMsgSizeForCobId(): Failed to cobId: " + Hex::toString(cobID) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterType CanLibrary::getPrinterPosition(canid_t cobID) {

    PrinterType position = MaxPrinterTypes;
    if( ( cobID >= minPrinter1CobID ) && (cobID <= maxPrinter1CobID ) ) {
        position = FirstPrinter;
    } else if( ( cobID >= minPrinter2CobID ) && (cobID <= maxPrinter2CobID ) ) {
        position = SecondPrinter;
    } else if( ( cobID >= minPrinter3CobID ) && (cobID <= maxPrinter3CobID ) ) {
        position = ThirdPrinter;
    } else {
        Debug::trace("CanLibrary.Error", "getPrinterPosition(): Requested printer position for unsupported cobId: 0x" + Hex::toString(cobID));
    }
    return position;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canid_t CanLibrary::getMinPrinterCobID(PrinterType prType) {
    switch(prType) {
        case FirstPrinter:
            return minPrinter1CobID;
        case SecondPrinter:
            return minPrinter2CobID;
        case ThirdPrinter:
            return minPrinter3CobID;
        default:
            Debug::trace("CanLibrary.Error", "getMinPrinterCobID(): Unsupported prPosition:" + Integer::toString(prType));
            return minPrinter1CobID;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canid_t CanLibrary::getMaxPrinterCobID(PrinterType prType) {
    switch(prType) {
        case FirstPrinter:
            return maxPrinter1CobID;
        case SecondPrinter:
            return maxPrinter2CobID;
        case ThirdPrinter:
            return maxPrinter3CobID;
        default:
            Debug::trace("CanLibrary.Error", "getMaxPrinterCobID(): Unsupported prPosition:" + Integer::toString(prType));
            return maxPrinter3CobID;
    }
}

////////////////////////////////KPC BOOTLOADER////////////////////////////////////////////////////
void CanLibrary::buildKPCBootLibrary() {

	Debug::trace( "CanLibrary", "buildKPCBootLibrary()");
	//build list of possible out going kpc bootloader can messages.

	addKPCBootMessage( 0x040, KpcBootReqStatus, true, 0 );
	addKPCBootMessage( 0x041, KpcBootCmd, true, 7 );
	addKPCBootMessage( 0x042, KpcBootFrame, true, 0 );
	addKPCBootMessage( 0x043, KpcBootFrame1, true, 0 );
	addKPCBootMessage( 0x044, KpcBootFrame2, true, 0 );
	addKPCBootMessage( 0x045, KpcBootFrame3, true, 0 );
	addKPCBootMessage( 0x046, KpcBootFrame4, true, 0 );
	addKPCBootMessage( 0x047, KpcBootFrame5, true, 0 );
	addKPCBootMessage( 0x048, KpcBootFrame6, true, 0 );
	addKPCBootMessage( 0x049, KpcBootFrame7, true, 0 );
	addKPCBootMessage( 0x04A, KpcBootFrame8, true, 0 );
	addKPCBootMessage( 0x04B, KpcBootFlush, true, 0 );
	addKPCBootMessage( 0x04C, KpcBootReqVersion, true, 0 );
	addKPCBootMessage( 0x04D, KpcBootReqAppVersion, true, 0 );

	//build list of possible in coming kpc bootloader can messages.
	addKPCBootMessage( 0x070, KpcBootStatus, false, 8 );
	addKPCBootMessage( 0x071, KpcBootVersion, false, 3 );
	addKPCBootMessage( 0x072, KpcBootAck, false, 4 );
	addKPCBootMessage( 0x073, KpcBootReady, false, 4 );
	addKPCBootMessage( 0x074, KpcBootAppVersion, false, 7 );

	//add so i can convert cobid to event for acks
	addKPCBootMessage( 0x043, KpcBootFrame1, false, 0 );
	addKPCBootMessage( 0x044, KpcBootFrame2, false, 0 );
	addKPCBootMessage( 0x045, KpcBootFrame3, false, 0 );
	addKPCBootMessage( 0x046, KpcBootFrame4, false, 0 );
	addKPCBootMessage( 0x047, KpcBootFrame5, false, 0 );
	addKPCBootMessage( 0x048, KpcBootFrame6, false, 0 );
	addKPCBootMessage( 0x049, KpcBootFrame7, false, 0 );
	addKPCBootMessage( 0x04A, KpcBootFrame8, false, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::addKPCBootMessage(unsigned int cobId, KPCBootEvent kpcEvent, bool outgoing, unsigned int canMsgSize) {
    if(outgoing) {
        Debug::trace("CanLibrary.addMessage", "addKPCBootMessage(): Adding incoming kpc boot event:" + Integer::toString(kpcEvent));
        std::vector<canid_t> kpcBootCob;
        kpcBootCob.push_back(cobId);
        kpcBootCanMap.insert( std::make_pair(kpcEvent, kpcBootCob) );
    } else {
        Debug::trace("CanLibrary.addMessage", "addKPCBootMessage(): Adding outgoing kpc boot event:"+Integer::toString(kpcEvent));
        CanLibMessage kpcBootCan;
        kpcBootCan.eventType = kpcEvent;
        kpcBootCan.cobId = cobId;
        kpcBootCan.msgSize = canMsgSize;
        kpcBootList.push_back(kpcBootCan);
    }
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanLibrary::acceptedKPCBootCobId(unsigned int cob) {
    KpcBootCanMap::iterator pIter = kpcBootCanMap.begin();
    for(;pIter != kpcBootCanMap.end(); pIter++) {
        if(std::find( (pIter->second).begin(), (pIter->second).end(), cob) != (pIter->second).end()) {
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
canid_t CanLibrary::getKPCBootCobID(KPCBootEvent event) {

    KpcBootCanMap::iterator pIter = kpcBootCanMap.find(event);
    if( pIter != kpcBootCanMap.end() ) {
        std::vector<canid_t> &ids = pIter->second;
        return ids.front();
    } else {
        Debug::trace( "CanLibrary.Error", "getKPCBootCobID(): Failed to get cobid for KPC event: " + Integer::toString(event));
        return 0;
    }
}

/////////////////////////////////////////KPC BOOT UPGRADER////////////////////////////////////////
void CanLibrary::buildKPCBootUpgraderLibrary() {

	Debug::trace( "CanLibrary", "buildKPCBootUpgraderLibrary()");
	//build list of possible out going kpc bootloader can messages.

	addKPCBootUpgraderMessage( 0x0B0, KpcBootUpgraderReqStatus, true, 0 );
	addKPCBootUpgraderMessage( 0x0B1, KpcBootUpgraderCmd, true, 4 );
	addKPCBootUpgraderMessage( 0x0B2, KpcBootUpgraderFrame, true, 0 );
	addKPCBootUpgraderMessage( 0x0B3, KpcBootUpgraderFrame1, true, 0 );
	addKPCBootUpgraderMessage( 0x0B4, KpcBootUpgraderFrame2, true, 0 );
	addKPCBootUpgraderMessage( 0x0B5, KpcBootUpgraderFrame3, true, 0 );
	addKPCBootUpgraderMessage( 0x0B6, KpcBootUpgraderFrame4, true, 0 );
	addKPCBootUpgraderMessage( 0x0B7, KpcBootUpgraderFrame5, true, 0 );
	addKPCBootUpgraderMessage( 0x0B8, KpcBootUpgraderFrame6, true, 0 );
	addKPCBootUpgraderMessage( 0x0B9, KpcBootUpgraderFrame7, true, 0 );
	addKPCBootUpgraderMessage( 0x0BA, KpcBootUpgraderFrame8, true, 0 );
	addKPCBootUpgraderMessage( 0x0BB, KpcBootUpgraderFlush, true, 0 );
	addKPCBootUpgraderMessage( 0x0BC, KpcBootUpgraderReqVersion, true, 0 );

	//build list of possible in coming kpc bootloader can messages.
	addKPCBootUpgraderMessage( 0x0D0, KpcBootUpgraderStatus, false, 8 );
	addKPCBootUpgraderMessage( 0x0D1, KpcBootUpgraderVersion, false, 9 );
	addKPCBootUpgraderMessage( 0x0D2, KpcBootUpgraderAck, false, 4 );
	addKPCBootUpgraderMessage( 0x0D3, KpcBootUpgraderReady, false, 4 );
	addKPCBootUpgraderMessage( 0x0D4, KpcBootUpgraderAppVersion, false, 7 );

	//add so i can convert cobid to event for acks
	addKPCBootUpgraderMessage( 0x0B3, KpcBootUpgraderFrame1, false, 0 );
	addKPCBootUpgraderMessage( 0x0B4, KpcBootUpgraderFrame2, false, 0 );
	addKPCBootUpgraderMessage( 0x0B5, KpcBootUpgraderFrame3, false, 0 );
	addKPCBootUpgraderMessage( 0x0B6, KpcBootUpgraderFrame4, false, 0 );
	addKPCBootUpgraderMessage( 0x0B7, KpcBootUpgraderFrame5, false, 0 );
	addKPCBootUpgraderMessage( 0x0B8, KpcBootUpgraderFrame6, false, 0 );
	addKPCBootUpgraderMessage( 0x0B9, KpcBootUpgraderFrame7, false, 0 );
	addKPCBootUpgraderMessage( 0x0BA, KpcBootUpgraderFrame8, false, 0 );
}

void CanLibrary::addKPCBootUpgraderMessage(unsigned int cobId, KPCBootUpgraderEvent kpcEvent, bool outgoing, unsigned int canMsgSize) {
    if(outgoing) {
        Debug::trace("CanLibrary.addMessage", "addKPCBootUpgraderMessage(): Adding incoming kpc boot event:" + Integer::toString(kpcEvent));
        std::vector<canid_t> kpcBootCob;
        kpcBootCob.push_back(cobId);
        kpcBootUpgraderCanMap.insert( std::make_pair(kpcEvent, kpcBootCob) );
    } else {
        Debug::trace("CanLibrary.addMessage", "addKPCBootUpgraderMessage(): Adding outgoing kpc boot event:"+Integer::toString(kpcEvent));
        CanLibMessage kpcBootCan;
        kpcBootCan.eventType = kpcEvent;
        kpcBootCan.cobId = cobId;
        kpcBootCan.msgSize = canMsgSize;
        kpcBootUpgraderList.push_back(kpcBootCan);
    }
}

void CanLibrary::addBoardSiteMessage(unsigned int cobId, BoardSiteEvent brdEvent, bool outgoing, unsigned int canMsgSize)
{
   if(outgoing) {
        Debug::trace("CanLibrary.addMessage", "Adding outgoing board site event: " + Integer::toString(brdEvent));
        std::vector<canid_t> brdSiteId;
        brdSiteId.push_back(cobId);
        brdSiteCanMap.insert( std::make_pair( brdEvent, brdSiteId ) );
    } else {
        Debug::trace("CanLibrary.addMessage", "Adding incoming board site event: " + Integer::toString(brdEvent));
        CanLibMessage brdCan;
        brdCan.eventType = brdEvent;
        brdCan.cobId = cobId;
        brdCan.msgSize = canMsgSize;
        brdSiteList.push_back(brdCan);
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CanLibrary::acceptedKPCBootUpgraderCobId(unsigned int cob) {
    KpcBootUpgraderCanMap::iterator pIter = kpcBootUpgraderCanMap.begin();
    for(;pIter != kpcBootUpgraderCanMap.end(); pIter++) {
        if(std::find( (pIter->second).begin(), (pIter->second).end(), cob) != (pIter->second).end()) {
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
canid_t CanLibrary::getKPCBootUpgraderCobID(KPCBootUpgraderEvent event) {

    KpcBootUpgraderCanMap::iterator pIter = kpcBootUpgraderCanMap.find(event);
    if( pIter != kpcBootUpgraderCanMap.end() ) {
        std::vector<canid_t> &ids = pIter->second;
        return ids.front();
    } else {
        Debug::trace( "CanLibrary.Error", "getKPCBootUpgraderCobID(): Failed to get cobid for KPC Upgrader event: " + Integer::toString(event));
        return 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void CanLibrary::buildBoardSiteLibrary()
{
	addBoardSiteMessage( 0x0F1, BrdSiteReqTestMode, false, 1 );	
	addBoardSiteMessage( 0x0F2, BrdSiteTestStatus, true, 1 );	
	addBoardSiteMessage( 0x0F3, BrdSiteBoardTest, false, 8 );	    
	addBoardSiteMessage( 0x0F4, BrdSiteBoardTestComplete, true, 8 );	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canid_t CanLibrary::getBoardSiteCobID( BoardSiteEvent brdEvent ) {

    Debug::trace( "CanLibrary", "getBoardSiteCobID() event: " + Integer::toString(brdEvent));

    BrdSiteCanMap::iterator pIter = brdSiteCanMap.find(brdEvent);
    if( pIter != brdSiteCanMap.end() ) {
        std::vector<canid_t> &ids = pIter->second;
        return ids.front();
    } else {
        Debug::trace( "CanLibrary.Error", "Failed to get cobid for board site event: " + Integer::toString(brdEvent));
        return 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
bool CanLibrary::acceptedBoardSiteCobId(unsigned int cob)
{
	BrdSiteCanMap::iterator pIter =	brdSiteCanMap.begin();
    for( ;pIter != brdSiteCanMap.end(); pIter++ ) {
        if( std::find( (pIter->second).begin(), (pIter->second).end(), cob) != (pIter->second).end() ) {
            return true;
        }
    }
    return false;

}

#endif // __linux__
