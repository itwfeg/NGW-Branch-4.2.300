#include <logmanager.h>

#include "manufacturingtest.h"
#include "ui_manufacturingtest.h"

#include <qtimer.h>
#include <QProcess>
#include <QComboBox>
#include <QMessageBox>
#include <QLabel>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include "filesystem.h"
#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"
#include "Devices.h"
#include "Events.h"
#include "Messages.h"
#include "process.h"
#include "python.h"
#include <QPalette>
#include <UiUtilities.h>
#include <QTimer>

#include <QtConcurrentRun>

#include <bitset>

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "plumainscreen.h"

#include "../dialogs/messageboxdialog.h"

//For QR Code creation � SOF-4399
#include <QTemporaryFile>
#include "../../sessionHandlerUtil/common/bitmap.h"
#include "../../sessionHandlerUtil/qrcode/qrapi.h"
#include "../../sessionHandlerUtil/qrcode/qrecclevel.h"

// FT-1223
// Hobart
#include "../../source/HFC/CfgBaseFieldOffsets.h"
#include "backgroundwidgetstyles.h"

// SOF-5997 - these values provided by Aaron
#define SHOOT_THROUGH_MAXVALUE 200
#define SHOOT_THROUGH_MINVALUE 40

const bool convertUppercase_ = true;

//----------------------------------------------------
// USE FOR DEBUGGING ONLY TO SPEED UP THE PROCESS
const bool DBG_AUTO_COMPLETE_ALL_TABS = false;
const bool DBG_AUTO_COMPLETE_WEIGHER_TAB = false;
const bool DBG_AUTO_COMPLETE_NETWORK_TAB = false;
const bool DBG_SKIP_FACTORY_DEFAULTS = false;
//----------------------------------------------------

manufacturingTest::manufacturingTest(SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent),
    ui(new Ui::manufacturingTest),
    player( NULL )
{
    //LOG_CLASS_INFO( "manufacturingTest", "manufacturingTest" );

    ui->setupUi(this);

    // HT-177
    exitingScreen = false;

    lastReadCount = 0;

    countsTestPassed = false;

    // FT-1222
    scanProcess = new QProcess( this );

    // FT-1335
    discoverBluetoothProcess = new QProcess( this );

    setDateTimeSuccessful = true;

    initialize();

    scaleScreen();
    expandTabs(ui->testTabsWidget);

    // FT-1335
    /* See runProcesses
     * if( !isArkBoardSite ) // SOF-3449
        startBluetoothDiscovery();
    */
    // HT-277
    if ( mainWindow_->devicesAttached( HIDDevBarcodeScannerAttached ) )
        ui->usbBarcodeScannerRecognizedAnswerLabel->setText("Yes");

    if ( mainWindow_->devicesAttached( HIDDevKeyboardAttached ) )
        ui->usbBarcodeScannerRecognizedAnswerLabel->setText("Yes");

    if ( mainWindow_->devicesAttached( HIDDevMouseAttached ) )
        ui->usbMouseRecognizedAnswerLabel->setText("Yes");

    // SOF-5596 - we want to start the printer/weigher reset asap - make sure menu panel is hidden first
    connect(mainWindow_, SIGNAL(hidingMenuPanel()), this, SLOT(onScreenFullyChanged()));

}

manufacturingTest::~manufacturingTest()
{
    // HT-177
    exitingScreen = true;

    if ( weigherCalibrate_ )
        delete weigherCalibrate_;

    if ( scanProcess )
    {
        if ( QProcess::Running == scanProcess->state() )
            scanProcess->kill();

        delete scanProcess;
    }

    // FT-1335
    if ( discoverBluetoothProcess )
    {
        if ( QProcess::Running == discoverBluetoothProcess->state() )
            discoverBluetoothProcess->kill();

        delete discoverBluetoothProcess;
    }

    // update the data used from the python scripts
    session_->scaleConfig->updateScriptData(1);

    delete ui;
}

void manufacturingTest::initialize()
{
    LOG_CLASS_INFO( "manufacturingTest", "initialize" );

    LOG_DEBUG( "Top of initialize() - background tasks...", NULL );

    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    // SOF-5996: Already have these values, so initialize here
    populateBoardType();
    populateSdCardSize();
    //

    /*
     * See runProcesses()

    // Set backlight of both displays to max
    QFuture<void> future_setMaxBacklight =
        QtConcurrent::run( this, &manufacturingTest::setMaxBacklight );

    // Get the Scale Model and set the Style on the Flash tab
    QFuture<QString> future_getScaleModel =
        QtConcurrent::run( this, &manufacturingTest::getScaleModel );

    // Get the rest of the Flash info and populate the Flash tab
    QFuture<void> future_getFlashInfo =
        QtConcurrent::run( this, &manufacturingTest::getFlashInfo );

    // Detect if module installed.
    QFuture<bool> future_isWifiModulePresent =
        QtConcurrent::run( this, &manufacturingTest::isWifiModulePresent );

    // Populate the Wired / Wireless MAC addresses
    QFuture<void> future_getMACAddress =
        QtConcurrent::run( this, &manufacturingTest::getMACAddress );

    // Populate the Gateway and DHCP IP address used for
    // Service Diagnostic mode on the Network tab
    QFuture<void> future_getNetworkInfo =
        QtConcurrent::run( this, &manufacturingTest::getNetworkInfo );

    //  FT-1626
    //  Get the scale's host name as part of the data capture
    QFuture<void> future_getHostname =
        QtConcurrent::run( this, &manufacturingTest::getHostname );

    */
    LOG_DEBUG( "initializing...", NULL );

    weigherCalibrate_ = NULL;

    //SOF-3752
    //Atmel peripheral Bd. 00-447780
    isPeripheralBoardTestMode = false;


    // SOF-3449
    isArkBoardSite = false;
    arkSerialFlash_ = 0;
    arkSDSlot0_ = 0;
    arkSDSlot1_ = 0;
    arkRTC_ = 0;
    arkUSB_ = 0;
    arkWifi_ = 0;
    arkBlueTooth_ = 0;
    arkEthernet_ = 0;
    arkMemory_ = 0;
    arkCan0_ = 0;
    kpcCan_ = 0;
    kpcModel_ = 0;
    kpcSerialFlash_ = 0;
    kpcValueMax_ = 0;


    // SOF-5595
    interlockEngaged_ = true;
    hasBeenDisengaged_ = false;
    swapToLinerlessDialog = NULL;
    swapLabelStockRoutineStarted_ = false;
    trackLabelStockSensorValue_ = true;
    trackStockMetrics_ = false;

    /*
     * SOF-5996: see runProcesses()
    if( isLinux() )
        populateBoardType();
    */

    // SOF-3244
    const DomWeigherConfigRead *weigherFields = &session_->scaleConfig->weigherConfigRead;
    int wayer_model = weigherFields->weigherModel();
    isHtmWeigher = ( SessionMainWindow::HTM == wayer_model ) ? true : false;

    gravity_ = 0;
    m_LbsCountsConvFactor_ = 0;
    zeroValue_ = 0;
    spanValue_ = 0;
    z0ElectronicOffset_ = 0;
    deadWeight_ = 0;
    x2_ = 0;
    x1_ = 0;
    x0_ = 0;
    y2_ = 0;
    y1_ = 0;
    y0_ = 0;
    xRot_0_ = 0;
    xRot_1_ = 0;
    xRot_2_ = 0;
    yRot_0_ = 0;
    yRot_1_ = 0;
    yRot_2_ = 0;
    zRot_0_ = 0;
    zRot_1_ = 0;
    zRot_2_ = 0;
    xGain_ = 0;
    yGain_ = 0;
    gCoeff_ = 0;
    checkSum_ = 0;

    // SOF-3294
    dotWearTimer = new QTimer(this);
    dotWearTimer->setSingleShot(true);
    dotWearTimer->setInterval(30000); //30 sec timeout
    dotWearTestPassed = false;
    haveDotWearTestData = false;
    haveHeadDotWearDetailData = false;
    haveBadDot = false;

    codeCheckerEnabled_ = scaleConfig->scaleConfig.isCodeCheckerEnabled();

    gcdValue_ = 0;

    // SOF-3555: HTi with Cutter (ARK board); but no Code Checker
    MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );
    const QString scaleModel( settings->scaleName() );

    if( scaleModel.length() >= 3 && !( scaleModel.toUpper().startsWith( "HTM") ) )
    {
        codeCheckerEnabled_ = false;
    }

    requestPrinterDotStatusAttempted = false;

    connect( dotWearTimer, SIGNAL( timeout() ), this, SLOT( onDotWearTimerTimeout() ) );

    // TODO:
    // Needs set to NTP server hostname or IP address
    // If Hillsboro can ping engdev1 we should just use
    // our NTP server
    //ntpServer = QString("pool.ntp.org");
    ui->lblWireNetworkResponseWait->hide();
    ui->lblWifiNetworkResponseWait->hide();
    ui->programFlashButton->setFixedSize(112,40);
    ui->startDisplayTestButton->setFixedHeight(40);

    ui->startBeeperTestButton->setFixedHeight(40);

    connect( ui->btnSizeLabels, SIGNAL(clicked()), this, SLOT(onResizeLabelsClicked()));

    if( isSSKPrinterAttached() )
    {
        ui->lblMediaSensorDisplay->setVisible( true );
        ui->btnSizeLabels->setVisible( false ); // SOF-5321 Hide per Jim's request
        ui->btnSizeLabels->setEnabled( false );
        ui->startLabelTakenSensorTestButton->setEnabled( false );

        //--------------------------------------
        // Printer status request to check low stock label
        //--------------------------------------
        session_->send(Printer, PrReqStatus);       // primary printer = 0
    }
    else
    {
        // Hide fields that are specific for Self Standing Self-Serve
        ui->lblMediaSensorDisplay->setVisible(false);
        ui->btnSizeLabels->setVisible( false );
        ui->btnSizeLabels->setEnabled( false );
        ui->lblStatusValue->setVisible( false );
        ui->lblLowStockLabel->setVisible( false );
        ui->lblLowStockData->setVisible( false );
        ui->lblHeadUpLabel->setVisible( false );
        ui->lblHeadUpData->setVisible( false );
    }

    ui->lblSelfServiceScannerPrompt->setVisible(false);
//    ui->rbSelfServiceScannerPromptAnswerNo->setVisible(false);
//    ui->rbSelfServiceScannerPromptAnswerYes->setVisible(false);
    ui->gbSelfServiceScannerPromptAnswers->setVisible(false);

    QString strModelName = session_->scaleConfig->initialScaleModelName;;

    LOG_DEBUG( "2 strModelName=%s", strModelName.toLatin1().constData() );

    /* SOF-5996: see runProcesses()
    if( isLinux() )
    {
        populateSdCardSize();
    }
    else
    {
        ui->gbSysInfo->hide();
    }
    */

    // FT-1270
    usbTabPassed = false;

    // Remove for now ... we will show it if we get a message telling us Cutter is attached
    removeCutterTab();

    cutterEnabled_ = scaleConfig->scaleConfig.isCutterEnabled();
    cutterEnabledOriginal_ = cutterEnabled_;
    cutterAttached_ = false;
    cutter_fired_ = false;
    cutter_state_ = CutterNone;

    // since cutter is initially not attached - disable actions
    ui->lblIsCutterAttachedResult->setText(tr("No"));
    ui->lblCutterStatus->setEnabled(false);
    ui->lblCutterStatusResult->setText(tr("Disabled"));
    ui->lblCutterStatusResult->setEnabled(false);
    ui->butEnableCutter->setText(tr("Disable"));
    ui->butEnableCutter->setEnabled(false);
    ui->butEnableCutter->setVisible(false); //SOF-5175

    ui->butEnableCutter->setText(tr("Enable"));

    if ( ( strModelName.length() >= 3 && strModelName.left( 3 ) == "HTs" ) /* ||
         ( isSelfServiceScale( strModelName ) ) */ )
    {
        // If we have an HTs, always hide the external speaker test and thats the only time to hide it.
        // HT-44 Audio tab of Manufacturing test doesn't work

        showCustomerSpeakerTest( false  );
    }
    else
    {
        showCustomerSpeakerTest( true  );
    }

    if( isLinux() )
    {
        ui->lblTapSeveralTimes->setVisible( false );
        ui->startBeeperTestButton->setEnabled( true );
        ui->startBeeperTestButton->setVisible( true );

        ui->lblConfirmBeeps->setText( "Did you hear the beeps?" );
    }
    else
    {
        ui->startBeeperTestButton->setEnabled( false );
        ui->startBeeperTestButton->setVisible( false );
        ui->lblTapSeveralTimes->setVisible( true );

        ui->lblConfirmBeeps->setText( "Did you hear the 5 pulses?" );
    }

    ui->startCheckerBoardPatternTestButton->setFixedHeight(40);
    ui->startVerticalLinePatternTestButton->setFixedHeight(40);
    ui->startLabelTakenSensorTestButton->setFixedHeight(40);
    ui->startMediaSensorTestButton->setFixedHeight(40);
    ui->startCalibrationTestButtton->setFixedHeight(40);
    ui->startPrinterLabelCutterTestButton->setFixedHeight(40);


    // HT-44 Audio tab of Manufacturing test doesn't work - updated,
    // only hide if HTs which was already done up above, so remove this
    // [ Code that was here was removed ]

    ui->startWiredNetworkTestButton->setFixedHeight(40);
    ui->startWirelessNetworkTestButton->setFixedHeight(40);

    // SOF-3244: Request the FIT Parameters
    haveFitParameters = false;
    loadCellCalibrationChecked = false;
    loadCellCalibrationFailed = false;

    checkFITParams = false;

    // FT-1223
    //-----------------------------
    // Send Weigher Inquiry Request
    //-----------------------------
    session_->send( Weigher, WgReqInquire );

    //-----------------------------
    // Send Printer Inquiry Request
    //-----------------------------
    session_->send(Printer, PrReqCutterStatus);       // primary printer = 0

    if (!session_->isPrepack())
    {
        // FT-1223
        //--------------------------------------------------------------------------
        // Request info for the "Allow printing without a Customer Display" checkbox
        //--------------------------------------------------------------------------
        session_->send( ConfigurationDev, CfgReadSingleField, "," + QString::number( fieldOffsetCfgGCDPrintWithoutCustomerDisplay ) );
    }
    else
    {
        session_->send( ConfigurationDev, CfgReadSingleField, "," + QString::number( fieldOffsetCfgGCDPrintWithoutCustomerDisplay ) );
    }

    // FT-1626
    //----------------------------------------------------------------------
    // Request IP address of server to ping / IP to send data to be captured
    //----------------------------------------------------------------------
    session_->send( ConfigurationDev, CfgReadSingleField, "," + QString::number( fieldOffsetCfgIMfgTestServer ) );

    // SOF-2717
    factoryResetDone = false;
    factoryResetAttempted = false;
    periphControllerVer = scaleConfig->printerPowerupTests.softwareVersion();
    periphControllerVerWeigher = scaleConfig->weigherPowerupTests.softwareVersion();

    // HW-63
    connect( &getTestStatusTimer, SIGNAL( timeout() ), SLOT( getTestStatusInternal() ) );

    //FT-1218 for HTs that doesn't have wifi module installed.
    if ( session_->scaleConfig->isWifiModulePresent_ )
        ui->startWirelessNetworkTestButton->setEnabled(true);
    else
    {
        ui->startWirelessNetworkTestButton->setEnabled(false);
        ui->startBtTestButton->setEnabled(false);
    }

    // initialize any ui elements here
    ui->stackedWidget->setCurrentWidget(ui->testsPage);
    ui->testTabsWidget->setCurrentWidget( ui->dateTimeTab );

    lastTabIndex = ui->testTabsWidget->currentIndex();

    ui->rightStopValueLabel->setText("0");
    ui->leftStopValueLabel->setText("0");

    answerOptions << "" << "No" << "Yes";

    initializeAnswers();

    flashProgrammed = false;
    calibrationPassed = false;
    weigherStopsPassed = false;

    weigherModeCountsTransmission = false;

    // FT-577
    checkLabelTakenSensorMode = false;
    checkSensorUnblockedBeforeTestPattern = false;

    // FT-735
    adjustLeftStop = 0;
    adjustRightStop = 0;

    // FT-1055
    ui->acceptStopsAdjustmentButton->setDisabled( true );

    stereoTestFiles.append("leftChannel.aac");
    stereoTestFiles.append("rightChannel.aac");
    stereoTestFiles.append("bothChannels.aac");

    if (currentManfTestMode == BoardTest)
    {
        ui->sysCtrlManufactureDateLabel->setText("Board Test Date:");
    }

    ui->sysCtrlManufactureDateLineEdit->setText(QDate::currentDate().toString("MM/dd/yyyy"));

    failIcon = QIcon(":/images/screens/fail.png");
    passIcon = QIcon(":/images/screens/pass.png");

    connect(ui->testTabsWidget, SIGNAL(currentChanged(int)), this, SLOT(testsIndexChanged(int)));
    if(!session_->isPrepack())
    {
        connect(ui->printerTestsTab, SIGNAL(currentChanged(int)), this, SLOT(printerTestsIndexChanged(int)));
        connect(ui->weigherTestsTab, SIGNAL(currentChanged(int)), this, SLOT(weigherTestsIndexChanged(int)));
    }


    // set left stop value to red color indicating out of range
    ui->leftStopValueLabel->setStyleSheet("background-color: rgb(255, 0, 0); font: 18pt \"MS Shell Dlg 2\";");

    // set right stop value to red color indicating out of range
    ui->rightStopValueLabel->setStyleSheet("background-color: rgb(255, 0, 0); font: 18pt \"MS Shell Dlg 2\";");

    // FT-1055
    connect( ui->saveValueButton, SIGNAL( clicked() ), this, SLOT( on_saveValueButton_clicked() ) );

    // FT-1222
    connect( ui->btnStartScan, SIGNAL( clicked() ), this, SLOT( startHteScan() ) );
    this->connect(scanProcess, SIGNAL( readyReadStandardOutput() ), this, SLOT( displayScanOutput() ) );
    this->connect(scanProcess, SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( finishedProcess( int, QProcess::ExitStatus) ) );
    this->connect(scanProcess, SIGNAL( readyReadStandardError() ), this, SLOT( readyReadStandardError() ) );

    // FT-38
    connect( ui->rbDateTimeCorrectAnswerYes,         SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    connect( ui->rbDateTimeCorrectAnswerNo,          SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    connect( ui->rbTouchScreenAnswerYes,             SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    connect( ui->rbTouchScreenAnswerNo,              SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );

    // SOF-304
    connect( ui->rbOperatorDisplayAnswerYes,         SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    connect( ui->rbOperatorDisplayAnswerNo,          SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );

    if(!session_->isPrepack() && !session_->scaleConfig->scaleConfig.scaleName().startsWith("FS"))
    {
        connect( ui->rbCustomerDisplayAnswerYes,     SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
        connect( ui->rbCustomerDisplayAnswerNo,      SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    }

    if( isSelfServiceScale("FS") )
    {
        ui->rbCustomerDisplayAnswerYes->setVisible(false);
        ui->rbCustomerDisplayAnswerNo->setVisible(false);
        ui->gbCustomerDisplayAnswers->setVisible(false);
        ui->lblCustomerDisplayTestMessage->setVisible(false);


        connect(ui->startLowStockSensorTestButton, SIGNAL( clicked() ) , SLOT( on_startLowStockSensorTest_clicked() ) );
    }

    connect( ui->rbBeeperTestAnswerYes,              SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    connect( ui->rbBeeperTestAnswerNo,               SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    if (!session_->isPrepack())    //	HA-62
    {
        connect( ui->rbStereoTestAnswerYes,              SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
        connect( ui->rbStereoTestAnswerNo,               SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
        connect( ui->rbFanSpinningTestAnswerYes,         SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
        connect( ui->rbFanSpinningTestAnswerNo,          SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
        connect( ui->rbCheckerBoardPatternTestAnswerYes, SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
        connect( ui->rbCheckerBoardPatternTestAnswerNo,  SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
        connect( ui->rbVerticalLinePatternTestAnswerYes, SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
        connect( ui->rbVerticalLinePatternTestAnswerNo,  SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    }

    // Cutter
    connect( ui->butEnableCutter,      SIGNAL( clicked() ), this, SLOT( on_enableCutterButton_clicked() ) );
    connect( ui->startPrinterLabelCutterTestButton,      SIGNAL( clicked() ), this, SLOT( on_startPrinterLabelCutterTestButton_clicked() ) );
    cutterTimer_ = new QTimer( this );
    connect( cutterTimer_, SIGNAL( timeout() ), SLOT( onCutterTimeout() ) );

    // SOF-3887
    connect( ui->butResetCutterDefault, SIGNAL( clicked() ), this, SLOT( butResetCutterDefault_clicked() ) );

    connect( ui->sysCtrlStyleLineEdit, SIGNAL( editingFinished() ), this, SLOT( setTabsBasedOnStyle() ) );

    currentManfTestMode = ManufTestPreTest;

    // ManfTest Enhancements (barcode scanner for elevated self-serve)
    connect( ui->rbSelfServiceAnswerNo, SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    connect( ui->rbSelfServiceAnswerYes, SIGNAL( clicked() ), this, SLOT ( enableBarCodeTest() ) );

    if (currentManfTestMode == BoardTest)
    {
        connect( ui->exitPushButton, SIGNAL( clicked() ), this, SLOT ( exitBoardTest() ) );
    }

    // HT-479
    if ( currentManfTestMode == ManufTestPreTest   ||
         currentManfTestMode == ManufTestFinalTest ||
         currentManfTestMode == ServiceDiagnosticMode )
    {
        connect( ui->rbSelfServiceAnswerNo, SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    }

    // Set the Date/Time
    setDateTimeLabel();

    // Set the initial Date in Calendar control
    ui->calendarWidget->setSelectedDate( QDate::currentDate() );

    // Set the initial Current Time in the Time control
    ui->timeEdit->setTime( QTime::currentTime() );

    connect( ui->btnSetDateTime, SIGNAL( clicked() ), this, SLOT( on_SaveDateTimeButton_clicked() ) );

    populateTimeZones();

    // Read setup values from the scalesettings DB file
    connectDB();

    // FT-39 / FT-1149
    currentTabOffset = 0;

    noWeigher = false;
    noCustomerDisplay = false;
    noWirelessNetworking = false;
    noCanBus = false;

    if (session_->isPrepack())	//	HA-62. No Customer Speakers for EPCP.
    {
        noCustomerSpeakers = true;

        showCustomerSpeakerTest( false );
    }
    else
    {
        // FT-1333
        noCustomerSpeakers = false;

        showCustomerSpeakerTest( true );
    }

    // FT-1378
    noBluetooth = false;

    // FT-1423 - Request info for summary report label
    // Request Version Info
    session_->send( SessionDev, SnVersionInfo );

    // Request Peripheral Controller Version Info
    session_->send( PrinterSystem, PrSysReqStatus, "0" );

    // Request the Weigher info
    session_->send( WeigherSystem, SysReqStatus );

    // Request Loader Version
    session_->send( MfgTestDev, RequestLoaderVersion );

    // FT-1418
    autoCompleteUsbTab = false;

    // FT-1417
    ui->sysCtrlManufactureDateLineEdit->installEventFilter( this );
    ui->sysCtrlSerialNumberLineEdit->installEventFilter( this );
    ui->sysCtrlStyleLineEdit->installEventFilter( this );
    ui->sysCtrlBoardAssemblyNumberLineEdit->installEventFilter( this );
    ui->sysCtrlBoardAssemblyRevisionLineEdit->installEventFilter( this );
    connect( &selectTextInFieldTimer, SIGNAL( timeout() ), SLOT( selectTextInField() ) );
    // FT-1335
    discoverBluetoothStatus = 1; // Default to no Bluetooth devices found

    // SOF-3449
    if( !isArkBoardSite )
        this->connect( discoverBluetoothProcess, SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( finishedBluetoothProcess( int, QProcess::ExitStatus) ) );

    LOG_DEBUG( "detecting wifi...", NULL );

    // FT-1218
    if( ui->startWirelessNetworkTestButton->isVisible() && ui->startBtTestButton->isVisible() )
    {
//        if ( (wifiPresent = future_isWifiModulePresent.result()) )
        if (session_->scaleConfig->isWifiModulePresent_)
            ui->startWirelessNetworkTestButton->setEnabled(true);
        else
        {
            ui->startWirelessNetworkTestButton->setEnabled(false);
            ui->startBtTestButton->setEnabled(false);
        }
    }

    // SOF-1923
    needUbootUpgrade = false;
    ubootUpgradeSuccessful = false;

    // Cannot accept messages sent form the front end after the summary report print
    // Messenger.receive stops printing in debug



    // HW-63

    /*
    LOG_DEBUG( "waiting for background tasks to complete...", NULL );
    future_setMaxBacklight.waitForFinished();
    future_getFlashInfo.waitForFinished();
    future_getMACAddress.waitForFinished();
    future_getNetworkInfo.waitForFinished();
    future_getScaleModel.waitForFinished();
    future_getHostname.waitForFinished(); // FT-1626
    */

    // NOTE: FT-1696 code moved to initLabelsOnDataReceived()

    shootThroughCalStarted = false;
    shootThroughRoutineCanceled = false;

    // SOF-5182
    printheadDisengaged_ = false;
    calDialog = NULL;
    calInitDialog = NULL;
    calBackingDialog = NULL;


    // Update tabs and sub-tabs based on Style (Model Name)
    LOG_DEBUG( "refreshing ui...", NULL );
    setTabsBasedOnStyle();

    translate();

    waitingForValueMax = false;
    changingScreens = false;

    activeDialog = NULL;

    defaultBtnPal = ui->btnSetDateTime->palette();
    installButtonEventFilter();

    LOG_DEBUG( "done!", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool manufacturingTest::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void manufacturingTest::translate()
{
    ui->retranslateUi( this );

    QLocale::Language lang = QLocale::English;
    if("EN.US" == mainWindow_->language())
    {
        lang = QLocale::English;
    }
    else if("EN.CA" == mainWindow_->language()) //Could be different than English
    {
        lang = QLocale::English;
    }
    else if("ES.MX" == mainWindow_->language()) // Spanish
    {
        lang = QLocale::Spanish;
    }
    else if("FR.CA" == mainWindow_->language()) // French
    {
        lang = QLocale::French;
    }

    ui->calendarWidget->setLocale(lang);


    if( weigherCalibrate_ )
    {
        weigherCalibrate_->translate();
    }
    update();
}

// FT-1417
//-----------------------------------------------------------------
// Event filters for the fields on the Flash tab
// Used to check for focus and select all of the text in the field.
//-----------------------------------------------------------------
bool manufacturingTest::eventFilter( QObject *obj, QEvent *e )
{
    if( QEvent::FocusIn == e->type() )
    {
        if( obj == ui->sysCtrlManufactureDateLineEdit )
        {
            selectTextInFieldWidget = ui->sysCtrlManufactureDateLineEdit;

            selectTextInFieldTimer.setSingleShot( true );
            selectTextInFieldTimer.start( 0 );
        }

        if( obj == ui->sysCtrlSerialNumberLineEdit )
        {
            selectTextInFieldWidget = ui->sysCtrlSerialNumberLineEdit;

            selectTextInFieldTimer.setSingleShot( true );
            selectTextInFieldTimer.start( 0 );
        }

        if( obj == ui->sysCtrlStyleLineEdit )
        {
            selectTextInFieldWidget = ui->sysCtrlStyleLineEdit;

            selectTextInFieldTimer.setSingleShot( true );
            selectTextInFieldTimer.start( 0 );
        }

        if( obj == ui->sysCtrlBoardAssemblyNumberLineEdit )
        {
            selectTextInFieldWidget = ui->sysCtrlBoardAssemblyNumberLineEdit;

            selectTextInFieldTimer.setSingleShot( true );
            selectTextInFieldTimer.start( 0 );
        }

        if( obj == ui->sysCtrlBoardAssemblyRevisionLineEdit )
        {
            selectTextInFieldWidget = ui->sysCtrlBoardAssemblyRevisionLineEdit;

            selectTextInFieldTimer.setSingleShot( true );
            selectTextInFieldTimer.start( 0 );
        }
    }

    if ( e->type() == QEvent::MouseButtonPress )
    {
        QPushButton * myBtn = dynamic_cast<QPushButton*>(obj);

        if (myBtn)
            myBtn->setPalette( QColor( hobart::colorPalette::lightBlue) );

    }

    if ( e->type() == QEvent::MouseButtonRelease )
    {
        QPushButton * myBtn = dynamic_cast<QPushButton*>(obj);

        if(myBtn)
            myBtn->setPalette( defaultBtnPal );

    }

    return _Mybase::eventFilter( obj, e );
}

void manufacturingTest::onScreenFullyChanged()
{
    LOG_CLASS_INFO("manufacturingTest", "onScreenChanged");

    LOG_DEBUG("Starting the printer/weigher reset.", NULL);

//    initiatePrinterWeigherReset();

}

void manufacturingTest::on_startLowStockSensorTest_clicked()
{

    if( isSSKPrinterAttached() && currentManfTestMode == ManufTestPreTest )
    {

        swapToLinerlessDialog = new MessageBoxDialog(mainWindow_);
        swapToLinerlessDialog->setAllowOverlayClickDismiss( false );
        swapToLinerlessDialog->setTitle( "Label Stock" );
        swapToLinerlessDialog->setText( tr( "Please unload the diecut stock and install linerless label stock."
                            "\n\nThen engage the printhead and close the door." ) );


        swapToLinerlessDialog->open();
        swapLabelStockRoutineStarted_ = true;

        session_->send( Printer, PrSensorsTracking, "0,1" );

        // Will not do the range check of the shoot through sensor
        checkLabelTakenSensorMode = false;

    }
}

void manufacturingTest::on_ShootThroughCanceled()
{
    LOG_CLASS_INFO("manufacturingTest", "onShootThroughCanceled");

    LOG_DEBUG("Cancel shoot through calibration", NULL);

    ui->lblStatusValue->setVisible(false);
    ui->startMediaSensorTestButton->setEnabled( true );
    ui->startMediaSensorTestButton->setText( "Start Shoot Through Calibration Test" );

    // turn sensors back on
    session_->send( Printer, PrSensorsTracking, "0,1" );

    shootThroughRoutineCanceled = true;
    shootThroughCalStarted = false;

    delete activeDialog;
    activeDialog = NULL;

}


// SOF-5995: this code was originally in the constructor, moved here for convenience
void manufacturingTest::initLabelsOnDataReceived()
{
    LOG_CLASS_INFO("manufacturingTest", "initLabelsOnDataReceived");
    //-------------------
    // Style on Flash tab
    //-------------------

    if(initialScaleModelName.length() >= 3 )
        ui->sysCtrlStyleLineEdit->setText( initialScaleModelName );

    //------------------------------------------------------------------------------------
    // Manufacture Date, Serial Number, Board Assembly # and Board Revision # on Flash tab
    //------------------------------------------------------------------------------------

    QString manufacturingDate;
    QString serialNumber;
    QString boardAssemblyNumber;
    QString boardRevisionNumber;

    if ( 4 < flashInfo.size() )
    {
        manufacturingDate = flashInfo[ 0 ];
        serialNumber = flashInfo[ 1 ];
        initialSerialNumber = serialNumber;
        boardAssemblyNumber = flashInfo[ 3 ];
        boardRevisionNumber = flashInfo[ 4 ];
    }

    // FT-1611 - Hillsboro wants the current date for the manufacture date, not the date that was in the flash.
    //if( !(manufacturingDate.isEmpty() ) )
    //    ui->sysCtrlManufactureDateLineEdit->setText( manufacturingDate );

    if( !(serialNumber.isEmpty() ) )
        ui->sysCtrlSerialNumberLineEdit->setText( serialNumber );

    if( !(boardAssemblyNumber.isEmpty() ) )
        ui->sysCtrlBoardAssemblyNumberLineEdit->setText( boardAssemblyNumber );

    if( !(boardRevisionNumber.isEmpty() ) )
        ui->sysCtrlBoardAssemblyRevisionLineEdit->setText( boardRevisionNumber );

    //-----------------------------
    // MAC Addresses on Network tab
    //-----------------------------

    ui->lblWiredTab_EthernetMACAddressData->setText( ethernetMacAddress );
    ui->lblWiredTab_WirelessMACAddressData->setText( wirelessMacAddress );

    ui->lblWirelessTab_EthernetMACAddressData->setText( ethernetMacAddress );
    ui->lblWirelessTab_WirelessMACAddressData->setText( wirelessMacAddress );

    //---------------------------------
    // More Network Info on Network tab
    //---------------------------------

    // FT-1331: Moved constant IP address to UI form
    //    //-----------------------------------------------
    //    // Gateway IP address for Manufacturing Test mode
    //    //-----------------------------------------------

    //    // FT-1269
    //    // Wireless not able to ping gateway IP addess from getNetworkInfo
    //    // This is gateway IP address for Hillsboro obtained from Matt French
    //    ui->txtIpToPing->setText( "10.31.16.1" );

    if( netInfo.count() >= 5 )
    {
        LOG_DEBUG( "Setting values on Service Diag/Network tab", NULL );

        //-------------------------------
        // Service Diagnostic mode: Wired
        //-------------------------------

        // Gateway IP address
        ui->lblServiceDiagWiredNetworkGatewayData->setText( ( netInfo[4] ).toUpper() );
        // DHCP Server IP address
        ui->lblServiceDiagWiredNetworkDHCPData->setText( ( netInfo[3] ).toUpper() );

        // Disable radio buttons for Gateway and DHCP if no IP address
        if( ui->rbServiceDiagWiredNetworkGateway->isVisible() && ( ( netInfo[4] ).toUpper() ) == "N/A" )
            ui->rbServiceDiagWiredNetworkGateway->setEnabled( false );

        if( ui->rbServiceDiagWiredNetworkDHCP->isVisible() && ( ( netInfo[3] ).toUpper() ) == "N/A" )
            ui->rbServiceDiagWiredNetworkDHCP->setEnabled( false );

        //----------------------------------
        // Service Diagnostic mode: Wireless
        //----------------------------------

        // Gateway IP address
        ui->lblServiceDiagWirelessNetworkGatewayData->setText( ( netInfo[4] ).toUpper() );
        // DHCP Server IP address
        ui->lblServiceDiagWirelessNetworkDHCPData->setText( ( netInfo[3] ).toUpper() );

        // Disable radio buttons for Gateway and DHCP if no IP address
        // FT-1696 Scale shuts down in Manufacturing
        // Note that you crash the scale if you try and disable them and they are hidden
        if( ui->rbServiceDiagWirelessNetworkGateway->isVisible() && ( ( netInfo[4] ).toUpper() ) == "N/A" )
            ui->rbServiceDiagWirelessNetworkGateway->setEnabled( false );

        if( ui->rbServiceDiagWirelessNetworkDHCP->isVisible() && ( ( netInfo[3] ).toUpper() ) == "N/A" )
            ui->rbServiceDiagWirelessNetworkDHCP->setEnabled( false );
    }

    // ---- END FT-1696 ---- //

}

// FT-1417
// Slot to select all of the text
void manufacturingTest::selectTextInField()
{
    selectTextInFieldWidget->selectAll();
}

// FT-1218
// SOF-5996 not used
bool manufacturingTest::isWifiModulePresent()
{
    bool answer = false;
    QProcess macAddressProcess;
    QStringList options;
    options.append(filesystem::absoluteFilePath("getMac.py",folderType::Python));

    macAddressProcess.start("python", options);

    if(!macAddressProcess.waitForStarted())
    {
        //this->ui->alertsLineEdit->setText(tr("Start configuration error."));
        answer = false;
    }

    if(!macAddressProcess.waitForFinished())
    {
        ;
    }

    QStringList macAddresses;
    QByteArray result = macAddressProcess.readAll();
    QString macAddress(result);
    macAddress = macAddress.mid(0,macAddress.length()-1);

    if(macAddress.size() > 0)
    {
        macAddresses = macAddress.split("|", QString::SkipEmptyParts);
        if(macAddresses[1].startsWith("No",Qt::CaseInsensitive)) answer = false;
        else answer = true;
    }

    return answer;
}

void manufacturingTest::setAnswerLabelValue(QLabel *answerLabel, AnswerType answer)
{
    answerLabel->setText(answerOptions.at(answer));
}

bool manufacturingTest::dateTimeStatus()
{
    if( DBG_AUTO_COMPLETE_ALL_TABS )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->dateTimeTab ), passIcon );
        tabStatus[ ui->testTabsWidget->indexOf( ui->dateTimeTab ) ] = 1;
        return true;
    }

    bool status = true;

    // SOF-3449
    if( isArkBoardSite )
    {
        if( arkRTC_ == 1)
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->dateTimeTab ), passIcon );

            tabStatus[ ui->testTabsWidget->indexOf( ui->dateTimeTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->dateTimeTab ), failIcon );

            tabStatus[ ui->testTabsWidget->indexOf( ui->dateTimeTab ) ] = 0;

            status = false;
        }

        getTestStatus();
    }
    else
    {
        // FT-38
        if ( ( ui->rbDateTimeCorrectAnswerYes->isChecked() ) && setDateTimeSuccessful )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->dateTimeTab ), passIcon );

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->dateTimeTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->dateTimeTab ), failIcon );

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->dateTimeTab ) ] = 0;

            status = status & false;
        }

        // SOF-891
        if( !setDateTimeSuccessful )
        {
            ui->rbDateTimeCorrectAnswerYes->setChecked( false );
            ui->rbDateTimeCorrectAnswerNo->setChecked( true );

            status = false;
        }
    }

    return status;
}

bool manufacturingTest::displayTestStatus()
{
    if( DBG_AUTO_COMPLETE_ALL_TABS )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->displayTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->displayTab ) ] = 1;
        return true;
    }

    bool status = true;

    //-----------------
    // Operator Display
    //-----------------
    // SOF-304
    if( !( ui->rbOperatorDisplayAnswerYes->isChecked() ) )
    {
        status = status & false;
    }

    if ( !noCustomerDisplay )
    {
        //-----------------
        // Customer Display
        //-----------------
        // SOF-304
        if( !( ui->rbCustomerDisplayAnswerYes->isChecked() ) )
        {
            status = status & false;
        }
    }

    //-------------
    // Touch screen
    //-------------
    if (!touchScreenTestStatus())
    {
        status = status & false;
    }

    //------------
    // Display tab
    //------------
    if (status)
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->displayTab),passIcon);

        // FT-39 / FT-1149
        tabStatus[ ui->testTabsWidget->indexOf( ui->displayTab ) ] = 1;
    }
    else
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->displayTab),failIcon);

        // FT-39 / FT-1149
        tabStatus[ ui->testTabsWidget->indexOf( ui->displayTab ) ] = 0;
    }
    return status;
}


