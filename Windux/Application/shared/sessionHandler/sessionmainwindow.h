/**
 * @file sessionmainwindow.h
 * Main window widget for scale session.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SESSIONMAINWINDOW_H
#define SESSIONMAINWINDOW_H

#include "overlaywidget.h"
#include "screens/screenwidget.h"
#include "widgets/minandmaxprintwidget.h"


#include <Session.h>
#include <ScaleClasses.h>

#include <QMainWindow>
#include <QMap>
#include <QStackedWidget>

class KeyboardWidget;
class NumberPadWidget;

class ModeWidget;
class OperateMode;
class SupervisorMode;
class ServiceMode;

class SessionDialog;
class SessionGestureRecognizer;
class WebBrowserDialog;
class RemovePeripheralsDialog;
class ManufacturingImageDialog;
class MadpInternalConfig;
class MadpCutTestPlu;
class DomCurrentPLU;

class QTranslator;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @enum SessionType
 * @brief Type of sessions.
 */
enum SessionType
{
    PrimarySessionType              = 1,                ///< Primary user interface session.
    SecondarySessionType            = 2                 ///< Secondary user interface session.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class SessionMainWindow
 * @brief Main window widget for a scale session.
 */
class SessionMainWindow : public QMainWindow
{
    Q_OBJECT

    friend class Overlay;
    friend class SessionDialog;

    Q_PROPERTY( bool isOperateMode READ isOperateMode )
    Q_PROPERTY( bool isSupervisorMode READ isSupervisorMode )
    Q_PROPERTY( bool isServiceMode READ isServiceMode )
    Q_PROPERTY( SessionType sessionType READ sessionType )
    Q_PROPERTY( bool isPrimarySessionType READ isPrimarySessionType )
    Q_PROPERTY( bool isSecondarySessionType READ isSecondarySessionType )
    Q_PROPERTY( bool isDialogOpen READ isDialogOpen )
    Q_PROPERTY( QString language READ language WRITE setLanguage )
    Q_PROPERTY( Qt::DockWidgetArea numberPadArea READ numberPadArea WRITE setNumberPadArea )
    Q_PROPERTY( bool offline READ offline WRITE setOffline )

    //-----------------------------------------------------------------
    // QA automated test properties:
    //
    // The property name and method(s) need to have different names
    // so that Squish can differentiate between them.  Otherwise Squish
    // always chooses the property name and will generate an error
    // "int object is not callable" when you try to call the method.
    //-----------------------------------------------------------------
    Q_PROPERTY( QString scaleTestWeight READ scaleTestWeight WRITE setScaleTestWeight )
    Q_PROPERTY( bool value WRITE endScaleTestWeight )
    Q_PROPERTY( bool printerLabelTaken READ printerLabelTaken )

    typedef SessionMainWindow _Myt;
    typedef QMainWindow _Mybase;

Q_SIGNALS:

    /**
     * This signal is emitted when cutting test is entered or exited.
     *
     * @param[in] value  @c true when on, @c false when off
     */
    void cuttingTestChanged( bool value );

    /**
     * This signal is emitted when the keyboard is shown or hidden.
     *
     * @param[in] value  @c true when keyboard is shown, @c false when hidden
     */
    void keyboardVisible( bool value );

    /**
     * Signal for when number pad layout style changes.
     */
    void numberPadKeyLayoutStyleChanged();

    /**
     * Signal for when number pad moved.
     *
     * @param[in] where  where the number pad was moved to
     */
    void numberPadMoved( Qt::DockWidgetArea where );


    /**
     * Signal for when the menu panel is dismissed.
     */

    void hidingMenuPanel();

public:

    static const int scalingDividerX_ = 1024;           ///< Scaling divider for width.
    static const int scalingDividerY_ = 600;            ///< Scaling divider for height.
    int currentX_;
    int currentY_;

    /**
     * Keyboard spawn locations.
     */
    enum KeyboardLocation
    {
        KeyboardLocationBottom,                         ///< Spawn keyboard on bottom of screen
        KeyboardLocationUsePrevious,                    ///< Spawn keyboard in previously spawned location
        KeyboardLocationSmart,                          ///< Spawn keyboard depending on where input widget is located
        KeyboardLocationTop                             ///< Spawn keyboard on top of screen
    };

