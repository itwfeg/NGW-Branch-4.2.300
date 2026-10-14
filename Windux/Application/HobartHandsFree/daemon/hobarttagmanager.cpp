/**
 * @file hobarttagmanager.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobarttag.h"
#include "hobarttag_v0.h"
#include "hobarttag_v1.h"
#include "hobarttagmanager.h"
#include "scanneragent.h"

#include <QCoreApplication>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QDebug>
#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>

QMutex hobartTagManager::instanceLock_;
hobartTagManager *hobartTagManager::instance_( NULL );

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTagManager::hobartTagManager() :
    m_( QReadWriteLock::Recursive ),
    active_( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTagManager::~hobartTagManager()
{
    qDebug() << "clearing devices...";

    // clear device map
    devices_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTagManager::device_ptr_list_t hobartTagManager::devices() const
{
    QReadLocker guard( &m_ );
    return devices_.values();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTagManager::setOperatorNames( const operator_name_map_t& value )
{
    QWriteLocker guard( &m_ );

    // save operator names for future use
    operatorNames_ = value;

    // set device names
    for ( operator_name_map_t::const_iterator i( operatorNames_.begin() ); i != operatorNames_.end(); ++i )
    {
        const QBluetoothAddress address( i.key() );

        if ( devices_.contains( address ) )
            devices_[address]->setOperatorName( i.value() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTagManager *hobartTagManager::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceLock_ );

        if ( !instance_ )
            instance_ = new hobartTagManager();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTagManager::destroy()
{
    if ( instance_ )
    {
        QMutexLocker guard( &instanceLock_ );

        if ( instance_ )
        {
            delete instance_;
            instance_ = NULL;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTagManager::updateDevice( const scannerAgentDeviceInfo& info )
{
    device_ptr_t device;

    const QBluetoothDeviceInfo deviceInfo( info.info );
    const QBluetoothAddress address( deviceInfo.address() );

    QWriteLocker guard( &m_ );

    // existing device
    if ( devices_.contains( address ) )
        device = devices_[address];

    // create new device
    else
    {
        qDebug() << "creating new device" << address;

        // need firmware version to create proper device
        if ( !info.firmwareVersion )
        {
            qWarning() << "bad device version" << info.firmwareVersion;
            return;
        }

        // create device
        if ( 0 == (info.firmwareVersion >> 8) )
            device = device_ptr_t( new hobartTag_V0( deviceInfo, this ) );
        else if ( 1 == (info.firmwareVersion >> 8) )
            device = device_ptr_t( new hobartTag_V1( deviceInfo, this ) );

        if ( !device )
        {
            qWarning() << "bad device version" << info.firmwareVersion;
            return;
        }

        // check if we have operator name
        if ( operatorNames_.contains( address ) )
            device->setOperatorName( operatorNames_[address] );

        // device should exist on main thread
        device->moveToThread( QCoreApplication::instance()->thread() );

        // connect signals
        connect( device.data(), SIGNAL(connecting()), SLOT(onDeviceConnecting()) );
        connect( device.data(), SIGNAL(disconnected()), SLOT(onDeviceDisconnected()) );

        // track device
        devices_[address] = device;
    }

    // update
    device->update( info );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTagManager::onDeviceConnecting()
{
    emit updateActiveConnections( ++active_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTagManager::onDeviceDisconnected()
{
    emit updateActiveConnections( --active_ );
}
