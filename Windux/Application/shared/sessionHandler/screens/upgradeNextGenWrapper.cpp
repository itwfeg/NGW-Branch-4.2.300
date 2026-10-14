/**
 * @file upgradeNextGenWrapper.cpp
 * @brief Upgrades next generation wrapper and displays progress.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author: Randy Blankley
 */

#include "upgradeNextGenWrapper.h"

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

///////////////////////////////////////////////////////////////////////////////////////////////////
upgradeNextGenWrapper::upgradeNextGenWrapper( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    state_( Initial ),
    allowInfo_( true )
{
    setObjectName( "upgradeNextGenWrapper" );

    // initialize
    initialize();
    createLayout();
    translate();

    // request upgrade information
    session_->send( NextGenWrapper, NGWReqUpgradeInfo, "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
upgradeNextGenWrapper::~upgradeNextGenWrapper()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::translate()
{
    title_->setText( tr( "Wrapper Upgrade" ) );

    nodeProgressLabel_->setText( tr( "Node" ) );
    nodeVersionLabel_->setText( tr( "Version" ) );
    nodeStatusLabel_->setText( tr( "Status" ) );

    begin_->setText( tr( "Start Upgrade" ) );
    cancel_->setText( tr( "Cancel Upgrade" ) );

    updateState( state_ );
    updateNodeStates();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::resizeEvent( QResizeEvent *e )
{
    nodeProgressLabel_->setFixedWidth( (width() * 30) / 100 );
    nodeVersionLabel_->setFixedWidth( (width() * 10) / 100 );

    message_->setFixedHeight( (height() * 20) / 100 );

    _Mybase::resizeEvent( e );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool upgradeNextGenWrapper::onScreenChanging()
{
    // cannot leave the screen while upgrading
    if ( UpgradeInProgress == state_ )
        return false;

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "upgradeNextGenWrapper", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // start up upgrade
    if (( NextGenWrapper == device ) && ( NGWUpgradeBegin == event ))
    {
        NGWUpgradeBeginMsg m( msg );

        if ( nodes_.size() )
            LOG_WARN( "upgrade in progress or completed, cannot handle new begin event!", NULL );
        else if ( !m.numberOfNodes() )
            LOG_WARN( "malformed upgrade message (zero nodes)", NULL );
        else
        {
            LOG_INFO( "upgrading %d nodes...", m.numberOfNodes() );

            // generate user feedback for each node
            for ( int n( m.numberOfNodes() ); n--; )
            {
                nodeInfo info;

                info.state = Pending;

                info.percentComplete = 0;

                info.progress = new QProgressBar( this );
                info.progress->setFixedHeight( nodeProgressLabel_->height() );
                info.progress->setObjectName( QString( "progress" ) + QString::number( n ) );
                info.progress->setOrientation( Qt::Horizontal );
                info.progress->setRange( 0, 100 );
                info.progress->setTextVisible( false );
                info.progress->setValue( 0 );

                info.description = createLabel( QString( "description" ) + QString::number( n ) );
                info.description->setFixedHeight( nodeProgressLabel_->height() );
                info.description->setVisible( false );

                info.version = createLabel( QString( "version" ) + QString::number( n ) );
                info.version->setFixedHeight( nodeVersionLabel_->height() );

                info.status = createLabel( QString( "status" ) + QString::number( n ), Qt::AlignLeft );
                info.status->setFixedHeight( nodeStatusLabel_->height() );

                nodeProgress_->insertWidget( 1, info.progress );
                nodeVersion_->insertWidget( 1, info.version );
                nodeStatus_->insertWidget( 1, info.status );

                nodes_.prepend( info );
            }

            updateState( UpgradeInProgress );
            updateNodeStates();
        }
    }

    // end of upgrade
    else if (( NextGenWrapper == device ) && ( NGWUpgradeEnd == event ))
    {
        LOG_INFO( "upgrade complete!", NULL );

        updateState( UpgradeComplete );
    }

    // status
    else if (( NextGenWrapper == device ) && ( NGWUpgradeStatus == event ))
    {
        NGWUpgradeStatusMsg m( msg );

        // process node
        if (( m.nodeId() <= 0 ) || ( nodes_.size() < m.nodeId() ))
            LOG_WARN( "invalid status node %d (%d)", m.nodeId(), nodes_.size() );
        else
        {
            nodeInfo &info( nodes_[ m.nodeId() - 1 ] );

            if ( NGWUpgradeStatusMsg::Error == m.state() )
            {
                LOG_WARN( "node %d in error state %d", m.nodeId(), m.error() );

                info.state = (nodeState)(ErrorNone + m.error());
                info.percentComplete = 100;

                updateNodeStates();
            }
            else if ( NGWUpgradeStatusMsg::Version == m.state() )
            {
                LOG_WARN( "node %d at version %s", m.nodeId(), qPrintable( m.version() ) );

                info.version->setText( m.version() );
            }
        }
    }

    // progress
    else if (( NextGenWrapper == device ) && ( NGWUpgradeProgress == event ))
    {
        NGWUpgradeProgressMsg m( msg );

        // process node
        if (( m.nodeId() <= 0 ) || ( nodes_.size() < m.nodeId() ))
            LOG_WARN( "invalid progress node %d (%d)", m.nodeId(), nodes_.size() );
        else if (( m.progress() < 0 ) || ( 100 < m.progress() ))
            LOG_WARN( "invalid percentage %d node %d", m.progress(), m.nodeId() );
        else
        {
            nodeInfo &info( nodes_[ m.nodeId() - 1 ] );

            LOG_INFO( "node %d at %d percent complete", m.nodeId(), m.progress() );

            info.state = ((100 == m.progress()) ? Complete : Upgrading);
            info.percentComplete = m.progress();

            updateNodeStates();
        }
    }

    // info
    else if (( NextGenWrapper == device ) && ( NGWUpgradeInfo == event ) && ( allowInfo_ ))
    {
        NGWUpgradeInfoMsg m( msg );

        int nodeCount( INT_MAX );
        nodeCount = qMin( nodeCount, m.nodeCount() );
        nodeCount = qMin( nodeCount, m.nodeVersionCount() );
        nodeCount = qMin( nodeCount, m.firmwareVersionCount() );

        if ( nodeCount )
        {
            QVector<QString> nodes( TokenParser::splitStringTokens( m.nodes() ) );
            QVector<QString> nodeVersions( TokenParser::splitStringTokens( m.nodeVersions() ) );
            QVector<QString> firmwareVersions( TokenParser::splitStringTokens( m.firmwareVersions() ) );

            if (( nodeCount <= nodes.size() ) &&
                ( nodeCount <= nodeVersions.size() ) &&
                ( nodeCount <= firmwareVersions.size() ))
            {
                for ( int n( nodeCount ); n--;  )
                {
                    QLabel *description( createLabel( QString( "description" ) + QString::number( n ) ) );
                    description->setFixedHeight( nodeProgressLabel_->height() );
                    description->setText( tr( "Node" ) + " " + nodes[n] );

                    QLabel *version( createLabel( QString( "version" ) + QString::number( n ) ) );
                    version->setFixedHeight( nodeProgressLabel_->height() );
                    version->setText( nodeVersions[n] );

                    QLabel *status( createLabel( QString( "status" ) + QString::number( n ), Qt::AlignLeft ) );
                    status->setFixedHeight( nodeProgressLabel_->height() );

                    if ( nodeVersions[n] == firmwareVersions[n] )
                        status->setText( tr( "No upgrade required" ) );
                    else
                        status->setText( tr( "New firmware version") + " " + firmwareVersions[n] );

                    nodeProgress_->insertWidget( 1, description );
                    nodeVersion_->insertWidget( 1, version );
                    nodeStatus_->insertWidget( 1, status );

                    infoLabels_.append( description );
                    infoLabels_.append( version );
                    infoLabels_.append( status );
                }

                if ( m.upgradeRequired() )
                {
                    LOG_WARN( "one or more nodes require an upgrade!", NULL );
                    cancel_->setVisible( false );
                }

                allowInfo_ = false;
            }
        }
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::updateState( state s )
{
    modifyPaletteBackground greenBackground( hobart::colorPalette::green );
    modifyPaletteBackground redBackground( hobart::colorPalette::red );
    modifyPaletteBackground yellowBackground( hobart::colorPalette::yellow );

    switch ( (state_ = s) )
    {
    case Initial:
    case UpgradeRequested:
        message_->setText( tr( "Press" ) + " '" + begin_->text() + "' " + tr( "to begin the upgrade" ) );
        yellowBackground( message_ );
        break;

    case UpgradeInProgress:
        message_->setText( tr( "WARNING: Do not remove power until the upgrade process is complete." ) );
        redBackground( message_ );
        break;

    case UpgradeComplete:
        message_->setText( tr( "Wrapper upgrade complete." ) );
        greenBackground( message_ );
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::updateNodeStates()
{
    updateNodeStatesTimer_->setSingleShot( true );
    updateNodeStatesTimer_->start( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::onPushButtonClicked()
{
    LOG_CLASS_INFO( "upgradeNextGenWrapper", "onPushButtonClicked" );

    if ( begin_ == sender() )
    {
        LOG_INFO( "requesting wrapper upgrade...", NULL );
        updateState( UpgradeRequested );

        begin_->setVisible( false );
        cancel_->setVisible( false );

        // go!
        session_->send( NextGenWrapper, NGWUpgradeStart, "" );

        // remove info labels
        foreach ( QLabel *label, infoLabels_ )
        {
            delete label;
        }

        infoLabels_.clear();
        allowInfo_ = false;

        // testing (uncomment for test messages)
        //testTimer_->setSingleShot( false );
        //testTimer_->start( 4000 );
    }
    else if ( cancel_ == sender() )
    {
        LOG_INFO( "user cancelled wrapper upgrade...", NULL );

        // cancel
        session_->send( NextGenWrapper, NGWUpgradeCancel, "" );

        mainWindow_->navigateHome();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::onScreenChanged()
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
void upgradeNextGenWrapper::onTest()
{
    static int tick( 0 );

    // simulate wrapper messages
    switch ( tick++ )
    {
    // begin
    case 0: {
        NGWUpgradeBeginMsg m;
        m.numberOfNodesRaw() = "6";
        onSessionMessage( m );
        break;
    }
    // node 1
    case 1: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "1";
        m.stateRaw() = "0";
        m.versionRaw() = "1.2";
        onSessionMessage( m );
        break;
    }
    case 2: {
        NGWUpgradeProgressMsg m;
        m.nodeIdRaw() = "1";
        m.progressRaw() = "25";
        onSessionMessage( m );
        break;
    }
    case 3: {
        NGWUpgradeProgressMsg m;
        m.nodeIdRaw() = "1";
        m.progressRaw() = "50";
        onSessionMessage( m );
        break;
    }
    case 4: {
        NGWUpgradeProgressMsg m;
        m.nodeIdRaw() = "1";
        m.progressRaw() = "75";
        onSessionMessage( m );
        break;
    }
    case 5: {
        NGWUpgradeProgressMsg m;
        m.nodeIdRaw() = "1";
        m.progressRaw() = "100";
        onSessionMessage( m );
        break;
    }
    // node 2
    case 6: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "2";
        m.stateRaw() = "0";
        m.versionRaw() = "3.4.5";
        onSessionMessage( m );
        break;
    }
    case 7: {
        NGWUpgradeProgressMsg m;
        m.nodeIdRaw() = "2";
        m.progressRaw() = "25";
        onSessionMessage( m );
        break;
    }
    case 8: {
        NGWUpgradeProgressMsg m;
        m.nodeIdRaw() = "2";
        m.progressRaw() = "50";
        onSessionMessage( m );
        break;
    }
    case 9: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "2";
        m.stateRaw() = "2";
        m.errorRaw() = "1";
        onSessionMessage( m );
        break;
    }
    // node 3
    case 10: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "3";
        m.stateRaw() = "0";
        m.versionRaw() = "6.7";
        onSessionMessage( m );
        break;
    }
    case 11: {
        NGWUpgradeProgressMsg m;
        m.nodeIdRaw() = "3";
        m.progressRaw() = "100";
        onSessionMessage( m );
        break;
    }
    // node 4
    case 12: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "4";
        m.stateRaw() = "0";
        m.versionRaw() = "8.9.a";
        onSessionMessage( m );
        break;
    }
    case 13: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "4";
        m.stateRaw() = "2";
        m.errorRaw() = "8";
        onSessionMessage( m );
        break;
    }
    // node 5
    case 14: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "5";
        m.stateRaw() = "0";
        m.versionRaw() = "0.7";
        onSessionMessage( m );
        break;
    }
    case 15: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "5";
        m.stateRaw() = "2";
        m.errorRaw() = "5";
        onSessionMessage( m );
        break;
    }
    // node 6
    case 16: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "6";
        m.stateRaw() = "0";
        m.versionRaw() = "5.4.3";
        onSessionMessage( m );
        break;
    }
    case 17: {
        NGWUpgradeStatusMsg m;
        m.nodeIdRaw() = "6";
        m.stateRaw() = "2";
        m.errorRaw() = "7";
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

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::onUpdateNodeStates()
{
    // process each node
    for ( int n( nodes_.size() ); n--; )
    {
        const nodeInfo &info( nodes_.at( n ) );

        // set description
        info.description->setText( tr( "Node" ) + " " + QString::number( n+1 ) );

        if ( info.description->isHidden() )
        {
            info.description->setGeometry( info.progress->geometry() );
            info.description->setVisible( true );

            info.description->raise();
        }

        // set status
        switch ( info.state )
        {
        case Pending:
            info.status->setText( tr( "Pending" ) );
            break;
        case Upgrading:
            info.status->setText( tr( "Upgrading..." ) );
            break;
        case Complete:
            info.status->setText( tr( "Complete" ) );
            break;

        case ErrorNone:
            info.status->setText( tr( "Success" ) );
            break;
        case ErrorNoResponseFromNode:
            info.status->setText( tr( "Node failed or missing" ) );
            break;
        case ErrorNodeTimeout:
            info.status->setText( tr( "No response from node" ) );
            break;
        case ErrorNodeWriteFailure:
            info.status->setText( tr( "Write failure occurred while programming" ) );
            break;
        case ErrorNodeChecksumFailure:
            info.status->setText( tr( "Programmed image checksum error" ) );
            break;
        case ErrorNodeEraseBank1Failure:
            info.status->setText( tr( "Flash erase failure on bank 1" ) );
            break;
        case ErrorNodeEraseBank2Failure:
            info.status->setText( tr( "Flash erase failure on bank 2" ) );
            break;
        case ErrorNodeIdFailure:
            info.status->setText( tr( "Node address is not supported" ) );
            break;
        case ErrorNodeWriteAddressOutOfRange:
            info.status->setText( tr( "Write address out of programming range" ) );
            break;

        default:
            info.status->setText( tr( "Unknown Error" ) );
            break;
        };

        // set progress
        QPalette p( info.progress->palette() );

        if ( ErrorNone <= (int)(info.state) )
            p.setBrush( QPalette::Highlight, QBrush( hobart::colorPalette::red ) );
        else
            p.setBrush( QPalette::Highlight, QBrush( hobart::colorPalette::green ) );

        info.progress->setPalette( p );
        info.progress->setValue( info.percentComplete );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::initialize()
{
    QFont f( font() );
    f.setPointSize( BUTTON_FONT_POINT_SIZE );

    modifyPaletteForeground blackForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );

    setAutoFillBackground( true );

    blackForeground( this );

    // ---- //

    // screen title
    title_ = createHeaderLabel( "title", true );

    // upgrade
    nodeProgressLabel_ = createHeaderLabel( "nodeProgressLabel" );
    nodeVersionLabel_ = createHeaderLabel( "nodeVersionLabel" );
    nodeStatusLabel_ = createHeaderLabel( "nodeStatusLabel" );

    // begin
    begin_ = new QPushButton( this );
    begin_->setEnabled( true );
    begin_->setFont( f );
    begin_->setObjectName( "begin" );

    connect( begin_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // cancel
    cancel_ = new QPushButton( this );
    cancel_->setEnabled( true );
    cancel_->setFont( f );
    cancel_->setObjectName( "cancel" );

    connect( cancel_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // message
    message_ = createLabel( "message" );
    message_->setAutoFillBackground( true );

    // ---- //

    updateNodeStatesTimer_ = new QTimer( this );

    connect( updateNodeStatesTimer_, SIGNAL( timeout() ), SLOT( onUpdateNodeStates() ) );

    testTimer_ = new QTimer( this );

    connect( testTimer_, SIGNAL( timeout() ), SLOT( onTest() ) );

    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeNextGenWrapper::createLayout()
{
    nodeProgress_ = new QVBoxLayout();
    nodeProgress_->setContentsMargins( QMargins() );

    nodeProgress_->addWidget( nodeProgressLabel_ );
    nodeProgress_->addStretch();

    nodeVersion_ = new QVBoxLayout();
    nodeVersion_->setContentsMargins( QMargins() );

    nodeVersion_->addWidget( nodeVersionLabel_ );
    nodeVersion_->addStretch();

    nodeStatus_ = new QVBoxLayout();
    nodeStatus_->setContentsMargins( QMargins() );

    nodeStatus_->addWidget( nodeStatusLabel_ );
    nodeStatus_->addStretch();

    QHBoxLayout *upgradeLayout( new QHBoxLayout() );
    upgradeLayout->setContentsMargins( QMargins() );

    upgradeLayout->addLayout( nodeProgress_ );
    upgradeLayout->addLayout( nodeVersion_ );
    upgradeLayout->addLayout( nodeStatus_, 1 );

    // ---- //

    QHBoxLayout *buttonLayout( new QHBoxLayout() );
    buttonLayout->setContentsMargins( QMargins() );

    buttonLayout->addStretch();
    buttonLayout->addWidget( begin_ );
    buttonLayout->addWidget( cancel_ );
    buttonLayout->addStretch();

    // ---- //

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 48, 24, 48, 8 ) );

    screenLayout->addWidget( title_ );
    screenLayout->addLayout( upgradeLayout, 1 );
    screenLayout->addLayout( buttonLayout );
    screenLayout->addWidget( message_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QLabel *upgradeNextGenWrapper::createHeaderLabel( const QString &name, bool isTitle )
{
    QFont f( font() );
    f.setPointSize( isTitle ? TITLE_FONT_POINT_SIZE : FONT_POINT_SIZE );

    modifyPaletteBackground blackBackground( hobart::colorPalette::black );
    modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    // label
    QLabel *l( new QLabel( this ) );
    l->setAlignment( Qt::AlignCenter );
    l->setAutoFillBackground( true );
    l->setFont( f );
    l->setObjectName( name );

    blackBackground( l );
    whiteForeground( l );

    return l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QLabel *upgradeNextGenWrapper::createLabel( const QString &name, Qt::Alignment alignment )
{
    QFont f( font() );
    f.setPointSize( FONT_POINT_SIZE );

    // label
    QLabel *l( new QLabel( this ) );
    l->setAlignment( alignment );
    l->setFont( f );
    l->setObjectName( name );

    return l;
}
