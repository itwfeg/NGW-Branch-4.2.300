/**
 * @file sessionmainwindow.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <process.h>

#include <sessionhooks.h>
#include <selfservicehook.h>

#include "sessionmainwindow.h"
#include "sessionmainwindowimpl.h"

#include "keyboardwidget.h"
#include "numberpadwidget.h"

#include "sessiondialog.h"
#include "filesystem.h"

#include "sessionmenupanel.h"
#include "sessiongesturerecognizer.h"

#include "dialogs/filedialog.h"
#include "dialogs/camerapredictionsdialog.h"
#include "dialogs/cuttingtestdialog.h"
#include "dialogs/texteditor.h"
#include "dialogs/messageboxdialog.h"
#include "dialogs/operatorlogindialog.h"
#include "dialogs/printersettingsdialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/sessionerrordialog.h"
#include "dialogs/webbrowserdialog.h"
#include "dialogs/removePeripheralsDialog.h"
#include "dialogs/weightsandmeasuresdialog.h"
#include "dialogs/manufacturingImageDialog.h"
#include "dialogs/auditlogdialog.h"
#include "widgets/minandmaxprintwidget.h"
#include "widgets/dynamiccontentwidget.h"

#include "modes/operatemode.h"
#include "modes/supervisormode.h"
#include "modes/servicemode.h"

#include "panels/mainmenupanel.h"
#include "panels/modepanel.h"

#include "platform.h"

#include "screens/checkUpgradeAccessCLA.h"
#include "screens/upgradeKPCSoftware.h"
#include "screens/checkUpgradeAccessWrapper.h"
#include "screens/cuttingtestreport.h"
#include "screens/cuttingTestHistory.h"
#include "screens/cuttingTestInitialInformation.h"
#include "screens/cuttingTestDetails.h"
#include "screens/diagwizardscreen.h"
#include "screens/operateNewWrapperRunIn.h"
#include "screens/buildamealscreen.h"
#include "screens/manufacturingtest.h"
#include "screens/meatspecsheetscreen.h"
#include "screens/plumainscreen.h"
#include "screens/upgradeAccessCLA.h"
#include "screens/upgradeAccessWrapper.h"
#include "screens/upgradeNextGenWrapper.h"
#include "screens/webbrowserscreen.h"
#include "../../source/HFC/CfgBaseFieldOffsets.h"

#include "screens/wrappersettings.h"

#include <backgroundwidgettypes.h>
#include <fileaccess.h>
#include <filesystem.h>
#include <palette.h>
#include <shellzip.h>

#include <HobSocket.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <QDesktopWidget>
#include <QSize>
#include <ScaleClasses.h>


#include <QtCore>
#include <QApplication>
#include <QGestureEvent>
#include <QSwipeGesture>
#include <QDateTime>

#include <QAbstractButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QWebView>
#include <QScrollBar>
#include <QSlider>
#include <QMouseEvent>

#include <softkeyswidget.h>

extern bool isDemoMode;

QTranslator *SessionMainWindow::installedTranslator_ = NULL;
QTranslator *SessionMainWindow::installedQtTranslator_ = NULL;

// time to wait for response to session register message (in ms)
const int SessionMainWindow::registerSessionTimeout_ = 120000;
const int W_A_M = 1;

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionMainWindow::SessionMainWindow( QIODevice *device, SessionType sessionType, const QSize &s, const QString& user, const QString& pwd, QWidget *parent ) :
    _Mybase( parent, Qt::FramelessWindowHint ),
    gestureRecognizer_( new SessionGestureRecognizer() ),
    keyboard_( NULL ),
    numberPad_( NULL ),
    previousKeyboardLocation_( KeyboardLocationBottom ),
    numberPadArea_( Qt::RightDockWidgetArea ),
    operateMode_( NULL ),
    supervisorMode_( NULL ),
    serviceMode_( NULL ),
    session_( device ),
    sessionType_( sessionType ),
    language_( "" ),
    translator_( NULL ),
    qtTranslator_( NULL ),
    offline_( false ),
    online_( false ),
    removePeripheralsVisible_( false ),
    allowTerminate_( false ),
    painting_(),
    pick5Toggled_( false  ),
    cuttingTestActive_( false ),
    cuttingTestDialogActive_( false ),
    cuttestInactivityLogout_( NULL ),
    initializePhase_( 0 ),
    testNumber_( 1 ),
    dialogCaller_( 0 ),
    physicalMouseAttached_( (PrimarySessionType == sessionType ? 0 : 1) ),
    physicalKeyboardAttached_( 0 ), //(PrimarySessionType == sessionType ? 0 : 1) ),
    physicalScannerAttached_( 0 ),
    usbDeviceAttached_( 0 ),
    menuPanelCount_( 0 ),
    blockError_( false ),
    x_offset_( 0 ),
    y_offset_( 0 ),
    inputWidget_( NULL ),
    errorCode_(-1),

    webBrowser_( NULL ),
    webBrowserVisible_( false ),
    overlay_( NULL ),
    headingToHome_( false )
{
    LOG_CLASS_INFO( "SessionMainWindow", "SessionMainWindow" );

    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "SessionMainWindow" );

    // set window size
    setFixedSize( s );
    currentX_=s.width();
    currentY_=s.height();
    sentSkipCustomer = false;

    // set application icon
    qApp->setWindowIcon( QIcon( ":/images/rsa.ico" ) );

    LOG_INFO( "attached keyboards %d mice %d", physicalKeyboardAttached_, physicalMouseAttached_ );

    // hide mouse when no physical mice attached
    // HW-220 - Always show the cursor
    // HT-57 - Only on Windows... for Linux you can plug mouse and see cursor to get to calibration
#if defined( Q_OS_LINUX )
    if ( !physicalMouseAttached_ )
        QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
#endif

    // HT-326
    cursorShownForRdpSession_ = false;
    // HT-390
    cursorShownForManfTestKey_ = false;

    // SOF-232
    showCursorForBoardTester_ = false;

    valuemaxIsEnabled_ = false;        // is this a valuemax enabled scale

    // SOF-3534
    boardSiteWasStarted = false;

#ifdef QT_DEBUG
    LOG_INFO( "DEBUG BUILD!", NULL );
#endif
#ifdef Q_OS_WIN
    physicalMouseAttached_++;
    manageCursor();
#endif

    internalConfigData_ = &session_.scaleConfig->internalConfig;
    minmaxprintwidget_ = NULL;

    // FT-578
    systemTestKeyWasInserted = false;

    // SOF-3971
    isManfTestKeyPresent = false;

    // install keyboard hooks
    qApp->installEventFilter( this );

    // register data types
    qRegisterMetaType<SessionMessage>( "SessionMessage" );

    // register gesturing
    grabGesture( gestureType_ = QGestureRecognizer::registerRecognizer( gestureRecognizer_ ) );

    // grab pointer to socket
    HobSocket *socket = dynamic_cast<HobSocket*>( device );

    // connect into session
    connect( &session_, SIGNAL( disconnected() ), SLOT( onSessionDisconnected() ) );
    connect( &session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceive(SessionMessage) ) );
    connect( &session_, SIGNAL( receiveErrorMessage(SessionMessage) ), SLOT( onSessionMessageReceiveError(SessionMessage) ), Qt::QueuedConnection );
    connect( &session_, SIGNAL( receiveLockDownScreenMessage(bool) ),  SLOT( onSessionMessageReceiveLockDownInternal(bool) ) );
    // TODO Cut Test: Find out if we need this.
    // Noticed that we had a signal slot combo when there was no slot method.
    // Dont know yet if this is an issue.  So we need the method? Nobody is calling it or emitting it the SIGNAL
    // connect( &session_, SIGNAL( loadLastCutTestReport() ),  SLOT( onLoadLastCuttingTestReport() ) );

    // check for remote connection
    if ( socket )
    {
        QString peerName( socket->peerName() );

        // remote session
        if ( peerName.length() )
            if (( "localhost" != peerName ) && ( "127.0.0.1" != peerName ))
            {
                fileAccess *a;

                // setup filesystem
                LOG_DEBUG( "connecting filesystem to host '%s'", qPrintable( peerName ) );
                filesystem::connectToHost( (void*)(&session_), peerName, user, pwd );

                // connect
                if ( (a = filesystem::instance( (void*)(&session_) )) )
                {
                    connect( a, SIGNAL( downloadFileInProgress(bool,QString) ), SLOT( onFilesystemDownloadFileInProgress(bool,QString) ) );
                    connect( a, SIGNAL( syncInProgress(bool) ), SLOT( onFilesystemSyncInProgress(bool) ) );
                }
            }
    }

    // save session type
    session_.scaleConfig->myConsole = sessionType;

    // connect
    connect( sessionHooks::instance(), SIGNAL( print() ), SLOT( onPrintKeyPressed() ) );

    // initialize
    LOG_TRACE( "initialize...", NULL );
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionMainWindow::~SessionMainWindow()
{
    LOG_CLASS_INFO( "SessionMainWindow", "~SessionMainWindow" );

    // remove hooks
    sessionHooks::destroy();

    // delete all dialogs
    LOG_TRACE( "rejecting dialogs...", NULL );
    rejectDialogs();

    // unregister gesturing
    QGestureRecognizer::unregisterRecognizer( gestureType_ );

    // delete input widgets
    if ( keyboard_ )
        keyboard_->deleteLater();

    if ( numberPad_ )
        numberPad_->deleteLater();

    if( minmaxprintwidget_ )
        delete minmaxprintwidget_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ModeWidget *SessionMainWindow::mode() const
{
    return qobject_cast<ModeWidget*>( centralWidget_->currentWidget() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::setMode( ModeWidget *widget )
{
    LOG_CLASS_INFO( "SessionMainWindow", "setMode" );

    ModeWidget *currentMode = mode();

    // unload current screen
    if ( currentMode )
    {
        LOG_DEBUG( "unloading screen...", NULL );

        if ( !currentMode->onScreenChanging() )
        {
            LOG_WARN( "cannot unload screen in mode '%s'", currentMode->objectName().toLatin1().constData() );
            return false;
        }
    }

    // already in requested mode nothing to do
    if ( widget == currentMode )
        return true;

    // when switching out of supervisor or service destroy the screen completely
    if (( supervisorMode_ == currentMode ) || ( serviceMode_ == currentMode ))
    {
        LOG_DEBUG( "unloading supervisor or service mode screen...", NULL );
        currentMode->setScreen( NULL );
    }

    // FT-965 PLU field in All fields / Supervisor shows last used PLU in Operator
    // leaving OperateMode to go someplace else. Don't care where, just clear the PLU
    if( operateMode_ == currentMode )
    {
        internalConfigData_->changeProductActive = false;
        currentPlu_ = &session_.scaleConfig->currentPLU;
        currentPlu_->itemNumber() = "0";                            // doing this will clear the PLU

        // exit any cutting test
        toggleCuttingTest( false );

        // remove any pick5 if it exists
        if( minmaxprintwidget_ )
            minmaxprintwidget_->hide();
    }

    // hide input widgets
    hideInputWidgets();

    // disconnect slots of current mode
    if ( currentMode )
        disconnect( currentMode );

    // connect slots
    connect( widget, SIGNAL( marginsChanged() ), SLOT( onModeMarginsChanged() ) );

    // set the new mode
    centralWidget_->setCurrentWidget( widget );

    onModeMarginsChanged();

    LOG_DEBUG( "activate mode attempt", NULL );
    // activate the mode
    widget->activate();
    LOG_DEBUG( "activate mode complete", NULL );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::isOperateMode() const
{
    return (( operateMode_ ) && ( operateMode_ == mode() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::isSupervisorMode() const
{
    return (( supervisorMode_ ) && ( supervisorMode_ == mode() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::isServiceMode() const
{
    return (( serviceMode_ ) && ( serviceMode_ == mode() ));
}

bool SessionMainWindow::isRemotePrintItem(QString plunum) const
{
    bool isRemoteItem = false;

    QString item;

    foreach( item, pluList)
        if( plunum == item )
            return true;

    return isRemoteItem;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::setLanguage( const QString& value )
{
    LOG_CLASS_INFO( "SessionMainWindow", "setLanguage" );

    if ( language_ == value )
        return;

    LOG_INFO( "setting language to '%s'", value.toLatin1().constData() );

    QTranslator *oldTranslator( translator_ );
    QTranslator *newTranslator( NULL );

    QTranslator *oldQtTranslator( qtTranslator_ );
    QTranslator *newQtTranslator( NULL );

    QLocale::Language lang = QLocale::English;
    QLocale::Country country = QLocale::UnitedStates;

    bool valid = false;
    bool validQt = false;

    // canadian english
    if ( "EN.CA" == value )
    {
        newQtTranslator = new QTranslator;
        validQt = newQtTranslator->load( "qt_en",  QLibraryInfo::location(QLibraryInfo::TranslationsPath) );
		
        newTranslator = new QTranslator;
        valid = newTranslator->load( ":/languages/lang_en_ca" );
        country = QLocale::Canada;
    }

    // american english
    else if ( "EN.US" == value )
    {
        valid = true;
        validQt = true;
    }

    // mexican spanish
    else if ( "ES.MX" == value )
    {
        newQtTranslator = new QTranslator;
        validQt = newQtTranslator->load( "qt_es",  QLibraryInfo::location(QLibraryInfo::TranslationsPath) );

        newTranslator = new QTranslator;
        valid = newTranslator->load( ":/languages/lang_es_mx" );
        lang = QLocale::Spanish;
        country = QLocale::Mexico;
    }

    // canadian french
    else if ( "FR.CA" == value )
    {
        newQtTranslator = new QTranslator;
        validQt = newQtTranslator->load( "qt_fr",  QLibraryInfo::location(QLibraryInfo::TranslationsPath) );

        newTranslator = new QTranslator;
        valid = newTranslator->load( ":/languages/lang_fr_ca" );
        lang = QLocale::French;
        country = QLocale::Canada;
    }

    // install the new translator
    if ( valid )
    {
        LOG_DEBUG( "installing...", NULL );

         MadpInternalConfig *intConfigData = &session_.scaleConfig->internalConfig;

        // install!
        installTranslator( translator_ = newTranslator );

        // delete the old one
        if ( oldTranslator )
            delete oldTranslator;

        // set language
        QLocale::setDefault( QLocale( lang, country ) );
        language_ = value;
        // propigate language change
        if( !intConfigData->ssactive_lock )
        {
            if ( operateMode_ )
                operateMode_->translate();
            if ( supervisorMode_ )
                supervisorMode_->translate();
            if ( serviceMode_ )
                serviceMode_ ->translate();
        }


        if ( numberPad_ )
            numberPad_->setLocale( lang );
        if ( keyboard_ )
            keyboard_->setLocale( lang );

        // save language selection to file
        if ( isPrimarySessionType() )
        {
            QDir langFilePath( QDir::temp() );
            QString langFilename( langFilePath.absoluteFilePath( "lang" ) );

            // remove existing file
            if (( langFilename.length() ) && ( QFile::exists( langFilename ) ))
                QFile::remove( langFilename );

            // create new file
            QFile langFile( langFilename );

            if ( !langFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
                LOG_WARN( "could not save language to '%s'", qPrintable( langFilename ) );
            else
            {
                LOG_INFO( "language file is '%s'", qPrintable( langFilename ) );

                QTextStream os( &langFile );
                os << value;

                langFile.close();
            }
        }
        // update language
        sessionHooks::instance()->updateCurrentLanguage( value );

        // update weights and measures information
        if( !intConfigData->ssactive_lock )
            requestWeightsMeasuresInfo();
    }

    // invalid translator, cleanup memory
    else if ( newTranslator )
    {
        delete newTranslator;
    }


    // intall the new translator for QT itself
    if( validQt )
    {
        LOG_DEBUG( "installing new translator for QT...", NULL );

        // install!
        installQtTranslator( qtTranslator_ = newQtTranslator );

        // delete the old one
        if( oldQtTranslator )
            delete oldQtTranslator;
    }
    else if( newQtTranslator )
    {
        // invalid translator, cleanup memory
        delete newQtTranslator;
    }
}

void SessionMainWindow::popFileDialog( int caller )
{
    dialogCaller_ = caller;
    fileerror_ = "";

    QString startDirStr = "";
    QString formatStr( "CSV (*.csv);;*.* (*)" );

    FileDialog filepicker( this );
    filepicker.setFilter( formatStr );
    filepicker.setOptions( QFileDialog::DontConfirmOverwrite );

    QStringList folders;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb" );
        folders.push_back( "/media/" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    filepicker.setAcceptMode( QFileDialog::AcceptOpen );
    filepicker.setDirectory( startDirStr );
    filepicker.setWindowTitle( tr( "Save file" ) );

    if ( QDialog::Accepted == filepicker.exec() )
    {
        qApp->processEvents();

        QStringList selected( filepicker.selectedFiles() );
        QString fileName;

        if ( 1 == selected.count() )
            fileName = selected[0];

        bool hasGoodFileName = checkFileName( fileName, 1 );  // the 2nd arg, 1 = file, 2 = folder, here we are checking a file

        if ( hasGoodFileName )
        {
#if defined( Q_OS_WIN )
            fileName.replace( "/", "\\\\" );
#endif
            QString args = fileName;

            session_.send( Weigher, WgSaveAuditTrailToDisc, args );
        }
    }

    if( W_A_M == dialogCaller_ )
    {
        // Show the Audit Trail Log for the weigher
        bool isBlocking = true;
        auditlogdialog *dialog = new auditlogdialog( this, isBlocking, fileerror_ );
        dialog->open();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::checkFileName(QString &myfile, int fileOrFolder)
{
    // fileOrFolder = 1 if checking a file, fileOrFolder = 2 if checking a folder

    bool validFile = false;
    QString errStr = "";

    if(myfile == "")
        errStr = tr( "A file was not selected. Please select a valid file to continue." );                                       // This is the string part of the error message,
    else
    {
        int filelen = myfile.length();

        if(filelen == 0)
            errStr = tr( "A file was not selected. Please select a valid file to continue." );
        else if(myfile.at(filelen - 1) == ('/'))
            errStr = tr( "The filename ends in a slash [/].  Please enter a valid file name." );
        else
        {
            if(fileOrFolder == 1)
                myfile = getFileName(myfile);				//checking a file
            else if(fileOrFolder == 2)
                myfile = myfile;							// checking a folder

            if(filelen <= 0)
                errStr = tr( "A valid file name has not been given.  The file name has no length." );
            else
            {
                if(fileOrFolder == 1 && myfile.indexOf("\\") > 0)
                    errStr = tr( "The \\ [slash] character is not allowed in the filename." );
                else if(myfile.indexOf(";") > 0)
                    errStr = tr( "The ; [semi-colon] character is not allowed in the filename." );
                else if(myfile.indexOf("*") > 0)
                    errStr = tr( "The * [asterisk] character is not allowed in the filename." );
                else if(myfile.indexOf("?") > 0)
                    errStr = tr( "The ? [question mark] character is not allowed in the filename." );
                else if((myfile.indexOf("\"") > 0) || (myfile.indexOf("\'") > 0))
                    errStr = tr( "The \" or \' [single or double quotes] character is not allowed in the filename." );
                else if((myfile.indexOf("<") > 0) || (myfile.indexOf(">") > 0))
                    errStr = tr( "The < or > [qreater or less than] character is not allowed in the filename." );
                else if((myfile.indexOf("+") > 0) || (myfile.indexOf("-") > 0))
                    errStr = tr( "The + or - [plus or minus] character is not allowed in the filename." );
                else if(myfile.indexOf("|") > 0)
                    errStr = tr( "The | [separator bar] character is not allowed in the filename." );
                //else if( myfile.indexOf("C:") > 0 )     // cannot contain C:
                //    errStr = tr("The file cannot be saved to the root file system");
                //else if( isLinux() && myfile.indexOf("media") < 0 )  // has to contain media (a path to a thumb)
                //    errStr = tr("The file cannot be saved to the root file system");
                else
                    validFile = true;
            }
        }
    }

    if ( validFile )
        fileerror_ =  tr("File saved");
    else
        fileerror_ = errStr;

    return validFile;
}

// Need to figure out, what this is trying to figure out.
QString SessionMainWindow::getFileName(QString myfile)
{
    //var fileObj;
    QString strippedFileName = "";
    QString retFile = "";

    bool doesFileAlreadyExist = QFile::exists(myfile);

    if( doesFileAlreadyExist )
    {
        retFile = myfile;                                           // then file name already exists and we can return it as is
    }
    else                                                            // we have to create a new one then
    {
        if( myfile.indexOf(".csv") > 0 )
        {
            // we dont have a file, so we need to create one. Do not include the entire path,
            // but do include the entension if the extension is .csv
            strippedFileName = myfile;                              // then we are good to go
        }
        else if(myfile.indexOf(".") > 0)
        {
            // we dont have a file, so we need to create one. Do not include the entire path, just the file.
            // An extension was added to the file, but we don't recogonize it, but include it.
            int getDot = myfile.indexOf(".") + 1;
            if (myfile.length() == getDot + 3)
                strippedFileName = myfile;
        }
        else
        {
            // we don't have a file, so create one. Do not include the entire path of the file.
            // No extension was given for the file, so default to the file to an csv file ext.
            strippedFileName = myfile + ".csv";
        }

        retFile = strippedFileName;
    }
    return retFile;
}

void SessionMainWindow::readPluWhiteList()
{
    QString whiteListFileName = "remote_print/plu_whitelist.csv";
    QString whiteListFileLocation = filesystem::absoluteFilePath(whiteListFileName, folderType::Apps);
    // Windows testing
    //QString whiteListFileLocation = "C:/hobart/apps/remote_print/plu_whitelist.csv";

    QFile file(whiteListFileLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        line = line.trimmed();
        pluList.append(line);
    }

    file.close();
}

bool SessionMainWindow::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::setNumberPadArea( Qt::DockWidgetArea value )
{
    if (( Qt::LeftDockWidgetArea == value ) || ( Qt::RightDockWidgetArea == value ))
        onNumberPadMoved( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::isKeyboardVisible() const
{
    return keyboard_->isVisible();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int SessionMainWindow::devicesAttached( int type ) const
{
    switch ( type )
    {
        case HIDDevMouseAttached:
            return physicalMouseAttached_;
        case HIDDevKeyboardAttached:
             return physicalKeyboardAttached_;
        case HIDDevBarcodeScannerAttached:
            return physicalScannerAttached_;
        default:
            break;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::setOffline( bool value )
{
    offline_ = value;
    toggleBackgroundImage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::setMenuPanelOpen( bool value )
{
    LOG_CLASS_INFO("SessionMainWindow", "setMenuPanelOpen");
    overlay_->setOpen( value );

    // menu panel close
    if ( !value )
    {
//        onMenuPanelDismissed();
        --menuPanelCount_;
    }

    // when menu panel is closed it is possible the operator logged into a different mode... so
    // here we activate the current mode
    if (( !overlay_->open() ) && ( !menuPanelCount_ ))
    {
        ModeWidget *m( mode() );

        if ( m )
        {
            LOG_DEBUG( "activate mode attempt", NULL );
            m->activate();
            LOG_DEBUG( "activate mode complete", NULL );

            // when not in supervisor mode reset panel state
            if ( !isSupervisorMode() )
                supervisorMode_->resetMenuPanelState();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showMenuPanel( MenuPanelMode panelMode, int panelModeEx )
{
    // create the panel
    SessionMenuPanel *panel( NULL );

    switch ( overlay_->area() )
    {
    case Qt::LeftDockWidgetArea:
        panel = new SessionMenuPanel( LeftDialogLocation, this );
        break;
    case Qt::RightDockWidgetArea:
        panel = new SessionMenuPanel( RightDialogLocation, this );
        break;
    default:
        break;
    }

    // populate the panel
    if ( panel )
    {
        // ensure all panels are bottom aligned
        panel->setLayoutAlignment( Qt::AlignBottom );

        // setup panel location
        panel->setDialogAlignment( Qt::AlignBottom );
        panel->setDialogAlignmentOffset( overlay_->bottomMargin() - 10 ); // subtract off whitespace amount
        panel->setDialogOffset( 50 );

        // figure out what mode they want
        ModeWidget *currentMode( mode() );
        MainMenuPanel::SelectedButton selected( MainMenuPanel::NoButtonSelected );

        switch ( panelMode )
        {
        case CurrentMenuPanelMode:
            selected = MainMenuPanel::MenuButtonSelected;
            break;

        case OperateMenuPanelMode:
            currentMode = operateMode_;
            selected = MainMenuPanel::MenuButtonSelected;
            break;

        case SupervisorMenuPanelMode:
            currentMode = supervisorMode_;
            selected = MainMenuPanel::MenuButtonSelected;
            break;

        case ServiceMenuPanelMode:
            currentMode = serviceMode_;            
            selected = MainMenuPanel::MenuButtonSelected;
            break;

        case SearchMenuPanelMode:
            selected = MainMenuPanel::SearchButtonSelected;
            break;

        case LanguageMenuPanelMode:
            selected = MainMenuPanel::LanguageButtonSelected;
            break;

        case HelpMenuPanelMode:
            selected = MainMenuPanel::HelpButtonSelected;
            break;

        case ShutdownMenuPanelMode:
            selected = MainMenuPanel::ShutdownButtonSelected;
            break;

        default:
            break;
        }

        // add main menu panel
        panel->addWidget( new MainMenuPanel( this, panel, selected ) );

        // add mode panel
        if (( MainMenuPanel::MenuButtonSelected == selected ) && ( currentMode ))
        {
            PanelWidget *menuPanel;
            PanelWidget *menuPanelEx( NULL );

            // for supervisor menu use saved menu panel state (i.e. compact vs. normal)
            if (( supervisorMode_ == currentMode ) && ( !panelModeEx ))
                panelModeEx = supervisorMode_->menuPanelState();

            if ( (menuPanel = currentMode->createMenuPanel( panelModeEx, panel )) )
            {
                panel->addWidget( menuPanel );

                ModePanel *modePanel( qobject_cast<ModePanel*>( menuPanel ) );
                menuPanelEx = (modePanel ? modePanel->createExtendedPanel() : NULL );
            }

            if ( menuPanelEx )
            {
                panel->addWidget( menuPanelEx );
            }
        }

        // connect slot for when panel activates
        connect( panel, SIGNAL( active(bool) ), SLOT( setMenuPanelOpen(bool) ) );

        // menu panel open
        ++menuPanelCount_;

        // open the panel
        panel->open();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionMainWindow::RegisterSessionResult SessionMainWindow::registerSession()
{
    LOG_CLASS_INFO( "SessionMainWindow", "registerSession" );

    bool registered = false;

    // check filesystem connection
    if ( !filesystem::isConnected() )
    {
        LOG_ERROR( "filesystem is not connected!", NULL );
        return RegisterSessionResultNoFilesystem;
    }

    // register session
    registeredSignal_.lock();

    RegisterSessionResult rc = RegisterSessionResultTimeout;

    LOG_DEBUG( "registering session of type %d...", sessionType() );
    session_.send( SessionDev, SnRegister, QString::number( sessionType() ) );

    // Note:
    // It may take 5-10 seconds for backend to respond... so while waiting for registration lets
    // load the web browser into memory.
    QWebView *webView( new QWebView( NULL ) );
    delete webView;

    // wait for the registered signal
    if ( isDemoMode )
        registered = true;
    else
    {
        static const int lockWaitTime = 50;

        LOG_DEBUG( "waiting %d ms for response...", registerSessionTimeout_ );
        int loops = (registerSessionTimeout_ / lockWaitTime);

        do
        {
            // process pending events
            QApplication::sendPostedEvents();
            QApplication::flush();

            QApplication::processEvents();

            // do not exit application while login dialog
            if ( !remoteAccessLogin_.tryLock() )
                ++loops;
            else
                remoteAccessLogin_.unlock();

            // do not exit application while restoring a database
            if ( !restoringDatabase_.tryLock() )
                ++loops;
            else
                restoringDatabase_.unlock();

            // exit loop if session terminated
            if ( terminate_.tryLock() )
                terminate_.unlock();
            else
            {
                loops = 0;
                rc = RegisterSessionResultRejected;
                break;
            }

        } while (( !registeredSignal_.tryLock( lockWaitTime ) ) && ( --loops ));

        // did we time out?
        registered = (loops ? true : false);
    }

    // reset our lock
    registeredSignal_.unlock();

    // check registration
    if ( !registered )
        LOG_ERROR( "failed to register session!", NULL );

    return (registered ? RegisterSessionResultOkay : rc);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::navigateHome()
{
    MadpScaleConfig *scaleConfig = &session_.scaleConfig->scaleConfig;
    MadpInternalConfig *intConfigData = &session_.scaleConfig->internalConfig;

    bool allowWebBrowserScreen( true );
    bool allowMealBuilderScreen( true );
    bool allowMeatSpecificationScreen( true );
    bool result;

    if ( session()->scaleConfig->quietDialogs )
        session()->scaleConfig->quietDialogs = false;

    // when no mode, do nothing
    if ( !operateMode_ )
        return false;

    // HTs
    // this feature is disabled
    if ( !operateMode_->isCached<WebBrowserScreen>() )
        allowWebBrowserScreen = false;

    // check for meal builder
    if ( !operateMode_->isCached<MealBuilderScreen>() || internalConfigData_->ssactive_lock )
        allowMealBuilderScreen = false;

    // check for meal builder
    if ( !operateMode_->isCached<MeatSheetScreen>() || internalConfigData_->ssactive_lock )
        allowMeatSpecificationScreen = false;


    //
    // change screen
    //

    if( session_.uiUtilities->isWrapOnly() )
    {
        result = switchToModeScreen<OperateMode, WrapperSettingsScreen>();
    }
    else
    {
        // use browser/flashkeys screen (0 = left screen) as home screen
        if (( allowWebBrowserScreen ) && ( 0 == scaleConfig->startScreenHome().toInt() ))
        {
            result = switchToModeScreen<OperateMode, WebBrowserScreen>();

            if( !internalConfigData_->scaleBootComplete || result )
            {
                int leftScreen( session_.scaleConfig->scaleConfig.dcaStartScreenType() );

                if( hobart::dynamicContentWidget::FixedFlashkeys == leftScreen )
                {
                   session_.scaleConfig->internalConfig.isLeftPanelActivated = true;
                   session_.scaleConfig->toggledState = true;
                }
            }
        }

        // use meal builder screen (2 = build-a-meal) as home screen
        else if (( allowMealBuilderScreen ) && ( 2 == scaleConfig->startScreenHome().toInt() ))
            result = switchToModeScreen<OperateMode, MealBuilderScreen>();

        else
        {
            session_.scaleConfig->internalConfig.isOkToLookUpPlu = true;
            session_.scaleConfig->internalConfig.isOkDroppedPLU = 0;
            result = switchToModeScreen<OperateMode, PluMainScreen>();
        }

        if ( intConfigData->scaleBootComplete )
            unlockMainWindow( true );

        // load default flashkey groups
        if ( result )
        {
            MealBuilderScreen *mealBuilderScreen( NULL );
            PluMainScreen *pluMainScreen( NULL );
            WebBrowserScreen *webBrowserScreen( NULL );

            // meal builder screen
            if ( (mealBuilderScreen = operateMode_->isCached<MealBuilderScreen>()) && allowMealBuilderScreen )
                mealBuilderScreen->resetFlashkeys( true );

            // plu main screen
            if ( (pluMainScreen = operateMode_->isCached<PluMainScreen>()) && !internalConfigData_->ssactive_lock )
            {
                if (online_) pluMainScreen->reloadWebViewers();
                    pluMainScreen->resetFlashkeys( true );
            }

            // web browser screen
            if ( (webBrowserScreen = operateMode_->isCached<WebBrowserScreen>()) && !internalConfigData_->ssactive_lock )
                webBrowserScreen->resetFlashkeys( true );

            // I could do a check to see if the printer settings actually changed if need be

            if ( internalConfigData_->hasExternalPrinterSettingsChanged )
            {
                session_.send(Printer, PrReadAllProductTextPrintParams);       // Request Printer Data Settings (ask for Prompts?)
                session_.send(Printer, PrReadAllExternalPrinters);             // Request the names of all of the external printers
            }
        }

        // not in supervisor mode anymore so reset panel state
        supervisorMode_->resetMenuPanelState();

        //unlockMainWindow( true );                                         // true means the screen isEnabled again
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::activateSelfServiceMode()
{
    LOG_CLASS_INFO( "SessionMainWindow", "activateSelfServiceMode" );

    const DomNormalScaleOperation *normalScaleOperation( &session_.scaleConfig->normalScaleOperation );
    const MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );
    ScaleConfiguration *config( session_.scaleConfig );

    config->internalConfig.activate_selfservice = true;

    selfServiceHook *hook( qobject_cast<selfServiceHook*>( sessionHooks::instance()->findHook( "selfServiceHook" ) ) );


        // install hook
        if (( !hook ) && ( session_.isServiceCounter() ))
        {
#ifndef QT_DEBUG
            if ( PrimarySessionType == sessionType() )
#endif
            {
                // this hook installed in DEBUG builds or for primary sessions
                sessionHooks::instance()->installHook( hook = new selfServiceHook( &session_ ) );

                // FT-1363: make sure we return home when hook deactivates
                connect( hook, SIGNAL( activeChanged() ), SLOT( onSelfServiceHookActiveChanged() ) );
            }
        }

        if ( !hook )
        {
            config->internalConfig.activate_selfservice = false;
            return false;
        }
        else if ( hook->active() )
        {
            config->internalConfig.activate_selfservice = false;
            return true;
        }

    operateMode_->shutdownKeyboards();

    // make sure no active item
    OperateMode *mode( retrieveMode<OperateMode>() );
    PluMainScreen *pluMainScreen( mode ? mode->isCached<PluMainScreen>() : NULL );

    if ( pluMainScreen )
        pluMainScreen->setCurrentItemNumber( "0" );

    // FT-1182: wait for item to be removed...
    QTime waitTimeout( QTime::currentTime().addMSecs( 3000 ) );

    for ( ;; )
    {
        QString currentItemNumber( mode->currentProduct()->itemNumber() );

        // check item removed
        if (( currentItemNumber.isEmpty() ) || ( "0" == currentItemNumber ))
            break;

        // process events
        QApplication::processEvents();

        // item was not cleared!
        if ( waitTimeout <= QTime::currentTime() )
        {
            LOG_WARN( "item not cleared in timely manner!", NULL );
            config->internalConfig.activate_selfservice = false;
            return false;
        }
    }

    // make sure we are in operate mode - initially didnt see why do this, but do it so we lose the keyboard from operate.
    switchToModeScreen<OperateMode, WebBrowserScreen>();

    // make sure any help dialog is closed (to free video player)
    freeWebBrowser();

    // send home screen
    QString startScreenHome( const_cast<MadpScaleConfig*>( scaleConfig )->startScreenHome() );

    // send start screen
    sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "0");           // display the flashkeys(0) over lone apple/keypad(1)
    sessionHooks::instance()->updateFieldValue( sessionHooks::selfServicePrintDisplayingLabel, "0");    // init printing to 0
    sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceByCountPrompt, "Enter");       // default state for Enter Key
    sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceRestrictedDialog, "0");        // default state for error dialogs cant look up PLUs

    //sessionHooks::instance()->updateFieldValue( sessionHooks::selfServicePrintDisplayingLabel, "0");    // SAME CODE AS PREV LINE init clicks to 0

    // center screen maps to keyed entry
    if ( 1 == startScreenHome.toInt() )
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceStartScreen, "keyed" );
    else
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceStartScreen, "softkey" );

    // send number pad style
    // telephone style - 1,2,3 keys on top
    if ( 1 == normalScaleOperation->popupKeypadStyle() )
        sessionHooks::instance()->updateFieldValue( sessionHooks::numberPadStyle, "123" );
    // keyboard style - 7,8,9 keys on top
    else
        sessionHooks::instance()->updateFieldValue( sessionHooks::numberPadStyle, "789" );

    // pjr383 - leave as is - brought back orig
    if ( normalScaleOperation->displaySelfServeStartScreen() || ( SelfServicePrintOption::SelfServOption_StartOperLogin == normalScaleOperation->selfServeStartScreenState() ) )
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceDisableAttract, "0" );
    else
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceDisableAttract, "1" );

    // pjr383 - leave as is - brought back orig
    if ( normalScaleOperation->bestSellersOnSelfServeMainScreen() )
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceShowBestSellers, "1" );
    else
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceShowBestSellers, "0" );

    if( normalScaleOperation->selfServeDisplayTareOnRandomWeight() )
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceTareFlashkeys, "1" );
	else
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceTareFlashkeys, "0" );
			
    // pjr383 - leave as is - brought back orig
    // if( SelfServicePrintOption::SelfServOption_PrintToScreenAndBarcode == normalScaleOperation->selfServePrintScreenState() )
    // this is barcode label on screen
    if( normalScaleOperation->selfServeDisplayBarcodeLabel() )
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceDisplayBarcodeLabel, "1" );
    else
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceDisplayBarcodeLabel, "0" );

    if ( scaleConfig->selfServeWeightRequiredToSelectItem() )       // pjr383 - KEEP
    {
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractInstructionalScreen, "1" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractPlaylistScreen, "0" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractMessage, "" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractRequiresStableWeight, "1" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractSlideshowSpeed, "2" ); // SOF-4529 - reduced the delay
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractWhenZeroWeight, "1" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceDisableAttract, "0" );
    }
    else if( scaleConfig->selfServeEnablePlaylists() )              // pjr383 - KEEP
    {
		sessionHooks::instance()->updateFieldValue( sessionHooks::attractInstructionalScreen, "0" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractPlaylistScreen, "1" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractMessage, "" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractRequiresStableWeight, "1" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractSlideshowSpeed, "5" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractWhenZeroWeight, "1" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceDisableAttract, "0" );
    }
    // PJR383 - Need to add Operator Login Screen
    else
    {
        // Right now this includes the NONE and Green Go button options. Now Im adding OR IF Operator Login
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractInstructionalScreen, "0" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractPlaylistScreen, "0" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractMessage, "" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractRequiresStableWeight, "0" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractSlideshowSpeed, "5" );
        sessionHooks::instance()->updateFieldValue( sessionHooks::attractWhenZeroWeight, "0" );
    }

    // pjr383 - leave as is for now
    // Amazon Go - display the label on the  screen
    if ( normalScaleOperation->selfServeDisplayLabelOnScreenGO() )
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServicePrintBarcodeToScreen, "1" );
    else
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServicePrintBarcodeToScreen, "0" );

    // update language
    sessionHooks::instance()->updateCurrentLanguage( session_.scaleConfig->getFPPLanguage() );

    // SOF-5120
    if(sessionError_){
        rejectDialog(sessionError_, true);
    }

    // go!
    hook->enter();

    // update weights and measures information
    //	FT-1637: Moved after hook->enter() so we don't send a WgReqInquire if SS is not active.
    requestWeightsMeasuresInfo();

    hideNumberPad();

    config->internalConfig.activate_selfservice = false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showDocument( const QString& filename )
{
    handleWebBrowserCommand( ShowDocumentFromFile, filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showDocument( const QByteArray& pdfData )
{
    handleWebBrowserCommand( ShowDocumentFromMemory, QString(), pdfData );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showHelp( bool showServiceHelp )
{
    Q_UNUSED( showServiceHelp );
/*
    PROOF OF CONCEPT CODE

    // show service mode help
    if ( showServiceHelp )
    {
        QString manual( "AWS General and Mechanical.pdf" );
        QString fullPath( filesystem::absoluteFilePath( manual, folderType::Html ) );

        // check for existance of manual
        if ( QFile::exists( fullPath ) )
        {
            handleWebBrowserCommand( ShowDocumentFromFile, fullPath );
            return;
        }
    }

    PROOF OF CONCEPT CODE
*/
    // show operate mode help
    handleWebBrowserCommand( ShowHelp );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showVideo( const QString& filename, int delay, bool isHelp )
{
    if ( !delay )
        handleWebBrowserCommand( ShowVideo, filename );
    else
    {
        queuedVideo_ = filename;

        QTimer::singleShot( delay, this, SLOT(onShowVideoTimer()) );
    }

    if ( isHelp )
    {
        blockError_ = true;

        QTimer::singleShot( 12000, this, SLOT(onBlockErrorsExpired()) );
    }
}

