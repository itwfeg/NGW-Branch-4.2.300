/**
 * @file candevman.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "candev.h"
#include "candevman.h"
#include "cansocket.h"

#include <iostream>
#include <iomanip>

#include <QCoreApplication>

QMutex canDevManager::instanceMutex_( QMutex::Recursive );
canDevManager *canDevManager::instance_( NULL );

///////////////////////////////////////////////////////////////////////////////////////////////////
canDevManager::canDevManager() :
    _Mybase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canDevManager::~canDevManager()
{
    std::cout << "canDevManager DTOR..." << std::endl;

    QMutexLocker guard( &instanceMutex_ );

    // destroy all devices
    std::cout << "destroying all devices..." << std::endl;

    foreach ( canDev *dev, devices_ )
        delete dev;

    // destroy all interfaces
    std::cout << "destroying all interfaces..." << std::endl;

    for ( interfaceMap::iterator i( interfaces_.begin() ); i != interfaces_.end(); ++i )
        delete i.value();

    std::cout << "canDevManager DTOR complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList canDevManager::interfaces() const
{
    QMutexLocker guard( &instanceMutex_ );
    return interfaces_.keys();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canDevManager::setInterfaces( const QStringList &value )
{
    QMutexLocker guard( &instanceMutex_ );

    const QStringList interfaces( interfaces_.keys() );

    // add new interfaces
    foreach( const QString &i, value )
        if ( !interfaces_.contains( i ) )
            interfaces_[i] = new canSocket( i, this );

    // remove old interfaces
    foreach( const QString &i, interfaces )
        if ( !value.contains( i ) )
        {
            delete interfaces_[i];
            interfaces_.remove( i );
        }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
QList<canDev*> canDevManager::attachedDevices() const
{
    QMutexLocker guard( &instanceMutex_ );
    return devices_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canDevManager::lockEvents()
{
    blockSignals( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canDevManager::unlockEvents()
{
    blockSignals( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canDevManager *canDevManager::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new canDevManager();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canDevManager::onDeviceDestroyed( QObject *d )
{
    QMutexLocker guard( &instanceMutex_ );

    std::cout << "remove device 0x" << std::uppercase << std::setfill('0') << std::setw( 8 ) << std::hex << d << " from map" << std::endl << std::dec;

    // destroy object
    canDev *dev( qobject_cast<canDev*>( d ) );

    if (( !dev ) || ( !devices_.contains( dev ) ))
    {
        // remove device from map
        if ( devices_.removeAll( dev ) )
            std::cout << "device removed" << std::endl;

        // process events
        Q_EMIT deviceLeft( dev );
    }
}
