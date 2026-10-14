/**
 * @file UnitTest.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Unit test for testing the peripheral messaging
 * Created on: Jul 24, 2015
 */
#include "UnitTest.h"
//Weigher Messages
#include "WeigherRequestStatus.h"
#include "WeigherRequestWakeup.h"
#include "WeigherMode.h"
#include "WeigherRequestInquire.h"
#include "WeigherGeneric.h"
#include "WeigherControl.h"
#include "WeigherConfiguration.h"
#include "WeigherRequestVersion.h"
#include "WeigherRequestSerialNum.h"

//Printer Messages
#include "PrinterRequestInquire.h"
#include "PrinterGeneric.h"
#include "PrinterRequestWakeup.h"
#include "PrinterRequestStatus.h"
#include "PrinterRequestPrintheadPower.h"
#include "PrinterRequestSensors.h"
#include "PrinterRequestInquire.h"
#include "PrinterFlush.h"
#include "PrinterEnable.h"
#include "PrinterDisable.h"
#include "PrinterMode.h"
#include "PrinterTest.h"
#include "PrinterEnableTakeup.h"
#include "PrinterRequestCalibrate.h"
#include "PrinterRequestLabelTransfer.h"
#include "PrinterConfiguration.h"
#include "PrinterNomSize.h"
#include "PrinterRequestVersion.h"
#include "PrinterMask.h"
#if 0
//NEW Printer messages
#include "PrinterLabelTransfer.h"
#include "PrinterExecute.h"
#include "PrinterRequest.h"
#endif

//Wrapper Messages
#include "WrapperRequestWakeup.h"
#include "WrapperRequestStatus.h"
#include "EvolWrapperInquire.h"
#include "EvolWrapperCycle.h"
#include "WrapperRequestSensors.h"
#include "EvolWrapperReqSystemInfo.h"
#include "EvolWrapperSystemInfoErase.h"
#include "EvolWrapperSystemInfoWrite.h"
#include "EvolWrapperReqSystemReset.h"
#include "EvolWrapperLoad.h"
#include "EvolWrapperControl.h"
#include "EvolWrapperWrap.h"
#include "WrapperMode.h"
#include "EvolWrapperTest.h"
#include "EvolWrapperConfiguration.h"
#include "EvolWrapperHeaterConfig.h"
#include "EvolWrapperGeneralConfig.h"
#include "EvolLabelApplierConfiguration.h"
#include "EvolWrapperSaveCycleCount.h"
#include "EvolWrapperReqCycleCount.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "NGWLabelReady.h"
#include "NGWLabelTaken.h"
#include "NGWLabelSetup.h"
#include "LabelApplier.h"
#include "NGWApplier.h"
#include "NGWRequestVersion.h"
#include "EvolWrapperWrap.h"
#include "WrapperIntakeMode.h"
#include "Wrapper.h"

//NGW Upgrade Messages
#include "NGWReqBootStatus.h"
#include "NGWRequestBootVersion.h"
#include "NGWBootStatus.h"
#include "NGWBootVersion.h"
#include "NGWBootNodeCMD.h"
#include "NGWBootFrame.h"
#include "NGWAck.h"
#include "NGWFlush.h"
#include "NGWBulkTransfer.h"
#include "NGWStartUpgrade.h"
#include "NGWUpgradeStatus.h"
#include "NGWUpgradeProgress.h"
#include "NGWUpgradeFinished.h"
#include "NGWUpgradeRequired.h"
#include "NGWUpgradeUIReady.h"
#include "NGWGeneric.h"
#include "LabelApplierLabelReady.h"
#include "LabelApplierLabelTaken.h"
#include "LabelApplierSetup.h"

#include "WrPPPConfig.h"
#include "WrPPPDist.h"
#include "WrPPPCalibration.h"
#include "WrPPPWakeup.h"
#include "WrPPPRequest.h"
#include "WrPPPVersion.h"
#include "WrPPPStatus.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"


