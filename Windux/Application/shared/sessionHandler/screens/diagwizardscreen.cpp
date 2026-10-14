/**
 * @file diagwizardscreen.h
 * Diagnostics wizard screen.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "diagwizardscreen.h"
#include "serviceHealthAgent.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../dialogs/messageboxdialog.h"
#include "../dialogs/sessionerrordialog.h"
#include "../dialogs/wrappercalibrationdialog.h"

#include "../modes/servicemode.h"
#include "../modes/supervisormode.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <logmanager.h>
#include <palette.h>
#include <filesystem.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsWizardScreen::DiagnosticsWizardScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    errorInfo_diagErrorCode( 0 ),
    state_( Unknown ),
    failureCode_( 0 ),
    serviceContactInfo_( "1.888.446.2278" )
{
    // initialize
    initialize();
    createLayout();
    translate();

    // request diagnostic configuration
    session_->send( Diagnostics, DiagConfigurationReq );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsWizardScreen::~DiagnosticsWizardScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::showWizard( const DiagShowWizardMsg &msg )
{
    // set state
    state_ = Init;
    setErrorInfo( msg );

    // diable next button until backend ready
    //next_->setVisible( true ); // RLBDIAG - Uncomment when supported
    next_->setEnabled( false );

    //home_->setVisible( false ); // RLBDIAG - Uncomment when supported

    healthAgent_->setVisible( false );

    // initialize wizard
    session_->send( Diagnostics, DiagInitWizardReq, errorInfo_diagEvent );
    wizardInitTimer_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::translate()
{
    title_->setText( tr( "DIAGNOSTICS" ) );
    errorCode_->setText( tr( "Error Code" ) + " " );

    next_->setText( tr( "Next" ) );
    home_->setText( tr( "Dismiss" ) ); // RLBDIAG - Revert when supported        "Return Home"
    calibrate_->setText( tr( "Calibrate Wrapper" ) );
    healthAgent_->setText( tr( "Open Health Agent" ) );

    refreshErrorInfo();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::resizeEvent( QResizeEvent *e )
{
    // make sure the photo has a fixed width
    photo_->setFixedWidth( (width() * 55) / 100 );

    refreshPhoto();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::setErrorInfo( const DiagShowWizardMsg &msg )
{
    // clear out existing photos
    diagPhotos_.clear();

    // setup error info
    errorInfo_event_ = msg.event();
    errorInfo_diagEvent = msg.diagEvent();
    errorInfo_diagErrorCode = msg.diagErrorCode();

    // refresh
    refreshErrorInfo();

    // set text
    stamp_->setText( msg.timestamp() );
    hardwareModel_->setText( msg.hardwareModel() + " " );
    hardwareSerial_->setText( msg.hardwareSerial() );
    hardwareCycles_->setText( QString::number( msg.hardwareCycles() ) );

    // start animating
    animationTimer_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsWizardScreen::onScreenChanging()
{
    LOG_CLASS_INFO( "DiagnosticsWizardScreen", "onScreenChanging" );

    // don't allow screen change when testing in progress
    if ( wizardInitTimer_->isActive() )
    {
        LOG_WARN( "cannot change screens, waiting for init response", NULL );
        session_->send( Error, ErrDiagnostics, "173001,5" );

        return false;
    }
    else if ( wizardTestingTimer_->isActive() )
    {
        LOG_WARN( "cannot change screens, waiting for testing response", NULL );
        session_->send( Error, ErrDiagnostics, "173002,5" );

        return false;
    }

    // check base class
    if ( !_Mybase::onScreenChanging() )
        return false;

    // exit diag wizard
    session_->send( Diagnostics, DiagExitWizard );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "DiagnosticsWizardScreen", "onSessionMessage" );

    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    if (( Diagnostics == device ) && ( DiagConfiguration == event ))
    {
        DiagConfigurationMsg m( msg );

        QString serviceContactInfo( m.serviceContactInfo() );

        if ( serviceContactInfo.length() )
            serviceContactInfo_ = serviceContactInfo;
    }

    else if (( Diagnostics == device ) && ( DiagInitWizard == event ))
    {
        DiagInitWizardMsg m( msg );

        // make sure this is our message
        if (( errorInfo_diagEvent == m.diagEvent() ) && (  Init == state_ ))
        {
            // stop timer, we have response
            wizardInitTimer_->stop();

            // check response
            if ( m.ready() )
            {
                LOG_INFO( "wizard init complete", NULL );

                state_ = Ready;
                refreshErrorInfo();

                // enable button
                next_->setEnabled( true );

                processErrorSpecificEvents();
            }
            else
            {
                LOG_WARN( "wizard init failed %d %s", m.errorCode(), qPrintable(m.errorMessage()) );

                state_ = CancelledInit;
                refreshErrorInfo();

                // show message
                QString text;
                text.append( tr( "Failed to initialize diagnostics" ) + " (" + QString::number( m.errorCode() ) + ")\n" );
                text.append( "\n");
                text.append( m.errorMessage() );

                MessageBoxDialog *dialog( new MessageBoxDialog( mainWindow_ ) );
                dialog->setTitle( tr( "DIAGNOSTICS" ) );
                dialog->setText( text );
                dialog->setStandardButtons( QDialogButtonBox::Ok );

                dialog->open();
            }
        }
    }

    else if (( Diagnostics == device ) && ( DiagPerformWizardTesting == event ))
    {
        DiagPerformWizardTestingMsg m( msg );

        // make sure this is our message
        if (( errorInfo_diagEvent == m.diagEvent() ) && ( Testing == state_ ))
        {
            // stop timer, we have response
            wizardTestingTimer_->stop();

            // switch buttons
            next_->setVisible( false );
            home_->setVisible( true );

            // process result
            if ( m.result() )
            {
                LOG_INFO( "wizard diag testing passed %s", qPrintable(errorInfo_diagEvent) );
                failureCode_ = 0;

                if ( mainWindow_->isServiceMode() )
                    showHealthAgentButton();

                MessageBoxDialog *dialog( new MessageBoxDialog( mainWindow_ ) );
                dialog->setTitle( tr( "DIAGNOSTICS" ) );
                dialog->setText( testingPassedText() );
                dialog->setStandardButtons( QDialogButtonBox::Ok );

                dialog->open();
            }
            else
            {
                LOG_WARN( "wizard diag testing failed %s %d", qPrintable(errorInfo_diagEvent), m.errorCode() );
                failureCode_ = m.errorCode();

                showHealthAgentButton();

                MessageBoxDialog *dialog( new MessageBoxDialog( mainWindow_ ) );
                dialog->setTitle( tr( "DIAGNOSTICS" ) );
                dialog->setText( testingFailedText( m.errorCode() ) );
                dialog->setStandardButtons( QDialogButtonBox::Ok );

                dialog->open();
            }

            state_ = Complete;
            refreshErrorInfo();
        }
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::onCalibrateWrapper()
{
    WrapperCalibrationDialog *d( new WrapperCalibrationDialog( true, mainWindow_ ) );

    // handle any error specific events
    if ( 9521 == errorInfo_diagErrorCode )
        d->addMechanism( 0, tr( "Primary Intake" ) );

    d->open();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::onClicked()
{
    LOG_CLASS_INFO( "DiagnosticsWizardScreen", "onSessionMessage" );

    if ( next_ == sender() )
    {
        LOG_INFO( "starting test for diag event %s...", qPrintable(errorInfo_diagEvent) );

        state_ = Testing;
        refreshErrorInfo();

        // disable button
        next_->setEnabled( false );

        // start test
        session_->send( Diagnostics, DiagPerformWizardTestingReq, errorInfo_diagEvent );
        wizardTestingTimer_->start();

    }
    else if ( home_ == sender() )
    {
        LOG_INFO( "returning home", NULL );

        mainWindow_->navigateHome();
    }
    else if ( calibrate_ == sender() )
    {
        LOG_INFO( "calibrating", NULL );

        onCalibrateWrapper();
    }
    else if ( healthAgent_ == sender() )
    {
        LOG_INFO( "opening health agent", NULL );

        if ( mainWindow_->isServiceMode() )
            mainWindow_->switchToModeScreen<ServiceMode, serviceHealthAgent>();
        else
            mainWindow_->switchToModeScreen<SupervisorMode, serviceHealthAgent>();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    OperateMode *operateMode( dynamic_cast<OperateMode*>( mode_ ) );

    // hide weight bar
    if ( operateMode )
        operateMode->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::onTimeout()
{
    LOG_CLASS_INFO( "DiagnosticsWizardScreen", "onTimeout" );

    if ( wizardInitTimer_ == sender() )
    {
        LOG_WARN( "timeout waiting for init to complete!", NULL );

        state_ = CancelledInit;
        refreshErrorInfo();

        // switch buttons
        next_->setVisible( false );
        home_->setVisible( true );

        if ( mainWindow_->isServiceMode() )
            showHealthAgentButton();

        session_->send( Error, ErrDiagnostics, "173003,5" );
    }
    else if ( wizardTestingTimer_ == sender() )
    {
        LOG_WARN( "timeout waiting for testing to complete!", NULL );

        state_ = CancelledTesting;
        refreshErrorInfo();

        // switch buttons
        next_->setVisible( false );
        home_->setVisible( true );

        if ( mainWindow_->isServiceMode() )
            showHealthAgentButton();

        session_->send( Error, ErrDiagnostics, "173004,5" );
    }
    else if ( animationTimer_ == sender() )
    {
        ++diagPhotoIndex_;
        refreshPhoto();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::initialize()
{
    QFont titleFont( font() );
    titleFont.setPointSize( 24 );

    QFont titleFontBold( titleFont );
    titleFontBold.setBold( true );

    QFont headerFont( font() );
    headerFont.setPointSize( 20 );

    QFont headerFontBold( headerFont );
    headerFontBold.setBold( true );

    QFont normalFont( font() );
    normalFont.setPointSize( 16 );

    QFont normalFontBold( normalFont );
    normalFontBold.setBold( true );

    modifyPaletteBackground setBackgroundPalette( QColor( 192, 192, 192 ) );

    modifyPaletteForeground setForegroundPalette(
        modifyPaletteForeground::createPalette( hobart::colorPalette::black, hobart::colorPalette::lightGray ) );

    setBackgroundPalette( this );
    setForegroundPalette( this );

    setAutoFillBackground( true );
    setFont( normalFont );

    // title
    title_ = new QLabel( this );
    title_->setObjectName( "title" );
    title_->setFont( titleFontBold );

    // hardware model
    hardwareModel_ = new QLabel( this );
    hardwareModel_->setObjectName( "hardwareModel" );
    hardwareModel_->setFont( titleFont );

    // hardware serial
    hardwareSerial_ = new QLabel( this );
    hardwareSerial_->setObjectName( "hardwareSerial" );
    hardwareSerial_->setFont( titleFont );

    // hardware cycles
    hardwareCycles_ = new QLabel( this );
    hardwareCycles_->setObjectName( "hardwareCycles" );
    hardwareCycles_->setFont( titleFont );

    // error code
    errorCode_ = new QLabel( this );
    errorCode_->setObjectName( "errorCode" );
    errorCode_->setFont( headerFontBold );

    // error code number
    errorCodeNum_ = new QLabel( this );
    errorCodeNum_->setObjectName( "errorCodeNum" );
    errorCodeNum_->setFont( headerFont );

    // description
    description_ = new QLabel( this );
    description_->setObjectName( "description" );
    description_->setFont( normalFontBold );

    // stamp
    stamp_ = new QLabel( this );
    stamp_->setObjectName( "stamp" );

    // photo
    photo_ = new QLabel( this );
    photo_->setObjectName( "photo" );

    // message
    message_ = new QLabel( this );
    message_->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
    message_->setObjectName( "message" );
    message_->setWordWrap( true );

    // next
    next_ = new QPushButton( this );
    next_->setEnabled( false );
    next_->setObjectName( "next" );

    next_->setVisible( false ); // RLBDIAG - Remove when supported

    connect( next_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    // home
    home_ = new QPushButton( this );
    home_->setObjectName( "home" );
    //home_->setVisible( false ); // RLBDIAG - Uncomment when supported

    connect( home_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    // calibrate
    calibrate_ = new QPushButton( this );
    calibrate_->setObjectName( "calibrate" );
    calibrate_->setVisible( false );

    connect( calibrate_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    // health agent
    healthAgent_ = new QPushButton( this );
    healthAgent_->setObjectName( "home" );
    healthAgent_->setVisible( false );

    connect( healthAgent_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    // wizard init timer
    wizardInitTimer_ = new QTimer( this );
    wizardInitTimer_->setSingleShot( true );
    wizardInitTimer_->setInterval( 30000 );

    connect( wizardInitTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // wizard testing timer
    wizardTestingTimer_ = new QTimer( this );
    wizardTestingTimer_->setSingleShot( true );
    wizardTestingTimer_->setInterval( 60000 );

    connect( wizardTestingTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // animation timer
    animationTimer_ = new QTimer( this );
    animationTimer_->setSingleShot( false );
    animationTimer_->setInterval( 1000 );

    connect( animationTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // connect slots
    connect( mode_, SIGNAL(screenChanged()), SLOT(onScreenChanged()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::createLayout()
{
    QHBoxLayout *title( new QHBoxLayout() );
    title->addWidget( title_ );
    title->addStretch();
    title->addWidget( hardwareModel_ );
    title->addWidget( hardwareSerial_ );
    title->addStretch();
    title->addWidget( hardwareCycles_ );

    QHBoxLayout *detail( new QHBoxLayout() );
    detail->addWidget( errorCode_ );
    detail->addWidget( errorCodeNum_ );
    detail->addStretch();
    detail->addWidget( description_ );
    detail->addStretch();
    detail->addWidget( stamp_ );

    QHBoxLayout *next( new QHBoxLayout() );
    next->addStretch();
    next->addWidget( next_ );
    next->addWidget( home_ );
    next->addWidget( calibrate_ );
    next->addWidget( healthAgent_ );
    next->addStretch();

    QVBoxLayout *message( new QVBoxLayout() );
    message->addWidget( message_, 1 );
    message->addLayout( next );

    QHBoxLayout *photo( new QHBoxLayout() );
    photo->addWidget( photo_ );
    photo->addLayout( message, 1 );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 20, 16, 20, 8 ) );
    screenLayout->addLayout( title );
    screenLayout->addLayout( detail );
    screenLayout->addLayout( photo, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::refreshErrorInfo()
{
    if ( !errorInfo_diagErrorCode )
        return;

    QString description( tr( "Unknown Error" ) );
    QString message( tr( "An unkown error has occured. Press the next button to diagnose the error." ) );

    // retrieve appropriate diagnositics message
    switch ( errorInfo_diagErrorCode )
    {
    case 9516:
        description = tr( "Blocked Sensor - Camera" );
        message = tr( "Camera and height sensors blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine.\n\nCheck for package too far forward. Clean both cameras, light tube, and height sensors.\n\nPress the START button." );
        break;
    case 9519:
        description = tr( "Blocked Sensor - Camera" );
        message = tr( "Camera and height sensors blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine.\n\nCheck for package too far forward. Clean both cameras, light tube, and height sensors.\n\nPress the START button." );
        break;
    case 9521:
        description = tr( "Measurement Error" );
        message = tr( "Measurement error while accepting package.\n\nPress the STOP button. Remove any packages, labels, debris in the machine.\n\nCheck for package too far forward. Clean both cameras, light tube, and height sensors.\n\nPress the START button." );
        break;
    case 9526:
        description = tr( "Package Centering Error" );
        message = tr( "Package is too far left.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9527:
        description = tr( "Package Centering Error" );
        message = tr( "Package is too far right.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9538:
        description = tr( "Film Pull Error" );
        message = tr( "Film not pulled. Press the STOP button. Check to make sure film is properly loaded and the gripper and film selector are clear of debris. Press the START button." );
        break;
    case 9565:
        description = tr( "Communication Error" );
        message = tr( "Wrapper lost CAN communications.\n\nCycle the circuit breaker. Press the START button." );
        break;
    case 9567:
        description = tr( "Open Door - Canopy" );
        message = tr( "Canopy open.\n\nPlease close the canopy. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9568:
        description = tr( "Open Door - Right Side" );
        message = tr( "Right-side access door open.\n\nPlease close the door. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9569:
        description = tr( "Open Door - Left Side" );
        message = tr( "Left-side access door open.\n\nPlease close the door. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9570:
        description = tr( "Open Door - Canopy" );
        message = tr( "Canopy open.\n\nPlease close the canopy. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9571:
        description = tr( "Open Door - Rear Film" );
        message = tr( "Rear film door open.\n\nPlease be sure both film doors are closed. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9572:
        description = tr( "Open Door - Rear Film" );
        message = tr( "Rear film door open.\n\nPlease be sure both film doors are closed. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9590:
        description = tr( "Low Film" );
        message = tr( "Low film on Top roll.\n\nPress the STOP button. Remove package from intake. Press the START button to resume wrapping." );
        break;
    case 9591:
        description = tr( "Low Film" );
        message = tr( "Low film on Bottom roll.\n\nPress the STOP button. Remove package from intake. Press the START button to resume wrapping." );
        break;
    case 9597:
    case 9598:
    case 9599:
    case 9600:
    case 9601:
    case 9602:
    case 9603:
    case 9604:
    case 9605:
    case 9606:
    case 9607:
    case 9608:
    case 9609:
    case 9610:
    case 9611:
    case 9612:
    case 9613:
    case 9614:
    case 9615:
    case 9616:
    case 9617:
    case 9618:
    case 9619:
    case 9620:
    case 9621:
    case 9622:
    case 9623:
    case 9624:
    case 9625:
    case 9626:
        description = tr( "Timings Error" );
        message = tr( "Timings memory error detected.\n\nPress the STOP button. Remove any packages, labels, debris in the machine.\n\nCalibrate the wrapper using the button below.\n\nPress the START button." );
        break;
    case 9631:
        description = tr( "Blocked Sensor - Elevator" );
        message = tr( "Package sensor across elevator is blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine from flashing area. Check both right and left sides. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9640:
        description = tr( "Stalled Motor - Primary Intake" );
        message = tr( "Primary Intake has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9641:
        description = tr( "Stalled Motor - Secondary Intake" );
        message = tr( "Secondary Intake has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9642:
        description = tr( "Stalled Motor - Elevator" );
        message = tr( "Elevator has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9643:
        description = tr( "Stalled Motor - Rear Folder" );
        message = tr( "Rear Folder has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9644:
        description = tr( "Stalled Motor - Front Folder" );
        message = tr( "Front Folder has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9645:
        description = tr( "Stalled Motor - Side Folder" );
        message = tr( "Side Folder has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9646:
        description = tr( "Stalled Motor - Side Clamp" );
        message = tr( "Side Clamp has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9647:
        description = tr( "Stalled Motor - Sealer Belt" );
        message = tr( "Sealer belt has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9648:
        description = tr( "Stalled Motor - Centering Conveyor" );
        message = tr( "Centering conveyor jammed.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;
    case 9649:
        description = tr( "Stalled Motor - Gripper" );
        message = tr( "Gripper has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9650:
        description = tr( "Stalled Motor - Pusher" );
        message = tr( "Pusher has Stalled.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;
    case 9655:
        description = tr( "Sensor Failure - Narrow Gripper" );
        message = tr( "The narrow gripper sensor has failed to close. Please verify the gripper is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9656:
        description = tr( "Sensor Failure - Narrow Gripper" );
        message = tr( "The narrow gripper sensor has failed to open. Please verify the gripper is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9657:
        description = tr( "Sensor Failure - Left Gripper" );
        message = tr( "The left gripper sensor has failed to close. Please verify the gripper is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9658:
        description = tr( "Sensor Failure - Left Gripper" );
        message = tr( "The left gripper sensor has failed to open. Please verify the gripper is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9659:
        description = tr( "Sensor Failure - Right Gripper" );
        message = tr( "The right gripper sensor has failed to close. Please verify the gripper is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9660:
        description = tr( "Sensor Failure - Right Gripper" );
        message = tr( "The right gripper sensor has failed to open. Please verify the gripper is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9661:
        description = tr( "Sensor Failure - Left Side Clamp" );
        message = tr( "The left side clamp has failed to close. Please verify the side clamp is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9662:
        description = tr( "Sensor Failure - Left Side Clamp" );
        message = tr( "The left side clamp has failed to open. Please verify the side clamp is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9663:
        description = tr( "Sensor Failure - Right Side Clamp" );
        message = tr( "The right side clamp has failed to close. Please verify the side clamp is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9664:
        description = tr( "Sensor Failure - Right Side Clamp" );
        message = tr( "The right side clamp has failed to open. Please verify the side clamp is clear of any trays and/or debris.\n\nMake sure any covers opened during the cleaning process are lowered and/or closed." );
        break;
    case 9667:
        description = tr( "Film Selector Unlatched" );
        message = tr( "Film Selector Lever is unlatched.\n\nPress the STOP button. Engage Film Selector Lever. Press the START button." );
        break;
    case 9682:
        description = tr( "Loose Film Roll" );
        message = tr( "The Upper Film Roll is loose on the film Arbor.\n\n1) Please tighten the Film Arbor knob, turning clockwise.\n2) Ensure the core grip is tight to the film core." );
        break;
    case 9683:
        description = tr( "Loose Film Roll" );
        message = tr( "The Lower Film Roll is loose on the film Arbor.\n\n1) Please tighten the Film Arbor knob, turning clockwise.\n2) Ensure the core grip is tight to the film core." );
        break;
    case 9684:
        description = tr( "Docking failure" );
        message = tr( "Printer Dock Failure.\n\nPlease make sure printer is docked in proper position. Push printer all the way into its docking station." );
        break;
    case 9685:
        description = tr( "Sensor Failure - Package Sensor" );
        message = tr( "Package Sensor across Sealer Belt is blocked.\n\nClear any obstructions from the package sensor path." );
        break;
    case 9686:
        description = tr("Wrapper Status");
        message = ( "Press Start Button and then place package on Intake." );
    case 9687:
        description = tr( "Intake Sensor Status" );
        message = tr( "Intake Package Sensor is blocked. Make sure sensor is clean and free of debris. " );
    case 9688:
        description = tr( "Air Pressure Failure" );
        message = tr( "Regulated air pressure is too high. Adjust air regulator to 50 psi. " );
    case 10017:
        description = tr( "Wrapper not Responding" );
        message = tr( "Wrapper did not respond.\n\nCycle the circuit breaker. Remove any packages, labels, debris in the machine. Press the START button.\n\nThen hit NEXT below." );
        break;
    case 10018:
        description = tr( "Wrapper not Responding" );
        message = tr( "The wrapper stopped communicating.\n\nCheck to be sure the wrapper cable is plugged in.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nThen hit NEXT below." );
        break;
    default:
        {
            // use error message description by default
            QString messageString( SessionErrorDialog::messageString( errorInfo_diagErrorCode, session_->isServiceCounter() ) );

            if ( messageString.length() )
            {
                description.clear();
                message = messageString;
            }
        }

        break;
    }

    if ( Init == state_ )
        message = tr( "ENTERING DIAGNOSTICS MODE..." );
    else if ( CancelledInit == state_ )
        message = tr( "Diagnostics failed to initialize.\n\nPlease check that the device is connected and online, then try again." );
    else if ( Testing == state_ )
        message = tr( "TESTING IN PROGRESS...\n\nPlease wait while diagnostics is being performed.\n\nDo not remove power or reboot the machine while diagnostics is in progress." );
    else if ( CancelledTesting == state_ )
        message = tr( "Diagnostics testing failed to complete.\n\nPlease check that the device is connected and online, then try again." );
    else if ( Complete == state_ )
    {
        if ( failureCode_ )
            message = testingFailedText( failureCode_ );
        else
            message = testingPassedText();
    }

    errorCodeNum_->setText( QString::number( errorInfo_diagErrorCode ) );

    description_->setText( description );
    message_->setText( message );

    refreshPhoto();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::refreshPhoto()
{
    if ( !errorInfo_diagErrorCode )
        return;

    //
    // load photos
    //

    bool isTwoPrinter = isTwoPrinterPhoto( errorInfo_diagErrorCode ) && session_->scaleConfig->isWrapperTwoPrinter();

    if ( diagPhotos_.isEmpty() )
    {
        QString diagPhotoName( "diag" + QString::number( errorInfo_diagErrorCode ) + ".png" );
        if( isTwoPrinter )
        {
            diagPhotoName.prepend("2p");
        }

        QString diagPhoto( filesystem::absoluteFilePath( diagPhotoName, folderType::Images ) );

        if ( QFile::exists( diagPhoto ) )
            diagPhotos_.append( QPixmap( diagPhoto ) );
        else
        {
            for ( char c( 'a' ); ; ++c )
            {
                diagPhotoName = "diag" + QString::number( errorInfo_diagErrorCode ) + QChar( c ) + ".png";
                if( isTwoPrinter )
                {
                    diagPhotoName.prepend("2p");
                }

                diagPhoto = filesystem::absoluteFilePath( diagPhotoName, folderType::Images );

                if ( !QFile::exists( diagPhoto ) )
                    break;

                diagPhotos_.append( QPixmap( diagPhoto ) );
            }
        }

        diagPhotoIndex_ = 0;
    }

    //
    // display next photo
    //

    if ( diagPhotos_.isEmpty() )
    {
        const QString diaga( filesystem::absoluteFilePath( "diaga.png", folderType::Images ) );
        const QString diagb( filesystem::absoluteFilePath( "diagb.png", folderType::Images ) );

        if (( !QFile::exists( diaga ) ) || ( !QFile::exists( diagb ) ))
            return;

        diagPhotos_.append( QPixmap( diaga ) );
        diagPhotos_.append( QPixmap( diagb ) );
    }

    if ( diagPhotos_.size() <= diagPhotoIndex_ )
        diagPhotoIndex_ = 0;

    const QPixmap p( diagPhotos_[diagPhotoIndex_].scaled( photo_->size(), Qt::KeepAspectRatioByExpanding ) );

    photo_->setPixmap(
        p.copy(
            (p.width() - photo_->width()) / 2,
            (p.height() - photo_->height()) / 2,
            photo_->width(),
            photo_->height() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::processErrorSpecificEvents()
{
    // handle any error specific events
    switch ( errorInfo_diagErrorCode )
    {
    // measurement error
    case 9521:

    // timings/memory error
    case 9597:
    case 9598:
    case 9599:
    case 9600:
    case 9601:
    case 9602:
    case 9603:
    case 9604:
    case 9605:
    case 9606:
    case 9607:
    case 9608:
    case 9609:
    case 9610:
    case 9611:
    case 9612:
    case 9613:
    case 9614:
    case 9615:
    case 9616:
    case 9617:
    case 9618:
    case 9619:
    case 9620:
    case 9621:
    case 9622:
    case 9623:
    case 9624:
    case 9625:
    case 9626:

        // allow calibrate
        calibrate_->show();

        // force
        onCalibrateWrapper();

        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString DiagnosticsWizardScreen::testingPassedText()
{
    QString description( tr( "Testing complete" ) );
    QString message( tr( "PASSED" ) );

    // retrieve appropriate diagnositics message
    switch ( errorInfo_diagErrorCode )
    {
    case 9655:
    case 9656:
    case 9657:
    case 9658:
    case 9659:
    case 9660:
        description = tr( "Gripper Testing is Complete" );
        message = tr( "Remove film that was pulled during testing." );
        break;

    case 9661:
    case 9662:
    case 9663:
    case 9664:
        description = tr( "Side Clamp Testing is Complete" );
        message = tr( "Remove film that was pulled during testing." );
        break;

    default:
        break;
    }

    // show message
    QString text;
    text.append( description + "\n" );
    text.append( "\n" );
    text.append( message );

    return text;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString DiagnosticsWizardScreen::testingFailedText( int failureCode )
{
    // show message
    QString text;
    text.append( tr( "PLEASE CALL FOR SERVICE:" ) + "\n" );
    text.append( "    " + serviceContactInfo_ + "\n" );
    text.append( "\n" );
    text.append( tr( "Provide the following error code:" ) + "\n" );
    text.append( "    " + QString::number( failureCode ) );

    return text;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsWizardScreen::showHealthAgentButton()
{
    healthAgent_->setVisible( true );
    healthAgent_->setEnabled( true );

    // when in operate mode we might not be able to switch to supervisor mode... check if password required
    if ( mainWindow_->isOperateMode() )
    {
        const MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

        // login required for supervisor mode
        // and not currently logged in
        if (( scaleConfig->superForcedLogin() ) &&
            ( !mainWindow_->retrieveMode<SupervisorMode>()->loggedIn() ))
        {
            healthAgent_->setEnabled( false );
        }
    }
}

bool DiagnosticsWizardScreen::isTwoPrinterPhoto(int code)
{
    switch( code )
    {
    case 9567:
    case 9568:
    case 9569:
    case 9571:
    case 9572:
    case 9590:
    case 9591:
        return true;
    default:
        return false;
    }
}