void SessionMainWindow::onBlockErrorsExpired()
{
    blockError_ = false;
}

void SessionMainWindow::onShowVideoTimer()
{
    handleWebBrowserCommand( ShowVideo, queuedVideo_ );
    queuedVideo_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showWebpage( const QString& url )
{
      LOG_CLASS_INFO( "SessionMainWindow", "showWebpage" );
    LOG_WARN("showWebpage URL %s", url.toStdString().c_str());
    // HTs
    // validate feature enabled
    if ( session_.uiUtilities->isFeatureLicensed( SCALE_WEB_BROWSER ) )
        handleWebBrowserCommand( ShowWebpage, url );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::togglePickFive( bool value )
{
    // nothing to do

    if ( pick5Toggled_ == value )
        return;

    // set mode
    pick5Toggled_ = value;

    if( pick5Toggled_ )
    {
        QString p5Name = "background_pick5.png";
        QString bgimg( filesystem::absoluteFilePath( p5Name, folderType::Layouts ) );
        modifyPaletteBackground setBackground( bgimg );        // set palette

        setBackground( this );
    }
    else
    {
        toggleBackgroundImage();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::toggleCuttingTest( bool value )
{
    // nothing to do

    if ( cuttingTestActive_ == value )
        return;

    // set mode
    cuttingTestActive_ = value;

    // if the actual test itself is active - no dialog
    cuttingTestDialogActive_ = ( value ? false : true); // the dialog being active is opposite of test going on
    toggleBackgroundImage();

    session()->scaleConfig->cuttingTestActive       = cuttingTestActive_;
    session()->scaleConfig->cuttingTestDialogActive = cuttingTestDialogActive_;

    Q_EMIT cuttingTestChanged( value );                 // This ends up popping the final comments dialog
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::flipBackgroundImage()
{
    // TODO: when offline they can force background to online image
    bool showOffline = offline();

    // load appropriate background
    QPixmap background( ":/images/background.png" );

    // check for cutting test
    if ( cuttingTestActive_ )
    {
        // make the background solid yellow
        QPainter painter( &background );
        painter.fillRect( background.rect(), QBrush( hobart::colorPalette::yellow ) );
    }
    // check for offline... when offline use different background
    else if ( showOffline )
    {
        // make the background solid red
        QPainter painter( &background );
        painter.fillRect( background.rect(), QBrush( hobart::colorPalette::red ) );
    }

    // set palette
    modifyPaletteBackground setBackground( background );
    setBackground( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::resetCuttingTest( bool value )
{
    if ( cuttingTestActive_ == value )
        return;

    // set mode
    cuttingTestActive_ = value;

    // if the actual test itself is active - no dialog
    cuttingTestDialogActive_ = ( value ? false : true); // the dialog being active is opposite of test going on

    session()->scaleConfig->cuttingTestActive       = cuttingTestActive_;
    session()->scaleConfig->cuttingTestDialogActive = cuttingTestDialogActive_;

    toggleBackgroundImage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::turnOffCuttingTest()
{
    // turn everything off
    cuttingTestActive_ = false;
    cuttingTestDialogActive_ = false;

    session()->scaleConfig->cuttingTestActive       = cuttingTestActive_;
    session()->scaleConfig->cuttingTestDialogActive = cuttingTestDialogActive_;

    toggleBackgroundImage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::launchCuttingTest( int beginOrEndTest )
{
    if ( ( ( !isCuttingTestActive() ) && ( 0 < testNumber_ ) ) || ( OriginalScreen::CUT_TEST_COMMENT_SCREEN == beginOrEndTest ) )
    {
        if ( switchToScreen<CuttingTestInitialInformation>() )
        {  
            CuttingTestInitialInformation *screen( dynamic_cast<CuttingTestInitialInformation*>( mode()->screen() ) );

            if ( screen )
                screen->updateInformation( testNumber_, beginOrEndTest );

            if( beginOrEndTest == CutTestState::CutTestState_End )
            {
                // load appropriate background
                QPixmap background( ":/images/background.png" );

                // set palette
                modifyPaletteBackground setBackground( background );
                setBackground( this );
            }
            cuttingTestDialogActive_ = true;
            session()->scaleConfig->cuttingTestDialogActive = true;
            session()->scaleConfig->operatorLoggedInForCutTest = false;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::launchCuttingTestHistory()
{
    if ( switchToScreen<CuttingTestHistory>() )
    {
        CuttingTestHistory *screen( dynamic_cast<CuttingTestHistory*>( mode()->screen() ) );

        // set report data
        if ( screen )
        {
            session()->scaleConfig->cuttingTestHistoryActive = true;
            screen->updateReportData( session()->scaleConfig->cuttingTestHistoryTotals );
        }
    }

    cuttingTestDialogActive_ = false;
    session()->scaleConfig->cuttingTestDialogActive = false;
    session()->scaleConfig->opLoggedInForCutTestHistory = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::launchCuttingTestDetails()
{
    if ( switchToScreen<CuttingTestDetails>() )
    {
        CuttingTestDetails *screen( dynamic_cast<CuttingTestDetails*>( mode()->screen() ) );
        int returnLocation = session_.scaleConfig->returnHomeLocation;

        // set report data
        if ( screen )
        {
            screen->setReturnHomeLocation( returnLocation );
            screen->updateReportData( session()->scaleConfig->cuttingTestTotals );
        }
    }    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::loadLastCuttingTestReport()
{
    if ( switchToScreen<CuttingTestReport>() )
    {
        CuttingTestReport *screen( qobject_cast<CuttingTestReport*>( mode()->screen() ) );

        // set report data
        if ( screen )
            screen->setReportData( session_.scaleConfig->cuttingTestTotals );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::startCutTestIdleTimer()
{
    const MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );
    int cuttestInterval = scaleConfig->cuttestInactivitylogout() * 60 * 1000;          // minutes * milliseconds

    if( 0 == cuttestInterval )
        return;

    if( cuttestInterval > 0 )       // Dont start up a 0 second CPU sucking timer
    {
        cuttestInactivityLogout_->setInterval( cuttestInterval );
        cuttestInactivityLogout_->start();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::resetCutTestIdleTimer()
{
    if ( isCuttingTestActive() || cuttingTestDialogActive_ )
    {
        if( cuttestInactivityLogout_ && cuttestInactivityLogout_->isActive() )
        {
            const MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );
            cuttestInactivityLogout_->stop();
            int cuttestInterval = scaleConfig->cuttestInactivitylogout() * 60 * 1000;          // need milliseconds
            if( cuttestInterval > 0 )       // Dont start up a 0 second CPU sucking timer
            {
                cuttestInactivityLogout_->setInterval( cuttestInterval );
                cuttestInactivityLogout_->start();
            }
        }
    }
}

bool SessionMainWindow::checkForSystemTestKey()
{
    return systemTestKeyWasInserted;
}

bool SessionMainWindow::getIsHeadingToHome() const
{
    return headingToHome_;
}

void SessionMainWindow::setIsHeadingToHome( bool value )
{
    headingToHome_ = value;
}

/*
int SessionMainWindow::getPrimaryPlatePosition() const
{
    return primaryPlatePosition_;
}

int SessionMainWindow::getSecondaryPlatePosition() const
{
    return secondaryPlatePosition_;
}

int SessionMainWindow::getRequiredPrimaryPlatePosition() const
{
    return requiredPrimaryPlatePosition_;
}

int SessionMainWindow::getRequiredSecondaryPlatePosition() const
{
    return requiredSecondaryPlatePosition_;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::resetSelfServicePlaylist()
{
    // update playlist
    const MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );
    const QString playlist( scaleConfig->selfServePlaylistFile() );
    sessionHooks::instance()->updateCurrentPlaylist( playlist );
}

// HT-361
///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::checkForAutomaticBackup( QString& autoBackupValue )
{
    bool rval = true;

    if( autoBackupValue.toInt() == 0 )
    {
        MessageBoxDialog *dialog( new MessageBoxDialog( this ) );

        dialog->setText( tr( "Automatic database backup has not been turned on." ) );
        dialog->setStandardButtons( QDialogButtonBox::Ok );

        dialog->open();

        rval = false;
    }

    return rval;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Need to determine if this is a bug.  This entire method is commented out
// TODO: when we merge clear tray to 2P last week.  But in 4.2.0-5.2.0 its live.
// TODO: determine if we need this or not. I'm guessing yes. 

void SessionMainWindow::manageCursor()
{
    LOG_CLASS_INFO( "SessionMainWindow", "manageCursor" );
    LOG_DEBUG("Managing cursor...", NULL);

    //SOF-5068 show cursor whenever we have a device attached or USB storage device
    if ( ( physicalMouseAttached_ > 0 ) || ( physicalKeyboardAttached_ > 0 ) || ( usbDeviceAttached_ > 0 )  || showCursorForBoardTester_ || cursorShownForRdpSession_)
    {
        QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
        LOG_DEBUG("Turning cursor on", NULL);

    }
    else
    {
        QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
        LOG_DEBUG("Turning cursor off", NULL);
    }
}


/* MERGED 2nd label applier to 4.2.0, 5.2.0, 4.2.100, and 5.2.100, they no longer use turnOnCursor() */

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::minimizeHelpDialog()
{
    handleWebBrowserCommand( Minimize );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::restoreHelpDialog()
{
    QTimer *timer( new QTimer( this ) );
    timer->setSingleShot( true );
    timer->start( 0 );

    connect( timer, SIGNAL(timeout()), SLOT(onRestoreHelpDialog()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::dismissNowServingOverlay( int rc )
{
    LOG_CLASS_INFO( "SessionMainWindow", "dismissNowServingOverlay" );

    LOG_INFO( "dismiss rc %d", rc );

    // FT-1488: dismiss GCD attract overlay
    if ( !session_.isPrepack() )
        session_.send( SessionDev, SnNowServingQueueManagement, QString::number( NowServingQueueManagement::DismissCustomerAttractDialog ) );

    // CANCEL CUSTOMER
    if ( QDialogButtonBox::ActionRole == rc )
    {
        // cancel serving this customer
        session_.send( SessionDev, SnNowServingQueueManagement, QString::number( NowServingQueueManagement::CancelServeCustomer ) + ",," + QString::number( nowServingCustomer_ ) );
    }

    // SKIP CUSTOMER (walk away)
    else if ( QDialogButtonBox::ResetRole == rc )
    {
        // serve next customer
        if(!sentSkipCustomer)
        {
            session_.send( SessionDev, SnNowServingQueueManagement, QString::number( NowServingQueueManagement::ServingNextCustomer ) );
            sentSkipCustomer = true;
        }
        else
        {
            sentSkipCustomer = false;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::hideInputWidgets()
{
    numberPad_->dismiss();
    keyboard_->dismiss();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showKeyboard()
{
    showKeyboard( KeyboardLocationSmart );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showKeyboard( KeyboardLocation loc )
{
    // WEH-107
    const MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );

    // nothing to do when physical keyboard attached
    if ( !scaleConfig->alwaysEnableVirtualKeyboard() )
        if (( physicalKeyboardAttached_ ) && ( !isDemoMode ))
            return;

    //-------------------------------------------------------------
    // SOF-3971: Do not show the keyboard if the Manufacturing Test
    //           key is in the scale
    //-------------------------------------------------------------
    if( isManfTestKeyPresent )
        return;  //

    // Note:
    // We need to check keyboard geometry even when keyboard is already visible. This is for when
    // operator is switching fields (i.e. top to bottom and vise versa).

    // HT-344
    QRect temp = calcKeyboardGeometry( loc );
    keyboard_->setFixedSize(temp.size());

    // resize keyboard
    keyboard_->setGeometry( temp );

    if ( keyboard_->isVisible() )
        return;

    // hide numberpad, show keyboard
    numberPad_->hide();
    keyboard_->show();


    // activate input widget
    if ( inputWidget_ )
        inputWidget_->activateWindow();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::hideKeyboard()
{
    if ( !keyboard_->isVisible() )
        return;

    keyboard_->dismiss();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showNumberPad()
{
	if( session()->isSelfServiceMode() || session()->scaleConfig->internalConfig.ssactive_lock )
        return;
        
    if ( numberPad_->isVisible() )
    {
        // FT-970
        // the visible property can become out of sync when multiple dialogs are being shown and
        // removed rapidly... so here we force a show() to ensure it is visible.
        numberPad_->show();

        return;
    }

    // resize number pad
    numberPad_->setGeometry( calcNumberPadGeometry() );

    // hide keyboard, show number pad
    keyboard_->hide();
    
    internalConfigData_->isLeftPanelActivated = false;
    numberPad_->show();

    // activate input widget
    if ( inputWidget_ )
        inputWidget_->activateWindow();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::hideNumberPad()
{
    if ( !numberPad_->isVisible() )
        return;

    numberPad_->dismiss();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::cancelActiveGesture()
{
    gestureRecognizer_->cancel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::swipeTheScreen(int direction)
{
    // swipe!!
    swipeScreen( direction );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::closeEvent( QCloseEvent *e )
{
#ifdef QT_DEBUG
    // debug mode allows termination at any time
#else
    // control when app closes in release mode
    if ( isPrimarySessionType() )
        if ( !allowTerminate_ )
        {
            e->ignore();
            return;
        }
#endif

    // close input widgets
    keyboard_->close();
    numberPad_->close();

    // make sure session is terminated
    session_.send( SessionDev, SnTerminate, "1" );

    _Mybase::closeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::event( QEvent *e )
{
    LOG_CLASS_INFO( "SessionMainWindow", "event" );

    // look for gesture event
    if ( QEvent::Gesture == e->type() )
    {
        QGestureEvent *gestureEvent( static_cast<QGestureEvent*>( e ) );
        QGesture *gesture( gestureEvent->gesture( Qt::SwipeGesture ) );

        LOG_DEBUG( "processing gesture...", NULL );

        // process swipe gesture
        if ( gesture )
        {
            QSwipeGesture *swipeGesture( static_cast<QSwipeGesture*>( gesture ) );

            if ( Qt::GestureFinished == swipeGesture->state() )
            {
                bool validDirection( true );

                // Note:
                // Squish refuses to record/playback swipe events... so in order to get around
                // this limitation I have added new logic for alt+arrow events. Basically we
                // handle the directional keys (plus alt modifier) for swipe events. This gesture
                // event, if valid, will be translated into a key event.
                int key;

                qreal angle( swipeGesture->swipeAngle() );

                // must be within 20 degrees of cardinal
                static const qreal theta = 20.0;

                // compute direction
                if (( (180.0-theta) <= angle ) && ( angle <= (180.0+theta) ))
                    key = Qt::Key_Left;
                else if (( (90.0-theta) <= angle ) && ( angle <= (90.0+theta) ))
                    key = Qt::Key_Up;
                else if (( (360.0-theta) <= angle ) || ( angle <= (theta) ))
                    key = Qt::Key_Right;
                else if (( (270.0-theta) <= angle ) && ( angle <= (270.0+theta) ))
                    key = Qt::Key_Down;
                else
                    validDirection = false;

                // handle the swipe
                if ( validDirection )
                {
                    // simulate key press/release
                    QApplication::postEvent( this, new QKeyEvent( QEvent::KeyPress, key, Qt::AltModifier ) );
                    QApplication::postEvent( this, new QKeyEvent( QEvent::KeyRelease, key, Qt::AltModifier ) );

                    e->setAccepted( true );
                    return true;
                }

            } // gesture finished
        } // swipe gesture
    } // gesture event

    return _Mybase::event( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::eventFilter( QObject *obj, QEvent *e )
{
    LOG_CLASS_INFO( "SessionMainWindow", "eventFilter" );

    // look for key press event
    // TO ADD ABILITY TO ALT+M to pop the mouse cursor
    
    // if ( QEvent::KeyPress == e->type() )                 //  ALWAYS CHECK THIS VERSION OF THE LINE IN
    // if ( QEvent::KeyPress == e->type() || (this == obj)) //  NEVER CHECK THIS VERSION OF THE LINE IN - but its what I need on Laptop

    // this is considered any activity so I must reset the inactive cut test timer

    // SOF-1551: Disable keystrokes until updates to Timings screen are finished
    if( internalConfigData_->timingsUpdatesInProgress ||
        internalConfigData_->sysInfoUpdatesInProgress ) // Also SOF-1915
    {
        if( QEvent::KeyPress == e->type() ||
            QEvent::KeyRelease == e->type() ||
            QEvent::MouseButtonRelease == e->type() ||
            QEvent::MouseButtonPress == e->type() ||
            QEvent::MouseButtonDblClick == e->type() )
        {
            if( internalConfigData_->timingsUpdatesInProgress )
                LOG_DEBUG( "eventFilter: timingsUpdatesInProgress is TRUE", NULL );

            if( internalConfigData_->sysInfoUpdatesInProgress )
                LOG_DEBUG( "eventFilter: sysInfoUpdatesInProgress is TRUE", NULL );

            LOG_DEBUG( "eventFilter: IGNORING WRAPPER KEYSTROKES", NULL );

            return true;
        }
    }

    if ( QEvent::KeyPress == e->type() )                    //  THIS IS THE LINE
    {
        QKeyEvent *keyEvent( static_cast<QKeyEvent*>( e ) );

        LOG_TRACE( "obj 0x%08x event 0x%08x type %d [%s/key press]", obj, e, e->type(), qPrintable( obj->objectName() ) );

        // this is considered any key activity so I must reset the inactive cut test timer
        resetCutTestIdleTimer();            // on a key press

        // ctrl+f
        // search button
        if (( Qt::ControlModifier == keyEvent->modifiers() ) && ( Qt::Key_F == keyEvent->key() ))
        {
            LOG_INFO( "search key was pressed", NULL );

            searchKeyPressed();
            return true;
        }

        // ctrl+p
        // print button
        else if (( Qt::ControlModifier == keyEvent->modifiers() ) && ( Qt::Key_P == keyEvent->key() ))
        {
            LOG_INFO( "print key was pressed", NULL );

            printKeyPressed();
            return true;
        }
		
#ifdef QT_DEBUG

        //
        // Development keyboard hooks
        //

        // alt+m
        // toggle mouse pointer on/off
        else if (( Qt::AltModifier == keyEvent->modifiers() ) && ( Qt::Key_M == keyEvent->key() ))
        {
            LOG_INFO( "toggle mouse!", NULL );

            if ( physicalMouseAttached_ )
            {
                physicalMouseAttached_ = 0;
                QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
            }
            else
            {
                physicalMouseAttached_ = 1;
                QApplication::restoreOverrideCursor();
            }
        }

        // alt+k
        // toggle virtual keyboard on/off
        else if (( Qt::AltModifier == keyEvent->modifiers() ) && ( Qt::Key_K == keyEvent->key() ))
        {
            LOG_INFO( "toggle keyboard!", NULL );

            if ( physicalKeyboardAttached_ )
                physicalKeyboardAttached_ = 0;
            else
                physicalKeyboardAttached_ = 1;
        }

        // alt+l
        // lock operate mode controls
        else if (( Qt::AltModifier == keyEvent->modifiers() ) && ( Qt::Key_L == keyEvent->key() ))
        {
            LOG_INFO( "locking operate mode...", NULL );
            operateMode_->setWidgetDragLock( true );
            operateMode_->setWidgetSettingsLock( true );
        }

        // alt+u
        // unlock operate mode controls
        else if (( Qt::AltModifier == keyEvent->modifiers() ) && ( Qt::Key_U == keyEvent->key() ))
        {
            LOG_INFO( "unlocking operate mode...", NULL );
            operateMode_->setWidgetDragLock( false );
            operateMode_->setWidgetSettingsLock( false );
        }

        // alt+w
        // weigher switch
        else if (( Qt::AltModifier == keyEvent->modifiers() ) && ( Qt::Key_W == keyEvent->key() ))
        {
            LOG_INFO( "weigher switch...", NULL );

            DomWeigherSwitch m;
            m[0] = QString::number( Weigher );
            m[1] = QString::number( WgConfigurationEnd );
            m[2] = QString();

            session_.handleMessage( m );
        }

        // alt+x
        // print multiple labels softkey
        else if (( Qt::AltModifier == keyEvent->modifiers() ) && ( Qt::Key_X == keyEvent->key() ))
        {
            LOG_INFO( "printing multiple labels...", NULL );
            session_.send( ProductDev, 48 );
        }
#endif
    }


    else if( QEvent::MouseButtonRelease == e->type() )
    {
        // this is considered any screen activity so I must reset the inactive cut test timer
        // could of done this on mousepress and mousedoublepress, but i would think both have a release.
        resetCutTestIdleTimer();            // on a key press
    }

    // look for the key release event
    else if (( QEvent::KeyRelease == e->type() ) && ( this == obj ))
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>( e );

        LOG_TRACE( "obj 0x%08x event 0x%08x type %d [%s/key release]", obj, e, e->type(), qPrintable( obj->objectName() ) );

        // enter
        // enter/return button
        if (( Qt::Key_Return == keyEvent->key() ) || (  Qt::Key_Enter == keyEvent->key() ))
        {
            // for enter key in operate mode we need to restore numberpad
            if (( isOperateMode() ) && ( !isDialogOpen() ))
            {
                // when forced fields discard event (screen will set focus to appropriate
                // widget based on current/next forced field)
                if ( operateMode_->isForcedFields() )
                {
                    LOG_DEBUG( "ignoring focus due to forced fields", NULL );
                    return true;
                }

                // restore numberpad
                operateMode_->setFocusForPopupNumberPad();
            }
        }
    }

    // update paint flags
    painting_ = (QEvent::Paint == e->type());

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::keyPressEvent( QKeyEvent *e )
{
    LOG_CLASS_INFO( "SessionMainWindow", "keyPressEvent" );

    // alt+arrow
    // swipe
    if (( Qt::AltModifier == e->modifiers() ) &&
        (( Qt::Key_Down == e->key() ) ||
         ( Qt::Key_Left == e->key() ) ||
         ( Qt::Key_Right == e->key() ) ||
         ( Qt::Key_Up == e->key() )))
    {
        ScreenWidget::SwipeDirection d;

        if ( Qt::Key_Down == e->key() )
        {
            LOG_INFO( "swiping down...", NULL );
            d = ScreenWidget::Down;
        }
        else if ( Qt::Key_Left == e->key() )
        {
            LOG_INFO( "swiping left...", NULL );
            d = ScreenWidget::Left;
        }
        else if ( Qt::Key_Right == e->key() )
        {
            LOG_INFO( "swiping right...", NULL );
            d = ScreenWidget::Right;
        }
        else if ( Qt::Key_Up == e->key() )
        {
            LOG_INFO( "swiping up...", NULL );
            d = ScreenWidget::Up;
        }

        // swipe!!
        swipeScreen( d );

        e->setAccepted( true );
        return;
    }

    _Mybase::keyPressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::moveEvent( QMoveEvent *e )
{
    keyboard_->setGeometry( calcKeyboardGeometry( KeyboardLocationUsePrevious ) );
    numberPad_->setGeometry( calcNumberPadGeometry() );

    _Mybase::moveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::resizeEvent( QResizeEvent *e )
{
    // update input widget geometry
    keyboard_->setGeometry( calcKeyboardGeometry( KeyboardLocationUsePrevious ) );
    numberPad_->setGeometry( calcNumberPadGeometry() );

    // refresh back
    toggleBackgroundImage();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onCheckMedia()
{
    bool mediaExists( false );

    if ( isPrimarySessionType() )
    {
#if defined( Q_OS_LINUX )

        QDir d( "/media/thumb" );

        if ( d.exists() ){
            mediaExists = true;

        }

#else
        // TODO

#endif
    }

    if (( overlay_ ) && ( mediaExists != overlay_->ejectMediaVisible() ))
    {
        overlay_->setEjectMediaVisible( mediaExists );

        if ( !painting_ )
            overlay_->repaint();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::ejectMedia()
{
    LOG_CLASS_INFO( "SessionMainWindow", "ejectMedia" );

#if defined( Q_OS_LINUX )

    QDir d( "/media/thumb" );

    if ( d.exists() )
    {
        int rc;

        LOG_DEBUG( "sync filesystem...", NULL );
        system( "sync" );

        LOG_DEBUG( "umount filesystem...", NULL );
        rc = system( "umount /media/thumb/" );

        LOG_TRACE( "umount filesystem... done! rc %d", rc );

        if ( 0 == rc )
        {
            MessageBoxDialog *dialog( new MessageBoxDialog( this ) );
            dialog->setBlockDialogs( true );
            dialog->setTitle( tr( "Device Manager" ) );
            dialog->setText(tr( "You may now safely remove the media from your device." ) );
            dialog->setStandardButtons( QDialogButtonBox::Ok );

            dialog->open();
        }
    }

#else
    // TODO

#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onFilesystemDownloadFileInProgress( bool active, const QString& filename )
{
    Q_UNUSED( filename );

    if ( overlay_ )
    {
        overlay_->setMessageVisible( active, tr( "Downloading file(s), please wait..." ) );

        if ( !painting_ )
            overlay_->repaint();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onFilesystemSyncInProgress( bool active )
{
    if ( overlay_ )
    {
        overlay_->setMessageVisible( active, tr( "Syncing, please wait..." ) );

        if ( !painting_ )
            overlay_->repaint();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onFocusChanged( QWidget *oldFocus, QWidget *newFocus )
{
    LOG_CLASS_INFO( "SessionMainWindow", "onFocusChanged" );

    LOG_TRACE( "focus changing from 0x%08x [%s] to 0x%08x [%s]",
        oldFocus, (oldFocus ? qPrintable( oldFocus->objectName() ) : ""),
        newFocus, (newFocus ? qPrintable( newFocus->objectName() ) : "") );

    inputWidget_ = NULL;

    // keep track of who has focus
    if (( !newFocus ) || ( !isAncestorOf( newFocus ) ))
        return;

    // numberpad only allowed in operate mode
    bool forceKeyboardInput( !isOperateMode() );

    // when dialog open only allow focus on dialog
    if ( isDialogOpen() )
    {
        // lock the mutex
        QMutexLocker lock( &const_cast<SessionMainWindow*>( this )->dialogListMutex_ );

        // grab head dialog
        if ( dialogList_.size() )
        {
            SessionDialog *dialog = dialogList_.front();

            // not ancestor of dialog!
            if ( !dialog->isAncestorOf( newFocus ) )
            {
                LOG_DEBUG( "widget not ancestor of open dialog", NULL );
                newFocus->clearFocus();
                return;
            }

            // FT-383: force keyboard for searching
            // FT-560: force keyboard for printer settings
            if (( qobject_cast<PrinterSettingsDialog*>( dialog ) ) ||
                ( qobject_cast<SearchDialog*>( dialog ) ) ||
                ( qobject_cast<CuttingTestDialog*>( dialog ) ))
            {
                forceKeyboardInput = true;
            }
        }
    }

    bool isKeyboardInput = false;
    bool isNumberPadInput = false;
    bool shouldHideInputWidgets = true;

    inputWidget_ = newFocus;

    QLineEdit *lineEdit;
    QPlainTextEdit *plainTextEdit;
    QTextEdit *textEdit;
    QComboBox *comboBox;
    QSpinBox *spinBox;
    QDoubleSpinBox *doubleSpinBox;
    QWebView *webView;

    // check for keyboard input
    if ( (lineEdit = qobject_cast<QLineEdit*>( newFocus )) )
    {
        const QValidator *v( lineEdit->validator() );

        if (( v ) && (( qobject_cast<const QIntValidator*>( v ) ) || ( qobject_cast<const QDoubleValidator*>( v ) )))
            isNumberPadInput = true;
        else
            isKeyboardInput = true;
    }
    else if ( (plainTextEdit = qobject_cast<QPlainTextEdit*>( newFocus )) )
        isKeyboardInput = true;
    else if ( (textEdit = qobject_cast<QTextEdit*>( newFocus )) )
        isKeyboardInput = true;
    else if ( (comboBox = qobject_cast<QComboBox*>( newFocus )) )
        isKeyboardInput = comboBox->isEditable();
    else if ( (webView = qobject_cast<QWebView*>( newFocus )) )
    {
        // do not hide input widgets as this removes focus from the web view
        shouldHideInputWidgets = false;

        // the hobart::webViewer is handling keyboard show/hide, if this changes uncomment this
        //isKeyboardInput = true;
    }

    // check for number pad input
    else if ( (spinBox = qobject_cast<QSpinBox*>( newFocus )) )
        isNumberPadInput = true;
    else if ( (doubleSpinBox = qobject_cast<QDoubleSpinBox*>( newFocus )) )
        isNumberPadInput = true;

    // keyboard input forced
    if ( forceKeyboardInput )
        isKeyboardInput |= isNumberPadInput;

    if ( isKeyboardInput )
        showKeyboard();
    else if ( isNumberPadInput )
        showNumberPad();
    else if ( shouldHideInputWidgets )
        hideInputWidgets();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onModeMarginsChanged()
{
    // move the number pad with new margins
    numberPad_->setGeometry( calcNumberPadGeometry() );

    // update margin of overlay
    overlay_->setBottomMargin( mapToGlobal( contentsRect().bottomRight() ).y() - numberPad_->geometry().bottom() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onNumberPadMoved( Qt::DockWidgetArea where )
{
    // save area
    numberPadArea_ = where;

    // move overlay to opposite side
    overlay_->setArea( Qt::LeftDockWidgetArea == where ? Qt::RightDockWidgetArea : Qt::LeftDockWidgetArea );

    // propigate signal
    Q_EMIT numberPadMoved( where );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onPrintKeyPressed()
{
    ModeWidget *m = mode();
    ScreenWidget *screen = (m ? m->screen() : NULL);
    PluMainScreen *pluMainScreen     = dynamic_cast<PluMainScreen*>( screen );
    if ( pluMainScreen )
    {
        if( pluMainScreen->checkExpirationDaysForced() )
           return;

        if( pluMainScreen->checkProductionEntryForced() )
            return;
    }

    if( internalConfigData_->scaleWentOutOfTilt )
    {
        session_.send( Error, ErrWeigherDevice, QString::number(WeigherDevError::WeigherErrValMaxTiltReached) + ",5" );
        return;
    }
    else if( internalConfigData_->selfservInvalidPlu )
    {
        session_.send( Error, ErrProductDevice, QString::number( ProductDevError::ProductErrLabelType ) + ",5" );

        //I don't think we need to return here. When we do, in the "display label on screen and print" config, when we hit "scan and print" it just pops the
        // dialog and does nothing.
        //return;
    }

    // TODO: this needs removed and fixed in the backend (page tracking is not applicable to this ui)
    // Do I need this at all?
    session_.send( SessionDev, SnWriteCurrentWebPage, "7,/operateRunScreenRandom.asp" );
    session_.send( SessionDev, SnSelectActiveWebPageFrame, "7" );

    // since we are printing we need to refresh the best seller list
    operateMode_->setRefreshBestSellers( true );

    // print!

    if( session_.uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
    {
        if( minmaxprintwidget_ && minmaxprintwidget_->isVisible() )
        {
            minmaxprintwidget_->printOnScreenPrintKey();
            return;
        }
    }

    session_.send( SessionDev, SnPrintKeyPressed );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onRequestWeightsMeasuresInfo()
{
    weightsandmeasuresdialog d( this, false );

    // send weights and measures information
    sessionHooks::instance()->updateFieldValue( sessionHooks::weightsAndMeasuresInfo, d.allDataFields() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onLogoutCutTestInactivity()
{
    // If we actually hit this timer, that means we have been inactive during a cut test.
    // In that case, suspend the current cut test and log out the current user

    CuttingTestInitialInformation *initscreen;
    cuttestInactivityLogout_->stop();
    
    // why start it back up if we are suspending and timing out the cut test?
    //int cuttestInterval = scaleConfig->cuttestInactivitylogout() * 60 * 1000;          // need milliseconds
    //if( cuttestInterval > 0 )       // Dont start up a 0 second CPU sucking timer
    //{
    //    cuttestInactivityLogout_->setInterval( cuttestInterval );
    //    cuttestInactivityLogout_->start();
    //}

    if( cuttingTestDialogActive_ )
    {
        // if we are on the initial dialog screen and timeout
        if ( (initscreen = dynamic_cast<CuttingTestInitialInformation*>( mode()->screen() )) )
            initscreen->inactivityLogout();
    }
    else
    {
        if ( !isCuttingTestActive() && !cuttingTestDialogActive_ )
            return;             // if we are not in a cut test, leave

        //MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

        if ( CutTestOption::CutTestOption_Default == session_.scaleConfig->scaleConfig.cuttingTestType() )      // Default
            return;

        // SUSPEND CUT TEST FLASHKEY - Now save the current cut test data. Do it like the Suspend Flashkey
        session_.send( SoftkeyDev, ExecuteSoftkey, QString::number( Action_CutTestSuspend ) + "," + "" + "," + "999035" );

        turnOffCuttingTest();

        // reset the cut test plu and spec sheet data
        MadpCutTestPlu *cutTestPlu  = &session_.scaleConfig->cutTestPlu;
        cutTestPlu->pluNumber = 0;
        cutTestPlu->meatSpecSheet = 0;
        cutTestPlu->cutTestNumber = 0;

        ModeWidget *m = mode();
        ScreenWidget *screen = (m ? m->screen() : NULL);
        PluMainScreen *pluMainScreen     = dynamic_cast<PluMainScreen*>( screen );
        MeatSheetScreen *meatSheetScreen = dynamic_cast<MeatSheetScreen*>( screen );

        // Now log out the current cut test user out
        session_.send( OperatorDev, OpLogout );

        session_.scaleConfig->setIsOperatorLoggedIn( false );
        session_.scaleConfig->setLoggedOperatorId("");
        session_.scaleConfig->setLoggedOperatorName("");
        session_.scaleConfig->setLoggedOperatorAccess(0);

        toggleCuttingTest( false );
        navigateHome();         // transition to the start a run screen

        if ( pluMainScreen )
        {
            // Reset the Main Screen Flashkeys to 999003
            //only works if we are on mainscreen, not meatspecsheet
            pluMainScreen->setSpecialFlasheyWidget( false );  // hide the cut test run screen
        }
        else if( meatSheetScreen )
        {
            // only works on meat spec sheet screen
            meatSheetScreen->clearFlashkeys();
        }

        // crashed HARD. Had to kill QT with taskmanager
        // pluMainScreen->resetFlashkeys( true );

        // Didnt work if on meat spec sheet and resetting the mainrunscreen flashkeys
        session_.send( ProductDev, ProductRead, "0" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onRestoreHelpDialog()
{
    handleWebBrowserCommand( Restore );

    // mark timer for deletion
    QTimer *timer( qobject_cast<QTimer*>( sender() ) );

    if ( timer )
        timer->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onSelfServiceHookActiveChanged()
{
    selfServiceHook *hook( qobject_cast<selfServiceHook*>( sessionHooks::instance()->findHook( "selfServiceHook" ) ) );

    if ( !hook )
        return;

    // when ss hook deactivates, browse to home screen
    if ( !hook->active() )
        navigateHome();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onSendCharacter( int key, Qt::KeyboardModifiers modifiers, const QString& text )
{
    QWidget *widget;

    if ( !(widget = inputWidget_) )
    {
        // FT-537: print key without input widget... redirect to this
        if ( Qt::ControlModifier != modifiers )
            return;

        widget = this;
    }

    // simulate key press
    QKeyEvent *keyPress = new QKeyEvent( QEvent::KeyPress, key, modifiers, text );
    QApplication::postEvent( widget, keyPress );

    // simulate key release
    QKeyEvent *keyRelease = new QKeyEvent( QEvent::KeyRelease, key, modifiers );
    QApplication::postEvent( widget, keyRelease );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onSendTestMessage()
{
    // marker for init complete
    session_.send( SessionDev, SnTestMessage );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onSessionDisconnected()
{
    LOG_CLASS_INFO( "SessionMainWindow", "onSessionDisconnected" );

    LOG_ERROR( "session has disconnected!", NULL );

    // when not visible there is nothing to do
    if ( !isVisible() )
        return;

    QString title( tr( "Restart Required" ) );

    QString text( tr(
        "The Hobart Service process has exited unexpectedly.\n"
        "\n"
        "We're going to collect some error info, and then we'll restart for you." ) );

    // for secondary session give different message
    if ( PrimarySessionType != sessionType() )
    {
        title = tr( "Disconnected" );

        text = tr(
            "You have been disconnected from your session.\n"
            "\n"
            "Press OK to close this window." );
    }

    MessageBoxDialog dialog( real_time_priority, this );

    dialog.setTitle( title );
    dialog.setText( text );
    dialog.setStandardButtons( QDialogButtonBox::Ok );

    LOG_DEBUG( "popping message box", NULL );
    dialog.exec();

#if defined( Q_OS_WIN )

    QString now( QDateTime::currentDateTime().toString( "yyyyMMdd_hhmmsszzz" ) );
    QString filename( QString( "logs_%1.zip" ).arg( now ) );
    QString cmd;

    QString rebootCmd( "shutdown /r" );

    // make list of logs to grab
    QStringList filters;
    filters << "*.txt" << "*.log" << "*.old";

    QDir pwd( QDir::current() );
    pwd.setFilter( QDir::Files );
    pwd.setNameFilters( filters );

    QDir tempDir( QDir::temp() );
    tempDir.setFilter( QDir::Files );
    tempDir.setNameFilters( filters );

    QStringList filesToZip;
    QStringList temp;

    temp = pwd.entryList();
    foreach ( const QString& f, temp )
        filesToZip.push_back( pwd.absoluteFilePath( f ) );

    temp = tempDir.entryList();
    foreach ( const QString& f, temp )
        filesToZip.push_back( tempDir.absoluteFilePath( f ) );

    // in windows no shell command to perform a zip... so invoke utility class...
    hobart::shellZip zip( pwd.absoluteFilePath( filename ) );
    zip.add( filesToZip );

#elif defined( Q_OS_LINUX )

    QString now( QDateTime::currentDateTime().toString( "yyyyMMdd_hhmmsszzz" ) );
    QString filename( QString( "logs_%1.tar.gz" ).arg( now ) );
    QString cmd( "tar -czvf " + filename + " *.txt *.log *.old /tmp/*.log /var/log/*.log /var/log/*.txt" );

    QString rebootCmd( "reboot" );

#endif

    // zip some logs
    if ( cmd.length() )
    {
        process p;

        LOG_DEBUG( "collecting logs...", NULL );

        if ( !p.start( cmd ))
            LOG_WARN( "could not launch log collector", NULL );
        else if ( !p.waitForFinished( 30000 ) )
            LOG_WARN( "log collector did not complete in a timely manner", NULL );
    }

    LOG_INFO( "closing application", NULL );
    allowTerminate_ = true;
    close();

#ifdef QT_DEBUG

    LOG_TRACE( "no reboot due to debug build", NULL );

#else

    // check if should reboot
    if ( PrimarySessionType != sessionType() )
        LOG_TRACE( "no reboot due to session type", NULL );

    // reboot
    else if ( rebootCmd.length() )
    {
        process p;

        LOG_DEBUG( "rebooting...", NULL );

        if ( !p.start( rebootCmd ))
            LOG_WARN( "could not perform reboot command", NULL );
        else if ( !p.waitForFinished( 30000 ) )
            LOG_WARN( "reboot did not complete in a timely manner", NULL );
    }

#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "SessionMainWindow", "onSessionMessageReceive" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // scale config
    if (( ConfigurationDev == device ) && ( CfgScaleWrite == event ))
    {
        MadpScaleConfig m( msg );

        //
        // INIT PHASE 0
        //
        if ( initializePhase_ <= 0 )
        {
            LOG_INFO( "creating modes...", NULL );

            // create modes
            operateMode_ = new OperateMode( this );
            supervisorMode_ = new SupervisorMode( this );
            serviceMode_ = new ServiceMode( this );

            operateMode_->createCachedScreens();
            supervisorMode_->createCachedScreens();
            serviceMode_->createCachedScreens();

            centralWidget_->addWidget( operateMode_ );
            centralWidget_->addWidget( supervisorMode_ );
            centralWidget_->addWidget( serviceMode_ );

            // set default mode to operate mode
            centralWidget_->setCurrentWidget( operateMode_ );
            connect( operateMode_, SIGNAL( marginsChanged() ), SLOT( onModeMarginsChanged() ) );

            // connect slots
            connect( operateMode_, SIGNAL( screenChanged() ), SLOT( cancelActiveGesture() ) );
            connect( supervisorMode_, SIGNAL( screenChanged() ), SLOT( cancelActiveGesture() ) );
            connect( serviceMode_, SIGNAL( screenChanged() ), SLOT( cancelActiveGesture() ) );

            ++initializePhase_;
        }

        LOG_TRACE( "checking language...", NULL );

        // update hooks with language options
        sessionHooks::instance()->updateFieldValue( sessionHooks::primaryLanguage, m.languageCode().toUpper() + "." + m.countryCode().toUpper() );
        sessionHooks::instance()->updateFieldValue( sessionHooks::secondaryLanguage, m.secLanguageCode().toUpper() + "." + m.secCountryCode().toUpper() );

        // update hooks with auto lookup options
        sessionHooks::instance()->updateFieldValue( sessionHooks::autoLookupNumOfDigits, m.autoLookupNumOfDigits() );
        sessionHooks::instance()->updateFieldValue( sessionHooks::autoLookupOverrideDigit, m.autoLookupOverrideDigit() );
        sessionHooks::instance()->updateFieldValue( sessionHooks::autoLookupProduct, m.autoLookupProduct() );

        // Just in case we got this Configuration message, and we have new timer values to set if they changed
        resetCutTestIdleTimer();
    }
    // normal scale operation
    else if (( ConfigurationDev == device ) && ( CfgScaleOperationWrite == event ))
    {
        DomNormalScaleOperation m( msg );

        // set keypad style
        // 1 = Telephone - 1,2,3 keys on top
        // 2 = PC Keyboard - 7,8,9 keys on top
        keyboard_->setKeyLayoutStyle( (1 == m.popupKeypadStyle()) ? KeyboardWidget::TelephoneKeyLayoutStyle : KeyboardWidget::CalculatorKeyLayoutStyle );
        numberPad_->setKeyLayoutStyle( (1 == m.popupKeypadStyle()) ? NumberPadWidget::TelephoneKeyLayoutStyle : NumberPadWidget::CalculatorKeyLayoutStyle );
        numberPad_->setBackKeyLayoutStyle( (1 == m.enableSearchOnKeypad()) ? NumberPadWidget::smallBackKeyLayoutStyle : NumberPadWidget::largeBackKeyLayoutStyle);

        if( m.selfServeEnablePlaylists() )          // pjr383 - KEEP
            resetSelfServicePlaylist();
    }

    // network connectivity
    else if (( NetworkConnectivityDev == device ) && ( CurrentStatus == event ))
    {
        MadpNetworkConnectivity m( msg );

        bool isNetworkConnected = m.isNetworkConnected();

        if ( offline() == isNetworkConnected )
        {
            offline_ = !isNetworkConnected;
            online_ = isNetworkConnected;
            LOG_INFO( "network is %s", (offline_ ? "OFFLINE" : "online") );

            toggleBackgroundImage();
        }
    }

    // register session
    else if (( SessionDev == device ) && ( SnRegister == event ))
    {
        //
        // INIT PHASE 1
        //
        if ( initializePhase_ <= 1 )
        {
            LOG_INFO( "session has been registered!", NULL );

            // fix primary language not setting upon startup
            ScaleConfiguration *scaleConfig( session_.scaleConfig );
            scaleConfig->currentLanguage = 1;

            // send startup sequence
            session_.send( SessionDev, SnStopDialogs );

            // FT-1735 Unable to Access Self Service - start tracking with 100,10,13 msg
            session_.send( SessionDev, SnTouchTypeSetting );
            session_.send( SessionDev, SnReadDateTime );
            session_.send( ConfigurationDev, CfgOperatorAccessRead );
            session_.send( ConfigurationDev, CfgPrepackRead );
            if ( !session_.isPrepack() )
                session_.send( ConfigurationDev, CfgAccumulateItemsRead );
            session_.send( ConfigurationDev, CfgScaleOperationRead );
            session_.send( ConfigurationDev, CfgPrintedFieldsRead );
            session_.send( Error, ErrSessionDevice, "100018,5" );
            session_.send( Printer, PrReadAllExternalPrinters );            // Give me the printer names

            // request peripheral controller versions
            session_.send( PrinterSystem, PrSysReqStatus, "0" );
            session_.send( WeigherSystem, SysReqStatus );

            // request peripheral controller versions
            //session_.send( PrinterSystem, PrSysReqStatus, "0" );          // asked and answered right? up 4.
            //session_.send( WeigherSystem, SysReqStatus );                 // same thing as 4 lines up right?

            // request firmware versions too
            session_.send( Weigher, WgReqInquire );
            session_.send( MfgTestDev, RequestPrinterLabelCount, QString::number( 0 ) ); // 164,23
            session_.send( SessionDev, SnVersionInfo );

            // ACR-8
            // Switched from sleep to timer
            QTimer *sendTestMessage( new QTimer( this ) );

            connect( sendTestMessage, SIGNAL(timeout()), SLOT(onSendTestMessage()) );

            sendTestMessage->setSingleShot( true );
            sendTestMessage->start( 0 );

            ++initializePhase_;
        }
        // SOF-5067: For FSS scales, automatically enable the ability to print without a GCD
        session()->uiUtilities->checkAllowPrintingNoGcd();

    }

    // terminate session
    else if (( SessionDev == device ) && ( SnTerminate == event ))
    {
        LOG_WARN( "terminating session!", NULL );
        allowTerminate_ = true;
        close();

        terminate_.tryLock();
    }

    // now serving queue management
    else if (( SessionDev == device ) && ( SnNowServingQueueManagement == event ))
    {
        NowServingQueueManagement m( msg );

        QString message;
        bool connectFinishedSignal( false );
        bool increaseFontPointSize( false );

        bool showCustomerButtons( false );

        // success
        if ( 0 == m.resultCode() )
        {
            if (( NowServingQueueManagement::ServingNextCustomer == m.action() ) ||
                ( NowServingQueueManagement::ServeSpecificCustomer == m.action() ) ||
                ( NowServingQueueManagement::IncrementCustomerNumber == m.action() ))
            {
                MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );

                QString customer( m.value1() );
                QString numberWaiting( m.value2() );

                if ( customer.length() )
                {
                    message = tr( "Serving Customer:" ) + "\n" + customer + "\n";

                    nowServingCustomer_ = customer.toInt();
                    showCustomerButtons = true;
                }

                // only show queue size when electronic system
                if (( 1 == scaleConfig->nowServingSysType() ) && ( numberWaiting.length() ))
                {
                    message += "\n";
                    message += tr( "Number Waiting:" ) + "\n" + numberWaiting + "\n";
                }

                connectFinishedSignal = true;
                increaseFontPointSize = true;
            }
        }

        // error
        else
        {
            message = tr( "Error:" ) + " " + QString::number( m.resultCode() ) + "\n\n";

            switch ( m.resultCode() )
            {
            case -20:
                message += tr( "Server responded, but the response was invalid." );
                break;
            case -12:
                message += tr( "An error occured while processing the request." );
                break;
            case -11:
                message += tr( "No server configured. Please configure a server in the supervisor menu." );
                break;
            case -10:
                message += tr( "Invalid configuration." );
                break;
            case -2:
                message += tr( "This action can only be performed from a scale." );
                break;
            case -1:
                message = tr( "This feature is not licensed. Please install a license." );
                break;
            case 1:
                message += tr( "Server responded, but the request was invalid." );
                break;
            case 2:
                message = tr( "Server is not licensed. Install a license on the server." );
                break;
            case 3:
                message = tr( "Queue is not setup properly. Please check your configuration in supervisor menu." );
                break;
            case 4:
                message = tr( "There are no customers queued at this time." );
                break;
            case 5:
                message += tr( "Server is not setup properly, please install a ticket template." );
                break;
            case 6:
                message += tr( "No printer attached to server." );
                break;
            case 7:
                message += tr( "Server unable to use the number you entered." );
                break;
            case 8:
                message += tr( "No printer attached to server." );
                break;
            default:
                message += tr( "An unknown error has occured." );
                break;
            }
        }

        // show message
        if ( !message.isEmpty() )
        {
            rejectDialogs();

            MessageBoxDialog *dialog( new MessageBoxDialog( this ) );
            dialog->setBlockDialogs( true );
            dialog->setTitle( tr( "Now Serving Queue Management" ) );
            dialog->setText( message );

            if ( !showCustomerButtons )
                dialog->setStandardButtons( QDialogButtonBox::Ok );
            else
            {
                modifyPaletteForeground setPaletteForeground(
                    modifyPaletteForeground::createPalette(
                        hobart::colorPalette::red,
                        hobart::colorPalette::red ) );

                dialog->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Ignore );

                QAbstractButton *okayButton( dialog->button( QDialogButtonBox::Ok ) );

                QAbstractButton *cancelButton( dialog->button( QDialogButtonBox::Cancel ) );
                setPaletteForeground( cancelButton );

                QAbstractButton *ignoreButton( dialog->button( QDialogButtonBox::Ignore ) );
                ignoreButton->setText( tr( "Skip\nCustomer" ) );

                dialog->setButtonRole( ignoreButton, QDialogButtonBox::ResetRole );
                dialog->setButtonRole( cancelButton, QDialogButtonBox::ActionRole );
                dialog->setButtonRole( okayButton, QDialogButtonBox::AcceptRole );

                dialog->setAllowOverlayClickDismiss( false );
            }

            if ( connectFinishedSignal )
                connect( dialog, SIGNAL( finished(int) ), SLOT( dismissNowServingOverlay(int) ) );

            if ( increaseFontPointSize )
            {
                int textFontPointSize( dialog->textFontPointSize() );

                dialog->setTextFontPointSize( (textFontPointSize*15)/10 );
            }

            dialog->open();
        }
    }

    // pjr383 - turns out I dont need this here I dont think. Use the one over in plumainscreen.cpp where WriteSoftkey == event
    // toggle flashkey depressed
    //else if( ( SoftkeyDev == device ) && ( WriteSoftkey == event ) )
    //{
    //    if( internalConfigData_->ssactive_lock )
    //    {
    //        DomSoftkey m( msg );
    //        //dcaPrimary_->setToggledFlashkey( &m );
    //    }
    //}

    // database restore
    else if (( BackupRestoreDev == device ) && ( Terminate  == event ))
    {
        // FT-1115 request configuration after db restore
        session_.send( ConfigurationDev, CfgOperatorAccessRead );
        session_.send( ConfigurationDev, CfgPrepackRead );
        if ( !session_.isPrepack() )
            session_.send( ConfigurationDev, CfgAccumulateItemsRead );
        session_.send( ConfigurationDev, CfgScaleOperationRead );
        session_.send( ConfigurationDev, CfgPrintedFieldsRead );
    }

    // hid device event
    else if (( HIDDev == device ) && ( isPrimarySessionType() ))
    {
        switch ( event )
        {
        // mouse attached
        case HIDDevMouseAttached:
            physicalMouseAttached_++;
            LOG_INFO( "physical mouse attached %d", physicalMouseAttached_ );
            break;

        // mouse removed
        case HIDDevMouseRemoved:
            // HW-220 - Always show the cursor
            // HT-57 - Only on Windows... for Linux you can plug mouse and see cursor to get to calibration
#if defined( Q_OS_LINUX )
            physicalMouseAttached_--;
            if ( physicalMouseAttached_ < 0 )
            {
                LOG_WARN( "negative mice attached, possibly missing mouse attach message at startup?", NULL );
                physicalMouseAttached_ = 0;
            }
#endif
            LOG_INFO( "physical mouse removed %d", physicalMouseAttached_ );
            break;

        // keyboard attached
        case HIDDevKeyboardAttached:
            physicalKeyboardAttached_++;
            // SOF-5068 show cursor if keyboards is attached
            LOG_INFO( "physical keyboard attached %d", physicalKeyboardAttached_ );
            break;

        // keyboard removed
        case HIDDevKeyboardRemoved:
            physicalKeyboardAttached_--;


            if ( physicalKeyboardAttached_ < 0 )
            {
                LOG_WARN( "negative keyboards attached, possibly missing keyboard attach message at startup?", NULL );
                physicalKeyboardAttached_ = 0;
            }
            LOG_INFO( "physical keyboard removed %d", physicalKeyboardAttached_ );
            break;

        case HIDDevBarcodeScannerAttached:
            physicalScannerAttached_++;
            break;

        case HIDDevBarcodeScannerRemoved:
            physicalScannerAttached_--;
            break;
        case HIDDevStorageDeviceAttached:

            LOG_INFO("USB Storage Device Attached", NULL);
            usbDeviceAttached_++;

            break;
        case HIDDevStorageDeviceRemoved:
            LOG_INFO("USB Storage Device Removed", NULL);
            usbDeviceAttached_--;
            break;

        default:
            break;
        }
        manageCursor();
    }

    else if (( PeripheralDev == device ) && ( WriteLostPeripheralsList == event ))
    {
        // if we ever receive a missing peripherial message, 138,20. We can receive it during boot
        // before it is complete, a couple minutes after boot, or at any time in any mode. In any
        // case, we never want to show the missing peripherial dialog until boot is complete.
        if( internalConfigData_->scaleBootComplete )
        {
            // if we are completely booted we can jump right to it
            showRemovePeripherals();
        }
        else
        {
            // indicates we have to pop the list once the boot process is complete
            internalConfigData_->isMissingPerifAtBoot = true;
        }
    }

    // firmware upgrade
    else if (( VersionManagerDev == device ) && ( InitiateUpgradeProcess == event ))
    {
        DomUpgradeMessage m( msg );

        const int upgradeDevice( m.device().toInt() );
        const int upgradeSubType( m.subType().toInt() );

        if (( Wrapper == upgradeDevice ) && ( 2 == upgradeSubType )) // Wrapper, WrEvolWrapper
            internalConfigData_->needsWrapperUpgrade = true;
        else if (( LabelApplier == upgradeDevice ) && ( 5 == upgradeSubType )) // Label Applier, CompactLabelApplier_EPSAC2
            internalConfigData_->needsCLAUpgrade = true;

        if ( internalConfigData_->scaleBootComplete )
            processFirmwareUpgrade();
    }

    // firmware upgrade
    else if (( NextGenWrapper == device ) && ( NGWUpgradeRequired == event ))
    {
        internalConfigData_->needsNextGenWrapperUpgrade = true;

        if ( internalConfigData_->scaleBootComplete )
            processFirmwareUpgrade();
    }

    // kpc loader upgrade
    else if( ( KPCBootloader == device ) && ( KpcBootLoaderUpgradeRequired == event ) )         // 20,28
    {
        LOG_DEBUG("got kpc upgrade", NULL);
        if(internalConfigData_->needsKPCLoaderUpgrade != true && restoringDatabase_.tryLock()){
            restoringDatabase_.unlock();
            internalConfigData_->needsKPCLoaderUpgrade = true;
            processFirmwareUpgrade();
        }
    }

    // kpc upgrader upgrade handling
    else if( ( KPCBootUpdater == device ) && ( KpcBootUpgraderUpgradeRequired == event ) )
    {
        if(internalConfigData_->needsKPCUpgraderUpgrade != true && restoringDatabase_.tryLock()){
            restoringDatabase_.unlock();
            internalConfigData_->needsKPCUpgraderUpgrade = true;
            processFirmwareUpgrade();
        }
    }

    // run in
    else if (( Wrapper == device) && ( 191 == event ))
    {
        if( isOperateMode() )
        {
            // if starting from operate mode - lose weightbarwidget at top.
            operateMode_->hideWeightBar();
        }
        switchToScreen<operateNewWrapperRunIn>();
    }

    else if (( Wrapper == device ) && ( WrReqUIWrapperModel == event ))
    {
        onShowManufacturingImageDialog();
    }

    // diagnostics wizard
    else if (( Diagnostics == device ) && ( DiagShowWizard == event ))
    {
        DiagnosticsWizardScreen *screen;
        DiagShowWizardMsg m( msg );

        LOG_INFO( "diag message: %s %s %d", qPrintable( m.event() ), qPrintable( m.diagEvent() ), m.diagErrorCode() );

        // NGW-149
        if ( isServiceMode() )
            LOG_WARN( "ignoring diag wizard event due to service mode", NULL );
        else if ( !switchToScreen<DiagnosticsWizardScreen>() )
            LOG_WARN( "failed to handle diagnostics message!", NULL );
        else if ( !(screen = dynamic_cast<DiagnosticsWizardScreen*>( mode()->screen() )) )
            LOG_WARN( "failed to open diagnostics screen!", NULL );
        else
        {
            screen->showWizard( m );
        }
    }

    // remote login credentials required
    else if (( OperatorDev == device ) && ( OpRemoteAccessCredentialsReq == event ))
    {
        DomRemoteAccessCredentialsReq m( msg );

        // default method is user name and password login
        OperatorLoginDialog::LoginType method = OperatorLoginDialog::RemoteUsernamePasswordRequired;

        // 1 = operator id validation
        // 2 = user name and password validation
        LOG_INFO( "remote access login method is %d", m.loginType() );

        if ( 1 == m.loginType() )
            method = OperatorLoginDialog::RemoteOperatorIdRequired;

        // create dialog
        OperatorLoginDialog dialog( method, this );

        // prompt login
        remoteAccessLogin_.lock();
        dialog.exec();
        remoteAccessLogin_.unlock();
    }

    // edit operator note
    else if (( OperatorDev == device ) && ( OpWriteNote == event ))
    {
        DomOperatorNoteData m( msg );

        // create dialog
        textEditor dialog( this, tr( "Operator Note" ), true, m.operatorNote() );

        // prompt operator note
        if ( QDialog::Accepted == dialog.exec() )
        {
            // save changes!
            // deformat the operator note.
            session_.send(OperatorDev, OpSaveNote, "\"" + dialog.getPlainTextEditorContents() + "\"");
        }
    }

    else if ( SessionDev == device )
    {


        /*
         * Removed with SOF-5068 since USB devices are being handled by USB manager
         *
         */
        // Service Key inserted
        if(SnKeyActivationStates == event)
        {

            MadpKeyActivation m( msg );

//          LOG_DEBUG( "SnKeyActivationStates: isManfTestKey=%d", m.isManfTestKey() );

//          // SOF-3971
            isManfTestKeyPresent = m.isManfTestKey();

            if( m.isQATestKey() )
            {
//            // FT-578 Don't have to leave System Test key in the scale once it has been inserted
//            // This will allow the special System Test methods to be used for automated testing
                systemTestKeyWasInserted = true;
            }

//        // HT-390
//        if( m.isManfTestKey() )
//        {
//            // Show the mouse cursor
//            if( turnOnCursor() )
//                cursorShownForManfTestKey_ = true;
//        }

//        // HT-390
//        if( !( m.isManfTestKey() ) )
//        {
//            // Do not show the mouse cursor
//            if( turnOffCursor( manfTestKeyCursor ) )
//                cursorShownForManfTestKey_ = false;
        }


        // FT-798
        // Special handling for session initialization message... this message tells us we are
        // done with initialization.
        else if ( SnTestMessage == event )
        {
            //
            // INIT PHASE 2
            //
            if ( initializePhase_ <= 2 )
            {
                // resume dialogs
                session_.send( SessionDev, SnRestartDialogs );
                session_.send( Error, ErrSessionDevice, "100018,5" );

                ScaleConfiguration *scaleConfig( session_.scaleConfig );

                registeredSignal_.unlock();

                // fix primary language not setting upon startup
                LOG_TRACE( "set language %s...", qPrintable( scaleConfig->getFPPLanguage() ) );
                setLanguage( scaleConfig->getFPPLanguage() );

                // navigate to starting screen
                LOG_TRACE( "opening operate main screen...", NULL );

                operateMode_->activate();

                // normal startup
                navigateHome();

                ++initializePhase_;
            }
        }

        // FT-1735 Unable to Access Self Service - start tracking with 100,10,13 msg
        else if ( SnWriteWeighModeTransactionType == event )
        {
            if( internalConfigData_->ssactive_lock )
                activateSelfServiceMode();

            toggleBackgroundImage();
        }

        // FT-804
        // session busy doing database restore
        else if ( SnDBRestoreInProgress == event )
        {
            // do not allow ui to exit until session restores the db
            restoringDatabase_.lock();

            LOG_WARN( "session is busy restoring a database!", NULL );
        }


        // FT-804
        // session done with database restore
        else if ( SnDBRestoreFinished == event )
        {
            LOG_INFO( "session database restore is complete", NULL );
            restoringDatabase_.unlock();
        }
        //--------------------------------------------------------------------------
        // HT-326
        // Active RDP Connection: Show the mouse cursor
        //--------------------------------------------------------------------------
        else if ( SnRdpActive == event ){
            cursorShownForRdpSession_ = true;
            manageCursor();
        }
        //--------------------------------------------------------------------
        // HT-326
        // No Active RDP Connection: Do Not Show the mouse cursor
        //--------------------------------------------------------------------
        else if(SnRdpInactive){
            cursorShownForRdpSession_ = false;
            manageCursor();
        }
    }
    else if ( Weigher == device )
    {
        // FT-1736
        if( WgInquire == event )
        {
            DomWeigherConfigRead m( msg );

            valuemaxIsEnabled_  = m.isScaleValuemax();

            LOG_INFO( "min weight to print %s", qPrintable( m.weigherMininumWt() ) );
            sessionHooks::instance()->updateFieldValue( sessionHooks::minWeightToPrint, m.weigherMininumWt() );
        }
        else if ( WgStatus == event )
        {
            LOG_INFO( "received a tilt status", NULL );

            //MadpWeightData *weightData( &config_->weightData );
            MadpWeightData m( msg );

            if( valuemaxIsEnabled_ )
                setOutOfTilt( m.tiltX(), m.tiltY() );
            else
                internalConfigData_->scaleWentOutOfTilt = false;
        }
    }

    else if ( CutTestDev == device )
    {
        // start cutting test
        if ( CuttingTestDtWrite == event )
        {
            DomCuttingTest m( msg );
            MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );

            // get test number
            testNumber_ =  m.testNum().toInt();

            // cutting test is prepack only - NOT ANY MORE with new cut test
            // make sure test number is valid and we are not already running a cut test

            if (( !isCuttingTestActive() ) && ( 0 < testNumber_ ))
            {
                // we need to be on main plu screen
                if ( switchToModeScreen<OperateMode,PluMainScreen>() )
                {
                    if( CutTestOption::CutTestOption_Default == scaleConfig->cuttingTestType() )            // Default
                    {
                        CuttingTestDialog d( m.testNum().toInt(), this );

                        // pop dialog
                        toggleCuttingTest( QDialog::Accepted == d.exec() );

                    }
                    else if( CutTestOption::CutTestOption_Option1 == scaleConfig->cuttingTestType() )      // Option1
                    {
                        // HT-426: removed code; it appears the screens will control dialogs properly

                        startCutTestIdleTimer();        // get it going

                        if( session_.scaleConfig->getIsOperatorLoggedIn() )
                        {
                            // Pressed Cut Test History Flashkey but we have to login an Operator if one is not logged in
                            // Dont want to pop this if we pressed the history flashkey
                            session_.scaleConfig->operatorLoggedInForCutTest = true;
                            session_.scaleConfig->returnHomeLocation = CutTestState::CutTestState_Begin;
                            launchCuttingTest( CutTestState::CutTestState_Begin );
                        }
                    }
                }
            }
        }

        else if ( CuttingTestDetailsWrite == event )
        {
            MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );

            if( CutTestOption::CutTestOption_Default == scaleConfig->cuttingTestType() )            // Default
            {
                // exit any cutting test
                // Pop the Details dialog for Original Cut Test
                toggleCuttingTest( false );

                // This loads the screen and then calls a method in the screen to update the data on screen
                loadLastCuttingTestReport();
            }
            else if( CutTestOption::CutTestOption_Option1 == scaleConfig->cuttingTestType() )      // Option1
            {
                // Pop the Details dialog for Option 1 Cut Test
                launchCuttingTestDetails();
            }
        }

        else if( CuttingTestHistoryWrite == event )
        {
            launchCuttingTestHistory();
        }

        else if ( CuttingTestSuspendSuccess == event )
        {
            // MOVED this back to operatemode.cpp where when they select to SUSPEND, this stuff happens.
            // Cut Test - This is where I come to after I send a flashkey action to suspend a cut test.
            // reset the cut test plu and spec sheet data
        }

    }

    // CAMERA softkeys
    else if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        DomLoadSoftkeyGroup m( msg );

        if ( hobart::softkeysWidget::CameraSystemPredictions == m.featureNum() )
        {
            // show camera prediction dialog in SS
            if ( session_.isSelfServiceMode() )
            {
                // invoke self service select item dialog
                sessionHooks::instance()->showCameraPredictionsDialog( &m );
            }
            // show camera prediction dialog in operate
            else if ( !CameraPredictionsDialog::isShown() )
            {
                CameraPredictionsDialog *d( new CameraPredictionsDialog( this ) );
                d->setSoftkeyGroup( &m );

                LOG_DEBUG( "showing camera dialog...", NULL );
                d->open();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onSessionMessageReceiveError( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "SessionMainWindow", "onSessionMessageReceiveError" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // error device type
    if ( Error == device )
    {
        int code( msg[2].toInt() );
        int style( msg[3].toInt() );

        LOG_DEBUG( "have error device %d event %d code %d style %d", device, event, code, style );

        // Note:
        // If there is a particular error message you wish to handle in your screen please
        // override isErrorMessageHandler() and check the message against what you want to handle.
        // If it is what you want then return true and it will not be processed below. Otherwise
        // return false and it will not be the message you are looking for.

        // check if mode/screen wants this error instead of our default handler
        if (( mode() ) && ( mode()->isErrorMessageHandler( msg ) ))
        {
            LOG_DEBUG( "ignoring error dialog message...", NULL );
            session_.handleMessage( msg );
            errorCode_ = -1;
            return;
        }

        // I dont think I need this anymore
        // also need to check if we are running a cut test, and the PLU doesn't exist
        //if( cuttingTestDialogActive_ && code == msg101001 )
        //{
        //    // JHC - handle PLU does not exist while cutting test dialog is on the screen
        //    LOG_DEBUG( "ignoring error dialog message...", NULL );
        //    session_.handleMessage( msg );
        //    errorCode_ = -1;
        //    return;
        //}


        //--------------------------------------------------------------------------
        // SOF-3534: Suppress "Initialization Complete" dialog box for ARK Boardsite
        //           Launch ARK Boardsite when the message comes into the UI
        //--------------------------------------------------------------------------
        const QString arkBoardsiteFile( "/etc/hobart/boardsite" );
        const QString peripheralBoardsiteFile( "/etc/hobart/peripheralTest" );//BCM

        if ( ( ( QFile::exists( arkBoardsiteFile ) ) || ( QFile::exists( peripheralBoardsiteFile ) ) ) )
        {
            if ( ErrSessionDevice == event && SessionDevError::SessionDevBrowserBootupCompleted == code )    // Initialization Complete message ...
            {
                LOG_DEBUG( "onSessionMessageReceiveError(): Received 103,100,100018", NULL );

                sleepFor( 2500 );                                // take a little snooze first to make sure everything is fine. Our code is sleepy
                internalConfigData_->scaleBootComplete = true;

                // SOF-232
                showCursorForBoardTester_ = true;

                if( !boardSiteWasStarted )
                {
                    // This will take over the screen and stay in board test
                    if ( switchToModeScreen<ServiceMode, manufacturingTest>() )
                    {
                        manufacturingTest *screen( dynamic_cast<manufacturingTest*>( serviceMode_ ? serviceMode_->screen() : NULL ) );

                        if ( screen )
                        {
                            // Set to Manufacturing Test:  Board Test Mode
                            screen->setManufacturingTestMode( manufacturingTest::BoardTest );

                            boardSiteWasStarted = true;
                        }
                    }
                }

                return;
            }
        }


        SessionErrorDialog *dialog( NULL );

        // dismiss dialog
        if ( ErrRemoveAnyModalDialogDevice == event )
        {
            int code = msg[2].toInt();
            errorCode_ = -1;

            LOG_DEBUG( "rejecting session dialog...", NULL );
            rejectDialog( sessionError_, true );

            // check for label taken sensor message, set signal
            if( session()->scaleConfig->scaleConfig.selfServeEnableFreshServeUI() &&
                    ( session()->isSelfServiceMode() || session()->scaleConfig->internalConfig.ssactive_lock ) )
            {

                sessionHooks::instance()->dismissCurrentErrDialog( QString::number( code ), style );

                if( ( code == ProductDevError::ProductErrLabelType ) || ( code == ProductDevError::ssProductErrLabelType ) )
                {
                    // use Case: Config:  Print Label RW/BC item - AFTER Looking up PLU and clicking OK on dialog
                    if( internalConfigData_->ssactive_lock )
                        internalConfigData_->selfservInvalidPlu = true;
                }
            }
            else
            {
                if ( 7005 == code )
                {
                    // only unlock when locked
                    if ( !labelTakenSignal_.tryLock() )
                        labelTakenSignal_.unlock();
                }
                else if( SessionDevError::SessionDevBrowserBootupCompleted == code )
                {
                    MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );
                    const QString boardTesterSD( "/etc/boardtester" );

                    //a 100,9 off of a 100018, which we do this with clicking on the OK on a 100018
                    // initialization complete, we are registered and ready to go!

                    sleepFor( 2500 );                                // take a little snooze first to make sure everything is fine. Our code is sleepy
                    internalConfigData_->scaleBootComplete = true;

                    // At bootup time, if we have Add Extra Day Flashkeys available, and the config is true, set it to false.
                    bool enableXtraDay      = scaleConfig->enableVariableShelfLifeTime();
                    bool hasXtraDayFlashkey = session_.scaleConfig->extraShelfLifeDayAvail;

                    if( enableXtraDay && hasXtraDayFlashkey )
                    {
                        // if we have add extra day flashkeys configured, and the incoming config val enableExtraSellByDay = true, disable it. Flashkeys take predcedence
                        QString param;
                        param += "," + QString::number( fieldOffsetCfgEnableVariableShelfLifeTime );
                        param += ",0,1";        // Disables extra shelf life day
                        session_.send( ConfigurationDev, CfgSaveSingleField, param );
                    }


                    // check for boardtest file present
                    // if so switch to board test screen
                    if (QFile::exists(boardTesterSD))
                    {
                        // SOF-232
                        showCursorForBoardTester_ = true;

                        // SOF-3534
                        if( !boardSiteWasStarted )
                        {
                            // This will take over the screen and stay in board test
                            if ( switchToModeScreen<ServiceMode, manufacturingTest>() )
                            {
                                manufacturingTest *screen( dynamic_cast<manufacturingTest*>( serviceMode_ ? serviceMode_->screen() : NULL ) );

                                if ( screen )
                                {
                                    // Set to Manufacturing Test:  Board Test Mode
                                    screen->setManufacturingTestMode( manufacturingTest::BoardTest );

                                    boardSiteWasStarted = true;
                                }
                            }
                        }
                    }

                    QString modelName = session_.scaleConfig->getScaleModelName();
                    if( modelName.contains( "NGW", Qt::CaseInsensitive ) )
                    {
                        session_.send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeAutoWeigh ) );
                        session_.send( SessionDev, SnReadWeighModeTransactionType );
                    }
                    else
                    {
                        if( internalConfigData_->didNotSendInitialReadWeighMode )
                        {
                            internalConfigData_->didNotSendInitialReadWeighMode = false;
                            session_.send( SessionDev, SnReadWeighModeTransactionType );
                        }
                        session_.send( TransactionTypeDev, TransactionTypeWriteCurrentType, "1" );       // Production
                    }

                }
                else if( 159001 == code || 159002 == code )
                {
                    session_.send( OperatorDev, OpLogout );
                    session()->scaleConfig->setIsOperatorLoggedIn( false );
                    session()->scaleConfig->setLoggedOperatorId("");
                    session()->scaleConfig->setLoggedOperatorName("");
                    session()->scaleConfig->setLoggedOperatorAccess(0);

                }
                //else if( 101153 <= code && 101156 >= code ) WAS else if( 101152 <= code && 101155 >= code )
                else if( ( ProductDevError::ProductErrBelowMinWeight <= code ) && ( ProductDevError::ProductErrAboveMaxTotalPrice >= code ) )
                {
                    // Received a 103,255 message to kill the Pick 5 out-of-range dialog
                    //
                    //   i.e. 103,255,101153,5
                    //        103,255,101154,5
                    //        103,255,101155,5
                    //        103,255,101156,5

                    if( session_.uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
                    {
                        if( minmaxprintwidget_ )
                        {
                            LOG_DEBUG( "onSessionMessageReceiveError(): sending productWriteSingleField", NULL );

                            // SOF-2730: Need to send productWriteSingleField - backend is waiting for
                            //           it so that it will print without any delay

                            session_.send( ProductDev, ProductWriteSingleField, currentPlu_->itemNumber() + ",0,0,0,1" );

                            minmaxprintwidget_->hide();
                        }
                    }
                }
                else if( ( code == ProductDevError::ProductErrLabelType ) || ( code == ProductDevError::ssProductErrLabelType ) )
                {
                    // use Case: Config:  Print Label RW/BC item - AFTER Looking up PLU and clicking OK on dialog
                    if( internalConfigData_->ssactive_lock )
                        internalConfigData_->selfservInvalidPlu = true;
                }

            }
        }

        // create dialog
        else if ( !blockError_ )
        {
            // Do not allow the dialog to pop under certain cutting test conditions. Add more conditions as needed.
            if( cuttingTestActive_ )
            {
                if( 101001 == code )
                {
                    QString return_error = QString::number( code ) + "5";
                    session_.send( Error, ErrRemoveAnyModalDialogDevice, return_error );
                    session_.handleMessage( msg );
                    errorCode_ = -1;
                    return;
                }
            }
			/* THIS CODE EXECUTES A CUSTOM DIALOG BOX FOR MIN MAX PRINTING	*/
            // ENABLED AND DEMOed IN  3.3.503.
            if( session_.uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
            {
                if( ( ProductDevError::ProductErrBelowMinWeight <= code ) && ( ProductDevError::ProductErrAboveMaxTotalPrice >= code ) )
                {
                    if( minmaxprintwidget_ == NULL )
                        minmaxprintwidget_  = new hobart::MinAndMaxPrintWidget( this, this ) ;

                    if( minmaxprintwidget_ )
                     {
                        // open dialog
                        if ( minmaxprintwidget_->isValid() )
                        { 
                            if( session()->scaleConfig->getIsPick5Enabled() && session()->scaleConfig->getIsPick5Active() )
                            {
                                // wasnt getting this info quick enough in dialog, so pass it in
                                currentPlu_ = &session_.scaleConfig->currentPLU;
                                QString minWt = currentPlu_->minWeight();
                                QString maxWt = currentPlu_->maxWeight();
                                QString minPr = currentPlu_->minPrice();
                                QString maxPr = currentPlu_->maxPrice();

                                minmaxprintwidget_->displayOutOfRangeData(QString::number( code ), minWt, maxWt, minPr, maxPr);
                                setScreenLocation();
                                minmaxprintwidget_->move( QPoint( x_offset_, y_offset_ ) );
                                minmaxprintwidget_->show();
                            }
                        }
                    }
                    return;
                }
            }

            if( session()->scaleConfig->scaleConfig.selfServeEnableFreshServeUI() &&
                    ( session()->isSelfServiceMode() || session()->scaleConfig->internalConfig.ssactive_lock ) )
            {
                sessionHooks::instance()->updateCurrentErrDialog( QString::number( code ), style );
            }
            // Error Dialogs
            else if ( (dialog = new SessionErrorDialog( msg, this )) )
            {
                // save dialog off for when we remove it later
                sessionError_ = dialog;
                errorCode_ = code;

                // open dialog
                if ( dialog->isValid() )
                    dialog->open();
                else
                {
                    LOG_WARN( "invalid dialog!", NULL );
                    delete dialog;
                }
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onShowManufacturingImageDialog()
{
    LOG_CLASS_INFO( "SessionMainWindow", "showManufacturingImageDialog" );

    LOG_DEBUG( "entering dialog exec manufacturing image...", NULL );
    ManufacturingImageDialog dialog( this );

    // pop the dialog
    int rc = dialog.exec();

    LOG_DEBUG( "dialog exec complete %d", rc );
}

void SessionMainWindow::onSessionMessageReceiveLockDownInternal(bool isLocked)
{
    LOG_CLASS_INFO( "SessionMainWindow", "onSessionMessageReceiveLockDownInternal" );

    if ( this )
    {
        if( isLocked )
        {
            this->setEnabled( true );

            LOG_DEBUG( "onSessionMessageReceiveLockDownInternal: ENABLED MAIN WINDOW", NULL );
        }
        else
        {
            this->setEnabled( false );

            LOG_DEBUG( "onSessionMessageReceiveLockDownInternal: DISABLED MAIN WINDOW", NULL );
        }
    }
}

void SessionMainWindow::onMenuPanelDismissed()
{
    Q_EMIT hidingMenuPanel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::unlockMainWindow(bool isEnabled)
{
    LOG_CLASS_INFO( "SessionMainWindow", "unlockMainWindow" );

    if ( this )
    {
        this->setEnabled( isEnabled );

        LOG_DEBUG( "unlockMainWindow: this->setEnabled( %d )", isEnabled );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::onShowRemovePeripheralsDialog()
{
    LOG_CLASS_INFO( "SessionMainWindow", "showRemovePeripherals" );

    // already showing missing perifs
    if ( removePeripheralsVisible_ )
        return;

    // Note:
    // The backend seems to only send the lost peripheral list once. If we have devices that take
    // longer to come online it is possible the message we have is outdated and showing devices
    // that are now connected. So lets compare our lost peripheral message against a current
    // peripheral list and reconcile the differences.
    MadpLostPeripheralList *lostPeripheralList( &session_.scaleConfig->lostPeripheralList_ );
    MadpPeripheralList *peripheralList( &session_.scaleConfig->peripheralList );

    // gcd
    if (( lostPeripheralList->gcd() ) && ( peripheralList->gcd() ))
        lostPeripheralList->setGcd( 0 );

    // label applier
    if (( lostPeripheralList->labelApplier() ) && ( peripheralList->labelApplier() ))
        lostPeripheralList->setLabelApplier( 0 );

    // printers
    if (( lostPeripheralList->firstPrinter() ) && ( peripheralList->firstPrinter() ))
        lostPeripheralList->setFirstPrinter( 0 );
    if (( lostPeripheralList->secondPrinter() ) && ( peripheralList->secondPrinter() ))
        lostPeripheralList->setSecondPrinter( 0 );
    if (( lostPeripheralList->thirdPrinter() ) && ( peripheralList->thirdPrinter() ))
        lostPeripheralList->setThirdPrinter( 0 );

    // weighers
    if (( lostPeripheralList->firstWeigher() ) && ( peripheralList->firstWeigher() ))
        lostPeripheralList->setFirstWeigher( 0 );
    if (( lostPeripheralList->secondWeigher() ) && ( peripheralList->secondWeigher() ))
        lostPeripheralList->setSecondWeigher( 0 );

    // wrapper
    if (( lostPeripheralList->wrapper() ) && ( peripheralList->wrapper() ))
        lostPeripheralList->setWrapper( 0 );

    // handle missing peripherals
    if ( lostPeripheralList->isMissingPeripheral() )
    {
        LOG_DEBUG( "entering dialog exec mising perifs...", NULL );
        removePeripheralsVisible_ = true;

        RemovePeripheralsDialog dialog( this );

        // pop the dialog
        int rc = dialog.exec();

        LOG_DEBUG( "dialog exec complete %d", rc );
        removePeripheralsVisible_ = false;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::setOutOfTilt( const QString& xTilt, const QString& yTilt )
{
    double tiltX =  xTilt.toDouble() / 100.0;
    double tiltY =  yTilt.toDouble() / 100.0;

    // now test at 3% tilt - also dont know how fast these are coming in,
    // may have to up the mod value if coming in fast ( (weigherCts % 5) == 0 )
    if( (tiltX > 3.001) || (tiltY > 3.001) || (tiltX < -3.001) || (tiltY < -3.001) )
    {
        internalConfigData_->scaleWentOutOfTilt = true;
        // If I need to report this everywhere then I need to display this now right here. If I only have to report
        // it when I have a weight field and an active PLU, then do it in the weightbarwidget like I am doing now.
        //session_->send( Error, ErrWeigherDevice, QString::number(WeigherDevError::WeigherErrValMaxTiltReached) + ",5" );				// 1025
    }
    else
    {
        internalConfigData_->scaleWentOutOfTilt = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::initialize()
{
    // make sure our images are ready
    Q_INIT_RESOURCE( sessionhandler );

    // set font
    QFont f( font() );
    f.setFamily( "Arial" );
    f.setPointSize( (height() * 14) / scalingDividerY_ );
    setFont( f );

    // set central widget
    setCentralWidget( centralWidget_ = new QStackedWidget( this ) );

    // create overlay
    overlay_ = new Overlay( Qt::LeftDockWidgetArea, this );

    // create check for media handler
    checkMediaHandler_ = new QTimer( this );
    checkMediaHandler_->setInterval( 5000 );
    checkMediaHandler_->setSingleShot( false );

    connect( checkMediaHandler_, SIGNAL( timeout() ), SLOT( onCheckMedia() ) );
    checkMediaHandler_->start();

    // create print handler
    printKeyHandler_ = new QTimer( this );

    connect( printKeyHandler_, SIGNAL( timeout() ), SLOT( onPrintKeyPressed() ) );

    // create weights and measures handler
    weightsMeasuresHandler_ = new QTimer( this );

    connect( weightsMeasuresHandler_, SIGNAL( timeout() ), SLOT( onRequestWeightsMeasuresInfo() ) );

    // create remove peripherals handler
    showRemovePeripherals_ = new QTimer( this );
    connect( showRemovePeripherals_, SIGNAL( timeout() ), SLOT( onShowRemovePeripheralsDialog() ) );

    cuttestInactivityLogout_ = new QTimer( this );
    cuttestInactivityLogout_->setSingleShot( false );
    connect( cuttestInactivityLogout_, SIGNAL( timeout() ), SLOT( onLogoutCutTestInactivity() ) );

    // create input devices
    keyboard_ = new KeyboardWidget( this );
    numberPad_ = new NumberPadWidget( this );

    // read the white list remote list printing
    readPluWhiteList();

    connect( keyboard_, SIGNAL( characterGenerated(int,Qt::KeyboardModifiers,QString) ), SLOT( onSendCharacter(int,Qt::KeyboardModifiers,QString) ) );
    connect( keyboard_, SIGNAL( visible(bool) ), SIGNAL( keyboardVisible(bool) ) );

    connect( numberPad_, SIGNAL( characterGenerated(int,Qt::KeyboardModifiers,QString) ), SLOT( onSendCharacter(int,Qt::KeyboardModifiers,QString) ) );
    connect( numberPad_, SIGNAL( keyLayoutStyleChanged() ), SIGNAL( numberPadKeyLayoutStyleChanged() ) );
    connect( numberPad_, SIGNAL( moved(Qt::DockWidgetArea) ), SLOT( onNumberPadMoved(Qt::DockWidgetArea) ) );

    // set background
    toggleBackgroundImage();

    // connect slots
    connect( qApp, SIGNAL( focusChanged(QWidget*,QWidget*) ), SLOT( onFocusChanged(QWidget*,QWidget*) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::toggleBackgroundImage()
{
    DomWeighModeTotalType *weighModeTotalType( &session_.scaleConfig->weighModeTotalType );

    // TODO: when offline they can force background to online image
    bool showOffline( offline() );

    // load appropriate background
    QPixmap background( size() );
    QPainter painter( &background );

    // check for cutting test
    if ( cuttingTestActive_ )
    {
        // make the background solid yellow
        painter.fillRect( background.rect(), QBrush( hobart::colorPalette::yellow ) );
    }
    // check mode has background
    else if ( weighModeTotalType->useBackgroundColor() )
    {
        QString color( QString( "%1" ).arg( weighModeTotalType->backgroundColor(), 6, 16, QLatin1Char('0') ) );

        painter.fillRect( background.rect(), QBrush( QColor( "#" + color ) ) );
    }
    // check for offline... when offline use different background
    else if ( showOffline )
    {
        // make the background solid red
        painter.fillRect( background.rect(), QBrush( hobart::colorPalette::red ) );
    }
    // if we toggled pick 5
    else if( pick5Toggled_ )
    {
        QString p5Name = "background_pick5.png";
        QString bgimg( filesystem::absoluteFilePath( p5Name, folderType::Layouts ) );

        painter.drawPixmap( background.rect(), QPixmap( bgimg ) );
    }
    // normal stainless steel background
    else
    {
        // use background image
        painter.drawPixmap( background.rect(), QPixmap( ":/images/background.png" ) );
    }

    // set palette
    modifyPaletteBackground setBackground( background );
    setBackground( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::printKeyPressed()
{
    LOG_CLASS_INFO( "SessionMainWindow", "printKeyPressed" );

    LOG_TRACE( "print key pressed", NULL );

    if ( !isOperateMode() )
        LOG_WARN( "ignoring print key (not in operate mode)", NULL );
    else if ( !operateMode_->isPrintingAllowed( true ) )
        LOG_WARN( "ignoring print key (not allowed)", NULL );
    else
    {
        // schedule timer to handle key press
        printKeyHandler_->setSingleShot( true );
        printKeyHandler_->start( 0 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::searchKeyPressed()
{

    MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );
    bool searchEnabled = scaleConfig->enableSearchMenuButton();

    if( searchEnabled )                 // most likely scenerio
    {
        SearchDialog dialog( this );

        if ( QDialog::Accepted == dialog.exec() )
        {
            if ( switchToModeScreen<OperateMode, PluMainScreen>() )
            {
                ModeWidget *m = mode();
                ScreenWidget *screen = (m ? m->screen() : NULL);

            PluMainScreen *pluMainScreen = qobject_cast<PluMainScreen*>( screen );

                if ( pluMainScreen )
                    pluMainScreen->setCurrentItemNumber( dialog.selectedItemNumber() );
            }
        }
    }
    else
    {
        session_.send( Error, ErrSessionDevice, "100057,5" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect SessionMainWindow::calcKeyboardGeometry( KeyboardLocation loc )
{
    LOG_CLASS_INFO("SessionMainWindow", "calcKeyboardGeometry");
    // sanity check
    if ( !keyboard_ )
        return QRect();

    // save/restore previous keyboard location
    if ( KeyboardLocationUsePrevious == loc )
        loc = previousKeyboardLocation_;
    else
        previousKeyboardLocation_ = loc;

    // get size and position
    const QSize parentSize( contentsRect().size() );
    const QPoint pos( mapToGlobal( contentsRect().topLeft() ) );

    const QSize widgetSize( (width() * 940) / scalingDividerX_, (height() * 230) / scalingDividerY_ );

    int verticalOffset( contentsMargins().top() );
    LOG_DEBUG("vertical offset: %d", verticalOffset );

    if (( KeyboardLocationSmart == loc ) && ( inputWidget_ ))
    {
        QPoint inputWidgetPos( inputWidget_->mapTo( this, QPoint( 0, 0 ) ) );

        QPlainTextEdit *plainTextEdit;
        QTextEdit *textEdit;

        // for multi line edit use cursor position instead of widget location
        if ( (plainTextEdit = qobject_cast<QPlainTextEdit*>( inputWidget_ )) )
        {
            QPoint topLeft( plainTextEdit->cursorRect().topLeft() );
            inputWidgetPos = inputWidget_->mapTo( const_cast<SessionMainWindow*>( this ), topLeft );
        }
        else if ( (textEdit = qobject_cast<QTextEdit*>( inputWidget_ )) )
        {
            QPoint topLeft( textEdit->cursorRect().topLeft() );
            inputWidgetPos = inputWidget_->mapTo( const_cast<SessionMainWindow*>( this ), topLeft );
        }

        // check if widget position is on top half of the screen
        if ( inputWidgetPos.y() < (parentSize.height() / 2) )
            loc = KeyboardLocationBottom;
    }

    if ( KeyboardLocationBottom == loc ){
        verticalOffset = parentSize.height() - widgetSize.height() - contentsMargins().bottom();
        //qDebug() << parentSize.height() << widgetSize.height() << contentsMargins().bottom() << verticalOffset;
    }

    return QRect(
        pos.x() + ((parentSize.width() - widgetSize.width()) / 2),
        pos.y() + verticalOffset,
        widgetSize.width(),
        widgetSize.height() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect SessionMainWindow::calcNumberPadGeometry() const
{
    const QPoint pos( mapToGlobal( contentsRect().topLeft() ) );
    const QSize widgetSize( (width() * 370) / scalingDividerX_, (height() * 370) / scalingDividerY_ );
/*
    // when operate mode we can calculate exact location
    if ( isOperateMode() )
    {
        PluMainScreen *screen( operateMode_->isCached<PluMainScreen>() );

        const QRect screenDcaGeometry( screen->dcaGeometry( 2 ) );
        const QPoint screenDcaPos( screen->mapTo( this, screenDcaGeometry.topLeft() ) );

        return QRect(
            pos.x() + screenDcaPos.x(),
            pos.y() + screenDcaPos.y(),
            widgetSize.width(),
            widgetSize.height() );
    }
*/
    int horizontalOffset( 5 );
    int verticalOffset( height() - widgetSize.height() - 5 );

    if ( Qt::RightDockWidgetArea == numberPadArea() )
        horizontalOffset = width() - widgetSize.width() - horizontalOffset;

    if ( mode() )
    {
        if ( Qt::RightDockWidgetArea == numberPadArea() )
            horizontalOffset -= mode()->margins().right();
        else
            horizontalOffset += mode()->margins().left();

        verticalOffset -= mode()->margins().bottom();
    }

    return QRect(
        pos.x() + horizontalOffset,
        pos.y() + verticalOffset,
        widgetSize.width(),
        widgetSize.height() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::swipeScreen( int direction )
{
    LOG_CLASS_INFO( "SessionMainWindow", "swipeScreen" );

    ModeWidget *m( mode() );

    if ( !m )
        return;

    // when dialog on screen ignore the swipe
    if ( isDialogOpen() )
    {
        LOG_DEBUG( "ignoring swipe due to dialog on screen", NULL );
        return;
    }

    // minimize help if on screen
    if (( ScreenWidget::Down == direction ) && ( webBrowserVisible_ ) && ( webBrowser_ ))
    {
        LOG_DEBUG( "minimizing help", NULL );
        minimizeHelpDialog();
        return;
    }

    bool handled( false );

    // retrieve pointer to screen
    ScreenWidget *screen( m->screen() );

    // make sure screen exists
    if ( !screen )
        LOG_WARN( "no screen to swipe", NULL );
    else
    {
        LOG_INFO( "handle swipe direction %d", direction );
        handled = screen->handleSwipe( (ScreenWidget::SwipeDirection) direction );
    }

    // restore help if swiping up
    if (( ScreenWidget::Up == direction ) && ( !handled ))
    {
        LOG_INFO( "operator swiped up, restoring help", NULL );
        restoreHelpDialog();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::sleepFor( int ms ) const
{
    // amout of time to sleep
    QTime sleep( QTime::currentTime().addMSecs( ms ) );

    do
    {
        QApplication::processEvents();

    } while ( QTime::currentTime() < sleep );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::requestWeightsMeasuresInfo()
{
    selfServiceHook *hook( qobject_cast<selfServiceHook*>( sessionHooks::instance()->findHook( "selfServiceHook" ) ) );

    //	FT-1637: If SS is not active - do nothing.
    if ( !hook )
        return;
    else if ( !hook->active() )
        return;

    weightsMeasuresHandler_->setSingleShot( true );
    weightsMeasuresHandler_->start( 0 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::setScreenLocation()
{
    LOG_CLASS_INFO( "SessionMainWindow", "setScreenLocation" );

    int my10width = 1024;       // 512
    int my10height = 600;       // 300

    bool use10Inch = true;

    QDesktopWidget *desktop = QApplication::desktop();

    if ( desktop )
    {
        const QRect desktopRect( desktop->screenGeometry( 0 ) );

        if ( ( desktopRect.width() > my10width ) || ( desktopRect.height() > my10height  ) )
            use10Inch = false;      // so then assuming 14 inch.  Revist if start using 21 inch Zeniths

        LOG_INFO( "primary screen size is (%d, %d)", desktopRect.width(), desktopRect.height() );
    }

    if( use10Inch )
    {
         x_offset_ = 40;
         y_offset_ = 150;
    }
    else
    {
        x_offset_ = 100;
        y_offset_ = 200;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::rejectDialog( SessionDialog *specific, bool forced )
{
    // lock the mutex
    QMutexLocker lock( &const_cast<SessionMainWindow*>( this )->dialogListMutex_ );

    SessionDialog *dialog( NULL );
    int dialogCount( 0 );

    // grab head dialog
    if ( (dialogCount = dialogList_.size()) )
        dialog = dialogList_.front();

    lock.unlock();

    if ( !dialog )
        return true;

    // make sure this is the dialog we are looking for
    if (( specific ) && ( dialog != specific ))
        return false;

    // make sure rejection is allowed for this dialog
    if ( !dialog->allowOverlayClickDismiss() )
        if ( !forced )
            return false;

    // reject!
    rejectDialogs( 1 );

    // return true when no more dialogs on screen
    return (dialogCount <= 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::rejectDialogs( int count )
{
    // loop until no dialogs or count exhausted
    while ( 0 < count-- )
    {
        // lock the mutex
        QMutexLocker lock( &dialogListMutex_ );

        SessionDialog *dialog( NULL );

        // grab head dialog
        if ( !dialogList_.empty() )
            dialog = dialogList_.front();

        lock.unlock();

        if ( !dialog )
            break;

        // reject the dialog!
        else
        {
            dialog->reject();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::isDialogOpen() const
{
    // lock the mutex
    QMutexLocker lock( &const_cast<SessionMainWindow*>( this )->dialogListMutex_ );

    // check for dialog
    return !dialogList_.empty();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::handleWebBrowserCommand( WebBrowserCommand fn, const QString& param, const QByteArray& fileData )
{
    LOG_CLASS_INFO( "SessionMainWindow", "handleWebBrowserCommand" );

    ///////////////////////////////////////////////////////////////////////////
    // handle command

    // minimize active web browser
    if ( Minimize == fn )
    {
        LOG_DEBUG( "minimize help dialog", NULL );

        if (( webBrowserVisible_ ) && ( webBrowser_ ))
            webBrowser_->reject();

        return;
    }

    // restore active web browser
    else if ( Restore == fn )
    {
        LOG_DEBUG( "restore help dialog", NULL );

        // already showing the browser or no browser to restore
        if (( webBrowserVisible_ ) || ( !webBrowser_ ))
            return;
    }

    // show document (in fresh instance of dialog)
    // show help (in fresh instance of dialog)
    // show video (in fresh instance of dialog)
    // show web page (in fresh instance of dialog)
    else if (( ShowDocumentFromFile == fn ) ||
        ( ShowDocumentFromMemory ==  fn ) ||
        ( ShowHelp == fn ) ||
        ( ShowVideo == fn ) ||
        ( ShowWebpage == fn ))
    {
        LOG_DEBUG( "showing help dialog", NULL );

        // already showing the browser
        if ( webBrowserVisible_ )
            return;

        freeWebBrowser();

        // for webpage, check extension against video files and switch function if needed
        if ( ShowWebpage == fn )
        {
            QString extension( param.right( 3 ) );
            extension = extension.toLower();

            if (( "asf" == extension ) ||   // advanced systems format
                ( "avi" == extension ) ||   // audio video interleave
                ( "flv" == extension ) ||   // flash video
                ( "m4v" == extension ) ||   // itunes video
                ( "mov" == extension ) ||   // apple quicktime
                ( "mp4" == extension ) ||   // mpeg-4 video
                ( "mpg" == extension ) ||   // mpeg video
                ( "swf" == extension ) ||   // shockwave flash
                ( "vob" == extension ) ||   // DVD video object file
                ( "wmv" == extension ))     // windows media
            {
                fn = ShowVideo;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // show dialog

    LOG_DEBUG( "entering dialog exec...", NULL );

    // generate new instance if missing
    if ( !webBrowser_ )
    {
        MadpScaleConfig *scaleConfig( &session_.scaleConfig->scaleConfig );

        bool allowAddressBar( scaleConfig->allowAddressBar() ); // FT-1451

        // HTs
        // this feature is disabled
        if ( !session_.uiUtilities->isFeatureLicensed( SCALE_WEB_BROWSER ) )
            allowAddressBar = false;

        LOG_TRACE( "creating new dialog...", NULL );
        webBrowser_ = new WebBrowserDialog( this );

        webBrowser_->setUrlVisible( true, allowAddressBar );

        // show help
        if ( ShowHelp == fn )
        {
            webBrowser_->setBrowserName( tr( "Operator's Manual" ) );

            // if scaleui is not running as a primary session then we are remote
            if (session_.scaleConfig->myConsole != 1)
            {
                if (PLATFORM == platformType::Windows)
                {
                    QString programFilesPath(getenv("PROGRAMFILES"));
                    // TODO:
                    // Needs changed once we get the final operator's manual
                    QString helpPath = programFilesPath + "/HTe Remote Scale Access/scaleui/html/HT_Operator_Manual.htm";
                    webBrowser_->showHtml(helpPath);
                }
            }
            else
            {
                // build HTML path adding the scaleModel type here
                QString programFilesPath(getenv("PROGRAMFILES"));
                QString htmlPath;

                // It is a Service Counter scale
                if ( "3" == scaleConfig->scaleModel() )
                {
                    htmlPath = "ServiceCounter/HT_Operator_Manual.htm";

                    if ( scaleConfig->scaleName().startsWith("FS") )
                        htmlPath = "ServiceCounter/FS_Operator_Manual.htm";
                }

                // It is a Prepack Scale (or unknown)
                else
				{
				if(session()->scaleConfig->isWrapperTwoPrinter()){
					 htmlPath = "Prepack/NGW2P/NGW_Operator Manual.htm";
				}
				
				else
					htmlPath = "Prepack/HT_Operator_Manual.htm";
				}

                //
                LOG_INFO("Help scale model is %s ", scaleConfig->scaleModel().toLatin1().constData());
                LOG_INFO("Manual is located in %s", htmlPath.toLatin1().constData());
                LOG_INFO("Manual full path is : %s", filesystem::absoluteFilePath(htmlPath,folderType::Html).toLatin1().constData());
                // runnign locally on scale
                // Old code below
                //webBrowser_->showHtml(filesystem::absoluteFilePath("HT_Operator_Manual.htm",folderType::Html));
                // HT-15 combined code should have both Operator Manuals
                webBrowser_->showHtml(filesystem::absoluteFilePath(htmlPath,folderType::Html));
                // Need to load appropriate User's Manual
        	}
        }

        // show document
        else if ( ShowDocumentFromFile == fn )
        {
            LOG_INFO( "showing document '%s'", param.toLatin1().constData() );
            webBrowser_->showDocument( param );
        }

        // show document
        else if ( ShowDocumentFromMemory == fn )
        {
            //LOG_INFO( "showing document '%s'", param.toLatin1().constData() );
            webBrowser_->showDocument( fileData );
        }

        // show video
        else if ( ShowVideo == fn )
        {
            LOG_INFO( "playing video '%s'", param.toLatin1().constData() );
            webBrowser_->playVideo( param );
        }

        // drill in to specific help document based on scale model
        // show web page
        else if ( ShowWebpage == fn )
        {
            webBrowser_->setBrowserName( tr( "Web Browser" ) );
            webBrowser_->showHtml( param );
        }

        overlay_->setDialogIconVisible( true );
    }

    // execute the dialog
    webBrowserVisible_ = true;
    int rc = webBrowser_->exec();
    webBrowserVisible_ = false;

    LOG_DEBUG( "dialog exec complete %d", rc );

    // operator is done with the dialog
    if ( QDialog::Accepted == rc )
    {
        LOG_DEBUG( "free dialog...", NULL );
        freeWebBrowser();
    }

    // update overlay
    overlay_->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::freeWebBrowser()
{
    LOG_CLASS_INFO( "SessionMainWindow", "freeWebBrowser" );

    if ( webBrowser_ )
    {
        LOG_TRACE( "flushing events...", NULL );
        QApplication::sendPostedEvents();
        QApplication::flush();

        QApplication::processEvents();

        LOG_TRACE( "deleting dialog...", NULL );
        WebBrowserDialog *doomed = webBrowser_;

        webBrowser_ = NULL;
        delete doomed; // FT-566 hard delete... do not deleteLater()

        overlay_->setDialogIconVisible( false );
    }

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::processFirmwareUpgrade()
{
    if( internalConfigData_->isClaModelInitialization )
    {
        // SOF-4929: Make sure the CLA Upgrade screen is displayed
        switchToScreen<checkUpgradeAccessCLA>();
    }
    else
    {
        if ( internalConfigData_->needsNextGenWrapperUpgrade )
            switchToScreen<upgradeNextGenWrapper>();
        else if ( internalConfigData_->needsWrapperUpgrade )
            switchToScreen<checkUpgradeAccessWrapper>();
        else if ( internalConfigData_->needsCLAUpgrade )
            switchToScreen<checkUpgradeAccessCLA>();
        else if ( internalConfigData_->needsKPCLoaderUpgrade )
            switchToScreen<upgradeKPCSoftware>();
        else if ( internalConfigData_->needsKPCUpgraderUpgrade )
            switchToScreen<upgradeKPCSoftware>();
        internalConfigData_->needsNextGenWrapperUpgrade =
            internalConfigData_->needsWrapperUpgrade =
            internalConfigData_->needsCLAUpgrade = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::showRemovePeripherals()
{
    showRemovePeripherals_->setSingleShot( true );
    showRemovePeripherals_->start( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::printerLabelTaken() const
{
    LOG_CLASS_INFO( "SessionMainWindow", "printerLabelTaken" );

//    const MadpKeyActivation *keyActivation = &session_.scaleConfig->keyActivation;

//    if ( !keyActivation->isQATestKey() )
    // FT-578
    if ( !systemTestKeyWasInserted )
    {
        LOG_WARN( "QA test key was not installed!", NULL );
        return false;
    }

    static const int lockWaitTime = 50;
    static const int timeoutTime = 60000;

    LOG_TRACE( "Resetting signal...", NULL );

    // lock if not locked, otherwise its already locked and we are ready...
    labelTakenSignal_.tryLock();

    LOG_DEBUG( "Waiting %d ms for signal...", timeoutTime );
    int loops = (timeoutTime / lockWaitTime);

    do
    {
        QApplication::sendPostedEvents();
        QApplication::flush();

        QApplication::processEvents();

    } while (( !labelTakenSignal_.tryLock( lockWaitTime ) ) && ( --loops ));

    LOG_DEBUG( "Done waiting %d", loops );

    // when timed out return false, otherwise we locked and success!
    return (loops ? true : false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::setScaleTestWeight( const QString& value )
{
    LOG_CLASS_INFO( "SessionMainWindow", "setScaleTestWeight" );

//    const MadpKeyActivation *keyActivation = &session_.scaleConfig->keyActivation;

//    if ( !keyActivation->isQATestKey() )
    // FT-578
    if ( !systemTestKeyWasInserted )
    {
        LOG_WARN( "QA test key was not installed!", NULL );
        return;
    }

    LOG_DEBUG( "Setting scale test weight to '%s'", qPrintable( value ) );

    session_.send( Weigher, WgTestSimulateWgStatus, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::endScaleTestWeight( bool value )
{
    Q_UNUSED( value )

    LOG_CLASS_INFO( "SessionMainWindow", "endScaleTestWeight" );

//    const MadpKeyActivation *keyActivation = &session_.scaleConfig->keyActivation;

//    if ( !keyActivation->isQATestKey() )
    // FT-578
    if ( !systemTestKeyWasInserted )
    {
        LOG_WARN( "QA test key was not installed!", NULL );
        return;
    }

    LOG_DEBUG( "End Scale Fake Weight Mode", NULL );

    session_.send( Weigher, WgEndTestSimulateWgStatus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::installTranslator( QTranslator *translator )
{
    // nothing to do
    if ( installedTranslator_ == translator )
        return;

    // get rid of current translator
    if ( installedTranslator_ )
        QApplication::removeTranslator( installedTranslator_ );

    // mark it as installed
    installedTranslator_ = translator;

    // install it!
    if ( installedTranslator_ )
        QApplication::installTranslator( installedTranslator_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::installQtTranslator( QTranslator *translator )
{
    // nothing to do
    if ( installedQtTranslator_ == translator )
        return;

    // get rid of current translator
    if ( installedQtTranslator_ )
        QApplication::removeTranslator( installedQtTranslator_ );

    // mark it as installed
    installedQtTranslator_ = translator;

    // install it!
    if ( installedQtTranslator_ )
        QApplication::installTranslator( installedQtTranslator_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int SessionMainWindow::getErrorCode() const
{
    return errorCode_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int SessionMainWindow::getCutTestNumber() const
{
    return testNumber_;
}
