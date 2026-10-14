/**
 * @file upgradeKPCSoftware.cpp
 * @brief Upgrades KPC Software and displays progress.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author: Joe E Crew
 */

#include "upgradeKPCSoftware.h"
#include "ui_upgradeKPCSoftware.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

#include <backgroundwidgetstyles.h>
#include <logmanager.h>
#include <palette.h>

#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////
upgradeKPCSoftware::upgradeKPCSoftware( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::upgradeKPCSoftware),
    state_( Initial )
{
    ui->setupUi(this);

    initialize();                           // initialize

    translate();

    // STEP 1: Scale Received (KPCBootUpdater, KpcUpgradeRequired - 21,28 ) which forced this screen to load

    // STEP 2: request upgrade information - REQ_INFO
    internalConfigData = &session_->scaleConfig->internalConfig;

    if( internalConfigData->needsKPCUpgraderUpgrade )
        session_->send( KPCBootUpdater, KpcBootUpgraderReqUpgradeInfo);        // send (21,30)
    else if( internalConfigData->needsKPCLoaderUpgrade )
    {
        session_->send( KPCBootloader, KpcBootLoaderReqUpgradeInfo);            // send (20,30)
        // Give chance to cancel
        cancelTimer_->setSingleShot(true);
        cancelTimer_->start(10000);
        cancelCountdown_->start(1000); // update time once every second
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
upgradeKPCSoftware::~upgradeKPCSoftware()
{
    internalConfigData->needsKPCLoaderUpgrade = false;
    internalConfigData->needsKPCUpgraderUpgrade = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::translate()
{
    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        title_->setText( tr("2nd Applier Upgrade") );
        kpc_type_label_->setText( tr("2nd Applier Type") );;
    }
    else
    {
        title_->setText( tr("KPC Upgrade") );
        kpc_type_label_->setText( tr("KPC Type") );;
    }

    old_version_label_->setText( tr("Current Version") );;
    new_version_label_->setText( tr("New Version") );;

    start_->setText( tr("Start") );
    done_->setText( tr("Done") );
    cancel_->setText( tr("Cancel") );

    status_label_->setText( tr("Status") );

    updateState( state_ );
    updateNodeStates();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool upgradeKPCSoftware::onScreenChanging()
{
    // cannot leave the screen while upgrading
    if ( !alldone_ )
        return false;

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    OperateMode *operateMode( dynamic_cast<OperateMode*>( mode_ ) );

    // make sure weight bar is hidden
    if ( operateMode )
        operateMode->hideWeightBar();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::initialize()
{
    alldone_        = false;

    status_         = ui->lblStatus;
    message_        = ui->lblMessage;
    kpc_type_       = ui->lblKpcType;
    new_version_    = ui->lblVersionNew;
    old_version_    = ui->lblVersionOld;

    start_          = ui->butStart;
    done_           = ui->butDone;
    cancel_         = ui->butCancel;
    completedness_  = ui->progressBar;

    title_          = ui->lblTitle;

    kpc_type_label_    = ui->lblKpcTypeLabel;
    new_version_label_ = ui->lblVersionNewLabel;
    old_version_label_ = ui->lblVersionOldLabel;
    status_label_      = ui->lblStatusLabel;

    internalConfigData = &session_->scaleConfig->internalConfig;

    start_->setText( tr("Start") );
    done_->setText( tr("Done") );
    cancel_->setText( tr("Cancel (10)") );

    nodeinfo_state_ = Pending;

    updateState( Initial );

    // create and SIGNALS AND SLOTS
    connect( ui->butDone, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );
    connect( ui->butStart, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );
    connect( ui->butCancel, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // used for testing
    start_->setEnabled(false);
    start_->setEnabled(false);
    start_->setVisible(false);
    done_->setVisible(false);

    if( internalConfigData->needsKPCLoaderUpgrade )
    {
        cancel_->setEnabled(true);
        cancel_->setVisible(true);

        cancelTimer_ = new QTimer(this);
        cancelCountdown_ = new QTimer(this);
        connect( cancelTimer_,              SIGNAL( timeout() ), SLOT( onCancelTimeout() ) );
        connect( cancelCountdown_,          SIGNAL( timeout() ), SLOT( updateCancelTime() ) );
    }
    else
    {
        cancel_->setEnabled(false);
        cancel_->setVisible(false);
    }

    startTimer_ = new QTimer( this );               // kicks the party off when we arrive at this screen
    updateNodeStatesTimer_ = new QTimer( this );    // update the states on a regular basic
    updateProgressTimer_ = new QTimer( this );
    doneTimer_ = new QTimer( this );
    testTimer_ = new QTimer( this );

    connect( testTimer_,                SIGNAL( timeout() ), SLOT( onTest() ) );
    connect( doneTimer_,                SIGNAL( timeout() ), SLOT( onFinishedUpgrade() ) );
    connect( startTimer_,               SIGNAL( timeout() ), SLOT( onStartUpgrade() ) );
    connect( updateProgressTimer_,      SIGNAL( timeout() ), SLOT( onUpdateProgressState() ) );
    connect( updateNodeStatesTimer_,    SIGNAL( timeout() ), SLOT( onUpdateNodeStates() ) );

    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::updateState( state s )
{
    modifyPaletteBackground greenBackground( hobart::colorPalette::green );
    modifyPaletteBackground redBackground( hobart::colorPalette::red );
    modifyPaletteBackground yellowBackground( hobart::colorPalette::yellow );

    switch ( (state_ = s) )
    {
        case Initial:
        case UpgradeRequested:
        {
            if( alldone_ )
                break;
            message_->setText( tr( "Please wait for upgrade to begin..." ) );
            yellowBackground( message_ );
            break;
        }
        case UpgradeInProgress:
        {
            if( alldone_ )
                break;

            cancel_->setEnabled(false);
            cancel_->setVisible(false);

            message_->setText( tr( "WARNING: Do not remove power during upgrade..." ) );
            redBackground( message_ );
            break;
        }

        case UpgradeComplete:
        {
            if( session_->scaleConfig->isWrapperTwoPrinter() )
            {
                message_->setText( tr( "2nd Applier upgrade complete." ) );
            }
            else
            {
                message_->setText( tr( "KPC upgrade complete." ) );
            }
            greenBackground( message_ );
        }
        break;

        case CancelRequested:
        {
            if( alldone_ )
                break;

            message_->setText( tr("Canceling upgrade...") );
            redBackground( message_ );

            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::updateNodeStates()
{
    updateNodeStatesTimer_->setSingleShot( true );
    updateNodeStatesTimer_->start( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::updateProgressState()
{
    updateProgressTimer_->setSingleShot( true );
    updateProgressTimer_->start( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::onPushButtonClicked()
{
    LOG_CLASS_INFO( "upgradeKPCSoftware", "onPushButtonClicked" );


    if ( start_ == sender() )
    {
        LOG_INFO( "requesting kpc upgrade...", NULL );

        start_->setEnabled( false );

        // STEP 4 - Let RTC know the UI is ready (21,29)
        if( internalConfigData->needsKPCUpgraderUpgrade )
            session_->send( KPCBootUpdater, KpcBootUpgraderUpgradeUIReady );
        else if( internalConfigData->needsKPCLoaderUpgrade )
            session_->send( KPCBootloader, KpcBootLoaderUpgradeUIReady );

        // testing (uncomment for test messages)
        testTimer_->setSingleShot( false );
        testTimer_->start( 4000 );
    }
    else if ( done_ == sender() )
    {
        LOG_INFO( "user doned the kpc upgrade...", NULL );

        // done - so head on home
        alldone_ = true;
        state_ = UpgradeComplete;
        nodeinfo_state_ = Complete;

        mainWindow_->navigateHome();
    }
    else if ( cancel_ == sender() )
    {
        if( internalConfigData->needsKPCLoaderUpgrade )
        {
            updateState(CancelRequested);
            session_->send( KPCBootloader, KpcBootLoaderUpgradeCancel );
            status_->setText( tr("Upgrade cancelled") );
            cancelTimer_->stop();
        }

        if(startTimer_->isActive())
        {
            startTimer_->stop();
        }

        doneTimer_->start();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::onUpdateNodeStates()
{
    // process each node and set status
    switch ( nodeinfo_state_ )
    {
        case ErrorNone:
            status_->setText( tr( "Success" ) );
            break;
        case ErrorTimeout:
            status_->setText( tr( "KPC upgrade timed out.  No response from board" ) );
            break;
        case ErrorWriteFailure:
            status_->setText( tr( "Write failure occurred while upgrading KPC" ) );
            break;
        case ErrorChecksumFailure:
            status_->setText( tr( "KPC upgrade image checksum error" ) );
            break;
        case ErrorUnknownRecordType:
            status_->setText( tr( "Unknown upgrade file. Cannot perform upgrade." ) );
            break;
        case ErrorEraseAppFailure:
            status_->setText( tr( "Upgrade failure. The application flash memory could not be erased." ) );
            break;
        case ErrorBootFailure:
            status_->setText( tr( "Upgrade failure. Could not erase the boot loader flash section." ) );
            break;
        case ErrorComponentNotSet:
            status_->setText( tr( "Upgrade failure. Component ( App or Bootloader) not set." ) );
            break;
        case ErrorImageMismatch:
            status_->setText( tr( "Upgrade failure. The image in the KPC does not match the upgrade file." ) );
            break;
        case ErrorImageChecksumMismatch:
            status_->setText( tr( "Upgrade failure. CRC KPC image checksum error." ) );
            break;
        case ErrorModelTypeError:
            status_->setText( tr( "Upgrade failure. Incorrect KPC model type." ) );
            break;
        case ErrorAddressOutOfRange:
            status_->setText( tr( "Upgrade failure. Address written to is out of range." ) );
            break;
        case ErrorFrameOutOfRange:
            status_->setText( tr( "Upgrade failure. Can frame written to is out of range." ) );
            break;

        default:
        {
            //if( internalConfigData->needsKPCLoaderUpgrade && cancelTimer_->isActive())
            if( internalConfigData->needsKPCLoaderUpgrade )
            {
                status_->setText( tr( "Upgrade will begin shortly" ) );
            }
            else
            {
                status_->setText( tr( "Unknown Error" ) );
            }
            break;
        }
    };

    // set progress
    QPalette p( completedness_->palette() );

    if ( ErrorNone <= (int)(nodeinfo_state_) )
        p.setBrush( QPalette::Highlight, QBrush( hobart::colorPalette::red ) );
    else
        p.setBrush( QPalette::Highlight, QBrush( hobart::colorPalette::green ) );

    completedness_->setPalette( p );
    if( cancelTimer_ )
    {
        //if( internalConfigData->needsKPCLoaderUpgrade && !cancelTimer_->isActive())
        if( internalConfigData->needsKPCLoaderUpgrade )
        {
            completedness_->setValue( nodeinfo_percentComplete_ );
        }
        else if( internalConfigData->needsKPCUpgraderUpgrade )
        {
            completedness_->setValue( nodeinfo_percentComplete_ );
        }
        else
        {
            completedness_->setValue( 0 );
        }
    }
    else
    {
        if( internalConfigData->needsKPCUpgraderUpgrade )
        {
            completedness_->setValue( nodeinfo_percentComplete_ );
        }
        else
        {
            completedness_->setValue( 0 );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::onStartUpgrade()
{
    LOG_CLASS_INFO( "upgradeKPCSoftware", "onStartUpgrade" );

    LOG_INFO( "requesting kpc upgrade onStartUpgrade()...", NULL );

    // STEP 4 - Let RTC know the UI is ready (21,29)
    if( internalConfigData->needsKPCUpgraderUpgrade )
        session_->send( KPCBootUpdater, KpcBootUpgraderUpgradeUIReady );
    else if( internalConfigData->needsKPCLoaderUpgrade )
        session_->send( KPCBootloader, KpcBootLoaderUpgradeUIReady);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::onFinishedUpgrade()
{
    LOG_CLASS_INFO( "upgradeKPCSoftware", "onFinishedUpgrade" );

    LOG_INFO( "user doned the kpc upgrade...", NULL );

    // done - so head on home
    alldone_ = true;
    state_ = UpgradeComplete;
    nodeinfo_state_ = Complete;

    mainWindow_->navigateHome();
}

void upgradeKPCSoftware::onCancelTimeout()
{
    cancel_->setEnabled(false);
    cancel_->setVisible(false);

    session_->send(KPCBootloader, KpcBootLoaderUpgradeRequest);
}

void upgradeKPCSoftware::updateCancelTime()
{
    if( internalConfigData->needsKPCLoaderUpgrade )
    {
        cancel_->setText( QString( tr( "Cancel (%1)" ).arg( cancelTimer_->remainingTime() / 1000 ) ) );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::onUpdateProgressState()
{
    // update the progress bar

    QPalette p( completedness_->palette() );

    if ( ErrorNone <= (int)(nodeinfo_state_) )
        p.setBrush( QPalette::Highlight, QBrush( hobart::colorPalette::red ) );
    else
        p.setBrush( QPalette::Highlight, QBrush( hobart::colorPalette::green ) );

    completedness_->setPalette( p );

    if( cancelTimer_ )
    {
        //if( cancelTimer_ && internalConfigData->needsKPCLoaderUpgrade && !cancelTimer_->isActive())
        if( internalConfigData->needsKPCLoaderUpgrade )
        {
            completedness_->setValue( nodeinfo_percentComplete_ );
        }
        else if( internalConfigData->needsKPCUpgraderUpgrade )
        {
            completedness_->setValue( nodeinfo_percentComplete_ );
        }
        else
        {
            completedness_->setValue( 0 );
        }
    }
    else
    {
        if( internalConfigData->needsKPCUpgraderUpgrade )
        {
            completedness_->setValue( nodeinfo_percentComplete_ );
        }
        else
        {
            completedness_->setValue( 0 );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "upgradeKPCSoftware", "onSessionMessage" );


    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // STEP 3: Received Upgrade Info 21,31
    if ( ( KPCBootUpdater == device ) && ( KpcBootUpgraderUpgradeInfo == event ) )   // 21,31, like 17,9
    {
        DomKpcUpgradeInfoMsg m( msg );

        old_version_->setText( m.currentVersion() );
        new_version_->setText( m.newVersion() );

        kpc_type_->setText( "Bootloader upgrade" );

        if ( m.currentVersion() == m.newVersion() )
        {
            status_->setText( tr( "No upgrade required. The version numbers are the same" ) );

            // trigger done and leave the page
            doneTimer_->setSingleShot( true );
            doneTimer_->start( 3000 );
        }
        else
        {
            status_->setText( tr( "New firmware version") + " " + m.newVersion() );

            // trigger start upgrade and get it going.
            startTimer_->setSingleShot( true );
            startTimer_->start( 3000 );
        }
    }

    else if( ( KPCBootloader == device ) && ( KpcBootLoaderUpgradeInfo == event ) )           // 20,31, like 17,9
    {
        DomKpcLoaderUpgradeInfoMsg m( msg );

        old_version_->setText( m.currentVersion() );
        new_version_->setText( m.newVersion() );

        kpc_type_->setText( "Application upgrade" );

        if ( m.currentVersion() == m.newVersion() )
        {
            status_->setText( tr( "No upgrade required. The version numbers are the same" ) );

            // trigger done and leave the page
            doneTimer_->setSingleShot( true );
            doneTimer_->start( 3000 );
        }
        else
        {
            status_->setText( tr( "New firmware version") + " " + m.newVersion() );

            //if( internalConfigData->needsKPCLoaderUpgrade && !cancelTimer_->isActive())
            if( internalConfigData->needsKPCLoaderUpgrade )
            {
                // trigger start upgrade and get it going.
                startTimer_->setSingleShot( true );
                startTimer_->start( 3000 );
            }
        }
    }

    // STEP 5: start up the upgrade process
    else if ( ( ( KPCBootloader == device  ) && ( KpcBootLoaderStartUpgrade == event   ) ) ||         // 20,24  ( like 17,0)
              ( ( KPCBootUpdater == device ) && ( KpcBootUpgraderStartUpgrade == event )  ) )         // 21,24  ( like 17,0)
    {
        LOG_INFO( "starting upgrade...", NULL );

        status_->setText( tr("Pending.....") );

        completedness_->setValue( 0 );

        nodeinfo_state_ = Pending;
        nodeinfo_percentComplete_ = 0;

        updateState( UpgradeInProgress );
        //updateNodeStates();
    }

    // STEP 6 - UI receives ongling progress (21,26)
    else if ( ( KPCBootUpdater == device ) && ( KpcBootUpgraderUpgradeProgress == event ) )        // 21,26, like 17,9
    {
        DomKpcUpgradeProgressMsg m( msg );

        int did_this = m.progress().toInt();
        nodeinfo_percentComplete_ = did_this;

        // process node
        if (( did_this < 0 ) || ( 100 < did_this ))
            LOG_WARN( "invalid percentage %d ", m.progress().toInt() );
        else
        {
            LOG_INFO( "progress is %d percent complete", did_this );

            if( 100 == did_this )
            {
                nodeinfo_state_ = Complete;
                state_ = UpgradeComplete;
                status_->setText( tr("Upgrade complete") );
                alldone_ = true;
                updateState( UpgradeComplete );

                // trigger done and leave the page
                doneTimer_->setSingleShot( true );
                doneTimer_->start( 3000 );
            }
            else
            {
                nodeinfo_state_ = Upgrading;
                status_->setText( tr("Upgrading...") );
            }

            updateState( UpgradeInProgress );
            updateProgressState();                      //updateNodeStates();
        }
    }

    // STEP 6 - UI receives ongling progress (20,26)
    else if ( ( KPCBootloader == device ) && ( KpcBootLoaderUpgradeProgress == event ) )       // 20,26, like 17,9
    {
        DomKpcLoaderUpgradeProgressMsg m( msg );

        int did_this = m.progress().toInt();
        nodeinfo_percentComplete_ = did_this;

        // process node
        if (( did_this < 0 ) || ( 100 < did_this ))
            LOG_WARN( "invalid percentage %d ", m.progress().toInt() );
        else
        {
            LOG_INFO( "progress is %d percent complete", did_this );

            if( 100 == did_this )
            {
                nodeinfo_state_ = Complete;
                state_ = UpgradeComplete;
                status_->setText( tr("Upgrade complete") );
                alldone_ = true;
                updateState( UpgradeComplete );

                // trigger done and leave the page
                doneTimer_->setSingleShot( true );
                doneTimer_->start( 3000 );
            }
            else
            {
                nodeinfo_state_ = Upgrading;
                status_->setText( tr("Upgrading...") );
            }

            updateState( UpgradeInProgress );
            updateProgressState();                      //updateNodeStates();
        }
    }

    // STEP 7 - random status message (21,25)
    else if ( ( KPCBootUpdater == device ) && ( KpcBootUpgraderUpgradeStatus == event )  )       // 21,25        (like 17,1)
    {
        DomKpcUpgradeStatusMsg m( msg );

        int the_fault = m.fault().toInt();
        nodeState the_state = (nodeState)(the_fault);

        if ( DomKpcUpgradeStatusMsg::BOOT_FAULT == the_fault )
        {
            LOG_WARN( "KPC is in error state %d", the_fault );

            nodeinfo_state_ = the_state;
            nodeinfo_percentComplete_ = 100;

            updateNodeStates();
        }
    }

    // STEP 7 - random status message (21,25)
    else if ( ( KPCBootloader == device ) && ( KpcBootLoaderUpgradeStatus == event ) )          // 20,25
    {
        DomKpcLoaderUpgradeStatusMsg m( msg );

        int the_fault = m.fault().toInt();
        nodeState the_state = (nodeState)(the_fault);

        if ( DomKpcLoaderUpgradeStatusMsg::BOOT_FAULT == the_fault )
        {
            LOG_WARN( "KPC is in error state %d", the_fault );

            nodeinfo_state_ = the_state;
            nodeinfo_percentComplete_ = 100;

            updateNodeStates();
        }
    }
    // end of upgrade
    else if ( ( KPCBootloader == device ) && ( KpcBootLoaderUpgradeFinished == event ) ||        // 20,27
              ( KPCBootUpdater == device ) && ( KpcBootUpgraderUpgradeFinished == event )  )     // 21,27        (like 17,3)
    {
        // are we going to use this?
        LOG_INFO( "upgrade finished!", NULL );

        alldone_ = true;
        updateState( UpgradeComplete );

        // trigger done and leave the page
        doneTimer_->setSingleShot( true );
        doneTimer_->start( 3000 );
    }

    else if ( ( KPCBootloader == device ) && ( KpcBootLoaderUpgradeUpdate ) )
    {
        internalConfigData = &session_->scaleConfig->internalConfig;

		// this should only be received on firmware upgrade, but checking just to make sure
        if( internalConfigData->needsKPCLoaderUpgrade )
            session_->send( KPCBootloader, KpcBootLoaderReqUpgradeInfo);            // send (20,30)
    }
    /* stops upgrade when mid upgrade
    else if ( ( KPCBootloader == device ) && ( KpcBootLoaderUpgradeStopped == event ) )
    {
        // handle upgrade stopped
        LOG_INFO( "Upgrade has stopped!", NULL );

        // we need to swap the new sourceKPC with the older one which should have been saved in /root
        QFile oldKPC( "/root/sourceKPCApp.bin" );
        if( oldKPC.exists() )
        {
            QString newKPCPath = "/lib/firmware/kpc/app/sourceKPCApp.bin";
            QFile newKPC( newKPCPath );
            if( newKPC.exists() )
            {
                newKPC.remove();
                if( !oldKPC.copy( newKPCPath ) )
                {
                    LOG_INFO( "old sourceKPCApp.bin not copied", NULL );
                }
            }
        }

        // reboot device
        // if cancel was successful this should boot into previous version
        // otherwise it'll finish the upgrade on boot
        session_->send(SessionDev, SnShutdownRequest, "5");
    }
    */


    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeKPCSoftware::onTest()
{
    static int tick( 0 );

    // simulate wrapper messages
    switch ( tick++ )
    {
    // begin
    case 0: {
        DomKpcStartUpgradeMsg m;
        // m.numberOfNodesRaw() = "6";      // do not need this argument
        onSessionMessage( m );
        break;
    }
    // node 1
    case 1: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "1";
        m.stateRaw() = "0";
        //m.versionRaw() = "1.2";
        onSessionMessage( m );
        break;
    }
    case 2: {
        DomKpcUpgradeProgressMsg m;
        m.progressRaw() = "25";
        onSessionMessage( m );
        break;
    }
    case 3: {
        DomKpcUpgradeProgressMsg m;
        m.progressRaw() = "50";
        onSessionMessage( m );
        break;
    }
    case 4: {
        DomKpcUpgradeProgressMsg m;
        m.progressRaw() = "75";
        onSessionMessage( m );
        break;
    }
    case 5: {
        DomKpcUpgradeProgressMsg m;
        m.progressRaw() = "100";
        onSessionMessage( m );
        break;
    }
    // node 2
    case 6: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "2";
        m.stateRaw() = "0";
        //m.versionRaw() = "3.4.5";
        onSessionMessage( m );
        break;
    }
    case 7: {
        DomKpcUpgradeProgressMsg m;
        m.progressRaw() = "25";
        onSessionMessage( m );
        break;
    }
    case 8: {
        DomKpcUpgradeProgressMsg m;
        m.progressRaw() = "50";
        onSessionMessage( m );
        break;
    }
    case 9: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "2";
        m.stateRaw() = "2";
        //m.errorRaw() = "1";
        onSessionMessage( m );
        break;
    }
    // node 3
    case 10: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "3";
        m.stateRaw() = "0";
        //m.versionRaw() = "6.7";
        onSessionMessage( m );
        break;
    }
    case 11: {
        DomKpcUpgradeProgressMsg m;
        m.progressRaw() = "100";
        onSessionMessage( m );
        break;
    }
    // node 4
    case 12: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "4";
        m.stateRaw() = "0";
        //m.versionRaw() = "8.9.a";
        onSessionMessage( m );
        break;
    }
    case 13: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "4";
        m.stateRaw() = "2";
        //m.errorRaw() = "8";
        onSessionMessage( m );
        break;
    }
    // node 5
    case 14: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "5";
        m.stateRaw() = "0";
        //m.versionRaw() = "0.7";
        onSessionMessage( m );
        break;
    }
    case 15: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "5";
        m.stateRaw() = "2";
        //m.errorRaw() = "5";
        onSessionMessage( m );
        break;
    }
    // node 6
    case 16: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "6";
        m.stateRaw() = "0";
        //m.versionRaw() = "5.4.3";
        onSessionMessage( m );
        break;
    }
    case 17: {
        DomKpcUpgradeStatusMsg m;
        m.nodeIdRaw() = "6";
        m.stateRaw() = "2";
        //m.errorRaw() = "7";
        onSessionMessage( m );
        break;
    }
    // end
    case 18: {
        NGWUpgradeEndMsg m;
        onSessionMessage( m );
        break;
    }
    default:
        break;
    }
}
