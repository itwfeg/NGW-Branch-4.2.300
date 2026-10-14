/**
 * @file BoardSiteTestManager.cpp
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Manages the board site testing of the ARK board. This Manager should only 
 *        run if boardsite file is present in /etc/hobart phase 1 or if CAN message requesting
 *        a board site manager phase 1/2. Board site manager will automatically preform all ARK
 *        board tests that can be run without supervision. The test results will be passed up to the 
 *        UI.In phase 1 the manager will wait for the board site host controller to start tests 
 *        and the result will be sent back to the host.  
 * Created on: Feb 4, 2020
 */
#include "BoardSiteTestManager.h"
#include "BoardSiteTestStatus.h"
#include "BoardSiteTest.h"
#include "BoardSiteTestComplete.h"
#include "BoardSiteEvent.h"
#include "RS485Manager.h"
#include "PythonScripter.h"
#include "KpcBootEvent.h"
#include "PrinterEvent.h"
#include "WeigherEvent.h"
#include "WeigherMode.h"
#include "KPCBootCmd.h"
#include "Printer.h"
#include "PrinterEvent.h"
#include "PrinterGeneric.h"
#include "PrinterPrinterKind.h"
#include "PrinterFactoryDefaultsComplete.h"
#include "WeigherStatus.h"
#include "WeigherInquire.h"
#include "WeigherConfiguration.h"
#include "WeigherGeneric.h"
#include "WeigherRequestStatus.h"
#include "PrinterCutterStatus.h"
#include "PrinterHeadWear.h"
#include "PrinterHeadDotStatus.h"
#include "Integer.h"
#include "Debug.h"
 
std::mutex BoardSiteTestManager::instanceMutex_;
BoardSiteTestManager *BoardSiteTestManager::pInstance_(NULL);

