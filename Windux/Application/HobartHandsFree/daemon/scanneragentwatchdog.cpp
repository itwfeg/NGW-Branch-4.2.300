/**
 * @file scanneragenttoggle.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobarttagmanager.h"
#include "otaupgrademanager.h"
#include "scanneragent.h"
#include "scanneragentwatchdog.h"

#include <QDebug>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////
scannerAgentWatchdog::scannerAgentWatchdog( scannerAgent *agent, QObject *parent ) :
    _Mybase( parent ),
    tagManager_( hobartTagManager::instance() ),
    upgradeManager_( otaUpgradeManager::instance() ),
    agent_( agent ),
    active_( 0 ),
    activeTimer_( new QTimer( this ) ),
    resetAgentTimer_( new QTimer( this ) )
{
    connect( activeTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );
    connect( resetAgentTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // signal across threads
    connect( agent_, SIGNAL(deviceDiscovered(scannerAgentDeviceInfo)), tagManager_, SLOT(updateDevice(scannerAgentDeviceInfo)), Qt::QueuedConnection );
    connect( agent_, SIGNAL(deviceDiscoveredOTA(scannerAgentDeviceInfo)), upgradeManager_, SLOT(updateDevice(scannerAgentDeviceInfo)), Qt::QueuedConnection );

    connect( tagManager_, SIGNAL(updateActiveConnections(int)), SLOT(onUpdateActiveConnections(int)) );
    connect( upgradeManager_, SIGNAL(updateActiveConnections(int)), SLOT(onUpdateActiveConnections(int)) );

    if ( agent_->isScanningActive() )
        resetAgentTimer_->start( RESET_AGENT_TIMEOUT );
    else
        activeTimer_->start( ACTIVE_TIMEOUT );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scannerAgentWatchdog::~scannerAgentWatchdog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scannerAgentWatchdog::onTimeout()
{
    if ( activeTimer_ == sender() )
    {
        ; // TODO disconnect all devices
    }

    else if ( resetAgentTimer_ == sender() )
    {
        if ( !agent_->isScanningActive() )
        {
            qDebug() << "=== RESET SCANNER AGENT ===";

            agent_->stopScanning();
            agent_->reset();
            agent_->startScanning();

            if ( !agent_->isScanningActive() )
                qWarning() << "scanner agent not active after reset";
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scannerAgentWatchdog::onUpdateActiveConnections( int /*newActive*/ )
{
    const int active( tagManager_->activeConnections() + upgradeManager_->activeConnections() );

    qDebug() << "active connections" << active;

    // start scanning when no active devices
    if (( 0 == active ) && ( active < active_ ))
    {
        agent_->startScanning();

        resetAgentTimer_->start( RESET_AGENT_TIMEOUT );
        activeTimer_->stop();
    }

    // stop scanning when active devices
    else if (( 1 == active ) && ( active_ < active ))
    {
        agent_->stopScanning();

        resetAgentTimer_->stop();
        activeTimer_->start( ACTIVE_TIMEOUT );
    }

    active_ = active;
}
