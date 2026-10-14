/**
 * @file commclient.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "commclient.h"

#include <messagefactory.h>
#include <ping.h>
#include <pong.h>

#include <QTimer>
#include <QDebug>

#ifdef Q_OS_WIN
#include <WinSock.h>
#else
#include <arpa/inet.h>
#endif

QMutex commClient::instanceLock_;
commClient *commClient::instance_( NULL );

///////////////////////////////////////////////////////////////////////////////////////////////////
commClient::commClient() :
    _Mybase(),
    sock_( new QTcpSocket( this ) ),
    keepAlive_( new QTimer( this ) ),
    timeout_( 0 )
{
    // socket signals
    connect( sock_, SIGNAL(connected()), SLOT(onConnected()) );
    connect( sock_, SIGNAL(disconnected()), SLOT(onDisconnected()) );
    connect( sock_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(onError(QAbstractSocket::SocketError)) );
    connect( sock_, SIGNAL(readyRead()), SLOT(onReadyRead()) );
    connect( sock_, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(onStateChanged(QAbstractSocket::SocketState)) );

    // setup keep alive
    keepAlive_->start( KEEP_ALIVE_TIMEOUT );

    connect( keepAlive_, SIGNAL(timeout()), SLOT(onTimeout()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
commClient::~commClient()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QAbstractSocket::SocketError commClient::error() const
{
    return sock_->error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString commClient::errorString() const
{
    return sock_->errorString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::connectToHost( const QString& hostName, quint16 port )
{
    sock_->connectToHost( hostName, port );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::sendMessage( const message *msg ) const
{
    const std::string buffer( msg->serialize() );
    const uint32_t bufferLen( htonl( buffer.size() ) );

    QByteArray a;
    a.append( reinterpret_cast<const char*>( &bufferLen ), sizeof(bufferLen) );
    a.append( buffer.c_str(), buffer.size() );

    sock_->write( a );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool commClient::waitForConnected( int msecs )
{
    return sock_->waitForConnected( msecs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
commClient *commClient::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceLock_ );

        if ( !instance_ )
            instance_ = new commClient();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::destroy()
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
void commClient::onConnected()
{
    qDebug() << this << "socket connect";
    Q_EMIT connected();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onDisconnected()
{
    qDebug() << this << "socket disconnect";
    Q_EMIT disconnected();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onError( QAbstractSocket::SocketError socketError )
{
    qWarning() << this << "socket error" << socketError << sock_->errorString();
    Q_EMIT error( socketError );

    sock_->disconnectFromHost();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onReadyRead()
{
    a_.append( sock_->readAll() );

    for ( ;; )
    {
        size_t len( 0 );

        const message *msg( messageFactory::create( a_.constData(), a_.length(), &len ) );

        // nothing processed
        if ( !len )
            break;

        // process!
        else if ( msg )
        {
            processMessage( msg );
            delete msg;
        }

        if ( a_.length() == (int) len )
        {
            a_.clear();
            break;
        }

        a_ = a_.right( a_.length() - len );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onStateChanged( QAbstractSocket::SocketState socketState )
{
    qWarning() << this << "socket state changed to" << socketState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onTimeout()
{
    if ( keepAlive_ == sender() )
    {
        // socket timeout
        if ( 5 < ++timeout_ )
        {
            qWarning() << this << "socket timeout";
            sock_->disconnectFromHost();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::processMessage( const message *msg )
{
    qDebug() << this << "recv message" << msg->type_ << msg->version_;

    switch ( msg->type_ )
    {
    case message::HFPD_PING: {
        pong response;
        sendMessage( &response );

        timeout_ = 0;
        break;
    }

    default:
        break;
    }

    Q_EMIT recvMessage( msg );
}