BoardSiteTestManager *BoardSiteTestManager::getInstance( BoardSitePhase phase )
{
	
	if ( pInstance_ )
	    return pInstance_;

    std::lock_guard<std::mutex> guard( instanceMutex_ );
    pInstance_ = new BoardSiteTestManager( phase );
	
	return pInstance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BoardSiteTestManager::BoardSiteTestManager( BoardSitePhase phase ): Hobart::CEvolMsgReceiver("BoardSiteTestManager")
{
  
    Debug::trace( "BoardSiteTestManager", "Entered constructor: " + Integer::toString(phase) );

    registerEvent( BoardSite, BrdSiteTestReqStatus );
	registerEvent( BoardSite, BrdSiteBoardTest );
    registerEvent( BoardSite, BrdSitePeripheralAttached );
    registerEvent( BoardSite, BrdSitePeripheralKPCModel );
    registerEvent( BoardSite, BrdSiteReqCompleteResults );
    registerEvent( BoardSite, BrdSitePeripheralFlashPass );
    registerEvent( BoardSite, BrdSitePeripheralFlashFail );
    
    registerEvent( Weigher, WgStatus );	
    
    if( phase_ == _PHASE0 ) { 
        registerEvent( Printer, PrResetFactoryDefaultsComplete );
        registerEvent( Printer, PrKind );
        registerEvent( Printer, PrCutterStatus );
        registerEvent( Printer, PrHeadDotStatus );
        registerEvent( Printer, PrHeadDotWear );
	
    }

#if defined ( __linux__ )
    phase_              = phase;
    state_              = _Init;
    busy_               = false;
    can0Operational_    = false;
    kpcSerialFlashTest_ = false;
    kpcModelTest_       = false;
    kpcValueMax_        = false;
    defaultsCmplt_      = false;
    memTestOnce_        = false;


    autoIndex_          = 0;
    wgStatusCntr_       = 0;
    waitCntr_           = 0;

    cState_ = CHome_;

    uiMsg.setSourceObject( this );
    cUiMsg.setSourceObject( this );
    
    if( phase_ == _PHASE0) {
        Debug::trace( "BoardSiteTestManager", "initialize list of auto tests.");

        
        autoTestsList_.push_back( _ARK_SERIAL_FLASH );
        autoTestsList_.push_back( _ARK_SD_SLOT_0 );
        autoTestsList_.push_back( _ARK_SD_SLOT_1 );    
        autoTestsList_.push_back( _ARK_USB );
        autoTestsList_.push_back( _ARK_WIFI );
        autoTestsList_.push_back( _ARK_ETHERNET );
               
        autoTestsList_.push_back( _ARK_CAN_0 );
        autoTestsList_.push_back( _KPC_CAN0 );
        autoTestsList_.push_back( _KPC_MODEL );
        autoTestsList_.push_back( _KPC_SERIAL_FLASH );
        autoTestsList_.push_back( _KPC_VALUEMAX );
        autoTestsList_.push_back( _KPC_CUTTER );
        autoTestsList_.push_back( _ARK_MEMORY ); 
        autoTestsList_.push_back( _ARK_BLUE );
        autoTestsList_.push_back( _KPC_DOTWEAR );
        autoTestsList_.push_back( _ARK_RTC );
    } else if( phase_ == _LVDS_ONLY_PHASE0 ) {
        //wait for session to startup before we start.
        PAL_absclass::PAL_sleep(1000);
    } 

    pMyRunThread_ = new Util::CTLSThread( *this, 1, 0, "BoardSiteTestManager" );    
#endif

    Debug::trace( "BoardSiteTestManager", "Exit constructor");    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BoardSiteTestManager::~BoardSiteTestManager()
{
 
	Debug::trace("BoardSiteTestManager", "~BoardSiteTestManager(): --Entry");
	deregisterAllEvents();

	ShutdownNicely();

    #if defined ( __linux__ )
	//tell the thread to exit
	m_ExitEvent.EventSet();

	//wait for the thread to exit before deleting all the resources!
	if ( pMyRunThread_ ) {
		while ( pMyRunThread_->IsRunning() ) {
			PAL_absclass::PAL_sleep(1);
		}
		delete pMyRunThread_;
		pMyRunThread_ = NULL;
	}
    #endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD BoardSiteTestManager::Run()
{
	do
	{
        //yield thread
        PAL_absclass::PAL_sleep( 1 );

        std::lock_guard<std::mutex> guard( mutex_ );

        switch( state_ ) 
        {
            case _Init: {
                Debug::trace("BoardSiteTestManager", "_Init: _ProcessTest");
                if( phase_ == _PHASE0 ) {
                    //process the list of test we will automatically execute.
                    autoProcessTest();                                   
                } else {
                    Debug::trace("BoardSiteTestManager", "Run(): notify host we are ready to start processing tests");
                    // notify host we are ready to start processing tests.
                    BoardSiteTestStatus status( this, true );
                    status.setSourceObject(this);
                    dispatchEvent(&status);

                    state_ = _WaitingForTest;
                }
                break;
            }
            case _WaitingForTest: {
                break;
            }
            case _ProcessTest: {               
                if( phase_ == _PHASE0 ) {
                    Debug::trace("BoardSiteTestManager", "Run(): _ProcessTest");
                    //finish our test list.
                    autoProcessTest();    
                }
                break;
            }
            case _SendTestResults: {
                Debug::trace("BoardSiteTestManager", "Run(): _SendTestResults");
                dispatchEvent( &uiMsg );
                state_ = _Exit;
                break;
            }
            case _StartUpKpc: {    
                Debug::trace("BoardSiteTestManager", "Run(): _StartUpKpc");
                PAL_absclass::PAL_sleep( 10 );        
                //start our KPC timer then wait for response.
                createTimer( waitTime, KPCTimeout_, CHobartTimer::PERIODIC_TIMER );
                state_ = _WaitingForKPC;
                break;
            }
            case _SetupForDefaults: {
                Debug::trace("BoardSiteTestManager", "Run(): _SetupForDefaults");  
                //start our KPC timer then wait for response.
                createTimer( waitTime, KPCTimeout_, CHobartTimer::PERIODIC_TIMER );
                state_ = _WaitForDefaults;
                break;
            }
            case _SendCutterStatus: {
                    Debug::trace( "BoardSiteTestManager", "_SendCutterStatus" );
                    PrinterGeneric msg( PrReqCutterStatus, this, FirstPrinter );
                    dispatchEvent( &msg );
                    state_ = _WaitForCutterStatus;
                break;
            }
            case _WaitingForKPC:
            case _WaitForKPCModel:
            case _WaitForWgStatus: 
            case _WaitForCutterCut:
            case _WaitForCutterStatus:
            case _WaitForCutterComplete:
            case _WaitForDotWear:
            case _WaitForDefaults: {
                if( state_ == _WaitForDefaults ) {
                    Debug::trace("BoardSiteTestManager", "Run(): _WaitForDefaults");  
                    waitCntr_++;
                    if( waitCntr_ >= WAIT_RETRY_CNT ) {
                        waitCntr_ = 0;
                        Debug::trace("BoardSiteTestManager", "_WaitForDefaults: PrResetFactoryDefaults");
                        //prep for the next two tests
                        PrinterGeneric genMsg( PrResetFactoryDefaults, this, FirstPrinter );
                        dispatchEvent( &genMsg ); 
                    }
                }
                if( state_ == _WaitingForKPC )
                    Debug::trace("BoardSiteTestManager", "Run(): _WaitingForKPC");
                if( state_ == _WaitForKPCModel )
                    Debug::trace("BoardSiteTestManager", "Run(): _WaitForKPCModel");
                if( ( state_ == _WaitForWgStatus ) && ( kpcValueMax_ ) ) {
                    Debug::trace( "BoardSiteTestManager","Value max test done process next test.");
                    state_ = _ProcessTest;
                } else {
                    Debug::trace("BoardSiteTestManager", "Run(): _WaitForWgStatus"); 
                }
                if( state_ == _WaitForCutterCut )
                    Debug::trace("BoardSiteTestManager", "Run(): _WaitForCutterCut");
                if( state_ == _WaitForCutterStatus )
                    Debug::trace("BoardSiteTestManager", "Run(): _WaitForCutterStatus");
                if( state_ == _WaitForCutterComplete )
                    Debug::trace("BoardSiteTestManager", "Run(): _WaitForCutterComplete");
                if( state_ == _WaitForDotWear )
                    Debug::trace("BoardSiteTestManager", "Run(): _WaitForDotWear");
                
                PAL_absclass::PAL_sleep( 50 );
                break;
            }

            case _StartCutterTest: {
                Debug::trace("BoardSiteTestManager", "Run(): _StartCutterTest");  
                createTimer( waitTime, KPCCutterTimeout_, CHobartTimer::PERIODIC_TIMER );

                PrinterGeneric msg( PrCutterCut, this, FirstPrinter );
                dispatchEvent( &msg );
                state_ = _WaitForCutterComplete;
                break;
            }
            case _Exit: {
                Debug::trace("BoardSiteTestManager", "Run(): _Exit");
                //TO DO destroy thread... Just sleep for now.
                PAL_absclass::PAL_sleep( 9000 );
                break;
            }
        }       
	} while ( !m_ExitEvent.IsEventSet() ); 

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BoardSiteTestManager::timeoutProcess(__int32 timer_id )
{
    std::lock_guard<std::mutex> g( mutex_ );
    if( timer_id == KPCTimeout_ )  {
        if( state_ == _WaitingForKPC ) {
            Debug::trace("BoardSiteTestManager", "timeoutProcess(): _WaitingForKPC");    
            state_ = _ProcessTest;
        } else if ( state_ == _WaitForDefaults ) {
            Debug::trace("BoardSiteTestManager", "timeoutProcess(): _WaitForDefaults");   
            state_ = _ProcessTest;
        }
    }
    if( timer_id == KPCCutterTimeout_ ) {
        Debug::trace("BoardSiteTestManager", "timeoutProcess(): KPCCutterTimeout_"); 
        if( ( state_ == _WaitForCutterStatus  ) || ( state_ == _WaitForCutterCut  ) ){
            //test failed. Execute the next test.
            state_ = _ProcessTest; 
        } 
    }
    if( timer_id == KPCDotWearTimeOut_ ) {
        Debug::trace("BoardSiteTestManager", "timeoutProcess(): KPCDotWearTimeOut_");
        if( state_ == _WaitForDotWear ) {
            //circuit is unusable on this rev of board....
            uiMsg.setKpcDotWearTest();
            //test failed. Execute the next test.
            state_ = _ProcessTest; 
        }     
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BoardSiteTestManager::processEvent( Util::CSmartPtr<EvolMessageWrapper> & msg )
{
	if ( !msg )
	{
        Debug::trace( "BoardSiteTestManager.Error", "Received NULL message" );
        return;
	}

	std::lock_guard<std::mutex> guard( mutex_ );

    Debug::trace( "BoardSiteTestManager", "processEvent() device: " + Integer::toString(msg->getDeviceType()) );
    Debug::trace( "BoardSiteTestManager", "processEvent() event: " + Integer::toString(msg->getEventType()) );
    if( phase_ == _PHASE0 ) {
        if( msg->getDeviceType() == BoardSite ) {
            switch ( msg->getEventType() )
            {
                case BrdSiteTestReqStatus: {
                    BoardSiteTestStatus status( this, busy_ );
                    status.setSourceObject(this);
                    dispatchEvent(&status);
                    break;
                }
                case BrdSiteBoardTest: {
                    state_ = _ProcessTest;
                    Debug::trace( "BoardSiteTestManager", "processEvent() BrdSiteBoardTest");                
                    //processTest( msg->Data() );
                    break;
                }
                case BrdSitePeripheralAttached: {
                    Debug::trace( "BoardSiteTestManager", "Received BrdSitePeripheralAttached message" );
                    //we have been notified that the KPC is out of it's bootloader.
                    can0Operational_ = true;    

                    //did we receive the message before we could get started?
                    if( ( state_ == _StartUpKpc ) || ( state_ == _WaitingForKPC ) ) {
                        Debug::trace( "BoardSiteTestManager", "Received before: " + Integer::toString(state_) );
                        //cancel our wait timer.
                        cancelTimer( KPCTimeout_ );
                        //resume our testing. 
                        state_ = _ProcessTest;    
                    }

                    deregisterEvent( BoardSite, BrdSitePeripheralAttached );
                    break;
                }
                case BrdSitePeripheralKPCModel: {
                    Debug::trace( "BoardSiteTestManager", "Received BrdSitePeripheralKPCModel message" );
                    kpcModelTest_ = true;
                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::KpcModelTest );
                    cUiMsg.setPercentComplete( 45 );
                    dispatchEvent( &cUiMsg );
                    break;
                }
                case BrdSiteReqCompleteResults: {
                    Debug::trace( "BoardSiteTestManager", "Received BrdSiteReqCompleteResults message" );
                    dispatchEvent( &uiMsg );
                    break;
                }
                case BrdSitePeripheralFlashPass: {
                    Debug::trace( "BoardSiteTestManager", "Received BrdSitePeripheralFlash message" );                    
                    if( ( state_ == _SetupForDefaults ) || ( state_ == _WaitForDefaults ) ) {

                        //cancel our wait timer.
                        cancelTimer( KPCTimeout_ );
                        defaultsCmplt_ = true;
                        kpcSerialFlashTest_ = true;                        
                        state_ = _ProcessTest;

                        deregisterEvent( Printer, PrResetFactoryDefaultsComplete );
                    }                
                    break;
                }
                case BrdSitePeripheralFlashFail: {
                    if( ( state_ == _SetupForDefaults ) || ( state_ == _WaitForDefaults ) ) {

                        //cancel our wait timer.
                        cancelTimer( KPCTimeout_ );
                        defaultsCmplt_ = true;
                        kpcSerialFlashTest_ = false;                        
                        state_ = _ProcessTest;

                        deregisterEvent( Printer, PrResetFactoryDefaultsComplete );
                    }
                    break;
                }
                case BrdSitePeripheralValuePass: {
                    kpcValueMax_ = true;
                    state_ = _ProcessTest; 
                    deregisterEvent( Weigher, WgStatus );
                    break;
                }
                case BrdSitePeripheralCutterStat: {
                    Debug::trace( "BoardSiteTestManager", "Received BrdSitePeripheralCutterStat message" );                    
                    break;
                }
                default:
                    break;
            }
        } else if( msg->getDeviceType() == Printer ) {
            if( msg->getEventType() == PrResetFactoryDefaultsComplete ) {
                PrinterFactoryDefaultsComplete cmpltMsg( msg->Data() );
                Debug::trace( "BoardSiteTestManager", "Received PrResetFactoryDefaultsComplete message" );

                if( ( state_ == _SetupForDefaults ) || ( state_ == _WaitForDefaults ) ) {

                    //cancel our wait timer.
                    cancelTimer( KPCTimeout_ );
                    defaultsCmplt_ = true;
                
                    if( cmpltMsg.getSavedStatus() ) {
                        kpcSerialFlashTest_ = true;
                    } else {
                        kpcSerialFlashTest_ = false;
                    }
                    
                    state_ = _ProcessTest;
                    deregisterEvent( Printer, PrResetFactoryDefaultsComplete );
                }                
            } else if( msg->getEventType() == PrKind ) {
                PrinterPrinterKind kindMsg( msg->Data() );
                
                Debug::trace( "BoardSiteTestManager", "Received PrKind message: " + Integer::toString(kindMsg.getPrinterKind()) );
                if( ( kindMsg.getPrinterKind() == KPCPrinter ) || 
                     ( kindMsg.getPrinterKind() == SSKPCPrinter ) ) {
                    kpcModelTest_ = true;
                } else {
                    kpcModelTest_ = false;                    
                }
                
                deregisterEvent( Printer, PrKind );
            } else if( msg->getEventType() == PrCutterStatus ) {
                cancelTimer( KPCCutterTimeout_ );

                Debug::trace( "BoardSiteTestManager", "Received PrCutterStatus" );
                PrinterCutterStatus status( msg->Data() );
                Debug::trace( "BoardSiteTestManager", "installed: " + Integer::toString(status.getInstalled()));
                Debug::trace( "BoardSiteTestManager", "home: " + Integer::toString(status.getHome()));
                Debug::trace( "BoardSiteTestManager", "interlock: " + Integer::toString(status.getInterlock()));
                Debug::trace( "BoardSiteTestManager", "jamed: " + Integer::toString(status.getJammed()));
                if( status.getInstalled() ) {
                    if( ( cState_ == CHome_ ) && ( !status.getHome() ) ) {
                        Debug::trace( "BoardSiteTestManager", "cState_: CNotHome_" );
                        cState_ = CNotHome_;
                    }
                    if( ( cState_ == CNotHome_ ) && ( status.getHome() ) ) {
                        Debug::trace( "BoardSiteTestManager", "cState_: CReturnHome_" );
                        cState_ = CReturnHome_;
                        uiMsg.setKpcCutterTest(); 
                        state_ = _ProcessTest;                    
                    }
                } else {
                    Debug::trace( "BoardSiteTestManager", "Cutter not installed!" );
                    //Cutter device not detected.
                    uiMsg.clrKpcCutterTest();
                }
                
                if( ( state_ == _WaitForCutterStatus ) && ( status.getInstalled() ) ) {
                    state_ = _StartCutterTest;
                } else {                
                    if( ( state_ == _WaitForCutterComplete ) && ( status.getJammed() ) ) {
                        Debug::trace( "BoardSiteTestManager", "Cutter jammed!" );
                        uiMsg.clrKpcCutterTest();
                        state_ = _ProcessTest;    
                    }
                    if( ( !status.getInstalled() ) && ( state_ == _WaitForCutterStatus ) ) {
                        state_ = _ProcessTest; 
                    }
                }  
            } else if( msg->getEventType() == PrHeadDotWear ) {      
                Debug::trace( "BoardSiteTestManager", "Received PrHeadDotWear" );      
                cancelTimer( KPCDotWearTimeOut_ );
                
                PrinterHeadWear dotWear( msg->Data() );

                bool functional_ = false;
                
                //if we have any good dots then dot wear is working.
                for( int i =0; i < dotWear.getHeadSize(); i++ ) {
                    if( dotWear.getDotStatus(i) == DotGood ) {
                        functional_ = true;
                        break;
                    }
                }
                
                if( functional_ ) {
                    uiMsg.setKpcDotWearTest();
                } else {
                    uiMsg.clrKpcDotWearTest();
                }
                state_ = _ProcessTest;
            } else if( msg->getEventType() ==  PrHeadDotStatus ) {
                Debug::trace( "BoardSiteTestManager", "Received PrHeadDotStatus" );                  
                cancelTimer( KPCDotWearTimeOut_ );
                
                PrinterHeadDotStatus status( msg->Data() );
                Debug::trace( "BoardSiteTestManager", "Status: " + Integer::toString(status.getStatus()) );

                if( ( status.getStatus() == PrinterHeadDotStatus::_AllDotsGood ) || ( status.getStatus() == PrinterHeadDotStatus::_DotsMixed ) ) {
                    Debug::trace( "BoardSiteTestManager", "_AllDotsGood" );
                    uiMsg.setKpcDotWearTest();
                } else if( status.getStatus() == PrinterHeadDotStatus::_AllDotsBad ) {
                    Debug::trace( "BoardSiteTestManager", "_AllDotsBad" );
                    uiMsg.clrKpcDotWearTest();
                }else {
                    Debug::trace( "BoardSiteTestManager", "_TestFailed" ); 
                    uiMsg.clrKpcDotWearTest();
                } 
                state_ = _ProcessTest;
            }
        } else if( msg->getDeviceType() == Weigher ) {
            if( msg->getEventType() ==  WgStatus ) {
                WeigherStatus statMsg( msg->Data() );
                wgStatusCntr_++;
                Debug::trace( "BoardSiteTestManager", "Received WgStatus message x: " + Integer::toString( statMsg.getAccelX() ) 
                + " y: " + Integer::toString( statMsg.getAccelY() ) + " z: " + Integer::toString( statMsg.getAccelZ() ));
                
                //do we have accelerometer data? If the KPC is talking to the accelerometer 
                //then one of these should be non zero.
                if( ( statMsg.getAccelX() != 0 ) ||  ( statMsg.getAccelY() != 0 ) ||  
                    ( statMsg.getAccelZ() != 0 ) ){
                    Debug::trace( "BoardSiteTestManager","kpcValueMax_ = true");
                    kpcValueMax_ = true;
                    
                    deregisterEvent( Weigher, WgStatus );
                    registerEvent( Printer, PrCutterStatus );
                } else {
                    kpcValueMax_ = false;
                }
                /* first several status messages the accel values are zero 
                causing intermittent failure of the valuemax test. */
                if( ( wgStatusCntr_ >= MAX_STATUS_CNT ) && ( kpcValueMax_ ) && ( autoIndex_ == 10  )) {
                    state_ = _ProcessTest; 

                    deregisterEvent( Weigher, WgStatus );
                    registerEvent( Printer, PrCutterStatus );
                }            
            } 
        }
    } else {
        if( msg->getDeviceType() == BoardSite ) {
            switch ( msg->getEventType() )
            {
                case BrdSiteBoardTest: {
                    Debug::trace( "BoardSiteTestManager", " BrdSiteBoardTest process test!" );     
                    state_ = _ProcessTest;
                    processTest( msg->Data() );
                    break;
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BoardSiteTestManager::autoProcessTest( void )
{    
    if( !autoTestsList_.empty() ) {
        for( autoIndex_; autoIndex_ < autoTestsList_.size(); autoIndex_++  ) {
            Debug::trace( "BoardSiteTestManager", "autoIndex_ " + Integer::toString( autoIndex_ ) );
            switch( autoTestsList_[ autoIndex_ ] )
            {
                case _ARK_SERIAL_FLASH: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_SERIAL_FLASH" );
                    //test performs read, write, erase on serial flash and verifies
                    const std::string command( PythonScripter::buildPythonCall("boardSiteFlashTest.py") );
                    std::string testResults;
                    PAL_absclass::PAL_executeWithResults(command, testResults);

                    if ( testResults[0] == '0' ) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_SERIAL_FLASH test complete: pass" );
                        //notify host that the test passed!
                        uiMsg.setArkSerialFlashTest();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_SERIAL_FLASH test complete: failed" );
                        uiMsg.clrArkSerialFlashTest();
                    }
                    
                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkFlashTest );
                    cUiMsg.setPercentComplete( 5 );
                    dispatchEvent( &cUiMsg );
                    break;
                }    
                case _ARK_SD_SLOT_0: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_SD_SLOT_0" );            
                    const std::string command( PythonScripter::buildPythonCall("boardSiteSD0Test.py") );
                    std::string testResults = "";
                    PAL_absclass::PAL_executeWithResults( command, testResults );

                    if( testResults[0] == '0' ) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_SD_SLOT_0 test complete: pass" );
                        uiMsg.setArkSDSlot0Test();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_SD_SLOT_0 test complete: failed = " + testResults );
                        uiMsg.clrArkSDSlot0Test();
                    }               

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkSDSlot0Test );
                    cUiMsg.setPercentComplete( 10 );
                    dispatchEvent( &cUiMsg );
                    break;
                }    
                case _ARK_SD_SLOT_1: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_SD_SLOT_1" );            
                    const std::string command( PythonScripter::buildPythonCall("boardSiteSD1Test.py") );
                    std::string testResults = "";
                    PAL_absclass::PAL_executeWithResults( command, testResults );

                    if( testResults[0] == '0' ) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_SD_SLOT_1 test complete: pass" );
                        uiMsg.setArkSDSlot1Test();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_SD_SLOT_1 test complete: failed = " + testResults );
                        uiMsg.clrArkSDSlot1Test();
                    }               

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkSDSlot1Test );
                    cUiMsg.setPercentComplete( 15 );
                    dispatchEvent( &cUiMsg );
                    break;
                }    
                case _ARK_RTC: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_RTC" );            
                    const std::string command( PythonScripter::buildPythonCall("boardSiteRTCTest.py") );
                    std::string testResults = "";
                    PAL_absclass::PAL_executeWithResults( command, testResults );
                    if( testResults[0] == '0' ) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_RTC test complete: pass" );
                        uiMsg.setArkRTCTest();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_RTC test complete: failed" );
                        uiMsg.clrArkRTCTest();
                    }

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkRTCTest );
                    cUiMsg.setPercentComplete( 100 );
                    dispatchEvent( &cUiMsg );
                    break;
                }    
                case _ARK_USB: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_USB" );
                    //launch python script that looks for devices on the USB ports. Hung to define!
                    const std::string command( PythonScripter::buildPythonCall("boardSiteUSBTest.py") );
                    std::string testResults = "";
                    PAL_absclass::PAL_executeWithResults(command, testResults);
                    
                    if ( testResults[0] == '0' ) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_USB test complete: pass" );
                        uiMsg.setArkUSBTest();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_USB test complete: failed " + testResults );
                        uiMsg.clrArkUSBTest();
                    }

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkUSBTest );
                    cUiMsg.setPercentComplete( 20 );
                    dispatchEvent( &cUiMsg );
                    break;
                }    
                case _ARK_WIFI: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_WIFI" );
                    const std::string command( PythonScripter::buildPythonCall("boardSiteWifiTest.py") );
                    std::string testResults = "";
                    PAL_absclass::PAL_executeWithResults(command, testResults);
                    //test result should be a list of AP's unless test failed then it should be empty
                    if ( testResults.length() != 0 ) {
                        if( testResults.length() > 3 ) {
                            Debug::trace( "BoardSiteTestManager", " _ARK_WIFI test complete: pass" );
                            uiMsg.setArkWifiTest();
                        } else if( ( testResults.find('2') != -1 ) && (  testResults.find('2') >= 1 ) ){
                            Debug::trace( "BoardSiteTestManager", " _ARK_WIFI test complete: failed" );
                            uiMsg.clrArkWifiTest();
                        } else {
                            Debug::trace( "BoardSiteTestManager", " _ARK_WIFI test complete: failed" );
                            uiMsg.clrArkWifiTest();
                        }
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_WIFI test complete: failed" );
                        uiMsg.clrArkWifiTest();
                    }

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkWifiTest );
                    cUiMsg.setPercentComplete( 25 );
                    dispatchEvent( &cUiMsg );
                    break;
                }
                case _ARK_BLUE: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_BLUE" );
                    const std::string command( PythonScripter::buildPythonCall("boardSiteBlueTest.py") );
                    std::string testResults = "";
                    PAL_absclass::PAL_executeWithResults(command, testResults);
                    //test result should be a list of AP's unless test failed then it should be empty
                    if ( testResults[0] == '0' ) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_BLUE test complete: pass" );
                        uiMsg.setArkBlueToothTest();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_BLUE test complete: failed" );
                        uiMsg.clrArkBlueToothTest();
                    }

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkBToothTest );
                    cUiMsg.setPercentComplete( 70 );
                    dispatchEvent( &cUiMsg );

                    //start our next test  
                    createTimer( waitTime , KPCDotWearTimeOut_, CHobartTimer::PERIODIC_TIMER );                   
                    Debug::trace( "BoardSiteTestManager", "performing test: _KPC_DOTWEAR" );
                    PrinterGeneric msg( PrReqHeadDotStatus, this, FirstPrinter );
                    dispatchEvent( &msg );
                    state_ = _WaitForDotWear;
                    break;
                }    
                case _ARK_ETHERNET: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_ETHERNET" );            
                    const std::string command( PythonScripter::buildPythonCall("boardSiteEtherTest.py") );
                    std::string testResults = "";
                    PAL_absclass::PAL_executeWithResults(command, testResults);
                    if (testResults.length() != 0) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_ETHERNET test complete: passed" );
                        uiMsg.setArkEthernetTest();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_ETHERNET test complete: failed" );
                        uiMsg.clrArkEthernetTest();
                    }

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkEthernetTest );
                    cUiMsg.setPercentComplete( 30 );
                    dispatchEvent( &cUiMsg );

                    //is the peripheral alreay up?
                    if( !can0Operational_ ) {
                        state_ = _StartUpKpc;   
                    }            
                    break;
                }    
                case _ARK_MEMORY: {
                    if( !memTestOnce_ ) {
                        Debug::trace( "BoardSiteTestManager", "performing test: _ARK_MEMORY" );   
                        //testing of 10M takes ~40 sec.
                        int result = system(" /usr/bin/memtester 10M 1"); 
                        if( !result ) {
                            Debug::trace( "BoardSiteTestManager", " _ARK_MEMORY test complete: passed" );
                            uiMsg.setArkMemoryTest();
                        } else {
                            Debug::trace( "BoardSiteTestManager", " _ARK_MEMORY test complete: failed" );
                            uiMsg.clrArkMemoryTest();
                        }

                        //update UI with our current progress.
                        cUiMsg.setTest( BoardSitePercentComplete::ArkMemTest );
                        cUiMsg.setPercentComplete( 65 );
                        dispatchEvent( &cUiMsg );

                        memTestOnce_ = true;
                    }
                    break;
                }
                case _ARK_CAN_0: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_CAN_0" );
                    if( can0Operational_ ) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_CAN_0 test complete: passed" );
                        uiMsg.setArkCan0Test();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_CAN_0 test complete: failed" );
                        uiMsg.clrArkCan0Test();
                    }

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::ArkCan0Test );
                    cUiMsg.setPercentComplete( 35 );
                    dispatchEvent( &cUiMsg );

                    //request the kind for model test
                    PrinterGeneric msg( PrReqKind, this, FirstPrinter );
                    dispatchEvent( &msg );
                    break;
                } 
                case _KPC_CAN0: {
                    Debug::trace( "BoardSiteTestManager", "performing test: _KPC_CAN0" );
                    if( can0Operational_ ) {
                        Debug::trace( "BoardSiteTestManager", " _KPC_CAN0 test complete: passed" );
                        uiMsg.setKpcCanTest();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _KPC_CAN0 test complete: failed" );
                        uiMsg.clrKpcCanTest();

                        //no use going any further.
                        state_ = _SendTestResults;
                    }                    

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::KpcCan0Test );
                    cUiMsg.setPercentComplete( 40 );
                    dispatchEvent( &cUiMsg );
                    break;
                }   
                case _KPC_MODEL: {
                    //if CAN bus is operational then we should have already received the prKind msg.
                    if( can0Operational_ ) {
                        if( kpcModelTest_ ) {
                            Debug::trace( "BoardSiteTestManager", " _KPC_MODEL test complete: passed" );
                            uiMsg.setKpcModelTest();                            
                        } else {
                            Debug::trace( "BoardSiteTestManager", " _KPC_MODEL test complete: failed" );
                            uiMsg.clrKpcModelTest();
                        }
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _KPC_CAN0 & _KPC_MODEL test complete: failed" );
                        uiMsg.clrKpcModelTest();
                    }


                    //if we have not already received the message 
                    if( !defaultsCmplt_ )  {  
                        //prep for the next two tests
                        PrinterGeneric genMsg( PrResetFactoryDefaults, this, FirstPrinter );
                        dispatchEvent( &genMsg );
                        
                        Debug::trace( "BoardSiteTestManager", " Sending PrResetFactoryDefaults" );
                        //PAL_absclass::PAL_sleep( 100 );
                        
                        state_ = _SetupForDefaults;
                    }

                    WeigherRequestStatus statMsg( this );
                    dispatchEvent( &statMsg );                    

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::KpcModelTest );
                    cUiMsg.setPercentComplete( 45 );
                    dispatchEvent( &cUiMsg );                                       
                    break;
                }    
                case _KPC_SERIAL_FLASH: {   
                    if( kpcSerialFlashTest_ ) {
                        Debug::trace( "BoardSiteTestManager", " _KPC_SERIAL_FLASH test complete: passed" );
                        uiMsg.setKpcSerialFlashTest();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _KPC_SERIAL_FLASH test complete: failed" );
                        uiMsg.clrKpcSerialFlashTest();
                    }
                    Debug::trace( "BoardSiteTestManager", "wgStatusCntr_: " + Integer::toString(wgStatusCntr_) );
                    //we may have received all the status messages at this point.
                    if( wgStatusCntr_ < MAX_STATUS_CNT ) {  
                        state_ = _WaitForWgStatus;
                    } else if( wgStatusCntr_ == 0 ) {
                        WeigherRequestStatus statMsg( this );
                        dispatchEvent( &statMsg ); 
                        state_ = _WaitForWgStatus;                   
                    } 

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::KpcSerialFlashTest );
                    cUiMsg.setPercentComplete( 50 );
                    dispatchEvent( &cUiMsg );
                    break;
                } 
                case _KPC_VALUEMAX: {
                    if( kpcValueMax_ ) {
                        Debug::trace( "BoardSiteTestManager", " _KPC_VALUEMAX test complete: passed" );
                        uiMsg.setKpcValueMaxTest();
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _KPC_VALUEMAX test complete: failed" );
                        uiMsg.clrKpcValueMaxTest();
                    }
                    Debug::trace( "BoardSiteTestManager", " _KPC_VALUEMAX reset weigher defaults!" );
                    // tell the weigher to restore configuration to default values.
                    WeigherGeneric wMsg( WgResetFactoryDefaults, this );
	                dispatchEvent( &wMsg );

                    //Cutter test: check if installed before running test.
                    createTimer( waitTime, KPCCutterTimeout_, CHobartTimer::PERIODIC_TIMER );
                    
                    //_WaitForCutterStatus;
                    state_ = _SendCutterStatus;

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::KpcValueMaxTest );
                    cUiMsg.setPercentComplete( 55 );
                    dispatchEvent( &cUiMsg );
                    break;
                }
                case _KPC_CUTTER: {                                        
                    Debug::trace( "BoardSiteTestManager", "performing test: _KPC_CUTTER" );
                    if( uiMsg.getKpcCutterResult() ) {
                        Debug::trace( "BoardSiteTestManager", "_KPC_CUTTER test complete: passed" );
                    } else {
                        Debug::trace( "BoardSiteTestManager", "_KPC_CUTTER test complete: failed" );
                    } 

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::KpcCutterTest );
                    cUiMsg.setPercentComplete( 60 );
                    dispatchEvent( &cUiMsg );
                    break;
                }
                case _KPC_DOTWEAR: {                    
                    if( uiMsg.getKpcDotWearResult() ) {
                        Debug::trace( "BoardSiteTestManager", "_KPC_DOTWEAR test complete: passed" );
                    } else {
                        Debug::trace( "BoardSiteTestManager", "_KPC_DOTWEAR test complete: failed" );                        
                    }

                    //update UI with our current progress.
                    cUiMsg.setTest( BoardSitePercentComplete::KpcDotWearTest );
                    cUiMsg.setPercentComplete( 75 );
                    dispatchEvent( &cUiMsg );
                    break;
                }   
            }
            
            if( ( state_ == _StartUpKpc ) || ( state_ == _SetupForDefaults ) || (  state_ == _SendTestResults ) 
                || ( state_ == _SendCutterStatus ) || ( state_ == _WaitForWgStatus ) || ( state_ == _WaitForDotWear ) ) {
                Debug::trace( "BoardSiteTestManager", "state " +  Integer::toString(state_) + " autoIndex_ " + Integer::toString( autoIndex_ ) );
                autoIndex_++;
                break;
            }                
        }
        if( autoIndex_ == autoTestsList_.size() ) {
            Debug::trace( "BoardSiteTestManager", "end of auto testing.");
            state_ = _SendTestResults;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  IO defined in userspace /sys/class/gpio
//  lvds0 backlight_enable: gpio39      lvds1 backlight_enable: gpio38 
//  lvds0 backlight_voltage: gpio32     lvds1 backlight_voltage: gpio35
//  lvds1 display_detect: gpio105       lvds1 display_detect: gpio174
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////
void BoardSiteTestManager::processTest( const char *recvBuffer )
{
    if( recvBuffer != NULL ) {
        busy_ = true;
        BoardSiteTest test( recvBuffer );
        Debug::trace( "BoardSiteTestManager", "processEvent() BrdSiteBoardTest getTestCode: " + Integer::toString(test.getTestCode()));                
        switch( test.getTestCode() )
        {
            case _ARK_NO_TEST: {
                break;
            }
            case _ARK_IO: {
                break;
            }
            case _ARK_CONFIGURATION_IO: {
                break;
            }
            case _ARK_SERIAL_FLASH: {
                Debug::trace( "BoardSiteTestManager", "processTestperforming test: _ARK_SERIAL_FLASH" );
                //test performs read, write, erase on serial flash and verifies
                const std::string command( PythonScripter::buildPythonCall("boardSiteFlashTest.py") );
				std::string testResults;
				PAL_absclass::PAL_executeWithResults(command, testResults);

                if ( testResults[0] == '0' ) {
                    Debug::trace( "BoardSiteTestManager", " _ARK_SERIAL_FLASH test complete: pass" );
                    //notify host that the test passed!
                    BoardSiteTestComplete msg( this, true );
                    msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                } else {
                    Debug::trace( "BoardSiteTestManager", " _ARK_SERIAL_FLASH test complete: failed" );
                    //notify host that the test failed!
                    BoardSiteTestComplete msg( this, false );
                    msg.setErrorCode( BoardSiteTestComplete::BSTSFWrite );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                }
                break;
            }
            case _ARK_CAN_0: {
                //if we have reached this manager then out CAN interface is good.
                BoardSiteTestComplete msg( this, true );
                msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                msg.setSourceObject( this );
                dispatchEvent( &msg );
                break;
            }
            case _ARK_CAN_1: {
                break;
            }
            case _ARK_AUDIO: {
                Debug::trace( "BoardSiteTestManager", "performing test: _ARK_AUDIO" );
                system("/usr/bin/aplay /usr/local/hobart/audio/audiocheck_1khz_sin.wav"); 
                // notify host we are ready so the host can sample the IO.
                BoardSiteTestStatus status( this, true );
                status.setSourceObject(this);
    			dispatchEvent(&status);                                           
                break;
            }
            case _ARK_USB: {
                Debug::trace( "BoardSiteTestManager", "performing test: _ARK_USB" );
                //launch python script that looks for devices on the USB ports. Hung to define!
                const std::string command( PythonScripter::buildPythonCall("boardSiteUSBTest.py") );
				std::string testResults = "";
				PAL_absclass::PAL_executeWithResults(command, testResults);

                Debug::trace( "BoardSiteTestManager", "testResults: " + testResults);
                if ( testResults[0] == '0' ) {
                    Debug::trace( "BoardSiteTestManager", " _ARK_USB test complete: pass" );
                    //notify host that the test passed!
                    BoardSiteTestComplete msg( this, true );
                    msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                } else {
                    Debug::trace( "BoardSiteTestManager", " _ARK_USB test complete: failed " + testResults );
                    BoardSiteTestComplete msg( this, false );                   
                    msg.setSourceObject( this );

                    if( testResults[0] == '1' ) {
                        msg.setExpected(1);
                        msg.setErrorCode( BoardSiteTestComplete::BSTUSB_1 );
                    } else if( testResults[0] == '2' ) {
                        msg.setExpected(2);
                        msg.setErrorCode( BoardSiteTestComplete::BSTUSB_2 );
                    } else if( testResults[0] == '3' ) {
                        msg.setExpected(3);
                        msg.setErrorCode( BoardSiteTestComplete::BSTUSB_3 );
                    } else if( testResults[0] == '4' ) {
                        msg.setExpected(4);
                        msg.setErrorCode( BoardSiteTestComplete::BSTUSB_4 );
                    } else if( testResults[0] == '5' ) {
                        msg.setExpected(5);
                        msg.setErrorCode( BoardSiteTestComplete::BSTUSB_5 );
                    } else if( testResults[0] == '6' ) {
                        msg.setExpected(6);
                        msg.setErrorCode( BoardSiteTestComplete::BSTUSB_6 );
                    } else if( testResults[0] == '7' ) {                        
                        msg.setExpected(7);
                        msg.setErrorCode( BoardSiteTestComplete::BSTUSB_7 );
                    } else if( testResults[0] == '8' ) {                        
                        msg.setExpected(8);
                        msg.setErrorCode( BoardSiteTestComplete::BSTUSB_8 );
                    } 

                    msg.setResult(0);
                    //notify host that the test failed!
                    dispatchEvent( &msg );
                }
                break;
            }
            case _ARK_WIFI: {
                Debug::trace( "BoardSiteTestManager", "performing test: _ARK_WIFI" );
                const std::string command( PythonScripter::buildPythonCall("boardSiteWifiTest.py") );
				std::string testResults = "";
				PAL_absclass::PAL_executeWithResults(command, testResults);
                //test result should be a list of AP's unless test failed then it should be empty
                if ( testResults.length() != 0 ) {
                    if( testResults == "2" ) {
                        Debug::trace( "BoardSiteTestManager", " _ARK_WIFI test complete: failed" );
                        BoardSiteTestComplete msg( this, false );
                        msg.setErrorCode( BoardSiteTestComplete::BSTWIFICARDNOTPRESENT );
                        msg.setSourceObject( this );
                        dispatchEvent( &msg );
                    } else {
                        Debug::trace( "BoardSiteTestManager", " _ARK_WIFI test complete: pass" );
                        BoardSiteTestComplete msg( this, true );
                        msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                        msg.setSourceObject( this );
                        dispatchEvent( &msg );
                    }
                } else {
                    Debug::trace( "BoardSiteTestManager", " _ARK_WIFI test complete: failed" );
                    BoardSiteTestComplete msg( this, false );
                    msg.setErrorCode( BoardSiteTestComplete::BSTWIFI );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                }
                break;
            }
            case _ARK_BLUE: {
                break;
            }    
            case _ARK_ETHERNET: {   
                Debug::trace( "BoardSiteTestManager", "performing test: _ARK_EHTERNET" );            
                const std::string command( PythonScripter::buildPythonCall("boardSiteEtherTest.py") );
				std::string testResults = "";
				PAL_absclass::PAL_executeWithResults(command, testResults);
                if (testResults.length() != 0) {
                    BoardSiteTestComplete msg( this, true );
                    msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                } else {
                    BoardSiteTestComplete msg( this, false );
                    msg.setErrorCode( BoardSiteTestComplete::BSTETHERNET );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                }               
                break;
            }
            case _ARK_LVDS0: { 
                // no completion message needed for this test.               
                if( test.getTestParam1() == _VideoRed ) {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS0  _VideoRed" );            
                    system(" /usr/local/hobart/python/red.sh");                     
                } else if( test.getTestParam1() == _VideoBlue ) {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS0  _VideoBlue" );            
                    system(" /usr/local/hobart/python/blue.sh"); 
                } else if( test.getTestParam1() == _VideoGreen ) {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS0  _VideoGreen" );            
                    system(" /usr/local/hobart/python/green.sh"); 
                } else if( test.getTestParam1() == _Backlight5V ) {                                                          
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS0  _Backlight5V" );  
                    system( "echo 0 > /sys/class/gpio/gpio32/value" );           
                } else if( test.getTestParam1() == _Backlight12V ) {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS0  _Backlight12V" );                                
                    system( "echo 1 > /sys/class/gpio/gpio32/value" );                   
                } else if( test.getTestParam1() == _BacklightOn ) {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS0  _BacklightOn" );            
                    system( "echo 1 > /sys/class/gpio/gpio39/value" ); 
                } else if( test.getTestParam1() == _BacklightOff ) {
                    Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS0  _BacklightOff" );            
                    system( "echo 0 > /sys/class/gpio/gpio39/value" ); 
                }
                // notify host we are ready so the host can sample the IO.
                BoardSiteTestStatus status( this, true );
                status.setSourceObject(this);
    			dispatchEvent(&status);                            
                break;
            }
            case _ARK_LVDS1: {
                if( phase_ != _LVDS_ONLY_PHASE0 ) {
                    // no completion message needed for this test.               
                    if( test.getTestParam1() == _VideoRed ) {
                        Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS1  _VideoRed" );            
                        system(" /usr/local/hobart/python/red1.sh"); 
                    } else if( test.getTestParam1() == _VideoBlue ) {
                        Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS1  _VideoBlue" );            
                        system(" /usr/local/hobart/python/blue1.sh"); 
                    } else if( test.getTestParam1() == _VideoGreen ) {
                        Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS1  _VideoGreen" );            
                        system(" /usr/local/hobart/python/green1.sh"); 
                    } else if( test.getTestParam1() == _Backlight5V ) {
                        Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS1  _Backlight5V" );
                        system( "echo 0 > /sys/class/gpio/gpio35/value" );                                 
                    } else if( test.getTestParam1() == _Backlight12V ) {
                        Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS1  _Backlight12V" );            
                        system( "echo 1 > /sys/class/gpio/gpio35/value" ); 
                    } else if( test.getTestParam1() == _BacklightOn ) {
                        Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS1  _BacklightOn" ); 
                        system( "echo 1 > /sys/class/gpio/gpio38/value" ); 
                    } else if( test.getTestParam1() == _BacklightOff ) {
                        Debug::trace( "BoardSiteTestManager", "performing test: _ARK_LVDS1  _BacklightOff" );
                        system( "echo 0 > /sys/class/gpio/gpio38/value" );
                    }
                    // notify host we are ready so the host can sample the IO.
                    BoardSiteTestStatus status( this, true );
                    status.setSourceObject(this);
                    dispatchEvent(&status);
                } else {

                    // For standalone LVDS GCD test fixture. Turn on 5V backlight and paint display red, blue, green 
                    Debug::trace( "BoardSiteTestManager", " _LVDS_ONLY_PHASE0 performing test: _ARK_LVDS1  _Backlight5V" );
                    system( "echo 0 > /sys/class/gpio/gpio35/value" ); 
                    Debug::trace( "BoardSiteTestManager", " _LVDS_ONLY_PHASE0 performing test: _ARK_LVDS1  _BacklightOn" ); 
                    system( "echo 1 > /sys/class/gpio/gpio38/value" ); 
                    Debug::trace( "BoardSiteTestManager", " _LVDS_ONLY_PHASE0 performing test: _ARK_LVDS1  _VideoRed" );            
                    system(" /usr/local/hobart/python/red1.sh"); 
 
                    PAL_absclass::PAL_sleep( 1000 );

                    cUiMsg.setTest( BoardSitePercentComplete::ArkFlashTest );
                    cUiMsg.setPercentComplete( 50 );
                    dispatchEvent( &cUiMsg );
                    
                    
                    Debug::trace( "BoardSiteTestManager", "_LVDS_ONLY_PHASE0 performing test: _ARK_LVDS1  _VideoBlue" );            
                    system(" /usr/local/hobart/python/blue1.sh"); 

                    PAL_absclass::PAL_sleep( 1000 );

                    cUiMsg.setTest( BoardSitePercentComplete::ArkFlashTest );
                    cUiMsg.setPercentComplete( 75 );
                    dispatchEvent( &cUiMsg );
                    

                    Debug::trace( "BoardSiteTestManager", " _LVDS_ONLY_PHASE0 performing test: _ARK_LVDS1  _VideoGreen" );            
                    system(" /usr/local/hobart/python/green1.sh");
                    
                    PAL_absclass::PAL_sleep( 1000 );
                  
                    cUiMsg.setTest( BoardSitePercentComplete::ArkFlashTest );
                    cUiMsg.setPercentComplete( 100 );
                    dispatchEvent( &cUiMsg );

                    state_ = _WaitingForTest;
                }
                break;
            }
            case _ARK_RESET_BUTTON: {
                break;
            }
            case _ARK_SD_SLOT_0: {
                Debug::trace( "BoardSiteTestManager", "performing test: _ARK_SD_SLOT_0" );            
                const std::string command( PythonScripter::buildPythonCall("boardSiteSD0Test.py") );
				std::string testResults = "";
				PAL_absclass::PAL_executeWithResults( command, testResults );

                if( testResults[0] == '0' ) {
                    Debug::trace( "BoardSiteTestManager", " _ARK_SD_SLOT_0 test complete: pass" );
                    BoardSiteTestComplete msg( this, true );
                    msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                } else {
                    Debug::trace( "BoardSiteTestManager", " _ARK_SD_SLOT_0 test complete: failed = " + testResults );
                    BoardSiteTestComplete msg( this, false );
                    if( testResults[0] == '1' ) {
                        msg.setErrorCode( BoardSiteTestComplete::BSTMissSD0Card );
                    } else if( testResults[0] == '2' ) {
                        msg.setErrorCode( BoardSiteTestComplete::BSTSD0Write );
                    } else if( testResults[0] == '3' ) {
                        msg.setErrorCode( BoardSiteTestComplete::BSTSD0Erase );                        
                    }
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                }               
                break;
            }
            case _ARK_SD_SLOT_1: {
                Debug::trace( "BoardSiteTestManager", "performing test: _ARK_SD_SLOT_1" );            
                const std::string command( PythonScripter::buildPythonCall("boardSiteSD1Test.py") );
				std::string testResults = "";
				PAL_absclass::PAL_executeWithResults( command, testResults );
                if( testResults[0] == '0' ) {
                    Debug::trace( "BoardSiteTestManager", " _ARK_SD_SLOT_1 test complete: pass" );
                    BoardSiteTestComplete msg( this, true );
                    msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                } else {
                    Debug::trace( "BoardSiteTestManager", " _ARK_SD_SLOT_1 test complete: failed" );
                    BoardSiteTestComplete msg( this, false );
                    if( testResults[0] == '1' ) {
                        msg.setErrorCode( BoardSiteTestComplete::BSTMissSD1Card );
                    } else if( testResults[0] == '2' ) {
                        msg.setErrorCode( BoardSiteTestComplete::BSTSD1Write );
                    } else if( testResults[0] == '3' ) {
                        msg.setErrorCode( BoardSiteTestComplete::BSTSD1Erase );                        
                    }
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                }               
                break;
            }
            case _ARK_MEMORY: {
                Debug::trace( "BoardSiteTestManager", "performing test: _ARK_MEMORY" );   
                //testing of 10M takes ~40 sec.
                int result = system(" /usr/bin/memtester 10M 1"); 
                if( !result ) {
                    BoardSiteTestComplete msg( this, true );
                    msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                } else {
                    BoardSiteTestComplete msg( this, false );
                    
                    msg.setExpected(0);
                    msg.setResult( (unsigned short)result );
                    msg.setSourceObject( this );
                    if( result == 1 ) {
                        msg.setErrorCode( BoardSiteTestComplete::BSTMemoryFailure_1 );
                    } else if( result == 2 ) {
                      msg.setErrorCode( BoardSiteTestComplete::BSTMemoryFailure_2 );  
                    } else if( result == 3 ) {
                       msg.setErrorCode( BoardSiteTestComplete::BSTMemoryFailure_3 );  
                    }
                    dispatchEvent( &msg );
                }
                break;
            }
            case _ARK_RTC: {
                Debug::trace( "BoardSiteTestManager", "performing test: _ARK_RTC" );            
                const std::string command( PythonScripter::buildPythonCall("boardSiteRTCTest.py") );
				std::string testResults = "";
				PAL_absclass::PAL_executeWithResults( command, testResults );
                if( testResults[0] == '0' ) {
                    Debug::trace( "BoardSiteTestManager", " _ARK_RTC test complete: pass" );
                    BoardSiteTestComplete msg( this, true );
                    msg.setErrorCode( BoardSiteTestComplete::BSTNoError );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );

                } else {
                    Debug::trace( "BoardSiteTestManager", " _ARK_RTC test complete: failed" );
                    BoardSiteTestComplete msg( this, false );
                    msg.setErrorCode( BoardSiteTestComplete::BSTRTCFailure );
                    msg.setSourceObject( this );
                    dispatchEvent( &msg );
                }
                break;
            }
            case _KPC_RS485_SETUP: {
                //we do nothing here. K64 host issues the transfer size.
                break;
            }
            case _KPC_RS485_TRANSFER: {
                //send test pattern out rs485 interface
                RS485Manager *pRS485Mgr = RS485Manager::getInstance();
                if( pRS485Mgr != NULL ) {
                    pRS485Mgr->transferTestPattern();                    
                } else {
                    busy_ = false;
                    //notify host that the test failed!
                    BoardSiteTestComplete msg( this, false );
                    msg.setErrorCode( BoardSiteTestComplete::BSTRs485Init );
                    msg.setSourceObject(this);
                    dispatchEvent(&msg);
                }
                break;
            }
        }
    }
    busy_ = false;
}

