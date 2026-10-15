/**
 * @file sessionerrordialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "../platform/python.h"

#include <logmanager.h>
#include <sessionhooks.h>

#include "sessionerrordialog.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <ScaleClasses.h>

#include <QAbstractButton>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QFile>
#include "python.h"

// SOF-4594 Non-English translation issue for Spanish and French
// Turns out - These dont translate
/*
#define BUTTON_TEXT_KILOGRAMS       "Kilograms"
#define BUTTON_TEXT_POUNDS          "Pounds"
#define BUTTON_TEXT_REBOOT          "Reboot"
#define BUTTON_TEXT_REZERO          "Rezero"
#define BUTTON_TEXT_IGNORE          "Ignore"
#define BUTTON_TEXT_SHUTDOWN        "Shutdown"
#define BUTTON_TEXT_UPDATEONLY      "Update Only"
#define BUTTON_TEXT_UPDATEPRINT     "Update and Print"
#define BUTTON_TEXT_DIAGNOSE        "Diagnose"
#define BUTTON_TEXT_WATCH_VIDEO     "Watch Video"
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionErrorDialog::SessionErrorDialog( const SessionMessage& msg, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    msg_( msg ),
    allowDismiss_( false ),
    valid_( true ),
    needToSendClosedMessage_( true ),
    needToForwardMessage_( false ),
    ignoreButtonPressed_( false ),
    defaultRole_( QDialogButtonBox::RejectRole )
{
    LOG_CLASS_INFO( "SessionErrorDialog", "SessionErrorDialog" );

    setObjectName( "SessionErrorDialog" );

    internalConfigData_ = &session_->scaleConfig->internalConfig;

    prKind() = session_->scaleConfig->peripheralList.firstPrinter();
    // hook into session
    if ( session_ )
    {
        // connect signals
        connect( session_, SIGNAL( receiveErrorMessage(SessionMessage) ), SLOT( onSessionMessageReceiveErrorInternal(SessionMessage) ), Qt::QueuedConnection );
        connect( session_, SIGNAL( receiveLockDownScreenMessage(bool) ),  SLOT( onSessionMessageReceiveLockDownInternal(bool) ) );
    }
    else
    {
        LOG_WARN( "parent is not main window", NULL );
        valid_ = false;
    }

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionErrorDialog::~SessionErrorDialog()
{
    LOG_CLASS_INFO( "SessionErrorDialog", "~SessionErrorDialog" );

    // for dialog that cannot be dismissed by operator, forward message onto ui
    if ( needToForwardMessage_ )
    {
        SessionMessage ui;
        ui.resize( 4 );

        ui[0] = msg_[0];
        ui[1] = msg_[1];
        ui[2] = msg_[2];
        ui[3] = msg_[3];

        LOG_TRACE( "forwarding message to ui...", NULL );
        session_->handleMessage( ui );
    }

    // make sure closed message is sent
    else
    {
        sendClosedMessage( defaultRole_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::hideEvent( QHideEvent *event )
{
    // dismiss any active dialog
    sessionHooks::instance()->dismissDialog();

    _Mybase::hideEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::showEvent( QShowEvent *event )
{
    QList<QAbstractButton*> dialogButtons( buttons() );
    sessionHooks::buttonMap hookButtons;

    // create list of button hooks
    foreach ( QAbstractButton *button, dialogButtons )
        hookButtons[ buttonRole( button ) ] = button->text();

    // show dialog
    sessionHooks::instance()->showDialog( title(), text(), hookButtons );

    // connect
    connect( sessionHooks::instance(), SIGNAL( dialogButtonPress(int) ), SLOT( onHookDialogButtonPress(int) ) );

    _Mybase::showEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onButtonClicked( QAbstractButton *button )
{
    LOG_CLASS_INFO( "SessionErrorDialog", "onButtonClicked" );

    if ( !button )
        return;
    else if ( ignoreButtonPressed_ )
        return;

    QDialogButtonBox::ButtonRole role = buttonRole( button );
    LOG_INFO( "operator pressed button '%s' role %d", button->text().toLatin1().constData(), role );

    sendClosedMessage( role );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onDiagnoseButtonClicked()
{
    LOG_CLASS_INFO( "SessionErrorDialog", "onDiagnoseButtonClicked" );
    LOG_INFO( "diagnose button pressed", NULL );

    // start up the diagnose wizard
    session_->send( Diagnostics, DiagStartErrorHandler, diagEvent_ + (diagEventForced_ ? ",1" : ",0") );

    sendClosedMessage( defaultRole_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "SessionErrorDialog", "onSessionMessageReceive" );

    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    int code( msg_[2].toInt() );

    // for platter tare do not allow button presses while unstable
    if ( 101043 == code )
    {
        bool processing( false );

        // disable platter tare
        if (( SessionDev == device ) && ( SnPlatterTareUnstable == event ))
        {
            ignoreButtonPressed_ = true;
            processing = true;
        }

        // enable platter tare
        else if (( ProductDev == device ) && ( ProductWriteSingleField == event ))
        {
            ignoreButtonPressed_ = false;
            processing = true;
        }

        // enable/disable buttons
        if ( processing )
        {
            QList<QAbstractButton*>	list( _Mybase::buttons() );

            foreach ( QAbstractButton *b, list )
                b->setEnabled( !ignoreButtonPressed_ );
        }
    }

    // diagnostics error handler
    if (( Diagnostics == device ) && ( DiagCheckErrorHandler == event ))
    {
        DiagCheckErrorHandlerMsg m( msg );

        if (( code == ((m.device() * 1000) + m.errorCode()) ) && ( diagEvent_.isEmpty() ))
        {
            if ( !m.available() )
                LOG_TRACE( "no diagnostic wizard available", NULL );
            else if ( m.force() )
            {
                QTimer *timer( new QTimer( this ) );
                timer->setSingleShot( true );

                connect( timer, SIGNAL(timeout()), SLOT(onDiagnoseButtonClicked()) );

                timer->start( 0 );
            }
            else
            {
                QPushButton *button( new QPushButton( tr( "Diagnose" ), this ) );

                // fix text colors
                modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::gray ) );
                setForegroundPalette( button );

                connect( button, SIGNAL(clicked()), SLOT(onDiagnoseButtonClicked()) );

                buttonsArea_->insertStretch( 0 );
                buttonsArea_->insertWidget( 0, button );
            }

            // save off diag event
            diagEvent_ = m.event();
            diagEventForced_ = m.force();
        }
    }

    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onSessionMessageReceiveError( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "SessionErrorDialog", "OnSessionMessageReceiveError" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // error device dismiss dialog event
    if (( Error == device ) && ( ErrRemoveAnyModalDialogDevice == event ))
    {
        int code = msg[2].toInt();

        LOG_TRACE( "have error device %d event %d code %d", device, event, code );

        LOG_DEBUG( "msg: %d, msg_: %d", msg[2].toInt(), msg_[2].toInt() );

        // dismiss this dialog
        if ( code == msg_[2].toInt() )
        {
            LOG_DEBUG( "have rejection message for our dialog", NULL );

            // we can test right here what kind of dialog box are we closing, and it's message number, in case
            // we need to do some validation off of a dialog box, like indicating, we are done booting.

            // any dialog box that is dismissed will reset this.  Found too many codes that was an issue, reset for all of them
            internalConfigData_->selfServicePreventClicks = false;

            if( SessionDevError::SessionDevBrowserBootupCompleted == code )              // 100018
            {
                // indicates we just clicked OK on initialize complete on boot, now we can go full speed ahead
                // NOTE 11-13-2015: Just noticed, that if you click the background beside the dialog and not the
                // OK button, this method is not triggered. In the other method that is I still called,
                // sessionmainwindow::OnSessionMessageReceiveError, I set the scalebootcomplete to true there
                // but Im not testing for the missingPerifAtBoot

                internalConfigData_->scaleBootComplete = true;

                if ( ( internalConfigData_->needsNextGenWrapperUpgrade ) ||
                     ( internalConfigData_->needsWrapperUpgrade ) ||
                     ( internalConfigData_->needsCLAUpgrade ) ||
                     ( internalConfigData_->needsKPCLoaderUpgrade ) ||
                    ( internalConfigData_->needsKPCUpgraderUpgrade ) )
                {
                    mainWindow_->processFirmwareUpgrade();
                }

                QString modelName = session_->scaleConfig->getScaleModelName();
                if( modelName.contains( "NGW", Qt::CaseInsensitive ) )
                {
                    session_->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeAutoWeigh ) );
                    session_->send( SessionDev, SnReadWeighModeTransactionType );
                }
                else
                {
                    if ( internalConfigData_->didNotSendInitialReadWeighMode )
                    {
                        internalConfigData_->didNotSendInitialReadWeighMode = false;
                        session_->send( SessionDev, SnReadWeighModeTransactionType );
                    }
                }

                if ( internalConfigData_->isMissingPerifAtBoot )      // then we received a missing perif, 138,20, msg during boot
                {
                    internalConfigData_->isMissingPerifAtBoot = false;
                    mainWindow_->showRemovePeripherals();
                }
            }

            // when we are dismissed from the backend there is no need to send a removal
            // message back to them
            needToSendClosedMessage_ = false;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onSessionMessageReceiveLockDown( bool isLocked )
{
    LOG_CLASS_INFO( "SessionErrorDialog", "onSessionMessageReceiveLockDown" );

    if( isLocked )
    {
        // To Lock the screen, it indicates we have to set the screen being enabled to false
        // the if means we have previously received a 100,10,13 but scaleBootCompletes is not yet true

        if ( internalConfigData_->ssactive_tenative_lock )
        {
            LOG_DEBUG( "onSessionMessageReceiveLockDown: DISABLING MAIN WINDOW", NULL );

            mainWindow_->setEnabled( false );       // disable the mainwindow

            // 1: need to disable the Mode word string(ie Self Service String) on top bar that is still clickable
            //    but you cant really do anything if you do select it, nor the menu that is popable so....

            // 2: swiping is disabled on plumainscreen.cpp where the swipes are recogonized. uses ssactive_lock_tenative

            // 3: Disabling the silkscreen swipes off the screen below the touchscreen probably doesnt work?
        }
    }
    else
    {
        LOG_DEBUG( "onSessionMessageReceiveLockDown: ENABLING MAIN WINDOW", NULL );

        // to unlock the screen, then we have to set the screen being enabled to true
        mainWindow_->setEnabled( true );            // enable the mainwindow
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onWatchVideoButtonClicked()
{
    LOG_CLASS_INFO( "SessionErrorDialog", "onWatchVideoButtonClicked" );
    LOG_INFO( "watch video button pressed", NULL );

    // no video defined
    if ( videoHelp_.isEmpty() )
    {
        LOG_WARN( "empty video filename", NULL );
        return;
    }

    // show video
    LOG_WARN( "showing video '%s'", qPrintable( videoHelp_ ) );
    mainWindow_->showVideo( videoHelp_, 2500, true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onHookDialogButtonPress( int role )
{
    QList<QAbstractButton*> dialogButtons( buttons() );

    // figure out what was pressed
    foreach ( QAbstractButton *button, dialogButtons )
        if ( role == buttonRole( button ) )
        {
            onButtonClicked( button );
            return;
        }

    // role not found in button list... reject dialog
    reject();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onSessionMessageReceiveErrorInternal( const SessionMessage& msg )
{
    onSessionMessageReceiveError( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::onSessionMessageReceiveLockDownInternal( bool isLocked )
{
    onSessionMessageReceiveLockDown( isLocked );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::initialize()
{
    LOG_CLASS_INFO( "SessionErrorDialog", "initialize" );

    int code = msg_[2].toInt();
    int style = msg_[3].toInt();

    LOG_INFO( "msg[0] = %d", msg_[0].toInt() );
    LOG_INFO( "msg[1] = %d", msg_[1].toInt() );

    //-------------------------------------------------------------------------------------
    // SOF-3092: If Pre-Pack need to catch the first "Initialization complete ... " message
    //           and boot again, in order to clean-up printer not being ready
    //-------------------------------------------------------------------------------------
    if( session_->isPrepack() )
    {
        const QString wrapperSecondBootFile( "/etc/wrapperSecondBoot" );
        if ( !(QFile::exists( wrapperSecondBootFile ) ) )
        {
            python pythonProcess;
            QStringList args;

            pythonProcess.execCommand( "wrapperSecondBoot", args, false, false, (20 * 1000) );
        }
    }

    int paramCount = 0;
    QVector<QString> params;

    LOG_INFO( "popping session dialog code %d style %d", code, style );

    QString text( "" );
    if( ( ProductDevError::ProductErrForcedFieldNotEntered == code ) && ( session_->isSelfServiceMode() ) )     // 101046
        text = messageString( code, ScaleModel::ScaleModel_ESS );
    else if(  session_->isSelfServeAndPrintingToScreen() && checkForLabelErrors( code  ) )
        text = messageString( code, ScaleModel::ScaleModel_ESS );
    else
        text = messageString( code, ScaleModel::ScaleModel_EPP );

    QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton;

    // check for parameters
    if ( 6 <= msg_.size() )
    {
        paramCount = msg_[4].toInt();
        params = TokenParser::splitStringTokens( msg_[5] );

        if ( params.size() != paramCount )
        {
            LOG_WARN( "param count mismatch %d %d", paramCount, params.size() );
            paramCount = qMin( paramCount, params.size() );
        }

        if ( paramCount )
        {
            QString buffer;

            // add empty strings to increase array size
            while ( params.size() < 10 )
                params.push_back( "" );

            buffer.sprintf( text.toLatin1().constData(),
                params[0].toLatin1().constData(),
                params[1].toLatin1().constData(),
                params[2].toLatin1().constData(),
                params[3].toLatin1().constData(),
                params[4].toLatin1().constData(),
                params[5].toLatin1().constData(),
                params[6].toLatin1().constData(),
                params[7].toLatin1().constData(),
                params[8].toLatin1().constData(),
                params[9].toLatin1().constData() );

            if ( buffer.length() )
                text = buffer;
        }

        if( session_->scaleConfig->internalConfig.isOkToLookUpPlu  )
        {
            // must of been an error so reset it
            session_->scaleConfig->internalConfig.isOkToLookUpPlu = true;
            session_->scaleConfig->internalConfig.isOkDroppedPLU = 0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // video help

    if (( 9012 == code ) || ( 9013 == code ) ||
        ( 9538 == code ) ||
        ( 9590 == code ) || ( 9591 == code ) ||
        ( 9675 == code ) || ( 9676 == code ))
    {
        videoHelp_ = "NGW_LoadingFilm.mp4";
    }


    if ( videoHelp_.length() )
    {
        QPushButton *button = addButton( tr( "Watch Video" ), QDialogButtonBox::AcceptRole );

        connect( button, SIGNAL(clicked(bool)), SLOT(onWatchVideoButtonClicked()) );
    }

    ///////////////////////////////////////////////////////////////////////////
    // set buttons

    switch ( style )
    {
    // no dialog, a message instead of a dialog
    case 0:
    case 9:

        // text message without buttons, no messages required
        allowDismiss_ = true;
        needToSendClosedMessage_ = false;

        break;

    // dialog box with OK and CANCEL buttons
    case 1:
    case 2:

        //buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
        addButton( tr( "OK" ), QDialogButtonBox::AcceptRole );
        addButton( tr( "Cancel" ), QDialogButtonBox::RejectRole );

        break;

    // dialog with no buttons and cannot be dismissed
    case 3:

        // this dialog dismissed from backend, no messages required
        needToSendClosedMessage_ = false;
        needToForwardMessage_ = true;

        break;

    // new plu dialog
    case 4:

        // Note:
        // This error is not handled here... instead we are passing it onto the screen for
        // handling. This case statement should never be hit! If it is, something is wrong in
        // a lower layer it should have been trapped.

        LOG_ERROR( "got create PLU dialog error!", NULL );
        valid_ = false;

        break;

    // dialog box with only an OK button
    case 5:
    case 7:

        //buttons = QDialogButtonBox::Ok;
        addButton( tr( "OK" ), QDialogButtonBox::AcceptRole );

        // when operator dismiss we accept
        defaultRole_ = QDialogButtonBox::AcceptRole;
        allowDismiss_ = true;

        break;

    // dialog box with only a CANCEL button
    case 6:
    case 8:

        //buttons = QDialogButtonBox::Cancel;
        addButton( tr( "Cancel" ), QDialogButtonBox::RejectRole );

        // when operator dismiss we reject
        defaultRole_ = QDialogButtonBox::RejectRole;

        // SOF-3347: Do not allow dismiss on "Press the calibration switch" message
        if( code != 1010 )
            allowDismiss_ = true;

        break;

    // dialog box with REZERO and IGNORE
    case 11:

        addButton( tr( "Rezero" ), QDialogButtonBox::AcceptRole );
        addButton( tr( "Ignore" ), QDialogButtonBox::RejectRole ); // HA-13

        // when operator dismiss we reject
        defaultRole_ = QDialogButtonBox::RejectRole;
        allowDismiss_ = true;

        break;

    // dialog box with REBOOT and SHUTDOWN
    case 12:

        addButton( tr( "Reboot" ), QDialogButtonBox::AcceptRole );
        addButton( tr( "Shutdown" ), QDialogButtonBox::RejectRole );
        break;

    // dialog box requesting to recalibrate in KG or LBs
    case 13:

        addButton( tr( "Pounds" ), QDialogButtonBox::AcceptRole );
        addButton( tr( "Kilograms" ), QDialogButtonBox::RejectRole );
        break;

    // dialog box with UPDATEANDPRINT and CANCEL
    case 14:

        addButton( tr( "Update and Print" ), QDialogButtonBox::AcceptRole );
        addButton( tr( "Cancel" ), QDialogButtonBox::RejectRole );
        //addButton( QDialogButtonBox::Cancel );

        // when operator dismiss we reject
        defaultRole_ = QDialogButtonBox::RejectRole;
        allowDismiss_ = true;

        break;

    // dialog box with UPDATEONLY and CANCEL
    case 15:

        addButton( tr( "Update Only" ), QDialogButtonBox::AcceptRole );
        addButton( tr( "Cancel" ), QDialogButtonBox::RejectRole );
        //addButton( QDialogButtonBox::Cancel );

        // when operator dismiss we reject
        defaultRole_ = QDialogButtonBox::RejectRole;
        allowDismiss_ = true;

        break;

    // dialog box with UPDATEANDPRINT and UPDATEONLY and CANCEL
    case 16:

        addButton( tr( "Update and Print" ), QDialogButtonBox::AcceptRole );
        addButton( tr( "Update Only" ), QDialogButtonBox::ApplyRole );
        addButton( tr( "Cancel" ), QDialogButtonBox::RejectRole );
        //addButton( QDialogButtonBox::Cancel );

        // when operator dismiss we reject
        defaultRole_ = QDialogButtonBox::RejectRole;
        allowDismiss_ = true;

        break;

    // dialog box with REZERO
    case 17:
        addButton( tr( "Rezero" ), QDialogButtonBox::AcceptRole );
        break;

    // dialog box with REBOOT and CANCEL
    case 18:

        addButton( tr( "Reboot" ), QDialogButtonBox::AcceptRole );
        addButton( tr( "Cancel" ), QDialogButtonBox::RejectRole );
        //addButton( QDialogButtonBox::Cancel );

        // when operator dismiss we reject
        defaultRole_ = QDialogButtonBox::RejectRole;
        allowDismiss_ = true;

        break;


    default:
        valid_ = false;
    }

    ///////////////////////////////////////////////////////////////////////////
    // init base dialog class

    // special case for platter tare, they must press okay
    if ( 101043 == code )
        allowDismiss_ = false;

    // Note:
    // As per Debra and Scott, the session errors should not display a warning icon. Originally
    // Jim said they _always_ should do this... but apparently this is not so. I am commenting
    // this for now. If, in the future (hello future selves *wave*!), they want this back simply
    // uncomment the below line.
    //setIcon( QPixmap( ":/images/dialogs/warningIcon.png" ) );

    // special case for operator note
    if ( 102007 == code )
        setTitle( tr( "Operator Note" ) );
    else
    {
        setTitle( tr( "System Message" ) );
    }

    setText( text );

    if ( QDialogButtonBox::NoButton != buttons )
        setStandardButtons( buttons );

    setSizeToFit( true );

    ///////////////////////////////////////////////////////////////////////////
    // request diagnostics

    session_->send( Diagnostics, DiagCheckErrorHandlerReq, QString::number( code / 1000 ) + "," + QString::number( code % 1000 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionErrorDialog::sendClosedMessage( QDialogButtonBox::ButtonRole role )
{
    LOG_CLASS_INFO( "SessionErrorDialog", "sendClosedMessage" );

    if ( !needToSendClosedMessage_ )
        return;

    SessionMessage ui;
    SessionMessage backend;

    // we need to perform two things here:
    // 1) update the session handler with a proper 103,x message with what button was pressed
    // 2) send 103,255 message to backend

    ui.resize( 8 );
    backend.resize( 5 );

    ui[0] = msg_[0];
    ui[1] = msg_[1];
    ui[2] = msg_[2];
    ui[4] = msg_[3];

    backend[0] = msg_[0];
    backend[1] = QString::number( ErrRemoveAnyModalDialogDevice );
    backend[2] = msg_[2];
    backend[3] = msg_[3];

    LOG_INFO( "handling role %d", role );

    switch ( role )
    {
    case QDialogButtonBox::AcceptRole:
        ui[7] = "1";
        backend[4] = "1";
        break;
    case QDialogButtonBox::RejectRole:
        ui[7] = "0";
        backend[4] = "0";
        break;
    case QDialogButtonBox::ApplyRole:
        ui[7] = "0";
        backend[4] = "2";
        break;
    default:
        LOG_WARN( "unhandled button role %d", role );
        break;
    }

    LOG_TRACE( "sending message to backend...", NULL );
    session_->send( backend );

    LOG_TRACE( "sending message to ui...", NULL );
    session_->handleMessage( ui );

    needToSendClosedMessage_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString SessionErrorDialog::printerType( int code )
{
    switch ( code )
    {
    case 0:
        return tr( "Primary Printer" );
    case 1:
        return tr( "Second Printer" );
    case 2:
        return tr( "Third Printer" );
    default:
        break;
    }

    return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int &SessionErrorDialog::prKind()
{
    static int thePrKind = 0;
    return thePrKind;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionErrorDialog::checkForLabelErrors( int code )
{
    bool retval = false;

    if( 101003 == code || 101005 == code || 101006 == code || 101016 == code || 101019 == code || 101051 == code )
        retval = true;

    return retval;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <>
QString SessionErrorDialog::messageStringT<0>( int code, int isScale )
{
    Q_UNUSED( code )
    Q_UNUSED( isScale )

    return SessionErrorDialog::tr( "%s" );
}

template <>
QString SessionErrorDialog::messageStringT<ErrWeigherDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 1000:
        return SessionErrorDialog::tr( "Unknown weigher error." );
    case 1001:
        return SessionErrorDialog::tr( "Weigher stopped communicating.\nCheck to be sure the weigher cable is plugged in." );
    case 1002:
        return SessionErrorDialog::tr( "Negative gross weight." );
    case 1003:
        return SessionErrorDialog::tr( "Over gross weight." );
    case 1004:
        return SessionErrorDialog::tr( "The weigher has been detected and is now going through its warmup procedure, please wait." );
    case 1005:
        return SessionErrorDialog::tr( "Scale Rezero\nRezero in progress." );
    case 1006:
        return SessionErrorDialog::tr( "Scale Rezero\nScale zero is out of maintenance zone. Remove all weight from the platter and press the 'REZERO' button." );
    case 1007:
        return SessionErrorDialog::tr( "Outside zero maintenance zone." );
    case 1008:
        return SessionErrorDialog::tr( "Remove the weight from platter." );
    case 1009:
        return SessionErrorDialog::tr( "Select the type of weights you will be using to calibrate the weigher." );
    case 1010:
        return SessionErrorDialog::tr( "Press the calibrate switch." );
    case 1011:
        return SessionErrorDialog::tr( "Delta change is too big." );
    case 1012:
        return SessionErrorDialog::tr( "Weigher calibration succeeded." );
    case 1013:
        return SessionErrorDialog::tr( "Weigher not attached." );
    case 1014:
        return SessionErrorDialog::tr( "Scale Not Zero\nRemove the weight from the platter or press the 'REZERO' button." );
    case 1015:
        return SessionErrorDialog::tr( "Invalid station address. Resetting." );
    case 1016:
        return SessionErrorDialog::tr( "The Weigher dropped offline.\nCheck to be sure the weigher cable is plugged in." );
    case 1017:
        return SessionErrorDialog::tr( "You must remove all weight from the platter and then enter a value for the tare before you can print a label." );
    case 1018:
        return SessionErrorDialog::tr( "A weigher configuration value is out of range.  Contact Hobart Service to troubleshoot the scale." );
    case 1019:
        return SessionErrorDialog::tr( "Weigher rezero limit exceeded, the weight on the platter is too great." );
    case 1020:
        return SessionErrorDialog::tr( "Weigher is being updated remotely.  Please wait ..." );
    case 1021:
        return SessionErrorDialog::tr( "The Weigher must be level to Calibrate.\n\nWeigher is more than 1/2 degree out of level.\n\nPlease level the scale to Calibrate.\n\nTo level the scale please go to the Menu > Service > Weigher Information Screen and adjust the X and Y tilt under 1/2 degree." );
    case 1022:
        return SessionErrorDialog::tr( "Weigher has encountered a Value Max EEP Communications Failure.\n\nPlease remove all weight from the platter and press the 'REBOOT' button or Click 'CANCEL'." );
    case 1023:
        return SessionErrorDialog::tr( "Weigher has encountered a Value Max EEP Checksum Failure.\n\n" );
    case 1024:
        return SessionErrorDialog::tr( "Weigher has encountered a Value Max EEP Acceleration Communications Failure.\n\nPlease remove all weight from the platter and press the 'REBOOT' button or Click 'CANCEL'." );
    case 1025:
        return SessionErrorDialog::tr( "The Scale exceeds 3 degrees of tilt.\n\nPlease level your scale or contact Hobart Service." );
    case 1501:
        return SessionErrorDialog::tr( "Weigher was disconnected.\nThis scale is out of service.  Please contact store personnel." );
    case 1502:
        return SessionErrorDialog::tr( "Negative gross weight.\nThis scale is out of service.  Please contact store personnel." );
    case 1503:
        return SessionErrorDialog::tr( "Over gross weight.\nThis scale is out of service.  Please contact store personnel." );
    case 1507:
        return SessionErrorDialog::tr( "Outside zero maintenance zone.\nThis scale is out of service.  Please contact store personnel." );
    case 1511:
        return SessionErrorDialog::tr( "Weigher not attached.\nThis scale is out of service.  Please contact store personnel." );
    case 1519:
        return SessionErrorDialog::tr( "Weigher rezero limit exceeded, the weight on the platter is too great." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrOperationModeDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
//	Begin OpModeErrorSource errors
    case OperationModeError::NoErrorSource:
        return SessionErrorDialog::tr( "\n\nUnknown OperateMode Error: 4000." );
    case OperationModeError::WrapperPressStart:
        return SessionErrorDialog::tr( "\n\nPress the wrapper START button." );
    case OperationModeError::WrapperIdleError:
        return SessionErrorDialog::tr( "Wrapper is idle.\nRemove all packages from the wrapper.\nPress the \"CANCEL\" button to ignore this warning." );
    case OperationModeError::WrapperGhostPackage:
        return SessionErrorDialog::tr( "The item on the scale does not \nweigh enough to print or scale Not Zero\nThe Weigher platter may not be clean.\nPlease clear platter of any loose product\nor press the 'Rezero' button." );
    case OperationModeError::WrapperOverWeight:
        return SessionErrorDialog::tr( "Package on wrapper intake is too heavy.\n\nGross package weight must be less than 15.00 lb or 6.820 kg." );
    case OperationModeError::PrinterOverflowFirstPrinter:
        return SessionErrorDialog::tr( "Primary printer overflow error.\nPrimary printer labels removed.\n\nPress the \"CANCEL\" button to continue." );
    case OperationModeError::PrinterOverflowSecondPrinter:
        return SessionErrorDialog::tr( "Second printer overflow error.\nSecond printer labels removed.\n\nPress the \"CANCEL\" button to continue." );
    case OperationModeError::PrinterOverflowThirdPrinter:
        return SessionErrorDialog::tr( "Third printer overflow error.\nThird printer labels removed.\n\nPress the \"CANCEL\" button to continue." );
    case OperationModeError::InsertServiceKey:
        return SessionErrorDialog::tr( "Insert the Service key." );
    case OperationModeError::LaPackageOverrun:
        return SessionErrorDialog::tr( "Unused Message" );
    case OperationModeError::WrapperTestEncoderWindow_1_Failed:
        return SessionErrorDialog::tr( "This Wrapper Test is invalid for this encoder count.\n\nValid encoder count range: 11 to 119" );
    case OperationModeError::WrapperTestEncoderWindow_2_Failed:
        return SessionErrorDialog::tr( "This Wrapper Test is invalid for this encoder count.\n\nValid encoder count range: 66 to 24" );
    case OperationModeError::WrapperTestEncoderWindow_3_Failed:
        return SessionErrorDialog::tr( "This Wrapper Test is invalid for this encoder count.\n\nValid encoder count range: 245 to 100" );
    case OperationModeError::EvolWrapperHoming:
        return SessionErrorDialog::tr( "The Wrapper is Homing!!\n\nPlease Wait!!" );
    case OperationModeError::ProductionEntryProductsEntered:
        return SessionErrorDialog::tr( "You have selected products for the current Production Entry.  Leaving Production Entry mode will not print labels for these products.\nPress OK to leave Production Entry mode, or\nPress CANCEL to remain in Production Entry mode." );
    case OperationModeError::EvolWrapperCalPlacePackage:
        return SessionErrorDialog::tr( "Calibrate Wrapper\n\nPlease place the package on the Primary intake." );
    case OperationModeError::PkgDischargeErrorFirstPrinter:
        return SessionErrorDialog::tr( "FAILED TO TAKE LABEL.\nPackages may not be labeled correctly.\n\nPrimary Printer" );
    case OperationModeError::PkgDischargeErrorSecondPrinter:
        return SessionErrorDialog::tr( "FAILED TO TAKE LABEL.\nPackages may not be labeled correctly.\n\nSecond Printer" );
    case OperationModeError::PkgDischargeErrorThirdPrinter:
        return SessionErrorDialog::tr( "FAILED TO TAKE LABEL.\nPackages may not be labeled correctly.\n\nThird Printer" );
    case OperationModeError::ProductRequiredToWrap:
        return SessionErrorDialog::tr( "Please select a product in order to process the package." );	// HA-65
    case OperationModeError::BuildingAirPressure:
        return SessionErrorDialog::tr( "Wrapper building air pressure.\n\nPlease wait." );

//	End of OpModeErrorSource errors - Then what are these?
    case 4101:
        return SessionErrorDialog::tr( "Select a product or category" );
    case 4102:
        return SessionErrorDialog::tr( "Select a product or category" );
    case 4103:
        return SessionErrorDialog::tr( "Please press print key" );
    case 4104:
        return SessionErrorDialog::tr( "Please place your item on the scale" );
    case 4105:
        return SessionErrorDialog::tr( "Select a product or category" );
    case 4106:
        return SessionErrorDialog::tr( "How many of these are you buying?" );
    case 4107:		// NOTE: This used to be msg4019, but it did not exist in OpModeErrorSource [CCD Jan 2015].
        return SessionErrorDialog::tr( "Please select the wrapper model" );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrLabelApplierDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 5001:
        return SessionErrorDialog::tr( "Label Applier stopped communicating.\n\nCheck to be sure the label applier cable is plugged in." );
    case 5002:
        return SessionErrorDialog::tr( "The Label Applier power switch is \"OFF\".\n\nTurn this switch \"ON\"." );
    case 5003:
        return SessionErrorDialog::tr( "The First Printer Arm is out of position.\n\nLock this Arm into position." );
    case 5004:
        return SessionErrorDialog::tr( "The Second Printer Arm is out of position. \n\nLock this Arm into position." );
    case 5005:
        return SessionErrorDialog::tr( "The First Printer Station is not responding.\n\nCheck the station power switch." );
    case 5006:
        return SessionErrorDialog::tr( "The Second Printer Station is not responding.\n\nCheck the station power switch." );
    case 5007:
        return SessionErrorDialog::tr( "A hardware error (Weigher) has occurred! If this error continues to occur, service may be required.\n\nPress \"CANCEL\" to continue." );
    case 5008:
        return SessionErrorDialog::tr( "A hardware error (Printer1) has occurred! If this error continues to occur, service may be required.\n\nPress \"CANCEL\" to continue." );
    case 5009:
        return SessionErrorDialog::tr( "A hardware error (Printer2) has occurred! If this error continues to occur, service may be required.\n\nPress \"CANCEL\" to continue." );
    case 5010:
        return SessionErrorDialog::tr( "Packages may not be labeled.\n\nPress OK to continue." );
    case 5011:
        return SessionErrorDialog::tr( "REMOVE ALL PACKAGES\n\"Semi-Auto\" modes do not permit wrapper use.\nChange to \"Auto\" mode.\n\nPress OK to continue." );
    case 5012:
        return SessionErrorDialog::tr( "The Label Applier dropped offline.\n\nCheck to be sure the label applier cable is plugged in." );
    case 5013:
        return SessionErrorDialog::tr( "Packages may not be labeled.\n\nPress OK to continue." );
    case 5014:
        return SessionErrorDialog::tr( "Packages may not be labeled.\n\nPress OK to continue." );
    case 5015:
        return SessionErrorDialog::tr( "Packages may not be labeled correctly.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 5016:
        return SessionErrorDialog::tr( "Deleting all CLA Zip files from the firmware directory." );
    case 5017:
        return SessionErrorDialog::tr( "CLA Zip files have been Deleted." );
    case 5018:
        return SessionErrorDialog::tr( "Motor 1 Bad Hall Readings" );
    case 5019:
        return SessionErrorDialog::tr( "Weigher Conveyor Jammed\n\nPress the STOP button.\nRemove any packages, labels, debris in the machine.\nPress the START button." );
    case 5020:
        return SessionErrorDialog::tr( "Weigher Conveyor Stalled" );
    case 5021:
        return SessionErrorDialog::tr( "Weigher Package Sensor is blocked" );
    case 5022:
        return SessionErrorDialog::tr( "Motor 2 Bad Hall Readings" );
    case 5023:
        return SessionErrorDialog::tr( "CLA Conveyor Jammed\n\nPress the STOP button.\nRemove any packages, labels, debris in the machine.\nPress the START button." );
    case 5024:
        return SessionErrorDialog::tr( "CLA Conveyor Stalled" );
    case 5025:
        return SessionErrorDialog::tr( "CLA Package Sensor is blocked" );
    case 5026:
        return SessionErrorDialog::tr( "Applier Package Sensor is blocked" );
    case 5027:
        return SessionErrorDialog::tr( "Motor 1 Fuse May be Blown" );
    case 5028:
        return SessionErrorDialog::tr( "Motor 2 Fuse May be Blown" );
    case 5029:
        return SessionErrorDialog::tr( "Wakeup Sensor is blocked" );
    case 5030:
        return SessionErrorDialog::tr( "Package Gate Switch is tripped" );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrPrinterDevice>( int code, int isScale )
{
    QString printer;
    const int FSSPrinter = 12; // from ScaleClasses, 12th item in PrinterKind enum
    const int prKind = getPrKindValue();

    // Note:
    // In the original implementation (i.e. errorMessage.java) codes less than 7500 would append a
    // printer type onto the message.

    if (( 7000 < code ) && ( code < 7500 ))
    {
        int printerTypeCode = (code - 7000) / 100;

        printer = printerType( printerTypeCode );
        code -= (printerTypeCode * 100);
    }

    switch ( code )
    {
    case 7001:
        return printer + ": " + SessionErrorDialog::tr( "Printer communication lost. Check printer cable." );
    case 7002:
        return printer + ": " + SessionErrorDialog::tr( "Printer turned off." );
    case 7003:
    {
        if(prKind == FSSPrinter)
            return SessionErrorDialog::tr( "Printhead up - This scale is out of service.\nThe Printhead must be engaged to continue printing.\nPlease contact store personnel."  );
        else
            return printer + ": " + SessionErrorDialog::tr( "Label cassette open.\n\nInsert label cassette to continue printing.\n\nPlease always use the original printer cassette that was purchased with the scale.\n\nPlease remember to clean the printhead." );
    }
    case 7004:
        return printer + ": " + SessionErrorDialog::tr( "Printer label sizing." );
    case 7005:
        return printer + ": " + SessionErrorDialog::tr( "Take the label and place it on the package." );
    case 7006:
        return printer + ": " + SessionErrorDialog::tr( "Take the label and place it on the package." );
    case 7007:
    {
        if( isScale )
            return printer + ": " + SessionErrorDialog::tr( "Label jammed." );
        else
            return printer + ": " + SessionErrorDialog::tr( "Label jammed.  Check labels and cycle printhead lever." );
    }
    case 7008:
        return printer + ": " + SessionErrorDialog::tr( "The label is missing.\nPlease attempt to print the item again." );
    case 7009:
        return printer + ": " + SessionErrorDialog::tr( "Out of labels." );
    case 7010:
        return printer + ": " + SessionErrorDialog::tr( "Label cassette missing." );
    case 7011:
        return printer + ": " + SessionErrorDialog::tr( "Printer hardware error." );
    case 7012:
        return printer + ": " + SessionErrorDialog::tr( "Thermal overload print error." );
    case 7013:
        return printer + ": " + SessionErrorDialog::tr( "Printer shutdown due to thermal overload." );
    case 7014:
        return printer + ": " + SessionErrorDialog::tr( "Invalid setup. Resetting." );
    case 7015:
        return printer + ": " + SessionErrorDialog::tr( "Calibrate media sensor\n\nDisengage the printhead." );
    case 7016:
        return printer + ": " + SessionErrorDialog::tr( "Calibrate media sensor\n\nPosition the label stock so there is only backing paper without a label on it and no black bar covering the media sensor." );
    case 7017:
        return printer + ": " + SessionErrorDialog::tr( "Calibrate media sensor\n\nEngage the printhead." );
    case 7018:
        return printer + ": " + SessionErrorDialog::tr( "Calibrate media sensor\n\nTurn the supply roll clockwise until the backing paper under the media sensor is taut. Hold in this position until calibration is complete." );
    case 7019:
        return printer + ": " + SessionErrorDialog::tr( "Calibrate media sensor\n\nCalibrating." );
    case 7020:
        return printer + ": " + SessionErrorDialog::tr( "Calibration complete.\n\nPrinter will size the label stock." );
    case 7021:
        return printer + ": " + SessionErrorDialog::tr( "Calibrate label taken sensor\n\nMake sure that the label taken sensor is clear from all items." );
    case 7022:
        return printer + ": " + SessionErrorDialog::tr( "Calibrate label taken sensor\n\nCalibrating." );
    case 7023:
        return printer + ": " + SessionErrorDialog::tr( "Calibrate label taken sensor\n\nCalibration is complete." );
    case 7024:
        return printer + ": " + SessionErrorDialog::tr( "\nThe Security label applier is off." );
    case 7025:
        return printer + ": " + SessionErrorDialog::tr( "\nThe Security label applier is out of labels." );
    case 7026:
        return printer + ": " + SessionErrorDialog::tr( "Printer lever forward.\n\nMove printer lever back\nto engage the printhead." );
    case 7027:
        return SessionErrorDialog::tr( "Invalid character in printer name.\nThe printer name may not contain a space in the name." );
    case 7028:
        return SessionErrorDialog::tr( "Invalid character in printer name. The following characters are invalid: space comma \\n @ ` ~ ! # $ ^ & * ( ) = + [ ] { } \\ | ; : \\ , < > / ?" );
    case 7029:
        return SessionErrorDialog::tr( "\nContinuous label stock\ndoes not require sizing." );
    case 7030:
        return SessionErrorDialog::tr( "\nContinuous label stock detected.\nIf this is correct, press OK.\n\nIf not correct, press CANCEL, then remove and reinsert the label cassette." );
    case 7031:
        return SessionErrorDialog::tr( "\nLabel Cutter Jammed.\n\nCheck the cutter for jammed or bound label stock." );
    case 7032:
        return SessionErrorDialog::tr( "\nLabel Cutter Interlock Open.\n\nClose panel on the front of the cutter.\n\nSnap the Cutter back in place when finished." );
    case 7033:
        return SessionErrorDialog::tr( "\nLabel Cutter Timeout.\n\nThe command to engage the label cutter timed out." );
    case 7034:
        return SessionErrorDialog::tr( "Print job has been canceled - Interlock is open.\n\nNumber of labels to print: %s\nNumber of labels printed: %s\nNumber of labels remaining: %s" );
    case 7035:
        return SessionErrorDialog::tr( "Warning! Label stock is running out." );
    case 7036:
        return SessionErrorDialog::tr( "Out of Label stock.\n\nThis scale is out of service.\n\nPlease contact store personnel." );
    case 7037:
        return SessionErrorDialog::tr( "\nLabel Cutter Interlock Open.\n\nClose side door when finished.\n\nCutter cannot operate while door is open." ); //SOF-5065
    case 7038:
        return SessionErrorDialog::tr( "\nPrinter Door Interlock Open.\n\nClose side door when finished." ); //SOF-5060
    case 7126:
        return printer + ": " + SessionErrorDialog::tr( "Printer lever forward.\n\nMove printer lever back\nto engage the printhead." );
    case 7501:
        return SessionErrorDialog::tr( "Printer communication lost. Check printer cable.\nThis scale is out of service.  Please contact store personnel." );
    case 7502:
        return SessionErrorDialog::tr( "Printer turned off.\nThis scale is out of service.  Please contact store personnel." );
    case 7503:
    {
        if(prKind == FSSPrinter)
            return SessionErrorDialog::tr("Printhead up-This scale is out of service.\nThe Printhead must be engaged to continue printing.\nPlease contact store personnel.");
        else
            return SessionErrorDialog::tr( "Label cassette open - This scale is out of service.\nInsert the label cassette to continue printing or please contact store personnel." );
    }
    case 7505:
        return SessionErrorDialog::tr( "Take the label and place it on the package." );
    case 7506:
        return SessionErrorDialog::tr( "Take the label and place it on the package." );
    case 7507:
        return SessionErrorDialog::tr( "Label jammed.\nThis scale is out of service.  Please contact store personnel." );
    case 7508:
        return SessionErrorDialog::tr( "The label is missing.\nPlease attempt to print the item again. \nContact store personnel if the problem continues." );
    case 7509:
        return SessionErrorDialog::tr( "Out of labels.\nThis scale is out of service.  Please contact store personnel." );
    case 7510:
        return SessionErrorDialog::tr( "Label cassette missing.\nThis scale is out of service.  Please contact store personnel." );
    case 7511:
        return SessionErrorDialog::tr( "Printer hardware error.\nThis scale is out of service.  Please contact store personnel." );
    case 7512:
        return SessionErrorDialog::tr( "Thermal overload print error.\nThis scale is out of service.  Please contact store personnel." );
    case 7513:
        return SessionErrorDialog::tr( "Printer shutdown due to thermal overload.\nThis scale is out of service.  Please contact store personnel." );
    case 7557:
        return SessionErrorDialog::tr( "Label \ncassette open.\n\nInsert \n\nlabel cassette to \ncontinue \n\nprinting.\nThis scale \n\nis \nout of \nservice.  Please contact store personnel." );
    case 7558:
        return SessionErrorDialog::tr( "Two external EPP printers cannot be selected at the same time." );
    case 7559:
        return SessionErrorDialog::tr( "A network printer is not attached to print the document." );
    case 7560:
        return SessionErrorDialog::tr( "The command to print the web page failed." );
    case 7561:
        return SessionErrorDialog::tr( "No file loaded to print." );
    case 7562:
        return SessionErrorDialog::tr( "The selected file was sent to the printer." );
    case 7563:
        return SessionErrorDialog::tr( "A invalid network printer was selected to print the document." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrWrapperDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    // Note:
    // In the original implementation (i.e. errorMessage.java) codes [9002, 9028] would append the error code.
    switch ( code )
    {
    case 9001:
        return SessionErrorDialog::tr( "The wrapper was disconnected.\n\nCheck to be sure the wrapper cable is plugged in." );
    case 9002:
        return SessionErrorDialog::tr( "MACHINE IDLE\n\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\n PRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9003:
        return SessionErrorDialog::tr( "EEP write error on wrapper.\n\nCall Service." ) + "\n\n" + QString::number( code );
    case 9004:
        return SessionErrorDialog::tr( "REMOVE PACKAGES FROM WRAPPER\n-Check package length sensor for debris that may be holding it down\n-Check package centering arms and pivots for debris that may be holding them open\n-Check scale for ZERO\n-REMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\n-PRESS AND RELEASE THE STOP SWITCH\n-PRESS GREEN START BUTTON\n-or PRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9005:
        return SessionErrorDialog::tr( "REMOVE PACKAGES FROM WRAPPER\n-Check package length sensor for debris that may be holding it down\n-Check package centering arms and pivots for debris that may be holding them open\n-Check scale for ZERO\n-REMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\n-PRESS AND RELEASE THE STOP SWITCH\n-PRESS GREEN START BUTTON\n-or PRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9006:
        return SessionErrorDialog::tr( "WRAPPER MAIN MOTOR STALLED\n\nTurn handwheel counterclockwise to manually move through a complete machine cycle\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9007:
        return SessionErrorDialog::tr( "WRAPPER INTAKE PUSHER JAMMED\n\nClean intake pusher, clear any debris from the elevator and the clean out pan.\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9008:
        return SessionErrorDialog::tr( "Wrapper length switch error." ) + "\n\n" + QString::number( code );
    case 9009:
        return SessionErrorDialog::tr( "Wrapper width switch error." ) + "\n\n" + QString::number( code );
    case 9010:
        return SessionErrorDialog::tr( "WRAPPER INTAKE PUSHER UNABLE TO FIND HOME\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9011:
        return SessionErrorDialog::tr( "WRAPPER PACKAGE JAMMED\n\nTurn handwheel clockwise to free jammed package, clear any debris from the elevator, film selector, gripper, side clamps, and underfolder rollers\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9012:
        return SessionErrorDialog::tr( "WRAPPER DID NOT PULL WIDE FILM\n\nCheck the wide film supply and film threading, clean the gripper pads, side clamp pads, and film selector rollers\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9013:
        return SessionErrorDialog::tr( "Wrapper did not pull narrow film.\nCheck narrow film supply." ) + "\n\n" + QString::number( code );
    case 9014:
        return SessionErrorDialog::tr( "WRAPPER RIGHT SIDE COVER OPEN\n\nReposition right side cover\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9015:
        return SessionErrorDialog::tr( "WRAPPER LEFT SIDE COVER OPEN\n\nReposition left side cover\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9016:
        return SessionErrorDialog::tr( "WRAPPER CLEAN OUT PAN OPEN\n\nRemove clean out pan, clear debris\nclose clean out pan\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9017:
        return SessionErrorDialog::tr( "Wrapper top cover is open.\nClose top cover." ) + "\n\n" + QString::number( code );
    case 9018:
        return SessionErrorDialog::tr( "Wrapper top cover is open.\nClose top cover." ) + "\n\n" + QString::number( code );
    case 9019:
        return SessionErrorDialog::tr( "PACKAGE ON WRAPPER INTAKE IS TOO TALL\n\nMaximum package height is 5.25 inches\nEnsure high package panel is installed properly\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9020:
        return SessionErrorDialog::tr( "FILM SUPPLY DOOR OPEN\n\nClose the rear film supply door\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9021:
        return SessionErrorDialog::tr( "Wrapper handwheel door is open.\nClose handwheel door." ) + "\n\n" + QString::number( code );
    case 9022:
        return SessionErrorDialog::tr( "WEIGHER PLATTER IS NOT SEATED\n\nCheck that the 3 weigher platters are installed correctly with magnets to rear of wrapper.\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen." ) + "\n\n" + QString::number( code );
    case 9023:
        return SessionErrorDialog::tr( "WRAPPER ELEVATOR IS COLLAPSED\n\nOpen the top cover, lift the white elevator assembly until it locks in the up position\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\nPRESS {OK} to return to the previous screen." ) + "\n\n" + QString::number( code );
    case 9024:
        return SessionErrorDialog::tr( "WRAPPER SEALER BELT IS NOT IN DOWN AND LOCKED POSITION\nTo lower the sealer belt, open the top cover, support the sealer belt with the black rollers, lift the latch release lever and gently lower the sealer into the locked position\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\n PRESS {OK} to return to the previous screen" ) + "\n\n" + QString::number( code );
    case 9025:
        return SessionErrorDialog::tr( "General fault detected on wrapper sealer belt." ) + "\n\n" + QString::number( code );;
    case 9026:
        return SessionErrorDialog::tr( "General fault detected on wrapper intake." ) + "\n\n" + QString::number( code );;
    case 9027:
        return SessionErrorDialog::tr( "PACKAGE ON WRAPPER INTAKE IS TOO LONG\n\nMaximum package length is 10 inches\n\nREMOVE ALL PACKAGES AND LABELS FROM THE SYSTEM\nPRESS AND RELEASE THE STOP SWITCH\nPRESS GREEN START BUTTON\nor\n PRESS {OK} to return to the previous screen." ) + "\n\n" + QString::number( code );;
    case 9028:
        return SessionErrorDialog::tr( "Heater hardware error on wrapper.\nCall Service." ) + "\n\n" + QString::number( code );
    case 9100:
        return SessionErrorDialog::tr( "The last package cycled does not fit into this Pack code." );
    case 9200:
        return SessionErrorDialog::tr( "Enter the new packcode name." );
    case 9201:
        return SessionErrorDialog::tr( "This will reset the wrapper's configuration.\n\n Press OK to continue." );
    case 9501:
        return SessionErrorDialog::tr( "Wrapper status\n\nThe wrapper stopped communicating.\nCheck to be sure the wrapper cable is plugged in.\n\nPress the STOP button, then press the START button to home the machine." );
    case 9502:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary intake sensor or belt failure.\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\nIf problem persists please call service." );
    case 9503:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary intake motor failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9504:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary intake slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9505:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary intake motor failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9506:
        return SessionErrorDialog::tr( "Wrapper status\n\nHeight sensor calibration failure.\n\nPress the STOP button. Turn off circuit breaker, clean camera's, light tube and height sensors. \n\nRemove any packages, labels, debris in the machine. Turn on circuit breaker when complete." );
    case 9507:
        return SessionErrorDialog::tr( "Wrapper status\n\nRear Folder belt or sensor failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9508:
        return SessionErrorDialog::tr( "Wrapper status\n\nRear Folder motor electrical failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9509:
        return SessionErrorDialog::tr( "Wrapper status\n\nRear Folder slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9510:
        return SessionErrorDialog::tr( "Wrapper status\n\nFront Folder belt or sensor failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9511:
        return SessionErrorDialog::tr( "Wrapper status\n\nFront Folder motor electrical failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9512:
        return SessionErrorDialog::tr( "Wrapper status\n\nSealer Belt motor electrical failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9513:
        return SessionErrorDialog::tr( "Wrapper status\n\nPusher sensor or belt failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9514:
        return SessionErrorDialog::tr( "Wrapper status\n\nPusher motor electrical failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9515:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary conveyor not at home.\n\nPress the START button to home the machine." );
    case 9516:
        return SessionErrorDialog::tr( "Wrapper status\n\nCamera and height sensors blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine.\n\nCheck for package too far forward. Clean both cameras, light tube, and height sensors.\n\nPress the START button." );
    case 9517:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary intake jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9518:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary conveyor did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9519:
        return SessionErrorDialog::tr( "Wrapper status\n\nIntake blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9520:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary timing error.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9521:
        return SessionErrorDialog::tr( "Wrapper status\n\nMeasurement Error.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9522:
        return SessionErrorDialog::tr( "Wrapper status\n\nIntake sensor error.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9523:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary intake jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9524:
        return SessionErrorDialog::tr( "Wrapper status\n\nCentering conveyor not at home.\n\nPress the START button to home the machine." );
    case 9525:
        return SessionErrorDialog::tr( "Wrapper status\n\nCameras need alignment.\n\nPress the STOP button. Turn off circuit breaker, clean camera's, light tube and height sensors. \n\nRemove any packages, labels, debris in the machine. Turn on circuit breaker when complete." );
    case 9526:
        return SessionErrorDialog::tr( "Wrapper status\n\nPackage is too far left.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9527:
        return SessionErrorDialog::tr( "Wrapper status\n\nPackage is too far right.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9528:
        return SessionErrorDialog::tr( "Wrapper status\n\nCentering conveyor did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9529:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9530:
        return SessionErrorDialog::tr( "Wrapper status\n\nElevator not at home.\n\nPress the START button to home the machine." );
    case 9531:
        return SessionErrorDialog::tr( "Wrapper status\n\nElevator jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9532:
        return SessionErrorDialog::tr( "Wrapper status\n\nElevator motor electrical failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9533:
        return SessionErrorDialog::tr( "Wrapper status\n\nElevator failed to home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9534:
        return SessionErrorDialog::tr( "Wrapper status\n\nElevator sensor cam failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9535:
        return SessionErrorDialog::tr( "Wrapper status\n\nElevator sensor is not working.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9536:
        return SessionErrorDialog::tr( "Wrapper status\n\nElevator slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9537:
        return SessionErrorDialog::tr( "Wrapper status\n\nPackage will not wrap with selected film. \n\nCheck roll sizes in wrapper settings page.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9538:
        return SessionErrorDialog::tr( "Wrapper status\n\nFilm not pulled. Press the STOP button. Check to make sure film is properly loaded and the gripper and film selector are clear of debris. Press the START button." );
    case 9539:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper not at home.\n\nPress the START button to home the machine." );
    case 9540:
        return SessionErrorDialog::tr( "Wrapper status\n\nKnife Error.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9541:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9542:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9543:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper sensor or belt failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9544:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper motor electrical failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9545:
        return SessionErrorDialog::tr( "Wrapper status\n\nLate film cut.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9546:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide clamps not at home.\n\nPress the START button to home the machine." );
    case 9547:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide clamps jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9548:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide folders not at home.\n\nPress the START button to home the machine." );
    case 9549:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide folders jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9550:
        return SessionErrorDialog::tr( "Wrapper status\n\nRear folder not at home.\n\nPress the START button to home the machine." );
    case 9551:
        return SessionErrorDialog::tr( "Wrapper status\n\nRear folder jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9552:
        return SessionErrorDialog::tr( "Wrapper status\n\nFront folder not at home.\n\nPress the START button to home the machine." );
    case 9553:
        return SessionErrorDialog::tr( "Wrapper status\n\nFront folder jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9554:
        return SessionErrorDialog::tr( "Wrapper status\n\nPusher not at home.\n\nPress the START button to home the machine." );
    case 9555:
        return SessionErrorDialog::tr( "Wrapper status\n\nPusher jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9556:
        return SessionErrorDialog::tr( "Wrapper status\n\nSealer belt jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9557:
        return SessionErrorDialog::tr( "Wrapper status\n\nKnife jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9558:
        return SessionErrorDialog::tr( "Wrapper status\n\nLabel applier did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9559:
        return SessionErrorDialog::tr( "Wrapper status\n\nTemperature Error" );
    case 9560:
        return SessionErrorDialog::tr( "Wrapper status\n\nHeater Error" );
    case 9561:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9562:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Clamp sensor or belt failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9563:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Clamp motor electrical failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9564:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Clamp slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9565:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrapper lost CAN communications.\n\nCycle the circuit breaker. Press the START button." );
    case 9566:
        return SessionErrorDialog::tr( "Wrapper status\n\nReserved1.\n\nCycle the circuit breaker. Press the START button." );
    case 9567:
        return SessionErrorDialog::tr( "Wrapper status\n\nCanopy open.\n\nPlease close the canopy. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9568:
        return SessionErrorDialog::tr( "Wrapper status\n\nRight-side access door open.\n\nPlease close the door. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9569:
        return SessionErrorDialog::tr( "Wrapper status\n\nLeft-side access door open.\n\nPlease close the door. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9570:
        return SessionErrorDialog::tr( "Wrapper status\n\nCanopy open.\n\nPlease close the canopy. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9571:
        return SessionErrorDialog::tr( "Wrapper status\n\nRear film door open.\n\nPlease be sure both film doors are closed. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9572:
        return SessionErrorDialog::tr( "Wrapper status\n\nRear film door open.\n\nPlease be sure both film doors are closed. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9573:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Folder sensor or belt failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9574:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Folder motor electrical failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9575:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide clamps did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9576:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide folders did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9577:
        return SessionErrorDialog::tr( "Wrapper status\n\nRear folder did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9578:
        return SessionErrorDialog::tr( "Wrapper status\n\nFront folder did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9579:
        return SessionErrorDialog::tr( "Wrapper status\n\nPusher did not find home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9580:
        return SessionErrorDialog::tr( "Reserved3" );
    case 9581:
        return SessionErrorDialog::tr( "Wrapper status\n\nPackage stopped on secondary intake.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9582:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Folder slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9583:
        return SessionErrorDialog::tr( "Press the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9584:
        return SessionErrorDialog::tr( "Package is too tall" );
    case 9585:
        return SessionErrorDialog::tr( "Wrapper status\n\nNode 1 motor fuse may be blown.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9586:
        return SessionErrorDialog::tr( "Wrapper status\n\nNode 2 motor fuse may be blown.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9587:
        return SessionErrorDialog::tr( "Wrapper status\n\nNode 3 motor fuse may be blown.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9588:
        return SessionErrorDialog::tr( "Wrapper status\n\nNode 4 motor fuse may be blown.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9589:
        return SessionErrorDialog::tr( "Wrapper status\n\nNode 5 motor fuse may be blown.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9590:
        return SessionErrorDialog::tr( "\nWarning. Low film on Top roll.\n\nPress the STOP button. Remove package from intake. Press the START button to resume wrapping." );
    case 9591:
        return SessionErrorDialog::tr( "\nWarning. Low film on Bottom roll.\n\nPress the STOP button. Remove package from intake. Press the START button to resume wrapping." );
    case 9592:
        return SessionErrorDialog::tr( "Wrapper status\n\nMemory error on node1.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9593:
        return SessionErrorDialog::tr( "Wrapper status\n\nMemory error on node2.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9594:
        return SessionErrorDialog::tr( "Wrapper status\n\nMemory error on node3.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9595:
        return SessionErrorDialog::tr( "Wrapper status\n\nMemory error on node4.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9596:
        return SessionErrorDialog::tr( "Wrapper status\n\nMemory error on node5.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9597:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node1.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9598:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node2.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9599:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node3.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9600:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node4.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9601:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node5.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9602:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node1.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9603:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node2.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9604:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node3.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9605:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node4.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9606:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node5.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9607:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node1.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9608:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node2.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9609:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node3.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9610:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node4.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9611:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node5.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9612:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node1.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9613:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node2.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9614:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node3.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9615:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node4.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9616:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings memory error on node5.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9617:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node1.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9618:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node2.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9619:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node3.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9620:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node4.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9621:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node5.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9622:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node1.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9623:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node2.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9624:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node3.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9625:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node4.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9626:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap timings flash memory error on node5.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9627:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap box timings out of bounds not saved.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9628:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap box timings memory error.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service.." );
    case 9629:
        return SessionErrorDialog::tr( "Wrapper status\n\nToo many wrap boxes write aborted.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9630:
        return SessionErrorDialog::tr( "Wrapper status\n\nToo many wrap boxes read aborted.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9631:
        return SessionErrorDialog::tr( "Package sensor across elevator is blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9632:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap box update timeout failure.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9633:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrap box update memory failure.\n\nCycle the circuit breaker. Press the START button.\n\nIf problem persists please call service." );
    case 9634:
        return SessionErrorDialog::tr( "Node1 communication failure. Reset the wrapper.\n\nCycle the circuit breaker. Press the START button." );
    case 9635:
        return SessionErrorDialog::tr( "Node3 communication failure. Reset the wrapper.\n\nCycle the circuit breaker. Press the START button." );
    case 9636:
        return SessionErrorDialog::tr( "Node4 communication failure. Reset the wrapper.\n\nCycle the circuit breaker. Press the START button." );
    case 9637:
        return SessionErrorDialog::tr( "Node5 communication failure. Reset the wrapper.\n\nCycle the circuit breaker. Press the START button." );
    case 9638:
        return SessionErrorDialog::tr( "Pusher slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9639:
        return SessionErrorDialog::tr( "Wrap box Timings Corruption." );
    case 9640:
        return SessionErrorDialog::tr( "Primary Intake has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9641:
        return SessionErrorDialog::tr( "Secondary Intake has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9642:
        return SessionErrorDialog::tr( "Elevator has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9643:
        return SessionErrorDialog::tr( "Rear Folder has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9644:
        return SessionErrorDialog::tr( "Front Folder has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9645:
        return SessionErrorDialog::tr( "Side Folder has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9646:
        return SessionErrorDialog::tr( "Side Clamp has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9647:
        return SessionErrorDialog::tr( "Sealer belt has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9648:
        return SessionErrorDialog::tr( "Wrapper status\n\nCentering conveyor jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9649:
        return SessionErrorDialog::tr( "Gripper has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9650:
        return SessionErrorDialog::tr( "Pusher has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
    case 9651:
        return SessionErrorDialog::tr( "Sealer belt motor move error." );
    case 9652:
        return SessionErrorDialog::tr( "\nThe Primary label is still at the printer." );
    case 9653:
        return SessionErrorDialog::tr( "\nThe Second label is still at the printer." );
    case 9654:
        return SessionErrorDialog::tr( "The wrap timings are not compatible with the selected model. Check model selection in wrapper system info." );

    case 9655:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9656:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9657:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9658:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9659:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9660:
        return SessionErrorDialog::tr( "Wrapper status\n\nGripper Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9661:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Clamp Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9662:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Clamp Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9663:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Clamp Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9664:
        return SessionErrorDialog::tr( "Wrapper status\n\nSide Clamp Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9665:
        return SessionErrorDialog::tr( "Wrapper status\n\nUpper Film Roll Failed to Switch.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9666:
        return SessionErrorDialog::tr( "Wrapper status\n\nLower Film Roll Failed to Switch.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9667:
        return SessionErrorDialog::tr( "Wrapper status\n\nFilm Selector Lever is unlatched.\n\nPress the STOP button. Engage Film Selector Lever. Press the START button." );
    case 9668:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary Applier Wand Failed to Home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9669:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary Applier Wand Failed to Move.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9670:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary Applier Hand Failed to Home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9671:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrimary Applier Wand Vacuum Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9672:
        return SessionErrorDialog::tr( "Wrapper status\n\nAir Compressor Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9673:
        return SessionErrorDialog::tr( "Wrapper status\n\nRegulated Air Pressure Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9674:
        return SessionErrorDialog::tr( "Wrapper status\n\nActuator Air Pressure Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9675:
        return SessionErrorDialog::tr( "Wrapper status\n\nUpper Film Pull not Detected.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9676:
        return SessionErrorDialog::tr( "Wrapper status\n\nLower Film Pull not Detected.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9677:
        return SessionErrorDialog::tr( "Wrapper status\n\n24V Power Supply Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9678:
        return SessionErrorDialog::tr( "Wrapper status\n\n170V Inrush Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9679:
        return SessionErrorDialog::tr( "Wrapper status\n\n170V Power Supply Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9680:
        return SessionErrorDialog::tr( "Wrapper status\n\nUpper Film Brake Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9681:
        return SessionErrorDialog::tr( "Wrapper status\n\nLower Film Brake Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9682:
        return SessionErrorDialog::tr( "Wrapper status\n\nUpper Film Roll is Loose.\n\nThe Upper Film Roll is loose on the film Arbor.\n\nPlease tighten the Film Arbor knob, turning clockwise.\n\nEnsure the core grip is tight to the film core." );
    case 9683:
        return SessionErrorDialog::tr( "Wrapper status\n\nLower Film Roll is Loose.\n\nThe Lower Film Roll is loose on the film Arbor.\n\nPlease tighten the Film Arbor knob, turning clockwise.\n\nEnsure the core grip is tight to the film core." );
    case 9684:
        return SessionErrorDialog::tr( "Wrapper status\n\nPrinter Dock Failure.\n\nPlease make sure printer is docked in proper position." );
    case 9685:
        return SessionErrorDialog::tr( "Wrapper status\n\nPackage Sensor across Sealer Belt is blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Clear any obstructions from the package sensor. Press the START button." );
    case 9686:
        return SessionErrorDialog::tr( "Wrapper status\n\nPress Start Button and then place package on Intake." );
    case 9687:
        return SessionErrorDialog::tr( "Wrapper status\n\nIntake Package Sensor is blocked. Please make sure sensor is clean and free of debris. \n\nIf problem persists please call service." );
    case 9688:
        return SessionErrorDialog::tr( "Regulated air pressure is too high. Adjust air regulator to within the range 45-55 psi psi. " );
    case 9689:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary Applier Wand Failed to Home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9690:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary Applier Wand Failed to Move.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9691:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary Applier Hand Failed to Home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9692:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary Applier Wand Vacuum Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9693:
        return SessionErrorDialog::tr( "Wrapper status\n\nnSecondary Applier Timeout.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9694:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary Applier jammed while wrapping package. Press the Stop button. Remove any packages, labels, debris in the machine. Press the Start button" );
    case 9695:
        return SessionErrorDialog::tr( "Wrapper status\n\nSecondary Applier failed to return to the Home position.\n\nPress the Stop button. Remove any packages, labels, debris in the machine. Press the Start button" );
    case 9698:
        return SessionErrorDialog::tr( "MaxEventError = 197" );
    case 9696:
        return SessionErrorDialog::tr( "NoEventError = 195," );
    case 9699:
        return SessionErrorDialog::tr( "StopEventError = 198," );
    case 9700:
        return SessionErrorDialog::tr( "EmergencyEventError," );
    case 9701:
        return SessionErrorDialog::tr( "InvalidEventTableIndex" );
    case 9702:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrapper did not respond.\n\nCycle the circuit breaker. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9703:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrapper did not respond.\n\nCycle the circuit breaker. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9704:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrapper did not respond.\n\nCycle the circuit breaker. Remove any packages, labels, debris in the machine. Press the START button." );
    case 9705:
        return SessionErrorDialog::tr( "\n\n\nThe Wrapper is updating, please wait." );
    case 9706:
        return SessionErrorDialog::tr( "PLU Exception:\nEnter a PLU number." );
    case 9707:
        return SessionErrorDialog::tr( "Auto config:\nEnter a tray name." );
    case 9708:
        return SessionErrorDialog::tr( "Do you wish to delete this tray?" );
    case 9709:
        return SessionErrorDialog::tr( "Do you wish to delete this Height box?" );
    case 9710:
        return SessionErrorDialog::tr( "The Wrapper did not respond to the last SavePackageTimings message." );
    case 9711:
        return SessionErrorDialog::tr( "\nAn invalid Sealer belt temperature was entered. The previous value will be restored." );
    case 9712:
        return SessionErrorDialog::tr( "Adding this Height box exceeds memory for the Wrapper. This Height box will be discarded." );
    case 9713:
        return SessionErrorDialog::tr( "Adding this Wrap box exceeds memory for the Wrapper. This Wrap box will be discarded." );
    case 9714:
        return SessionErrorDialog::tr( "Deleting all Wrapper TAR files from the firmware directory." );
    case 9715:
        return SessionErrorDialog::tr( "Wrapper TAR files have been Deleted." );
    case 9716:
        return SessionErrorDialog::tr( "\nThe last package cycled\ndoes not match this tray size." );
    case 9717:
        return SessionErrorDialog::tr( "\nAre you sure that you want to reset the Wrapper Timings?" );
    case 9718:
        return SessionErrorDialog::tr( "\nWrapper operation has changed.\nPlease cycle the Wrapper circuit breaker." );
    case 9719:
        return SessionErrorDialog::tr( "\nThe camera threshold is being saved.\n\nThere will be a short delay before the camera scan continues." );
    case 9720:
        return SessionErrorDialog::tr( "\nInvalid tray length.\n\nPlease measure and enter the length again." );
    case 9722:
        return SessionErrorDialog::tr( "\nWrapper Error.\n\nInvalid system configuration. Please configure System model in System Info." );

    case 9723:
        return SessionErrorDialog::tr( "The device timings file does not exist. Please save timings." );
    case 9724:
        return SessionErrorDialog::tr( "Some device timings are missing from the saved file and can't be processed.\nPlease resave timings." );
    case 9725:
        return SessionErrorDialog::tr( "The device timings file was empty." );
    case 9726:
        return SessionErrorDialog::tr( "The device timings were successfully saved to a file." );
    case 9727:
        return SessionErrorDialog::tr( "There was an error saving the device timings to a file.\nPlease try again." );
    case 9728:
        return SessionErrorDialog::tr( "Operating System error while saving device timings.\nPlease try again." );
        // SOF-5463: Error and Status Dialogs for NEW 2P Project with single sliding 2nd Printer
        // All of these previous errors are no longer needed so we should not even be firing the WrPPPStatus,
        // unless it is decided to be used with the new 2P Project.  In any case, the following messages
        // can be reclaimed and used again
        // 9729, 9730, 9731, 9732, 9733, 9734, 9735, 9736, 9737, 9738, 9739, 9740, 9741
    //case 9729:
    //    return SessionErrorDialog::tr( "Primary Printer Plate Sensor needs calibration." );
    //case 9730:
    //    return SessionErrorDialog::tr( "Secondary Printer Plate Sensor needs calibration." );
    //case 9731:
    //    return SessionErrorDialog::tr( "Primary Printer Plate Sensor failed internal calibration.\n\nIf problem persists please call service" );
    //case 9732:
    //    return SessionErrorDialog::tr( "Secondary Printer Plate Sensor failed internal calibration.\n\nIf problem persists please call service" );
    //case 9733:
    //    return SessionErrorDialog::tr( "Primary Printer Plate Sensor start measurement failure.\n\nIf problem persists please call service" );
    //case 9734:
    //    return SessionErrorDialog::tr( "Secondary Printer Plate Sensor start measurement failure.\n\nIf problem persists please call service" );
    //case 9735:
    //    return SessionErrorDialog::tr( "Secondary Printer Plate Sensor address set failure.\n\nIf problem persists please call service" );
    //case 9736:
    //    return SessionErrorDialog::tr( "Printer Plate Sensor communication failure.\n\nIf problem persists please call service" );
    //case 9737:
    //    return SessionErrorDialog::tr( "Both Printer Plate Sensors needs calibration.\n\nPlease call service" );
    //case 9738:
    //    return SessionErrorDialog::tr( "Primary Printer Plate Temperaure Sensor configuration failure.\n\nIf problem persists please call service" );
    //case 9739:
    //    return SessionErrorDialog::tr( "Primary Printer Plate Temperaure Sensor address failure.\n\nIf problem persists please call service" );
    //case 9740:
    //    return SessionErrorDialog::tr( "Secondary Printer Plate Temperaure Sensor configuration failure.\n\nIf problem persists please call service" );
    //case 9741:
    //    return SessionErrorDialog::tr( "Secondary Printer Plate Temperaure Sensor address failure.\n\nIf problem persists please call service" );
    case 9742:
        return SessionErrorDialog::tr( "Warning: Wrapper Configuration not set for two printer system.\n\nPlease call service" );
    case 9743:
        return SessionErrorDialog::tr( "Warning: Wrapper Configuration discharge does not match.\n\nPlease call service" );
    case 9744:
        return SessionErrorDialog::tr( "Weight detected: Please replace package on weigher" );
    case 9745:
        return SessionErrorDialog::tr( "Canopy cover or doors open.\n\nPlease close the canopy or doors. Remove any packages, labels, debris in the machine. Press the START button." );
    // SOF-5463: Error and Status Dialogs for NEW 2P Project - No longer triggering printer plater errors
    //case 9746:
    //    return SessionErrorDialog::tr( "Printer plate sensors did not respond.\n\nPlease reboot the scale.\n\nIf problem persists please call service." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrWrapperStatusDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 10014:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrapper did not respond.\n\nCycle the circuit breaker. Remove any packages, labels, debris in the machine. Press the START button." );
    case 10015:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrapper did not respond.\n\nCycle the circuit breaker. Remove any packages, labels, debris in the machine. Press the START button." );
    case 10016:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrapper did not respond.\n\nCycle the circuit breaker. Remove any packages, labels, debris in the machine. Press the START button." );
    case 10017:
        return SessionErrorDialog::tr( "Wrapper status\n\nWrapper did not respond.\n\nCycle the circuit breaker. Remove any packages, labels, debris in the machine. Press the START button." );
    case 10018:
        return SessionErrorDialog::tr( "Wrapper status\n\nThe wrapper stopped communicating.\n\nCheck to be sure the wrapper cable is plugged in.\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrTouchScreenDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 14000:
        return SessionErrorDialog::tr( "Unknown touch screen calibration error." );
    case 14001:
        return SessionErrorDialog::tr( "Touch screen calibration failed to start correctly." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrBarcodeScanner>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 16000:
        return SessionErrorDialog::tr( "Unknown Barcode Scanner error." );
    case 16001:  // from app, received a read 1 from ui, and cant find it in the db. cant really happen tho bc we sent the item to the ui initially anyway
        return SessionErrorDialog::tr( "Record not found in database." );
    case 16002:  // from ui, cant add&save current scan code bc plu field is < 0 (how can this happen???)
        return SessionErrorDialog::tr( "Invalid product number." );
    case 16003:  // from ui, cant add&save current scan code bc plu field is blank
        return SessionErrorDialog::tr( "Please enter a Product Number." );
    case 16004:  // from ui, cant delete or add&save an item because there is no scan code bc no row selected or item scanned
        return SessionErrorDialog::tr( "The GS1 code is currently blank. Please scan a new item" );
    case 16005:  // from ui, no plu in the plu field when trying to do a delete
        return SessionErrorDialog::tr( "Can't delete current item.  The Product Number field is blank." );
    case 16006:  // from ui, no row selected when trying to do a delete
        return SessionErrorDialog::tr( "An item must be selected from the table in order to be deleted." );
    case 16007:  // from ui, trying to add a scan code and plu that already exist in the table
        return SessionErrorDialog::tr( "Cannot add the following item.\nThe selected scan code and product already exists" );
    case 16008:  // from application after looking up a scan code and there isnt one in the database. The PLU still like;y exists by itself though
        return SessionErrorDialog::tr( "The scanned item does not exist in the database.\nPlease enter the product number manually." );
    case 16009:  // from application after looking up a scan code and there isnt one in the database. The PLU still like;y exists by itself though
        return SessionErrorDialog::tr( "The scanned item already exists in the database." );
    case 16010:  // from application after clicking on a row but the plu in the row doesnt exist
        return SessionErrorDialog::tr( "The selected barcode has a product number currently not in the database." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrSessionDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case SessionDevError::SessionDevErrUnknownError:
        return SessionErrorDialog::tr( "Unknown Session error." );
    case SessionDevError::SessionDevErrNotPrimary:
        return SessionErrorDialog::tr( "You must be at the scale to perform that operation." );
    case SessionDevError::SessionDevErrCommLoss:
        return SessionErrorDialog::tr( "Communication was lost between the browser and the scale." );
    case SessionDevError::SessionDevAllFilesDeleted:
        return SessionErrorDialog::tr( "All files have been deleted." );
    case SessionDevError::SessionDevBootableCFInProgress:
        return SessionErrorDialog::tr( "Working...\nDo not remove the CompactFlash Reader/Writer or the card until the process is complete.\nNote: This process will take some time." );
    case SessionDevError::SessionDevBootableCFComplete:             // 5
        return SessionErrorDialog::tr( "Compact flash is ready." );
    case SessionDevError::SessionDevBootableCFError:
        return SessionErrorDialog::tr( "Error!\nMake sure the CompactFlash Reader/Writer is plugged in and check that the flash card is inserted correctly in the Reader/Writer." );
    case SessionDevError::SessionDevReqNewDirName:
        return SessionErrorDialog::tr( "Enter a new directory name." );
    case SessionDevError::SessionDevErrDuplicateDirectory:
        return SessionErrorDialog::tr( "Duplicate name!\nEnter a new directory name." );
    case SessionDevError::SessionDevDeleteAllFilesInProgress:
        return SessionErrorDialog::tr( "Deleting all files..." );
    case SessionDevError::SessionDevChangingLanguages:              // 10
        return SessionErrorDialog::tr( "Working...\nPlease wait while the scale changes language." );
    case SessionDevError::SessionDevDBCompressStarting:
        return SessionErrorDialog::tr( "Please wait.\nCompressing the database ..." );
    case SessionDevError::SessionDevEnteringMonitorMode:
        return SessionErrorDialog::tr( "Please wait.\n\nEntering Monitor Mode ..." );
    case SessionDevError::SessionDevRemoveThumbDrive:
        return SessionErrorDialog::tr( "Please remove the USB drive before returning to Operate mode." );
    case SessionDevError::SessionDevDateTimeReadError:
        return SessionErrorDialog::tr( "When reading the date/time, year determined to be < 1900.\n\nPlease notify Hobart." );
    case SessionDevError::SessionDevDeleteAllFilesFailed:             // 15
        return SessionErrorDialog::tr( "Database error occurred when deleting all files!\n\nPlease notify Hobart." );
    case SessionDevError::SessionDevBootableCFErrorSuperKeyPresent:
        return SessionErrorDialog::tr( "Error! This is a supervisor/service key. Make sure you have service key removed." );
    case SessionDevError::SessionDevBrowserBootupStarted:
        return SessionErrorDialog::tr( "Initialization and loading sequence.\n\nPlease wait ...." );
    case SessionDevError::SessionDevBrowserBootupCompleted:
        return SessionErrorDialog::tr( "Initialization and loading sequence is complete.\n\nYou may now begin." );          // THE ALWAYS FAMOUS 100018
    case SessionDevError::SessionDevTimedWrSealerBeltOff:
        return SessionErrorDialog::tr( "Scheduled event - power to the\nWrapper sealer belt has been turned OFF." );
    case SessionDevError::SessionDevTimedWrSealerBeltOn:                        // 20
        return SessionErrorDialog::tr( "Scheduled event - power to the\nWrapper sealer belt has been turned ON." );
    case SessionDevError::SessionDevCommExistingBeforeShutdown:
        return SessionErrorDialog::tr( "Communications with the scale is in progress.\n\nPress OK to force the shutdown now.\n   or\nPress CANCEL to cancel the shutdown.\n   or\nwait, and the shutdown will occur when communications is finished." );
    case SessionDevError::SessionDevShuttingdown:
        return SessionErrorDialog::tr( "The scale is shutting down.\n\nPlease wait ..." );
    case SessionDevError::SessionDevShutdownComplete:
        return SessionErrorDialog::tr( "The scale application is ready to shutdown.\n\nPress the Shut Down button to power down the scale or Reboot to restart." );
    case SessionDevError::SessionDevInactivityAutoLogout:
        return SessionErrorDialog::tr( "The remote operator has been logged out due to inactivity." );
    case SessionDevError::SessionDevShutdownAreYouSure:         // 25
        return SessionErrorDialog::tr( "Are you sure you want to shutdown?\n\nPress OK to initiate the shutdown.\n   or\nPress CANCEL to cancel the shutdown." );
    case SessionDevError::SessionDevDBClosingTemporarily:
        return SessionErrorDialog::tr( "Please wait.\nDatabase closing temporarily ..." );
    case SessionDevError::SessionDevUsingEmptyDatabase:
        return SessionErrorDialog::tr( "The database was corrupt and has been replaced with an empty database.  The corrupted directory has been renamed.\n\nPlease load your database again." );
    case SessionDevError::SessionDevCheckForThumbdrive:
        return SessionErrorDialog::tr( "Please remove the USB drive before continuing with Shutdown." );
    case SessionDevError::SessionDevLoadingIntranetIE:
        return SessionErrorDialog::tr( "Loading Browser.\n\nPlease wait ......" );
    case SessionDevError::SessionDevShutdownWithoutFBWFAreYouSure:              // 30
        return SessionErrorDialog::tr( "!! CAUTION !!\nOS configuration mode ONLY\n\nContinuing will reboot the scale in OS configuration mode.  This mode will require multiple reboots to restart the Scale application." );
    case SessionDevError::SessionDevNoUpdateMessageBeforeDesktop:
        return SessionErrorDialog::tr( "!! ATTENTION !!\n\nThis mode provides basic OS Desktop use ONLY.  Permanent configuration changes cannot be made." );
    case SessionDevError::SessionDevCouldNotUnprotectOS:
        return SessionErrorDialog::tr( "Could not put the scale into OS Configuration mode.  As a result, the scale will not be rebooted." );
    case SessionDevError::SessionDevUsingMirrorDatabase:
        return SessionErrorDialog::tr( "The database was corrupt and has been replaced with a previous backup.  Verify current pricing before labeling product." );
    case SessionDevError::SessionDevConfigWindowsWithoutRestrictedLogin:
        return SessionErrorDialog::tr( "!! CAUTION !!\nOS configuration mode\n\nAfter configuring the OS, return to the scale application via the task bar." );
    case SessionDevError::SessionDevConfigWindowsWithRestrictedLogin:           // 35
        return SessionErrorDialog::tr( "!! CAUTION !!\nOS configuration mode\n\nContinuing will require you to login to the Administrator account to configure the OS.\n\nWhen finished configuring, reboot the scale." );
    case SessionDevError::SessionDevShutdownWithoutFBWFLinuxAreYouSure:
        return SessionErrorDialog::tr( "!! CAUTION !!\nOS configuration mode\n\nContinuing will put you in Config OS Mode, and place you at the desktop. Please reboot when finished to restart the scale application." );
    case SessionDevError::SessionDevAccumulateItemsNoProductsInList:
        return SessionErrorDialog::tr( "Cannot print the label because there are no items in the Accumulated Items list." );
    case SessionDevError::SessionDevAccumulateItemsRenderingManyProducts:
        return SessionErrorDialog::tr( "Too many items in the Accumumlate List for the label stock." );
    case SessionDevError::SessionDevProductionEntryNotEnabled:
        return SessionErrorDialog::tr( "\nProduction Entry mode is not enabled." );
    case SessionDevError::SessionDevAccumulateItemsNotEnabled:                  // 40
        return SessionErrorDialog::tr( "\nAccumulate Items mode is not enabled." );
    case SessionDevError::SessionDevAccumulateItemsEmptyList:
        return SessionErrorDialog::tr( "There are no Items in the Accumulated Items list." );
    case SessionDevError::SessionDevAccumulateItemsItemRemoved:
        return SessionErrorDialog::tr( "\n Transaction removed from the Accumulated Items List." );
    case SessionDevError::SessionDevAccumulateItemsUndefinedLabelType:
        return SessionErrorDialog::tr( "\nInstalled Label Stock is Not Defined." );
    case SessionDevError::SessionDevAccumulateItemsNoValidLabelType:
        return SessionErrorDialog::tr( "\n No Label Type available for the installed Label Stock." );
    case  SessionDevError::SessionDevItemAddedToAccumulateItemsList:            // 45
        return SessionErrorDialog::tr( "\n\n                                     ITEM ADDED" );
    case  SessionDevError::SessionDevAccumulateItemsMaxItemsExceeded:
        return SessionErrorDialog::tr( "You cannot add more than 99 items." );
    case  SessionDevError::SessionDevRebootImmediate:
        return SessionErrorDialog::tr( "Changes in scale settings require a mandatory system restart.\n\nThe scale will now restart." );
    case  SessionDevError::SessionDevRemoveUSBDrive:
        return SessionErrorDialog::tr( "Remove the USB drive key to continue scale shutdown.");
    case  SessionDevError::SessionDevRemoteAccessProhibited:
        return SessionErrorDialog::tr( "Remote access to this scale is prohibited.");
    case SessionDevError::SessionBackingUpDatabase:                                 // 50
        return SessionErrorDialog::tr( "Please wait.\nBacking up the database ...");
    case SessionDevError::SessionDevDBIsCorrupt:
        return SessionErrorDialog::tr( "The database is corrupt!  The scale will now restart to handle this issue.");
    case SessionDevError::SessionDevDeleteAllFilesAlreadyInProgress:
        return SessionErrorDialog::tr( "Delete All Files is already in progress ..." );
    case SessionDevError::SessionDevAccumulateItemsFirstItemBarcodeFitError:
        return SessionErrorDialog::tr( "This item will not be added to the list because the label's barcode would be incorrect" );
    case SessionDevError::SessionDevAccumulateItemsFirstItemBarcodeFitWarning:
        return SessionErrorDialog::tr( "Warning:  The first item in the list will cause the label's barcode to be incorrect" );
    case SessionDevError::SessionDevBeginSelfService:                               // 55
        return SessionErrorDialog::tr("Begin Self Service");
    case SessionDevError::SessionDevSelfServInitAndLoading:
        return SessionErrorDialog::tr("Self Service initialization and loading sequence.\n\nPlease wait ...." );
    case SessionDevError::SessionDevSearceFeatureDisabled:
        return SessionErrorDialog::tr("The Search feature has been intentionally disabled." );
    case SessionDevError::SessionDevToggleModeNotEnabled:
        return SessionErrorDialog::tr("Toggle Prepack Mode is not enabled.");
    case SessionDevError::SessionDevRemotePrintWarning:
        return SessionErrorDialog::tr("Printing remote label please wait....");
    case SessionDevError::SessionDevPick5ActiveStatus:                              // 60
        return SessionErrorDialog::tr("The scale has %s PICK 5 Mode.");
    case SessionDevError::SessionDevRemoveUSBServiceKey:
        return SessionErrorDialog::tr("SessionDevRemoveUSBServiceKey");
    case SessionDevError::SessionDevOnlineOrderingActive:
    	return SessionErrorDialog::tr("SessionDevOnlineOrderingActive");
    case SessionDevError::SessionDevOnlineOrderingEnabled:
		return SessionErrorDialog::tr("SessionDevOnlineOrderingEnabled");
    case SessionDevError::SessionDevWillNotDisableExtraDay:
		return SessionErrorDialog::tr("Add an Extra Sell by Day is a configured Flashkey in Operate Mode.  The Sell By Time can be updated but enabling or disabling the feature will not be saved. The feature can only be changed with the Flashkey in Operate Mode.");
    case SessionDevError::SessionDevSecLabelEnableToggleStatus:
		return SessionErrorDialog::tr("Second Label is %s");

    
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrProductDevice>( int code, int isScale )
{
    switch ( code )
    {
    case 101000:
        return SessionErrorDialog::tr( "Unknown product error." );
    case 101001:
        if( QFile::exists( "/var/log/WebServiceRequests.log" ) )
            return SessionErrorDialog::tr( "Item Code not found on the Server or the Scale." );
        else
            return SessionErrorDialog::tr( "Item Code not found." );
    case 101002:
        return SessionErrorDialog::tr( "Cannot change the product record at this time." );
    case 101003:
        if( ScaleModel::ScaleModel_ESS == isScale )           // means we are self serv and printing a label to the screen
            return SessionErrorDialog::tr( "Cannot print the label because of a weighing error.\n\nThe only available option is to scan a label." );
        else
            return SessionErrorDialog::tr( "Cannot print the label because of a weighing error." );
    case 101004:
        return SessionErrorDialog::tr( "Invalid reporting class for this product." );
    case 101005:
        return SessionErrorDialog::tr( "Invalid label type for this product." );
    case 101006:
        if( ScaleModel::ScaleModel_ESS == isScale )           // means we are self serv and printing a label to the screen
            return SessionErrorDialog::tr( "Invalid or missing graphic for this product.\n\nThe only available option is to scan a label." );
        else
            return SessionErrorDialog::tr( "Invalid or missing graphic for this product." );
    case 101007:
        return SessionErrorDialog::tr( "Enter the product number to USE." );
    case 101008:
        return SessionErrorDialog::tr( "Enter the product number to COPY and EDIT." );
    case 101009:
        return SessionErrorDialog::tr( "Enter the new product number." );
    case 101010:
        return SessionErrorDialog::tr( "You may not use the print key in\n\"Prepack\" mode." );
    case 101011:
        return SessionErrorDialog::tr( "Please select a product before pressing the print key." );
    case 101012:
        return SessionErrorDialog::tr( "Internal product error. No current transaction." );
    case 101013:
        return SessionErrorDialog::tr( "Please place your item on the scale and then press the print key." );
    case 101014:
        return SessionErrorDialog::tr( "The item on the scale does not weigh enough to print." );
    case 101015:
        return SessionErrorDialog::tr( "This product cannot be processed when a scale is not present in the system." );
    case 101016:
        return SessionErrorDialog::tr( "Primary label type does not exist." );
    case 101017:
        return SessionErrorDialog::tr( "Second label type does not exist." );
    case 101018:
        return SessionErrorDialog::tr( "Third label type does not exist." );
    case 101019:
        if( ScaleModel::ScaleModel_ESS == isScale )           // means we are self serv and printing a label to the screen
            return SessionErrorDialog::tr( "Primary label type will not fit on the label stock.\n\nThe only available option is to scan a label." );
        else
            return SessionErrorDialog::tr( "Primary label type will not fit on the label stock." );
    case 101020:
        return SessionErrorDialog::tr( "Second label type will not fit on the label stock." );
    case 101021:
        return SessionErrorDialog::tr( "Third label type will not fit on the label stock." );
    case 101022:
        return SessionErrorDialog::tr( "Primary label type was changed so it will fit on the label stock." );
    case 101023:
        return SessionErrorDialog::tr( "Second label type was changed so it will fit on the label stock." );
    case 101024:
        return SessionErrorDialog::tr( "Third label type was changed so it will fit on the label stock." );
    case 101025:
        return SessionErrorDialog::tr( "You must enter a valid By count value." );
    case 101026:
        return SessionErrorDialog::tr( "Are you sure you want to delete this product?" );
    case 101027:
        return SessionErrorDialog::tr( "A label cannot be printed.\nThe weigher has not correctly weighed the item." );
    case 101028:
        return SessionErrorDialog::tr( "The product number you entered already exists. Please try again." );
    case 101029:
        return SessionErrorDialog::tr( "There is no Expanded text for the product number you entered. Please try again." );
    case 101030:
        return SessionErrorDialog::tr( "There is no Special message for the product number you entered. Please try again." );
    case 101031:
        return SessionErrorDialog::tr( "There is no Product note for the product number you entered. Please try again." );
    case 101032:
        return SessionErrorDialog::tr( "There is no Nutrifacts for the product number you entered. Please try again." );
    case 101033:
        return SessionErrorDialog::tr( "The Primary printer needs to size the label.\n\nPlease size the Primary Printer." );
    case 101034:
        return SessionErrorDialog::tr( "The Second printer needs to size the label.\n\nPlease size the Second Printer." );
    case 101035:
        return SessionErrorDialog::tr( "The Third printer needs to size the label.\n\nPlease size the Third Printer." );
    case 101036:
        return SessionErrorDialog::tr( "Printing label." );
    case 101037:
        return SessionErrorDialog::tr( "You may not access a fixed weight or fluid ounce product on this scale." );
    case 101038:
        return SessionErrorDialog::tr( "You may not enter a character while searching for a range of PLUs.  Enter [0-9] only." );
    case 101039:
        return SessionErrorDialog::tr( "You must enter the price before a label can be printed." );
    case 101040:
        return SessionErrorDialog::tr( "You must enter the by count before a label can be printed." );
    case 101041:
        return SessionErrorDialog::tr( "You must enter the tare before a label can be printed." );
    case 101042:
        return SessionErrorDialog::tr( "No label types have been selected." );
    case 101043:
        return SessionErrorDialog::tr( "Platter tare.\n\nPlace tray on platter and press OK." );
    case 101044:
        return SessionErrorDialog::tr( "There is no Marquee text for the product number you entered. Please try again." );
    case 101045:
        return SessionErrorDialog::tr( "The tare you entered is invalid." );
    case 101046:
    {
        if( ScaleModel::ScaleModel_ESS == isScale )
        {
            const QString txt = sessionHooks::instance()->getFieldValue(sessionHooks::selfServiceByCountPrompt);
            return SessionErrorDialog::tr ( "You must first enter the quantity on the keypad to the right and then press") + " " + tr( txt.toStdString().c_str() ) + ".";
        }
        else
            return SessionErrorDialog::tr( "A value must be entered for the forced field before you can print a label." );
    }
    case 101047:
        return SessionErrorDialog::tr( "The Product Number for the associated flashkey is invalid.  Please check the Flashkey Setup and validate the Product Number." );
    case 101048:
        return SessionErrorDialog::tr( "The EAN Mask under Product Default Values is either blank or not yet set up. \nThe EanPrefix cannot be edited until the mask setup is complete." );
    case 101049:
        return SessionErrorDialog::tr( "You may not select a random weight product while in Production Entry mode." );
    case 101050:
        return SessionErrorDialog::tr( "This product can only be used in \"Manual weigh\" mode." );
    case 101051:
        if( ScaleModel::ScaleModel_ESS == isScale )           // means we are self serv and printing a label to the screen
            return SessionErrorDialog::tr( "The selected label type may only be used with a Random weight product.\n\nThe only available option is to scan a label." );
        else
            return SessionErrorDialog::tr( "The selected label type may only be used with a Random weight product." );
    case 101052:
        return SessionErrorDialog::tr( "Fresh Production Planning:\nProduced quantity for PLU %s %s will be updated by %s for production. \n Are you sure?" );
    case 101053:
        return SessionErrorDialog::tr( "Fresh Production Planning:\nProduced quantity for all PLU's entered will be updated for production.\nAre you sure?" );
    case 101054:
        return SessionErrorDialog::tr( "Fresh Production Planning:\nFailed to update." );
    case 101055:
        return SessionErrorDialog::tr( "You may not select a Production Planning Ingredient product while in Production Entry mode." );
    case 101056:
        return SessionErrorDialog::tr( "You may not print Shelf Life Expiration labels while in Production Entry mode." );
    case 101057:
        return SessionErrorDialog::tr( "Please select a product before pressing the 'Print Multiple Labels' button." );
    case 101058:
        return SessionErrorDialog::tr( "You may not select 'Print Multiple Labels' while in Production Entry mode." );
    case 101059:
        return SessionErrorDialog::tr( "You may not select a random weight product for the 'Print Multiple Labels' function." );
    case 101060:
        return SessionErrorDialog::tr( "Fresh Production Planning:\nProduced quantity for PLU %s %s will be updated by %s for each label printed for production. \n Are you sure?" );
    case 101061:
        return SessionErrorDialog::tr( "You may not print Shelf Life Expiration labels while using the Print Multiple Labels function." );
    case 101062:
        return SessionErrorDialog::tr( "You may not select 'Print Multiple Labels' for products that are Production Planning Type 2." );
    case 101063:
        return SessionErrorDialog::tr( "There is no expiration entry for this item.\nDo you wish to create a new entry?" );
    case 101064:
        return SessionErrorDialog::tr( "You are trying to reprint the expiration label for a product that has expired.\n\nPress OK to continue and print the label,\nor CANCEL to not print the label." );
    case 101065:
        return SessionErrorDialog::tr( "The expiration entry for this item has not expired.\nDo you wish to overwrite the current entry?" );
    case 101066:
        return SessionErrorDialog::tr( "The total price is invalid." );
    case 101067:
        return SessionErrorDialog::tr( "\nLabel placement and/or label rotation\nhas been changed." );
    case 101068:
        return SessionErrorDialog::tr( "There is no expiration entry for this item." );
    case 101069:
        return SessionErrorDialog::tr( "Invalid Backdate. The entered Backdate may not be a future date." );
    case 101070:
        return SessionErrorDialog::tr( "Invalid Backdate." );
    case 101071:
        return SessionErrorDialog::tr( "You may not print when the Graphical Customer Display is disconnected." );
    case 101072:
        return SessionErrorDialog::tr( "You may not select a random weight product while in 'Accumulate Items' mode." );
    case 101073:
        return SessionErrorDialog::tr( "You may not select a Production Planning Ingredient product while in 'Accumulate Items' mode." );
    case 101074:
         return SessionErrorDialog::tr( "\n\n                                     ITEM ADDED" );
    case 101075:
        return SessionErrorDialog::tr( "Please select a product before adding it to the 'Accumulated Items' list." );
    case 101076:
        return SessionErrorDialog::tr( "Are you sure you want to remove all currently existing items you are accumulating?\n\nPress OK to remove them all,\nor CANCEL to continue accumulating with the current items." );
    case 101077:
        return SessionErrorDialog::tr( "Please place your item on the scale before adding it to the 'Accumulated Items' list." );
    case 101078:
        return SessionErrorDialog::tr( "This product can only be used in \"Prepack\" mode." );
    case 101079:
        return SessionErrorDialog::tr( "This product cannot be accessed on a self-service scale.  Please contact store personnel for assistance." );
    case 101080:
        return SessionErrorDialog::tr( "The tare on the platter exceeds the maximum tare." );
    case 101081:
        return SessionErrorDialog::tr( "You may not use the print key in\n\"Automatic\" mode." );
    case 101082:
        return SessionErrorDialog::tr( "The maximum number of sides have already been added." );
    case 101083:
        return SessionErrorDialog::tr( "The correct number of sides have not been added to this meal." );
    case 101084:
        return SessionErrorDialog::tr( "Please place the meal on the scale." );
    case 101085:
        return SessionErrorDialog::tr( "This product may only be used as a side item when sides are free." );
    case 101086:
        return SessionErrorDialog::tr( "This item is not available in self-service mode." );
    case 101087:
        return SessionErrorDialog::tr( "Fixed weight items are not available in Production Entry mode when weight verification is enabled." );
    case 101088:
        return SessionErrorDialog::tr( "Fixed weight items are not available for Print Multiple Labels when weight verification is enabled." );
    case 101089:
        return SessionErrorDialog::tr( "Fixed weight items are not available when weight verification is enabled and there is no weigher." );
    case 101090:
        return SessionErrorDialog::tr( "Need to verify the package's weight.  Please place the item on the scale." );
    case 101091:
        return SessionErrorDialog::tr( "UNDER WEIGHT\n\nThis package does not meet minimum weight requirements.\nIt only weighs %s." );
    case 101092:
        return SessionErrorDialog::tr( "This item is not valid for the current cut test because it isn't included in the 'Meat Spec Sheet'." );
    case 101093:
        return SessionErrorDialog::tr( "Fixed weight and fluid ounce items are not allowed in a cutting test." );
    case 101094:
        return SessionErrorDialog::tr( "A remote print request could not be processed due to the scale being in Supervisor Mode.  Please finish and return to Operate mode." );
    case 101095:
        return SessionErrorDialog::tr( "Cannot print remotely to a scale in Service Mode." );
    case 101096:
        return SessionErrorDialog::tr( "Cannot print remotely to a scale in Self Service Mode." );
    case 101097:
        return SessionErrorDialog::tr( "Cannot print remotely to a scale with a product requiring Keyboard Net Weight." );
    case 101098:
        return SessionErrorDialog::tr( "Cannot print remotely to a scale with a product requiring Fixed Weight Verification." );
    case 101099:
        return SessionErrorDialog::tr( "Cannot print Random Weight items at this time." );
    case 101100:
        return SessionErrorDialog::tr( "Cannot print remotely multiple or streaming labels." );
    case 101101:
        return SessionErrorDialog::tr( "A remote print request is waiting to be processed.  Please finish with your Production Entry list." );
    case 101102:
        return SessionErrorDialog::tr( "A remote print request is waiting to be processed.  Please finish with your Accumulate Items list." );
    case 101103:
        return SessionErrorDialog::tr( "Forced Unit Price values cannot be entered at this time." );
    case 101104:
        return SessionErrorDialog::tr( "A valid forced Tare value must be entered.." );
    case 101105:
        return SessionErrorDialog::tr( "A valid forced COOL value must be selected.." );
    case 101106:
        return SessionErrorDialog::tr( "An invalid By Count value was entered that exceeds 99" );
    case 101107:
        return SessionErrorDialog::tr( "The weigher is in an invalid state. Check the scale for the weigher status." );
    case 101108:
        return SessionErrorDialog::tr( "Internal Error. The Scale Session Manager cannot be found." );
    case 101109:
        return SessionErrorDialog::tr( "Internal Error. The Scale DB Configuration cannot be found." );
    case 101110:
        return SessionErrorDialog::tr( "The printer is in an invalid state. Check the scale for the printer status." );
    case 101111:
        return SessionErrorDialog::tr( "Cannot print to the selected production planning label" );
    case 101112:
        return SessionErrorDialog::tr( "Internal Error. The transaction record for the selected item cannot be found." );
    case 101113:
        return SessionErrorDialog::tr( "A remote print request could not be processed due to the existing Build A Meal transaction.  Please finish with this meal." );
    case 101114:
        return SessionErrorDialog::tr( "Invalid product number entered" );
    case 101115:
        return SessionErrorDialog::tr( "Invalid By Count value entered. Please enter a value 1-99." );
    case 101116:
        return SessionErrorDialog::tr( "Invalid Unit Price value entered. Please enter a valid scale value." );
    case 101117:
        return SessionErrorDialog::tr( "Invalid Tare value entered. Please enter a valid scale value." );
    case 101118:
        if(ScaleModel::ScaleModel_ESS == isScale)
            return SessionErrorDialog::tr( "Printhead up-This scale is out of service.\nThe Printhead must be engaged to continue printing.\nPlease contact store personnel."  );
        else
            return SessionErrorDialog::tr( "Label cassette open.Insert label cassette to continue printing.\n\nPlease always use the original printer cassette that was purchased with the scale.\n\nPlease remember to clean the printhead." );
    case 101119:
        return SessionErrorDialog::tr( "Printer label sizing." );
    case 101120:
    {
        if( ScaleModel::ScaleModel_ESS == isScale )
            return SessionErrorDialog::tr( "Label jammed.\n\nThe only available option is to scan a label." );
        else if( ScaleModel::ScaleModel_ESC == isScale )
            return SessionErrorDialog::tr( "Label jammed." );
        else
            return SessionErrorDialog::tr( "Label jammed. Check labels and cycle printhead lever." );
    }
    case 101121:
        return SessionErrorDialog::tr( "Out of labels." );
    case 101122:
        return SessionErrorDialog::tr( "Label cassette missing." );
    case 101123:
        return SessionErrorDialog::tr( "Printer shutdown due to thermal overload." );
    case 101124:
        return SessionErrorDialog::tr( "Printer shutdown due to thermal shutdown." );
    case 101125:
        return SessionErrorDialog::tr( "Printer shutdown due to printer hardware error." );
    case 101126:
        return SessionErrorDialog::tr( "Printer error due to media shutdown." );
    case 101127:
        return SessionErrorDialog::tr( "Cannot print label due to Printer Manager being deleted." );
    case 101128:
        return SessionErrorDialog::tr( "Label Taken Error." );
    case 101129:
        return SessionErrorDialog::tr( "Take Label Error." );
    case 101130:
        return SessionErrorDialog::tr( "This scale is not licensed for Remote Printing." );
    case 101131:
        return SessionErrorDialog::tr( "Cannot create a ProductUpdate() for printing a new label." );
    case 101132:
        return SessionErrorDialog::tr( "Printing on the default label because the overridden label cannot be used." );
    case 101133:
        return SessionErrorDialog::tr( "Printing on corrected default label because the overridden label cannot be used." );
    case 101134:
        return SessionErrorDialog::tr( "Invalid default and overridden label type." );
    case 101135:
        return SessionErrorDialog::tr( "Cannot print on the default or overridden label type because the label cannot fit." );
    case 101136:
        return SessionErrorDialog::tr( "Unknown label error. Cannot not print on the default or overridden label type" );
    case 101137:
        return SessionErrorDialog::tr( "Printing remote label. Please wait ..." );
    case 101138:
        return SessionErrorDialog::tr( "A remote print request is waiting to be processed.  Please finish with the current item." );
    case 101139:
        return SessionErrorDialog::tr( "Forced By Count values cannot be entered at this time." );
    case 101140:
        return SessionErrorDialog::tr( "Label missing. Please print the item again and check if the scale is out of paper." );
    case 101141:
        return SessionErrorDialog::tr( "Cannot print. Peripheral Communication Error." );
    case 101142:
        return SessionErrorDialog::tr( "A Random Weight PLU was sent to the scale. Please add a weight and press the Print button on the scale to receive a label." );
    case 101143:
        return SessionErrorDialog::tr( "This PLU is blocked for scale printing. Enter the quantity made in the Fresh Production App to print labels and keep on-hands correct." );
    case 101144:
        return SessionErrorDialog::tr( "The entered Unit Price exceeds the maximum allowed Unit Price." );
    case 101145:
        return SessionErrorDialog::tr( "A product number was not passed in to be printed." );
    case 101146:
        return SessionErrorDialog::tr( "The entered number of Streaming Labels is 0 or invalid." );
    case 101147:
        return SessionErrorDialog::tr( "Cannot find the current Product to process the message." );
    case 101148:
        return SessionErrorDialog::tr( "Socket timed out waiting to print." );
    case 101149:
        return SessionErrorDialog::tr( "Could not open the socket during remote registration." );
    case 101150:
        return SessionErrorDialog::tr( "Unknown Mode Remote Label Printer Error. Reset the mode to Normal and reboot the scale." );
    case 101151:
        return SessionErrorDialog::tr( "Remote Printing Label Error. Check the scale to see if it is out of labels or has labels jammed. Also check all printer sensors." );
    case 101152:
        return SessionErrorDialog::tr( "The print job has been initiated." );
    case 101153:
        // was 101152
        return SessionErrorDialog::tr( "The TOTAL WEIGHT on the platter %s lb is BELOW \n\nminimum %s lb and maximum %s lb weight for Pick 5.\n\nPlease add more product to print." );
    case 101154:
        // was 101153
        return SessionErrorDialog::tr( "The TOTAL WEIGHT on the platter %s lb is ABOVE \n\nminimum %s lb and maximum %s lb weight for Pick 5.\n\nPlease remove product to print." );
    case 101155:
        // was 101154
        return SessionErrorDialog::tr( "The TOTAL PRICE of the current item $%s is BELOW \n\nminimum $%s and maximum $%s price for Pick 5.\n\nPlease add more product to print." );
    case 101156:
        // was 101155
        return SessionErrorDialog::tr( "The TOTAL PRICE of the current item $%s is ABOVE \n\nminimum $%s and maximum $%s price for Pick 5.\n\nPlease remove product to print." );
    case 101157:
        return SessionErrorDialog::tr( "The Maximum Price is less than the Minimum Price." );
    case 101158:
        return SessionErrorDialog::tr( "The Unit Price is greater than the Maximum Price" );
    case 101159:
        return SessionErrorDialog::tr( "The Unit Price is less than the Minimum Price." );
    case 101160:
        return SessionErrorDialog::tr( "The Maximum Weight is less than the Minimum Weight." );
    case 101161:
        return SessionErrorDialog::tr( "The current Remote Print job has been canceled from the scale. (via UI dialog Cancel button)" );
    case 101162:
        return SessionErrorDialog::tr( "The submitted barcode number is invalid for the Template Label. The printing request has been canceled." );
    case 101163:
        return SessionErrorDialog::tr( "The submitted Template Label does not exist. The printing request has been canceled." );
    case 101164:
        return SessionErrorDialog::tr( "The Remote Print Request has been added to the Print Queue for processing." );
    case 101165:
        return SessionErrorDialog::tr( "Print job queue is full." );
    case 101166:
        return SessionErrorDialog::tr( "Invalid operation occurred while processing job." );
    case 101167:
        return SessionErrorDialog::tr( "Print job queue is empty." );
    case 101168:
        return SessionErrorDialog::tr( "The number of Sell By Days entered is invalid." );
    case 101169:
        return SessionErrorDialog::tr( "Cannot select a Discount Type of Cents off when the number of Currency Precision Digits is set to 0." );
    case 101170:
        return SessionErrorDialog::tr( "The UPC number must be a value greater than 0 if making UPC barcodes." );
    case 101171:
        return SessionErrorDialog::tr( "The prefix must be 2 or 4 if printing UPC barcodes with Random Weight items.\n\nA Prefix not 2 or 4 is only valid when printing non-UPC Barcode types." );
    case 101172:
        return SessionErrorDialog::tr( "A prefix of 3 or 5 is invalid for making UPC barcodes with Fixed Weight items and will not print on the label.\n\nA Prefix of 3 or 5 is valid when printing non-UPC Barcode types." );
    case 101173:
        return SessionErrorDialog::tr( "The assigned nutrifacts label will not be printed because there is no data to fill it." );
    case 101174:
        return SessionErrorDialog::tr( "No Active Cut Test\n\n\nA Cut Test has not been started. The Scale cannot STOP a Cut Test that is not running." );
    case 101175:
        return SessionErrorDialog::tr( "No Active Cut Test\n\n\nA Cut Test has not been started. The Scale cannot SUSPEND a Cut Test that is not running." );
    case 101176:
        return SessionErrorDialog::tr( "  When to add add an Extra Day time is currently set to 00:00AM.\nPlease update the time to add an extra day." );
    case 101177:
        return SessionErrorDialog::tr( "  Add an Extra Sell by Day Feature is already enabled." );
    case 101178:
         return SessionErrorDialog::tr( "Please Click the Done button to leave the Label Placement Screen." );
    case ProductDevError::ProductDevWaitForLabelToPrintOn2P:
         return SessionErrorDialog::tr( "Allow the wrapper to print the current label before requesting new products.\n\nPlease check the last wrapped product and make sure it is labeled correctly." );
    case 101505:
        return SessionErrorDialog::tr( "Invalid label type for this product.\nPlease contact store personnel." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrOperatorDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 102000:
        return SessionErrorDialog::tr( "Unknown operator error." );
    case 102001:
        return SessionErrorDialog::tr( "Operator record not found." );
    case 102002:
        return SessionErrorDialog::tr( "Invalid service Id entered." );
    case 102003:
        return SessionErrorDialog::tr( "The operator you have entered does not exist. Click OK to create a new operator or CANCEL to exit." );
    case 102006:
        return SessionErrorDialog::tr( "The operator number you have entered does not have access to that function." );
    case 102007:
        return SessionErrorDialog::tr( "%s\n" );
    case 102008:
        return SessionErrorDialog::tr( "Cannot delete an operator that does not exist." );
    case 102009:
        return SessionErrorDialog::tr( "Enter your operator ID, or\nHold your ID tag near the scale." );
    case 102010:
        return SessionErrorDialog::tr( "Warning!\n\nIf you renumber an Operator, you will lose any totals or Operator Change records associated with this operator.\nPress OK to continue, or\nPress CANCEL to not proceed with the renumbering." );
    case 102011:
        return SessionErrorDialog::tr( "Enter the operator's new Operator Number." );
    case 102012:
        return SessionErrorDialog::tr( "An operator already exists with that Operator Number." );
    case 102013:
        return SessionErrorDialog::tr( "That RFID tag has already been assigned to operator:\n%s" );
    case 102014:
        return SessionErrorDialog::tr( "That RFID tag has not been assigned to an operator:\n%s" );
    case 102015:
        return SessionErrorDialog::tr( "An operator must be logged in to edit this field." );
    case 102016:
        return SessionErrorDialog::tr( "The current operator does not have access to that field." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrTotalTypeDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 104000:
        return SessionErrorDialog::tr( "Unknown total type error." );
    case 104001:
        return SessionErrorDialog::tr( "Total type error." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrGraphicDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 105000:
        return SessionErrorDialog::tr( "Unknown graphic error." );
    case 105001:
        return SessionErrorDialog::tr( "Graphic error." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrExpandedTextDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 106000:
        return SessionErrorDialog::tr( "Unknown expanded text error." );
    case 106001:
        return SessionErrorDialog::tr( "Expanded text error." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrBackupRestoreDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 109000:
        return SessionErrorDialog::tr( "Unknown backup/restore error." );
    case 109001:
        return SessionErrorDialog::tr( "A file was not selected. Please select a valid file to continue." );
    case 109002:
        return SessionErrorDialog::tr( "A valid file name has not been given.  The file name has no length." );
    case 109003:
        return SessionErrorDialog::tr( "The \\ [slash] character is not allowed in the filename." );
    case 109004:
        return SessionErrorDialog::tr( "The ; [semi-colon] character is not allowed in the filename." );
    case 109005:
        return SessionErrorDialog::tr( "The filename ends in a slash [/].  Please enter a valid file name." );
    case 109006:
        return SessionErrorDialog::tr( "The * [asterisk] character is not allowed in the filename." );
    case 109007:
        return SessionErrorDialog::tr( "The ? [question mark] character is not allowed in the filename." );
    case 109008:
        return SessionErrorDialog::tr( "The \" or \' [single or double quotes] character is not allowed in the filename." );
    case 109009:
        return SessionErrorDialog::tr( "The < or > [qreater or less than] character is not allowed in the filename." );
    case 109010:
        return SessionErrorDialog::tr( "The + or - [plus or minus] character is not allowed in the filename." );
    case 109011:
        return SessionErrorDialog::tr( "The | [separator bar] character is not allowed in the filename." );
    case 109012:
        return SessionErrorDialog::tr( "The [space] character is not allowed in the filename." );
    case 109013:
        return SessionErrorDialog::tr( "No data was selected to backup/restore/merge." );
    case 109014:
        return SessionErrorDialog::tr( "Deleting product data.\nPlease wait..." );
    case 109015:
        return SessionErrorDialog::tr( "Deleting resource data.\nPlease wait..." );
    case 109016:
        return SessionErrorDialog::tr( "Deleting operator data.\nPlease wait..." );
    case 109017:
        return SessionErrorDialog::tr( "Deleting self serve data.\nPlease wait..." );
    case 109018:
        return SessionErrorDialog::tr( "Deleting COOL data.\nPlease wait..." );
    case 109019:
        return SessionErrorDialog::tr( "Deleting flashkey data.\nPlease wait..." );
    case 109020:
        return SessionErrorDialog::tr( "Error writing to the backup file.  The USB drive may not be inserted properly, or it may be full." );
    case 109021:
        return SessionErrorDialog::tr( "Counting records to backup.\nPlease wait..." );
    case 109022:
        return SessionErrorDialog::tr( "Self Service Configuration has been modified.\n\nPLEASE REBOOT" );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrClassDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 110000:
        return SessionErrorDialog::tr( "Unknown class error." );
    case 110001:
        return SessionErrorDialog::tr( "Class record not found.\nWould you like to create a new one?" );
    case 110002:
        return SessionErrorDialog::tr( "Class record not found.\nWould you like to create a new one?" );
    case 110003:
        return SessionErrorDialog::tr( "Reporting class not found." );
    case 110004:
        return SessionErrorDialog::tr( "Cannot add new classes to the broken links class." );
    case 110005:
        return SessionErrorDialog::tr( "New classes may not be added to the All Classes Root Node. They must be added to class 999999 or deeper." );
    case 110006:
        return SessionErrorDialog::tr( "A class cannot report to itself." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrConfigurationDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 112000:
        return SessionErrorDialog::tr( "Unknown Marquee Error." );
    case 112002:
        return SessionErrorDialog::tr( "Saving Marquee data to the database ....." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrVersionManagerDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    QString currentLang;
    QString retMsg;
    QStringList args;

    switch ( code )
    {
    case 120001:
        return SessionErrorDialog::tr( "Backing up the database, preparing for new scale firmware.\nDO NOT TURN OFF THE SCALE!" );
    case 120002:
        // This is the only message like this in the entire system.
        // Gets called during an upgrade before the language is known

        {
            python pythonProcess;
            pythonProcess.execCommand("languageSettings", args);

            if (pythonProcess.getExitCode() == 0)
            {
                currentLang = pythonProcess.readStdOut();
            }
            else
            {
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }

        // Do not translate any of these
        if(currentLang == "spanish")
            retMsg = "Restaurando la base dedatos después de que el nuevo firmware ha sido cargado.\n¡NO APAGUE LA BALANZA!";
        else if( currentLang == "french" )
            retMsg = "Restauration de la base de données à la suite du chargement du nouveau micrologiciel.\nNE PAS ÉTEINDRE LA BALANCE!";
        else
            retMsg =  "Restoring the database after new firmware was loaded.\nDO NOT TURN OFF THE SCALE!";

        return retMsg;
    case 120003:
        return SessionErrorDialog::tr( "Checking for new scale firmware.\nDO NOT TURN OFF THE SCALE!\nIn the event an upgrade is available the scale will reboot itself..." );
    case 120004:
        return SessionErrorDialog::tr( "An error occurred during the process of loading new scale firmware." );
    case 120005:
        return SessionErrorDialog::tr( "Current software system invalid." );
    case 120006:
        return SessionErrorDialog::tr( "Resultant software system would be invalid." );
    case 120007:
        return SessionErrorDialog::tr( "Nothing new to install." );
    case 120008:
        return SessionErrorDialog::tr( "USB Peripheral Upgrade Complete." );
    case 120009:
        return SessionErrorDialog::tr( "No path to evolution1.tar." );
    case 120010:
        return SessionErrorDialog::tr( "No path to evolution2.tar." );
    case 120011:
        return SessionErrorDialog::tr( "Starting SC Serial EEP Write." );
    case 120012:
        return SessionErrorDialog::tr( "SC Serial EEP Write Complete." );
    case 120013:
        return SessionErrorDialog::tr( "SC Serial EEP Write Failed." );
    case 120014:
        return SessionErrorDialog::tr( "USB Weigher Code Downloading." );
    case 120015:
        return SessionErrorDialog::tr( "USB Weigher Code Download Complete." );
    case 120016:
        return SessionErrorDialog::tr( "USB Weigher Code Hardware Version Mismatch." );
    case 120017:
        return SessionErrorDialog::tr( "Cant find Evolution.ini file." );
    case 120018:
        return SessionErrorDialog::tr( "Loading data files.\nPlease wait." );
    case 120019:
        return SessionErrorDialog::tr( "Loading complete." );
    case 120020:
        return SessionErrorDialog::tr( "USB Printer Code Downloading." );
    case 120021:
        return SessionErrorDialog::tr( "USB Printer Code Download Complete." );
    case 120022:
        return SessionErrorDialog::tr( "USB Printer Code Hardware Version Mismatch." );
    case 120023:
        return SessionErrorDialog::tr( "Failed to flash printer too many times. Try new printer or new software." );
    case 120024:
        return SessionErrorDialog::tr( "To delete the Wrapper upgrade files from the System, press OK.\nTo keep the files, press Cancel.\n\nNote: If you delete the files, you will need files\non a USB drive to upgrade the next time." );
    case 120025:
        return SessionErrorDialog::tr( "Weigher/Printer Code Downloading ..." );
    case 120026:
        return SessionErrorDialog::tr( "Weigher/Printer Code Download Complete." );
    case 120027:
        return SessionErrorDialog::tr( "Weigher/Printer Code Download FAILED!" );
    case 120028:
        return SessionErrorDialog::tr( "Weigher/Printer is resetting ..." );
    case 120029:
        return SessionErrorDialog::tr( "Atmel Device Downloading ..." );
    case 120030:
        return SessionErrorDialog::tr( "Atmel Device Download Complete." );
    case 120031:
        return SessionErrorDialog::tr( "Atmel Device Download FAILED!" );
    case 120080:
        return SessionErrorDialog::tr( "Please remove the USB Bulk Storage Device." );
    case 120081:
        return SessionErrorDialog::tr( "To delete the CLA upgrade files from the System, press OK.\nTo keep the files, press Cancel.\n\nNote: If you delete the files, you will need files\non a USB drive to upgrade the next time." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrCategoryDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 121000:
        return SessionErrorDialog::tr( "Unknown category setup error." );
    case 121001:
        return SessionErrorDialog::tr( "Category does not exist.\nWould you like to create a new one?" );
    case 121002:
        return SessionErrorDialog::tr( "Are you sure you want to delete this category?" );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrNetworkDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 122000:
        return SessionErrorDialog::tr( "Unknown network error." );
    case 122001:
        return SessionErrorDialog::tr( "Network setup has failed." );
    case 122002:
        return SessionErrorDialog::tr( "DNS setup has failed." );
    case 122003:
        return SessionErrorDialog::tr( "Invalid encryption key code." );
    case 122004:
        return SessionErrorDialog::tr( "Reinitializing the network. Please wait." );
    case 122005:
        return SessionErrorDialog::tr( "Reinitializing DNS. Please wait." );
    case 122006:
        return SessionErrorDialog::tr( "Reinitializing the wireless card. Please wait." );
    case 122007:
        return SessionErrorDialog::tr( "For DNS resolver setup to complete,\nMake sure the network uses DHCP then reboot." );
    case 122008:
        return SessionErrorDialog::tr( "LEAP authentication successful." );
    case 122009:
        return SessionErrorDialog::tr( "LEAP authentication failed." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrTotalsDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 123001:
        return SessionErrorDialog::tr( "There are no totals for this Product." );
    case 123002:
        return SessionErrorDialog::tr( "There are no totals for this Operator." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrSpecialsDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 124001:
        return SessionErrorDialog::tr( "This special does not exist.\nWould you like to create a new one?" );
    default:
        break;
    }

    return QString();
}
template <>
QString SessionErrorDialog::messageStringT<ErrOperatorChangesDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 131001:
        return SessionErrorDialog::tr( "Getting Operator Changes Records.\nPlease wait ....." );
    case 131002:
        return SessionErrorDialog::tr( "No Operator Changes Records exist for these parameters." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrCOOLDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 135001:
        return SessionErrorDialog::tr( "The COOL Category you have entered does not exist. Click OK to create a new category or CANCEL to exit." );
    case 135002:
        return SessionErrorDialog::tr( "The COOL Country you have entered does not exist. Click OK to create a new country or CANCEL to exit." );
    case 135003:
        return SessionErrorDialog::tr( "The COOL Production Step you have entered does not exist. Click OK to create a new production step or CANCEL to exit." );
    case 135004:
        return SessionErrorDialog::tr( "The COOL Pre-text you have entered does not exist. Click OK to create a new pre-text or CANCEL to exit." );
    case 135005:
        return SessionErrorDialog::tr( "The Operator must either select a COOL text, create new COOL text and select it, or select COOL Not Required. If COOL Tracking Number is required you also have to enter a Tracking Number." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrSoftkeyDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 136001:
        return SessionErrorDialog::tr( "Flashkey not found." );
    case 136002:
        return SessionErrorDialog::tr( "Please select a flashkey to edit." );
    case 136004:
        return SessionErrorDialog::tr( "Please enter a web site address." );
    case 136005:
        return SessionErrorDialog::tr( "Please enter an operator ID." );
    case 136006:
        return SessionErrorDialog::tr( "Please enter a product number" );
    case 136007:
        return SessionErrorDialog::tr( "Please enter a group number." );
    case 136008:
        return SessionErrorDialog::tr( "Please enter a cents off discount amount." );
    case 136009:
        return SessionErrorDialog::tr( "Please enter a percent off discount amount." );
    case 136010:
        return SessionErrorDialog::tr( "Please enter a string to search." );
    case 136011:
        return SessionErrorDialog::tr( "Illegal web address.  Please begin the web page address as http://" );
    case 136012:
        return SessionErrorDialog::tr( "You must first enter a product number to continue." );
    case 136013:
        return SessionErrorDialog::tr( "You must first enter a category number to continue." );
    case 136014:
        return SessionErrorDialog::tr( "You must first enter a tare value to continue." );
    case 136015:
        return SessionErrorDialog::tr( "The scale is currently configured for a single language.  A second language must be loaded in order to use the Change Language feature." );
    case 136016:
        return SessionErrorDialog::tr( "The address location specified for this flashkey is blank.  Please check the address in the flashkey editor." );
    case 136201:
        return SessionErrorDialog::tr( "Flashkey Feature not found." );
    case 136202:
        return SessionErrorDialog::tr( "Please select a flashkey Feature in addition to entering a flashkey Group number." );
    case 136203:
        return SessionErrorDialog::tr( "Please select a flashkey Feature before editing any cells." );
    case 136204:
        return SessionErrorDialog::tr( "The maximum number of flashkey rows have been reached." );
    case 136205:
        return SessionErrorDialog::tr( "The minimum number of flashkey rows have been reached." );
    case 136206:
        return SessionErrorDialog::tr( "The maximum number of flashkey columns have been reached." );
    case 136207:
        return SessionErrorDialog::tr( "The minimum number of flashkey columns have been reached." );
    case 136208:
        return SessionErrorDialog::tr( "Please select a cell to increase in size." );
    case 136209:
        return SessionErrorDialog::tr( "You cannot span rows past the last row of flashkeys." );
    case 136210:
        return SessionErrorDialog::tr( "Please select a cell to increase in size." );
    case 136211:
        return SessionErrorDialog::tr( "You cannot span cols past the last column of flashkeys." );
    case 136212:
        return SessionErrorDialog::tr( "Please select a cell to decrease in size." );
    case 136213:
        return SessionErrorDialog::tr( "The current cell is already the minimum size." );
    case 136214:
        return SessionErrorDialog::tr( "Please select a cell to decrease in size." );
    case 136215:
        return SessionErrorDialog::tr( "The current cell is already the minimum size." );
    case 136216:
        return SessionErrorDialog::tr( "You must first create (edit) the cell before you can modify the size." );
    case 136217:
        return SessionErrorDialog::tr( "You must first delete the adjacent cell before expanding the currently selected flashkey." );
    case 136218:
        return SessionErrorDialog::tr( "You may not delete a cell that does not currently exist." );
    case 136219:
        return SessionErrorDialog::tr( "The currently selected cell is already at it's minimum span size.  Delete the cell if you wish to completely remove it." );
    case 136220:
        return SessionErrorDialog::tr( "Please select a flashkey to delete." );
    case 136401:
        return SessionErrorDialog::tr( "Flashkey Style not found." );
    case 136601:
        return SessionErrorDialog::tr( "Flashkey Group does not exist. Would you like to create it?" );
    case 136602:
        return SessionErrorDialog::tr( "You must have a valid flashkey group number entered." );
    case 136603:
        return SessionErrorDialog::tr( "The entered flashkey Group does not exist.  Begin creating flashkeys to create this group." );
    case 136604:
        return SessionErrorDialog::tr( "Initializing ......." );
    case 136605:
        return SessionErrorDialog::tr( "The deleted row contains flashkey items.  Are you sure you want to delete this row?" );
    case 136606:
        return SessionErrorDialog::tr( "Deleting this row will delete spanned cells.  Shorten the spanned cells to continue." );
    case 136607:
        return SessionErrorDialog::tr( "The deleted column contains flashkey items.  Are you sure you want to delete this column?" );
    case 136608:
        return SessionErrorDialog::tr( "Deleting this column will delete spanned cells.  Shorten the spanned cells to continue." );
    case 136801:
        return SessionErrorDialog::tr( "The following operation will delete the currently loaded flashkey group.  Are you sure you want to delete the current group?" );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrImportDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 144000:
        return SessionErrorDialog::tr( "Unknown Import error." );
    case 144001:
        return SessionErrorDialog::tr( "Importing graphics and fonts from USB drive." );
    case 144002:
        return SessionErrorDialog::tr( "Importing graphics from USB drive." );
    case 144003:
        return SessionErrorDialog::tr( "Import complete." );
    case 144004:
        return SessionErrorDialog::tr( "Import complete. Please reboot to make new fonts available." );
    case 144005:
        return SessionErrorDialog::tr( "Import complete. Source directory does not exist." );
    case 144006:
        return SessionErrorDialog::tr( "All files copied from USB drive. It is ok to remove USB drive. Please wait for files to be properly stored..." );
    case 144007:
        return SessionErrorDialog::tr( "A file cannot be imported.  Please select directories only." );
    case 144008:
        return SessionErrorDialog::tr( "Export complete." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrNetworkConnectivityDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 145000:
        return SessionErrorDialog::tr( "Unknown Network Connectivity error." );
    case 145001:
        return SessionErrorDialog::tr( "Lost network connectivity.\nPlease check to see if ethernet cable is plugged in properly.\nIf using wireless make sure you can connect to access point." );
    case 145002:
        return SessionErrorDialog::tr( "Lost connection to Graphical Customer Display" );
    case 145003:
        return SessionErrorDialog::tr( "Restarting Graphical Customer Display" );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrSynchronizationDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 146000:
        return SessionErrorDialog::tr( "Unknown status" );
    case 146001:
        return SessionErrorDialog::tr( "Synchronization active" );
    case 146002:
        return SessionErrorDialog::tr( "Sending changes" );
    case 146003:
        return SessionErrorDialog::tr( "Receiving changes" );
    case 146004:
        return SessionErrorDialog::tr( "Applying changes" );
    case 146005:
        return SessionErrorDialog::tr( "Can't find primary" );
    case 146006:
        return SessionErrorDialog::tr( "Clear" );
    case 146007:
        return SessionErrorDialog::tr( "Synchronizing scales" );
    case 146020:
        return SessionErrorDialog::tr( "Synchronizing scales" );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrTransactionDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 151002:
        return SessionErrorDialog::tr( "No Voidable Transactions exist for these parameters." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrHourlyTotalDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 153001:
        return SessionErrorDialog::tr( "Getting Hourly Totals Records.\nPlease wait ....." );
    case 153002:
        return SessionErrorDialog::tr( "No Hourly Totals Records exist for these parameters." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrOperatorTotalDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 154001:
        return SessionErrorDialog::tr( "Getting Operator Totals Records.\nPlease wait ....." );
    case 154002:
        return SessionErrorDialog::tr( "No Operator Totals Records exist for these parameters." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrProductTotalDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 156001:
        return SessionErrorDialog::tr( "Getting Product Totals Records.\nPlease wait ....." );
    case 156002:
        return SessionErrorDialog::tr( "No Product Totals Records exist for these parameters." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrWindowsConfigurationDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 158001:
        return SessionErrorDialog::tr( "Wireless Profile Backup Complete." );
    case 158002:
        return SessionErrorDialog::tr( "Wireless Profile Backup Failed." );
    case 158003:
        return SessionErrorDialog::tr( "Wireless Profile Restore Complete." );
    case 158004:
        return SessionErrorDialog::tr( "Wireless Profile Restore Failed." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrCutTestDev>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 159001:
        return SessionErrorDialog::tr( "The maximum number of Suspended Cutting Tests have been reached for this operator. Please delete an existing Cutting Test or finish an existing Cutting Test that was previously suspended." );
    case 159002:
        return SessionErrorDialog::tr( "The maximum number of Suspended Cutting Tests have been reached. Please delete an existing Cutting Test or finish an existing Cutting Test that was previously suspended." );
    case 159003:
        return SessionErrorDialog::tr( "The current Cutting Test has been suspended due to inactivity." );
    case 159004:
        return SessionErrorDialog::tr( "The selected test does not have a valid product and can only be deleted." );
    case 159005:
        return SessionErrorDialog::tr( "You cannot delete a Cutting Test that has been completed." );
    case 159006:
        return SessionErrorDialog::tr( "You cannot delete this Cutting Test.  It has already been deleted." );
    case 159007:
        return SessionErrorDialog::tr( "You do not have the access level to delete this Cutting Test." );
    case 159008:
        return SessionErrorDialog::tr( "You may only delete your own Cutting Tests." );
    case 159009:
        return SessionErrorDialog::tr( "You may only resume your own Cutting Tests." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrGCDSessionDevice>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 161002:
        return SessionErrorDialog::tr( "Lost connection to Graphical Customer Display" );
    case 161003:
        return SessionErrorDialog::tr( "Restarting Graphical Customer Display" );
    case 161004:
        return SessionErrorDialog::tr( "The Graphical Customer Display firmware is being updated.\nDO NOT TURN OFF THE SCALE!" );
    case 161005:
        return SessionErrorDialog::tr( "The Graphical Customer Display media is being updated.\nDO NOT TURN OFF THE SCALE!" );
    case 161006:
        return SessionErrorDialog::tr( "A Graphical Customer Display media update is already in progres. Please wait for it's completion before starting a new update..." );
    case 161007:
        return SessionErrorDialog::tr( "The scale must be reset in order to activate the Customer Display configuration change." );
    case 161008:
        return SessionErrorDialog::tr( "The GCD media update has failed. The USB drive can be removed now" );
    case 161009:
        return SessionErrorDialog::tr( "The GCD media update was successfully completed. The USB drive can be removed now" );
    case 161010:
        return SessionErrorDialog::tr( "The GCD media update has failed." );
    case 161011:
        return SessionErrorDialog::tr( "The GCD media update was successfully completed." );
    case 161012:
        return SessionErrorDialog::tr( "The GCD firmware update has failed." );
    case 161013:
        return SessionErrorDialog::tr( "The GCD firmware update was successfully completed." );
    case 161014:
        return SessionErrorDialog::tr( "The GCD firmware update has failed. The USB drive can be removed now." );
    case 161015:
        return SessionErrorDialog::tr( "The GCD firmware update was successfully completed. The USB drive can be removed now." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrDeviceDev>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 171001:
        return SessionErrorDialog::tr( "Device not found." );
    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrDiagnostics>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 173000:
        return SessionErrorDialog::tr( "Could not run diagnostics:\n  1) Event is too old and diagnostics no longer available\n  2) This event is not diagnosable" );
    case 173001:
        return SessionErrorDialog::tr( "Please wait while diagnostics is initialized!" );
    case 173002:
        return SessionErrorDialog::tr( "Please wait for diagnostics to complete!" );
    case 173003:
        return SessionErrorDialog::tr( "Diagnostics failed to initialize." );
    case 173004:
        return SessionErrorDialog::tr( "Diagnostics testing failed to complete." );
    case 173005:
        return SessionErrorDialog::tr( "Maintenance log export complete!" );
    case 173006:
        return SessionErrorDialog::tr( "Maintenance log export failed!\n\nPlease check destination drive access and permissions and try again." );
    case 173007:
        return SessionErrorDialog::tr( "Send of maintenance log complete!" );
    case 173008:
        return SessionErrorDialog::tr( "Send of maintenance log failed!\n\nPlease check configuration and network status and try again." );
    case 173100:
        return SessionErrorDialog::tr( "Please select one (and only one) file." );
    case 173101:
    case 173102:
        return SessionErrorDialog::tr( "Please select a file." );
    case 173103:
        return SessionErrorDialog::tr( "Invalid characters in the filename.\n\nPlease try again using valid file characters." );

    default:
        break;
    }

    return QString();
}

template <>
QString SessionErrorDialog::messageStringT<ErrDepartments>( int code, int isScale )
{
    Q_UNUSED( isScale )

    switch ( code )
    {
    case 175001:
        return SessionErrorDialog::tr( "The Default Department cannot be modified." );
    case 175002:
        return SessionErrorDialog::tr( "The selected department record cannot found in the database." );
    case 175003:
        return SessionErrorDialog::tr( "You are trying to delete the Current Active Department.\nPress OK to continue?" );
    case 175004:
        return SessionErrorDialog::tr( "Deleting department-related data.\nPlease wait ..." );
    default:
        break;
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString SessionErrorDialog::messageString(int code , int isScale)
{
    LOG_CLASS_INFO( "SessionErrorDialog", "messageString" );

    int event = (code / 1000);

    LOG_INFO("event = %d", event);
    LOG_INFO("code = %d", code);

    QString m;

    switch ( event )
    {
    case 0:
        m = messageStringT<0>( code, isScale ); // debug message
        break;
    case ErrWeigherDevice:
        m = messageStringT<ErrWeigherDevice>( code, isScale );
        break;
//    case ErrEventDevice:
//        m = messageStringT<ErrEventDevice>( code, isScale );
//        break;
    case ErrOperationModeDevice:
        m = messageStringT<ErrOperationModeDevice>( code, isScale );
        break;
    case ErrLabelApplierDevice:
        m = messageStringT<ErrLabelApplierDevice>( code, isScale );
        break;
    case ErrPrinterDevice:
        m = messageStringT<ErrPrinterDevice>( code, isScale );
        break;
    case ErrWrapperDevice:
        m = messageStringT<ErrWrapperDevice>( code, isScale );
        break;
    case ErrWrapperStatusDevice:
        m = messageStringT<ErrWrapperStatusDevice>( code, isScale );
        break;
    case ErrTouchScreenDevice:
        m = messageStringT<ErrTouchScreenDevice>( code, isScale );
        break;
    case ErrBarcodeScanner:
        m = messageStringT<ErrBarcodeScanner>( code, isScale );
        break;
    case ErrSessionDevice:
        m = messageStringT<ErrSessionDevice>( code, isScale );
        break;
    case ErrProductDevice:
        m = messageStringT<ErrProductDevice>( code, isScale );
        break;
    case ErrOperatorDevice:
        m = messageStringT<ErrOperatorDevice>( code, isScale );
        break;
    case ErrTotalTypeDevice:
        m = messageStringT<ErrTotalTypeDevice>( code, isScale );
        break;
    case ErrGraphicDevice:
        m = messageStringT<ErrGraphicDevice>( code, isScale );
        break;
    case ErrExpandedTextDevice:
        m = messageStringT<ErrExpandedTextDevice>( code, isScale );
        break;
    case ErrBackupRestoreDevice:
        m = messageStringT<ErrBackupRestoreDevice>( code, isScale );
        break;
    case ErrClassDevice:
        m = messageStringT<ErrClassDevice>( code, isScale );
        break;
    case ErrConfigurationDevice:
        m = messageStringT<ErrConfigurationDevice>( code, isScale );
        break;
    case ErrVersionManagerDevice:
        m = messageStringT<ErrVersionManagerDevice>( code, isScale );
        break;
    case ErrCategoryDevice:
        m = messageStringT<ErrCategoryDevice>( code, isScale );
        break;
    case ErrNetworkDevice:
        m = messageStringT<ErrNetworkDevice>( code, isScale );
        break;
    case ErrTotalsDevice:
        m = messageStringT<ErrTotalsDevice>( code, isScale );
        break;
    case ErrSpecialsDevice:
        m = messageStringT<ErrSpecialsDevice>( code, isScale );
        break;
    case ErrOperatorChangesDevice:
        m = messageStringT<ErrOperatorChangesDevice>( code, isScale );
        break;
    case ErrCOOLDevice:
        m = messageStringT<ErrCOOLDevice>( code, isScale );
        break;
    case ErrSoftkeyDevice:
        m = messageStringT<ErrSoftkeyDevice>( code, isScale );
        break;
    case ErrImportDevice:
        m = messageStringT<ErrImportDevice>( code, isScale );
        break;
    case ErrNetworkConnectivityDevice:
        m = messageStringT<ErrNetworkConnectivityDevice>( code, isScale );
        break;
    case ErrSynchronizationDevice:
        m = messageStringT<ErrSynchronizationDevice>( code, isScale );
        break;
    case ErrTransactionDevice:
        m = messageStringT<ErrTransactionDevice>( code, isScale );
        break;
    case ErrHourlyTotalDevice:
        m = messageStringT<ErrHourlyTotalDevice>( code, isScale );
        break;
    case ErrOperatorTotalDevice:
        m = messageStringT<ErrOperatorTotalDevice>( code, isScale );
        break;
    case ErrProductTotalDevice:
        m = messageStringT<ErrProductTotalDevice>( code, isScale );
        break;
    case ErrWindowsConfigurationDevice:
        m = messageStringT<ErrWindowsConfigurationDevice>( code, isScale );
        break;
    case ErrCutTestDev:
        m = messageStringT<ErrCutTestDev>( code, isScale );
        break;
    case ErrGCDSessionDevice:
        m = messageStringT<ErrGCDSessionDevice>( code, isScale );
        break;
    case ErrDeviceDev:
        m = messageStringT<ErrDeviceDev>( code, isScale );
        break;
    case ErrDiagnostics:
        m = messageStringT<ErrDiagnostics>( code, isScale );
        break;
    case ErrDepartments:
        m = messageStringT<ErrDepartments>( code, isScale );
//    case ErrEOFDevice:
//        m = messageStringT<ErrEOFDevice>( code, isScale );
//        break;
    default:
        break;
    }

    if ( !m.length() )
        m = tr( "You have received a critical error, but I don't recognize the error code:\n\n" ) + QString::number( code );

    return m;
}


