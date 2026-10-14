#ifndef MANUFACTURINGTEST_H
#define MANUFACTURINGTEST_H

#include "python.h"
#include "screenwidget.h"

#include "../widgets/weigherCalibrate.h"

#include <videoplayerwidget.h>

#include <ScaleConfiguration.h>

#include <Messages.h>

#include <QtGlobal>

#include <QComboBox>
#include <QLabel>
#include <QTimer>
#include <QStringList>
#include <QSet>
#include "../dialogs/messageboxdialog.h"

class QTimer;
class MessageBoxDialog;

namespace Ui {
class manufacturingTest;
}

/**
 * @class manufacturingTest
 * @brief Conducts manufacturing test of all scale subsystems
 *
 */

class manufacturingTest : public ScreenWidget
{
    Q_OBJECT

    typedef manufacturingTest _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit manufacturingTest(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~manufacturingTest();

    // FT-1222
    void setManufacturingTestMode( int manufTestMode  );

    void initiatePrinterWeigherReset();

    // FT-1222
    enum ManufTestModeValue
    {
        ManufTestPreTest = 1,
        ManufTestFinalTest,
        ServiceDiagnosticMode,
        ManufTestPrepack,
        BoardTest
    };

    //Enum for printerType
    enum printerType
    {
        PrimaryPrinter      = 0,
        SecondPrinter       = 1,
        ThirdPrinter        = 2
    };



    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve screen help text.
     *
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @returns  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();


public slots:


protected:

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

// FT-1328: Had to put reboot back in, due to need to enable openbox again
// FT-1330: Remove reboot after setting date/time in Manufacturing Test
    // FT-1271
    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

    // FT-1417
    virtual bool eventFilter( QObject *, QEvent * );


private slots:

    void on_programFlashButton_clicked();

    // SOF-304
    void on_startDisplayTestButton_clicked();

    void on_startBeeperTestButton_clicked();

    void on_startStereoTestButton_clicked();

    void on_startWiredNetworkTestButton_clicked();

    void on_startWirelessNetworkTestButton_clicked();

    // Removed when Vertical Lines test label added
    // void on_startPrintUpcBarcodeTestButton_clicked();

    void on_startCheckerBoardPatternTestButton_clicked();
    void on_startVerticalLinePatternTestButton_clicked();

    // SOF-232
    void on_startCalibrationTestButtton_clicked();

    void on_startSataTestButton_clicked();

    // SOF-896
    void on_startIOSataTestButton_clicked();

    // Removed when Vertical Lines test label added
    // void on_scannedBarcodeLineEdit_editingFinished();

    void on_startMediaSensorTestButton_clicked();
    void on_startLabelTakenSensorTestButton_clicked();

    void onResizeLabelsClicked();

    void on_acceptStopsAdjustmentButton_clicked();
    void on_exitPushButton_clicked();
    void on_reportPushButton_clicked();

    // FT-1055
    void on_saveValueButton_clicked();

    void playAudioFile();

    void on_startBtTestButton_clicked();
    void on_startCanbusTestButton_clicked();
    void getTestStatus();
    void getTestStatusInternal();

    void onCutterTimeout();
    void on_enableCutterButton_clicked();
    void on_startPrinterLabelCutterTestButton_clicked();

    void testsIndexChanged(int currentTabIndex);
    void weigherTestsIndexChanged(int currentTabIndex);
    void printerTestsIndexChanged(int currentTabIndex);

    // FT-1222
    void startHteScan();
    void displayScanOutput();
    void finishedProcess( int value, QProcess::ExitStatus exitStatus );
    void readyReadStandardError();

    void on_SaveDateTimeButton_clicked();

    void setTabsBasedOnStyle();

    // FT-1417
    void selectTextInField();

    // FT-1335
    void finishedBluetoothProcess( int value, QProcess::ExitStatus exitStatus );

    // Enhancements
    void enableBarCodeTest();
    void disableScanner();

    // SOF-1296
    /**
    * SLOT method called when the timer used to wait for confirmation
    * when resetting the Weigher to factory defaults.
    *
    * @param[in] none
    * @returns  none
    */
    void confirmWeigherReset();
    void confirmPrinterReset();

    // SOF-3294
    /**
     * Slot for when Dot status isn't returned
     */
    void onDotWearTimerTimeout();

    // SOF-3763 - Test Printer Cassette
    void on_startPrinterCassetteTestButton_clicked();

    // SOF-3887
    void butResetCutterDefault_clicked();

    // SOF-4020
    /**
    * SLOT method called when the timer used to wait for confirmation
    *      that Report is complete.
    *
    * @param[in] none
    * @returns  none
    */
    void confirmReportDone();

    //SOF-5321
    void dialogButtonClicked();

    // SOF-5996
    void onScreenFullyChanged();
    void on_startLowStockSensorTest_clicked();

    void on_ShootThroughCanceled();

private:

    enum AnswerType {
        None = 0,
        No = 1,
        Yes = 2
    };

    enum NetworkTestType {
        Wired = 0,
        Wireless = 1
    };

    enum CutterState
    {
        CutterNone = 0,
        CutterHome = 1,
        CutterLeftHome = 2,
        CutterReturnedHome = 3,
        CutterTestFinished = 4
    };

    Ui::manufacturingTest *ui;
    hobart::weigherCalibrate *weigherCalibrate_;
    QStringList answerOptions;
    int lastReadCount;
    bool countsTestPassed;

    void initializeAllAnswerLabelValues();
    void initializeAllRadioButtons(); // FT-38
    void initializeAnswers();
    void setAnswerLabelValue(QLabel *answerLabel, AnswerType answer);
    bool soundTestStatus();
    bool displayTestStatus();
    bool networkTestStatus();
    bool usbTestStatus();
    bool bluetoothTestStatus();
    bool fanTestStatus();
    bool sataTestStatus();
    bool weigherTestStatus();
    bool printerTestStatus();
    bool touchScreenTestStatus();
    bool canbusTestStatus();
    bool dateTimeStatus();
    bool flashProgramTestStatus();
    void execPythonCommand(QString command,
                           QStringList arguments=QStringList());

    void initialize();

    void SaveDataOnExit();
    void conductNetworkTest(NetworkTestType networkTest);

    videoPlayer *player;

	
    QIcon passIcon;
    QIcon failIcon;
    bool calibrationPassed;
    bool weigherStopsPassed;
    bool flashProgrammed;

    bool cutterAttached_;
    bool cutterEnabled_;
    bool cutterEnabledOriginal_;
    bool cutter_fired_;

    CutterState cutter_state_;

    QStringList stereoTestFiles;
    int currentPlaybackIndex;
    //QString ntpServer;
    int lastTabIndex;
    int currentPrinterTestTabIndex;
    int currentWeigherTestTabIndex;
    bool weigherModeCountsTransmission;

    // FT-577
    // Controls what step in the label taken sensor test is being executed
    bool checkSensorUnblockedBeforeTestPattern;
    bool checkLabelTakenSensorMode;
    bool lastLabelSensorMessageBlocked;


    bool isLinux();

    // FT-735: 0=Not started, 1=In Progress, 2=Complete
    int adjustLeftStop;
    int adjustRightStop;

    // FT-1055
    void setStopsFlags();
    int leftStopSavedValue;
    int rightStopSavedValue;
    int upperValue;
    int lowerValue;
    void resetStopsValues();

    // FT-1218
    bool wifiPresent;
    bool isWifiModulePresent();

    // FT-1222
    QProcess *scanProcess;
    int currentManfTestMode;

    QString initialScaleModelName;
    QString initialSerialNumber;
    QString getScaleModel();

    // Date/Time tab
    void setDateTimeLabel();

    void populateTimeZones();

    QString dbTimezone;

    // Functions for DB files used by Setup Utility
    QString databaseFileLocation;
    void connectDB();
    bool updateDB(QString key, QString value);

    void getMACAddress();
    void getNetworkInfo();

    QString ipAddressToPing;

    // FT-39 / FT-1149
    int numberOfTabs;
    int currentTabOffset;
    int tabStatus[ 12 ];    // If you add tabs to the UI you need to update this declaration
    void autoWalkThruTabs();
    void generateSummaryReport();

    void getFlashInfo();

    // FT-1696 ... Save off the values from the background processes
    QStringList flashInfo;
    QStringList netInfo;

    // FT-1270
    bool usbTabPassed;

    // FT-1269 / FT-1272
    QString dbConnectionType;

// FT-1328: Had to put reboot back in, due to need to enable openbox again
// FT-1330: Remove reboot after setting date/time in Manufacturing Test
    // FT-1271
    void saveDateTimeAfterServiceKeyRemoved();
    void reboot();

    void setMaxBacklight();

    QTimer getTestStatusTimer;
    QTimer *cutterTimer_;

    bool noWeigher;
    bool noCustomerDisplay;
    bool noWirelessNetworking;

    // FT-1333
    bool noCustomerSpeakers;

    // FT-1378
    bool noBluetooth;

    // FT-1223
    // Save initial Weigher Configsdd values
    int weigherModel;
    QString weigherFilterSpeed;
    int weigherScaleFactor; // Used for Division Size
    QString weigherMaximumWt;
    QString weigherMininumWt;
    QString netWeightDRLimitCheck;
    QString dualRangeLimit;
    bool isScaleValuemax;
    bool requestingValuemax;
    QString gcdLanguage;
    // Save original value for "Allow Printing without a Customer Display"
    QString originalPrintWithoutGCDValue;

    // FT-1423
    QString applicationVer;
    QString loaderVer;
    QString periphControllerVer;

    QString periphControllerVerWeigher;

    // FT-1418
    bool autoCompleteUsbTab;

    // FT-1417
    QTimer selectTextInFieldTimer;
    QLineEdit *selectTextInFieldWidget;

    // FT-1335
    QProcess *discoverBluetoothProcess;
    void startBluetoothDiscovery();
    int discoverBluetoothStatus;

    // FT-1610
    QString ethernetMacAddress;
    QString wirelessMacAddress;

    // FT-1626
    void getHostname();
    QString hostName;
    void sendScaleInformation();

    bool killStartupNetworking();

    // HT-177
    bool exitingScreen;

    // HT-430
    void populateBoardType();
    void populateSdCardSize();
    QString boardManf;

    // SOF-304
    void runOperatorDisplayTest();
    void runCustomerDisplayTest();

    // SOF-1296
    bool weigherResetSuccessful;
    bool resetFactoryDefaults();
    QTimer *startWeigherResetTimer;
    bool weigherResetTimerDone;


    // SOF-3047 - support 3 digit version numbers
    bool compareSoftwareVersions(QString v1, QString v2);

    // SOF-1719
    bool printerResetSuccessful;
    QTimer *startPrinterResetTimer;
    bool printerResetTimerDone;

    // SOF-1753
    bool dateTimeValidationErrorDisplayed;


    /**
     * Sleep function.
     */
    void sleepFor( int milliseconds ) const;

    // SOF-722: Testing a Depopulated board
    bool checkIfDepopBoard();
    bool isDepopBoardTestMode;

    //SOF-3752
    //Atmel peripheral Bd. 00-447780
    bool isPeripheralBoardTestMode;


    // SOF-891:  Validate Real-Time Clock
    bool setDateTimeSuccessful;

    // SOF-1773
    void getUbootVersionFromUsd();
    QString newLoaderVer;

    bool needUbootUpgrade;
    bool ubootUpgradeSuccessful;

    // SOF-1823
    void showCustomerSpeakerTest( bool showTest );
    bool isSelfServiceScale( QString modelName );

    // SOF-464
    bool isValidModel( QString modelName );
    bool isValidBoardRevNumber( QString boardRevNum );

    // SOF-2717
    bool factoryResetDone;
    bool factoryResetAttempted;

    // SOF-3556
    bool noCanBus;

    // SOF-3166
    void removeCutterTab();
    void showCutterTab();

    // SOF-3244: ValueMax FIT Parameters & Load Cell Calibration
    bool isHtmWeigher;
    bool haveFitParameters;
    bool loadCellCalibrationChecked;
    bool loadCellCalibrationFailed;
    long gravity_;
    long m_LbsCountsConvFactor_;
    long zeroValue_;
    long spanValue_;
    long z0ElectronicOffset_;
    long deadWeight_;
    long x2_;
    long x1_;
    long x0_;
    long y2_;
    long y1_;
    long y0_;
    long xRot_0_;
    long xRot_1_;
    long xRot_2_;
    long yRot_0_;
    long yRot_1_;
    long yRot_2_;
    long zRot_0_;
    long zRot_1_;
    long zRot_2_;
    long xGain_;
    long yGain_;
    long gCoeff_;
    long checkSum_;

    void checkLoadCellCalibration();
    QTimer *startRetrieveFitParamTimer;
    void confirmFitParams();
    //SOF-5608
    bool waitingForValueMax;
    bool checkFITParams;

    // SOF-3294: Validate Code Checker
    QTimer * dotWearTimer;
    bool codeCheckerEnabled_;
    bool dotWearTestPassed;
    bool haveBadDot;
    bool haveDotWearTestData;
    bool haveHeadDotWearDetailData;

    /**
     * Sends a request for dot wear and starts a timeout timer.
     *
     */
    void startDotWearTest();
    /**
     * Stops the timeout timer if active
     *
     */
    void dotWearTestFinished();

    //SOF-3449: Board Site Tests
    bool isArkBoardSite;
    int arkSerialFlash_;
    int arkSDSlot0_;
    int arkSDSlot1_;
    int arkRTC_;
    int arkUSB_;
    int arkWifi_;
    int arkBlueTooth_;
    int arkEthernet_;
    int arkMemory_;
    int arkCan0_;
    int kpcCan_;
    int kpcModel_;
    int kpcSerialFlash_;
    int kpcValueMax_;
    int kpcCutter_;
    int kpcDotWear_;

    // PJR-425
    int gcdValue_;
    void gcdDone();
    void findGcd();

    // SOF-5182
    bool printheadDisengaged_;
    MessageBoxDialog *calDialog;
    MessageBoxDialog *calInitDialog;
    MessageBoxDialog *calBackingDialog;

    // SOF-5995
    MessageBoxDialog *swapToLinerlessDialog;
    bool swapLabelStockRoutineStarted_;
    bool interlockEngaged_;
    bool hasBeenDisengaged_;
    QSet<int> lowHighReadings;
    bool labelStockSensorPassed_;
    bool trackLabelStockSensorValue_;
    bool trackStockMetrics_;

    // SOF-5996
    void runProcesses();
    void initLabelsOnDataReceived();

    // SOF-5997
    QLabel *ceilingComparison;
    QLabel *ceilingComparisonLabel;
    bool shootThroughValueOK;
    bool shootThroughRoutineCanceled;


    // SOF-3518: Need to delay requesting the Printer Dot Status
    void requestPrinterDotStatus();
    bool requestPrinterDotStatusAttempted;

    // SOF-3853:  Do not calibrate media sensor when there is a Cutter attached
    void removeMediaSensorCalibration();

    // SOF-4020
    QTimer *startReportTimer;
    bool reportTimerDone;

    // SOF-4024: Add X and Y to label
    QString xTilt;
    QString yTilt;
    bool valueMaxIsEnabled_;

    bool isSSKPrinterAttached();

    void installButtonEventFilter();

    bool shootThroughCalStarted;
    bool changingScreens;

    QPalette defaultBtnPal;
    QStringList calibrationSteps;
    void initShootThroughDialogText(MessageBoxDialog* diag);
    MessageBoxDialog* activeDialog;

    enum GapCalStates
    {
        _CalInit,
        _BackingPaper,
        _LabelPlusBacking,
        _CalcDeflections,
        _FailureSettingI,
        _FailureMemory,
        _Done
    };
};


#endif // MANUFACTURINGTEST_H