bool manufacturingTest::soundTestStatus()
{
    if( DBG_AUTO_COMPLETE_ALL_TABS )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->audioTab ) ] = 1;
        return true;
    }

    bool status = true;

    // FT-38
    if ( !noCustomerSpeakers ) // FT-1333
    {
        // Need to check Operator side and Customer side

        if ( ( ( ui->rbBeeperTestAnswerYes->isChecked() ) &&
             ( ( ui->rbStereoTestAnswerYes->isChecked() ) ) ) )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),passIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->audioTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),failIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->audioTab ) ] = 0;

            status = status & false;
        }
    }
    else
    {
        // Only need to check Operator side
        if ( ( ui->rbBeeperTestAnswerYes->isChecked() ) )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),passIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->audioTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),failIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->audioTab ) ] = 0;

            status = status & false;
        }
    }

    return status;
}

bool manufacturingTest::usbTestStatus()
{
    LOG_CLASS_INFO( "manufacturingTest", "usbTestStatus" );

    if( DBG_AUTO_COMPLETE_ALL_TABS )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->usbTab ) ] = 1;
        return true;
    }

    // SOF-3449
    if( isArkBoardSite )
    {
        if( arkUSB_ == 1)
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),passIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->usbTab ) ] = 1;

            usbTabPassed = true;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),failIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->usbTab ) ] = 0;
        }

        getTestStatus();
    }
    else
    {
        // It's not the USB tab's turn yet - Don't set the passIcon yet
        int btUsbOffset = ui->testTabsWidget->indexOf( ui->usbTab );
        if ( ( currentTabOffset < btUsbOffset ) && ( currentManfTestMode != ServiceDiagnosticMode ) )
        {
            ui->testTabsWidget->setTabIcon( ui->testTabsWidget->indexOf( ui->usbTab ), failIcon );

            return false;
        }

        LOG_TRACE( "usbTestStatus: usbBarcodeScannerRecognizedAnswerLabel=%s", (ui->usbBarcodeScannerRecognizedAnswerLabel->text() == "Yes") ? "YES" : "NO" );
        LOG_TRACE( "usbTestStatus: usbMouseRecognizedAnswerLabel=%s", (ui->usbMouseRecognizedAnswerLabel->text() == "Yes") ? "YES" : "NO" );
        LOG_TRACE( "usbTestStatus: gbSelfServiceAnswers->isHidden()=%s", (ui->gbSelfServiceAnswers->isHidden()) ? "YES" : "NO" );
        LOG_TRACE( "usbTestStatus: rbSelfServiceAnswerYes->isChecked()=%s", (ui->rbSelfServiceAnswerYes->isChecked()) ? "YES" : "NO" );
        LOG_TRACE( "usbTestStatus: rbSelfServiceScannerPromptAnswerYes->isChecked()=%s", (ui->rbSelfServiceScannerPromptAnswerYes->isChecked()) ? "YES" : "NO" );

        if ( ( ( ui->usbBarcodeScannerRecognizedAnswerLabel->text() == "Yes" ) &&
               ( ui->usbMouseRecognizedAnswerLabel->text() == "Yes" ) &&
               (  ( ui->gbSelfServiceAnswers->isHidden() ) ||
                  ( !( ui->gbSelfServiceAnswers->isHidden() ) && ui->rbSelfServiceAnswerNo->isChecked() ) ||
                  ( !( ui->gbSelfServiceAnswers->isHidden() ) && ui->rbSelfServiceAnswerYes->isChecked() && ui->rbSelfServiceScannerPromptAnswerYes->isChecked() )
               )
             ) ||
             (
                 // SOF-4765: Mark the USB tab as "Passed" for Service Diagnostics mode when the device has an ARA board
                 ( boardManf == "ARA") && ( currentManfTestMode == ServiceDiagnosticMode ) && ( autoCompleteUsbTab )

             )
           )
        {
            // FT-1270
            // Once the tab has passed once it should always show as passed,
            // even if the USB devices are then un-plugged
            usbTabPassed = true;
        }

        if ( usbTabPassed )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),passIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->usbTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),failIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->usbTab ) ] = 0;
        }
    }

    return usbTabPassed;
}

bool manufacturingTest::fanTestStatus()
{
    LOG_CLASS_INFO( "manufacturingTest", "fanTestStatus" );

    if( DBG_AUTO_COMPLETE_ALL_TABS )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->fanTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->fanTab ) ] = 1;
        return true;
    }

    bool status = true;

    if (session_->isPrepack())	//	HA-62
    {
        LOG_DEBUG("Test not required for a Prepack scale - returning true.", NULL );
        return true;
    }

    // SOF-5995
    if ( isSelfServiceScale("FS"))
    {
        LOG_DEBUG("Test not required for a FSS scale - returning true.", NULL);
        return true;
    }

    // FT-38
    if ( (ui->rbFanSpinningTestAnswerYes->isChecked() ) )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->fanTab),passIcon);

        // FT-39 / FT-1149
        tabStatus[ ui->testTabsWidget->indexOf( ui->fanTab ) ] = 1;
    }
    else
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->fanTab),failIcon);

        // FT-39 / FT-1149
        tabStatus[ ui->testTabsWidget->indexOf( ui->fanTab ) ] = 0;

        status = status & false;
    }

    return status;
}

bool manufacturingTest::canbusTestStatus()
{
    LOG_CLASS_INFO( "manufacturingTest", "canbusTestStatus" );

    bool status = true;

    // SOF-3449
    if( isArkBoardSite )
    {
        if( arkSDSlot0_ == 1 && arkSDSlot1_ == 1 && arkMemory_ == 1 &&
            arkCan0_ == 1 && kpcCan_ == 1 && kpcModel_ )
        {
            tabStatus[ ui->testTabsWidget->indexOf( ui->canbusTab ) ] = 1;

            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->canbusTab ), passIcon );
        }
        else
        {
            tabStatus[ ui->testTabsWidget->indexOf( ui->canbusTab ) ] = 0;

            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->canbusTab ), failIcon );

            status = status & false;
        }

        getTestStatus();
    }
    else
    {
        // SOF-3556
        if ( noCanBus )
        {
            LOG_DEBUG("CANbus test not needed - returning true.", NULL );

            return true;
        }

        // SOF-930: Hide testing of 2nd SD card slot on I/O tab
        //    if ( ( ui->canbusAnswerLabel->text() == "No" ) ||
        //         ( ui->sataAnswerLabel_IO->text() == "No" ) ) // SOF-896

        if ( ( ui->canbusAnswerLabel->text() == "No" ) )
        {
            tabStatus[ ui->testTabsWidget->indexOf( ui->canbusTab ) ] = 0;
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->canbusTab ), failIcon );
            status = status & false;
        }
        else
        {
            tabStatus[ ui->testTabsWidget->indexOf( ui->canbusTab ) ] = 1;
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf( ui->canbusTab ), passIcon );
        }
    }

    return status;
}

bool manufacturingTest::touchScreenTestStatus()
{
    bool status = true;

    // FT-38
    if( !( ui->rbTouchScreenAnswerYes->isChecked() ) )
    {
        status = status & false;
    }
    return status;
}

void manufacturingTest::printerTestsIndexChanged(int currentTabIndex)
{
    currentPrinterTestTabIndex = currentTabIndex;
    testsIndexChanged(lastTabIndex);
}

void manufacturingTest::weigherTestsIndexChanged(int currentTabIndex)
{
    currentWeigherTestTabIndex = currentTabIndex;
    testsIndexChanged(lastTabIndex);
}


void manufacturingTest::testsIndexChanged(int currentTabIndex)
{
    LOG_CLASS_INFO( "manufacturingTest", "testsIndexChanged" );

    LOG_DEBUG( "testsIndexChanged: currentManfTestMode=%d", currentManfTestMode );

    QDate highlightedDate;
    highlightedDate = ui->calendarWidget->selectedDate();
    // WEH-97 set the date correctly everytime the time tab is focused
    if(ui->testTabsWidget->indexOf(ui->dateTimeTab) == currentTabIndex)
    {
        QDate highlightedDate;
        highlightedDate = ui->calendarWidget->selectedDate();
        ui->calendarWidget->setSelectedDate(highlightedDate);
        setDateTimeLabel();

        // SOF-1723
        if( currentManfTestMode == ManufTestFinalTest )
        {
            //-------------------------------------
            // Current year MUST be 2018 or greater
            //-------------------------------------

            if( highlightedDate.year() < 2018 )
            {
                // SOF-1753
                if( !dateTimeValidationErrorDisplayed )
                {
                    dateTimeValidationErrorDisplayed = true;

                    MessageBoxDialog dialog( real_time_priority, mainWindow_ );
                    dialog.setAllowOverlayClickDismiss( false );

                    dialog.setTitle( "FAILURE:  DATE / TIME VALIDATION" );
                    dialog.setText( "Year must be 2018 or greater.  Date/Time did not get saved in Manufacturing Pre-Test.\n\nYou may need to replace the battery or the board.\n\nPress OK to continue." );
                    dialog.setStandardButtons( QDialogButtonBox::Ok );
                    dialog.exec();

                    // Return to Home (i.e. PluMainScreen).
                    mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();
                }
            }
        }
    }
    else if (ui->testTabsWidget->indexOf(ui->weigherTab) == currentTabIndex)
    {
        if ( ui->weigherTestsTab->indexOf( ui->calibrationPage ) == currentTabIndex) // Calibration tab
        {
            // On the Weigher->Calibration tab: Need to STOP the counts

            if (weigherModeCountsTransmission)
            {
                resetStopsValues();

                // Turn OFF counts
                QString msg;
                msg = "3";

                session_->send( Weigher, WgMode, msg );
                weigherModeCountsTransmission = false;
            }
        }
        else if ( ui->weigherTestsTab->indexOf( ui->countsPage ) == currentTabIndex) // Counts tab
        {
            if (weigherModeCountsTransmission)
            {
                resetStopsValues();

                // Turn OFF counts
                QString msg;
                msg = "3";

                session_->send( Weigher, WgMode, msg );
                weigherModeCountsTransmission = false;
            }

            QString msg;
            msg = "1";

            session_->send( Weigher, WgMode, msg );
            weigherModeCountsTransmission = true;
        }
        else
        {
            // On the Weigher->Stop tab: Need to START the counts

            setStopsFlags();

            QString msg;
            msg = "1";

            session_->send( Weigher, WgMode, msg );
            weigherModeCountsTransmission = true;
        }
    }
    else
    {
        if (weigherModeCountsTransmission)
        {
            resetStopsValues();

            // Turn OFF counts
            QString msg;
            msg = "3";

            session_->send( Weigher, WgMode, msg );
            weigherModeCountsTransmission = false;
        }
    }

    // FT-647
    if ( !( ui->testTabsWidget->indexOf(ui->printerTab) == currentTabIndex ) /*Not finished tests*/ )
    {
        // Printer Sensor OFF
        session_->send( Printer, PrSensorsTracking, "0,0" );
    }
    else if ( ( ui->testTabsWidget->indexOf( ui->printerTab ) == currentTabIndex ) && isSSKPrinterAttached() )
    {
        session_->send( Printer, PrSensorsTracking, "0,1" );

        if(!shootThroughCalStarted)
            ui->startMediaSensorTestButton->setText( "Start Shoot Through Calibration Test" );

        ui->label_35->setText( "Shoot Through Calibration Test Passed?" );
    }


    // SOF-5995: if the scale has a built in barcode scanner or we just successfully programmed the model (with scanner) in flash, enable the scanner test
    if ( isSelfServiceScale("FS") )
    {
        if ( ui->testTabsWidget->indexOf(ui->usbTab) == currentTabIndex)
        {
            if( ( isSelfServiceScale("B") && !flashProgrammed ) || ( flashProgrammed && ui->sysCtrlStyleLineEdit->text().contains("B") ) )
            {
                ui->usbBarcodeScannerRecognizedAnswerLabel->setText("Yes");
                ui->rbSelfServiceAnswerYes->setChecked(true);
                ui->lblSelfService->show();
                ui->gbSelfServiceAnswers->show();
                enableBarCodeTest();
            }

            // hide barcode scanner stuff if it is not a barcode scanner model
            else
            {
                ui->usbBarcodeScannerRecognizedAnswerLabel->setText("No");
                ui->gbSelfServiceAnswers->hide();
                ui->lblSelfService->hide();
            }
        }
        // set these labels to default
        else if ( ui->testTabsWidget->indexOf(ui->printerTab) == currentTabIndex )
        {
            ui->lblLowStockLabel->setText("Low Stock:");
            trackLabelStockSensorValue_ = true;
        }

    }
    // SOF-237: In "Board Test" mode, Board Revision Number is the only field that is editable
    if( ui->testTabsWidget->indexOf( ui->flashTab ) == currentTabIndex )
    {
        if( currentManfTestMode == BoardTest )
        {
            ui->sysCtrlBoardAssemblyRevisionLineEdit->setFocus();
        }
    }

    lastTabIndex = currentTabIndex;
}

bool manufacturingTest::printerTestStatus()
{
    LOG_CLASS_INFO( "manufacturingTest", "printerTestStatus" );

    if( isSSKPrinterAttached() )
    {
        /*
        if(!shootThroughCalStarted)
            ui->startMediaSensorTestButton->setText( "Start Shoot Through Calibration Test" );

        ui->label_35->setText( "Shoot Through Calibration Test Passed?" );
        */
    }

    if( DBG_AUTO_COMPLETE_ALL_TABS )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->printerTab ) ] = 1;
        return true;
    }

    bool status = true;

    // SOF-3449
    if( isArkBoardSite )
    {
        // SOF-232: Only need the Checkerboard Test pattern to be printed
        if ( ( ui->rbCheckerBoardPatternTestAnswerYes->isChecked() ) &&
             ( kpcCutter_ == 1 ) && ( kpcDotWear_ == 1 ) )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),passIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->printerTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),failIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->printerTab ) ] = 0;

            status = status & false;
        }

        getTestStatus();
    }
    else
    {
        if (currentManfTestMode != BoardTest)
        {
            if (session_->isPrepack())	//	HA-62
            {
                LOG_DEBUG("Test not required for a Prepack scale - returning true.", NULL );
                return true;
            }
            else
            {
                //-------------------
                // Printer Test Label
                //-------------------
                //----------------------------------------------------------------------------
                // TODO: Nothing is implemented for "Print UPC Barcode Test", so
                // the "Answer" is nothing for now until test is implemented in future release
                // i.e. (ui->upcBarcodePrintAnswerLabel->text() == "")))
                //----------------------------------------------------------------------------
                // FT-38

                if ( (ui->rbCheckerBoardPatternTestAnswerYes->isChecked() ) &&
                     (ui->rbVerticalLinePatternTestAnswerYes->isChecked() ) )

                {
                    ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),passIcon);
                    status = status & true;
                }
                else
                {
                    ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),failIcon);
                    status = status & false;

                    // SOF-5997: for final test on FSS, ignore the test label tab
                    if (currentManfTestMode == ManufTestFinalTest && isSelfServiceScale("FS") )
                    {
                        ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),passIcon);
                        status = true;
                    }
                }

                //----------------
                // Printer Sensors
                //----------------

                // SOF-3853: Only calibrate the media sensor when there is NOT a cutter
                if( !cutterAttached_ )
                {
                    if (!((ui->mediaSensorCalibrationTestAnswerLabel->text() == "Yes") &&
                          (ui->takenSensorTestPassedAnswerLabel->text() == "Yes")))
                    {
                        ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),failIcon);
                        status = status & false;
                    }
                    else
                    {
                        // for standalone self serve
                        if( isSSKPrinterAttached() && ui->lblLowStockData->text() == "0" )
                            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),failIcon);
                        else
                            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),passIcon);
                    }
                }

                // cutter attached
                else
                {
                    if (!((ui->takenSensorTestPassedAnswerLabel->text() == "Yes")))
                    {
                        ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),failIcon);
                        status = status & false;
                    }

                    // label taken sensor test passed

                    else
                    {

                        /* SOF-5997
                         * If we are in final test and an FS unit, only pass the test if the current sensor reading
                         * is under the designated threshold
                         */

                        if ( currentManfTestMode == ManufTestFinalTest && isSelfServiceScale("FS") )
                        {
                            if (shootThroughValueOK)
                                ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),passIcon);
                            else
                            {
                                ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),failIcon);
                                status = status & false;
                            }
                        }

                        // If we are in pre-test on FS, only advance the tab if the label stock sensor test has passed ( and the media sensor test )
                        else if ( currentManfTestMode == ManufTestPreTest && isSelfServiceScale("FS") )
                        {
                            if ( ui->mediaSensorCalibrationTestAnswerLabel->text() == "Yes")
                            {
                                ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab), passIcon);
                            }
                            else
                            {
                                ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),failIcon);
                                status = status & false;
                            }
                        }

                        else
                            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),passIcon);
                    }
                }

                // SOF-3763
                //-----------------
                // Printer Cassette
                //-----------------
                //
                //=================================================================
                // THE PRINTER CASSETTE TEST WILL BE AN OPTIONAL TEST, AND WILL NOT
                // HAVE TO BE COMPLETED SUCCESSFULLY IN MANUFACTURING TEST IN
                // ORDER FOR THE SCALE TO PASS.
                //=================================================================
//                {
//                    if (ui->lblCassetteTestPassedResult->text() == "Yes" )
//                        ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cassetteTestTab),passIcon);
//                    else
//                    {
//                        ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cassetteTestTab),failIcon);
//                        status = status & false;
//                    }
//                }

                //----------------
                // Label Cutter
                //----------------
                if( cutterAttached_ )
                {
                    if (ui->lblLabelCutterTestAnswer->text() == "Yes")
                        ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cutterTab),passIcon);
                    else
                    {
                        ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cutterTab),failIcon);
                        status = status & false;
                    }
                }

                // check label stock sensors test on cutter tab (FS)
                if ( currentManfTestMode == ManufTestPreTest && isSelfServiceScale("FS") )
                {
                    if ( !ui->labelStockSensorTestResult->text().contains("Passed") )
                    {
                        ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cutterTab), failIcon);
                        status = status & false;
                    }
                }
            }
        }
        else
        {
            //------------------
            // Board Tester mode
            //------------------

            // SOF-232: Only need the Checkerboard Test pattern to be printed
            if ( ui->rbCheckerBoardPatternTestAnswerYes->isChecked() )
            {
                ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),passIcon);
            }
            else
            {
                ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),failIcon);
                status = status & false;
            }

            //----------------
            // Label Cutter
            //----------------
            if( cutterAttached_ )
            {
                if (ui->lblLabelCutterTestAnswer->text() == "Yes" )
                    ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cutterTab),passIcon);
                else
                {
                    ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cutterTab),failIcon);
                    status = status & false;
                }
            }
        }

        //------------
        // Printer tab
        //------------
        if (status)
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),passIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->printerTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),failIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->printerTab ) ] = 0;
        }
    }

    return status;
}

bool manufacturingTest::bluetoothTestStatus()
{
    if( DBG_AUTO_COMPLETE_ALL_TABS )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->bluetoothTab ) ] = 1;
        return true;
    }

    bool status = true;

    // SOF-3449
    if( isArkBoardSite )
    {
        if( arkBlueTooth_ == 1 )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),passIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->bluetoothTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),failIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->bluetoothTab ) ] = 0;

            status = status & false;
        }

        getTestStatus();
    }
    else
    {
        // FT-1378
        if ( noBluetooth )
            return true;

        // FT-1335
        int btTabOffset = ui->testTabsWidget->indexOf( ui->bluetoothTab );

        // It's not the Bluetooth tab's turn yet - Don't set the passIcon yet
        if ( ( currentTabOffset < btTabOffset ) && ( currentManfTestMode != ServiceDiagnosticMode ) )
        {
            ui->testTabsWidget->setTabIcon( ui->testTabsWidget->indexOf( ui->bluetoothTab ), failIcon );

            return false;
        }

        if (!(ui->btMouseRecognizedAnswerLabel->text() == "Yes"))
        {
            status = status & false;
        }
        if (status)
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),passIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->bluetoothTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),failIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->bluetoothTab ) ] = 0;
        }
    }

    return status;
}

bool manufacturingTest::weigherTestStatus()
{
    LOG_CLASS_INFO( "manufacturingTest", "weigherTestStatus" );

    if( DBG_AUTO_COMPLETE_ALL_TABS || DBG_AUTO_COMPLETE_WEIGHER_TAB )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->weigherTab ) ] = 1;
        return true;
    }

    bool status = true;

    if ( noWeigher )
    {
        LOG_DEBUG("Weigher not attached - returning true.", NULL );
        return true;
    }
    if (session_->isPrepack())	//	HA-62
    {
        LOG_DEBUG("Test not required for a Prepack scale - returning true.", NULL );
        return true;
    }

    // SOF-3449
    if( isArkBoardSite )
    {
        if ( countsTestPassed && kpcValueMax_ == 1 )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),passIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->weigherTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),failIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->weigherTab ) ] = 0;

            status = status & false;
        }

        getTestStatus();
    }
    else
    {
        //-------------------------------------------------------------
        // Weigher Stops only available with Manufacturer Pre-Test mode
        //-------------------------------------------------------------
        if ( currentManfTestMode == ManufTestPreTest )
        {
            if( adjustLeftStop == 1 )
            {
                int leftStop = ui->leftStopValueLabel->text().toInt();

                LOG_DEBUG("leftStop=%d, upperValue=%d, lowerValue=%d", leftStop, upperValue, lowerValue );

                if (leftStop >= upperValue && leftStop <= lowerValue )  // FT-1055
                {
                    //LOG_DEBUG("LEFT STOP PASSED");

                    // set left stop to green color indicating in range
                    ui->leftStopValueLabel->setStyleSheet("background-color: rgb(0, 255, 0); font: 18pt \"MS Shell Dlg 2\";");

                    if( ui->acceptStopsAdjustmentButton->isVisible() )
                        ui->acceptStopsAdjustmentButton->setEnabled( true );
                }
                else
                {
                    // set stop value to red color indicating out of range
                    ui->leftStopValueLabel->setStyleSheet("background-color: rgb(255, 0, 0); font: 18pt \"MS Shell Dlg 2\";");

                    // set weigher tab icon to fail
                    ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->stopsPage),failIcon);
                    weigherStopsPassed = false;

                    ui->acceptStopsAdjustmentButton->setDisabled( true );
                }
            }

            if( adjustRightStop == 1 )
            {
                int rightStop = ui->rightStopValueLabel->text().toInt();

                LOG_DEBUG("rightStop=%d, upperValue=%d, lowerValue=%d", rightStop, upperValue, lowerValue );

                if (rightStop >= upperValue && rightStop <= lowerValue )  // FT-1055
                {
                    //LOG_DEBUG("RIGHT STOP PASSED");

                    // set left stop to green color indicating in range
                    ui->rightStopValueLabel->setStyleSheet("background-color: rgb(0, 255, 0); font: 18pt \"MS Shell Dlg 2\";");

                    ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->stopsPage),passIcon);
                    weigherStopsPassed = true;

                    if( ui->acceptStopsAdjustmentButton->isVisible() )
                        ui->acceptStopsAdjustmentButton->setEnabled( true );
                }
                else
                {
                    // set stop value to red color indicating out of range
                    ui->rightStopValueLabel->setStyleSheet("background-color: rgb(255, 0, 0); font: 18pt \"MS Shell Dlg 2\";");

                    // set weigher tab icon to fail
                    ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->stopsPage),failIcon);
                    weigherStopsPassed = false;

                    ui->acceptStopsAdjustmentButton->setDisabled( true );
                }
            }

            //--------------------------------------------------------
            // Both the Left and Right stop values have to be accepted
            // before the Stops tab can be considered passed
            //--------------------------------------------------------
            if ( adjustLeftStop != 2 || adjustRightStop != 2 )
                status = false;

            if ( !weigherStopsPassed )
                status = status & false;
        }

        //---------------------------------------------------------------------------------------------
        // Weigher Calibration only available with Manufacturer Final Test and Service Diagnostic modes
        //---------------------------------------------------------------------------------------------
        if ( currentManfTestMode == ManufTestFinalTest || currentManfTestMode == ServiceDiagnosticMode )
        {
            if (!calibrationPassed)
            {
                ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->calibrationPage),failIcon);
                status = status & false;
            }
            else
            {
                ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->calibrationPage),passIcon);
            }
        }

        else if ( currentManfTestMode == BoardTest )
        {
            if (!countsTestPassed)
            {
                ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->countsPage),failIcon);
                status = status & false;
            }
            else
            {
                ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->countsPage),passIcon);
            }
        }

        if (status)
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),passIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->weigherTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),failIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->weigherTab ) ] = 0;
        }
    }

    return status;
}

bool manufacturingTest::networkTestStatus()
{
    if( DBG_AUTO_COMPLETE_ALL_TABS || DBG_AUTO_COMPLETE_NETWORK_TAB )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->networkTab ) ] = 1;
        return true;
    }

    bool status = true;

    // SOF-3499
    if( isArkBoardSite )
    {
        if( arkEthernet_ == 1 && arkWifi_ == 1 )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),passIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->networkTab ) ] = 1;
        }
        else
        {

            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),failIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->networkTab ) ] = 0;

            status = false;
        }

        getTestStatus();
    }
    else
    {
        if( currentManfTestMode != ServiceDiagnosticMode )
        {
            //------------------------------------------
            // Always check both Wired and Wireless tabs
            // for the Manufacturing Test modes
            //------------------------------------------

            //------
            // Wired
            //------
            if ((ui->wiredPingAnswerLabel->text() == "Yes"))
            {
                ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wiredTab),passIcon);
            }
            else
            {
                ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wiredTab),failIcon);
                status = status & false;
            }

            if ( !noWirelessNetworking )
            {
                //---------
                // Wireless
                //---------
                if ((ui->wirelessPingAnswerLabel->text() == "Yes"))
                {
                    ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wirelessTab),passIcon);
                }
                else
                {
                    ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wirelessTab),failIcon);
                    status = status & false;
                }
            }
        }
        else
        {
            //----------------------------------------------------
            // For Service Diagnostic mode, only check the tab for
            // the connection type that the scale is setup to use
            //----------------------------------------------------

            if ( dbConnectionType == "0" )
            {
                //------
                // Wired
                //------
                if ((ui->wiredPingAnswerLabel->text() == "Yes"))
                {
                    ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wiredTab),passIcon);
                }
                else
                {
                    ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wiredTab),failIcon);
                    status = status & false;
                }
            }

            if ( dbConnectionType == "1" )
            {
                //---------
                // Wireless
                //---------
                if ((ui->wirelessPingAnswerLabel->text() == "Yes"))
                {
                    ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wirelessTab),passIcon);
                }
                else
                {
                    ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wirelessTab),failIcon);
                    status = status & false;
                }
            }
        }

        //------------
        // Network tab
        //------------
        if (status)
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),passIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->networkTab ) ] = 1;
        }
        else
        {

            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),failIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->networkTab ) ] = 0;
        }
    }

    return status;
}

bool manufacturingTest::flashProgramTestStatus()
{
    if( DBG_AUTO_COMPLETE_ALL_TABS )
    {
        ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),passIcon);
        tabStatus[ ui->testTabsWidget->indexOf( ui->flashTab ) ] = 1;
        return true;
    }

    bool status = true;

    // SOF-1923
    bool ubootOK = true;
    if( needUbootUpgrade && !ubootUpgradeSuccessful )
        ubootOK = false;

    status = status & flashProgrammed & ubootOK;

    // SOF-3449
    if( isArkBoardSite )
    {
        if( ( status ) && ( arkSerialFlash_ == 1 ) && ( kpcSerialFlash_ == 1 ) )
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),passIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->flashTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),failIcon);

            tabStatus[ ui->testTabsWidget->indexOf( ui->flashTab ) ] = 0;
        }

        getTestStatus();
    }
    else
    {
        if (status)
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),passIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->flashTab ) ] = 1;
        }
        else
        {
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),failIcon);

            // FT-39 / FT-1149
            tabStatus[ ui->testTabsWidget->indexOf( ui->flashTab ) ] = 0;
        }
    }

    return status;
}

void manufacturingTest::getTestStatus()
{
    getTestStatusTimer.setSingleShot( true );
    getTestStatusTimer.start( 0 );
}

void manufacturingTest::getTestStatusInternal()
{
    bool status = true;

    switch( currentManfTestMode )
    {
        case ManufTestPreTest:

            status = status & flashProgramTestStatus();
            status = status & dateTimeStatus();
            status = status & usbTestStatus();
            status = status & displayTestStatus();
            status = status & networkTestStatus();
            status = status & soundTestStatus();
            status = status & bluetoothTestStatus();
            status = status & fanTestStatus();
            status = status & printerTestStatus();
            status = status & weigherTestStatus();

            break;

        case ManufTestFinalTest:

            status = status & dateTimeStatus();
            status = status & flashProgramTestStatus();
            status = status & printerTestStatus();
            status = status & weigherTestStatus();

            break;

        case ServiceDiagnosticMode:

            status = status & dateTimeStatus();
            status = status & flashProgramTestStatus();
            status = status & usbTestStatus();
            status = status & displayTestStatus();
            status = status & networkTestStatus();
            status = status & soundTestStatus();
            status = status & bluetoothTestStatus();
            status = status & printerTestStatus();
            status = status & weigherTestStatus();

            break;

        case ManufTestPrepack:

            status = status & dateTimeStatus();
            status = status & flashProgramTestStatus();
            status = status & usbTestStatus();
            status = status & displayTestStatus();
            status = status & networkTestStatus();
            status = status & soundTestStatus();
            status = status & bluetoothTestStatus();
            status = status & canbusTestStatus();

            break;

        case BoardTest:

            if(!isPeripheralBoardTestMode)
            {
                status = status & dateTimeStatus();
                status = status & flashProgramTestStatus();
                status = status & usbTestStatus();
                status = status & displayTestStatus();
                status = status & networkTestStatus();
                status = status & soundTestStatus();
                status = status & bluetoothTestStatus();
                status = status & canbusTestStatus();

                if( !isDepopBoardTestMode )
                {
                    status = status & fanTestStatus();
                    status = status & printerTestStatus();
                    status = status & weigherTestStatus();
                }
            }
            else
            {
                //SOF-3752
                //Atmel peripheral Bd. 00-447780
                status = status & fanTestStatus();
                status = status & printerTestStatus();
                status = status & weigherTestStatus();

            }


            break;
    }

    // Advance to next tab if current tab is complete
    autoWalkThruTabs();
}

void manufacturingTest::initializeAllAnswerLabelValues()
{
    // USB
    setAnswerLabelValue(ui->usbBarcodeScannerRecognizedAnswerLabel,None);
    setAnswerLabelValue(ui->usbMouseRecognizedAnswerLabel,None);

    // Bluetooth
    setAnswerLabelValue(ui->btMouseRecognizedAnswerLabel,None);

    // CANbus
    setAnswerLabelValue(ui->canbusAnswerLabel,None);
    setAnswerLabelValue(ui->sataAnswerLabel,None);
    setAnswerLabelValue(ui->sataAnswerLabel_IO,None);

    setAnswerLabelValue(ui->weigherCountsTestPassedAnswerLabel, None);

    // Printer Labels
    // Removed when Vertical Lines test label added
    //setAnswerLabelValue(ui->upcBarcodePrintAnswerLabel,None);

    // Printer Sensors and Cutter
    setAnswerLabelValue(ui->mediaSensorCalibrationTestAnswerLabel, None);
    setAnswerLabelValue(ui->takenSensorTestPassedAnswerLabel, None);
    setAnswerLabelValue(ui->lblCassetteTestPassedResult, None);
    setAnswerLabelValue(ui->lblLabelCutterTestAnswer, None);

    // Network
    setAnswerLabelValue(ui->wiredPingAnswerLabel,None);
    setAnswerLabelValue(ui->wirelessPingAnswerLabel,None);

    // SOF-3449
    if( isArkBoardSite )
    {
        ui->lblBoardSiteArkRtcAnswerLabel->setText( tr( "No" ) );

        ui->lblBoardSiteArkSerialFlashAnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteKpcSerialFlashAnswerLabel->setText( tr( "No" ) );

        ui->lblBoardSiteArkUsbAnswerLabel->setText( tr( "No" ) );

        ui->lblBoardSiteArkEthernetAnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteKpcWifiAnswerLabel->setText( tr( "No" ) );

        ui->lblBoardSiteArkBtAnswerLabel->setText( tr( "No" ) );

        ui->lblBoardSiteKpcCutterAnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteKpcDotWearAnswerLabel->setText( tr( "No" ) );

        ui->weigherCountsTestPassedAnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteKpcValueMaxAnswerLabel->setText( tr( "No" ) );

        ui->lblBoardSiteArkUsdSlot0AnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteArkUsdSlot1AnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteArkMemoryAnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteArkCan0AnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteKpcCanAnswerLabel->setText( tr( "No" ) );
        ui->lblBoardSiteKpcModelAnswerLabel->setText( tr( "No" ) );
    }
}

