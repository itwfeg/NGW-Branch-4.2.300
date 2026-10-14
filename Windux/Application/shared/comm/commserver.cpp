/**
 * @file commserver.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include <QtNetwork/QtNetwork>

#include "commserver.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
commServer::commServer( unsigned int port ) :
    address_( QHostAddress::Any ),
    port_( port ),
    tcpServer_( NULL ),
    session_( NULL ),
    client_( NULL ),
    readErrors_( 0 )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
commServer::commServer( const QHostAddress& address, unsigned int port ) :
    address_( address ),
    port_( port ),
    tcpServer_( NULL ),
    session_( NULL ),
    client_( NULL ),
    readErrors_( 0 )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
commServer::~commServer()
{
    if ( tcpServer_ )
        delete tcpServer_;

    if ( session_ )
        delete session_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool commServer::write( const QByteArray& data ) const
{
    LOG_CLASS_INFO( "commServer", "write" );

    unsigned int written = 0;

    unsigned int dataLen = data.length();

    if ( !client_ )
        LOG_WARN( "no client!", NULL );
    else
    {
        LOG_INFO( "writing %d bytes...", dataLen );
        LOG_DEBUG_DUMP( ((const void*) data), dataLen );

        written = client_->write( data );
    }

    if ( written != dataLen )
        LOG_ERROR( "write error!", NULL );

    return (written == dataLen);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::sessionOpened()
{
    LOG_CLASS_INFO( "commServer", "sessionOpened" );

    // Save the used configuration
    if ( session_ )
    {
        QString id;

        QNetworkConfiguration config = session_->configuration();

        if ( QNetworkConfiguration::UserChoice == config.type() )
            id = session_->sessionProperty( QLatin1String( "UserChoiceConfiguration" ) ).toString();
        else
            id = config.identifier();

        QSettings settings( QSettings::UserScope, QLatin1String( "HobartGcd" ) );

        settings.beginGroup( QLatin1String( "QtNetwork" ) );
        settings.setValue( QLatin1String( "DefaultNetworkConfiguration" ), id );
        settings.endGroup();
    }

    // instantiate TCP Server
    tcpServer_ = new QTcpServer( this );

    // connect a signal for new client connections
    connect(
        tcpServer_,
        SIGNAL( newConnection() ),
        SLOT( sessionConnect() ) );

    // Listen on port for client connections
    if ( !tcpServer_->listen( address_, port_ ) )
        LOG_ERROR( "error listening!", NULL );
    else
    {
        LOG_INFO( "listening on ip %s port %d",
            tcpServer_->serverAddress().toString().toLatin1().constData(),
            tcpServer_->serverPort() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::sessionConnect()
{
    LOG_CLASS_INFO( "commServer", "sessionConnect" );

    LOG_INFO( "new connection...", NULL );

    // handle new client connection by saving client TCP server pointer
    if ( (client_ = tcpServer_->nextPendingConnection()) )
    {
        connect(
            client_,
            SIGNAL( readyRead() ),
            SLOT( clientRead() ) );

        connect(
            client_,
            SIGNAL( disconnected() ),
            SLOT( clientDisconnected() ) );

        connect(
            client_,
            SIGNAL( error( QAbstractSocket::SocketError ) ),
            SLOT( clientError( QAbstractSocket::SocketError ) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::clientRead()
{
    LOG_CLASS_INFO( "commServer", "clientRead" );

    // append all data from socket to our buffer
    buffer_.append( client_->readAll() );

    bool valid = false;

    LOG_INFO( "got data...", NULL );
    LOG_DEBUG_DUMP( ((const void*) buffer_), buffer_.length() );

    unsigned int len = 0;

    // parse data
    while ( read( buffer_, len ) )
    {
        LOG_DEBUG( "successfully parsed message of %d bytes", len );

        // removed parsed data
        buffer_.remove( 0, len );
        readErrors_ = 0;

        // success!
        valid = true;
        len = 0;
    }

    // error parsing data, if too many reset our buffer!
    if (( !valid ) && ( READ_ERROR_LIMIT <= ++readErrors_ ))
    {
        LOG_ERROR( "read error limit, clearing buffer...", NULL );
        LOG_ERROR_DUMP( ((const void*) buffer_), buffer_.length() );

        buffer_.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::clientError( QAbstractSocket::SocketError socketError )
{
    LOG_CLASS_INFO( "commServer", "clientError" );

    if ( QTcpSocket::RemoteHostClosedError == socketError )
    {
        LOG_WARN( "remote host was closed", NULL );

        disconnect();
        return;
    }

    LOG_ERROR( "socket error %d '%s'", socketError, client_->errorString().toLatin1().constData() );

    tcpServer_->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::clientDisconnected()
{
    if ( client_ )
    {
        client_->deleteLater();
        client_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::initialize()
{
    QNetworkConfigurationManager manager;

    // open the session and set network configuration based on discovered IP and port
    if ( !(QNetworkConfigurationManager::NetworkSessionRequired & manager.capabilities()) )
        sessionOpened();
    else
    {
        // Get saved network configuration
        QSettings settings( QSettings::UserScope, QLatin1String( "Hobart" ) );

        settings.beginGroup( QLatin1String( "QtNetwork" ) );
        const QString id = settings.value( QLatin1String( "DefaultNetworkConfiguration" ) ).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier( id );
        if ( QNetworkConfiguration::Discovered != (QNetworkConfiguration::Discovered & config.state()) )
            config = manager.defaultConfiguration();

        // Instantiate network object with existing network configuration assuming it exists
        session_ = new QNetworkSession( config, this );

        // Connect slot sessionOpened routine to handle setting up network monitor of port
        connect(
            session_,
            SIGNAL( opened() ),
            this,
            SLOT( sessionOpened() ) );

        // open the network session
        session_->open();
    }
}
