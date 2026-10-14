/**
 * @file devicediscovery.cpp
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#include "devicediscovery.h"

#include <QByteArray>
#include <QHostAddress>
#include <QTcpSocket>
#include <QUdpSocket>

DeviceDiscovery *DeviceDiscovery::instance_( nullptr );
QMutex DeviceDiscovery::instanceMutex_;

///////////////////////////////////////////////////////////////////////////////////////////////////
DeviceDiscovery::DeviceDiscovery() :
    _Mybase()
{
    // open up listener socket
    sock_ = new QUdpSocket( this );

    if ( !sock_->bind( QHostAddress::AnyIPv4, UDP_PORT ) )
        qCritical() << "failed to bind discovery udp port";
    else
    {
        connect( sock_, &QUdpSocket::readyRead, this, &_Myt::readPendingDatagrams);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString DeviceDiscovery::hostname( const QString& device ) const
{
    if ( hostnames_.contains( device ) )
        return hostnames_[device];

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DeviceDiscovery *DeviceDiscovery::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DeviceDiscovery::readPendingDatagrams()
{
    static const int timeout( 8192 );

    while ( sock_->hasPendingDatagrams() )
    {
        QByteArray a;
        a.resize( sock_->pendingDatagramSize() );

        QHostAddress sender;
        quint16 senderPort;
        
        sock_->readDatagram( a.data(), a.size(), &sender, &senderPort );

        if ( !a.contains( '|' ) )
            continue;

        const QString as( a );
        const QStringList parts( as.trimmed().split( '|' ) );

        // check gateway datagram
        qDebug() << parts;

        if ( parts.size() < 3 )
            continue;
        else if ( "hobartGateway" != parts[0] )
            continue;

        // check we have already processed this gateway
        const QString addr( parts[1] );

        if ( 3 != addr.count( "." ) )
            continue;
        else if (( devices_.contains( addr ) ) || ( others_.contains( addr ) ))
            continue;

        // request capabilities
        QTcpSocket s;
        s.connectToHost( addr, SM_BACKEND_PORT );

        if ( !s.waitForConnected( timeout ) )
        {
            qWarning() << "failed to connect to client";
            continue;
        }

        const QByteArray m( "getCaps\n" );

        if ( m.length() != s.write( m ) )
        {
            qWarning() << "failed to request caps";
            continue;
        }
        else if ( !s.waitForBytesWritten( timeout ) )
        {
            qWarning() << "timeout sending request";
            continue;
        }
        else if ( !s.waitForReadyRead( timeout ) )
        {
            qWarning() << "timeout waiting for response";
            continue;
        }

        const QString caps( s.readAll() );

        // check edgify in capabilities
        if ( !caps.contains( "edgify" ) )
        {
            others_.append( addr );
            continue;
        }

        devices_.append( addr );

        // check for hostname
        if ( 4 <= parts.size() )
            hostnames_[addr] = parts[3];

        emit devicesChanged();
    }
}