// FT-38
void manufacturingTest::initializeAllRadioButtons()
{
    // Date/Time
    ui->rbDateTimeCorrectAnswerYes->setChecked( false );
    ui->rbDateTimeCorrectAnswerNo->setChecked( false );

    // Display->Touch screen calibration
    ui->rbTouchScreenAnswerYes->setChecked( false );
    ui->rbTouchScreenAnswerNo->setChecked( false );

    // Display->Operator - White / Black & Color Bar / Gray screen test
    ui->rbOperatorDisplayAnswerYes->setChecked( false );
    ui->rbOperatorDisplayAnswerNo->setChecked( false );

    // Display->Customer - White / Black & Color Bar / Gray screen test
    ui->rbCustomerDisplayAnswerYes->setChecked( false );
    ui->rbCustomerDisplayAnswerNo->setChecked( false );

    // USB -> Self Service Embedded BarCode Scanner (does this model have it)
    ui->rbSelfServiceAnswerNo->setChecked( false );
    ui->rbSelfServiceAnswerYes->setChecked( false );

    // USB -> Self Service Embedded BarCode Scanner (test power on/off)
    ui->rbSelfServiceScannerPromptAnswerNo->setChecked( false );
    ui->rbSelfServiceScannerPromptAnswerYes->setChecked( false );
    ui->rbSelfServiceScannerPromptAnswerRetry->setChecked( false );

    // Audio->Beeper test
    ui->rbBeeperTestAnswerYes->setChecked( false );
    ui->rbBeeperTestAnswerNo->setChecked( false );

    // Fan
    ui->rbFanSpinningTestAnswerYes->setChecked( false );
    ui->rbFanSpinningTestAnswerNo->setChecked( false );

    // Printer->Test Label - Checkerboard Test Pattern
    ui->rbCheckerBoardPatternTestAnswerYes->setChecked( false );
    ui->rbCheckerBoardPatternTestAnswerNo->setChecked( false );

    // Printer->Test Label - Vertical Line Test Pattern
    ui->rbVerticalLinePatternTestAnswerYes->setChecked( false );
    ui->rbVerticalLinePatternTestAnswerNo->setChecked( false );
}

void manufacturingTest::initializeAnswers()
{
    initializeAllAnswerLabelValues();
}

bool manufacturingTest::onScreenChanging()
{
    SaveDataOnExit();
    changingScreens = true;
    return _Mybase::onScreenChanging();
}

// FT-1328: Had to put reboot back in, due to need to enable openbox again
// FT-1330: Remove reboot after setting date/time in Manufacturing Test
// FT-1271
bool manufacturingTest::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    LOG_CLASS_INFO( "manufacturingTest", "isErrorMessageHandler" );

    // Message that the Service key has been removed
    //103,255,120080
    if ( Error == errorMsg[0].toInt() && ErrRemoveAnyModalDialogDevice == errorMsg[1].toInt() && 120080 == errorMsg[2].toInt() && 3 == errorMsg[3].toInt())
    {
        return true;
    }

    // SOF-2708
    if ( ( Error == errorMsg[0].toInt() ) &&
         ( ErrRemoveAnyModalDialogDevice == errorMsg[1].toInt() ) &&
         ( 120026 == errorMsg[2].toInt() ) )
    {
        //-----------------------------------------------------------------------------------
        // Message that "Weigher/Printer Code Download Complete" is dismissed: 103,255,120026
        //-----------------------------------------------------------------------------------

        LOG_DEBUG( "isErrorMessageHandler(): Received 103,255,120026 message", NULL );

        session_->send( PrinterSystem, PrSysReqStatus, "0" );

        // Request Peripheral Controller Version Info again so the correct & newest version is displayed on the
        // Summary Report label. The FPC code will be automatically installed at some point during first boot.

        LOG_DEBUG( "Requesting Peripheral Controller Version after FPC code updated. ", NULL );
    }

    return _Mybase::isErrorMessageHandler( errorMsg );
}

void manufacturingTest::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "manufacturingTest", "onSessionMessage" );

    LOG_DEBUG( "START onSessionMessage()", NULL );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    int mediaSensorValue(0);
    if(msg.length() >= 8)
    {
        mediaSensorValue = msg[7].toInt();
    }

    // Printer Device message
    if (Printer == device )
    {
        LOG_DEBUG( "onSessionMessage PRINTER message", NULL );

        if( PrCutterStatus == event )
        {
            LOG_DEBUG( "onSessionMessage PRINTER + PrCutterStatus message", NULL );

            DomPrinterCutterStatus m( msg );

            cutterAttached_ = m.installed();        // based on a wire - physical presence, its there or not
            interlockEngaged_ = m.interlock();      // Used in SOF-5595

            if( cutterAttached_ )
            {
                LOG_DEBUG( "onSessionMessage PrCutterStatus msg CUTTER ATTACHED", NULL );

                showCutterTab();

                // SOF-3853
                if( !isSSKPrinterAttached() )
                    removeMediaSensorCalibration();

                if( cutter_fired_ )
                {
                    // UPDATE ( Printer, PrReqCutterStatus, 0,1,1,0 )       // home bit 0 so cutter not home
                    // UPDATE ( Printer, PrReqCutterStatus, 1,1,1,0 )       // home bit 1 so cutter returned home
                    // UPDATE ( Printer, PrReqCutterStatus, 0,1,1,1 )       // jam bit 1, not returned home

                    if( (cutter_state_ == CutterHome) && ( !m.home() ) )
                    {
                        // we were at home but cutter left home to cut
                        LOG_DEBUG( "onSessionMessage PrCutterStatus CutterLeftHome && has not returned yet", NULL );
                        cutter_state_ = CutterLeftHome;
                    }
                    else if( (cutter_state_ == CutterHome) && ( m.home() ) )
                    {
                        // means we have sent cut command but it has not started cutting yet
                        LOG_DEBUG( "onSessionMessage PrCutterStatus CutterHome and still home", NULL );
                    }
                    else if( (cutter_state_ == CutterLeftHome) && ( m.home() ) )
                    {
                        // means we have returned home after cutting and the test passes
                        cutter_fired_ = false;
                        cutter_state_ = CutterHome;
                        ui->lblLabelCutterTestAnswer->setText( "Yes" );
                        LOG_DEBUG( "onSessionMessage PrCutterStatus CutterHome after cutting", NULL );
                    }
                    else if( m.jammed() )
                    {
                        // means we are jammed. Could of been jammed to start or while testing. Report it
                        cutter_fired_ = false;
                        cutter_state_ = CutterHome;
                        ui->lblLabelCutterTestAnswer->setText( "Jammed" );
                    }
                }
                else
                {
                    // must of been initital request for status

                    if( m.home() )
                        cutter_state_ = CutterHome;

                    ui->lblIsCutterAttachedResult->setText(tr("Yes"));
                    ui->lblCutterStatus->setEnabled(true);
                    ui->lblCutterStatusResult->setEnabled(true);
//                    ui->butEnableCutter->setEnabled(true);

                    if( cutterEnabled_ )
                    {
                        ui->lblCutterStatusResult->setText(tr("Enabled"));
                        ui->butEnableCutter->setText(tr("Disable"));
                    }
                    else
                    {
                        ui->lblCutterStatusResult->setText(tr("Disabled"));
                        ui->butEnableCutter->setText(tr("Enable"));
                    }
                }
            }
            else
            {
                // SOF-3166
                removeCutterTab();
            }

            if( shootThroughCalStarted && m.interlock() && !printheadDisengaged_ )
            {
                if(calInitDialog == activeDialog)
                {

                    calInitDialog->accept();
                    delete calInitDialog;

                    calDialog = new MessageBoxDialog( mainWindow_ );
                    calDialog->setStandardButtons( QDialogButtonBox::Cancel );
                    calDialog->setTitle( "BACKING PAPER ONLY" );
                    calDialog->setText( tr( "Calibrating..." ) );
                    calDialog->setAllowOverlayClickDismiss(false);

                    QAbstractButton *cancelButton = calDialog->button(QDialogButtonBox::Cancel);
                    connect(cancelButton, SIGNAL(clicked()), SLOT(on_ShootThroughCanceled()) );

                    activeDialog = calDialog;

                    calDialog->open();
                 }

                if(calBackingDialog == activeDialog)
                {
                    calBackingDialog->accept();
                    delete calBackingDialog;

                    calDialog = new MessageBoxDialog( mainWindow_ );
                    calDialog->setStandardButtons( QDialogButtonBox::Cancel );
                    calDialog->setTitle( "BACKING PAPER AND LABEL" );
                    calDialog->setText( tr( "Calibrating..." ) );
                    calDialog->setAllowOverlayClickDismiss(false);

                    QAbstractButton *cancelButton = calDialog->button( QDialogButtonBox::Cancel );
                    connect(cancelButton, SIGNAL(clicked()), SLOT(on_ShootThroughCanceled()) );

                    activeDialog = calDialog;

                    calDialog->open();
                }

                shootThroughCalStarted = false;

                // SOF-5751
                if ( !trackStockMetrics_ )
                    session_->send( Printer, PrGapCalNext );
            }
        }
        else if ( PrStockMetrics == event )
        {

            DomPrinterStockMetrics m( msg );
            bool ok;

            // SOF-5643: check to see if the printer sized diecut correctly after the sensor calibration
            // Ignore clearLabelDimensions PrStockMetrics message on FS
            if ( trackStockMetrics_  && m.stockWidthInDots() > 0)
            {
                m.stockLength().toDouble(&ok);

                ui->mediaSensorCalibrationTestAnswerLabel->hide();
                ui->mediaSensorCalibrationTestAnswerLabel->setText("Yes");

                if (ok)
                {
                    // physical prescence but uncalibrated sensor
                    if ( (int) m.stockLength().toDouble() == 0)
                        ui->mediaSensorCalibrationTestAnswerLabel->setText("No - continuous stock sized instead of die-cut.");
                }

                // no physical prescence of sensor
                else if ( m.stockLength() == "continuous")
                    ui->mediaSensorCalibrationTestAnswerLabel->setText("No - continuous stock sized instead of die-cut.");


                ui->mediaSensorCalibrationTestAnswerLabel->show();
                trackStockMetrics_ = false;
            }
            // End SOF-5643
        }
        else if (PrSensors == event )
        {
            DomPrinterMonitorSensors m( msg );

            //------------------------
            //-- Label taken sensor --
            //------------------------
            QString labelSensorReading = m.labelTakenSensor();
            int isBlocking;

            if ( m.isPrintheadDisengaged() == "0" )
                printheadDisengaged_ = false;
            else
            {
                printheadDisengaged_ = true;

                // we have started to swap label stock and we've disengaged the printhead
                if (swapLabelStockRoutineStarted_)
                {
                    hasBeenDisengaged_ = true;
                }
            }

            if(labelSensorReading.toInt() >= 250)
                // Not Blocked
                isBlocking = 0;
            else
                isBlocking = 1;

            // Once we have started the Test Pattern, need to make sure we see a transition of the Label Taken Sensor
            //   (i.e. see a Blocked Label Taken Sensor)
            if( checkLabelTakenSensorMode )
            {
                if( isBlocking )
                {
                    // Saw the label sensor change state - Now it's OK to check for it being set to unblocked
                    lastLabelSensorMessageBlocked = true;
                }
            }

            LOG_DEBUG("isBlocking = %d", isBlocking );
            LOG_DEBUG("checkSensorUnblockedBeforeTestPattern = %d", checkSensorUnblockedBeforeTestPattern );
            LOG_DEBUG("checkLabelTakenSensorMode = %d", checkLabelTakenSensorMode );

            if( checkSensorUnblockedBeforeTestPattern )
            {
                // Do not send the message for the test pattern until the label taken sensor is not blocked
                if( !isBlocking )
                {
                    checkSensorUnblockedBeforeTestPattern = false;

                    // Now ready to check for the Label Taken sensor being unblocked after the Test Pattern
                    checkLabelTakenSensorMode = true;

                    // Now send the Test Pattern message
                    QString msg;
                    // msg = "0,4,0";  // FT-1055 Print Vertical Lines test pattern
                    msg = "0,6,0";  // SOF-3362 Print NEW Manufacturing test label with barcode
                    session_->send( Printer, PrTestLabel, msg );
                    LOG_DEBUG("Send start label test pattern message", NULL );
                }
            }
            else if( checkLabelTakenSensorMode )
            {
                // First make sure we have seen a blocked Label Taken Sensor since the test label was printed
                if( lastLabelSensorMessageBlocked )
                {
                    if( !isBlocking )
                    {
                        ui->takenSensorTestPassedAnswerLabel->setText( "Yes" );

                        // SOF-5997: compare the shoot through sensor value to ceiling of 170

                        if (currentManfTestMode == ManufTestFinalTest && isSelfServiceScale("FS") )
                        {
                            int currentValue = m.mediaSensor().toInt();

                            if (currentValue >= SHOOT_THROUGH_MAXVALUE || currentValue <= SHOOT_THROUGH_MINVALUE)
                            {
                                QString comparison_ = currentValue >= SHOOT_THROUGH_MAXVALUE ? "greater" : "less";

                                ceilingComparisonLabel->setText("Shoot Through Sensor Value OK?           No");
                                ceilingComparison->setText("Current Media reading " + comparison_ + " is not in range of 40-200; sensor test failed.");
                                shootThroughValueOK = false;
                            }

                            else
                            {
                                ceilingComparisonLabel->setText("Shoot Through Sensor Value OK?           Yes");
                                ceilingComparison->setText("Current Media reading is in range of 40-200; sensor test passed.");
                                shootThroughValueOK = true;
                            }

                            ceilingComparison->show();

                        }
                        // End SOF-5997

                        checkLabelTakenSensorMode = false;
                    }
                }
            }

            // SOF-5595 - monitor printhead and interlock to dismiss swapToLinerlessDialog dialog
            else if (swapLabelStockRoutineStarted_)
            {

                LOG_DEBUG("Swapping label stock - monitoring for interlock and printhead engaged", NULL);

                if ( ( hasBeenDisengaged_ && !printheadDisengaged_ ) && interlockEngaged_  && swapToLinerlessDialog->isVisible() )
                {

                    swapToLinerlessDialog->accept();
                    swapToLinerlessDialog = NULL;
                    hasBeenDisengaged_ = false;
                    swapLabelStockRoutineStarted_ = false;
                    trackLabelStockSensorValue_ = false;


                    QString labelStockResult;

                    // check the label stock readings
                    if (lowHighReadings.size() <= 1 )
                    {
                        labelStockSensorPassed_ = false;
                        labelStockResult = "Failed. No high/low readings detected while\nswapping to linerless stock.";
                    }
                    else
                    {
                        labelStockSensorPassed_ = true;
                        labelStockResult = "Passed. High/low readings were detected \nwhile swapping to linerless stock.";
                    }

                    lowHighReadings.clear();
                    ui->labelStockSensorTestResult->setText(labelStockResult);
                }
                else
                    session_->send( Printer, PrSensorsTracking, "0,1" );

            }
            //Media sensor value display
            ui->lblMediaSensorDisplay->setText(QString("Media: %1").arg(mediaSensorValue));


            std::bitset<sizeof(int)> regTemp ( m.regulatorTemp() );

            if ( trackLabelStockSensorValue_)
                ui->lblLowStockData->setText( QString::number( regTemp[1] ) );

            /*
             * 3/29/23: Per Jim, monitor for high/low on label stock sensor during swap to linerless
             * If we do not receive a high/low reading during this time, do not pass the test
             */

            if (swapLabelStockRoutineStarted_)
            {
                lowHighReadings.insert(regTemp[1]);
            }
            //

            QString isPrintHeadEngaged = m.isPrintheadDisengaged();
            QString isPrintHeadEngagedText;

            switch(isPrintHeadEngaged.toInt())
            {
                case 0:
                    isPrintHeadEngagedText = tr( "Engaged" );
                    break;

                case 1:
                    isPrintHeadEngagedText = tr( "Disengaged" );
                    break;

                default:
                    isPrintHeadEngagedText = tr( "Unknown" );
                    break;
            }
            ui->lblHeadUpData->setText( isPrintHeadEngagedText );
            printerTestStatus();

        }
        else if ( PrTestCassette == event )
        {
            // SOF-3763 - Test Printer Cassette

            LOG_DEBUG( "onSessionMessage PRINTER + PrTestCassette message", NULL );

            DomPrCassetteStatus m( msg );

            ui->lblMeasuredLinesValue->setText( m.measuredLines() );
            ui->lblActualLinesValue->setText( m.actualLines() );

            if( m.testResult() == "1" )
                ui->lblCassetteTestPassedResult->setText( "Yes" );
            else
                ui->lblCassetteTestPassedResult->setText( "No" );
        }
        else if ( PrResetFactoryDefaultsComplete == event ) // 7,58
        {
            // SOF-1719
            DomPrinterResetFactoryDefaultsStatus m( msg );

            printerResetSuccessful = ( m.resetStatus() == "1" ? true : false );
            confirmPrinterReset();  // exit the wait loop

            LOG_DEBUG("PrinterResetFactoryDefaults: Result=%s", ( printerResetSuccessful ? "SUCCESS" : "FAILED" ) );
        }
        else if ( PrHeadDotStatus == event ) // 7,179
        {
            //--------------------------------
            // SOF-3294: Validate Code Checker
            //--------------------------------
            DomPrHeadDotStatus m( msg );
            haveDotWearTestData = true;

            if( m.status() == 0 )
            {
                LOG_DEBUG("Code Checker Status: PASSED, Status=0", NULL );
                dotWearTestPassed = true;
            }
            // Dot Wear failed
            else
            {
                LOG_DEBUG("Code Checker Status: FAILED, Status=%d", m.status() );
            }

            dotWearTestFinished();
        }
        else if( PrHeadDotWear == event ) // 7,175
        {
            //----------------------------------------------------------------------
            // SOF-3584: Need to determine if there were any BAD dots.
            //           Only fail the print head for a BAD dot, not a marginal dot.
            //----------------------------------------------------------------------
            DomPrHeadDotWear m( msg );

            QString statusListStr = m.dotsStatusList();

            for( int loop = 0; ( loop < m.dotsSizeList().toInt() ) && ( !haveBadDot ) ; ++loop )
            {
                if( statusListStr.at( loop ) == '1' )
                    haveBadDot = true;
            }

            haveHeadDotWearDetailData = true;

            LOG_DEBUG("PrHeadDotStatus, haveBadDot=%d", haveBadDot );
        }

        else if( PrGapCalStatus == event )
        {
            /*
            float deflectionVoltage = msg[3].toFloat();
            float driveCurrent = msg[4].toFloat();
            float backingVoltage = msg[5].toFloat();
            float labelVoltage = msg[6].toFloat();

            ui->lblDeflectionVoltageValue->setText( QString::number( deflectionVoltage, 'f', 2 ) );
            ui->lblDriveCurrentValue->setText( QString::number( driveCurrent, 'f', 2 ) );
            ui->lblBackingVoltageValue->setText( QString::number( backingVoltage, 'f', 2 ) );
            ui->lblLabelVoltageValue->setText( QString::number( labelVoltage, 'f', 2 ) );
            */
            int state = msg[2].toInt();

            if (shootThroughRoutineCanceled && ( state != _FailureMemory && state != _CalInit ) )
                return;

            switch( state )
            {
                case _CalInit:
                {
                    shootThroughRoutineCanceled = false;
                    ui->startMediaSensorTestButton->setEnabled( true );

                    shootThroughCalStarted = true;
                    ui->lblStatusValue->setText( "Cal Init" );

                    calibrationSteps.clear();
                    calibrationSteps.append("Open interlock door\n");
                    calibrationSteps.append("Disengage the printhead\n");
                    calibrationSteps.append("Position backing paper ONLY under the shoot through (gap) sensor\n");
                    calibrationSteps.append("ENGAGE the printhead\n");
                    calibrationSteps.append("Close the interlock door");

                    calInitDialog = new MessageBoxDialog( mainWindow_ );
                    calInitDialog->setAllowOverlayClickDismiss( false );
                    calInitDialog->setTitle( "BACKING PAPER ONLY" );

//                    calInitDialog->setText( tr( "Open interlock door, disengage the printhead, and position backing paper ONLY under the shoot through (gap) sensor."
//                                                "\n\nThen engage the printhead and close door." ) );
                    calInitDialog->setStandardButtons( QDialogButtonBox::Cancel );

                    QAbstractButton *cancelButton = calInitDialog->button( QDialogButtonBox::Cancel );
                    connect(cancelButton, SIGNAL(clicked()), SLOT(on_ShootThroughCanceled()) );

                    activeDialog = calInitDialog;

                    initShootThroughDialogText( calInitDialog );

                    calInitDialog->open();

                    break;
                }
                case _BackingPaper:
                {

//                    session_->send( Printer, PrSensorsTracking, "0,0" );

                    ui->startMediaSensorTestButton->setEnabled( true );

                    ui->lblStatusValue->setText( "Backing Paper" );
                    ui->startMediaSensorTestButton->setText( "Next" );
                    shootThroughCalStarted = true;

                    if ( calDialog == activeDialog )
                    {
                        calDialog->accept();
                        delete calDialog;

                        calBackingDialog = new MessageBoxDialog (mainWindow_);

                        calibrationSteps.clear();
                        calibrationSteps.append("Open interlock door\n");
                        calibrationSteps.append("Disengage the printhead\n");
                        calibrationSteps.append("Position the center of a label under the shoot through (gap) sensor\n");
                        calibrationSteps.append("ENGAGE the printhead\n");
                        calibrationSteps.append("Close interlock door");

                        calBackingDialog->setStandardButtons( QDialogButtonBox::Cancel );
                        calBackingDialog->setTitle( "BACKING PAPER AND LABEL" );
                        calBackingDialog->setAllowOverlayClickDismiss( false );

                        QAbstractButton *cancelButton = calBackingDialog->button( QDialogButtonBox::Cancel );
                        connect(cancelButton, SIGNAL(clicked()), SLOT(on_ShootThroughCanceled()) );

                        activeDialog = calBackingDialog;

                        initShootThroughDialogText( calBackingDialog );

//                        calBackingDialog->setText( tr( "Open interlock door, disengage the printhead, and position backing paper AND label under the shoot through (gap) sensor."
//"                                                   \n\nThen engage the printhead and close interlock door." ) );

                        calBackingDialog->open();
                    }

                    break;
                }
                case _LabelPlusBacking:
                {
//                    session_->send( Printer, PrSensorsTracking, "0,0" );

                    ui->startMediaSensorTestButton->setEnabled( true );

                    ui->lblStatusValue->setText( "Label + Backing Paper" );
                    shootThroughCalStarted = true;

                    if ( calDialog )
                    {
                        calDialog->accept();
                        delete calDialog;
                    }

                    MessageBoxDialog calDoneDialog( mainWindow_ );
                    calDoneDialog.setTitle( "Position Paper" );
                    calDoneDialog.setText( tr( "Shoot Through Calibration Done, printer will now size labels.\nHit \"Ok\"" ) );
                    calDoneDialog.setStandardButtons( QDialogButtonBox::Ok );
                    calDoneDialog.exec();

                    session_->send( Printer, PrGapCalNext );

                    trackStockMetrics_ = true;
                    break;
                }
                case _CalcDeflections:
                {
                    session_->send( Printer, PrSensorsTracking, "0,1" );

                    ui->lblStatusValue->setText( "Calc Deflections" );

                    break;
                }
                case _FailureSettingI:
                {
                    ui->lblStatusValue->setText( "Failure Setting I" );
                    ui->mediaSensorCalibrationTestAnswerLabel->setText( "No" );
                    session_->send( Printer, PrGapCalNext );

                    break;
                }
                case _FailureMemory:
                {
                    /* In the context of cancelling and restarting, this "error" occurs because the firmware has to free the memory used for the calibration first
                    and it sends this status message

                    */

                    if (shootThroughRoutineCanceled)
                    {
                        session_->send(Printer, PrStartGapCalibration);
                        shootThroughRoutineCanceled = false;
                    }
                    else
                    {
                        ui->lblStatusValue->setText( "Failure Memory" );
                        ui->mediaSensorCalibrationTestAnswerLabel->setText( "No" );
                        session_->send( Printer, PrGapCalNext );
                    }

                    break;
                }
                case _Done:
                {
                    shootThroughCalStarted = false;
                    shootThroughRoutineCanceled = false;

                    ui->lblStatusValue->setText( "Done" );

                    /* SOF-5643: defer this to later check
                    ui->mediaSensorCalibrationTestAnswerLabel->setText( "Yes" );
                    */

                    ui->startMediaSensorTestButton->setEnabled( true );
                    ui->startMediaSensorTestButton->setText( "Calibrate Shoot Through" );

                    // need to size label after shoot through cal to calibrate label taken
                    ui->btnSizeLabels->setEnabled( false );
                    ui->startLabelTakenSensorTestButton->setEnabled( true );

                    break;
                }
            }
        }
    }

    else if (Error == device)
    {
        if (ErrWeigherDevice == event)
        {
            returnedErrorsWeigherDev m(msg);

            if (1012 == m.errorCode())
            {
                calibrationPassed = true;
            }
        }

        // see if label taken sensor calibration passed
        if (ErrPrinterDevice == event)
        {
            returnedErrorsPrinterDev m(msg);

            if (7020 == m.errorCode())
            {
                /* Defer to later check
                    ui->mediaSensorCalibrationTestAnswerLabel->setText("Yes");
                */

                ui->lblMediaSensorDisplay->setText("Media: ");
                ui->lblMediaSensorDisplay->setVisible(false);

                // FT-647
                // Printer Sensor OFF
                session_->send( Printer, PrSensorsTracking, "0,0" );

                trackStockMetrics_ = true;
            }
        }

        // FT-1328: Had to put reboot back in, due to need to enable openbox again
        // FT-1330: Remove reboot after setting date/time in Manufacturing Test
        // FT-1271
        if ( ErrRemoveAnyModalDialogDevice == msg[1].toInt( ))
        {
            returnedErrorsRemoveAnyModalDialogDev m(msg);

            if ( ( 120080 == m.errorCode() ) && ( 3 == m.errorData() ) )
            {
                LOG_DEBUG("Received message that there is no service key", NULL );

                // OK to save the Date/Time and reboot
                saveDateTimeAfterServiceKeyRemoved();
            }
        }
    }

    else if (Weigher == device)
    {
        if (WgStatusCounts == event)
        {
            // Weigher Stops
            DomWeigherInfoCountsRead m(msg);

            // FT-735
            if( adjustLeftStop == 1 )
                ui->leftStopValueLabel->setText(m.zeroes());

            if( adjustRightStop == 1 )
                ui->rightStopValueLabel->setText(m.zeroes());

            if (currentManfTestMode == BoardTest)
            {
                if (lastReadCount == 0)
                    lastReadCount = ui->loadCellCountsLineEdit->text().toInt();

                // update the load cell counts screen
                ui->loadCellCountsLineEdit->setText(m.zeroes());

                if ((ui->loadCellCountsLineEdit->text().toInt() - lastReadCount) > 50)
                {
                    countsTestPassed = true;
                    setAnswerLabelValue(ui->weigherCountsTestPassedAnswerLabel, Yes);
                }
            }

            if( isHtmWeigher )
            {
                if(valueMaxIsEnabled_)
                {
                    xTilt.sprintf( "%.2f", m.tiltX().toDouble() / 100.0 );
                    yTilt.sprintf( "%.2f", m.tiltY().toDouble() / 100.0 );
                }
                else
                {
                    xTilt = "N/A";
                    yTilt = "N/A";
                }
            }
        }

        // SOF-3244: Grab the FIT parameters
        else if ( WgValueMaxFitParams == event )
        {
            DomWeigherFitParamsRead m( msg );

            gravity_ = m.gravity();
            m_LbsCountsConvFactor_ = m.m_LbsCountsConvFactor();
            zeroValue_ = m.zeroValue();
            spanValue_ = m.spanValue();
            z0ElectronicOffset_ = m.z0ElectronicOffset();
            deadWeight_ = m.deadWeight();
            x2_ = m.x2();
            x1_ = m.x1();
            x0_ = m.x0();
            y2_ = m.y2();
            y1_ = m.y1();
            y0_ = m.y0();
            xRot_0_ = m.xRot_0();
            xRot_1_ = m.xRot_1();
            xRot_2_ = m.xRot_2();
            yRot_0_ = m.yRot_0();
            yRot_1_ = m.yRot_1();
            yRot_2_ = m.yRot_2();
            zRot_0_ = m.zRot_0();
            zRot_1_ = m.zRot_1();
            zRot_2_ = m.zRot_2();
            xGain_ = m.xGain();
            yGain_ = m.yGain();
            gCoeff_ = m.gCoeff();
            checkSum_ = m.checkSum();

            LOG_DEBUG("ValueMax FIT Parameters:", NULL );
            LOG_DEBUG("Gravity = %ld", gravity_ );
            LOG_DEBUG("Pounds/Counts Conv Factor = %ld", m_LbsCountsConvFactor_ );
            LOG_DEBUG("Zero value = %ld", zeroValue_ );
            LOG_DEBUG("Span value = %ld", spanValue_ );
            LOG_DEBUG("Electronic offset = %ld", z0ElectronicOffset_ );
            LOG_DEBUG("Dead weight = %ld", deadWeight_ );
            LOG_DEBUG("x2 = %ld", x2_ );
            LOG_DEBUG("x1 = %ld", x1_ );
            LOG_DEBUG("x0 = %ld", x0_ );
            LOG_DEBUG("y2 = %ld", y2_ );
            LOG_DEBUG("y1 = %ld", y1_ );
            LOG_DEBUG("y0 = %ld", y0_ );
            LOG_DEBUG("X-Rotate-0 = %ld", xRot_0_ );
            LOG_DEBUG("X-Rotate-1 = %ld", xRot_1_ );
            LOG_DEBUG("X-Rotate-2 = %ld", xRot_2_ );
            LOG_DEBUG("Y-Rotate-0 = %ld", yRot_0_ );
            LOG_DEBUG("Y-Rotate-1 = %ld", yRot_1_ );
            LOG_DEBUG("Y-Rotate-2 = %ld", yRot_2_ );
            LOG_DEBUG("Z-Rotate-0 = %ld", zRot_0_ );
            LOG_DEBUG("Z-Rotate-1 = %ld", zRot_1_ );
            LOG_DEBUG("Z-Rotate-2 = %ld", zRot_2_ );
            LOG_DEBUG("X-Gain factor = %ld", xGain_ );
            LOG_DEBUG("Y-Gain factor = %ld", yGain_ );
            LOG_DEBUG("Gravity coefficient = %ld", gCoeff_ );
            LOG_DEBUG("Checksum = %ld", checkSum_ );


            //SOF-5608: only validate load cell when receiving accurate fit params
            if ( checkFITParams &&  ( x2_ != 0 || gravity_ != 0 ) )
            {
                checkFITParams = false;
                waitingForValueMax = false;
                checkLoadCellCalibration();
            }
            else
            {
                session_->send(Weigher, WgReqValueMax, "4");
            }
        }

        // FT-1223
        // Get Weigher config settings and save them
        else if ( WgInquire == event )
        {
            DomWeigherConfigRead m( msg );

            weigherModel = m.weigherModel();
            weigherFilterSpeed = m.weigherFilterSpeed();
            weigherScaleFactor = m.weigherScaleFactor();
            weigherMaximumWt = m.weigherMaximumWt();
            weigherMininumWt = m.weigherMininumWt();
            netWeightDRLimitCheck = m.netWeightDRLimitCheck();
            dualRangeLimit = m.dualRangeLimit();
            isScaleValuemax = m.isScaleValuemax();
            gcdLanguage = m.gcdLanguage();

            valueMaxIsEnabled_ = m.isScaleValuemax(); 
        }
        // SOF-1296
        else if ( WgResetFactoryDefaultsComplete == event ) // 1,23
        {
            DomWeigherResetFactoryDefaultsStatus m( msg );

            weigherResetSuccessful = ( m.resetStatus() == "1" ? true : false );
            confirmWeigherReset();  // exit the wait loop

            LOG_DEBUG("WeigherResetFactoryDefaults: Result=%s", ( weigherResetSuccessful ? "SUCCESS" : "FAILED" ) );
        }
    }

    else if ( MfgTestDev == device )
    {
        // Flash programming passed as reported by backend
        if ( event == WriteOfMfgDataPassed )
        {
            ui->testTabsWidget->setTabIcon( ui->testTabsWidget->indexOf(ui->flashTab), passIcon );
            flashProgrammed = true;

            LOG_INFO("Programming Flash PASSED (WriteOfMfgDataPassed)", NULL );
        }

        // Flash programming failed as reported by backend
        if ( event == WriteOfMfgDataFailed )
        {
            ui->testTabsWidget->setTabIcon( ui->testTabsWidget->indexOf(ui->flashTab), failIcon );
            flashProgrammed = false;

            LOG_INFO("Programming Flash FAILED (WriteOfMfgDataFailed)", NULL );
        }
    }

    // USB Device message
    else if ( HIDDev == device )
    {
        // USB Barcode Scanner attached
        if ( event == HIDDevBarcodeScannerAttached )
        {
            ui->usbBarcodeScannerRecognizedAnswerLabel->setText("Yes");
        }
        // USB Barcode Scanner removed
        else if ( event == HIDDevBarcodeScannerRemoved )
        {
            ui->usbBarcodeScannerRecognizedAnswerLabel->setText("No");
        }

        // USB Keyboard Attached
        else if ( event == HIDDevKeyboardAttached )
        {
            ui->usbBarcodeScannerRecognizedAnswerLabel->setText("Yes");
        }
        // USB Keyboard Removed
        else if ( event == HIDDevKeyboardRemoved )
        {
            ui->usbBarcodeScannerRecognizedAnswerLabel->setText("No");
        }

        // USB Mouse Attached
        else if ( event == HIDDevMouseAttached )
        {
            ui->usbMouseRecognizedAnswerLabel->setText("Yes");
        }
        // USB Mouse Removed
        else if ( event == HIDDevMouseRemoved )
        {
            ui->usbMouseRecognizedAnswerLabel->setText("No");
        }
    }

    // FT-1423
    // Version Info
    if (( SessionDev == device ) && ( SnVersionInfo == event ))
    {
        LOG_DEBUG( "got version info...", NULL );
        DomCurrentScaleSetup m( msg );

        applicationVer = m.version();
        ui->applicationVersionLabel->setText(m.version());
    }

    // Peripheral Controller Version Info
    else if (( PrinterSystem == device ) && ( PrSysStatus == event ))
    {
        LOG_DEBUG( "got peripheral controller version info...", NULL );
        DomPrinterPowerupTests m( msg );

        periphControllerVer = m.softwareVersion();

        LOG_DEBUG( "OnSessionMessage: periphControllerVer (Printer)=%s", periphControllerVer.toLatin1().constData() );
    }

    else if( ( WeigherSystem == device ) && ( SysUsbWgStatus == event ) )
    {
        DomUsbWeigherPowerupTests m( msg );

        periphControllerVerWeigher = m.softwareVersion();

        LOG_DEBUG( "Got peripheral controller version info (Weigher): %s", periphControllerVerWeigher.toLatin1().constData() );
    }


    // Loader Version
    else if (( MfgTestDev == device ) && ( ReadLoaderVersion == event ))
    {
        LOG_DEBUG( "got loader version...", NULL );
        DomMfgReadLoaderVersion m( msg );

        loaderVer = m.version();

        LOG_DEBUG( "OnSessionMessage: loaderVer=%s", loaderVer.toLatin1().constData() );
    }