UnitTest *UnitTest::pInstance = (UnitTest *) NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
UnitTest *UnitTest::getInstance(DUTTestType test) {
    if(pInstance == 0) {
        pInstance = new UnitTest(test);
    }
    return pInstance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UnitTest::UnitTest(DUTTestType test): Hobart::CEvolMsgReceiver("UnitTest") {
    Debug::trace( "UnitTest", "Start Unit Test...." );

    _testType = test;

    _wait = false;

    _weigherTestComplete = false;
    _printerTestComplete = true;
    _wrapperTestComplete = true;
    _upgradeTestComplete = false;
    _pppTestComplete =  false;

    _executeTest = false;
    _testCount = 1;
    _testTime = 1000 * 5;   //ticks per second * seconds  30

    Debug::trace( "UnitTest", "Starting  unit test of: " + Integer::toString(_testType));
    if( _testType == _UpgradeUI_Messaging ) {
		registerEvent(NGWrapper, _UpgradeUIReady);
		registerEvent(NGWrapper, _StartUpgrade);
    }

    if( _testType == _PPPMessaging ) {

		registerEvent(Wrapper, WrPPPWakeup);
		registerEvent(Wrapper, WrPPPConfig);
		registerEvent(Wrapper, WrPPPDist);
        registerEvent(Wrapper, WrPPPCalibration);
		registerEvent(Wrapper, WrPPPVersion);
        registerEvent(Wrapper, WrPPPRequest);
        registerEvent(Wrapper, WrPPPStatus);
    }

    /* start test timer */
	createTimer(_testTime, _MessageTimer, CHobartTimer::PERIODIC_TIMER);

    _runThread = new Util::CTLSThread(*this, 1, 0, "UnitTest Thread" );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
UnitTest::~UnitTest() {
    Debug::trace( "UnitTest", "~UnitTest()" );
    //tell the thread to exit
    m_ExitEvent.EventSet();

    if( _runThread ) {
        while ( _runThread->IsRunning() )
            PAL_absclass::PAL_sleep( 1 );
        _runThread = NULL;
    }

    cancelTimer(_MessageTimer);
    shutdownTimerThreads();
    ShutdownNicely();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::testPrepackMessaging( unsigned short test ) {
    if( !_executeTest ) {
        Debug::trace( "UnitTest", "testWeigherMessaging(): " + Integer::toString(test) );
        switch(test) {
            case 1: {
                Debug::trace( "UnitTest", "Test: WeigherRequestInquire" );
                WeigherRequestInquire wr( this );
                dispatchEvent( &wr );
                _executeTest = true;
                break;
            }
            case 2: {
                Debug::trace( "UnitTest", "Test: WeigherRequestStatus" );
                WeigherRequestStatus ws(this);
                dispatchEvent( &ws );
                _executeTest = true;
                break;
            }
            case 3: {
                PrinterRequestVersion prv(this);
                dispatchEvent(&prv);
                _executeTest = true;
                break;
            }
            case 4: {
                Debug::trace( "UnitTest", "Test: PrinterRequestWakeup" );
                PrinterRequestWakeup pw(this);
                pw.setPrinterType(FirstPrinter);
                dispatchEvent( &pw );
                _executeTest = true;
                break;
            }
            case 5: {
                Debug::trace( "UnitTest", "Test: PrinterRequestStatus" );
                PrinterRequestStatus ps(this);
                ps.setPrinterType(FirstPrinter);
                dispatchEvent( &ps );
                _executeTest = true;
                break;
            }
            case 6: {
                Debug::trace( "UnitTest", "Test: PrinterRequestSensors" );
                PrinterRequestSensors ps(this);
                ps.setPrinterType(FirstPrinter);
                dispatchEvent(&ps);
                _executeTest = true;
                break;
            }
            case 7: {
                Debug::trace( "UnitTest", "Test: PrinterRequestInquire" );
                PrinterRequestInquire pi(this);
                pi.setPrinterType(FirstPrinter);
                dispatchEvent(&pi);
                _executeTest = true;
                break;
            }
            case 8: {
                WrapperRequestWakeup wrw(this);
                dispatchEvent(&wrw);
                _executeTest = true;
                break;
            }
            case 9: {
                Debug::trace( "UnitTest", "Test: WrapperRequestStatus" );
                WrapperRequestStatus wrs(this);
                dispatchEvent(&wrs);
                _executeTest = true;
                break;
            }
            case 10: {
                //EvolWrapperInquire wi(this);
                //wi.setConfigStatus(WrapperConfigError);
                //dispatchEvent(&wi);
                _executeTest = true;
                break;
            }
            case 11: {
                Debug::trace( "UnitTest", "Test: WeigherRequestVersion" );
                WeigherRequestVersion wrv(this);
                dispatchEvent(&wrv);
                _executeTest = true;
                break;
            }
            case 12: {
                Debug::trace( "UnitTest", "Test: WeigherRequestSerialNum" );
                WeigherRequestSerialNum wrs(this);
                dispatchEvent(&wrs);
                _executeTest = true;
                break;
            }
            case 13: {
                Debug::trace( "UnitTest", "Test: EvolWrapperReqCycleCount" );
                EvolWrapperReqCycleCount wrcc(this);
                dispatchEvent(&wrcc);
                _executeTest = true;
                break;
            }
            case 14: {
                Debug::trace( "UnitTest", "Test: EvolWrapperReqSystemInfo" );
                EvolWrapperReqSystemInfo rsi(this);
                dispatchEvent(&rsi);
                _executeTest = true;
                break;
            }
            case 15: {
                Debug::trace( "UnitTest", "Test: EvolWrapperTest" );
                EvolWrapperTest wt(this);
                wt.setTestCode(EvolWrapperTest::KnifeTest);
                wt.setDataInt1(0);
                wt.setDataInt2(0);
                wt.setDataLong(0);
                dispatchEvent(&wt);
                _executeTest = true;
                break;
            }
            case 16: {
                Debug::trace( "UnitTest", "Test: WrapperRequestSensors" );
                WrapperRequestSensors wrs(this);
                dispatchEvent(&wrs);
                _executeTest = true;
                break;
            }
            case 17: {
                Debug::trace( "UnitTest", "Test: EvolWrapperLoad" );
                EvolWrapperLoad wl(this, LowerRoll);
                dispatchEvent(&wl);
                _executeTest = true;
                break;
            }
            case 18: {
                Debug::trace( "UnitTest", "Test: EvolWrapperWrap" );
                EvolWrapperWrap ww(this, WrapperIntakeMode::FastIntake, 15, true, 150, 3223);
                dispatchEvent(&ww);
                _executeTest = true;
                break;
            }
            case 19: {
                Debug::trace( "UnitTest", "Test: WrapperMode" );
                WrapperMode wm(this, WrServiceMode);
                dispatchEvent(&wm);
                _executeTest = true;
                break;
            }
            case 20: {
                Debug::trace( "UnitTest", "Test: NGWRequestVersion" );
                NGWRequestVersion rv(this);
                dispatchEvent(&rv);
                _executeTest = true;
                break;
            }
            case 21: {
                Debug::trace( "UnitTest", "Test: Skip" );
                test = 0;
                _executeTest = true;
                break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool UnitTest::testWeigherMessaging( unsigned short test ) {
    bool result = false;
    if( !_executeTest ) {
        Debug::trace( "UnitTest", "testWeigherMessaging(): " + Integer::toString(test) );
        switch(test) {
            case 1: {
                Debug::trace( "UnitTest", "Test: WeigherRequestWakeup" );
                WeigherRequestWakeup ww( this );
                dispatchEvent( &ww );
                _executeTest = true;
                break;
            }
            case 2: {
                Debug::trace( "UnitTest", "Test: WeigherMode" );
                WeigherMode wm( this, ScalePrepackMode );
                dispatchEvent( &wm );
                _executeTest = true;
                break;
            }
            case 3: {
                Debug::trace( "UnitTest", "Test: WeigherEnable" );
                WeigherGeneric wg( WgEnable, this );
                wg.setDeviceType( Weigher );
                dispatchEvent( &wg );
                _executeTest = true;
                break;
            }
            case 4: {
                Debug::trace( "UnitTest", "Test: WeigherDisable" );
                WeigherGeneric wg( WgDisable, this );
                wg.setDeviceType( Weigher );
                dispatchEvent( &wg );
                _executeTest = true;
                break;
            }
            case 5: {
                Debug::trace( "UnitTest", "Test: WeigherRezero" );
                WeigherGeneric wg( WgRezero, this );
                wg.setDeviceType( Weigher );
                dispatchEvent( &wg );
                _executeTest = true;
                break;
            }
            case 6: {
                Debug::trace( "UnitTest", "Test: WeigherReset" );
                WeigherGeneric wg( WgReset, this );
                wg.setDeviceType( Weigher );
                dispatchEvent( &wg );
                _executeTest = true;
                break;
            }
            case 7: {
                Debug::trace( "UnitTest", "Test: WeigherColdReset" );
                WeigherGeneric wg( WgColdReset, this );
                dispatchEvent( &wg );
                _executeTest = true;
                break;
            }
            case 8: {
                Debug::trace( "UnitTest", "Test: WeigherControl" );
                WeigherControl wc(this, 0x01);
                dispatchEvent( &wc );
                _executeTest = true;
                break;
            }
            case 9: {
                Debug::trace( "UnitTest", "Test: WeigherRequestStatus" );
                WeigherRequestStatus ws(this);
                dispatchEvent( &ws );
                _executeTest = true;
                break;
            }
            case 10: {
                Debug::trace( "UnitTest", "Test: WeigherConfiguration" );
                WeigherConfiguration wc(this);
                dispatchEvent( &wc );
                _weigherTestComplete = true;
                break;
            }
            default: {
                result = true;
                break;
            }
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::dispatchPrinterRequest(PrRequestCode prReqCode) {
    #if 0
    HobString debugHeader = HobString("dispatchPrinterRequest(): ");
    Debug::trace("UnitTest", debugHeader+"PrinterRequest code:"+Integer::toString(prReqCode));
    PrinterRequest prReq(this, FirstPrinter, prReqCode);
    dispatchEvent(&prReq);
    
    PrinterRequest prReq2(this, SecondPrinter, prReqCode);
    dispatchEvent(&prReq2);
    
    PrinterRequest prReq3(this, ThirdPrinter, prReqCode);
    dispatchEvent(&prReq3);
    #endif
    _executeTest = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::dispatchPrinterExecute(PrExecuteCode prExecCode) {
    HobString debugHeader = HobString("dispatchPrinterExecute(): ");
    #if 0
    Debug::trace("UnitTest", debugHeader+"PrinterExecute code:"+Integer::toString(prExecCode));
    PrinterExecute prExec(this, FirstPrinter, prExecCode);
    dispatchEvent(&prExec);
    
    PrinterExecute prExec2(this, SecondPrinter, prExecCode);
    dispatchEvent(&prExec2);
    
    PrinterExecute prExec3(this, ThirdPrinter, prExecCode);
    dispatchEvent(&prExec3);
    #endif
    _executeTest = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::dispatchPrinterConfiguration() {
    //Set up a test configuration
    PrinterConfiguration prConfig(this, FirstPrinter);
    UINT8 deviceSign[6] = {'T','E','S','T','\0','\0'};
    prConfig.setDeviceSignature(&deviceSign[0]);
    prConfig.setLabelWidth(NarrowLabel);
    prConfig.setMediaSensorAdjustment(0xFF);
    prConfig.setOutOfMediaCount(0xAA);
    prConfig.setPrintContrast(0x07);
    prConfig.setConfigWord0(0x01);
    prConfig.setConfigWord1(0x02);
    prConfig.setConfigWord2(0x03);
    prConfig.setConfigWord3(0x04);
    prConfig.setConfigWord4(0x05);
    prConfig.setConfigWord5(0x06);
    prConfig.setConfigWord6(0x07);
    prConfig.setConfigWord7(0x08);
    prConfig.setConfigWord8(0x09);
    prConfig.setConfigWord9(0x0a);
    prConfig.setConfigWord10(0x0b);
    prConfig.setConfigWord11(0x0c);
    prConfig.setConfigWord12(0x0d);
    prConfig.setConfigWord13(0x0e);
    prConfig.setMediaSensorType(PrMediaSensorError);
    prConfig.setSLAEjectLine(0x0123);
    prConfig.setPrintPosition(0x3210);
    prConfig.setPrintPositionAbsolute(0x0000);
    
    dispatchEvent(&prConfig);
    
    prConfig.setPrinterType(SecondPrinter);
    dispatchEvent(&prConfig);
    
    prConfig.setPrinterType(ThirdPrinter);
    dispatchEvent(&prConfig);
    _executeTest = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::dispatchPrinterLabelTransfer() {
   #if 0
    PrinterLabelTransfer prLabelXfer(this, FirstPrinter);
    prLabelXfer.setSize(0x09876543);
    prLabelXfer.setApplySecurityLabel(false);
    dispatchEvent(&prLabelXfer);
    
    PrinterLabelTransfer prLabelXfer2(this, SecondPrinter);
    prLabelXfer2.setSize(0x01234567);
    prLabelXfer2.setApplySecurityLabel(false);
    dispatchEvent(&prLabelXfer2);
    
    PrinterLabelTransfer prLabelXfer3(this, ThirdPrinter);
    prLabelXfer3.setSize(0x0000FFFF);
    prLabelXfer3.setApplySecurityLabel(false);
    dispatchEvent(&prLabelXfer3);
    _executeTest = true;
    #endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::dispatchPrinterMask() {
    PrinterMask prMask(this, FirstPrinter);
    prMask.setSensorMask(0x01);
    prMask.setSensor2Mask(0x23);
    prMask.setUserMask(0x45);
    dispatchEvent(&prMask);
    
    PrinterMask prMask2(this, SecondPrinter);
    prMask2.setSensorMask(0x67);
    prMask2.setSensor2Mask(0x89);
    prMask2.setUserMask(0xab);
    dispatchEvent(&prMask2);
    
    PrinterMask prMask3(this, ThirdPrinter);
    prMask3.setSensorMask(0xcd);
    prMask3.setSensor2Mask(0xef);
    prMask3.setUserMask(0x01);
    dispatchEvent(&prMask3);
    _executeTest = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::dispatchPrinterSize() {
    PrinterNomSize prNomSize(this, FirstPrinter);
    prNomSize.setActualSize(0x0123);
    prNomSize.setMeasuredSize(0x4567);
    dispatchEvent(&prNomSize);
    
    PrinterNomSize prNomSize2(this, SecondPrinter);
    prNomSize2.setActualSize(0x89ab);
    prNomSize2.setMeasuredSize(0xcdef);
    dispatchEvent(&prNomSize2);
    
    PrinterNomSize prNomSize3(this, ThirdPrinter);
    prNomSize3.setActualSize(0x0123);
    prNomSize3.setMeasuredSize(0x4567);
    dispatchEvent(&prNomSize3);
    _executeTest = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::dispatchPrinterTest() {
    PrinterTest pt(this, FirstPrinter);
    pt.setData(0x0a);
    dispatchEvent(&pt);
    
    PrinterTest pt2(this, SecondPrinter);
    pt2.setData(0x0b);
    dispatchEvent(&pt2);
    
    PrinterTest pt3(this, ThirdPrinter);
    pt3.setData(0x0c);
    dispatchEvent(&pt3);
    _executeTest = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool UnitTest::testNGWPrinterMessaging( unsigned short test) {
    HobString debugHeader = HobString("testNGWPrinterMessage(): ");
    bool result = false;
    if(!_executeTest) {
        switch(test) {
            //Test outgoing messages
            case 0: {
                Debug::trace("UnitTest", debugHeader+"PrinterRequestWakeup");
                dispatchPrinterRequest(REQUEST_WAKEUP);
                break;
            }
            case 1: {
                Debug::trace("UnitTest", debugHeader+"PrinterRequestConfiguration");
                dispatchPrinterRequest(REQUEST_CONFIGURATION);
                break;
            }
            case 2: {
                Debug::trace("UnitTest", debugHeader+"PrinterRequestStatus");
                dispatchPrinterRequest(REQUEST_STATUS);
                break;
            }
            case 3: {
                Debug::trace("UnitTest", debugHeader+"PrinterRequestSensors");
                dispatchPrinterRequest(REQUEST_SENSORS);
                break;
            }
            case 4: {
                Debug::trace("UnitTest", debugHeader+"PrinterRequestVersion");
                dispatchPrinterRequest(REQUEST_VERSION);
                break;
            }
            case 5: {
                Debug::trace("UnitTest", debugHeader+"PrinterRequestError");
                dispatchPrinterRequest(REQUEST_ERROR);
                break;
            }
            case 6: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteEnablePrinter");
                dispatchPrinterExecute(EXECUTE_ENABLE_PRINTER);
                break;
            }
            case 7: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteDisablePrinter");
                dispatchPrinterExecute(EXECUTE_DISABLE_PRINTER);
                break;
            }
            case 8: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteResetPrinter");
                dispatchPrinterExecute(EXECUTE_RESET);
                break;
            }
            case 9: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecutePrintheadOn");
                dispatchPrinterExecute(EXECUTE_PRINTHEAD_ON);
                break;
            }
            case 10: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecutePrintheadOff");
                dispatchPrinterExecute(EXECUTE_PRINTHEAD_OFF);
                break;
            }
            case 11: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteEnableTakeup");
                dispatchPrinterExecute(EXECUTE_ENABLE_TAKEUP);
                break;
            }
            case 12: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteDisableTakeup");
                dispatchPrinterExecute(EXECUTE_DISABLE_TAKEUP);
                break;
            }
            case 13: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteCalibrateMediaSensor");
                dispatchPrinterExecute(EXECUTE_CALIBRATE_MEDIA_SENSOR);
                break;
            }
            case 14: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteCalibrateLabelTakenSensor");
                dispatchPrinterExecute(EXECUTE_CALIBRATE_LABEL_TAKEN_SENSOR);
                break;
            }
            case 15: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteEjectLabel");
                dispatchPrinterExecute(EXECUTE_EJECT_LABEL);
                break;
            }
            case 16: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteAdvanceLabel");
                dispatchPrinterExecute(EXECUTE_ADVANCE_LABEL);
                break;
            }
            case 17: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteSizeLabel");
                dispatchPrinterExecute(EXECUTE_SIZE_LABEL);
                break;
            }
            case 18: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecutePrintLabel");
                dispatchPrinterExecute(EXECUTE_PRINT_LABEL);
                break;
            }
            case 19: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteLoadTeachUnsizedLabel");
                dispatchPrinterExecute(EXECUTE_TEACH_UNSIZED_LABEL);
                break;
            }
            case 20: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteLoadTeachContinuousLabel");
                dispatchPrinterExecute(EXECUTE_TEACH_CONTINUOUS_LABEL);
                break;
            }
            case 21: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteLoadTeachContinuousStream");
                dispatchPrinterExecute(EXECUTE_TEACH_CONTINUOUS_STREAM_LABEL);
                break;
            }
            case 22: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteLoadTeachDieCutLabel");
                dispatchPrinterExecute(EXECUTE_TEACH_DIE_CUT_LABEL);
                break;
            }
            case 23: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteLoadTeachDieCutStream");
                dispatchPrinterExecute(EXECUTE_TEACH_DIE_CUT_STREAM_LABEL);
                break;
            }
            case 24: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteLoadTeachPrinterCalibration");
                dispatchPrinterExecute(EXECUTE_TEACH_PRINTER_CALIBRATION);
                break;
            }
            case 25: {
                Debug::trace("UnitTest", debugHeader+"PrinterExecuteSpeedKeyPrint");
                dispatchPrinterExecute(EXECUTE_TEACH_SPEED_KEY_PRINT);
                break;
            }
            case 26: {
                Debug::trace("UnitTest", debugHeader+"PrinterConfiguration");
                dispatchPrinterConfiguration();
                break;
            }
            case 27: {
                Debug::trace("UnitTest", debugHeader+"PrinterMask");
                dispatchPrinterMask();
                break;
            }
            case 28: {
                Debug::trace("UnitTest", debugHeader+"PrinterSize");
                dispatchPrinterSize();
                break;
            }
            case 29: {
                Debug::trace("UnitTest", debugHeader+"PrinterTest");
                dispatchPrinterTest();
                break;
            }
            case 30: {
                Debug::trace("UnitTest", debugHeader+"PrinterLabelTransfer");
                dispatchPrinterLabelTransfer();
                break;
            }
            default: {
                Debug::trace("UnitTest", debugHeader+"Test out of bounds");
                break;
            }
        }
    } else {
        Debug::trace("UnitTest", debugHeader+"Busy executing test at the moment, ignoring test:"+Integer::toString(test));
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool UnitTest::testPrinterMessaging( unsigned short test ) {
   
    bool result = false;
#if 0     
    if( !_executeTest ) {
        switch(test) {
            case 15: {
                Debug::trace( "UnitTest", "Test: PrinterRequestWakeup" );
                PrinterRequestWakeup pw(this);
                pw.setPrinterType(FirstPrinter);
                dispatchEvent( &pw );

                PrinterRequestWakeup pw1(this, SecondPrinter);
                dispatchEvent( &pw1 );

                PrinterRequestWakeup pw2(this, ThirdPrinter);
                dispatchEvent( &pw2 );

                _executeTest = true;
                break;
            }
            case 16: {
                Debug::trace( "UnitTest", "Test: PrinterRequestStatus" );
                PrinterRequestStatus ps(this);
                ps.setPrinterType(FirstPrinter);
                dispatchEvent( &ps );

                ps.setPrinterType(SecondPrinter);
                dispatchEvent( &ps );

                ps.setPrinterType(ThirdPrinter);
                dispatchEvent( &ps );

                _executeTest = true;
                break;
            }
            case 17: {
                Debug::trace( "UnitTest", "Test: PrinterRequestPrintheadPower" );
                PrinterRequestPrintheadPower pp(this);
                pp.setPrinterType(FirstPrinter);
                pp.setPowerState();
                pp.setShortCircuitProtection();
                dispatchEvent(&pp);

                pp.setPrinterType(SecondPrinter);
                dispatchEvent(&pp);

                pp.setPrinterType(ThirdPrinter);
                dispatchEvent(&pp);

                _executeTest = true;
                break;
            }
            case 18: {
                Debug::trace( "UnitTest", "Test: PrinterRequestSensors" );
                PrinterRequestSensors ps(this);
                ps.setPrinterType(FirstPrinter);
                dispatchEvent(&ps);

                ps.setPrinterType(SecondPrinter);
                dispatchEvent(&ps);

                ps.setPrinterType(ThirdPrinter);
                dispatchEvent(&ps);

                _executeTest = true;
                break;
            }
            case 19: {
                Debug::trace( "UnitTest", "Test: PrinterRequestInquire" );
                PrinterRequestInquire pi(this);
                pi.setPrinterType(FirstPrinter);
                dispatchEvent(&pi);

                pi.setPrinterType(SecondPrinter);
                dispatchEvent(&pi);

                pi.setPrinterType(ThirdPrinter);
                dispatchEvent(&pi);

                _executeTest = true;
                break;
            }
            case 20: {
                Debug::trace( "UnitTest", "Test: PrinterFlush" );
                PrinterFlush pf(this);
                pf.setPrinterType(FirstPrinter);
                dispatchEvent(&pf);

                pf.setPrinterType(SecondPrinter);
                dispatchEvent(&pf);

                pf.setPrinterType(ThirdPrinter);
                dispatchEvent(&pf);

                _executeTest = true;
                break;
            }
            case 21: {
                Debug::trace( "UnitTest", "Test: PrinterEnable" );
                PrinterEnable pe(this);
                pe.setPrinterType(FirstPrinter);
                dispatchEvent(&pe);

                pe.setPrinterType(SecondPrinter);
                dispatchEvent(&pe);

                pe.setPrinterType(ThirdPrinter);
                dispatchEvent(&pe);

                _executeTest = true;
                break;
            }
            case 22: {
                Debug::trace( "UnitTest", "Test: PrinterDisable" );
                PrinterDisable pd(this);
                pd.setPrinterType(FirstPrinter);
                dispatchEvent(&pd);

                pd.setPrinterType(SecondPrinter);
                dispatchEvent(&pd);

                pd.setPrinterType(ThirdPrinter);
                dispatchEvent(&pd);

                _executeTest = true;
                break;
            }
            case 23: {
                Debug::trace( "UnitTest", "Test: PrinterTest" );
                PrinterTest pt(this);
                pt.setData(0xaa);
                pt.setPrinterType(FirstPrinter);
                dispatchEvent(&pt);

                pt.setPrinterType(SecondPrinter);
                dispatchEvent(&pt);

                pt.setPrinterType(ThirdPrinter);
                dispatchEvent(&pt);

                _executeTest = true;
                break;
            }
            case 24: {
                Debug::trace( "UnitTest", "Test: PrinterEnableTakeup" );
                PrinterEnableTakeup pe(this);
                pe.setPrinterType(FirstPrinter);
                dispatchEvent(&pe);

                pe.setPrinterType(SecondPrinter);
                dispatchEvent(&pe);

                pe.setPrinterType(ThirdPrinter);
                dispatchEvent(&pe);

                _executeTest = true;
                break;
            }
            case 25: {
                Debug::trace( "UnitTest", "Test: PrinterRequestCalibrate" );
                PrinterRequestCalibrate pc(this);
                pc.setPrinterType(FirstPrinter);
                pc.setCalibrationTypeMediaSensor();
                dispatchEvent(&pc);

                pc.setPrinterType(SecondPrinter);
                dispatchEvent(&pc);

                pc.setPrinterType(ThirdPrinter);
                dispatchEvent(&pc);

                _executeTest = true;
                break;
            }
            case 26: {
                Debug::trace( "UnitTest", "Test: PrinterRequestLabelTransfer" );
                PrinterRequestLabelTransfer pl(this);
                pl.setActualDataLength(0x1234);
                pl.setTotalDataLength(0x1234);
                pl.setPrinterType(FirstPrinter);
                dispatchEvent(&pl);

                pl.setPrinterType(SecondPrinter);
                dispatchEvent(&pl);

                pl.setPrinterType(ThirdPrinter);
                dispatchEvent(&pl);

                _executeTest = true;
                break;
            }
            case 27: {
                Debug::trace( "UnitTest", "Test: PrinterConfiguration" );
                PrinterConfiguration pc(this, FirstPrinter);
                dispatchEvent(&pc);

                PrinterConfiguration pc1(this, SecondPrinter);
                dispatchEvent(&pc1);

                PrinterConfiguration pc2(this, ThirdPrinter);
                dispatchEvent(&pc2);

                _executeTest = true;
                break;
            }
            case 28: {
                Debug::trace( "UnitTest", "Test: PrinterNomSize" );
                PrinterNomSize pn(this, FirstPrinter);
                pn.setActualSize(0x123);
                pn.setMeasuredSize(0x456);

                dispatchEvent(&pn);

                PrinterNomSize pn1(this, SecondPrinter);
                pn1.setActualSize(0x789);
                pn1.setMeasuredSize(0x123);

                dispatchEvent(&pn1);

                PrinterNomSize pn2(this, ThirdPrinter);
                pn2.setActualSize(0x456);
                pn2.setMeasuredSize(0x789);

                dispatchEvent(&pn2);
                _executeTest = true;
                _weigherTestComplete = true;
                _printerTestComplete = true;
                _wrapperTestComplete = false;
                break;
            }
            default: {
                _executeTest = true;
                result = true;
                break;
            }
        }
    }
#endif    
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool UnitTest::testWrapperMessaging( unsigned short test ) {
    bool result = false;
    if( !_executeTest ) {
        switch(test) {
            case 1: {
                NGWLabelSetup ls(this);
                ls.setStationId(_PrimaryStation);
                ls.setLabelSize(400);
                ls.setLabelRotation(90);
                ls.setLabelLocation(20);
                dispatchEvent(&ls);
                _executeTest = true;
                break;
            }
            case 2: {
                WrapperRequestWakeup wrw(this);
                dispatchEvent(&wrw);
                _executeTest = true;
                break;
            }
            case 31: {
                WrapperRequestStatus wrs(this);
                dispatchEvent(&wrs);
                _executeTest = true;
                break;
            }
            case 3: {
                EvolWrapperInquire wi(this);
                wi.setConfigStatus(WrapperConfigError);
                dispatchEvent(&wi);
                _executeTest = true;
                break;
            }
            case 4: {
                EvolWrapperCycle wc(this);
                wc.setPullFilm();
                dispatchEvent(&wc);
                _executeTest = true;
                break;
            }
            case 5: {
                WrapperRequestSensors wrs(this);
                dispatchEvent(&wrs);
                _executeTest = true;
                break;
            }
            case 6: {
                EvolWrapperReqSystemInfo wsi(this);
                dispatchEvent(&wsi);
                _executeTest = true;
                break;
            }
            case 7: {
                EvolWrapperSystemInfoErase sie(this);
                dispatchEvent(&sie);
                _executeTest = true;
                break;
            }
            case 8: {
                EvolWrapperSystemInfoWrite siw(this);
                siw.setAwsCycleCount(0x12345678);
                std::string serial = "45-1112-726";
                siw.setAwsSerialNumber(serial);
                std::string date = "06272015";
                siw.setAwsDateOfManufacture(date);
                std::string install = "09012015";
                siw.setAwsDateOfInstall(install);
                siw.setModelNumber(EvolWrapperSystemInfo::AWS2RL);
                dispatchEvent(&siw);
                _executeTest = true;
                break;
            }
            case 9: {
                EvolWrapperReqSystemReset rsr(this);
                dispatchEvent(&rsr);
                _executeTest = true;
                break;
            }
            case 10: {
                EvolWrapperLoad wl(this, LowerRoll);
                dispatchEvent(&wl);
                _executeTest = true;
                break;
            }
            case 11: {
                EvolWrapperRequestPackageTimings rpt(EvolWrapperRequestPackageTimings::CurrentTimings, this);
                dispatchEvent(&rpt);
                _executeTest = true;
                break;
            }
            case 12: {
                EvolWrapperControl wc(this);
                wc.setSealerBeltTempSetting(400);
                wc.setSealerBeltHeaterEnabled(true);
                wc.setFilmWidthSelection(UpperRoll);
                wc.setFilmTensionFrontToBack(50);
                wc.setFilmTensionRightToLeft(100);
                wc.setIntakeMode(WrapperIntakeMode::NormalIntake);
                dispatchEvent(&wc);
                _executeTest = true;
                break;
            }
            case 13: {
                EvolWrapperWrap ww(this, WrapperIntakeMode::FastIntake, 15, true, 150, 3223);
                dispatchEvent(&ww);
                _executeTest = true;
                break;
            }
            case 14: {
                WrapperMode wm(this, WrServiceMode);
                dispatchEvent(&wm);
                _executeTest = true;
                break;
            }
            case 15: {
                EvolWrapperTest wt(this);
                wt.setTestCode(EvolWrapperTest::KnifeTest);
                wt.setDataInt1(0);
                wt.setDataInt2(0);
                wt.setDataLong(0);
                dispatchEvent(&wt);
                _executeTest = true;
                break;
            }
            case 16: {
                EvolWrapperConfiguration wc(this);
                EvolWrapperHeaterConfig HtCfg = wc.getHeaterConfig();
                HtCfg.setFilmHeaterIsUsed(true);
                HtCfg.setFilmHeaterMaxTemp(150);
                HtCfg.setFilmHeaterMinTemp(125);
                HtCfg.setFilmHeaterDefaultTemp(150);
                HtCfg.setChecksum(1234);
                HtCfg.setSealerHeaterIsUsed(true);
                HtCfg.setSealerHeaterMaxTemp(350);
                HtCfg.setSealerHeaterMinTemp(325);
                HtCfg.setSealerHeaterDefaultTemp(350);
                wc.setHeaterConfig(HtCfg);
                EvolWrapperGeneralConfig genCfg = wc.getWrapperConfig();

                WrapperFilmWidthDimensions dimensions = genCfg.getTopFilmRollDimensions();
                dimensions.setRollDiameter(20);
                dimensions.setRollWidth(15);
                dimensions.setRemainingOnRoll(9);
                WrapperFilmWidthDimensions dimension = genCfg.getBottomFilmRollDimensions();
                dimension.setRollWidth(18);
                dimension.setRollDiameter(25);
                dimension.setRemainingOnRoll(7);
                genCfg.setWrapperOperation(AutomaticOperation);
                genCfg.setHardwareAddOn(SecondPrinterApplier);
                genCfg.setFilmTensionFrontToBack(5);
                genCfg.setFilmTensionRightToLeft(20);
                genCfg.setPackageGrandTotal(11525);
                genCfg.setWrapperCycleTotal(200000);
                genCfg.setChecksum(1234);
                wc.setWrapperGenConfig(genCfg);
                EvolLabelApplierConfiguration applierCfg = wc.getLabelApplierConfig();
                applierCfg.setHasAutoRotation(true);
                applierCfg.setPriLabelSizeDefault(4);
                applierCfg.setPriLabelWidth(WideLabel);
                applierCfg.setPriLabelRotationDefault(Rotate90);
                applierCfg.setSecLabelLocationDefault(LocRight);
                applierCfg.setPriLabelOffsetOnPkg(10);
                applierCfg.setPriRotationCalibrationOffset(1);

                applierCfg.setSecLabelSizeDefault(2);
                applierCfg.setSecLabelWidth(WideLabel);
                applierCfg.setSecLabelRotationDefault(Rotate315);
                applierCfg.setSecLabelLocationDefault(LocLeft);
                applierCfg.setSecLabelOffsetOnPkg(10);
                applierCfg.setSecRotationCalibrationOffset(1);
                applierCfg.setIsSecondCLAAttached(true);
                applierCfg.setSideDischargeType(LeftDischarge);
                wc.setLabelConfig(applierCfg);
                dispatchEvent(&wc);
                _executeTest = true;
                break;
            }
            case 17: {
                EvolWrapperSaveCycleCount scc(this, 0x12345678);
                dispatchEvent(&scc);
                _executeTest = true;
                break;
            }
            case 18: {
                EvolWrapperReqCycleCount rcc(this);
                dispatchEvent(&rcc);
                _executeTest = true;
                break;
            }
            case 19: {
                NGWLabelReady lr(this, 1, _PrimaryStation);
                dispatchEvent(&lr);
                _executeTest = true;
                break;
            }
            case 20: {
                NGWLabelTaken lt(this, 1, _SecondaryStation);
                dispatchEvent(&lt);
                _executeTest = true;
                break;
            }
            default: {
                _wrapperTestComplete = true;
                result = true;
                break;
            }
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool UnitTest::testNGWUpgradeMessaging(unsigned short test) {

	bool result = false;
	if( !_executeTest ) {
		switch(test) {
			case 1: {
				/* tell the timer to wait until we get the response. */
				_wait = true;
				if(!_executeTest) {
					Debug::trace( "UnitTest", "_UpgradeRequired: " + Integer::toString(test));
					NGWGeneric genMsg(_UpgradeRequired, this);
					dispatchEvent(&genMsg);
					_executeTest = true;
				}
				break;
			}
			case 2: {
				Debug::trace( "UnitTest", "notify the UI of the start of upgrade for nodes: " + Integer::toString(test));
				NGWStartUpgrade msg(this, 1);
				msg.setSourceObject(this);
				dispatchEvent(&msg);
				_executeTest = true;
				break;

			}
			case 3: {
				Debug::trace( "UnitTest", "notify the UI of the version for node 1: " + Integer::toString(test));
				std::string s = "1.5.0";
				/* notify the UI of the version for node 1. */
				NGWUpgradeStatus msg(this, 1 );
				msg.setStateVersion();
				msg.setVersion(s);
				Debug::trace( "UnitTest", "version" + msg.getVersion());
				//msg.setError(NGWUpgradeStatus::_None);
				dispatchEvent(&msg);
				_executeTest = true;
				break;
			}
			case 4: {
				/* notify the UI of the version for node 1.
				NGWUpgradeStatus msg(this, 1 );
				msg.setStateUpgrade();
				msg.setError(NGWUpgradeStatus::_None);
				dispatchEvent(&msg);*/
				_executeTest = true;
				break;
			}
			case 5: {
				/* inform the UI that node 1 is 0% complete download started.  */
				Debug::trace( "UnitTest", "nform the UI that node 1 is 0% complete: " + Integer::toString(test));
				NGWUpgradeProgress msg(this, 1, 0);
				msg.setSourceObject(this);
				dispatchEvent(&msg);
				_executeTest = true;
				break;
			}
			case 6: {
				/* inform the UI that node 1 is 25% complete download started.  */
				Debug::trace( "UnitTest", "inform the UI that node 1 is 25% complete: " + Integer::toString(test));
				NGWUpgradeProgress msg(this, 1, 25);
				msg.setSourceObject(this);
				dispatchEvent(&msg);
				_executeTest = true;
				break;
			}
			case 7: {
				/* inform the UI that node 1 is 50% complete download started.  */
				Debug::trace( "UnitTest", "inform the UI that node 1 is 50% complete: " + Integer::toString(test));
				NGWUpgradeProgress msg(this, 1, 50);
				msg.setSourceObject(this);
				dispatchEvent(&msg);
				_executeTest = true;
				break;
			}
			case 8: {
				/* inform the UI that node 1 is 75% complete download started.  */
				Debug::trace( "UnitTest", "inform the UI that node 1 is 75% complete: " + Integer::toString(test));
				NGWUpgradeProgress msg(this, 1, 75);
				msg.setSourceObject(this);
				dispatchEvent(&msg);
				_executeTest = true;
				break;
			}
			case 9: {
				/* inform the UI that node 1 is 100% complete download started.  */
				Debug::trace( "UnitTest", "inform the UI that node 1 is 100% complete: " + Integer::toString(test));
				NGWUpgradeProgress msg(this, 1, 100);
				msg.setSourceObject(this);
				dispatchEvent(&msg);
				_executeTest = true;
				break;
			}
			case 10: {
				/* inform the UI that we are skipping node 2 upgrade (100% complete). */
				Debug::trace( "UnitTest", "inform the UI that we are skipping node 2 upgrade: " + Integer::toString(test));
				NGWUpgradeProgress msg(this, 2);
				msg.setPercentComplete(100);
				msg.setSourceObject(this);
				dispatchEvent(&msg);
				_executeTest = true;
				break;
			}
			case 11: {
				/* tell the UI that we are finished upgrading the wrapper */
				Debug::trace( "UnitTest", "tell the UI that we are finished upgrading the wrapper: " + Integer::toString(test));
				NGWUpgradeFinished msg(this);
				msg.setSourceObject(this);
			    dispatchEvent(&msg);
			    _executeTest = true;
			    _upgradeTestComplete = true;
			    break;
			}
		}
	}
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool UnitTest::testAWS3Messaging(unsigned short test){
	bool result = false;
	if( !_executeTest ) {
		switch(test) {
			case 1:
			case 2: {
				Debug::trace( "UnitTest", "testAWS3Messaging() EvolWrapperWrap: " + Integer::toString(test));
                EvolWrapperWrap ww(this, WrapperIntakeMode::FastIntake, 15, true, 1500, 3223);
                dispatchEvent(&ww);
			    _executeTest = true;
				break;
			}
			case 3: {
				Debug::trace( "UnitTest", "testAWS3Messaging() LabelApplierLabelReady: " + Integer::toString(test));
				LabelApplierLabelReady ready(this);
				ready.setStationId(PrimaryPrinterStation);
				ready.setPackageId(125);
				ready.setSourceObject(this);
			    dispatchEvent(&ready);
			    _executeTest = true;
				break;
			}
			case 4: {
				Debug::trace( "UnitTest", "testAWS3Messaging() LabelApplierLabelTaken: " + Integer::toString(test));
				LabelApplierLabelTaken taken(this);
				taken.setStationId(PrimaryPrinterStation);
				taken.setPackageId(125);
				taken.setSourceObject(this);
			    dispatchEvent(&taken);
			    _executeTest = true;
				break;
			}
			case 5: {
				Debug::trace( "UnitTest", "testAWS3Messaging() LabelApplierSetup: " + Integer::toString(test));
				LabelApplierSetup setup(this);
				setup.setPrimaryLabelSize(400);
				setup.setPrimaryLabelRotation(Rotate270);
				setup.setPrimaryLabelPlacement(LocLeft);
				setup.setSecondaryLabelSize(550);
				setup.setSecondaryLabelRotation(Rotate180);
				setup.setSecondaryLabelPlacement(LocRight);
				setup.setSourceObject(this);
			    dispatchEvent(&setup);
			    _executeTest = true;
				break;
			}
			case 6: {
				Debug::trace( "UnitTest", "testAWS3Messaging() WrapperMode: " + Integer::toString(test));
                WrapperMode wm(this, WrServiceMode);
                dispatchEvent(&wm);
                _executeTest = true;
				break;
			}
			case 7: {
				Debug::trace( "UnitTest", "testAWS3Messaging() EvolWrapperControl: " + Integer::toString(test));
                EvolWrapperControl wc(this);
                wc.setSealerBeltTempSetting(400);
                wc.setSealerBeltHeaterEnabled(true);
                wc.setFilmWidthSelection(UpperRoll);
                wc.setFilmTensionFrontToBack(50);
                wc.setFilmTensionRightToLeft(100);
                wc.setIntakeMode(WrapperIntakeMode::NormalIntake);
                dispatchEvent(&wc);
                _executeTest = true;
                _aws3TestComplete = true;
				break;
			}
		}
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD UnitTest::Run() {
	Debug::trace( "UnitTest", "Testing: " + Integer::toString(_testType));

	while( !m_ExitEvent.IsEventSet() ) {

    	switch( _testType ) {

			case _UpgradeUI_Messaging: {
				if( !_upgradeTestComplete )
					testNGWUpgradeMessaging(_testCount);
				if( _testCount == _UITestNGWWrapper ) {
					_testType = _UnitTestEnd;
				}
				break;
			}
			case _Printer__Messaging: {
		        if( !_printerTestComplete )
		        	testNGWPrinterMessaging( _testCount );
		        if( _testCount == _UITestNGWPrinterEnd ) {
		        	_testType = _UnitTestEnd;
		        }
				break;
			}
			case _Weigher__Messaging: {
				if( !_weigherTestComplete )
		            testWeigherMessaging( _testCount );
				if( _testCount == _UITestNGWWeigher ) {
					_testType = _UnitTestEnd;
				}
				break;
			}
			case _NGWrapper_Messaging: {
		        if( !_wrapperTestComplete )
		            testWrapperMessaging( _testCount );
		        if( _testCount == _UITestWrapperEnd ) {
		        	_testType = _UnitTestEnd;
		        }
				break;
			}
			case _AllWrapperPeriph_Messaging: {
				break;
			}
			case _AWS3Messaging: {
				if( !_aws3TestComplete )
					testAWS3Messaging(_testCount );
				if( _testCount == _AWS3TestEnd )
					_testType = _UnitTestEnd;
				break;
			}
			case _UnitTestEnd:
			{
				Debug::trace( "UnitTest",  "Test Complete." );
			    m_ExitEvent.EventSet();
				break;
			}

    	}
        PAL_absclass::PAL_sleep(1);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ) {

	Debug::trace( "UnitTest","processEvent() Device: "  + Integer::toString(msg->getDeviceType()));
    if( msg.get() != NULL ) {
        switch (msg->getDeviceType()) {
             case NGWrapper: {
            	switch((NGWEvents) msg->getEventType()) {
            		case _UpgradeUIReady: {
            			_wait = false;
            			break;
            		}
                    default:
                    	break;
            	}
            	break;
             }
            case Wrapper:{
                handleWrappermessage( (WrEvent) msg->getEventType(), msg->Data() );
                break;
            }
			default:
				break;
        }
    } else {
        Debug::trace( "UnitTest.error", "Received NULL message" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::handleWrappermessage( WrEvent wrEvent, const char *recvBuffer )
{
    static int cntr_ = 0;
    static bool _once = false;
    switch( wrEvent )
    {
		case WrPPPWakeup: 
		{
			Debug::trace("UnitTest.PPPMsg", "WrPPPWakeup");
			PPPWakeup wake( recvBuffer );
			wake.show();

            PPPRequest req( this, PPPRequest::PPPReq::_Set_Rate, 300 );	
            dispatchEvent( &req );		
			break;
		}
		case WrPPPConfig: 
		{
			Debug::trace("UnitTest.PPPMsg", "WrPPPConfig");
			PPPConfig pppConfig( recvBuffer );
			pppConfig.show();
			//dispatchEvent( &pppConfig );
            break;
		}
		case WrPPPDist: 
		{
			Debug::trace("UnitTest.PPPMsg", "WrPPPDist");
			PPPDist dist( recvBuffer );

            //PPPRequest req( this, PPPRequest::PPPReq::_Config, 0 );	
            //dispatchEvent( &req );
            	
			dist.show();
            cntr_++;
            if( cntr_ >= 20 ) {
                cntr_ = 0;
                if( !_once ) {
                    _once = true;
                    //PPPRequest req( this, PPPRequest::PPPReq::_Cal, 0 );	
                    //dispatchEvent( &req );
                }
            }

			break;
		}
        case WrPPPRequest: 
        {
            Debug::trace("UnitTest.PPPMsg", "WrPPPRequest");
            PPPRequest req( recvBuffer );	
            req.show();
            //dispatchEvent( &req );
            break;
        } 
		case WrPPPCalibration: 
		{
			Debug::trace("UnitTest.PPPMsg", "WrPPPCalibration");
            PPPCalib cal( recvBuffer );
            cal.show();
            //dispatchEvent( &cal );
			break;
		}
		case WrPPPVersion: 
		{
			Debug::trace("UnitTest.PPPMsg", "WrPPPVersion");
            PPPVersion ver( recvBuffer );
            ver.show();
			break;
		}
        case WrPPPStatus:
        {
			Debug::trace("UnitTest.PPPMsg", "WrPPPStatus");
            PPPStatus status( recvBuffer );
            status.show();
            break;
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UnitTest::timeoutProcess( __int32 _id ) {
    Debug::trace( "UnitTest", "timeoutProcess(): " + Integer::toString(_testCount) );
    if( _id == _MessageTimer ) {

        if( !_wait ) {
        	_executeTest = false;
        	_testCount++;
        }
    }
}