    /**
     * Which menu panel should be shown.
     */
    enum MenuPanelMode
    {
        CurrentMenuPanelMode,                           ///< Open panel with current mode panel shown
        OperateMenuPanelMode,                           ///< Open panel with the operate mode panel shown
        SupervisorMenuPanelMode,                        ///< Open panel with the supervisor mode panel shown
        ServiceMenuPanelMode,                           ///< Open panel with the service mode panel shown
        SearchMenuPanelMode,                            ///< Open panel with the search mode panel shown
        LanguageMenuPanelMode,                          ///< Open panel with the language mode panel shown
        HelpMenuPanelMode,                              ///< Open panel with the help mode panel shown
        ShutdownMenuPanelMode                           ///< Open panel with the shutdown mode panel shown
    };

    /**
     * Result code for registerSession() method.
     */
    enum RegisterSessionResult
    {
        RegisterSessionResultOkay,                      ///< Good result, registration success.
        RegisterSessionResultNoFilesystem,              ///< Filesystem could not connect.
        RegisterSessionResultTimeout,                   ///< No response from scale.
        RegisterSessionResultRejected                   ///< Scale rejected connection.
    };

    // HT-374: Moved to here instead of being duplicated several times
    enum WeigherModels
    {
        None = 0,	//	0 NS means these models are not supported in the HT series
        ESS,		//	  NS
        U2000S1,	//	  NS
        U2000S3,	//	  NS
        UWS,		//	  NS
        QuantHC,	//	5 NS
        ESC,		//	  NS
        EPSA,		//	  NS
        HLX1,       //    0.005 dual range
        EzUSB,		//    NS
        AWS,        // 10 0.005 dual range
        EPSA2,      //    0.01 single range
        AWS2,       //    0.01 single range
        EPSAC2,     //    New USB Compact Label Applier
        SASCALE1,   //    SASCALE-1 for SASCALE
        SASCALE2,   // 15 SASCALE-2 for SASCALE: dual range
        HTM			//	  New CAN Weigher dual range for KPCweigher/HTm model scale (SOF-3140)
    };

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] device  session device
     * @param[in] sessionType  session type
     * @param[in] s  window size
     * @param[in] user  username
     * @param[in] pwd  password
     * @param[in] parent  parent widget
     */
    explicit SessionMainWindow( QIODevice *device, SessionType sessionType, const QSize &s, const QString& user = QString(), const QString& pwd = QString(), QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~SessionMainWindow();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Returns the mode widget for the main window. This function returns zero if the mode has not
     * been set.
     *
     * @returns  mode widget
     */
    virtual ModeWidget *mode() const;

    /**
     * Check if current mode is operate.
     *
     * @returns  @c true if in operate mode, @c false otherwise
     */
    virtual bool isOperateMode() const;

    /**
     * Check if current mode is supervisor.
     *
     * @returns  @c true if in supervisor mode, @c false otherwise
     */
    virtual bool isSupervisorMode() const;

    /**
     * Check if current mode is service.
     *
     * @returns  @c true if in service mode, @c false otherwise
     */
    virtual bool isServiceMode() const;

    /**
     * Checks to find out if this PLU being requested can only be used for remote printing.
     *
     * @returns  @c true if in service mode, @c false otherwise
     */
    virtual bool isRemotePrintItem(QString plunum) const;

    /**
      * Retrieve session object.
      *
      * @returns  reference to session
      */
    virtual Session *session() const {return &const_cast<_Myt*>( this )->session_;}

    /**
     * Retrieve session type.
     *
     * @returns  session type
     */
    virtual SessionType sessionType() const {return sessionType_;}

    /**
     * Check if session is primary type.
     *
     * @returns  @c true if primary session, @c false otherwise
     */
    virtual bool isPrimarySessionType() const {return (PrimarySessionType == sessionType());}

    /**
     * Check if session is secondary type.
     *
     * @returns  @c true if secondary session, @c false otherwise
     */
    virtual bool isSecondarySessionType() const {return (SecondarySessionType == sessionType());}

    /**
     * Check for open dialog on screen.
     *
     * @returns  @c true if dialog or panel open, @c false otherwise
     */
    virtual bool isDialogOpen() const;

    /**
     * Returns the current Cut Test number
     * @return testNumber_ value
     */
    virtual int getCutTestNumber() const;

    /**
      * Returns the error code of the sessionerrordialog if there is one
      *@returns errorCode_ value
      */
    int getErrorCode() const;

    /**
     * Check for specific open dialog on screen.
     *
     * @returns  @c true if dialog or panel open, @c false otherwise
     */
    template <class Dialog>
    bool isDialogOpen() const;

    /**
     * Check for cutting test active.
     *
     * @returns  @c true if active, @c false otherwise
     */
    virtual bool isCuttingTestActive() const {return cuttingTestActive_;}

    /**
     * Check if overlay is visible.
     *
     * @returns  @c true if overlay visible, @c false otherwise
     */
    virtual bool isOverlayVisible() const {return overlay_->isVisible();}

    /**
     * Set overlay visibility.
     *
     * @param[in] value  @c true to set overlay visible, @c false otherwise
     */
    virtual void setOverlayVisible( bool value ) {return overlay_->setVisible( value );}

    /**
     * Retrieve current installed language.
     *
     * @returns  current install language
     */
    virtual QString language() const {return language_;}

    /**
     * Install language translator.
     *
     * This method will intall a language translator and apply it to all child windows. The
     * following languages are supported:
     * @arg EN.CA (Canadian English)
     * @arg EN.US (United States English)
     * @arg ES.MX (Mexican Spanish)
     * @arg FR.CA (Canadian French)
     *
     * @param[in] value  language from above list
     */
    virtual void setLanguage( const QString& value );

    /**
     * Retrieve number pad widget.
     *
     * @returns  pointer to number pad
     */
    virtual NumberPadWidget *numberPad() {return numberPad_;}

    /**
     * Retrieve number pad area.
     *
     * @returns  area where number pad resides
     */
    virtual Qt::DockWidgetArea numberPadArea() const {return numberPadArea_;}

    /**
     * Set number pad area.
     *
     * @param[in] value  area where number pad resides
     */
    virtual void setNumberPadArea( Qt::DockWidgetArea value );

    /**
     * Check if keyboard is visible.
     *
     * @returns  @c true if visible, @c false otherwise
     */
    virtual bool isKeyboardVisible() const;

    /**
     * Check if we are offline.
     *
     * @returns  @c true if offline, @c false otherwise
     */
    virtual bool offline() const {return offline_;}

    /**
     * Open the file dialog for some file dialog, and then pop their original file dialog back up.
     *
     * @returns  @c none
     */
    virtual void popFileDialog( int caller );

    /**
     * Retrieve number of devices attached.
     *
     * @param[in] type  HID device type
     * @return  number of attached
     */
    virtual int devicesAttached( int type ) const;

    /**
     * Retrieve pointer to mode.
     *
     * @returns  pointer to mode
     *
     * @tparam Mode  mode type
     */
    template <class Mode>
    Mode *retrieveMode();

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Registered session with scale service.
     *
     * @returns  register session result
     */
    virtual RegisterSessionResult registerSession();

    /**
     * Navigate to the home screen.
     */
    virtual bool navigateHome();

    /**
     * Activate self service mode.
     *
     * @returns  @c true on success, @c false otherwise
     */
    virtual bool activateSelfServiceMode();

    /**
     * Print key was pressed.
     */
    virtual void printKeyPressed();

    /**
     * Search key was pressed.
     */
    virtual void searchKeyPressed();

    /**
     * Show a document.
     *
     * This method will generate a new instance of video player and show the passed in document
     * @p filename.
     *
     * @param[in] filename  document to show
     *
     * @sa minimizeHelpDialog()
     * @sa restoreHelpDialog()
     */
    virtual void showDocument( const QString& filename );

    /**
     * Show a document.
     *
     * This method will show the passed in document contents represented as byte array
     * @p pdfData.
     *
     * @param[in] pdfData  contents of document
     *
     * @sa minimizeHelpDialog()
     * @sa restoreHelpDialog()
     */
    virtual void showDocument( const QByteArray& pdfData );

    /**
     * Show the help dialog.
     *
     * This method will generate a new instance of help and display it.
     *
     * @param[in] showServiceHelp  show service mode help
     *
     * @sa minimizeHelpDialog()
     * @sa restoreHelpDialog()
     */
    virtual void showHelp( bool showServiceHelp = false );

    /**
     * Show the list of missing peripherals dialog.
     */
    virtual void showRemovePeripherals();

    /**
     * Show a video.
     *
     * This method will generate a new instance of video player and start playing the passed in
     * video @p filename.
     *
     * @param[in] filename  filename to play
     * @param[in] delay  amount of time to wait (in ms) before playing
     * @param[in] isHelp  @c true if help video, @c false otherwise
     *
     * @sa minimizeHelpDialog()
     * @sa restoreHelpDialog()
     */
    virtual void showVideo( const QString& filename, int delay = 0, bool isHelp = false );

    /**
     * Show a web page.
     *
     * This method will open the passed in url in a web page dialog box.
     *
     * @param[in] url  url to open
     *
     * @sa minimizeHelpDialog()
     * @sa restoreHelpDialog()
     */
    virtual void showWebpage( const QString& url );

    /// Switches to the check upgrade screen.
    virtual void processFirmwareUpgrade();

    /**
     * Toggle cutting test on/off.
     *
     * @param[in] value  @c true to turn on, @c false to turn off
     */
    virtual void toggleCuttingTest( bool value );

    /**
     * Toggle pick5 test on/off.
     *
     * @param[in] value  @c true to turn on, @c false to turn off
     */
    virtual void togglePickFive( bool value );

    /**
     * Turn the background yellow again after restarting the test back up
     */
    virtual void flipBackgroundImage();

    /**
     * Reset some of the variables for a cutting test
     *
     * @param[in] value @c true to reset on, @c false to turn off
     */
    virtual void resetCuttingTest( bool value );

    /**
     * turn off the current cutting test
     *
     * @param[in] none
     * @param[out] none
     */
    virtual void turnOffCuttingTest();

    /**
     * Display the last cutting test report.
     */
    virtual void loadLastCuttingTestReport();

    /**
     * Start a Cut Test.
     *
     * This method will start a cut test and allow users to begin entering SubPrimal products to be
     * entered either manually or if a Meat Spec Sheet exists, wieth flashkeys
     *
     * @param[in] beginOrEndTest - indicates if this was this screen was launched to Begin or End a test.
     * @returns[out] none
     *
     */
    virtual void launchCuttingTest(int beginOrEndTest);

    /**
     * View Cut Test History
     *
     * This method will allow users to begin viewing cut test history. If the access rights are 4 or below
     * their actions will be restricted to viewing their own tests and an access >= 5 will allow all rights
     *
     * @param[in] none
     * @returns[out] none
     *
     */
    virtual void launchCuttingTestHistory();

    /**
     * View Cut Test Details
     *
     * This method will allow users to begin viewing details of a specific Cutting Test
     *
     * @param[in] returnHomeLocation - where the Details screen was originally launched from
     * @returns[out] none
     */
    virtual void launchCuttingTestDetails();

    /**
     * Change modes.
     *
     * For supervisor and service mode this method will attempt to unload the current screen!
     *
     * @returns  @c true if mode was changed, @c false otherwise
     *
     * @tparam Mode  mode to switch to
     */
    template <class Mode>
    bool switchToMode();

    /**
     * Change mode and screen.
     *
     * @returns  @c true if mode and/or screen was changed, @c false otherwise
     *
     * @tparam Mode  mode to switch to
     * @tparam Screen  screen to switch to
     */
    template <class Mode, class Screen>
    bool switchToModeScreen();

    /**
     * Change screen in current mode.
     *
     * @returns  @c true if screen was changed, @c false otherwise
     *
     * @tparam Screen  screen to switch to
     */
    template <class Screen>
    bool switchToScreen();

    // HT-361
    /**
     * Checks if Automatic Backup has been turned on and displays message if not.
     * Returns false when automatic database backup is not turned on.
     *
     * @returns  @c true on success, @c false otherwise
     */
    virtual bool checkForAutomaticBackup( QString& autoBackupValue );

    virtual bool checkForSystemTestKey();

    /**
     * Entirely way too many flashkey requests going from Super to Operate. This cuts down the num of requests by 49.6%.
     * Returns true when headed from Supervisor to Operate Mode
     *
     * @returns  @c true on success, @c false otherwise
     */
    virtual bool getIsHeadingToHome() const;

    /**
     * Entirely way too many flashkey requests going from Super to Operate. This cuts down the num of requests by 49.6%.
     * Returns true when headed from Supervisor to Operate Mode
     *
     * @returns  @c true on success, @c false otherwise
     */
    virtual void setIsHeadingToHome(bool value);

    // second label applier
    /* Think we can handle this in operate mode,  leaving this here just in case
    int getPrimaryPlatePosition() const;
    int getSecondaryPlatePosition() const;
    int getRequiredPrimaryPlatePosition() const;
    int getRequiredSecondaryPlatePosition() const;
    */

public Q_SLOTS:

    //
    // properties
    //

    /**
     * Set offline state.
     *
     * @param[in]  offline state
     */
    virtual void setOffline( bool value );

    /**
     * Set the menu panel open.
     *
     * @param[in] value  @c true when panel is open, @c false otherwise
     */
    virtual void setMenuPanelOpen( bool value );

    /**
     * Eject removable media.
     */
    virtual void ejectMedia();

    //
    // panels
    //

    /**
     * Show the menu panel for the current state.
     *
     * @param[in] panelMode  menu panel to show (i.e. operater, search, language)
     * @param[in] panelModeEx  extended panel mode
     */
    virtual void showMenuPanel( MenuPanelMode panelMode = CurrentMenuPanelMode, int panelModeEx = 0 );

    //
    // help dialog
    //

    /**
     * Minimize the help dialog.
     */
    virtual void minimizeHelpDialog();

    /**
     * Restore the help dialog.
     */
    virtual void restoreHelpDialog();

    //
    // now serving dialog
    //

    /**
     * Dismiss the now serving overlay.
     */
    virtual void dismissNowServingOverlay( int rc );

    //
    // input widgets
    //

    /**
     * Hide input widgets (i.e. keyboard and number pad).
     */
    virtual void hideInputWidgets();

    /**
     * Show the keyboard.
     */
    virtual void showKeyboard();

    /**
     * Show the keyboard in specific location.
     *
     * @param[in] loc  keyboard location
     */
    virtual void showKeyboard( KeyboardLocation loc );

    /**
     * Hide the keyboard.
     */
    virtual void hideKeyboard();

    /**
     * Show the number pad.
     */
    virtual void showNumberPad();

    /**
     * Hide the number pad.
     */
    virtual void hideNumberPad();

    //
    // gestures
    //

    /**
     * Cancel any active gesture.
     */
    virtual void cancelActiveGesture();

    /**
     * swipe the screen automatically when no swipe occurred.
     */
    virtual void swipeTheScreen(int direction);

protected:

    /**
     * Close event.
     *
     * @param[in,out] e  event
     */
    virtual void closeEvent( QCloseEvent *e );

    /**
     * Event filter.
     *
     * @param[in,out] e  event
     * @returns  @c true to filter, @c false otherwise
     */
    virtual bool event( QEvent *e );

    /**
     * Application event filter.
     *
     * @param[in] obj  object being filtered
     * @param[in,out] e  event
     * @returns  @c true to filter, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /**
     * Key press event.
     *
     * @param[in,out] e  event
     */
    virtual void keyPressEvent( QKeyEvent *e );

    /**
     * Move event.
     *
     * @param[in,out] e  move event
     */
    virtual void moveEvent( QMoveEvent *e );

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Sets the current mode.
     *
     * For supervisor and service mode this method will attempt to unload the current screen!
     *
     * @param[in] widget  mode widget
     * @returns  @c true if mode was changed, @c false otherwise
     *
     * @sa mode()
     */
    virtual bool setMode( ModeWidget *widget );

private Q_SLOTS:

    /**
     * Slot for error blocking expired.
     */
    void onBlockErrorsExpired();

    /**
     * Slot to check for media.
     */
    void onCheckMedia();

    /**
     * Slot to filesystem progress.
     */
    void onFilesystemDownloadFileInProgress( bool active, const QString& filename );

    /**
     * Slot to filesystem progress.
     */
    void onFilesystemSyncInProgress( bool active );

    /**
     * Slot to handle focus change.
     */
    void onFocusChanged( QWidget *oldFocus, QWidget *newFocus );

    /**
     * Slot to handle mode margins changing.
     */
    void onModeMarginsChanged();

    /**
     * Slot to handle number pad moved.
     */
    void onNumberPadMoved( Qt::DockWidgetArea where );

    /**
     * Slot to handle print key press.
     */
    void onPrintKeyPressed();

    /**
     * Slot to handle requesting weights and measures info.
     */
    void onRequestWeightsMeasuresInfo();

    /**
     * Slot to restore help dialog.
     */
    void onRestoreHelpDialog();

    /**
     * Slot to handle self service deativation.
     */
    void onSelfServiceHookActiveChanged();

    /**
     * Slot to send a character to the input widget.
     */
    void onSendCharacter( int key, Qt::KeyboardModifiers modifiers, const QString& text );

    /**
     * Slot to send test message to backend.
     */
    void onSendTestMessage();

    /**
     * Slot to handle session disconnect.
     */
    void onSessionDisconnected();

    /**
     * Slot to handle session message.
     */
    void onSessionMessageReceive( const SessionMessage& msg );

    /**
     * Slot to handle session error messages.
     */
    void onSessionMessageReceiveError( const SessionMessage& msg );

    /**
     * Show remove peripherals dialog.
     */
    void onShowRemovePeripheralsDialog();

    /**
     * Show the manufacturing image dialog.
     */
    void onShowManufacturingImageDialog();

    /**
     * Show video.
     */
    void onShowVideoTimer();

    /**
     * Log out the user and suspend the cut test if the user is inactive
     */
    void onLogoutCutTestInactivity();

    /**
     * Slot to handle locking the main screen messages.
     */
    void onSessionMessageReceiveLockDownInternal( bool isLocked );

    void onMenuPanelDismissed();

private:

    SessionGestureRecognizer *gestureRecognizer_;
    Qt::GestureType gestureType_;
    hobart::MinAndMaxPrintWidget *minmaxprintwidget_;

    KeyboardWidget *keyboard_;
    NumberPadWidget *numberPad_;
    KeyboardLocation previousKeyboardLocation_;

    Qt::DockWidgetArea numberPadArea_;

    OperateMode *operateMode_;
    SupervisorMode *supervisorMode_;
    ServiceMode *serviceMode_;

    QStackedWidget *centralWidget_;

    Session session_;
    SessionType sessionType_;

    QString language_;
    QString fileerror_;

    QStringList pluList;

    QTranslator *translator_;
    QTranslator *qtTranslator_;

    MadpInternalConfig *internalConfigData_;
    MadpCutTestPlu *cutTestPlu;

    DomCurrentPLU *currentPlu_;

    bool offline_;
    bool online_;
    bool removePeripheralsVisible_;
    bool allowTerminate_;
    bool painting_;
    bool pick5Toggled_;
    bool cuttingTestActive_;
    bool cuttingTestDialogActive_;
    QTimer *cuttestInactivityLogout_;

    int initializePhase_;
    int testNumber_;
    int dialogCaller_;
    int physicalMouseAttached_;
    int physicalKeyboardAttached_;
    int physicalScannerAttached_;
    int usbDeviceAttached_;

    int menuPanelCount_;

    QTimer *queuedVideoTimer_;
    QString queuedVideo_;

    QTimer *blockErrorTimer_;
    bool blockError_;

    int x_offset_;
    int y_offset_;

    int nowServingCustomer_;

    // second label applier stuff
    // this stuff may be able to just be handled in operate mode, just gonna leave this here just in case
    /*
    // these are all in inches
    int primaryPlatePosition_, secondaryPlatePosition_;
    int requiredPrimaryPlatePosition_, requiredSecondaryPlatePosition_;
    int pkgId_; // don't think the UI needs this information, at least not yet
    */

    QWidget *inputWidget_;

    QMutex registeredSignal_;
    QMutex remoteAccessLogin_;
    QMutex restoringDatabase_;
    QMutex terminate_;

    QTimer *checkMediaHandler_;
    QTimer *printKeyHandler_;
    QTimer *weightsMeasuresHandler_;

    QTimer *showRemovePeripherals_;


    static const int registerSessionTimeout_;

    // FT-578
    bool systemTestKeyWasInserted;

    // HT-326
    bool cursorShownForRdpSession_;

    // HT-390
    bool cursorShownForManfTestKey_;

    // SOF-232
    bool showCursorForBoardTester_;

    bool valuemaxIsEnabled_;        // is this a valuemax enabled scale

    //SOF-5068 - manages all cases where cursor should be shown
    void manageCursor();

    // SOF-3971
    bool isManfTestKeyPresent;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Determine if the scale is 3 degrees out of tilt.
     */
    void setOutOfTilt(const QString& xTilt, const QString& yTilt);

    /**
     * Toggle background image.
     */
    void toggleBackgroundImage();

    /** checkFileName()
     *
     *  checkFileName checks to determine if a valid file was entered. If the filename
     *  is an empty string, or one of a number of illegal characters, a warning will
     *  be displayed to the user indicating the problem.
     *  @param[in] myfile       - the file name the user entered
     *        [in] fileOrFolder - indicates if a filename or foldername was entered
     *  @return if file was valid
     */
    bool checkFileName( QString &myfile, int fileOrFolder );

    /** getFileName()
     *
     *  getFileName determines if a filename was entered with an extension. The extension
     *  is displayed in the file type combo box, and is automatically append the extension
     *  selected by default. However, without question some user will add the extension
     *  while entering the file name itself. This method fixes any issues that may be
     *  encountered if the user adds the entension themselves.
     *  @param: myfile       - the file name the user entered
     *  @return: String - the valid file name with the extension properly appended.
     */
    QString getFileName(QString myfile);

    /**
     * The reads at boot up time the list of PLUs that are reserved for remote printing
     *
     * @param[in] none
     * @returns  @c has a stringlist of valid remote print PLUs
     */
    void readPluWhiteList();

    /**
     * The method determines if we are running on LINUX or WINDOWS to set initial directories
     *
     * @param[in] none
     * @returns  @c true if system OS is a LINUX based system, @c else false which indicates Windows
     */
    static bool isLinux();

    /**
     * Calculate keyboard geometry.
     */
    QRect calcKeyboardGeometry( KeyboardLocation loc = KeyboardLocationSmart );

    /**
     * Calculate number pad geometry.
     */
    QRect calcNumberPadGeometry() const;

    /**
     * Swipe the screen.
     */
    void swipeScreen( int direction );

    /**
     * Sleep function.
     */
    void sleepFor( int milliseconds ) const;

    /**
     * Request weights and measures info.
     */
    void requestWeightsMeasuresInfo();

    /**
     * find desktop center to place dialog
     */
    void setScreenLocation();

    //
    // dialog management
    //

    typedef QList<SessionDialog*> SessionDialogList;

    SessionDialogList dialogList_;
    QMutex dialogListMutex_;

    SessionDialog *sessionError_;
    int errorCode_;

    /**
     * Reject one dialog (if possible).
     */
    bool rejectDialog( SessionDialog *dialog = NULL, bool forced = false );

    /**
     * Forcefully reject @p count dialogs that are queued.
     */
    void rejectDialogs( int count = INT_MAX );

    /**
     * Enable the mainWindow after leaving self service
     */
    void unlockMainWindow( bool isEnabled );

    /**
     * Start the cut test operator inactivity timeout with no questions asked
     */
    void startCutTestIdleTimer();

    /**
     * Resets the cut test operator inactivity timeout
     */
    void resetCutTestIdleTimer();

    /**
     * Updates the playlist in self service mode if the playlist has changed.
     */
    void resetSelfServicePlaylist();

    //
    // video player
    //

    enum WebBrowserCommand
    {
        Minimize,
        Restore,
        ShowDocumentFromFile,
        ShowDocumentFromMemory,
        ShowHelp,
        ShowVideo,
        ShowWebpage
    };

    /**
     * Web browser dialog management.
     */
    void handleWebBrowserCommand( WebBrowserCommand fn, const QString& param = QString(), const QByteArray& fileData = QByteArray() );

    /**
     * Check for web browser dialog visible.
     */
    bool isWebBrowserVisible() const {return webBrowserVisible_;}

    /**
     * Cleanup memory of web browser.
     */
    void freeWebBrowser();

    WebBrowserDialog *webBrowser_;
    bool webBrowserVisible_;

    // SOF-3534
    bool boardSiteWasStarted;

    //
    // overlay
    //

    /**
     * @class Overlay
     * @brief Overlay class for painting the slide button.
     */
    class Overlay : public OverlayWidget
    {
        typedef Overlay _Myt;
        typedef OverlayWidget _Mybase;

    public:

        ///////////////////////////////////////////////////////////////////////
        // CTOR / DTOR

        /// Constructor.
        explicit Overlay( Qt::DockWidgetArea area, QWidget *parent = 0 );

        /// Destructor.
        virtual ~Overlay();

        ///////////////////////////////////////////////////////////////////////
        // Properties

        /// Retrieve widget dock area.
        virtual Qt::DockWidgetArea area() const {return area_;}

        /// Retrieve bottom margin.
        virtual int bottomMargin() const {return bottomMargin_;}

        /// Check if min/max icon is visible.
        virtual bool dialogIconVisible() const {return (dialogIconCounter_ > 0);}

        /// Check if eject media button is visible.
        virtual bool ejectMediaVisible() const {return ejectMediaVisible_;}

        /// Check if overlay is opened.
        virtual bool open() const {return (openCounter_ > 0);}

        /// Set widget dock area.
        virtual void setArea( Qt::DockWidgetArea value );

        /// Set bottom margin.
        virtual void setBottomMargin( int value );

        /// Set if min/max icon is visible.
        virtual void setDialogIconVisible( bool value );

        /// Set if eject media button is visible.
        virtual void setEjectMediaVisible( bool value ) {ejectMediaVisible_ = value;}

        /// Set if status message is visible.
        virtual void setMessageVisible( bool value, const QString& text = QString() );

        /// Set if overlay is open.
        virtual void setOpen( bool value );

    protected:

        /// Event filter.
        virtual bool eventFilter( QObject *obj, QEvent *event );

        /// Paint event.
        virtual void paintEvent( QPaintEvent *event );

        /// Resize event.
        virtual void resizeEvent( QResizeEvent *event );

    private:

        typedef QMap<int, QPixmap> pixmapMap;
        typedef QMap<int, QRect> pixmapRectMap;

        SessionMainWindow *mainWindow_;
        Qt::DockWidgetArea area_;

        int bottomMargin_;

        int openCounter_;
        int dialogIconCounter_;

        bool messageVisible_;
        QString message_;

        static const QMargins buttonWhitespace_;

        /// Calculate graphic location rects.
        void calcGraphicRects();

        /// Handle menu button press.
        bool handleMenuButtonPress( const QPoint& pos );

        //
        // buttons
        //

        enum ButtonType
        {
            PanelButtonType,                        // button with arrow to open/close panel
            HomeButtonType,                         // button with home icon
            ZeroScaleButtonType,                    // button with zero scale icon
            EjectMediaButtonType                    // eject media button
        };

        enum ButtonState
        {
            NormalPanelButtonState,                 // white arrow to open panel
            OpenPanelButtonState,                   // blue arrow to close panel
            NormalHomeButtonState,                  // home button centered
            OverlayHomeButtonState,                 // home button with icon overlay
            NormalZeroScaleButtonState,             // zero scale button
            NormalEjectMediaButtonState             // eject media button
        };

        /// Convert button details into a button id
        static int makeButtonId( Qt::DockWidgetArea a, ButtonState s ) {
            return ((a*1000) + s);
        }

        /// Convert button details into a button rect id
        static int makeButtonRectId( Qt::DockWidgetArea a, ButtonType t ) {
            return ((a*1000) + t);
        }

        /// Retrieve pixmap
        void buttonInfo( ButtonType t, QPixmap& pixmap, QRect& rect ) const;

        pixmapMap button_;
        pixmapRectMap buttonRect_;

        bool ejectMediaVisible_;

        //
        // icons
        //

        enum IconType
        {
            NoIconType = 0,                         // no icon
            MinimizeIconType,                       // minimize icon
            RestoreIconType                         // restore icon
        };

        /// Retrieve current icon type
        IconType currentIconType() const;

        /// Convert icon details into an icon id
        static int makeIconId( Qt::DockWidgetArea a, IconType t ) {
            return ((a*1000) + t);
        }

        /// Convert icon details into an icon rect id
        static int makeIconRectId( Qt::DockWidgetArea a ) {
            return a;
        }

        /// Retrieve icon
        bool iconInfo( QPixmap& pixmap, QRect& rect ) const;

        pixmapMap icon_;
        pixmapRectMap iconRect_;

    };

    Overlay *overlay_;

    //
    // language support
    //

    /**
     * Install a translator or two.
     */
    static void installTranslator( QTranslator *translator );
    static void installQtTranslator( QTranslator *translator );

    static QTranslator *installedTranslator_;
    static QTranslator *installedQtTranslator_;

public:

    //-------------------------------------------------------------------------
    // Automated Testing Support
    //-------------------------------------------------------------------------

    /**
     * This method does nothing, please do not use. It is only here because Qt requires a READ
     * method for a WRITE method to exist.
     *
     * @returns  empty string
     */
    virtual QString scaleTestWeight() const {return QString();}

    /**
     * This method will block until a label has been taken from the printer (i.e. blocks until
     * the label taken sensor is cleared).
     *
     * This method requires the option "AutoTesterNeedsLabelTaken" to be enabled in the
     * configuration file (i.e. config_linux.dat). It also requires a QA service key to function
     * properly.
     *
     * @warning
     * This function is for automated testing use only!
     *
     * @returns  @c true when label taken, @c false if error (i.e. timeout or no key)
     */
    virtual bool printerLabelTaken() const;

public Q_SLOTS:

    //------------------------------------------------------------------------------
    // These methods need to remain as "public Q_SLOTS" so that Squish can see them.
    //------------------------------------------------------------------------------

    /**
     * This method will send a dummy scale weight to the backend. The backend will then adjust the
     * platter weight to the sent weight and update the ui as if a weight was placed on the
     * platter.
     *
     * This method will put the backend into 'fake weight mode' where all weight messages
     * from the weigher are intercepted and thrown away.
     *
     * This method requires requires a QA service key to function properly.
     *
     * @warning
     * This function is for automated testing use only!
     *
     * @param[in] value  weight
     */
    virtual void setScaleTestWeight( const QString& value );

    /**
     * This method will send a message to the backend to end 'fake weight mode'. Weight
     * messages from the weigher will then be passed on to the UI.
     *
     * This method requires requires a QA service key to function properly.
     *
     * @warning
     * This function is for automated testing use only!
     *
     * @param[in] value  weight
     */
    virtual void endScaleTestWeight( bool value );

private:

    mutable QMutex labelTakenSignal_;
    bool sentSkipCustomer;
    bool headingToHome_;

};

#include "sessionmainwindowimpl.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONMAINWINDOW_H