//    else if ( SessionDev == msg[0].toInt() && SnWriteDateTime == msg[1].toInt(0))
//    {
//        // Date and Time ... Update the date and time on the Date/Time tab
//        setDateTimeLabel();
//    }
    else if (SessionDev == msg[0].toInt()  && SnWriteDateTime == msg[1].toInt(0))
    {
        // 100,6
        //DEFINE_FIELD( int, hobYear, setHobYear, hobYearRaw, 2 )
        //DEFINE_FIELD( int, hobMonth, setHobMonth, hobMonthRaw, 3 )
        //DEFINE_FIELD( int, hobDay, setHobDay, hobDayRaw, 4 )
        //DEFINE_FIELD( int, hobHour, setHobHour, hobHourRaw, 5 )
        //DEFINE_FIELD( int, hobMinute, setHobMinute, hobMinuteRaw, 6 )
        //DEFINE_FIELD( QString, hobAMPM, setHobAMPM, hobAMPMRaw, 7 )

        QString text( session_->uiUtilities->updateDateTime() );
        //if()

        if ( convertUppercase_ )
            text = text.toUpper();

        //ui->CurrentDateTimeTimeLabel->setText(text);
        //this->setDateTimeLabel();

        if( setDateTimeSuccessful )
            this->ui->lblCurrentDateTimeData->setText(text);
    }
    else if (( ReportManagerDev == device ) && ( TerminateReport == event )) // 116,2
    {
        //-----------------------------------------------------------------------
        // SOF-4020
        // Wait for confirmation that the Manufacturing Test report has completed
        //-----------------------------------------------------------------------

        LOG_DEBUG("Received Manufacturing-Test-Report-Complete", NULL );

        reportTimerDone = true;
    }

    // FT-1223
    //------------------------------------------------------
    // Processes message for allowing printing without a GCD
    //------------------------------------------------------
    if (( ConfigurationDev == device ) && ( CfgWriteSingleField == event ))
    {
        DomConfigWriteSingleField m( msg );

        // FT-1223
        //------------------------------------------------------
        // Processes message for allowing printing without a GCD
        //------------------------------------------------------
        if( m.offsetIndex() == QString::number( fieldOffsetCfgGCDPrintWithoutCustomerDisplay ) )  // Make sure this is message that was requested
        {
            QVector<QString> newValueList = TokenParser::splitStringTokens( m.newValue() );
            if(newValueList.count() == 1)
            {
                originalPrintWithoutGCDValue = newValueList.at( 0 );
            }
        }

        // FT-1626 Get IP Address from config_linux.dat/config.dat and set the text box on the Network tab
        if( m.offsetIndex() == QString::number( fieldOffsetCfgIMfgTestServer ) )  // Make sure this is message that was requested
        {
            ui->txtIpToPing->setText( m.newValue() );

            LOG_DEBUG( "IP Address from config file=%s", ui->txtIpToPing->text().toLatin1().constData() );
        }
    }

    // SOF-3449
    //-----------------------------------------------------------------
    // Processes Board Site message, containing status of various tests
    //-----------------------------------------------------------------
    if (( BoardSite == device ) && ( BrdSiteBoardTestCompleteUI == event )) // 23,4
    {
        DomBoardSiteTestsCompleteMsg m( msg );

        arkSerialFlash_ = m.arkSerialFlash();
        arkSDSlot0_ = m.arkSDSlot0();
        arkSDSlot1_ = m.arkSDSlot1();
        arkRTC_ = m.arkRTC();
        arkUSB_ = m.arkUSB();
        arkWifi_ = m.arkWifi();
        arkBlueTooth_ = m.arkBlueTooth();
        arkEthernet_ = m.arkEthernet();
        arkMemory_ = m.arkMemory();
        arkCan0_ = m.arkCan0();
        kpcCan_ = m.kpcCan();
        kpcModel_ = m.kpcModel();
        kpcSerialFlash_ = m.kpcSerialFlash();
        kpcValueMax_ = m.kpcValueMax();
        kpcCutter_ = m.kpcCutter();
        kpcDotWear_ = m.kpcDotWear();


        LOG_DEBUG("Board Site Test Results:", NULL );
        LOG_DEBUG("ARK Serial Flash = %d", arkSerialFlash_ );
        LOG_DEBUG("ARK SD Slot 0 = %d", arkSDSlot0_ );
        LOG_DEBUG("ARK SD Slot 1 = %d", arkSDSlot1_ );
        LOG_DEBUG("ARK RTC = %d", arkRTC_ );
        LOG_DEBUG("ARK USB = %d", arkUSB_ );
        LOG_DEBUG("ARK WIFI = %d", arkWifi_ );
        LOG_DEBUG("ARK Bluetooth = %d", arkBlueTooth_ );
        LOG_DEBUG("ARK Ethernet = %d", arkEthernet_ );
        LOG_DEBUG("ARK Memory = %d", arkMemory_ );
        LOG_DEBUG("ARK CAN0 = %d", arkCan0_ );
        LOG_DEBUG("KPC CAN = %d", kpcCan_ );
        LOG_DEBUG("KPC Model = %d", kpcModel_ );
        LOG_DEBUG("KPC Serial Flash = %d", kpcSerialFlash_ );
        LOG_DEBUG("KPC ValueMax = %d", kpcValueMax_ );
        LOG_DEBUG("KPC Cutter = %d", kpcCutter_ );
        LOG_DEBUG("KPC Dot Wear = %d", kpcDotWear_ );


        //------------------------------------------------
        // Update Pass/Fail fields on the appropriate tabs
        //------------------------------------------------
        if( arkRTC_ == 1 )
        {
            ui->lblBoardSiteArkRtcAnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkSerialFlash_ == 1 )
        {
            ui->lblBoardSiteArkSerialFlashAnswerLabel->setText( tr( "Yes" ) );
        }

        if( kpcSerialFlash_ == 1 )
        {
            ui->lblBoardSiteKpcSerialFlashAnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkUSB_ == 1 )
        {
            ui->lblBoardSiteArkUsbAnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkEthernet_ == 1 )
        {
            ui->lblBoardSiteArkEthernetAnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkWifi_ == 1 )
        {
            ui->lblBoardSiteKpcWifiAnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkBlueTooth_ == 1 )
        {
            ui->lblBoardSiteArkBtAnswerLabel->setText( tr( "Yes" ) );
        }

        if( kpcCutter_ == 1 )
        {
            ui->lblBoardSiteKpcCutterAnswerLabel->setText( tr( "Yes" ) );
        }

        if( kpcDotWear_ == 1 )
        {
            ui->lblBoardSiteKpcDotWearAnswerLabel->setText( tr( "Yes" ) );
        }

        if( kpcValueMax_ == 1 )
        {
            ui->lblBoardSiteKpcValueMaxAnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkSDSlot0_ == 1 )
        {
            ui->lblBoardSiteArkUsdSlot0AnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkSDSlot1_ == 1 )
        {
            ui->lblBoardSiteArkUsdSlot1AnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkMemory_ == 1 )
        {
            ui->lblBoardSiteArkMemoryAnswerLabel->setText( tr( "Yes" ) );
        }

        if( arkCan0_ == 1 )
        {
            ui->lblBoardSiteArkCan0AnswerLabel->setText( tr( "Yes" ) );
        }

        if( kpcCan_ == 1 )
        {
            ui->lblBoardSiteKpcCanAnswerLabel->setText( tr( "Yes" ) );
        }

        if( kpcModel_ == 1 )
        {
            ui->lblBoardSiteKpcModelAnswerLabel->setText( tr( "Yes" ) );
        }
    }

    // SOF-3516
    //-------------------------------------
    // Processes Board Site - Test Progress
    //-------------------------------------
    if (( BoardSite == device ) && ( BrdSitePercentCompete == event )) // 23,8
    {
        DomBoardSiteTestsProgressMsg m( msg );

        ui->lblCurrentTestNameData->setText( m.testDescription() );

        ui->lblOverallTestProgressData->setText( m.percentComplete() + "%" );

        LOG_DEBUG("Test Progress Received: Test=%s; Percent=%s", m.testDescription().toLatin1().constData(), m.percentComplete().toLatin1().constData() );
    }

    //-------------------------------------
    // Update all test statuses (FAIL/PASS)
    //-------------------------------------
    getTestStatus();
}

void manufacturingTest::SaveDataOnExit()
{
    if( cutterEnabled_ != cutterEnabledOriginal_ )
    {
        ScaleConfiguration *scaleConfig( session_->scaleConfig );
        scaleConfig->scaleConfig.isCutterEnabledRaw() = QString::number( cutterEnabled_ );

        QString param;
        param += "," + QString::number( fieldOffsetCfgIsCutterEnabled);
        param += "," + QString::number( cutterEnabled_ );
        param += "," + QString::number(cutterEnabledOriginal_);

        session_->send( ConfigurationDev, CfgSaveSingleField, param );
    }

    // Make sure Printer Sensor messages are OFF
    session_->send( Printer, PrSensorsTracking, "0,0" );
}


/////////////////////////////////////////////////////////////////////////////////////////////////
void manufacturingTest::on_enableCutterButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_programFlashButton_clicked" );

    LOG_DEBUG( "START on_enableCutterButton_clicked()", NULL );

    if( !cutterAttached_)
    {
        cutterEnabled_ = false;
        ScaleConfiguration *scaleConfig( session_->scaleConfig );
        scaleConfig->scaleConfig.isCutterEnabledRaw() = cutterEnabled_;
        return;
    }

    if( cutterAttached_ )
    {
        if( cutterEnabled_ )
        {
            LOG_DEBUG( "on_enableCutterButton_clicked() cutterAttached_ && cutterEnableD_ BOTH TRUE", NULL );

            // its attached and enabled.  Its still going to be attached, but lets disable it.
            cutterEnabled_ = false;
            ui->lblCutterStatusResult->setText(tr("Disabled"));
            ui->butEnableCutter->setText(tr("Enable"));

            session_->send(Printer, PrCutterDisabled);       // primary printer = 0
        }
        else
        {
            LOG_DEBUG( "on_enableCutterButton_clicked() cutterAttached_ TRUE cutterEnableD_ FALSE", NULL );

            // its attached and disabled.  Its still going to be attached, but not enabled too.
            cutterEnabled_ = true;
            ui->lblCutterStatusResult->setText(tr("Enabled"));
            ui->butEnableCutter->setText(tr("Disable"));

            session_->send(Printer, PrCutterEnabled);       // primary printer = 0
        }

        ScaleConfiguration *scaleConfig( session_->scaleConfig );
        scaleConfig->scaleConfig.isCutterEnabledRaw() = cutterEnabled_;
    }

    LOG_DEBUG( "END on_enableCutterButton_clicked()", NULL );
}

void manufacturingTest::on_programFlashButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_programFlashButton_clicked" );

    LOG_DEBUG( "Beginning of on_programFlashButton_clicked()", NULL );

    bool rebootNeeded = false;

    // SOF-1923
    // Check if u-boot needs to be upgraded
    python pythonProcess;
    QStringList args;
    needUbootUpgrade = false;

    if ( isLinux() )
    {
        if (( pythonProcess.execCommand( "ubootNeedsUpgraded", args, false, false, (20 * 1000) ) ) &&
            ( 0 == pythonProcess.getExitCode() ))
        {

            LOG_INFO("needUbootUpgrade = TRUE", NULL );
            needUbootUpgrade = true;
        }
            ubootUpgradeSuccessful = false;

    }

    const QString sysCtrlManufactureDateLineEdit( ui->sysCtrlManufactureDateLineEdit->text() );
    const QString sysCtrlSerialNumberLineEdit( ui->sysCtrlSerialNumberLineEdit->text().trimmed() );
    const QString sysCtrlStyleLineEdit( ui->sysCtrlStyleLineEdit->text().trimmed() );
    const QString sysCtrlBoardAssemblyNumberLineEdit( ui->sysCtrlBoardAssemblyNumberLineEdit->text().trimmed() );
    const QString sysCtrlBoardAssemblyRevisionLineEdit( ui->sysCtrlBoardAssemblyRevisionLineEdit->text().trimmed() );

    //---------------------
    // Validate text fields
    //---------------------
    if (( sysCtrlManufactureDateLineEdit.isEmpty() || sysCtrlManufactureDateLineEdit.contains( '\"' ) ) ||
        ( sysCtrlSerialNumberLineEdit.isEmpty() || sysCtrlSerialNumberLineEdit.contains( '\"' ) ) ||
        ( sysCtrlStyleLineEdit.isEmpty() || sysCtrlStyleLineEdit.contains( '\"' ) ) ||
        ( sysCtrlBoardAssemblyNumberLineEdit.isEmpty() || sysCtrlBoardAssemblyNumberLineEdit.contains( '\"' ) ) ||
        ( sysCtrlBoardAssemblyRevisionLineEdit.isEmpty() || sysCtrlBoardAssemblyRevisionLineEdit.contains( '\"' ) ))
    {
        MessageBoxDialog dialog( mainWindow_ );
        dialog.setText( tr( "All fields must be entered." ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );

        dialog.exec();
        return;
    }

    // SOF-484: Validate fields on the Flash tab
    if( !isValidModel( ui->sysCtrlStyleLineEdit->text() ) ||
        !isValidBoardRevNumber( ui->sysCtrlBoardAssemblyRevisionLineEdit->text() ) )
    {
        return;
    }

    // See if we need to reboot because the Serial Number changed
    if( initialSerialNumber != sysCtrlSerialNumberLineEdit )
        rebootNeeded = true;

    //--------------------------------
    // Program System Controller Flash
    //--------------------------------

    // MSG order:
    //  Style
    //  Board Assembly Number
    //  Board Assembly Revision
    //  Manufacturing Date
    //  Serial Number

    const QString msg(
        sysCtrlStyleLineEdit + "," +
        sysCtrlBoardAssemblyNumberLineEdit + "," +
        sysCtrlBoardAssemblyRevisionLineEdit + "," +
        sysCtrlManufactureDateLineEdit + "," +
        sysCtrlSerialNumberLineEdit );

    session_->send( MfgTestDev, WriteMfgData, msg );

    LOG_INFO("Programming Flash: Sent WriteMfgData message", NULL );

    // Update tabs and sub-tabs based on Style (Model Name)
    setTabsBasedOnStyle();

    // FT-1223
    //-------------------------------------------------------------
    // If there is not a weigher, as determined by the Model Name,
    // then save the configuration with Weigher Model set to "None"
    //-------------------------------------------------------------
    if ( noWeigher )
    {
        MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );

        QString param;
        param += weigherFilterSpeed;
        param += "," + QString(settings->isScaleMetric() ? "1" : "0");
        param += "," + QString::number( weigherScaleFactor );
        param += ",\"" + weigherMaximumWt + "\"";
        param += ",\"" + weigherMininumWt + "\"";
        param += ",,,,,";   // placeholders for lastDateCalibrated, timesCalibrated, lastDateConfigured, timesConfigured
        param += QString::number( SessionMainWindow::None ); // "None" for Weigher Model
        param += "," + netWeightDRLimitCheck;
        param += "," + dualRangeLimit;
        param += "," + QString( isScaleValuemax ? "1" : "0" );
        param += "," + gcdLanguage;

        session_->send( Weigher, WgConfig, param );
    }
    else
    {
        //-----------------------------------------------------------------------------------
        // SOF-3958
        //
        // If an ARK board is in the scale and the scale is configured as an HTi scale model:
        //
        //  Set the Weigher Model to "HT Series"
        //  Disable ValueMax
        //-----------------------------------------------------------------------------------

        // NEED TO CHECK isScaleValuemax VALUE ... MAY NOT HAVE TO DO ANY OF THIS
        // NEED TO CHECK ORIGINAL WEIGHER MODEL ...
        // NEED TO CHECK ORIGINAL SCALE MODEL ...

        if( ( currentManfTestMode == ManufTestPreTest ||
              currentManfTestMode == ManufTestFinalTest ||
              currentManfTestMode == ServiceDiagnosticMode ) &&
            ( ( ui->sysCtrlStyleLineEdit->text().startsWith( "HTI", Qt::CaseInsensitive ) ) ) &&
            ( boardManf == "ARK" )
          )
        {
            MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );

            QString param;

            //----------------------------------------------------------
            // Set the Weigher Model to "HT Series" and disable ValueMax
            //----------------------------------------------------------
            param = weigherFilterSpeed;
            param += "," + QString(settings->isScaleMetric() ? "1" : "0");
            param += "," + QString::number( weigherScaleFactor );
            param += ",\"" + weigherMaximumWt + "\"";
            param += ",\"" + weigherMininumWt + "\"";
            param += ",,,,,";   // placeholders for lastDateCalibrated, timesCalibrated, lastDateConfigured, timesConfigured
            param += QString::number( SessionMainWindow::HLX1 ); // "HT Series" for Weigher Model
            param += "," + netWeightDRLimitCheck;
            param += "," + dualRangeLimit;
            param += "," + QString( "0" ); // Disable ValueMax
            param += "," + gcdLanguage;

            LOG_DEBUG( "on_programFlashButton_clicked(): ARK board, set Weigher to HT Series & No ValueMax", NULL );
            LOG_DEBUG( "on_programFlashButton_clicked(): param=%s", param.toLatin1().constData() );

            session_->send( Weigher, WgConfig, param );
        }
    }
    //-----------------------------------------------------------------------
    // SOF-3244: Validate that the ValueMax load cell has been calibrated ...
    //           Need to ensure that the Weigher Model is "HTM"
    //-----------------------------------------------------------------------
    if( currentManfTestMode == ManufTestPreTest ||
        currentManfTestMode == ServiceDiagnosticMode )
    {
        // If the Scale Model is "HTM" ...
        if( ( ui->sysCtrlStyleLineEdit->text().startsWith( "HTM", Qt::CaseInsensitive ) ) ||
            ( ui->sysCtrlStyleLineEdit->text().startsWith( "FS", Qt::CaseInsensitive ) ) ||     // Free Standing Self-Serve
            ( ui->sysCtrlStyleLineEdit->text().startsWith( "KS", Qt::CaseInsensitive ) ) )      // Kiosk
        {

            checkFITParams = true;

            if( !isHtmWeigher || checkFITParams || isSelfServiceScale("FS") )
            {
                MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );
                QString param;
                param += weigherFilterSpeed;
                param += "," + QString(settings->isScaleMetric() ? "1" : "0");
                param += "," + QString::number( weigherScaleFactor );
                param += ",\"" + weigherMaximumWt + "\"";
                param += ",\"" + weigherMininumWt + "\"";
                param += ",,,,,";   // placeholders for lastDateCalibrated, timesCalibrated, lastDateConfigured, timesConfigured
                param += QString::number( SessionMainWindow::HTM ); // "HTM" Weigher Model
                param += "," + netWeightDRLimitCheck;
                param += "," + dualRangeLimit;

                // SOF-5403
                if (isSelfServiceScale("FS"))
                {
                    // Enable value max for FS-V variant and re-request FIT parameters for load cell calibration test below
                    if (sysCtrlStyleLineEdit.contains("V"))
                    {
                        LOG_DEBUG("Enabling value max for FS-V scale", NULL);
                        param += "," + QString( "1" );
                    }
                    // We are changing to a non FS-V variant
                    else
                    {
                        param += "," + QString( "0" ); // disable value max
                        // For non value max enabled FS scales, checkLoadCellCalibration will fail - and thus the manufacturing test
                        checkFITParams = false;
                    }
                }
                // The regular code for SOF-3244 - dealing with HTm unit
                else
                {
                    LOG_DEBUG( "on_programFlashButton_clicked(): Setting Weigher Model to HTM", NULL );
                    isHtmWeigher = true;
                    param += "," + QString( "1" ); // enable value max
                }
                param += "," + gcdLanguage;
                session_->send(Weigher, WgConfig, param.trimmed());

            }

            /*
             * SOF-5608: if VM was just enabled, FIT parameter request may return readings from the (still)
             * disabled acceleromter - give time to start it and get updated readings
             */


            if (checkFITParams)
            {

                session_->send( Weigher, WgReqValueMax, "4" ); // 1,40,4
                waitingForValueMax = true;

                startRetrieveFitParamTimer = new QTimer( this );
                startRetrieveFitParamTimer->setSingleShot( true );
                startRetrieveFitParamTimer->setInterval( 10000 );
                connect( startRetrieveFitParamTimer, SIGNAL( timeout() ), SLOT( confirmFitParams() ) );
                startRetrieveFitParamTimer->start();

                while ( waitingForValueMax && !changingScreens)
                {
                    LOG_DEBUG("Waiting on accelerometer", NULL);
                    sleepFor( 1000 );

                }
                startRetrieveFitParamTimer->stop();

            }
        }

        //--------------------------------
        // SOF-3294: Validate Code Checker
        //--------------------------------

        // SOF-5608: dot wear test data is only ever requested if we are in pre-test...
        // See autoWalkThruTabs()

        if( codeCheckerEnabled_ && currentManfTestMode == ManufTestPreTest )
        {
            LOG_DEBUG( "Validate Code Checker: haveDotWearTestData=%d  haveHeadDotWearDetailData=%d", haveDotWearTestData, haveHeadDotWearDetailData );

            if( !haveDotWearTestData )
            {
                // Print Head Dot Wear Status has already been requested,
                // but has not been returned yet.  Need to wait a little
                // longer for it.
                QTimer *waitForDotWearDataTimer;
                waitForDotWearDataTimer = new QTimer( this );
                waitForDotWearDataTimer->setSingleShot( true );
                waitForDotWearDataTimer->setInterval( 30000 );
                waitForDotWearDataTimer->start();

                while( !haveDotWearTestData && !changingScreens)
                {
                    LOG_TRACE( "Validate Code Checker: NOT haveDotWearTestData Loop", NULL );

                    sleepFor( 50 );
                }

                waitForDotWearDataTimer->stop();
            }

            if( haveDotWearTestData && !dotWearTestPassed && !haveHeadDotWearDetailData )
            {
                //---------------------------------------------------------
                // SOF-3584
                // If there are bad or marginal dots as indicated in the
                // Print Head Dot Wear Status message, the "detail" message
                // will be requested and sent by the backend automatically.
                // Need to wait for it to be received.
                //---------------------------------------------------------
                QTimer *waitForDotWearDataTimer;
                waitForDotWearDataTimer = new QTimer( this );
                waitForDotWearDataTimer->setSingleShot( true );
                waitForDotWearDataTimer->setInterval( 30000 );
                waitForDotWearDataTimer->start();

                while( !haveHeadDotWearDetailData && !changingScreens )
                {
                    LOG_TRACE( "Validate Code Checker: NOT haveHeadDotWearDetailData Loop", NULL );

                    sleepFor( 50 );
                }

                waitForDotWearDataTimer->stop();
            }


            LOG_DEBUG( "Validate Code Checker: haveDotWearTestData=%d", haveDotWearTestData );
            LOG_DEBUG( "Validate Code Checker: dotWearTestPassed=%d", dotWearTestPassed );
            LOG_DEBUG( "Validate Code Checker: haveHeadDotWearDetailData=%d", haveHeadDotWearDetailData );
            LOG_DEBUG( "Validate Code Checker: haveBadDot=%d", haveBadDot );


            if( !dotWearTestPassed && haveBadDot )
            {
                MessageBoxDialog dialog( real_time_priority, mainWindow_ );
                dialog.setAllowOverlayClickDismiss( false );

                dialog.setTitle( "PRINTER DOT STATUS FAILURE" );
                dialog.setText( "This Print Head has Bad dots.\n\nGo to Service->Printer Dot Status for details.\n\nPress OK to continue." );
                dialog.setStandardButtons( QDialogButtonBox::Ok );
                dialog.exec();

                // Return to Home (i.e. PluMainScreen)
                mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();
            }
        }
    }

    // FT-1223
    //----------------------------------------------------------------------------
    // If there is not a Customer Display, as determined by the Model Name,
    // then save the configuration that allows printing without a Customer Display
    //----------------------------------------------------------------------------
    if( noCustomerDisplay )
    {
        QString param;
        param += "," + QString::number( fieldOffsetCfgGCDPrintWithoutCustomerDisplay );
        param += ",\"1\"";  // This value allows printing without a Customer Display
        param += "," + originalPrintWithoutGCDValue;

        session_->send( ConfigurationDev, CfgSaveSingleField, param );
    }

    //-------------------------------------------------
    // For NGW wrappers extract the node board firmware
    // SOF-1063: use ngs firmware for full bright can
    //-------------------------------------------------
    if ( "NGW" == sysCtrlStyleLineEdit.left( 3 ).toUpper() && '1' == sysCtrlStyleLineEdit[7] )
    {
        // setup pattern for firmware
        QDir folder( filesystem::folder( folderType::Firmware ) );
        folder.setNameFilters( QStringList() << "htseries-any-ngw-*-release.package.tgz" );

        // find firmware
        QStringList firmware( folder.entryList() );

        // check result
        if ( 1 != firmware.size() )
            LOG_WARN( "could not find NGW firmware!", NULL );
        else
        {
            firmware[0] = folder.absoluteFilePath( firmware[0] );

            LOG_INFO( "using NGW firmware '%s'", qPrintable( firmware[0] ) );

            python pythonProcess;

            // install
            if ( !pythonProcess.execCommand( "installUpgrade", firmware, false, false, (20 * 1000) ) )
                LOG_WARN( "failed to install NGW firmware", NULL );
            else if ( 0 != pythonProcess.getExitCode() )
                LOG_WARN( "NGW firmware installation bad exit code", NULL );
            else
            {
                LOG_INFO( "successfully installed NGW firmware!", NULL );
            }
        }
    }
    else if ( ( "NGW" == sysCtrlStyleLineEdit.left( 3 ) ) &&
              ( session_->scaleConfig->isWrapperTwoPrinter() ) )
    {
        LOG_INFO("NGW 2P System", NULL);
        // setup pattern for firmware
        QDir folder( filesystem::folder( folderType::Firmware ) );
        folder.setNameFilters( QStringList() << "htseries-any-ngw2p-*-release.package.tgz" );

        // find firmware
        QStringList firmware( folder.entryList() );

        // check result
        if ( 1 != firmware.size() )
            LOG_WARN( "could not find NGW firmware!", NULL );
        else
        {
            firmware[0] = folder.absoluteFilePath( firmware[0] );

            LOG_INFO( "using NGW firmware '%s'", qPrintable( firmware[0] ) );

            python pythonProcess;

            // install
            if ( !pythonProcess.execCommand( "installUpgrade", firmware, false, false, (20 * 1000) ) )
                LOG_WARN( "failed to install NGW firmware", NULL );
            else if ( 0 != pythonProcess.getExitCode() )
                LOG_WARN( "NGW firmware installation bad exit code", NULL );
            else
            {
                LOG_INFO( "successfully installed NGW firmware!", NULL );
            }
        }
    }
    else if (( "NGW" == sysCtrlStyleLineEdit.left( 3 ).toUpper() && ( '2' == sysCtrlStyleLineEdit[7] || '3' == sysCtrlStyleLineEdit[7] )) ||
             ( "NGS" == sysCtrlStyleLineEdit.left( 3 ).toUpper() && ( '2' == sysCtrlStyleLineEdit[7] || '3' == sysCtrlStyleLineEdit[7] )))
    {
        // setup pattern for firmware
        QDir folder( filesystem::folder( folderType::Firmware ) );
        folder.setNameFilters( QStringList() << "htseries-any-ngw-*-release.package.tgz" );

        // find firmware
        QStringList firmware( folder.entryList() );

        // check result
        if ( 1 != firmware.size() )
            LOG_WARN( "could not find NGW firmware!", NULL );
        else
        {
            firmware[0] = folder.absoluteFilePath( firmware[0] );

            LOG_INFO( "using NGW firmware '%s'", qPrintable( firmware[0] ) );

            python pythonProcess;

            // install
            if ( !pythonProcess.execCommand( "installUpgrade", firmware, false, false, (20 * 1000) ) )
                LOG_WARN( "failed to install NGW firmware", NULL );
            else if ( 0 != pythonProcess.getExitCode() )
                LOG_WARN( "NGW firmware installation bad exit code", NULL );
            else
            {
                LOG_INFO( "successfully installed NGW firmware!", NULL );
            }
        }
    }

    //-----------------------------------------
    // Determine if u-boot needs flash upgraded
    //-----------------------------------------
    if ( isLinux() )
    {
        ui->ubootProgrammingTextEdit->clear();

        if( needUbootUpgrade )
        {
            ui->ubootProgrammingTextEdit->append( tr( "Newer U-boot version found on uSD card.\nU-boot will be programmed and this may\ntake a few minutes.\n" ) );
            qApp->processEvents();

            ui->testTabsWidget->setDisabled( true );

            LOG_INFO("BEFORE call updateUbootBitmaps()", NULL );

            // SOF-1923 ... increased timeout, as some scales were taking more than a minute to complete this step
            if (( pythonProcess.execCommand( "updateUbootBitmaps", args, false, false, (300 * 1000) ) ) &&
                ( 0 == pythonProcess.getExitCode() ))
            {

                LOG_INFO("AFTER call updateUbootBitmaps() - PASSED", NULL );

                // SOF-1773: Get the new U-boot version from the uboot-version.txt file
                LOG_DEBUG( "Getting new U-boot version from USD card. ", NULL );
                getUbootVersionFromUsd();

                QFile file( "/usr/local/hobart/ubootUpdate.log" );

                if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) )
                   ui->ubootProgrammingTextEdit->append( tr( "\nERROR: Uboot update log not found." ) );
                else
                {
                    QTextStream in( &file );

                    while ( !in.atEnd() )
                       ui->ubootProgrammingTextEdit->append( in.readLine() );

                    file.close();
                }

                rebootNeeded = true;

                ubootUpgradeSuccessful = true;

                LOG_INFO("AFTER updateUbootBitmaps(): ubootUpgradeSuccessful = PASSED", NULL );


                // SOF-2963: Need to reboot scale when u-boot is upgraded in Service Diagnostics mode on Flash tab
                if( ( currentManfTestMode == ServiceDiagnosticMode ) && rebootNeeded )
                {
                    MessageBoxDialog dialog( real_time_priority, mainWindow_ );

                    dialog.setText( tr( "You need to re-boot to finish the update.\n\nPlease remove Service Key and Press OK." ) );
                    dialog.setAllowOverlayClickDismiss( false );
                    dialog.setStandardButtons( QDialogButtonBox::Ok );

                    dialog.exec();

                    reboot();
                }
            }
            else
            {
                // SOF-1923

                ui->ubootProgrammingTextEdit->append( tr( "\nFAILURE: New U-boot version could not be updated.\n" ) );
                qApp->processEvents();

                ubootUpgradeSuccessful = false;

                LOG_INFO("AFTER updateUbootBitmaps(): ubootUpgradeSuccessful = FAILED", NULL );
            }

            ui->testTabsWidget->setEnabled( true );
        }
    }


    // HT-455: Only re-boot / give confirmation message for Prepack
    if( session_->isPrepack() )
    {
        if( rebootNeeded )
        {
            MessageBoxDialog dialog( real_time_priority, mainWindow_ );

            dialog.setText( "You need to re-boot to finish the update.\n\nPlease remove Service Key and Press OK." );
            dialog.setStandardButtons( QDialogButtonBox::Ok );

            dialog.exec();

            reboot();
        }
        else
        {
            MessageBoxDialog dialog( real_time_priority, mainWindow_ );

            dialog.setText( "Update complete.\n\nPlease press OK to continue." );
            dialog.setStandardButtons( QDialogButtonBox::Ok );

            dialog.exec();
        }
    }
}