void BoardSiteTestManager::processError( BoardSiteTests test, int errNumber )
{
    BoardSiteTestComplete msg( this, false );    
    msg.setSourceObject(this);

    switch( test )
    {
        case _ARK_IO: {
            break;
        }
        case _ARK_CAN_0: {
            msg.setErrorCode( BoardSiteTestComplete::BSTCan0 );
            break;
        }
        case _ARK_SERIAL_FLASH: {
            msg.setErrorCode( BoardSiteTestComplete::BSTSFWrite );
            break;
        }
        case _ARK_SD_SLOT_0: {
            if( errNumber == 1 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTMissSD0Card );
            } else if( errNumber == 2 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTSD0Write );
            } else if( errNumber == 3 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTSD0Erase );
            } else {
                //unknown error
            }
            break;
        }
        case _ARK_SD_SLOT_1: {
            if( errNumber == 1 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTMissSD1Card );
            } else if( errNumber == 2 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTSD1Write );
            } else if( errNumber == 3 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTSD1Erase );
            } else {
                //unknown error
            }
            break;
        }
        case _ARK_RTC: {
            msg.setErrorCode( BoardSiteTestComplete::BSTRTCFailure );
            break;
        }
        case _ARK_USB: {
            if( errNumber == 1 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTUSB_1 );
            } else if( errNumber == 2 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTUSB_2 );
            } else if( errNumber == 3 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTUSB_3 );                
            } else if( errNumber == 4 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTUSB_4 );                
            } else if( errNumber == 5 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTUSB_5 );                
            } else if( errNumber == 6 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTUSB_6 );                
            } else if( errNumber == 7 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTUSB_7 );                
            } else if( errNumber == 8 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTUSB_8 );                
            } else {
                //unknown error
            }
            break;
        }
        case _ARK_WIFI: {
            if( errNumber == 1 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTWIFI );  
            } else if( errNumber == 2 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTWIFICARDNOTPRESENT );                  
            } else {
                //unknown error
            }
            break;
        }
        case _ARK_BLUE: {
            if( errNumber == 1 ){
                msg.setErrorCode( BoardSiteTestComplete::BSTBlue_1 );                                  
            } else if( errNumber == 2 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTBlue_2 );
            } else if( errNumber == 3 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTBlue_3 );
            } else {
                //unknown error
            }
            break;
        }
        case _ARK_ETHERNET: {
            msg.setErrorCode( BoardSiteTestComplete::BSTETHERNET );
            break;
        }
        case _ARK_MEMORY: {
            if( errNumber == 1 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTMemoryFailure_1 );
            } else if( errNumber == 2 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTMemoryFailure_2 );
            } else if( errNumber == 4 ) {
                msg.setErrorCode( BoardSiteTestComplete::BSTMemoryFailure_3 );
            } else {
                //unknown error
            }
            break;
        }
        case _KPC_MODEL: {
            msg.setErrorCode( BoardSiteTestComplete::BSTKPCModel );
            break;
        }
        case _KPC_VALUEMAX: {
            msg.setErrorCode( BoardSiteTestComplete::BSTKPCValueMax );
            break;
        }
        case _KPC_SERIAL_FLASH: {
            msg.setErrorCode( BoardSiteTestComplete::BSTKPCSerialFlash );
            break;
        }
        case _KPC_CAN0: {
            msg.setErrorCode( BoardSiteTestComplete::BSTKPCCan );
            break;
        }
        case _KPC_CUTTER: {
            msg.setErrorCode( BoardSiteTestComplete::BSTKPCCutter );
            break;
        }
        case _KPC_DOTWEAR: {
            msg.setErrorCode( BoardSiteTestComplete::BSTKPCDotWear );
            break;
        }

        //Phase 1 Tests
        case _ARK_CAN_1: 
        case _ARK_AUDIO: 
        case _ARK_LVDS0: 
        case _ARK_LVDS1: 
        case _KPC_MEDIA: 
        case _KPC_IO: 
        case _KPC_RS485_SETUP: 
        case _KPC_RS485_TRANSFER: 
        case _KPC_WGH_ADC: 
        case _KPC_PRINT_HEAD: 
        case _KPC_STEPPER: 
        case _KPC_FAN: 
        case _KPC_LABEL_TAKEN: 
        case _ARK_RESET_BUTTON: 
        case _KPC_WGH_SWITCH: {
            //unsupported tests
            break;
        }
    }
    dispatchEvent(&msg);
}