// SOF-464: Validate Model
bool manufacturingTest::isValidModel( QString modelName )
{
    if( session_->isServiceCounter() )
    {
        //--------------------------------------------------------------------
        // Style (i.e. Model Name) must begin with either HTi, HTm, HTx or HTs
        //--------------------------------------------------------------------
        if( ( modelName.length() < 3 ) ||
            ( ( modelName.left( 3 ) != "HTi" ) &&
              ( modelName.left( 3 ) != "HTs" ) &&
              ( modelName.left( 3 ) != "HTm" ) &&   // SOF-3151
              ( modelName.left( 3 ) != "HTx" ) &&
              ( !( modelName.toUpper().startsWith( "FS" ) ) ) &&  // SOF-4958
              ( !( modelName.toUpper().startsWith( "KS" ) ) ) ) ) // SOF-4958
        {
            MessageBoxDialog dialog( mainWindow_ );
            dialog.setText( tr( "Invalid model entered.\n\nModel must begin with: HTi, HTs, HTm, HTx, FS or KS." ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok );

            dialog.exec();
            return false;
        }
    }

    if( session_->isPrepack() )
    {
        if( ( modelName.length() < 4 ) ||
            ( ( modelName.left( 3 ) != "AWS" ) &&
              ( modelName.left( 4 ).toUpper() != "EPCP" ) &&
              ( modelName.left( 4 ).toUpper() != "NGW1" ) &&
              ( modelName.left( 4 ).toUpper() != "NGW2" ) &&
              ( modelName.left( 4 ).toUpper() != "NGS1" ) ) )
        {
            MessageBoxDialog dialog( mainWindow_ );
            dialog.setText( tr( "Invalid model entered.\n\nModel must begin with: AWS, EPCP, NGW1, NGW2, or NGS." ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok );

            dialog.exec();
            return false;
        }

        //-----------------------------------------
        // Check wrapper model info for NGW and NGS
        //-----------------------------------------
        if (( "NGW" == modelName.left( 3 ).toUpper() ) ||
            ( "NGS" == modelName.left( 3 ).toUpper() ))
        {

            // must be in form NGWn-XXXXX
            if ( modelName.length() < 10 )
            {
                MessageBoxDialog dialog( mainWindow_ );
                dialog.setText( tr( "Invalid model entered.\n\nNGW and NGS wrapper models need to be 10 characters long." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();
                return false;
            }

            // must be in form NGWn-XXnXX
            if ( ( '-' != modelName[4] ) ||
                 ( 'L' != modelName[5] && 'W' != modelName[5] ) ||
                 ( 'S' != modelName[6] && 'I' != modelName[6] && 'Q' != modelName[6] && 'X' != modelName[6] ) ||
                 ( '1' != modelName[7] && '2' != modelName[7] && '3' != modelName[7] ) ||
                 ( "LR" != modelName.mid( 8, 2 ) && "RL" != modelName.mid( 8, 2 ) ) )
            {
                MessageBoxDialog dialog( mainWindow_ );
                dialog.setText( tr( "Invalid model entered.\n\nModel must be in the form \"NGWn-XXnXX\"." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();
                return false;
            }
        }
    }

    return true;
}

// SOF-464: Validate Board Revision Number - make sure it is all alpha for an ARA board
bool manufacturingTest::isValidBoardRevNumber( QString boardRevNum )
{
    bool rval = true;

    if( boardManf == "ARA" )
    {
        if( boardRevNum.length() >= 1 )
        {
            for( int i = 0; i < boardRevNum.length(); i++ )
            {
                if( !( boardRevNum[ i ].isLetter() ) )
                    rval = false;
            }
        }
        else
        {
            rval = false;
        }

        if( !rval )
        {
            MessageBoxDialog dialog( mainWindow_ );
            dialog.setText( tr( "Invalid Board Revision Number entered.\n\nBoard Revision Number must be a letter." ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok );

            dialog.exec();
        }
    }

    return rval;
}

//-------------------------------------------------------------------------
// SOF-232: Put Touch screen calibration button back in for Board Test mode
//-------------------------------------------------------------------------
void manufacturingTest::on_startCalibrationTestButtton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_startCalibrationTestButton_clicked" );

    python pythonProcess;
    QStringList args;

    // We will give them 30 minutes at most to calibrate touchscreen and at that point
    // return and fail
    if (pythonProcess.execCommand("calibrateTouchscreen",args, false, false, 30 * 60 * 1000))
    {
        if (pythonProcess.getExitCode() != 0)
        {
            LOG_DEBUG("Calibration of touch screen returned error, exit code %d", pythonProcess.getExitCode());
            return;
        }
    }
    else
    {
        LOG_DEBUG("Calibration of touch screen took longer than 30 minutes exiting with exit code %d", pythonProcess.getExitCode());
        return;
    }
}

void manufacturingTest::on_reportPushButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_reportPushButton_clicked" );

    LOG_DEBUG( "generating report", NULL );

    generateSummaryReport();
}

void manufacturingTest::on_exitPushButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_exitPushButton_clicked" );

    generateSummaryReport();


    QString body( "100025,1" );

    // shutdown!
    LOG_DEBUG( "sending shutdown message...", NULL );
    session_->send( Error, ErrSessionDevice, body );
}

//----------------------------------------------------------------
// SOF-304
// The tests for Operator and Customer Displays have been combined
// into a single test, instead of multiple tabs and buttons.
//----------------------------------------------------------------
void manufacturingTest::on_startDisplayTestButton_clicked()
{
    // Start the Operator Display test - if it has not passed
    if( !( ui->rbOperatorDisplayAnswerYes->isChecked() ) )
    {
        runOperatorDisplayTest();
    }

    if ( !noCustomerDisplay )
    {
        // Start the Customer Display test - if it has not passed
        if( !( ui->rbCustomerDisplayAnswerYes->isChecked() ) )
        {
            runCustomerDisplayTest();
       }
    }
}

void manufacturingTest::runOperatorDisplayTest()
{
    ui->rbOperatorDisplayAnswerYes->setChecked( false );
    ui->rbOperatorDisplayAnswerNo->setChecked( false );

    ui->stackedWidget->setCurrentWidget( ui->whiteScreenPage );
    qApp->processEvents();
    sleepFor( 2000 );

    ui->stackedWidget->setCurrentWidget(ui->blackTestPage);
    qApp->processEvents();
    sleepFor( 2000 );

    ui->stackedWidget->setCurrentWidget( ui->colorBarDisplayPage );
    qApp->processEvents();
    sleepFor( 2000 );

    ui->stackedWidget->setCurrentWidget(ui->grayScreenPage);
    qApp->processEvents();
    sleepFor( 2000 );

    ui->stackedWidget->setCurrentWidget(ui->testsPage);
    qApp->processEvents();
}

void manufacturingTest::runCustomerDisplayTest()
{
    ui->rbCustomerDisplayAnswerYes->setChecked( false );
    ui->rbCustomerDisplayAnswerNo->setChecked( false );

    // send message to gcd to display white screen test
    session_->send( GCDSessionDev, GcdSessionColorTestWhiteOn );

    sleepFor( 5000 );

    // send message to gcd to display color bar test
    session_->send( GCDSessionDev, GcdSessionColorTestOn );
}

void manufacturingTest::sleepFor( int ms ) const
{
    // amout of time to sleep
    QTime sleep( QTime::currentTime().addMSecs( ms ) );

    do
    {
        QApplication::processEvents();

    } while ( QTime::currentTime() < sleep );
}

void manufacturingTest::on_startBeeperTestButton_clicked()
{
    int volumeLevel = 20;

    // FT-38
    // Reset radio buttons to un-checked
    ui->rbBeeperTestAnswerYes->setChecked( false );
    ui->rbBeeperTestAnswerNo->setChecked( false );

    while (volumeLevel <= 100)
    {
        python pythonProcess;
        QStringList args;

        args.append(QString::number( (long) (((volumeLevel*1.0)/100.0) * 254.0)));
        if (pythonProcess.execCommand("sampleBeep",args,false,false, 30 * 1000))
        {
            if (pythonProcess.getExitCode() == 0)
            {

            }
        }
        volumeLevel = volumeLevel + 20;
    }
}

void manufacturingTest::playAudioFile()
{
    if (currentPlaybackIndex < stereoTestFiles.length())
    {
        python pythonProcess;
        QStringList args;

        args.append("100");

        if (pythonProcess.execCommand("setVolume",args,false,false, 30 * 1000))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                QString media( filesystem::absoluteFilePath(stereoTestFiles.at(currentPlaybackIndex),folderType::Audio) );

                player->load( media, false );
                player->setVolume( 1.0 );

                player->play();
                currentPlaybackIndex++;
            }
        }
    }
    else
    {
       player->stop();
       player->deleteLater();
       player = NULL;
    }
}

void manufacturingTest::on_startStereoTestButton_clicked()
{
    //if (session_->isPrepack())	//	HA-62 but now HT-63
    //	return;

    // dump old player
    if ( player )
    {
        delete player;
        player = NULL;
    }

    player = videoPlayer::create( videoPlayerDevice::Operator, this );

    connect( player, SIGNAL( finished() ), SLOT( playAudioFile() ) );

    currentPlaybackIndex = 0;

    // Reset radio buttons to un-checked
    ui->rbStereoTestAnswerYes->setChecked( false );
    ui->rbStereoTestAnswerNo->setChecked( false );

    playAudioFile();
}

void manufacturingTest::conductNetworkTest(NetworkTestType networkTest)
{
    LOG_CLASS_INFO( "manufacturingTest", "conductNetworkTest" );
    killStartupNetworking();
    if (networkTest == Wired)
    {
        ui->wiredPingAnswerLabel->setText("");
        ui->lblWireNetworkResponseWait->show();
        ui->startWiredNetworkTestButton->setDisabled( true );
        ui->startWiredNetworkTestButton->repaint();
        qApp->processEvents();
    }
    else
    {
        ui->wirelessPingAnswerLabel->setText("");
        ui->lblWifiNetworkResponseWait->show();
        ui->startWirelessNetworkTestButton->setDisabled( true );
        ui->startWirelessNetworkTestButton->repaint();
        qApp->processEvents();
    }

    python pythonProcess;
    QStringList args;

    QString fileToBackup = "";
    QDir dir;
    bool fileWasBackedUp = false;

    // FT-1269 / FT-1272 / HA-62
    if ( currentManfTestMode == ManufTestPreTest   ||
         currentManfTestMode == ManufTestFinalTest ||
         currentManfTestMode == ManufTestPrepack   ||
         currentManfTestMode == BoardTest )
    {
        //--------------------------------------------------------
        // Do not change network setup for Service Diagnostic mode
        //--------------------------------------------------------

        // takes both devices down

        //args.append("-C");
        //args.append("00");

        //---------------------------------------------
        // Set up the interface we need to run the test
        //---------------------------------------------
        /*if (pythonProcess.execCommand("setNetwork",args,false,false, 30 * 1000))
        {
            if (pythonProcess.getExitCode() != 0)
            {
                LOG_DEBUG("setNetwork failed, interface down exit code %d", pythonProcess.getExitCode() );
                //return;
            }
            else
            {
                LOG_DEBUG("setNetwork (down) successful", NULL );
            }
        }
        else
            return;
        */

        args.clear();

        if (networkTest == Wired)
        {
            args.append("wired");

            if(isLinux())
            {
                fileToBackup = "/etc/resolv.conf";
            }
            else
            {
                fileToBackup = "TBD";
            }
        }
        else
        {
            args.append("wireless");
            if(isLinux())
            {
                fileToBackup = "/etc/wpa_supplicant.conf";
            }
            else
            {
                fileToBackup = "TBD";
            }
        }

        //-----------------------------
        // BACKUP ORIGINAL NETWORK FILE
        //-----------------------------

        if (QFile::exists(fileToBackup))
        {
            if(networkTest == Wired)
                ui->lblWireNetworkResponseWait->setText("Creating network backup ...");
            else
                ui->lblWifiNetworkResponseWait->setText("Creating network backup ...");

            qApp->processEvents();

            fileWasBackedUp = true;

            if (QFile::exists(fileToBackup + "BACKUP"))
                dir.remove(fileToBackup + "BACKUP");

            QFile::copy(fileToBackup,fileToBackup + "BACKUP");
        }

        //------------------------------------------------------
        // Configure Manufacturing Test Network Environment:
        //
        // dhcpcd takes quite a while to get an IP address
        // wait two minutes to get a IP address from DHCP server
        //------------------------------------------------------

        // POS 44 Manf Enhancements
        if(networkTest == Wired)
            ui->lblWireNetworkResponseWait->setText("Configuring Network Environment ...");
        else
            ui->lblWifiNetworkResponseWait->setText("Configuring Network Environment ...");

        qApp->processEvents();

        if (pythonProcess.execCommand( "setMfgTestNetwork", args , false, false, (45 * 1000) ) )
        {
            if (pythonProcess.getExitCode() == 0)
            {
                LOG_DEBUG("setMfgTestNetwork successful", NULL );

                // network setup and ping successful
                // POS 44 Manf Enhancements
                if(networkTest == Wired)
                    ui->lblWireNetworkResponseWait->setText("Network Setup Successful, Beginning Ping Test ...");
                else
                    ui->lblWifiNetworkResponseWait->setText("Network Setup Successful, Beginning Ping Test ...");

                qApp->processEvents();
            }
            else
            {
                int exitCode = pythonProcess.getExitCode();
                LOG_DEBUG("setMfgTestNetwork failed, exit code %d", exitCode );

                // network setup did not happen so error
                if (networkTest == Wired)
                {
                    ui->wiredPingAnswerLabel->setText("No");
                    if( ui->startWiredNetworkTestButton->isVisible() )
                        ui->startWiredNetworkTestButton->setEnabled( true );
                    //ui->lblWireNetworkResponseWait->hide();
                    ui->lblWireNetworkResponseWait->setText(QString("Network Setup Failed, Exit Code: %1").arg(exitCode));
                }
                else
                {
                    ui->wirelessPingAnswerLabel->setText("No");
                    if( ui->startWirelessNetworkTestButton->isVisible() )
                        ui->startWirelessNetworkTestButton->setEnabled( true );
                    //ui->lblWifiNetworkResponseWait->hide();
                    ui->lblWifiNetworkResponseWait->setText(QString("Network Setup Failed, Exit Code: %1").arg(exitCode));
                }
                qApp->processEvents();
                return;
            }
        }
        else
        {
            // command did not complete in 45 seconds so fail

            LOG_DEBUG("setMfgTestNetwork failed: timeout", NULL );

            if (networkTest == Wired)
            {
                ui->wiredPingAnswerLabel->setText("No");
                if( ui->startWiredNetworkTestButton->isVisible() )
                    ui->startWiredNetworkTestButton->setEnabled( true );
                //ui->lblWireNetworkResponseWait->hide();
                ui->lblWireNetworkResponseWait->setText("Network Time-Out.");
            }
            else
            {
                ui->wirelessPingAnswerLabel->setText("No");
                if( ui->startWirelessNetworkTestButton->isVisible() )
                    ui->startWirelessNetworkTestButton->setEnabled( true );
                //ui->lblWifiNetworkResponseWait->hide();
                ui->lblWifiNetworkResponseWait->setText("Network Time-Out.");
            }
            qApp->processEvents();
            return;
        }
    }

    //-----------------------------------------------------
    // Ping the Gateway associated with specified interface
    //-----------------------------------------------------
    args.clear();
    args.append("-c");
    args.append("1");

    args.append( ipAddressToPing );

    LOG_DEBUG("IP address to ping: %s", ipAddressToPing.toLatin1().constData() );

    // Ping for Gateway IP address and after 20 seconds fail
    if (pythonProcess.execCommand( "ping", args, false, false, (20 * 1000) ) )
    {
        if (pythonProcess.getExitCode() == 0)
        {
            if (networkTest == Wired)
            {
                LOG_DEBUG("ping of Gateway successful (Wired)", NULL );

                ui->wiredPingAnswerLabel->setText("Yes");
                ui->lblWireNetworkResponseWait->setText("Ping of Gateway Successful!");

                // FT-1626
                //--------------------------------------------------------------------------------
                // Since the WIRED ping test succeeded, send the scale info so it can be captured.
                //--------------------------------------------------------------------------------
                // .
                // .
                // .
                // HT-381
                //-------------------------------------------------------------------------------------
                // If this scale only has wired networking then send the scale info to be captured now.
                //-------------------------------------------------------------------------------------
                if( ( noWirelessNetworking ) && ( currentManfTestMode == ManufTestPreTest || currentManfTestMode == ManufTestPrepack ) )
                {
                    sendScaleInformation();

                    LOG_DEBUG("sendScaleInformation() called for Wired", NULL );
                }
            }
            else
            {
                LOG_DEBUG("ping of Gateway successful (Wireless)", NULL );

                ui->wirelessPingAnswerLabel->setText("Yes");
                ui->lblWifiNetworkResponseWait->setText("Ping of Gateway Successful!");

                // HT-381
                //-----------------------------------------------------------------------------------
                // If this scale has wireless networking then send the scale info to be captured now.
                //-----------------------------------------------------------------------------------
                if( (!noWirelessNetworking) && ( currentManfTestMode == ManufTestPreTest || currentManfTestMode == ManufTestPrepack ) )
                {
                    sendScaleInformation();

                    LOG_DEBUG("sendScaleInformation() called for Wireless", NULL );
                }
            }
        }
        else
        {
            if (networkTest == Wired)
                ui->wiredPingAnswerLabel->setText("No");
            else
                ui->wirelessPingAnswerLabel->setText("No");
        }
    }
    else
    {
        LOG_DEBUG("ping of Gateway failed: timeout", NULL );

        if (networkTest == Wired)
            ui->wiredPingAnswerLabel->setText("No");
        else
            ui->wirelessPingAnswerLabel->setText("No");
    }
    qApp->processEvents();

    // FT-1269 / FT-1272 / HA-62
    if ( currentManfTestMode == ManufTestPreTest   ||
         currentManfTestMode == ManufTestFinalTest ||
         currentManfTestMode == ManufTestPrepack   ||
         currentManfTestMode == BoardTest )
    {
        //--------------------------------------------------------
        // Do not change network setup for Service Diagnostic mode
        //--------------------------------------------------------

        //----------------------------------------
        // Bring down interface we set up for test
        //----------------------------------------
        args.clear();
        args.append("-C");
        args.append("00");

        //Code may need a check in the past/future, currently redundant.
        //if (networkTest == Wired)
        //    args.append("00");
        //else
        args.append("00");

        if (pythonProcess.execCommand("setNetwork",args,false,false, 10 * 1000))
        {
            if (pythonProcess.getExitCode() != 0)
            {
                int exitCode = pythonProcess.getExitCode();
                LOG_DEBUG("setNetwork failed, interface up exit code %d", exitCode );
                if(networkTest == Wired)
                    ui->lblWireNetworkResponseWait->setText(QString("Network Setup Failed, Exit Code: %1").arg(exitCode));
                else
                    ui->lblWifiNetworkResponseWait->setText(QString("Network Setup Failed, Exit Code: %1").arg(exitCode));

                qApp->processEvents();
                return;
            }
            else
            {
                LOG_DEBUG("setNetwork (up) successful", NULL );
                if(networkTest == Wired)
                    ui->lblWireNetworkResponseWait->setText("Configuring Network Environment ...");
                else
                    ui->lblWifiNetworkResponseWait->setText("Configuring Network Environment ...");

                qApp->processEvents();
            }
        }

        //----------------------
        // RESTORE ORIGINAL FILE
        //----------------------
        if (QFile::exists(fileToBackup + "BACKUP") && fileWasBackedUp)
        {
            if(QFile::exists(fileToBackup))
                dir.remove(fileToBackup);

            QFile::copy(fileToBackup + "BACKUP",fileToBackup);
        }
    }

    if (networkTest == Wired)
    {
        ui->lblWireNetworkResponseWait->hide();
        if( ui->startWiredNetworkTestButton->isVisible() )
            ui->startWiredNetworkTestButton->setEnabled( true );
        //ui->lblWireNetworkResponseWait->hide();
        ui->lblWireNetworkResponseWait->setText("No Test in Progress ...");
    }
    else
    {
        ui->lblWifiNetworkResponseWait->hide();
        if( ui->startWirelessNetworkTestButton->isVisible() )
            ui->startWirelessNetworkTestButton->setEnabled( true );
        //ui->lblWifiNetworkResponseWait->hide();
        ui->lblWifiNetworkResponseWait->setText("No Test in Progress ...");
    }
}

bool manufacturingTest::isLinux()
{
    #if defined( Q_OS_LINUX )
        return true;
    #elif defined( Q_OS_WIN )
        return false;
    #endif
}

bool manufacturingTest::killStartupNetworking()
{
    python pythonProcess;
    QStringList options;
    options.clear();

    bool result( false );
	
    if (pythonProcess.execCommand("killStartupNetworking", options))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result= true;                   //TBD - updateCustomerVolume();
        }
    }
    //processingAButton = false;
	
	return result;
}

void manufacturingTest::on_startWiredNetworkTestButton_clicked()
{
    if ( currentManfTestMode == ManufTestPreTest   ||
         currentManfTestMode == ManufTestFinalTest ||
         currentManfTestMode == ManufTestPrepack   ||
         currentManfTestMode == BoardTest )	//	HA-62
    {
        if( ui->txtIpToPing->text() == NULL )
            return;

        ipAddressToPing = ui->txtIpToPing->text();

        // dont allow test to pass with bad MAC
        if ( currentManfTestMode == BoardTest )
            if (( ethernetMacAddress == "NO DEVICE" ) || ( ethernetMacAddress == "" ) || ( ethernetMacAddress == "00:00:00:00:00:00" ))
                return;

        //---------------------------------------------------------------------------------------------
        // SOF-188 Validate Ethernet MAC Address for an ACR (i.e. ARA) board: must begin with 00:60:9D:
        //---------------------------------------------------------------------------------------------
        // SOF-234: The 7th digit of the Ethernet MAC address must be 0, 1, 2, 3 or 4
        //---------------------------------------------------------------------------------------------
        // SOF-2399: Revert changes for SOF-234; Ethernet MAC Address must begin with 00:60:9D:
        //---------------------------------------------------------------------------------------------
        if( boardManf == "ARA" ||
            boardManf == "ARK" ) // SOF-3114
        {
            QString validMacAddressPrefix = "00:60:9D:";

            if( ( ethernetMacAddress.length() != 17 ) ||
                ( ethernetMacAddress.mid( 0, validMacAddressPrefix.length() ).compare( validMacAddressPrefix, Qt::CaseInsensitive ) ) )
            {
                MessageBoxDialog dialog( real_time_priority, mainWindow_ );
                dialog.setAllowOverlayClickDismiss( false );
                dialog.setTitle( "FAILURE: INVALID MAC ADDRESS" );

                dialog.setText( "Invalid Ethernet MAC Address.\n\nThe Ethernet MAC Address must be in the range: 00:60:9D:xx:xx:xx." );

                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();

                return;
            }
        }
    }
    else
    {
        //------------------------
        // Service Diagnostic mode
        //------------------------

        if( !(ui->rbServiceDiagWiredNetworkGateway->isChecked() ) &&
            !(ui->rbServiceDiagWiredNetworkDHCP->isChecked() ) &&
            !(ui->rbServiceDiagWiredNetworkManualIP->isChecked() ) )

            return;

        if( ( ui->rbServiceDiagWiredNetworkManualIP->isChecked() ) &&
            ( ui->txtServiceDiagWiredNetworkManual->text() == NULL ) )

            return;

        // Gateway IP Address
        if( ui->rbServiceDiagWiredNetworkGateway->isChecked() )
            ipAddressToPing = ui->lblServiceDiagWiredNetworkGatewayData->text();

        // DHCP Server IP Address
        if( ui->rbServiceDiagWiredNetworkDHCP->isChecked() )
            ipAddressToPing = ui->lblServiceDiagWiredNetworkDHCPData->text();

        // Manual IP Address
        if( ui->rbServiceDiagWiredNetworkManualIP->isChecked() )
            ipAddressToPing = ui->txtServiceDiagWiredNetworkManual->text();
    }

    conductNetworkTest( Wired );
    getTestStatus();
}

void manufacturingTest::on_startWirelessNetworkTestButton_clicked()
{
    if ( currentManfTestMode == ManufTestPreTest   ||
         currentManfTestMode == ManufTestFinalTest ||
         currentManfTestMode == ManufTestPrepack   ||
         currentManfTestMode == BoardTest )	//	HA-62
    {
        if( ui->txtIpToPing->text() == NULL )
            return;

        ipAddressToPing = ui->txtIpToPing->text();
    }
    else
    {
        //------------------------
        // Service Diagnostic mode
        //------------------------

        if( !(ui->rbServiceDiagWirelessNetworkGateway->isChecked() ) &&
            !(ui->rbServiceDiagWirelessNetworkDHCP->isChecked() ) &&
            !(ui->rbServiceDiagWirelessNetworkManualIP->isChecked() ) )

            return;

        if( ( ui->rbServiceDiagWirelessNetworkManualIP->isChecked() ) &&
            ( ui->txtServiceDiagWirelessNetworkManual->text() == NULL ) )

            return;

        // Gateway IP Address
        if( ui->rbServiceDiagWirelessNetworkGateway->isChecked() )
            ipAddressToPing = ui->lblServiceDiagWirelessNetworkGatewayData->text();

        // DHCP Server IP Address
        if( ui->rbServiceDiagWirelessNetworkDHCP->isChecked() )
            ipAddressToPing = ui->lblServiceDiagWirelessNetworkDHCPData->text();

        // Manual IP Address
        if( ui->rbServiceDiagWirelessNetworkManualIP->isChecked() )
            ipAddressToPing = ui->txtServiceDiagWirelessNetworkManual->text();
    }

    conductNetworkTest( Wireless );
    getTestStatus();
}

// Removed when Vertical Lines test label added
// TODO: Not release 1
//void manufacturingTest::on_startPrintUpcBarcodeTestButton_clicked()
//{
//    // TODO:
//    /* Post release 1
//    QString msg;
//    msg = msg + "2," + "0";

//    session_->send( Printer, PrTestLabel, msg );
//    */
//}

void manufacturingTest::on_startCheckerBoardPatternTestButton_clicked()
{
    // FT-38
    // Reset radio buttons to un-checked
    ui->rbCheckerBoardPatternTestAnswerYes->setChecked( false );
    ui->rbCheckerBoardPatternTestAnswerNo->setChecked( false );

    QString msg = "0,2,0";

    session_->send( Printer, PrTestLabel, msg );
}

void manufacturingTest::on_startVerticalLinePatternTestButton_clicked()
{
    // FT-38
    // Reset radio buttons to un-checked
    ui->rbVerticalLinePatternTestAnswerYes->setChecked( false );
    ui->rbVerticalLinePatternTestAnswerNo->setChecked( false );

    QString msg = "0,4,0";

    session_->send( Printer, PrTestLabel, msg );
}

// Removed when Vertical Lines test label added
//void manufacturingTest::on_scannedBarcodeLineEdit_editingFinished()
//{
//    // TODO:
//    // Future Post Release 1 test
//}

void manufacturingTest::on_startMediaSensorTestButton_clicked()
{

    // FT-647
    // Printer Sensor ON
    if( isSSKPrinterAttached() )
    {
        if( !shootThroughCalStarted )
        {
            session_->send( Printer, PrStartGapCalibration );

            ui->lblMediaSensorDisplay->setText("Media: ");
            ui->lblMediaSensorDisplay->setVisible(true);
            ui->mediaSensorCalibrationTestAnswerLabel->setText("");
        }
        else
        {
            ui->lblStatusValue->setText( "Calibrating..." );
            session_->send( Printer, PrGapCalNext );
        }

        ui->startMediaSensorTestButton->setEnabled( false );
    }
    else
    {
        session_->send( Printer, PrSensorsTracking, "0,1" );

        ui->mediaSensorCalibrationTestAnswerLabel->setText("");
        ui->lblMediaSensorDisplay->setText("Media: ");
        ui->lblMediaSensorDisplay->setVisible(true);
        QString msg;
        msg = msg + "0," + "1";

        session_->send( Printer, PrReqCalibrate, msg );
    }
}

void manufacturingTest::on_startLabelTakenSensorTestButton_clicked()
{
    // FT-1089
    LOG_CLASS_INFO( "manufacturingTest", "on_startLabelTakenSensorTestButton_clicked" );
    LOG_DEBUG("Start Label Taken Sensor Test clicked", NULL );

    ui->takenSensorTestPassedAnswerLabel->setText( "No" );
    LOG_DEBUG("Set Label SensorTestAnswer to NO", NULL );

    checkSensorUnblockedBeforeTestPattern = true;
    checkLabelTakenSensorMode = false;
    lastLabelSensorMessageBlocked = false;

    // Printer Sensor ON
    session_->send( Printer, PrSensorsTracking, "0,1" );
    LOG_DEBUG("Send START printer sensor tracking", NULL );
}

void manufacturingTest::onResizeLabelsClicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "onResizeLabelsClicked" );

    // enable label taken button
    ui->startLabelTakenSensorTestButton->setEnabled( true );
    ui->btnSizeLabels->setEnabled( false );

    // send message to size the labels for primary printer
    LOG_INFO("Resizing labels button clicked", NULL);
    session_->send( Printer, PrReqLabelSizing, "0" );
    session_->send( Printer, PrReqAreWeSizingLabels );
}

void manufacturingTest::on_startPrinterLabelCutterTestButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_startPrinterLabelCutterTestButton_clicked" );
    LOG_DEBUG("Start Cutter Test clicked", NULL );

    if( CutterHome == cutter_state_ )
    {
        // once we send this, wait to see if we receive a left home and returned home messages.
        cutter_fired_ = true;
        session_->send( Printer, PrCutterCut );             // send a cut command
        ui->lblLabelCutterTestAnswer->setText( "Testing" );

        cutterTimer_->setSingleShot( true );
        cutterTimer_->start( 3000 );
    }
    else
    {
        cutter_fired_ = false;
        ui->lblLabelCutterTestAnswer->setText( "Failed" );
    }
}

// SOF-3763 - Test Printer Cassette
void manufacturingTest::on_startPrinterCassetteTestButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_startPrinterCassetteTestButton_clicked" );

    LOG_DEBUG("Start Printer Cassette Test", NULL );

    session_->send( Printer, PrReqTestCassette );
}

void manufacturingTest::onCutterTimeout()
{
    // A command has been sent to cut.  Need to find out if it did and the cutter moved from home
    if( ( cutter_fired_) && ( CutterHome == cutter_state_ ) )
    {
        session_->send(Error, Printer, "7033,5");      // tell user we timed out trying to spin/engage the cutter
        cutter_fired_ = false;
    }
}

// SOF-3887 - Reset Cutter Default value
void manufacturingTest::butResetCutterDefault_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "butResetCutterDefault_clicked" );

    LOG_DEBUG("Send message to reset Cutter default value", NULL );

    session_->send( Printer, PrCutterBitDflt ); // 7,184

    MessageBoxDialog dialog( real_time_priority, mainWindow_ );
    dialog.setAllowOverlayClickDismiss( false );
    dialog.setModal( false );

    dialog.setTitle( tr( "CUTTER RESET" ) );
    dialog.setText( "The Cutter has been reset to its default value.\n\nIf a Cutter is attached, make sure the Interlock is closed\nbefore re-booting the scale." );
    dialog.setStandardButtons( QDialogButtonBox::Ok );
    dialog.exec();
}

// FT-1055
void manufacturingTest::on_saveValueButton_clicked()
{
    // Used to calculate valid range
    int initialWeightOffset = 10000;  // 1 lb
    int plusMinusRange = 5000;

    if( adjustLeftStop == 1 )
    {
        leftStopSavedValue = ui->leftStopValueLabel->text().toInt();
        upperValue = leftStopSavedValue - initialWeightOffset - plusMinusRange;
        lowerValue = leftStopSavedValue - initialWeightOffset + plusMinusRange;

        ui->lblSavedBaselineValue->setText( "Left Stop Baseline Value: " + QString::number( leftStopSavedValue ) + "   Valid range: " + QString::number( upperValue ) + " - " + QString::number( lowerValue ) );

        if( ui->acceptStopsAdjustmentButton->isVisible() )
            ui->acceptStopsAdjustmentButton->setEnabled( true );
    }

    if( adjustRightStop == 1 )
    {
        rightStopSavedValue = ui->rightStopValueLabel->text().toInt();
        upperValue = rightStopSavedValue - initialWeightOffset - plusMinusRange;
        lowerValue = rightStopSavedValue - initialWeightOffset + plusMinusRange;

        ui->lblSavedBaselineValue->setText( "Right Stop Baseline Value: " + QString::number( rightStopSavedValue ) + "   Valid range: " + QString::number( upperValue ) + " - " + QString::number( lowerValue ) );

        if( ui->acceptStopsAdjustmentButton->isVisible() )
            ui->acceptStopsAdjustmentButton->setEnabled( true );
    }
}

void manufacturingTest::on_acceptStopsAdjustmentButton_clicked()
{
    // FT-1055
    // Accept Left Stop Adjustment
    if( adjustLeftStop == 1)
    {
        rightStopSavedValue = upperValue = lowerValue = 0;

        ui->saveValueButton->setText( "Save Right Stop Baseline Value" );
        ui->acceptStopsAdjustmentButton->setText( "Accept Right Stop Adjustment" );
        ui->label_33->setText( "Place weight on RIGHT side of platter and press Save Baseline Value" );
        ui->label_32->setText( "Adjust RIGHT STOP until RIGHT STOP indicator turns Green." );

        ui->lblSavedBaselineValue->clear();

        adjustLeftStop = 2;

        ui->acceptStopsAdjustmentButton->setDisabled( true );

        setStopsFlags();
    }
    else if( adjustRightStop == 1 )
    // FT-1055
    // Accept Right Stop Adjustment
    {
        ui->saveValueButton->setText( "Save Left Stop Baseline Value" );
        ui->acceptStopsAdjustmentButton->setText( "Accept Left Stop Adjustment" );
        ui->label_33->setText( "Place weight on LEFT side of platter and press Save Baseline Value" );
        ui->label_32->setText( "Adjust LEFT STOP until LEFT STOP indicator turns Green." );

        ui->lblSavedBaselineValue->clear();

        adjustRightStop = 2;

        ui->acceptStopsAdjustmentButton->setDisabled( true );
    }
}

// FT-1055
void manufacturingTest::setStopsFlags()
{
    // FT-735
    if( !adjustLeftStop && !adjustRightStop )
        adjustLeftStop = 1;

    if( adjustLeftStop == 2 && !adjustRightStop )
        adjustRightStop = 1;

    if( adjustLeftStop == 2 && adjustRightStop == 2)
    {
        // It all passed, but let's do it again ...
        adjustLeftStop = 1;
        adjustRightStop = 0;

        // Reset counts
        ui->leftStopValueLabel->setText( "0" );
        ui->rightStopValueLabel->setText( "0" );

        // Change background back to Red
        ui->leftStopValueLabel->setStyleSheet("background-color: rgb(255, 0, 0); font: 18pt \"MS Shell Dlg 2\";");
        ui->rightStopValueLabel->setStyleSheet("background-color: rgb(255, 0, 0); font: 18pt \"MS Shell Dlg 2\";");

        // Set Failure icon
        ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->stopsPage),failIcon);
    }
}

// FT-1055
void manufacturingTest::resetStopsValues()
{
    adjustLeftStop = 0;
    adjustRightStop = 0;
    if( ui->saveValueButton->isVisible() )
    {
        ui->saveValueButton->setText( "Save Left Stop Baseline Value" );
        ui->acceptStopsAdjustmentButton->setText( "Accept Left Stop Adjustment" );
        ui->label_33->setText( "Place weight on LEFT side of platter and press Save Baseline Value" );
        ui->label_32->setText( "Adjust LEFT STOP until LEFT STOP indicator turns Green." );
    }
}

// FT-1335: Leave this in ... still used manually when in Service Diagnostic mode
void manufacturingTest::on_startBtTestButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_startBtTestButton_clicked" );

    python pythonProcess;
    QStringList args;

    ui->btMouseRecognizedAnswerLabel->setText("");

    if (pythonProcess.execCommand("bluetoothScan", args, false, false, ( 60 * 1000 ) ) )
    {
        LOG_DEBUG("pythonProcess.getExitCode()=%d" , pythonProcess.getExitCode() );

        if (pythonProcess.getExitCode() == 0)
            ui->btMouseRecognizedAnswerLabel->setText("Yes");
        else
            ui->btMouseRecognizedAnswerLabel->setText("No");
    }
    getTestStatus();
}

// SOF-896
void manufacturingTest::on_startIOSataTestButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_startIOSataTestButton_clicked" );

    python pythonProcess;
    QStringList args;

    ui->sataAnswerLabel_IO->setText( "" );
    ui->sataAnswerLabel_IO->repaint();

    if ( !pythonProcess.execCommand( "boardSataTest", args, false, false, ( 60 * 1000 ) ) )
        ui->sataAnswerLabel_IO->setText( "No" );
    else
    {
        const QString result( pythonProcess.readStdOut() );

        LOG_DEBUG( "pythonProcess.getExitCode()=%d %s", pythonProcess.getExitCode(), qPrintable( result ) );

        if ( 0 == pythonProcess.getExitCode() )
            ui->sataAnswerLabel_IO->setText("Yes");
        else
        {
            const QStringList results( result.split( '|' ) );

            QString r( "No" );

            if ( 2 <= results.size() )
            {
                r.clear();

                //--------------------------------
                // NO RESULTS FOR SATADOM TEST ...
                //--------------------------------

                if ( "FAIL" == results[0] )
                {
                    if ( !r.isEmpty() )
                        r += ", ";

                    r += "SSD0 failed";
                }

                if ( "FAIL" == results[1] )
                {
                    if ( !r.isEmpty() )
                        r += ", ";

                    r += "SSD1 failed";
                }
            }

            ui->sataAnswerLabel_IO->setText( r );
        }
    }

    getTestStatus();
}

void manufacturingTest::on_startSataTestButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_startSataTestButton_clicked" );

    python pythonProcess;
    QStringList args;

    ui->sataAnswerLabel_IO->setText( "" );
    ui->sataAnswerLabel_IO->repaint();

    if ( !pythonProcess.execCommand( "boardSataTest", args, false, false, ( 60 * 1000 ) ) )
        ui->sataAnswerLabel_IO->setText( "No" );
    else
    {
        const QString result( pythonProcess.readStdOut() );

        LOG_DEBUG( "pythonProcess.getExitCode()=%d %s", pythonProcess.getExitCode(), qPrintable( result ) );

        if ( 0 == pythonProcess.getExitCode() )
            ui->sataAnswerLabel_IO->setText("Yes");
        else
        {
            const QStringList results( result.split( '|' ) );

            QString r( "No" );

            if ( 3 <= results.size() )
            {
                r.clear();

                if ( "FAIL" == results[0] )
                    r += "SATA failed";

                if ( "FAIL" == results[1] )
                {
                    if ( !r.isEmpty() )
                        r += ", ";

                    r += "SSD0 failed";
                }

                if ( "FAIL" == results[2] )
                {
                    if ( !r.isEmpty() )
                        r += ", ";

                    r += "SSD1 failed";
                }
            }

            ui->sataAnswerLabel_IO->setText( r );
        }
    }

    getTestStatus();
}


void manufacturingTest::on_startCanbusTestButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "on_startcanbusTestButton_clicked" );

    python pythonProcess;
    QStringList args;

    ui->canbusAnswerLabel->setText("");
    //BCM CAN Bus test is different from ACR (i.e. ARA). ACR is a loopback test BCM is not.
    if( boardManf == "BCM" )
    {
        if (pythonProcess.execCommand("boardTestCanMasterBCM", args, false, false, ( 90 * 1000 ) ) )
        {
            LOG_DEBUG("pythonProcess.getExitCode()=%d" , pythonProcess.getExitCode() );

            if (pythonProcess.getExitCode() == 0)
                ui->canbusAnswerLabel->setText("Yes");
            else
                ui->canbusAnswerLabel->setText("No");
        }
        else
        {
            ui->canbusAnswerLabel->setText("No");
        }
    }
    else if ( boardManf == "ARA" || boardManf == "ARK" )
    {
        if (pythonProcess.execCommand("boardTestCanMaster", args, false, false, ( 90 * 1000 ) ) )
        {
            LOG_DEBUG("pythonProcess.getExitCode()=%d" , pythonProcess.getExitCode() );

            if (pythonProcess.getExitCode() == 0)
                ui->canbusAnswerLabel->setText("Yes");
            else
                ui->canbusAnswerLabel->setText("No");
        }
        else
        {
            ui->canbusAnswerLabel->setText("No");
        }
    }
    getTestStatus();
}

// FT-1335
void manufacturingTest::startBluetoothDiscovery()
{
    LOG_CLASS_INFO( "manufacturingTest", "startBluetoothDiscovery" );

    QStringList options;

    options.append( filesystem::absoluteFilePath( "bluetoothScan.py", folderType::Python ) );

    discoverBluetoothProcess->waitForStarted( 100 );
    discoverBluetoothProcess->start( "python", options );

    LOG_DEBUG( "startBluetoothDiscovery() - STARTED", NULL );
}

// FT-1335
//------------------------------------------------------------
// Slot call when the Bluetooth discovery process is finished.
// Saves exit value from the Python script.
//------------------------------------------------------------
void manufacturingTest::finishedBluetoothProcess( int value, QProcess::ExitStatus exitStatus )
{
    LOG_CLASS_INFO( "manufacturingTest", "finishedBluetoothProcess" );

    // HT-177
    if( exitingScreen )
        return;

    Q_UNUSED( exitStatus );

    discoverBluetoothStatus = value;  // 0 = Success

    //-----------------------------------------------------
    // Just need to set the Pass/Fail status on the BT tab.
    //-----------------------------------------------------
    if( discoverBluetoothStatus == 0 )
        ui->btMouseRecognizedAnswerLabel->setText( "Yes" );
    else
        ui->btMouseRecognizedAnswerLabel->setText( "No" );

    LOG_DEBUG( "finishedBluetoothProcess: exit code = %d", discoverBluetoothStatus );

    discoverBluetoothProcess->kill();
}

// FT-1222
//--------------------------------------------------------------------------
// Used to start the Python process that will execute the scan of the HTe PC
//--------------------------------------------------------------------------
void manufacturingTest::startHteScan()
{
    ui->btnStartScan->setDisabled( true );
    ui->scanResultsList->clear();

    QStringList options;

    options.append( filesystem::absoluteFilePath( "hteSmScan.py", folderType::Python ) );
    options.append( ui->txtHTeSmPC->text() );

    scanProcess->waitForStarted( 100 );
    scanProcess->start( "python", options );
}

// FT-1222
//--------------------------------------------
// Displays output from the scan of the HTe PC
//--------------------------------------------
void manufacturingTest::displayScanOutput()
{
    QString output = scanProcess->readAllStandardOutput();
    qDebug() << "standard output is:" << output;
    ui->scanResultsList->addItem( output.mid( 0, output.length() - 1 ) );
}

// FT-1222
//-----------------------------------------------------------
// Displays Standard Error output from the scan of the HTe PC
//-----------------------------------------------------------
void manufacturingTest::readyReadStandardError()
{
     QString output = scanProcess->readAllStandardError();
     ui->scanResultsList->addItem( output.mid( 0, output.length() - 1 ) );
     scanProcess->kill();
     if( ui->btnStartScan->isVisible() )
        ui->btnStartScan->setEnabled( true );
}

// FT-1222
//-------------------------------------------------------------------------
// Used to clean-up the Python process that executes the scan of the HTe PC
//-------------------------------------------------------------------------
void manufacturingTest::finishedProcess( int value, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED( value );
    Q_UNUSED( exitStatus );

    scanProcess->kill();
    if( ui->btnStartScan->isVisible() )
        ui->btnStartScan->setEnabled( true );
}


// SOF-5997: initiates the typical weigher/printer reset after the dialog has been fully loaded (i.e menu panel dismissed)
void manufacturingTest::initiatePrinterWeigherReset()
{

    if ( ( currentManfTestMode == ManufTestPreTest ) &&
         ( !factoryResetAttempted ) &&
         ( !factoryResetDone ) )
    {
        resetFactoryDefaults();
    }

    runProcesses();
}

// FT-1222
//--------------------------------------------
// Displays / Hides the tabs based on the mode
//--------------------------------------------
void manufacturingTest::setManufacturingTestMode( int manufTestMode )
{

    LOG_CLASS_INFO( "manufacturingTest", "setManufacturingTestMode" );

    LOG_DEBUG( "Top of setManufacturingTestMode()", NULL );

    dateTimeValidationErrorDisplayed = false;

    //--------------------------------------------------------------------------
    // NOTE: If you add or remove tabs for the different modes in this function,
    //       you must make corresponding changes in getTestStatusInternal()
    //--------------------------------------------------------------------------

    // FT-39 / FT-1149
    numberOfTabs = 0;
    currentTabOffset = 0;
    currentManfTestMode = manufTestMode;

    LOG_DEBUG( "setManufacturingTestMode(): currentManfTestMode=%d", currentManfTestMode );

    // SOF-3449
    const QString boardTesterSD( "/etc/hobart/boardsite" );

    if( ( currentManfTestMode  == BoardTest ) && ( QFile::exists( boardTesterSD ) ) )
    {
        isArkBoardSite = true;

        LOG_DEBUG( "setManufacturingTestMode(): isArkBoardSite=TRUE", NULL );
    }

    //SOF-3752
    //Atmel peripheral Bd. 00-447780
    const QString peripheralTesterSD( "/etc/hobart/peripheralTest" );

    if( ( currentManfTestMode  == BoardTest ) && ( QFile::exists( peripheralTesterSD ) ) )
    {
        isPeripheralBoardTestMode = true;

        LOG_DEBUG( "setManufacturingTestMode(): isPeripheralBoardTestMode=TRUE", NULL );
    }


    if( isLinux() )
    {
        // SOF-3449
        if( isArkBoardSite )
        {
            session_->send( BoardSite, BrdSiteReqCompleteResults ); // 23,7

            LOG_DEBUG( "setManufacturingTestMode(): Sent request for Board Site Results", NULL );
        }
    }
    else
    {
        boardManf = "Unknown";
    }

    // SOF-3763
    MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );
    const QString scaleModel( settings->scaleName() );

    // First reset and hide all of the tabs
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->serviceDiagTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->sataTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->canbusTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->weigherTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->printerTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->fanTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->bluetoothTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->audioTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->networkTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->displayTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->usbTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->flashTab ) );
    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->dateTimeTab ) );

    // Show all tabs
    ui->testTabsWidget->insertTab( 0,  ui->dateTimeTab,      "Time" );
    ui->testTabsWidget->insertTab( 1,  ui->flashTab,         "Flash" );
    ui->testTabsWidget->insertTab( 2,  ui->usbTab,           "USB" );
    ui->testTabsWidget->insertTab( 3,  ui->displayTab,       "Display" );
    ui->testTabsWidget->insertTab( 4,  ui->networkTab,       "Network" );
    ui->testTabsWidget->insertTab( 5,  ui->audioTab,         "Audio" );
    ui->testTabsWidget->insertTab( 6,  ui->bluetoothTab,     "BT" );
    ui->testTabsWidget->insertTab( 7,  ui->fanTab,           "Fan" );
    ui->testTabsWidget->insertTab( 8,  ui->printerTab,       "Printer" );
    ui->testTabsWidget->insertTab( 9,  ui->weigherTab,       "Weigher" );
    ui->testTabsWidget->insertTab( 10, ui->canbusTab,        "I/O" );
    ui->testTabsWidget->insertTab( 11, ui->sataTab,          "SSD" );
    ui->testTabsWidget->insertTab( 12, ui->serviceDiagTab,   "..." );

    // FT-1269 / FT-1272
    // Sub-tabs on the main Network tab
    ui->networkTypeTab->insertTab( 0, ui->wiredTab,         "Wired" );
    ui->networkTypeTab->insertTab( 1, ui->wirelessTab,      "Wireless" );

    // Sub-tabs on the main Weigher tab
    ui->weigherTestsTab->insertTab( 0, ui->calibrationPage, "Calibration" );
    ui->weigherTestsTab->insertTab( 1, ui->stopsPage,       "Stops" );
    ui->weigherTestsTab->insertTab( 2, ui->countsPage,      "Counts" );

    // Make everything visible on the Network tab
    ui->lblIpAddressToPing->show();
    ui->txtIpToPing->show();
    ui->gbServiceDiagWiredNetworkTest->show();
    ui->gbServiceDiagWirelessNetworkTest->show();

    if (currentManfTestMode != BoardTest)
    {
        ui->reportPushButton->setVisible(false);
        ui->exitPushButton->setVisible(false);
    }

    //--------------------------------------------------------
    // SOF-3449 - Hide the frames and fields for ARK Boardsite
    //--------------------------------------------------------
    ui->frameBoardSiteTime->hide();
    ui->lblBoardSiteArkSerialFlashPassed->hide();
    ui->lblBoardSiteArkSerialFlashAnswerLabel->hide();
    ui->lblBoardSiteKpcSerialFlashPassed->hide();
    ui->lblBoardSiteKpcSerialFlashAnswerLabel->hide();
    ui->frameBoardSiteUsb->hide();
    ui->frameBoardSiteNetwork->hide();
    ui->lblBoardSiteArkBtPassed->hide();
    ui->lblBoardSiteArkBtAnswerLabel->hide();
    ui->lblBoardSiteKpcCutterPassed->hide();
    ui->lblBoardSiteKpcCutterAnswerLabel->hide();
    ui->lblBoardSiteKpcDotWearPassed->hide();
    ui->lblBoardSiteKpcDotWearAnswerLabel->hide();
    ui->lblBoardSiteKpcValueMaxPassed->hide();
    ui->lblBoardSiteKpcValueMaxAnswerLabel->hide();
    ui->frameBoardSiteIO->hide();


    // SOF-3853: Do not calibrate media sensor when there is a Cutter attached
    if( cutterAttached_ && !isSSKPrinterAttached() )
        removeMediaSensorCalibration();

    switch( manufTestMode )
    {
        case ManufTestPreTest:		//	Not for Prepack

            ui->lblScreenMainTitle->setText( "Manufacturing Subsystem Tests: Pre-Test" );

            //-------------------------------------------------------------------
            // SOF-3970: Prevent out-of-level message from displaying in Pre-Test
            //           if there is a leftover weigherCalibrate_ object
            //-------------------------------------------------------------------
            if ( weigherCalibrate_ )
            {
                delete weigherCalibrate_;
                weigherCalibrate_ = NULL;
            }

            //-----------------------------------------------
            // Need to remove the tabs from right-to-left ...
            //-----------------------------------------------

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->serviceDiagTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->sataTab ) );
            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->canbusTab ) );

            // Hide the Calibration tab on the Weigher tab
            ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->calibrationPage ) );
            ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->countsPage ) );

            // Hide the Service Diagnostic portion of the Network tab
            ui->gbServiceDiagWiredNetworkTest->hide();
            ui->gbServiceDiagWirelessNetworkTest->hide();

            // FT-1335
            // Hide label and button on the Bluetooth tab
            ui->label_19->hide();
            ui->startBtTestButton->hide();

            // SOF-232
            ui->startCalibrationTestButtton->setVisible( false );

            // HT-387: Set the default tab icons for the tabs that are visible
            ui->testTabsWidget->setTabIcon( ui->testTabsWidget->indexOf( ui->dateTimeTab ), failIcon );
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->displayTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),failIcon);
            ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wiredTab),failIcon);
            ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wirelessTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->fanTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cutterTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),failIcon);
            ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->stopsPage),failIcon);

            // SOF-3887
            ui->butResetCutterDefault->setVisible( false );

            // SOF-3763 - Do not show Cassette tab for Pre-Test
            ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->cassetteTestTab ) );

            // SOF-5595 - hide the fan tab on FS units
            if ( isSelfServiceScale("FS") )
                ui->testTabsWidget->removeTab(ui->testTabsWidget->indexOf(ui->fanTab));

            initiatePrinterWeigherReset();

            break;

        case ManufTestFinalTest:		//	Not for Prepack

            ui->lblScreenMainTitle->setText( "Manufacturing Subsystem Tests: Final Test" );

            // SOF-3519
            weigherCalibrate_ = new hobart::weigherCalibrate( this, mainWindow_, 0 );
            ui->calibrationVerticalLayout->addWidget(weigherCalibrate_);

            //-----------------------------------------------
            // Need to remove the tabs from right-to-left ...
            //-----------------------------------------------

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->serviceDiagTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->sataTab ) );
            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->canbusTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->fanTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->bluetoothTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->audioTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->networkTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->displayTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->usbTab ) );

            ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->countsPage ) );

            // Hide the Stops tab on the Weigher tab
            ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->stopsPage ) );


            // Hide the Service Diagnostic portion of the Network tab
            ui->gbServiceDiagWiredNetworkTest->hide();
            ui->gbServiceDiagWirelessNetworkTest->hide();

            // SOF-232
            ui->startCalibrationTestButtton->setVisible( false );

            // HT-387: Set the default tab icons for the tabs that are visible
            ui->testTabsWidget->setTabIcon( ui->testTabsWidget->indexOf( ui->dateTimeTab ), failIcon );
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cutterTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),failIcon);
            ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->calibrationPage),failIcon);

            // SOF-3887
            ui->butResetCutterDefault->setVisible( false );

            // SOF-3763 - Do not show Cassette tab for Final Test
            ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->cassetteTestTab ) );
            ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf (ui->testLabelTab ) );

            // SOF-5597: hide the widgets associated with the shoot through calibration
            if ( isSelfServiceScale("FS") )
            {
                // only hide this for FS scales
                ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf (ui->testLabelTab ) );

                foreach (QObject *iter, ui->sensorsTab->children())
                {
                    if (QWidget *temp = qobject_cast<QWidget*>(iter))
                        temp->hide();
                }

                QWidget *temp = new QWidget(ui->sensorsTab);
                ceilingComparison = new QLabel(ui->sensorsTab);
                ceilingComparisonLabel = new QLabel(ui->sensorsTab);
                QHBoxLayout *lay = new QHBoxLayout(ui->sensorsTab);

                lay->addWidget(ui->label_36);
                lay->addWidget(ui->takenSensorTestPassedAnswerLabel);

                temp->setLayout(lay);

                // resize existing widgets
                ui->startLabelTakenSensorTestButton->setGeometry(10,70,641,41);
                ui->lblMediaSensorDisplay->setGeometry(10,150,641,41);
                ui->lblHeadUpLabel->setGeometry(10,210,175,41);
                ui->lblHeadUpData->setGeometry(390,210,125,41);
                ui->lblLowStockLabel->setGeometry(10,275,175,41);
                ui->lblLowStockData->setGeometry(390,275,125,41);
                temp->setGeometry(0,350,641,41);

                ceilingComparisonLabel->setGeometry(10,410, 800,41);
                ceilingComparisonLabel->setText("Shoot Through Sensor Value OK?");
                ceilingComparison->setGeometry(10, 470, 900, 41);

                ceilingComparison->setFont(ui->lblHeadUpLabel->font());
                ceilingComparisonLabel->setFont(ui->lblHeadUpLabel->font());
                ui->lblMediaSensorDisplay->setFont(ui->lblHeadUpLabel->font());

                ui->startLabelTakenSensorTestButton->setEnabled(true);
                ui->startLabelTakenSensorTestButton->setEnabled( true );

                ui->startLabelTakenSensorTestButton->show();
                ui->lblMediaSensorDisplay->show();
                ui->lblLowStockLabel->show();
                ui->lblHeadUpLabel->show();
                ui->lblHeadUpData->show();
                ui->lblLowStockData->show();

                ceilingComparisonLabel->show();
                temp->show();


            }

            break;

        case ServiceDiagnosticMode:

            ui->lblScreenMainTitle->setText( "Service Diagnostics" );

            // SOF-3519
            weigherCalibrate_ = new hobart::weigherCalibrate( this, mainWindow_, 0 );
            ui->calibrationVerticalLayout->addWidget(weigherCalibrate_);

            //-----------------------------------------------
            // Need to remove the tabs from right-to-left ...
            //-----------------------------------------------

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->sataTab ) );    // HT-386
            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->canbusTab ) );  // HT-386

            if (session_->isPrepack())	//	HA-62
            {
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->weigherTab ) );

                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->printerTab ) );
            }
            else
            {
                // Hide the Stops tab on the Weigher tab
                ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->stopsPage ) );
            }

            ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->countsPage ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->fanTab ) );

            // FT-1269 / FT-1272
            if ( dbConnectionType == "0" )
            {
                // Scale is set for Wired networking: Hide the Wireless tab on the Network
                ui->networkTypeTab->removeTab( ui->networkTypeTab->indexOf( ui->wirelessTab ) );
            }

            if ( dbConnectionType == "1" )
            {
                // Scale is set for Wireless networking: Hide the Wired tab on the Network
                ui->networkTypeTab->removeTab( ui->networkTypeTab->indexOf( ui->wiredTab ) );
            }

            // Hide the "Manufacturing Test-only" portion of the Network tab
            ui->lblIpAddressToPing->hide();
            ui->txtIpToPing->hide();

            // FT-1335
            // Display label and button on the Bluetooth tab
            ui->label_19->show();
            ui->startBtTestButton->show();

            // HT-387: Set the default tab icons for the tabs that are visible
            ui->testTabsWidget->setTabIcon( ui->testTabsWidget->indexOf( ui->dateTimeTab ), failIcon );
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->displayTab),failIcon);

            if( session_->isPrepack() )
            {
                // No Customer Display ...
                // SOF-304 - Hide fields for Customer Display test
                ui->lblCustomerDisplayTestMessage->hide();
                ui->gbCustomerDisplayAnswers->hide();
            }
            // SOF-232
            ui->startCalibrationTestButtton->setVisible( false );

            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),failIcon);
            ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wiredTab),failIcon);
            ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wirelessTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->sensorsTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cassetteTestTab),failIcon);
            ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->cutterTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),failIcon);
            ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->calibrationPage),failIcon);

            // SOF-3887
            ui->butResetCutterDefault->setVisible( true );

            // SOF-3763
            if( scaleModel.length() >= 3 && !( scaleModel.toUpper().startsWith( "HTM") ) )
            {
                ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->cassetteTestTab ) );
            }

            break;

        case BoardTest:

            LOG_DEBUG( "setManufacturingTestMode(): In BoardTest case", NULL );

            // SOF-3449             SOF-3752
            if( isArkBoardSite || isPeripheralBoardTestMode )
                isDepopBoardTestMode = false;
            else
                isDepopBoardTestMode = checkIfDepopBoard();

            if( isArkBoardSite )
            {
                //------------------------------------------------
                // Display the frames and fields for ARK Boardsite
                //  hide the non-boardsite fields on the same tab
                //------------------------------------------------

                ui->frameBoardSiteTime->show();
                ui->frameBoardSiteTime->activateWindow();
                ui->frameBoardSiteTime->raise();

                ui->lblBoardSiteArkSerialFlashPassed->show();
                ui->lblBoardSiteArkSerialFlashAnswerLabel->show();
                ui->lblBoardSiteKpcSerialFlashPassed->show();
                ui->lblBoardSiteKpcSerialFlashAnswerLabel->show();

                ui->frameBoardSiteUsb->show();
                ui->frameBoardSiteUsb->activateWindow();
                ui->frameBoardSiteUsb->raise();

                ui->frameBoardSiteNetwork->show();
                ui->frameBoardSiteNetwork->activateWindow();
                ui->frameBoardSiteNetwork->raise();

                ui->label_18->hide();
                ui->btMouseRecognizedAnswerLabel->hide();
                ui->lblBoardSiteArkBtPassed->show();
                ui->lblBoardSiteArkBtAnswerLabel->show();

                ui->printerTestsTab->setTabText( ui->printerTestsTab->indexOf( ui->testLabelTab ), tr( "Printer Tests" ) );
                ui->startVerticalLinePatternTestButton->hide();
                ui->label_31->hide();
                ui->gbVerticalLinePatternTestAnswers->hide();
                ui->lblBoardSiteKpcCutterPassed->show();
                ui->lblBoardSiteKpcCutterAnswerLabel->show();
                ui->lblBoardSiteKpcDotWearPassed->show();
                ui->lblBoardSiteKpcDotWearAnswerLabel->show();
                removeCutterTab();

                ui->lblBoardSiteKpcValueMaxPassed->show();
                ui->lblBoardSiteKpcValueMaxAnswerLabel->show();

                ui->frameBoardSiteIO->show();
                ui->frameBoardSiteIO->activateWindow();
                ui->frameBoardSiteIO->raise();

                //-------------------------------------------------------------
                // SOF-3516: Boardsite progress
                //           Show the last message that may have come in before
                //           the screen was ready to process it
                //-------------------------------------------------------------
                DomBoardSiteTestsProgressMsg *progressFields = &session_->scaleConfig->boardSiteProgress;

                if( progressFields )
                {
                    ui->lblCurrentTestNameData->setText( progressFields->testDescription() );
                    ui->lblOverallTestProgressData->setText( progressFields->percentComplete() + "%" );

                    LOG_DEBUG("setManufacturingTestMode(): Test Progress from scaleConfig: Test=%s; Percent=%s", progressFields->testDescription().toLatin1().constData(), progressFields->percentComplete().toLatin1().constData() );
                }
            }

            ui->lblScreenMainTitle->setText( "Board Level Tests" );

            //-----------------------------------------------
            // Need to remove the tabs from right-to-left ...
            //-----------------------------------------------

            if(!isPeripheralBoardTestMode)
            {

                // Remove the service diagnostic tab
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->serviceDiagTab ) );

                // SOF-232: Remove the SSD tab (i.e. SATA)
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->sataTab ) );

                // SOF-722: If Depopulated board is being tested, remove Weigher, Printer and Fan tabs
                if( isDepopBoardTestMode )
                {
                    LOG_DEBUG( "setManufacturingTestMode(): isDepopBoardTestMode=TRUE", NULL );

                    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->weigherTab ) );
                    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->printerTab ) );
                    ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->fanTab ) );
                }
                else
                {
                    LOG_DEBUG( "setManufacturingTestMode(): isDepopBoardTestMode=FALSE", NULL );

                    // Hide the Stops and Calibration sub-tabs on the Weigher tab
                    ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->stopsPage ) );
                    ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->calibrationPage ) );

                    // SOF-232 Hide the Sensors tab on the Printer tab
                    ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->sensorsTab ) );

                    //if( cutterAttached_ )
                    //{
                    //    ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->cutterTab ) );
                    //}

                    // SOF-232 Hide the fields for the Vertical Line Test Pattern
                    ui->startVerticalLinePatternTestButton->hide();
                    ui->label_31->hide();
                    ui->gbVerticalLinePatternTestAnswers->hide();
                }

                // SOF-930: Hide testing of 2nd SD card slot on I/O tab
                ui->label_30->hide();
                ui->startIOSataTestButton->hide();
                ui->label_37->hide();
                ui->sataAnswerLabel_IO->hide();

                // Hide the Service Diagnostic portion of the Network tab
                ui->gbServiceDiagWiredNetworkTest->hide();
                ui->gbServiceDiagWirelessNetworkTest->hide();

                // Hide label and button on the Bluetooth tab
                ui->label_19->hide();
                ui->startBtTestButton->hide();

                // SOF-232: Remove the Time tab
                //ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->dateTimeTab ) ); // SOF-889 ... Put it back in ...

                // In Board Test mode the SSD tab is getting cut off on the screen, so
                // text for Network tab is being shortened
                ui->testTabsWidget->insertTab( 4,  ui->networkTab,       "Net" );

                //------------------------------------------------
                // SOF-237: Disable all fields on Flash tab except
                // Board Revision Number for "Board Test" mode
                //------------------------------------------------
                ui->sysCtrlManufactureDateLineEdit->setDisabled( true );
                ui->sysCtrlSerialNumberLineEdit->setDisabled( true );
                ui->sysCtrlStyleLineEdit->setDisabled( true );
                ui->sysCtrlBoardAssemblyNumberLineEdit->setDisabled( true );

                // SOF-232 Do not show Self-Service section on USB tab for Board Test mode
                ui->lblSelfService->setVisible( false );
                ui->gbSelfServiceAnswers->hide();

                // SOF-232: Show the Touch screen calibration button for Board Test only
                ui->startCalibrationTestButtton->setVisible( true );

                // HT-387: Set the default tab icons for the tabs that are visible
                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),failIcon);
                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),failIcon);
                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->displayTab),failIcon);
                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),failIcon);
                ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wiredTab),failIcon);
                ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wirelessTab),failIcon);
                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),failIcon);
                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),failIcon);

                if( !isDepopBoardTestMode )
                {
                    ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->fanTab),failIcon);
                    ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),failIcon);
                    ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),failIcon);
                    ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),failIcon);
                    ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->countsPage),failIcon);
                }

                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->canbusTab),failIcon);
            }
            else
            {
                //SOF-3752
                //Atmel peripheral Bd. 00-447780
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->serviceDiagTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->sataTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->canbusTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->bluetoothTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->audioTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->networkTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->displayTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->usbTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->flashTab ) );
                ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->dateTimeTab ) );

                // Hide the Stops and Calibration sub-tabs on the Weigher tab
                ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->stopsPage ) );
                ui->weigherTestsTab->removeTab( ui->weigherTestsTab->indexOf( ui->calibrationPage ) );

                // SOF-232 Hide the Sensors tab on the Printer tab
                ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->sensorsTab ) );

                // SOF-3763 Hide the Cassette Test sub-tab on the Printer tab
                ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->cassetteTestTab ) );

                //if( cutterAttached_ )
                //{
                //    ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->cutterTab ) );
                //}

                // SOF-232 Hide the fields for the Vertical Line Test Pattern
                ui->startVerticalLinePatternTestButton->hide();
                ui->label_31->hide();
                ui->gbVerticalLinePatternTestAnswers->hide();

                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->fanTab),failIcon);
                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->printerTab),failIcon);
                ui->printerTestsTab->setTabIcon(ui->printerTestsTab->indexOf(ui->testLabelTab),failIcon);
                ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->weigherTab),failIcon);
                ui->weigherTestsTab->setTabIcon(ui->weigherTestsTab->indexOf(ui->countsPage),failIcon);

            }
            break;

        case ManufTestPrepack:		//	 No pre-test and final test for Prepack.

            ui->lblScreenMainTitle->setText( "Manufacturing Test for Prepack" );

            // SOF-3519
            weigherCalibrate_ = new hobart::weigherCalibrate( this, mainWindow_, 0 );
            ui->calibrationVerticalLayout->addWidget(weigherCalibrate_);

            //-----------------------------------------------
            // Need to remove the tabs from right-to-left ...
            //-----------------------------------------------

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->serviceDiagTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->sataTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->weigherTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->printerTab ) );

            ui->testTabsWidget->removeTab( ui->testTabsWidget->indexOf( ui->fanTab ) );

            // On the CAN tab, need to change the wording of the instructions if a BCM board
            if (boardManf == "BCM" )
                ui->label_4->setText( tr( "Connect CANbus cable from Blackbox to board then start CANbus Test" ) );

            // Hide the Service Diagnostic portion of the Network tab
            ui->gbServiceDiagWiredNetworkTest->hide();
            ui->gbServiceDiagWirelessNetworkTest->hide();

            // FT-1335
            // Hide label and button on the Bluetooth tab
            ui->label_19->hide();
            ui->startBtTestButton->hide();

            // Do not show Self-Service section on USB tab for Prepack
            ui->lblSelfService->setVisible( false );
            ui->gbSelfServiceAnswers->hide();

            // SOF-2561: Hide testing of 2nd SD card slot on I/O tab for wrapper
            ui->label_30->hide();
            ui->startIOSataTestButton->hide();
            ui->label_37->hide();
            ui->sataAnswerLabel_IO->hide();

            // HT-387: Set the default tab icons for the tabs that are visible
            ui->testTabsWidget->setTabIcon( ui->testTabsWidget->indexOf( ui->dateTimeTab ), failIcon );
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->flashTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->usbTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->displayTab),failIcon);

            if( session_->isPrepack() )
            {
                // No Customer Display ...
                // SOF-304 - Hide fields for Customer Display test
                ui->lblCustomerDisplayTestMessage->hide();
                ui->gbCustomerDisplayAnswers->hide();
            }

            // SOF-232
            ui->startCalibrationTestButtton->setVisible( false );

            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->networkTab),failIcon);
            ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wiredTab),failIcon);
            ui->networkTypeTab->setTabIcon(ui->networkTypeTab->indexOf(ui->wirelessTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->audioTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->bluetoothTab),failIcon);
            ui->testTabsWidget->setTabIcon(ui->testTabsWidget->indexOf(ui->canbusTab),failIcon);

            break;
    }

    // Default to the first tab
    ui->testTabsWidget->setCurrentIndex( 0 );

    // FT-39 / FT-1149
    numberOfTabs = ui->testTabsWidget->count();
    currentTabOffset = ui->testTabsWidget->currentIndex();

    // Initialize the status of each tab to 0 (i.e. fail)
    std::fill_n( tabStatus, numberOfTabs, 0 );

    // Update tabs and sub-tabs based on Style (Model Name)
    setTabsBasedOnStyle();
    runProcesses();
    //scaleScreen();
}

//--------------------------------------------
// SOF-722: Ask if testing a Depopulated board
//--------------------------------------------
bool manufacturingTest::checkIfDepopBoard()
{
    LOG_CLASS_INFO( "manufacturingTest", "checkIfDepopBoard" );

    LOG_DEBUG( "Top of checkIfDepopBoard()", NULL );

    bool rval = false;

    LOG_DEBUG( "checkIfDepopBoard(): Default=FALSE", NULL );

    MessageBoxDialog dialog( real_time_priority, mainWindow_ );

    dialog.setAllowOverlayClickDismiss( false );
    dialog.setTitle( "SELECT BOARD TESTER MODE" );
    dialog.setText( "\nAre you going to test a Depop board\n(Part # 00-448431 or Part # 00-448432)?" );

    dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

    int result = dialog.exec();

    if ( QDialog::Accepted == result )
    {
        rval = true;

        LOG_DEBUG( "checkIfDepopBoard(): CurrentValue=TRUE", NULL );
    }

    return rval;
}

//----------------------------------------
// Get the Scale Model: HTi, HTx, HTs, ...
//----------------------------------------
QString manufacturingTest::getScaleModel()
{
//    LOG_CLASS_INFO( "manufacturingTest", "getScaleModel" );

    initialScaleModelName = session_->scaleConfig->initialScaleModelName;
    /*
    python pythonProcess;
    QStringList args;

    if ( pythonProcess.execCommand("getModelName", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            initialScaleModelName = pythonProcess.readStdOut();

            LOG_DEBUG( "Initial Scale Model is %s", qPrintable( initialScaleModelName ) );
        }
        else
        {
            LOG_DEBUG( "Cannot get Scale Model. Exit code = %d", pythonProcess.getExitCode() );
        }
    }
    */
    return initialScaleModelName;
}

//----------------------------------------
// Gets the info programmed into the Flash:
//
//  Manufacturing Date
//  Serial Number
//  Model Name
//  Board Assembly Number
//  Board Revision Number
//
//----------------------------------------
void manufacturingTest::getFlashInfo()
{
//    LOG_CLASS_INFO( "manufacturingTest", "getFlashInfo" );

    flashInfo = session_->scaleConfig->flashInfo;
    /*
    QStringList args;

    args.append("--mfg");

    if ( pythonProcess.execCommand("getSerialFlashInfo", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            LOG_DEBUG( "getSerialFlashInfo successful", NULL );

            QString result = pythonProcess.readStdOut();
            LOG_DEBUG( "getSerialFlashInfo results=%s", result.toLatin1().constData() );
            flashInfo = result.split( "," );
        }
        else
        {
            LOG_DEBUG( "Cannot get Flash Info. Exit code = %d", pythonProcess.getExitCode() );
            //TEMP! DO NOT CHECK IN!
            // This is for testing on temp builds only
            //flashInfo << "First" << "This" << "is" << "a" << "list.";
        }
    }
    */
}

//----------------------------------------------------------------------
// Update the Date/Time label on the Time tab with the current date/time
//----------------------------------------------------------------------
void manufacturingTest::setDateTimeLabel()
{
    if( setDateTimeSuccessful )
    {
        QString txtDateTime( session_->uiUtilities->updateDateTime() );

        if( convertUppercase_ )
            txtDateTime = txtDateTime.toUpper();

        ui->lblCurrentDateTimeData->setText( txtDateTime );

        // FT-1376
        // Set Manufacturing Date on the Flash tab
        ui->sysCtrlManufactureDateLineEdit->setText(QDate::currentDate().toString("MM/dd/yyyy"));
    }
}

//--------------------------------------------------------------------------
// Update the Date/Time/Timezone for the System Clock and the Hardware Clock
//--------------------------------------------------------------------------
void manufacturingTest::on_SaveDateTimeButton_clicked()
{
    LOG_CLASS_INFO( "manufacturingTest", "SaveDateTime" );

    // Disable until finished with the Python call
    ui->btnSetDateTime->setDisabled( true );
    ui->btnSetDateTime->repaint();
    qApp->processEvents();

// FT-1328: Had to put reboot back in, due to need to enable openbox again
// FT-1330: Remove reboot after setting date/time in Manufacturing Test
    LOG_DEBUG( "Sending message to check for service key", NULL );

    // FT-1271
    // Send message telling backend to make sure there isn't a service key present
    //QString msg = "120080,3";
    //session_->send( Error, ErrVersionManagerDevice, msg );
    saveDateTimeAfterServiceKeyRemoved();
    // Have to request new time from backend as it is being set and redisplay it.
    //
// FT-1328: Had to put reboot back in, due to need to enable openbox again
//    // FT-1330: Moved logic to setup params and call setTime back into this function.
//    QStringList options;

//    // Format args to pass to setTime.py
//    options.append( "-Y" ); // Year
//    options.append( QString::number( ui->calendarWidget->selectedDate().year() ) );
//    options.append( "-M" ); // Month
//    options.append( QString("%1").arg( ui->calendarWidget->selectedDate().month(), 2, 10, QChar('0')).toUpper( ));
//    options.append( "-D" ); //Day
//    options.append( QString("%1").arg( ui->calendarWidget->selectedDate().day(), 2, 10, QChar('0')).toUpper() );
//    options.append( "-H" ); // Hour
//    options.append( QString("%1").arg( ui->timeEdit->time().hour(), 2, 10, QChar('0')).toUpper() );
//    options.append( "-m" ); // Minute
//    options.append( QString("%1").arg( ui->timeEdit->time().minute(), 2, 10, QChar('0')).toUpper() );
//    options.append( "-S" ); // Second
//    options.append( QString("%1").arg( ui->timeEdit->time().second(), 2 ,10, QChar('0')).toUpper() );
//    options.append( "-t" ); // Text of Timezone (e.g. America/Detroit)
//    options.append( QString("%1").arg( ui->cbTimeZone->itemData( ui->cbTimeZone->currentIndex() ).toString() ) );
//    options.append( "-A" );
//    options.append( "0" );

//    python setDateTimePythonProcess;

//    if ( setDateTimePythonProcess.execCommand( "setTime", options ))
//    {
//        if ( setDateTimePythonProcess.getExitCode() == 0 )
//        {
//            LOG_DEBUG( "setTime successful", NULL );

//            // Update setup values in the scalesettings DB file
//            updateDB("timezone", ui->cbTimeZone->itemData(ui->cbTimeZone->currentIndex()).toString());
//            updateDB("ntpTimeZone", (ui->cbTimeZone->itemData(ui->cbTimeZone->currentIndex()).toString()));

//            // Update the Date/Time on the Time tab
//            setDateTimeLabel();

//            ui->btnSetDateTime->setEnabled( true );
//        }
//        else
//        {
//            LOG_DEBUG( "setTime Failed, exit code %d", pythonProcess.getExitCode() );

//            ui->btnSetDateTime->setEnabled( true );
//        }
//    }
}

// FT-1328: Had to put reboot back in, due to need to enable openbox again
// FT-1330: Remove reboot after setting date/time in Manufacturing Test
// FT-1271
//--------------------------------------------------------------------------
// Update the Date/Time/Timezone for the System Clock and the Hardware Clock
//--------------------------------------------------------------------------
void manufacturingTest::saveDateTimeAfterServiceKeyRemoved()
{
    LOG_CLASS_INFO( "manufacturingTest", "saveDateTimeAfterServiceKeyRemoved" );


    QStringList options;

    // Disable until finished with the Python call
    ui->btnSetDateTime->setDisabled( true );
    ui->btnSetDateTime->repaint();
    qApp->processEvents();

    // Format args to pass to setTime.py
    options.append( "-Y" ); // Year
    options.append( QString::number( ui->calendarWidget->selectedDate().year() ) );
    options.append( "-M" ); // Month
    options.append( QString("%1").arg( ui->calendarWidget->selectedDate().month(), 2, 10, QChar('0')).toUpper( ));
    options.append( "-D" ); //Day
    options.append( QString("%1").arg( ui->calendarWidget->selectedDate().day(), 2, 10, QChar('0')).toUpper() );
    options.append( "-H" ); // Hour
    options.append( QString("%1").arg( ui->timeEdit->time().hour(), 2, 10, QChar('0')).toUpper() );
    options.append( "-m" ); // Minute
    options.append( QString("%1").arg( ui->timeEdit->time().minute(), 2, 10, QChar('0')).toUpper() );
    options.append( "-S" ); // Second
    options.append( QString("%1").arg( ui->timeEdit->time().second(), 2 ,10, QChar('0')).toUpper() );
    options.append( "-t" ); // Text of Timezone (e.g. America/Detroit)
    options.append( QString("%1").arg( ui->cbTimeZone->itemData( ui->cbTimeZone->currentIndex() ).toString() ) );
    options.append( "-A" );
    options.append( "0" );

    python setDateTimePythonProcess;

    if ( setDateTimePythonProcess.execCommand( "setTime", options ))
    {
        if ( setDateTimePythonProcess.getExitCode() == 0 )
        {
            LOG_DEBUG( "setTime successful", NULL );

            // Update setup values in the scalesettings DB file
            updateDB("timezone", ui->cbTimeZone->itemData(ui->cbTimeZone->currentIndex()).toString());
            updateDB("ntpTimeZone", (ui->cbTimeZone->itemData(ui->cbTimeZone->currentIndex()).toString()));

            setDateTimeSuccessful = true;   // SOF-891

            // Update the Date/Time on the Time tab
            setDateTimeLabel();

            if( ui->btnSetDateTime->isVisible() )
                ui->btnSetDateTime->setEnabled( true );

            //reboot();
        }
        else
        {
            LOG_DEBUG( "setTime Failed, exit code %d", setDateTimePythonProcess.getExitCode() );

            // SOF-891
            ui->lblCurrentDateTimeData->setText( tr( "Set Date/Time FAILED" ) );

            setDateTimeSuccessful = false;   // SOF-891

            if( ui->btnSetDateTime->isVisible() )
                ui->btnSetDateTime->setEnabled( true );
        }
    }
    ui->btnSetDateTime->setEnabled(true);
}

// FT-1328: Had to put reboot back in, due to need to enable openbox again
// FT-1330: Remove reboot after setting date/time in Manufacturing Test
// FT-1271
void manufacturingTest::reboot()
{
    LOG_CLASS_INFO("manufacturingTest", "reboot");

    LOG_DEBUG( "Sending reboot request", NULL );

    // Send Reboot request to backend
    session_->send( SessionDev, SnShutdownRequest, "5" );
}

//------------------------------------------------------------
// Populate the Timezone dropdown with list of valid timezones
//------------------------------------------------------------
void manufacturingTest::populateTimeZones()
{
    this->ui->cbTimeZone->clear();
    this->ui->cbTimeZone->addItem( "(UTC+00:00) Dublin, Edinburgh, Lisbon, London", QString( "Europe/London" ) );
    this->ui->cbTimeZone->addItem( "(UTC+01:00) Brussels, Copenhagen, Madrid, Paris", QString( "Europe/Paris" ) );
    this->ui->cbTimeZone->addItem( "(UTC+02:00) Cairo", QString( "Africa/Cairo" ) );
    this->ui->cbTimeZone->addItem( "(UTC+03:00) Moscow, St. Petersburg, Volgograd", QString( "Europe/Moscow" ) );
    this->ui->cbTimeZone->addItem( "(UTC+04:00) Abu Dhabi, Muscat", QString( "Asia/Muscat" ) );
    this->ui->cbTimeZone->addItem( "(UTC+05:00) Islamabad, Karachi", QString( "Asia/Karachi" ) );
    this->ui->cbTimeZone->addItem( "(UTC+06:00) Dhaka", QString( "Asia/Dhaka"  ));
    this->ui->cbTimeZone->addItem( "(UTC+07:00) Bangkok, Hanoi, Jakarta", QString( "Asia/Jakarta" ) );
    this->ui->cbTimeZone->addItem( "(UTC+08:00) Taipei", QString( "Asia/Taipei" ) );
    this->ui->cbTimeZone->addItem( "(UTC+09:00) Osaka, Sapporo, Tokyo", QString( "Asia/Tokyo" ) );
    this->ui->cbTimeZone->addItem( "(UTC+10:00) Brisbane", QString( "Australia/Brisbane" ) );
    this->ui->cbTimeZone->addItem( "(UTC+11:00) Solomon Is., New Caledonia", QString( "Pacific/Noumea" ) );
    this->ui->cbTimeZone->addItem( "(UTC+12:00) Auckland, Wellington", QString( "Pacific/Auckland" ));
    this->ui->cbTimeZone->addItem( "(UTC-01:00) Cape Verde Is.", QString( "Atlantic/Cape_Verde" ) );
    this->ui->cbTimeZone->addItem( "(UTC-02:00) Mid-Atlantic", QString( "Atlantic/Noronha" ) );
    this->ui->cbTimeZone->addItem( "(UTC-03:00) Greenland", QString( "America/Godthab" ) );
    this->ui->cbTimeZone->addItem( "(UTC-04:00) Atlantic Time (Canada)", QString( "America/Halifax" ) );
    this->ui->cbTimeZone->addItem( "(UTC-05:00) Eastern Time (US & Canada)", QString( "America/New_York" ) );
    this->ui->cbTimeZone->addItem( "(UTC-06:00) Central Time (US & Canada)", QString( "America/Chicago" ) );
    this->ui->cbTimeZone->addItem( "(UTC-07:00) Arizona", QString( "America/Phoenix"  ));
    this->ui->cbTimeZone->addItem( "(UTC-07:00) Mountain Time (US & Canada)", QString( "America/Denver" ) );
    this->ui->cbTimeZone->addItem( "(UTC-08:00) Pacific Time (US & Canada)", QString( "America/Los_Angeles" ) );
    this->ui->cbTimeZone->addItem( "(UTC-09:00) Alaska", QString( "America/Anchorage" ) );
    this->ui->cbTimeZone->addItem( "(UTC-10:00) Hawaii", QString( "Pacific/Honolulu" ) );
    this->ui->cbTimeZone->addItem( "(UTC-11:00) Coordinated Universal Time-11", QString( "Pacific/Midway" ) );
    //---------------------------------------------------------------------------------------------
    // "Habitations in this time zone use neighboring timezone based on political owner or region."
    //  (See http://en.wikipedia.org/wiki/UTC%E2%88%9212:00),
    //  so Linux does not have an entry for it, reassign to Pacific/Midway
    //---------------------------------------------------------------------------------------------
    this->ui->cbTimeZone->addItem( "(UTC-12:00) International Date Line West",QString( "Pacific/Midway" ) );
}

//-------------------------------------------------
// Read setup values from the scalesettings DB file
//-------------------------------------------------
void manufacturingTest::connectDB()
{
    QString databaseFileName = "settingswizard/scalesettings";
    databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

    QFile file(databaseFileLocation);
    file.open( QIODevice::ReadOnly | QIODevice::Text );

    QStringList theLine;
    QTextStream in( &file );

    while ( !in.atEnd() )
    {
        QString line = in.readLine();
        theLine = line.split("=", QString::KeepEmptyParts);

        if(theLine.size() > 1)
        {
           if( theLine.at(0) == "timezone" )
               dbTimezone = theLine.at(1);

           // FT-1269 / FT-1272
           if( theLine.at(0) == "connectionType" )
               dbConnectionType = theLine.at(1);
        }
        else
        {
           //------------------
           // Use Default Value
           //------------------

           if( theLine.at(0) == "timezone" )
               dbTimezone = "America/Detroit";
        }
   }

    ui->cbTimeZone->setCurrentIndex( ui->cbTimeZone->findData( dbTimezone ) );

    // update network tab based on dbConnectionType
//    getTestStatusInternal();
    initLabelsOnDataReceived();
    file.close();
}

//-------------------------------------------------
// Update setup values in the scalesettings DB file
//-------------------------------------------------
bool manufacturingTest::updateDB(QString key, QString value)
{
    LOG_CLASS_INFO( "manufacturingTest", "updateDB" );

    QString databaseFileName = "settingswizard/scalesettings";
    QString databaseTempFileName = "settingswizard/scalesettingstemp";
    QString databaseBackupFileName = "settingswizard/scalesettingsBACKUP";
    databaseFileLocation = filesystem::absoluteFilePath( databaseFileName, folderType::DBs );
    QString databaseTempFileLocation = filesystem::absoluteFilePath( databaseTempFileName, folderType::DBs );
    QString databaseBackupFileLocation = filesystem::absoluteFilePath( databaseBackupFileName, folderType::DBs );
    QFile file(databaseFileLocation);

    file.open( QIODevice::ReadOnly | QIODevice::Text );

    QStringList theLine;
    QStringList fileContents;
    QTextStream in(&file);
    QString line;
    fileContents.clear();

    while (!in.atEnd())
    {
        line = in.readLine();
        theLine = line.split( "=", QString::KeepEmptyParts);

        if(theLine.at(0) == key)
        {
            line = key + "=" + value;
        }

        fileContents.append(line);
    }

    file.close();
    QFile fileTemp(databaseTempFileLocation);

    fileTemp.open( QIODevice::WriteOnly | QIODevice::Text );

    QTextStream outStream(&fileTemp);

    for(int i = 0; i < fileContents.count(); i++)
    {
        outStream << fileContents.at(i) + "\n";
    }

    fileTemp.close();
    QDir dir;
    dir.remove(databaseFileLocation);

    if (QFile::exists(databaseBackupFileLocation))
    {
        dir.remove(databaseBackupFileLocation);
    }

    QFile::copy(databaseTempFileLocation,databaseBackupFileLocation);
    dir.rename(databaseTempFileLocation,databaseFileLocation);

    python pythonProcess;
    QStringList args;

    if (pythonProcess.execCommand( "sync", args ) )
    {
        if (pythonProcess.getExitCode() == 0)
        {
             LOG_DEBUG("sync Successful", NULL );
        }
        else
        {
            LOG_DEBUG("sync Failed, exit code %d", pythonProcess.getExitCode() );
        }
    }
    else
    {
        LOG_DEBUG("Could not execCommand for sync", NULL );
    }

    return true;
}

//---------------------------------------------------------
// Get the MAC addresses of the ethernet and wireless card,
// and update the display fields on the Network tab
//---------------------------------------------------------
void manufacturingTest::getMACAddress()
{
    LOG_CLASS_INFO( "manufacturingTest", "getMACAddress" );

    //QStringList args;

    QStringList macAddresses = session_->scaleConfig->macAddressFromScript.split("|");

    if(macAddresses.size() > 0 )
    {
        ethernetMacAddress = ( macAddresses[0].trimmed() ).toUpper();
        wirelessMacAddress = ( macAddresses[1].trimmed() ).toUpper();
        LOG_DEBUG("Ethernet MAC Address is %s", ethernetMacAddress.toLatin1().constData());
        LOG_DEBUG("Wireless MAC Address is %s", wirelessMacAddress.toLatin1().constData());
    }
    else
    {
        ethernetMacAddress = "N/A";
        wirelessMacAddress = "N/A";
        LOG_DEBUG("Ethernet MAC Address is %s", ethernetMacAddress.toLatin1().constData());
        LOG_DEBUG("Wireless MAC Address is %s", wirelessMacAddress.toLatin1().constData());

    }
    //python pythonProcess;
    //if ( pythonProcess.execCommand("getMac", args, false, false, ( 30 * 1000 ) ) )
    //{
        /*
        if ( pythonProcess.getExitCode() == 0 )
        {
            LOG_DEBUG( "get MAC address successful", NULL );

            QString result = pythonProcess.readStdOut();
            QStringList macAddresses = result.split( "|" );

            // FT-1610
            ethernetMacAddress = ( macAddresses[0].trimmed() ).toUpper();
            wirelessMacAddress = ( macAddresses[1].trimmed() ).toUpper();
            LOG_DEBUG("Ethernet MAC Address is %s", ethernetMacAddress.toLatin1().constData());
            LOG_DEBUG("Wireless MAC Address is %s", wirelessMacAddress.toLatin1().constData());
        }
        else
        {
            // FT-1610
            ethernetMacAddress = "N/A";
            wirelessMacAddress = "N/A";
            LOG_DEBUG("Ethernet MAC Address is %s", ethernetMacAddress.toLatin1().constData());
            LOG_DEBUG("Wireless MAC Address is %s", wirelessMacAddress.toLatin1().constData());

            LOG_DEBUG( "Cannot get MAC addresses. Exit code = %d", pythonProcess.getExitCode() );
        }

    }
    */
}

//-----------------------------------------------------------------
// Get the Gateway IP address and DHCP Server IP address and update:
//
// 1) The IP Address field to on Network tab that is visible
//    when in "Manufacturing Test" mode
//
// 2) The display fields on the Network tab that are visible
//    when in Service Diagnostic mode
//-----------------------------------------------------------------
void manufacturingTest::getNetworkInfo()
{
    LOG_CLASS_INFO( "manufacturingTest", "getNetworkInfo" );


    netInfo = session_->scaleConfig->netInfo;
    //python pythonProcess;
    // QStringList args;

    /*
    if ( pythonProcess.execCommand("getNetworkInfo", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            LOG_DEBUG( "getNetworkInfo successful", NULL );

            QString result = pythonProcess.readStdOut();
            LOG_DEBUG( "getNetworkInfo results=%s", result.toLatin1().constData() );
            netInfo = result.split( "," );
        }
        else
        {
            LOG_DEBUG( "getNetworkInfo failed. Exit code = %d", pythonProcess.getExitCode() );
        }
    }
    */
}

// SOF-1773
void manufacturingTest::getUbootVersionFromUsd()
{
    LOG_CLASS_INFO( "manufacturingTest", "getUbootVersionFromUsd" );

    python pythonProcess;
    QStringList args;

    newLoaderVer = "";

    if ( pythonProcess.execCommand("getUbootUsdVersion", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            LOG_DEBUG( "get Uboot Version from USD successful", NULL );

            QString result = pythonProcess.readStdOut();
            LOG_DEBUG( "Uboot version on USD card=%s", result.toLatin1().constData() );

            if( result != "None" )
                newLoaderVer = result;

            LOG_DEBUG( "getUbootVersionFromUsd: newLoaderVer=%s", newLoaderVer.toLatin1().constData() );
        }
    }
}

// FT-39 / FT-1149
//------------------------------------------------
// Checks the Pass/Fail status of the current tab,
// and advances to the next tab
//------------------------------------------------
void manufacturingTest::autoWalkThruTabs()
{
    LOG_CLASS_INFO( "manufacturingTest", "autoWalkThruTabs" );

    if( currentManfTestMode == ServiceDiagnosticMode )
    {
        #if 0 //SOF-3999 why are we running dotwear test in service dignostics?
              //Nothing on the page to display the results....

        // SOF-3518: Need to delay requesting the Printer Dot Status
        if( ( codeCheckerEnabled_ ) &&
            ( !requestPrinterDotStatusAttempted ) )
        {
            requestPrinterDotStatus();
        }

        #endif

        // SOF-4765: Mark the USB tab as "Passed" for Service Diagnostics mode when the device has an ARA board
        if( ( boardManf == "ARA") && ( autoCompleteUsbTab ) )
        {
            ui->usbMouseRecognizedAnswerLabel->setText( "Yes" );
            ui->usbBarcodeScannerRecognizedAnswerLabel->setText( "Yes" );
        }

        // Per Darrell - Service Diagnostic mode should not automatically walk through the tabs
        return;
    }

    LOG_DEBUG( "autoWalkThruTabs:  numberOfTabs=%d, currentTabOffset=%d", numberOfTabs, currentTabOffset );
    LOG_DEBUG( "autoWalkThruTabs:  autoCompleteUsbTab=%d", autoCompleteUsbTab );

    int dbgLoop;
    QString dbgString;
    for (dbgLoop = 0; dbgLoop < numberOfTabs; ++dbgLoop )
    {
        dbgString += "|";
        dbgString += (tabStatus[dbgLoop] == 1 ? "1" : "0" );
    }
    dbgString += "|";
    LOG_DEBUG( "tabStatus=%s", dbgString.toLatin1().constData() );

    // SOF-2717
    // Need to wait for the current FPC version(s) to be populated
    if ( ( currentManfTestMode == ManufTestPreTest ) &&
         ( currentTabOffset == 0 ) &&
         ( ( periphControllerVer == "" ) || ( periphControllerVerWeigher == "" ) ) )
    {
        return;
    }

    // SOF-2717

    /* See SOF-5997 - initiatePrinterWeigherReset
    if ( ( currentManfTestMode == ManufTestPreTest ) &&
         ( currentTabOffset == 0 )&&
         ( !factoryResetAttempted ) &&
         ( !factoryResetDone ) &&
         ( !loadCellCalibrationFailed ) )
    {
        resetFactoryDefaults();
    }
    */

    // SOF-3518: Need to delay requesting the Printer Dot Status
    if( ( codeCheckerEnabled_ ) &&
        ( currentManfTestMode == ManufTestPreTest ) &&
        ( currentTabOffset == 1 ) &&
        ( !requestPrinterDotStatusAttempted ) )
    {
        requestPrinterDotStatus();
    }

    //-------------------------------------------------------------------
    // ARK Boardsite - hide on-screen keyboard while background tests are
    //                 executing while on the Time tab
    //-------------------------------------------------------------------
    if( ( isArkBoardSite || isPeripheralBoardTestMode ) &&
        ( currentTabOffset == 0 ) )
    {
        mainWindow_->hideKeyboard();
    }


    if( currentTabOffset < numberOfTabs )
    {
        if( tabStatus[ currentTabOffset ] == 1 )
        {
            int weigherTabOffset = 0;

            // FT-1378
            int btTabOffset = 0;

            // FT-1418
            int usbTabOffset = 0;

            // SOF-3556
            int canBusTabOffset = 0;

            if ( noWeigher )
                weigherTabOffset = ui->testTabsWidget->indexOf( ui->weigherTab );

            // FT-1378 / FT-1335
            btTabOffset = ui->testTabsWidget->indexOf( ui->bluetoothTab );

            // FT-1418
            if ( autoCompleteUsbTab )
                usbTabOffset = ui->testTabsWidget->indexOf( ui->usbTab );

            // SOF-3556
            if( ( currentManfTestMode == ManufTestPrepack ) && ( noCanBus ) )
                canBusTabOffset = ui->testTabsWidget->indexOf( ui->canbusTab );

            currentTabOffset++;

            LOG_DEBUG( "Incremented currentTabOffset, =%d", currentTabOffset );

            if ( noWeigher )
            {
                if ( currentTabOffset == weigherTabOffset )
                {
                    //---------------------
                    // Skip the Weigher tab
                    //---------------------
                    currentTabOffset++;
                }
            }

            // FT-1378
            if ( noBluetooth )
            {
                if ( currentTabOffset == btTabOffset )
                {
                    //-----------------------
                    // Skip the Bluetooth tab
                    //-----------------------
                    currentTabOffset++;
                }
            }
            else // FT-1335
            {
                if ( currentTabOffset == btTabOffset )
                {
                    // Force the updating of the tab icon statuses

                    getTestStatus();
                }
            }

            // SOF-3556
            if ( ( currentManfTestMode == ManufTestPrepack ) && ( noCanBus ) )
            {
                if ( currentTabOffset == canBusTabOffset )
                {
                    //--------------------
                    // Skip the CANbus tab
                    //--------------------
                    currentTabOffset++;
                }
            }

            // FT-1418
            if ( autoCompleteUsbTab )
            {
                if ( currentTabOffset == usbTabOffset )
                {
                    //----------------------------------------------------------
                    // Auto-Complete tests so that the tab gets marked as Passed
                    // Skip the USB tab
                    //----------------------------------------------------------
                    ui->usbMouseRecognizedAnswerLabel->setText( "Yes" );
                    ui->usbBarcodeScannerRecognizedAnswerLabel->setText( "Yes" );

                    usbTestStatus();

                    currentTabOffset++;
                }
            }

            if( currentTabOffset < numberOfTabs )
            {
                ui->testTabsWidget->setCurrentIndex( currentTabOffset );

                if( DBG_AUTO_COMPLETE_ALL_TABS || DBG_AUTO_COMPLETE_WEIGHER_TAB || DBG_AUTO_COMPLETE_NETWORK_TAB )
                {
                    getTestStatus();
                }
            }
            else
            {
                //FT-40
                //--------------------------------------------------------------------------------------
                // Completed all of the tabs.  Ask if the test operator wants to print a Summary report.
                //--------------------------------------------------------------------------------------

                // Make sure all of the tabs still have a Pass status
                bool allPassed = true;

                for ( int loop = 0; loop < numberOfTabs; ++ loop )
                {
                    if ( noWeigher )
                    {
                        if ( loop == weigherTabOffset )
                        {
                            //----------------------------------
                            // Don't check if Weigher tab passed
                            //----------------------------------
                            continue;
                        }
                    }

                    // FT-1378
                    if ( noBluetooth )
                    {
                        if ( loop == btTabOffset )
                        {
                            //------------------------------------
                            // Don't check if Bluetooth tab passed
                            //------------------------------------
                            continue;
                        }
                    }

                    // SOF-3556
                    if ( noCanBus )
                    {
                        if ( loop == canBusTabOffset )
                        {
                            //--------------------------------------------
                            // Don't check if CANbus tab (i.e. I/O) passed
                            //--------------------------------------------
                            continue;
                        }
                    }

                    if ( tabStatus [ loop ] == 0 )
                        allPassed = false;
                }

                if ( allPassed )
                {
                    MessageBoxDialog dialog( real_time_priority, mainWindow_ );

                    dialog.setAllowOverlayClickDismiss( false );

                    if (currentManfTestMode == BoardTest)
                    {
                        dialog.setText( "\nAll Tests Passed. Scan QR code for MAC Address.\nPress OK to print a Test Report and Shutdown." );
                        dialog.setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

                        //SOF-4399
                        //Generate QRCode from Ethernet MAC Address
                        bitmap b(100, 100);

                        qrapi::encode( ethernetMacAddress.toStdString(), qr::LevelQ, b, 0, 0, 100, 100);
                        QTemporaryFile f;
                        f.setAutoRemove( true );

                        if ( f.open() )
                        {
                            const QString filename( f.fileName() );

                            const buffer img( b.toImage( "png" ) );
                            img.save( filename.toStdString() );

                            dialog.setIcon( QPixmap( filename ) );

                            f.close();
                        }

                        int result = dialog.exec();

                        if ( QDialog::Accepted == result )
                                on_exitPushButton_clicked();
                    }
                    else
                    {
                        if( ( currentManfTestMode == ManufTestPreTest && factoryResetDone ) ||
                            ( currentManfTestMode != ManufTestPreTest ) )
                        {
                            //-------------------------------------------------------------------------------------
                            // Resetting the Weigher & Printer to Factory Defaults was Successful OR Not Applicable
                            //-------------------------------------------------------------------------------------
                            dialog.setText( "All Tests Passed.\n\nPress OK to print a Test Report." );
                            dialog.setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

                            int result = dialog.exec();

                            if ( QDialog::Accepted == result )
                                    on_reportPushButton_clicked();

                            //----------------------------------------------------------------
                            // Need to exit Service Mode after re-setting the Weigher to
                            // Factory Defaults.  This forces the CAT-I W&M Configuration
                            // Parameters and Last Changed date to be updated by the scale
                            // application. To accomplish this we return to
                            // Home (i.e. PluMainScreen).
                            //
                            // Note:  This is being done for all modes where the Summary
                            //        Report is printed.
                            //----------------------------------------------------------------
                            mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();
                        }
                    }
                }
            }
        }
    }
}

// v1 = fpcVersion, v2 = compareVersion (5.6.0)
bool manufacturingTest::compareSoftwareVersions(QString v1, QString v2)
{
    LOG_CLASS_INFO( "manufacturingTest", "compareSoftwareVersions" );

    LOG_DEBUG( "compareSoftwareVersions(): v1: %s", v1.toLatin1().constData() );
    LOG_DEBUG( "compareSoftwareVersions(): v2: %s", v2.toLatin1().constData() );

    if( 0 == v1.length() )
    {
        LOG_DEBUG( "compareSoftwareVersions(): FALSE-1", NULL );
        return false;
    }

    if( v1.count(".") == 1 )    // if we get a 5.1 or something
        v1.append(".0");        // make sure we are comparing #.#.# to #.#.#

    // alright, lets split them up and compare the digits 1 by 1
    QStringList v1List( v1.split( "." ) );
    QStringList v2List( v2.split( "." ) );

    int v1Num = 0;  // originally I was comparing the element digits themselves. But this method (maybe) was crashing some
    int v2Num = 0;  // where so I pulled these out so I could see and debug them and print out debug on them easier

    for( int j = 0; j < v1List.size(); j++ )
    {
        v1Num = v1List[j].toInt();
        v2Num = v2List[j].toInt();

        if( v1Num > v2Num )
            return true;
        else if( v1Num < v2Num )
        {
            LOG_DEBUG( "compareSoftwareVersions(): FALSE-2", NULL );
            return false;
        }
    }

    // if you got to here it means that the peeled digits were equal all the way through
    // so return true because the original test was if fpc >= 5.6.  and is equal so true

    return true;
}

// SOF-1296
bool manufacturingTest::resetFactoryDefaults()
{
    LOG_CLASS_INFO( "manufacturingTest", "resetFactoryDefaults" );

    if( DBG_SKIP_FACTORY_DEFAULTS )
    {
        factoryResetDone = true;
        return true;
    }

    factoryResetAttempted = true;

    bool rval = true;
    bool weigherResetRval = false;
    bool printerResetRval = false;

    LOG_DEBUG( "Top of resetFactoryDefaults()", NULL );

    // SOF-1296
    if( currentManfTestMode == ManufTestPreTest && (boardManf == "ARA" || boardManf == "ARK") )
    {
        QString fpcVersion = periphControllerVer;
        QString fpcVersionWeigher = periphControllerVerWeigher;
        QString stdVersion = "5.6.0";

        LOG_DEBUG( "resetFactoryDefaults(): fpcVersion: %s", fpcVersion.toLatin1().constData() );
        LOG_DEBUG( "resetFactoryDefaults(): fpcVersionWeigher: %s", fpcVersionWeigher.toLatin1().constData() );

        // SOF-2837 ... Need to compare against a float
        // 8/29/2019 - No longer true, need to compare if the current 3 digit version is > 5.6
        // 8/29/2019 - SOF-3047 - support 3 digit version numbers supercedes SOF-2837

        // new compare method to compare the current fpcVerion against 5.6.0
        bool isNewerWeigher = compareSoftwareVersions( fpcVersionWeigher, stdVersion );
        bool isNewerPrinter = compareSoftwareVersions( fpcVersion, stdVersion );

        if( ( isNewerWeigher ) || ( boardManf == "ARK" ) )
        {
            //--------------------------------------------
            // First reset the Weigher to Factory Defaults
            //--------------------------------------------

            MessageBoxDialog dialog( real_time_priority, mainWindow_ );

            dialog.setAllowOverlayClickDismiss( false );
            dialog.setModal( false );

            if( !noWeigher ) // NOT a "Printer-only" scale
            {
                LOG_DEBUG( "Ready to set Weigher to Factory Defaults ...", NULL );

                //-------------------------------------------------------------------------------------------------
                // Send message to reset the Weigher to Factory Defaults if:
                //   Running Pre-Test for Scale
                //   Board is an ARA board
                //   Scale is not a Printer-Only scale (i.e. has a load cell)
                //   The FPC version is 5.6 or newer (must be 5.6 or newer to be able to reset to factory defaults)
                //-------------------------------------------------------------------------------------------------

                weigherResetSuccessful = false;

                dialog.setTitle( "WEIGHER RESET" );
                dialog.setText( "Setting the Weigher to Factory Defaults." );
                dialog.setStandardButtons( QDialogButtonBox::NoButton );
                dialog.show();

                session_->send( Weigher, WgResetFactoryDefaults ); // 1,22
                session_->send( Weigher, WgReqInquire );           // Get the new weigher config

                // Wait for confirmation that the weigher was successfully reset

                /*
                 * TODO: I've seen testing this that in certain instances the response can take longer
                 * than the allotted time the timer is given, should we give an option to retry instead of
                 * having to reboot?
                 *
                 * For now, I have disconnected the timers and only confirm the weigher/printer reset when we receive
                 * the actual status message from the backend.
                 */

                startWeigherResetTimer = new QTimer( this );
                startWeigherResetTimer->setSingleShot( true );
                startWeigherResetTimer->setInterval( 3000 );
//                connect( startWeigherResetTimer, SIGNAL( timeout() ), SLOT( confirmWeigherReset() ) );
                startWeigherResetTimer->start();

                weigherResetTimerDone = false;

                while( !weigherResetTimerDone )
                {
                    sleepFor( 1 );
                }

                startWeigherResetTimer->stop();

                if( weigherResetSuccessful )
                {
                    sleepFor( 2000 );

                    dialog.setTitle( "WEIGHER RESET" );
                    dialog.setText( "Weigher was successfully set to Factory Defaults." );
                    dialog.setStandardButtons( QDialogButtonBox::NoButton );
                    dialog.show();

                    sleepFor( 3000 );

                    weigherResetRval = true;

                    LOG_DEBUG( "Weigher reset to Factory Defaults successful", NULL );
                }
                else
                {
                    dialog.setTitle( "WEIGHER RESET" );
                    dialog.setText( "Reset Weigher to Factory Defaults FAILED.\n\nPress OK to reboot." );
                    dialog.setStandardButtons( QDialogButtonBox::Ok );
                    dialog.exec();

                    weigherResetRval = false;

                    LOG_DEBUG( "Weigher reset to Factory Defaults FAILED", NULL );

                    // SOF-2717
                    reboot();
                    return false;
                }
            }
            else
            {
                // Automatically pass for "Printer-only"
                weigherResetRval = true;
            }
        }
        else
        {
            if( !noWeigher )
            {
                // Must have FPC version 5.6 or higher ...
                MessageBoxDialog dialog( real_time_priority, mainWindow_ );
                dialog.setAllowOverlayClickDismiss( false );

                dialog.setTitle( "WEIGHER RESET" );
                dialog.setText( "Must have Peripheral Controller Version 5.6 or higher.\n\nCurrent version is " + periphControllerVerWeigher + "\n\nPress OK to reboot." );
                dialog.setStandardButtons( QDialogButtonBox::Ok );
                dialog.exec();

                rval = false;

                LOG_DEBUG( "Invalid FPC Version: %s", fpcVersion.toLatin1().constData() );

                // SOF-2717
                reboot();
                return false;
            }
        }

        if( ( isNewerPrinter ) || ( boardManf == "ARK" ) )
        {
            //------------------------------------------------------------
            // SOF-1719: Now reset the Printer Weigher to Factory Defaults
            //------------------------------------------------------------

            printerResetSuccessful = false;

            MessageBoxDialog dialog( real_time_priority, mainWindow_ );

            dialog.setAllowOverlayClickDismiss( false );
            dialog.setModal( false );
            dialog.setTitle( "PRINTER RESET" );
            dialog.setText( "Setting the Printer to Factory Defaults." );
            dialog.setStandardButtons( QDialogButtonBox::NoButton );
            dialog.show();

            QString msg;
            msg = "0";  // Default printer
            session_->send( Printer, PrResetFactoryDefaults, msg ); // 7,57

            LOG_DEBUG( "Sent message to reset printer to factory default.", NULL );

            // Wait for confirmation that the printer was successfully reset
            startPrinterResetTimer = new QTimer( this );
            startPrinterResetTimer->setSingleShot( true );
            startPrinterResetTimer->setInterval( 3000 );
//            connect( startPrinterResetTimer, SIGNAL( timeout() ), SLOT( confirmPrinterReset() ) );
            printerResetTimerDone = false;
            startPrinterResetTimer->start();

            while( !printerResetTimerDone )
            {
                sleepFor( 1 );
            }

            startPrinterResetTimer->stop();

            if( printerResetSuccessful )
            {
                sleepFor( 2000 );

                dialog.setTitle( tr( "PRINTER RESET" ) );
                dialog.setText( tr( "Printer was successfully set to Factory Defaults." ) );
                dialog.setStandardButtons( QDialogButtonBox::NoButton );
                dialog.show();

                sleepFor( 3000 );

                printerResetRval = true;

                LOG_DEBUG( "Printer reset to Factory Defaults successful", NULL );
            }
            else
            {
                dialog.setTitle( tr( "PRINTER RESET" ) );
                dialog.setText( tr( "Reset Printer to Factory Defaults FAILED.\n\nPress OK to reboot." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );
                dialog.exec();

                printerResetRval = false;

                LOG_DEBUG( "Printer reset to Factory Defaults FAILED", NULL );

                // SOF-2717
                reboot();
                return false;
            }

            rval = weigherResetRval & printerResetRval;

            factoryResetDone = true;
        }
        else
        {
            // Must have FPC version 5.6 or higher ...
            MessageBoxDialog dialog( real_time_priority, mainWindow_ );
            dialog.setAllowOverlayClickDismiss( false );

            dialog.setTitle( "PRINTER RESET" );
            dialog.setText( "Must have Peripheral Controller Version 5.6 or higher.\n\nCurrent version is " + periphControllerVer + "\n\nPress OK to reboot." );
            dialog.setStandardButtons( QDialogButtonBox::Ok );
            dialog.exec();

            rval = false;

            LOG_DEBUG( "Invalid FPC Version: %s", fpcVersion.toLatin1().constData() );

            // SOF-2717
            reboot();
            return false;
        }
    }
    else
    {
        // Return true for all other modes
        rval = true;

        factoryResetDone = true;
    }

    return rval;
}

// SOF-1296
void manufacturingTest::confirmWeigherReset()
{
    weigherResetTimerDone = true;
}

// SOF-1709
void manufacturingTest::confirmPrinterReset()
{
    printerResetTimerDone = true;
}

// SOF-4020
void manufacturingTest::confirmReportDone()
{
    reportTimerDone = true;
}

// SOF-5995: swapLabelDialog cancel button clicked- if we include it
void manufacturingTest::dialogButtonClicked()
{
    swapLabelStockRoutineStarted_ = false;
    hasBeenDisengaged_ = false; // since they will have to re-engage to restart the test anyway
    printerTestStatus();
}

//---------------------------------------------------------
// SOF-3244: Check if the HTm load cell has been calibrated
//           by comparing the calculated checksum against
//           the checksum received.
//---------------------------------------------------------
void manufacturingTest::checkLoadCellCalibration()
{
    LOG_CLASS_INFO( "manufacturingTest", "checkLoadCellCalibration" );

    loadCellCalibrationChecked = true;

    LOG_DEBUG("Top of checkLoadCellCalibration(): Validate FIT Parameters ...", NULL );

    long calcChecksumValue = 0;
    unsigned char *tmpPtr;
    int loopCtr;

    if( isHtmWeigher )
    {
        //-----------------------------------------------------------
        // Calculate the checksum by using each of the FIT parameters
        //-----------------------------------------------------------

        tmpPtr = (unsigned char *) &gravity_;
        for( loopCtr = 0; loopCtr < sizeof( gravity_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &m_LbsCountsConvFactor_;
        for( loopCtr = 0; loopCtr < sizeof( m_LbsCountsConvFactor_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &zeroValue_;
        for( loopCtr = 0; loopCtr < sizeof( zeroValue_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &spanValue_;
        for( loopCtr = 0; loopCtr < sizeof( spanValue_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &z0ElectronicOffset_;
        for( loopCtr = 0; loopCtr < sizeof( z0ElectronicOffset_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &deadWeight_;
        for( loopCtr = 0; loopCtr < sizeof( deadWeight_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &x2_;
        for( loopCtr = 0; loopCtr < sizeof( x2_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &x1_;
        for( loopCtr = 0; loopCtr < sizeof( x1_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &x0_;
        for( loopCtr = 0; loopCtr < sizeof( x0_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &y2_;
        for( loopCtr = 0; loopCtr < sizeof( y2_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &y1_;
        for( loopCtr = 0; loopCtr < sizeof( y1_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &y0_;
        for( loopCtr = 0; loopCtr < sizeof( y0_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &xRot_0_;
        for( loopCtr = 0; loopCtr < sizeof( xRot_0_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &xRot_1_;
        for( loopCtr = 0; loopCtr < sizeof( xRot_1_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &xRot_2_;
        for( loopCtr = 0; loopCtr < sizeof( xRot_2_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &yRot_0_;
        for( loopCtr = 0; loopCtr < sizeof( yRot_0_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &yRot_1_;
        for( loopCtr = 0; loopCtr < sizeof( yRot_1_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &yRot_2_;
        for( loopCtr = 0; loopCtr < sizeof( yRot_2_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &zRot_0_;
        for( loopCtr = 0; loopCtr < sizeof( zRot_0_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &zRot_1_;
        for( loopCtr = 0; loopCtr < sizeof( zRot_1_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &zRot_2_;
        for( loopCtr = 0; loopCtr < sizeof( zRot_2_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &xGain_;
        for( loopCtr = 0; loopCtr < sizeof( xGain_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &yGain_;
        for( loopCtr = 0; loopCtr < sizeof( yGain_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        tmpPtr = (unsigned char *) &gCoeff_;
        for( loopCtr = 0; loopCtr < sizeof( gCoeff_ ); ++loopCtr )
            calcChecksumValue += *tmpPtr++;

        //---------------------------------------------------------
        // Compare the calculated checksum to the checksum received
        //---------------------------------------------------------
        LOG_DEBUG("Calculated Checksum = %ld", calcChecksumValue );
        LOG_DEBUG("Checksum received = %ld", checkSum_ );

        if( calcChecksumValue != checkSum_ ||
            calcChecksumValue == 0 ||   // Get all zeroes when Accelerometer is un-plugged
            checkSum_ == 0 )            // or the Accelerometer has not been calibrated with the load cell
        {
            LOG_DEBUG( "checkLoadCellCalibration(): Load cell calibration FAILURE", NULL );

            loadCellCalibrationFailed = true;

            MessageBoxDialog dialog( real_time_priority, mainWindow_ );
            dialog.setAllowOverlayClickDismiss( false );

            dialog.setTitle( "VALUEMAX LOAD CELL CALIBRATION FAILURE" );
            dialog.setText( "This Load Cell has not been calibrated properly.\n\nThe Load Cell cannot be used in the scale.\n\nPress OK to continue." );
            dialog.setStandardButtons( QDialogButtonBox::Ok );
            dialog.exec();

            // Return to Home (i.e. PluMainScreen)
            mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();
        }

        LOG_DEBUG( "checkLoadCellCalibration(): Load cell calibration SUCCESSFUL", NULL );
    }
}

// SOF-3244
void manufacturingTest::confirmFitParams()
{
    LOG_CLASS_INFO( "manufacturingTest", "confirmFitParams" );

    /*
     * If the timer has expired, there has been an issue communicating with the load cell, so
     * fail the test
     */

    LOG_DEBUG("In confirmFitParams(): haveFitParameters set to TRUE", NULL );

    if ( waitingForValueMax )
    {
        waitingForValueMax = false;
        checkLoadCellCalibration();
    }

}

void manufacturingTest::generateSummaryReport()
{
    LOG_CLASS_INFO( "manufacturingTest", "generateSummaryReport" );

    // PJR 425
    findGcd();
    // Send message to generate the Summary Report
    QString msg;
    QString space;
    QString title;

    //--------------------------------
    // Temporary hack used @ Hillsboro
    //sleepFor( 2000 );
    //--------------------------------

    QString HobartPoint4 = QString::fromUtf8("\x1b""\x1b""fn=Hobart;ps=4""\x1b");
    QString HobartPoint6 = QString::fromUtf8("\x1b""\x1b""fn=Hobart;ps=6""\x1b");
    QString HobartPoint7 = QString::fromUtf8("\x1b""\x1b""fn=Hobart;ps=7""\x1b");
    QString HobartBoldPoint12 = QString::fromUtf8("\x1b""\x1b""fn=HobartBold;ps=12""\x1b");

    QString lineIndent = "  ";
    title = HobartPoint7;
    if( isArkBoardSite ) {
       //feed 1/4" more for board site reports
        int numOfLineFeeds = 10;
        for (int lineFeed = 0; lineFeed <= numOfLineFeeds; lineFeed++) {
             //space += lineIndent +  "Space" + "\n";
             space += "\n";
        }
    }
    
    if (currentManfTestMode == BoardTest)
    {
        title += space; 
        title += lineIndent + "Board Test Report -- Board Site";
    }
    else if ( currentManfTestMode == ServiceDiagnosticMode )
    {
        title += lineIndent + "Service Test Report";
    }
    else
    {
        title += lineIndent + "Manufacturing Test Report";
    }


    msg = HobartPoint7;
    msg += space; 

    msg += lineIndent + "Date: ";
    msg += QDate::currentDate().toString("MM/dd/yyyy  ");
    msg += QTime::currentTime().toString("hh:mm:ss AP");
    msg += "\n";

    msg += lineIndent + "Manufacture Date: ";
    msg += ui->sysCtrlManufactureDateLineEdit->text();
    msg += "\n";

    msg += lineIndent + "Serial #: ";
    msg += ui->sysCtrlSerialNumberLineEdit->text();
    msg += "\n";

    // FT-1610:  Add Mac Address to print on Manufacturing test label
    msg += lineIndent + "Ethernet MAC Address: ";
    msg += ethernetMacAddress;
    msg += "\n";

    msg += lineIndent + "Wireless MAC Address: ";
    msg += wirelessMacAddress;
    msg += "\n";

    msg += lineIndent + "Hostname: ";
    msg += hostName;
    msg += "\n";

    msg += lineIndent + "Model: ";
    msg += ui->sysCtrlStyleLineEdit->text();
    msg += "\n";

    msg+= lineIndent + "GCD: ";

    // 1 means disconnected, 0 means connected...
    if(gcdValue_ == 0)
        msg+= "Attached";
    else
        msg+="Not Attached";

    msg+="\n";
    // FT-1423
    msg += lineIndent + "Application Version: ";
    msg += applicationVer;
    msg += "\n";

    msg += lineIndent + "Loader Version: ";
    msg += ( newLoaderVer != "" ? newLoaderVer : loaderVer );
    msg += "\n";

    msg += lineIndent + "Peripheral Controller Version: ";
    msg += periphControllerVer + "-" + periphControllerVerWeigher;
    msg += "\n";

    msg += lineIndent + "Board Assembly Number: ";
    msg += ui->sysCtrlBoardAssemblyNumberLineEdit->text();
    msg += "\n";

    msg += lineIndent + "Board Revision Number: ";
    msg += ui->sysCtrlBoardAssemblyRevisionLineEdit->text();
    msg += "\n";

    if( isHtmWeigher )
    {
        msg += lineIndent + "X Tilt: " + xTilt;
        msg += "\n";
        msg += lineIndent + "Y Tilt: " + yTilt;
        msg += "\n\n";
    }
    else
    {
        msg += "\n";
    }

    msg += lineIndent + "Test Status:";

    bool overallStatusPassed = true;

    for ( int loop = 0; loop < numberOfTabs; ++ loop )
    {
        // Skip Weigher tab if no weigher and
        // Skip Bluetooth tab if no Bluetooth
        // Skip CANbus ( I/O ) tab if CANbus not tested (SOF-3556)
        if( ( noWeigher && loop == ui->testTabsWidget->indexOf( ui->weigherTab ) ) ||
            ( noBluetooth && loop == ui->testTabsWidget->indexOf( ui->bluetoothTab ) ) ||
            ( ( currentManfTestMode == ManufTestPrepack ) && ( noCanBus ) && ( loop == ui->testTabsWidget->indexOf( ui->canbusTab ) ) ) )
        {
            msg += "\n";
            msg += lineIndent + ui->testTabsWidget->tabText( loop );
            msg += " - SKIPPED";
        }
        else
        {
            if ( tabStatus [ loop ] == 1 )
            {
                // Grab text for tab description
                msg += "\n";
                msg += lineIndent + ui->testTabsWidget->tabText( loop );
                msg += " - PASSED";
            }
            else
            {
                msg += "\n";
                msg += lineIndent + ui->testTabsWidget->tabText( loop );
                msg += " - FAILED";
                overallStatusPassed = false;
            }
        }
    }

    // 3/30/23 - per Jim, include this result on pre test label
    if ( isSelfServiceScale("FS") && currentManfTestMode == ManufTestPreTest )
    {
        msg += "\n";
        msg += lineIndent + "Label Stock Sensor";
        msg += labelStockSensorPassed_ ? " - PASSED" : " - FAILED";
    }

    int numOfLineFeeds = 3;
    for (int lineFeed = 0; lineFeed <= numOfLineFeeds; lineFeed++)
        msg += "\n";

    msg += HobartBoldPoint12;

    if (overallStatusPassed)
    {
        msg += "        PASSED";
    }
    else
    {
        msg += "        FAILED";
    }

    if (currentManfTestMode == BoardTest)
    {
        msg += HobartPoint6;

        numOfLineFeeds = 5;
        for (int lineFeed = 0; lineFeed <= numOfLineFeeds; lineFeed++)
            msg += "\n";

        msg += lineIndent + "___________________________________\n";

        msg += HobartBoldPoint12;

        msg += "  " + ethernetMacAddress;
    }

    LOG_DEBUG("Manufacturing-Test-Report: Started", NULL );

    //SOF-4399
    //Generate QRCode from Ethernet MAC Address
    if( currentManfTestMode == BoardTest ){
        bitmap b(100, 100);

        qrapi::encode( ethernetMacAddress.toStdString(), qr::LevelQ, b, 0, 0, 100, 100);
        QTemporaryFile f;
        f.setAutoRemove( true );

        if ( f.open() )
        {
            const QString filename( f.fileName() );

            const buffer img( b.toImage( "png" ) );
            img.save( filename.toStdString() );

            ui->lblEthernetMACAddressQRCode->setPixmap( QPixmap( filename ) );

            f.close();
        }
    }

    //session_->send( SessionDev, SnManufacturingTestSummaryReport, msg );
    session_->send( ReportManagerDev, StartReport, "2,0,0,0,0," + title + "," + msg ); // 116,0

    //---------------------------------------------------------
    // SOF-4020
    // Wait for confirmation that the Manufacturing Test report
    // has completed before exiting
    //---------------------------------------------------------
    startReportTimer = new QTimer( this );
    startReportTimer->setSingleShot( true );
    startReportTimer->setInterval( 10000 );
    connect( startReportTimer, SIGNAL( timeout() ), SLOT( confirmReportDone() ) );
    reportTimerDone = false;
    startReportTimer->start();

    while( !reportTimerDone )
    {
        sleepFor( 50 );
    }

    startReportTimer->stop();

    LOG_DEBUG("Manufacturing-Test-Report: Exiting", NULL );

    //--------------------------------
    // Temporary hack used @ Hillsboro
    //sleepFor( 8000 );
    //--------------------------------
}

//-----------------------------------------------------------------------------------------------------------------------
// Need to check the Style (i.e. Model Name) and disable certain tabs and sub-tabs
//
//  e.g. HTiP         "P" indicates Printer Only (no load cell): disable Weigher tab
//
//       HTi-LS       "-L" or "-W" indicates no Customer Display: disable Customer sub-tab on Display tab
//       HTi-WS
//
//       HTs-7LST     "T" in last position indicates Wired networking only: disable the Wireless sub-tab on the Network tab
//
//                    FT-1378
//                    This also indicates there is no Bluetooth: disable the Bluetooth tab
//
//       FT-1333
//       HTs          "s" for System Controller indicates Solo i.MX6:
//                    No Customer speakers - disable start test button for Customer two-channel stereo test on Audio tab
//
//                    FT-1414
//                    No Bluetooth for HTs.  Disable the Bluetooth tab
//
//                    FT-1418
//                    Auto-complete and pass the USB tab if HTs scale
//
//       FT-1803
//       HTi-SSLS     If Self-Service Scale (i.e. SS after the dash in the model name)
//       HTi-SSLSB    Do not display or prompt for the Barcode Scanner test
//
//       SOF-1823     Also if Self-Service, do not need to test Customer speakers
//
//       SOF-2818     Auto-Complete and pass the USB tab for any scale that has
//                    an ARA board.
//
//       SOF-3556     If Prepack and the Wrapper model is NGW or EPCP-5i1, then enable the
//       NGW          CANbus tab (i.e. I/O tab).
//       EPCP-5i1
//                    Otherwise the CANbus tab should be disable for all other Wrapper models.
//
//       SOF-4763     Auto-Complete and pass the USB tab for any prepack device that has
//                    an ARA board.
//
//-----------------------------------------------------------------------------------------------------------------------
void manufacturingTest::setTabsBasedOnStyle()
{
    LOG_CLASS_INFO( "manufacturingTest", "setTabsBasedOnStyle" );

    QString strModelName = ui->sysCtrlStyleLineEdit->text();

    // SOF-3555: HTi with Cutter (ARK board); but no Code Checker
    if( strModelName.length() >= 3 && !( strModelName.toUpper().startsWith( "HTM" ) ) )
    {
        codeCheckerEnabled_ = false;
    }

    if (currentManfTestMode != BoardTest)
    {
        if(!session_->isPrepack())
        {
            //------------------------------
            // Check if "Printer Only" scale
            //------------------------------
            LOG_DEBUG( "6 ModelName=%s", strModelName.toLatin1().constData() );

            if( strModelName.length() >= 4 )
            {
                if( strModelName.mid( 3, 1 ).toUpper() == "P" )
                {
                    // Printer only - No Weigher: Disable Weigher tab

                    ui->testTabsWidget->setTabEnabled( ui->testTabsWidget->indexOf( ui->weigherTab ), false );

                    noWeigher = true;
                }
                else
                {
                    // NOT Printer only: Enable Weigher tab

                    ui->testTabsWidget->setTabEnabled( ui->testTabsWidget->indexOf( ui->weigherTab ), true );

                    noWeigher = false;
                }
            }
        }


        //-----------------------------
        // Check if No Customer Display
        //-----------------------------

        // First reset so it is always enabled
        if (session_->isPrepack() || session_->scaleConfig->scaleConfig.scaleName().startsWith("FS"))
            noCustomerDisplay = true;		//	HA-62
        else
        {
            noCustomerDisplay = false;

            // SOF-304 - Default to show fields for Customer Display test

            if ( !session_->scaleConfig->scaleConfig.scaleName().startsWith("FS") )
            {
                ui->lblCustomerDisplayTestMessage->show();
                ui->gbCustomerDisplayAnswers->show();
            }

            LOG_DEBUG( "7 ModelName=%s", strModelName.toLatin1().constData() );
            int dashOffset = strModelName.indexOf( "-" );

            if ( dashOffset != -1 )
            {
                if (strModelName.length() >= ( dashOffset + 3 ) )
                {
                    if ( ( strModelName.mid( ( dashOffset + 1 ), 1 ).toUpper() == "L" ) ||
                         ( strModelName.mid( ( dashOffset + 1 ), 1 ).toUpper() == "W" ) ||
                         ( strModelName.mid( ( dashOffset + 1 ), 2 ).toUpper() == "SS" ) ) // SOF-1796: Self-Service
                    {
                        // No Customer Display ...
                        // SOF-304 - Hide fields for Customer Display test
                        ui->lblCustomerDisplayTestMessage->hide();
                        ui->gbCustomerDisplayAnswers->hide();

                        noCustomerDisplay = true;
                    }
                }
            }
        }

        //-------------------------------
        // Check if Wired Networking only
        //-------------------------------

        // First reset so it is always enabled
        noWirelessNetworking = false;

        // FT-1378
        noBluetooth = false;
        ui->testTabsWidget->setTabEnabled( ui->testTabsWidget->indexOf( ui->bluetoothTab ), true );

        ui->networkTypeTab->setTabEnabled( ui->networkTypeTab->indexOf( ui->wirelessTab ), true );

        LOG_DEBUG( "8 ModelName=%s", strModelName.toLatin1().constData() );
        if ( strModelName.mid( ( strModelName.length() - 1 ), 1 ).toUpper() == "T" )
        {
            // Wired networking only: disable the Wireless sub-tab on the Network tab

            ui->networkTypeTab->setTabEnabled( ui->networkTypeTab->indexOf( ui->wirelessTab ), false );

            noWirelessNetworking = true;

            // FT-1378
            // Wired networking only: disable the Bluetooth tab
            ui->testTabsWidget->setTabEnabled( ui->testTabsWidget->indexOf( ui->bluetoothTab ), false );

            noBluetooth = true;
        }

        //--------------------
        // Check if Solo board
        //--------------------

        //if (!session_->isPrepack())	//	HA-62. No solo board for EPCP.
        //{
            // First reset so it is always enabled
        noCustomerSpeakers = false;

        showCustomerSpeakerTest( true );

        autoCompleteUsbTab = false;     // FT-1418

        // HT-44 Audio tab of Manufacturing test doesn't work - always enabled so dont need.
        //if( ui->startStereoTestButton->isVisible() )
        //    ui->startStereoTestButton->setEnabled( true );

        LOG_DEBUG( "9 ModelName=%s", strModelName.toLatin1().constData() );
        if ( strModelName.length() >= 3 )
        {
            if( strModelName.mid( 0, 3 ).toUpper() == "HTS" )   // SOF-836
            {
                // Solo board - Disable test for Customer Speakers

                // HT-44 Audio tab of Manufacturing test doesn't work - only hide if HTs
                //ui->startStereoTestButton->setDisabled( true );

                noCustomerSpeakers = true;

                showCustomerSpeakerTest( false );

                // FT-1414
                noBluetooth = true;
                ui->testTabsWidget->setTabEnabled( ui->testTabsWidget->indexOf( ui->bluetoothTab ), false );

                autoCompleteUsbTab = true;  // FT-1418
            }
        }

        //------------------------------------------------------------------
        // SOF-2818: Auto-Complete USB tab for Scale that has an ARA board
        //------------------------------------------------------------------
        // SOF-4763: Auto-Complete USB tab for Prepack that has an ARA board
        //------------------------------------------------------------------

        if( ( session_->isServiceCounter() && boardManf == "ARA" ) ||
            ( session_->isPrepack() && boardManf == "ARA" ) )

            autoCompleteUsbTab = true;

        //----------------------------
        // Check if Self-Service scale
        //----------------------------

        if( currentManfTestMode == ManufTestPreTest || currentManfTestMode == ManufTestFinalTest )
        {

            // SOF-1803 - Default to hide the fields for Barcode Scanner test
            ui->lblSelfService->hide();
            ui->gbSelfServiceAnswers->hide();
            ui->lblSelfServiceScannerPrompt->hide();
            ui->gbSelfServiceScannerPromptAnswers->hide();


            // hide low label stock sensor test if not FS unit
            ui->labelStockSensorTestResult->hide();
            ui->startLowStockSensorTestButton->hide();
            ui->lblStockSensorTest->hide();

            LOG_DEBUG( "10 ModelName=%s", strModelName.toLatin1().constData() );
            LOG_DEBUG( "setTabsBasedOnStyle-1: gbSelfServiceAnswers->isHidden()=%s", (ui->gbSelfServiceAnswers->isHidden()) ? "YES" : "NO" );

            if ( isSelfServiceScale( strModelName ) )
            {
                ui->lblSelfService->show();
                ui->gbSelfServiceAnswers->show();

                if (currentManfTestMode == ManufTestPreTest)
                {
                    ui->labelStockSensorTestResult->show();
                    ui->startLowStockSensorTestButton->show();
                    ui->lblStockSensorTest->show();
                }


                //LOG_DEBUG( "setTabsBasedOnStyle-2: gbSelfServiceAnswers->isVisible()=%s", (ui->gbSelfServiceAnswers->isVisible()) ? "YES" : "NO" );
                LOG_DEBUG( "setTabsBasedOnStyle-2: gbSelfServiceAnswers->isHidden()=%s", (ui->gbSelfServiceAnswers->isHidden()) ? "YES" : "NO" );

                /* 3/28/23: Not sure why this is here, but according to Jim, we need to show it on FS
                showCustomerSpeakerTest( false );

                // SOF-1823
                noCustomerSpeakers = true;
                */

                // SOF-2875: Still need to go through USB tab for Self-Serve - for the Built-In Barcode Scanner
                autoCompleteUsbTab = false;
            }
        }

        //------------------------------------------------------------------------------
        // SOF-3556: If Pre-Pack, only enable the CANbus tab for specific Wrapper models
        //------------------------------------------------------------------------------
        if( ( currentManfTestMode == ManufTestPrepack ) )
        {
            if( ( strModelName.toUpper().startsWith( "NGW" ) ) ||
                ( strModelName.toUpper().startsWith( "NGS" ) ) ||
                ( strModelName.toUpper().startsWith( "EPCP-5I1" ) ) )
            {
                ui->testTabsWidget->setTabEnabled( ui->testTabsWidget->indexOf( ui->canbusTab ), true );

                noCanBus = false;
            }
            else
            {
                ui->testTabsWidget->setTabEnabled( ui->testTabsWidget->indexOf( ui->canbusTab ), false );

                noCanBus = true;
            }
        }
    }
}

void manufacturingTest::showCustomerSpeakerTest( bool showTest )
{
    if( showTest )
    {
         ui->lblConfirmSpeakers->setEnabled( true );
         ui->startStereoTestButton->setEnabled( true );
         ui->rbStereoTestAnswerYes->setEnabled( true);
         ui->rbStereoTestAnswerNo->setEnabled( true );
         ui->gbStereoTestAnswers->setEnabled( true );

         ui->lblConfirmSpeakers->setVisible( true );
         ui->startStereoTestButton->setVisible( true );
         ui->rbStereoTestAnswerYes->setVisible( true);
         ui->rbStereoTestAnswerNo->setVisible( true );
         ui->gbStereoTestAnswers->setVisible( true );

    }
    else
    {
         ui->lblConfirmSpeakers->setVisible( false );
         ui->startStereoTestButton->setVisible( false );
         ui->rbStereoTestAnswerYes->setVisible( false );
         ui->rbStereoTestAnswerNo->setVisible( false );
         ui->gbStereoTestAnswers->setVisible( false );

         ui->lblConfirmSpeakers->setEnabled( false );
         ui->startStereoTestButton->setEnabled( false );
         ui->rbStereoTestAnswerYes->setEnabled( false );
         ui->rbStereoTestAnswerNo->setEnabled( false );
         ui->gbStereoTestAnswers->setEnabled( false );
    }
}

bool manufacturingTest::isSelfServiceScale( QString modelName )
{
    bool rval = false;

    int dashOffset = modelName.indexOf( "-" );

    if ( dashOffset != -1 )
    {
        if (modelName.length() >= ( dashOffset + 3 ) )
        {
            if ( ( modelName.mid( ( dashOffset + 1 ), 2 ).toUpper() == "SS" ) )
            {
                rval = true;
            }
        }
    }

    QString scaleName_ = session_->scaleConfig->scaleConfig.scaleName();

    // checking for FS with built in barcode scanner
    if(modelName == "B")
         return scaleName_.startsWith("FS") && scaleName_.contains("B");

    if (modelName == "FS")
        return scaleName_.startsWith("FS");

    return rval || session_->scaleConfig->scaleConfig.scaleName().startsWith("FS");
}

void manufacturingTest::setMaxBacklight()
{
    //LOG_CLASS_INFO( "manufacturingTest", "setMaxBacklight" );

    python *pythonProcess = new python();
    pythonProcess->setNonBlocking();

    connect(pythonProcess, SIGNAL(readyReadScript()), pythonProcess, SLOT(deleteLater()));

    // Set backlight of both displays to max
    QStringList args;
    args.append("both");
    args.append("254");

    if (pythonProcess->execCommand("setBrightness", args,false,false, 30 * 1000))
    {
        /*

        if (pythonProcess.getExitCode() == 0)
        {
            LOG_DEBUG("brightness of displays set", NULL);
        }
        else
        {
            // TODO:
            // what do we do here?
            LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }

        */
    }
    else
    {
        // TODO:
        // What do we do here?
    }

}

// FT-1626
//-------------------------
// Get the scale's hostname
//-------------------------
void manufacturingTest::getHostname()
{
   // LOG_CLASS_INFO( "manufacturingTest", "getHostname" );

   // python pythonProcess;
   // QStringList args;

    hostName = session_->scaleConfig->hostName_;

    /*
    if ( pythonProcess.execCommand("getHostname", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            hostName = pythonProcess.readStdOut();

            LOG_DEBUG( "get hostname successful, |%s|", hostName.toLatin1().constData() );
        }
        else
        {
            LOG_DEBUG( "Cannot get hostname. Exit code = %d", pythonProcess.getExitCode() );
        }
    }
    */
}

// HT-430
void manufacturingTest::populateBoardType()
{
    LOG_CLASS_INFO( "manufacturingTest", "populateBoardType" );

    if( !isLinux() )
        return;

    python pythonProcess;
    QStringList args;
    QString cpuCount = session_->scaleConfig->cpuCount;
    QString strCPUCount = "Unknown";

    if( cpuCount == "1" )
        strCPUCount = "Solo";
    else if( cpuCount == "2" )
            strCPUCount = "Dual";
    else if( cpuCount == "4" )
            strCPUCount = "Quad";


    /* if ( pythonProcess.execCommand("cpuCount", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            cpuCount = pythonProcess.readStdOut();

            if( cpuCount == "1" )
                strCPUCount = "Solo";
            else if( cpuCount == "2" )
                    strCPUCount = "Dual";
            else if( cpuCount == "4" )
                    strCPUCount = "Quad";

            LOG_DEBUG( "get CPU count successful, |%s|", cpuCount.toLatin1().constData() );
        }
        else
        {
            LOG_DEBUG( "Cannot get CPU count. Exit code = %d", pythonProcess.getExitCode() );
        }
    }
    */

    //--------------------
    // getBoardType.py ...
    //--------------------
    args.clear();
    boardManf = "Unknown";


    // if ( pythonProcess.execCommand("getBoardType", args, false, false, ( 30 * 1000 ) ) )
    // {
       // if ( pythonProcess.getExitCode() == 0 )
       // {
            LOG_DEBUG( "getBoardType successful", NULL );

            QString result = session_->scaleConfig->boardType;
            LOG_DEBUG( "getBoardType results=|%s|", result.toLatin1().constData() );

            QStringList boardTypes;
            boardTypes = result.split( "," );

            if( boardTypes.count() >= 2 )
            {
                QString isComboBoard;
                QString isBcmBoard;

                isComboBoard = boardTypes[0];
                isBcmBoard = boardTypes[1];

                if( isComboBoard == "1" )
                    boardManf = "ARA";
                else if( isBcmBoard == "1" )
                    boardManf = "BCM";

                if( boardTypes.count() >= 3 )
                {
                    QString isArkBoard;

                    isArkBoard = boardTypes[2];

                    if( isArkBoard == "1" )
                        boardManf = "ARK";
                }

                ui->lblBoardTypeData->setText( boardManf + " " + strCPUCount );
            }
      //  }
        else
        {
            LOG_DEBUG( "getBoardType failed. Exit code = %d", pythonProcess.getExitCode() );
        }
    //}
}

// HT-430
void manufacturingTest::populateSdCardSize()
{
   // LOG_CLASS_INFO( "manufacturingTest", "populateSdCardSize" );

    if( !isLinux() )
        return;

    python pythonProcess;
    QStringList args;
    QString sdCardSize = session_->scaleConfig->sdCardSize;

    ui->lblSdCardSizeData->setText( sdCardSize + " GB");


   /* if ( pythonProcess.execCommand("sdCardSize", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            sdCardSize = pythonProcess.readStdOut();

            LOG_DEBUG( "get SD card size successful, |%s|", sdCardSize.toLatin1().constData() );

            ui->lblSdCardSizeData->setText( sdCardSize + " GB");
        }
        else
        {
            LOG_DEBUG( "Cannot get SD card size. Exit code = %d", pythonProcess.getExitCode() );
        }
    }
    */
}

// FT-1626
//-----------------------------------------
// Send message to record scale information
//-----------------------------------------
void manufacturingTest::sendScaleInformation()
{
    LOG_CLASS_INFO( "manufacturingTest", "sendScaleInformation" );

    QString msg;
    python pythonProcess;
    QStringList args;
    QString portNumber = "5080";

    msg = "storeScaleData|";
    msg += ui->sysCtrlSerialNumberLineEdit->text() + "|";
    msg += ethernetMacAddress + "|";
    msg += wirelessMacAddress + "|";
    msg += hostName + "|";
    msg += ui->sysCtrlStyleLineEdit->text() + "|";
    msg += applicationVer + "|";
    msg += ( newLoaderVer != "" ? newLoaderVer : loaderVer ) + "|";     // SOF-4613
    msg += periphControllerVer + "-" + periphControllerVerWeigher + "|";
    msg += ui->sysCtrlBoardAssemblyNumberLineEdit->text() + "|";
    msg += ui->sysCtrlBoardAssemblyRevisionLineEdit->text() + "|";
    msg += ui->sysCtrlManufactureDateLineEdit->text() + "|";
    msg += QDate::currentDate().toString("MM/dd/yyyy  ");
    msg += QTime::currentTime().toString("hh:mm:ss AP");

    args.append( ipAddressToPing );
    args.append( portNumber );
    args.append( msg );

    LOG_DEBUG( "IP=%s, Port=%s", ipAddressToPing.toLatin1().constData(), portNumber.toLatin1().constData() );
    LOG_DEBUG( "msg=%s", msg.toLatin1().constData() );

    if ( pythonProcess.execCommand("sendDeviceEventMessage", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            LOG_DEBUG( "sendDeviceEventMessage SUCCESS", NULL );
        }
        else
        {
            LOG_DEBUG( "sendDeviceEventMessage FAILED", NULL );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test built in barcode scanner, if present
// Precondition:  Operator has selected 'yes' to the built-in barcode scanner's presence
// Postcondition: Barcode Scanner has been powered on and powered off
//
void manufacturingTest::enableBarCodeTest(){

    //connect revealed options to appropriate slots
    connect( ui->rbSelfServiceScannerPromptAnswerYes, SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );
    connect( ui->rbSelfServiceScannerPromptAnswerNo, SIGNAL( clicked() ), this, SLOT( getTestStatus() ) );

    // SOF-5995: add ability to retry the scanner "test"
    connect( ui->rbSelfServiceScannerPromptAnswerRetry, SIGNAL( clicked() ), this, SLOT( enableBarCodeTest() ) );

    ui->gbSelfServiceScannerPromptAnswers->setVisible(true);
    ui->rbSelfServiceScannerPromptAnswerRetry->setVisible(true);
    //ui->rbSelfServiceScannerPromptAnswerYes->setVisible(true);
    //ui->rbSelfServiceScannerPromptAnswerNo->setVisible(true);
    ui->lblSelfServiceScannerPrompt->setVisible(true);

    session_->send( BarcodeScanner, ScannerEnabled);

    // create check for media handler
    QTimer *scannerOnTimer = new QTimer( this );
    scannerOnTimer->setInterval( 1000 );
    scannerOnTimer->setSingleShot( true );

    // disable the scanner after ~1 second timer timeout()
    connect( scannerOnTimer, SIGNAL(timeout()), SLOT(disableScanner()));
    scannerOnTimer->start();
}

void manufacturingTest::disableScanner()
{
    session_->send(BarcodeScanner, ScannerDisabled);

    ui->rbSelfServiceScannerPromptAnswerRetry->setAutoExclusive(false);
    ui->rbSelfServiceScannerPromptAnswerRetry->setChecked(false);
    ui->rbSelfServiceScannerPromptAnswerRetry->setAutoExclusive(true);
}

// SOF-3166
void manufacturingTest::removeCutterTab()
{
    LOG_CLASS_INFO( "manufacturingTest", "removeCutterTab" );

    LOG_DEBUG( "removeCutterTab PrCutterStatus msg NO CUTTER", NULL );

    cutterEnabled_ = false;
    ScaleConfiguration *scaleConfig( session_->scaleConfig );
    scaleConfig->scaleConfig.isCutterEnabledRaw() = cutterEnabled_;

    if( isArkBoardSite )
    {
        ui->lblIsCutterAttachedResult->setText(tr("No"));
        ui->lblCutterStatus->setEnabled(false);                 // The cutter is currently
        ui->lblCutterStatusResult->setText(tr("Disabled"));     // Disabled or Enabled
        ui->butEnableCutter->setText(tr("Disabled"));
        ui->lblCutterStatusResult->setEnabled(false);           // ditto
        ui->butEnableCutter->setEnabled(false);
        ui->butResetCutterDefault->setEnabled( false );
        ui->printerTestsTab->removeTab( ui->printerTestsTab->indexOf( ui->cutterTab ) );  // so remove the cutterTab
    }
    else
    {
        ui->printerTestsTab->setTabEnabled( ui->printerTestsTab->indexOf( ui->cutterTab ), false );
    }
}

void manufacturingTest::showCutterTab()
{
    LOG_CLASS_INFO( "manufacturingTest", "showCutterTab" );

    LOG_DEBUG( "showCutterTab ... showing Cutter tab", NULL );

    // SOF-5084 - This function gets called every time we receive a cutter status, so the cutter is always shown
    // as enabled when it is attached. Setting it to what has been saved in the scale config instead
    //cutterEnabled_ = true;
    ScaleConfiguration *scaleConfig( session_->scaleConfig );
    //scaleConfig->scaleConfig.isCutterEnabledRaw() = cutterEnabled_;
    cutterEnabled_ = scaleConfig->scaleConfig.isCutterEnabled();

    ui->printerTestsTab->setTabEnabled( ui->printerTestsTab->indexOf( ui->cutterTab ), true );
}

void manufacturingTest::onDotWearTimerTimeout()
{
    LOG_CLASS_INFO( "manufacturingTest", "onDotWearTimerTimeout" );

    LOG_DEBUG( "onDotWearTimerTimeout(): Request Dot Wear Status - Timed Out", NULL );

    dotWearTestFinished();
}

void manufacturingTest::startDotWearTest()
{
    LOG_CLASS_INFO( "manufacturingTest", "startDotWearTest" );

    LOG_DEBUG( "startDotWearTest(): Request Dot Wear Status", NULL );

    session_->send(Printer, PrReqHeadDotStatus);   // 7,180 - Request the New Head Dot Status
    if( dotWearTimer )
    {
        dotWearTimer->start();
    }
}

void manufacturingTest::dotWearTestFinished()
{
    LOG_CLASS_INFO( "manufacturingTest", "dotWearTestFinished" );

    LOG_DEBUG( "dotWearTestFinished(): Dot Wear Status Request - Complete", NULL );

    if(dotWearTimer != NULL && dotWearTimer->isActive())
    {
        dotWearTimer->stop();
    }
}

void manufacturingTest::gcdDone()
{
    QProcess *gcd = qobject_cast<QProcess*>(sender());
    QString output = gcd->readAllStandardOutput();
    gcdValue_ = output.toInt();
    delete gcd; //
}

void manufacturingTest::findGcd()
{
    // PJR- 425 - include if GCD was attached during test in the final report
    QProcess *getGcd = new QProcess();
    QStringList path = QStringList() << "/usr/local/gpio/gcd_display_detect/value";
    connect(getGcd, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &manufacturingTest::gcdDone);
    getGcd->start("cat", path);
    getGcd->waitForFinished();
    //=============== END PJR-425
}

void manufacturingTest::runProcesses()
{

    setMaxBacklight();
    getScaleModel();
    getFlashInfo();
    getMACAddress();
    getHostname();
    getNetworkInfo();

    if( !isArkBoardSite ) // SOF-3449
        startBluetoothDiscovery();

    initLabelsOnDataReceived();
}

// SOF-3518
void manufacturingTest::requestPrinterDotStatus()
{
    requestPrinterDotStatusAttempted = true;

    startDotWearTest();
}

// SOF-3853
void manufacturingTest::removeMediaSensorCalibration()
{
    ui->label_13->setVisible(false);
    ui->startMediaSensorTestButton->setVisible(false);
    ui->lblMediaSensorDisplay->setVisible(false);
    ui->label_35->setVisible(false);
    ui->mediaSensorCalibrationTestAnswerLabel->setVisible(false);
}

bool manufacturingTest::isSSKPrinterAttached()
{
    MadpPeripheralList *periphList = &session_->scaleConfig->peripheralList;

    bool isAttached = false;

    if ( periphList->firstPrinter() == MadpPeripheralList::ptFSSS )
    {
        isAttached = true;
    }

    return isAttached;
}

void manufacturingTest::installButtonEventFilter()
{
    QList<QPushButton*> uiButtons;
    uiButtons = this->findChildren<QPushButton*>();

    foreach(QPushButton *btn, uiButtons)
    {
        btn->installEventFilter(this);
    }

}

void manufacturingTest::initShootThroughDialogText(MessageBoxDialog *diag)
{

    if (!diag)
        return;

    QTextDocument* doc = new QTextDocument(diag);
    QTextCursor* cursor = new QTextCursor(doc);

    QTextBlockFormat bf;
    bf.setLineHeight(10, QTextBlockFormat::LineDistanceHeight);
    cursor->setBlockFormat(bf);

    QTextListFormat listFormat;
    listFormat.setStyle(QTextListFormat::ListSquare);
    cursor->insertList(listFormat);

    foreach (const QString &step, calibrationSteps)
    {
        cursor->insertText(step);
    }

    QFont myFont;
    myFont.setPointSize(18);

    doc->setDefaultFont(myFont);
    diag->setText( doc->toHtml() );

}

