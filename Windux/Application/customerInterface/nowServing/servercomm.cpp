/**
 * @file servercomm.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "servercomm.h"

#include <QSignalMapper>
#include <QStringList>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

QMutex serverComm::instanceMutex_;
serverComm *serverComm::instance_( NULL );

///////////////////////////////////////////////////////////////////////////////////////////////////
serverComm::serverComm( const QString& server, int port, const QString& queue, int updatePort, modeType mode, QObject *parent ) :
    _Mybase( parent ),
    server_( server ),
    port_( port ),
    queue_( queue ),
    updatePort_( updatePort ),
    mode_( mode ),
    lastError_( 0 )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serverComm::~serverComm()
{
    // remove instance
    instance_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString serverComm::errorString() const
{
    switch ( error() )
    {
    // internal error codes
    case ERROR_SENDING_REQUEST:
        return tr( "Error sending request." );
    case TIMEOUT_WAITING_FOR_CONNECTION:
        return tr( "Connection timed out." );
    case TIMEOUT_WAITING_FOR_RESPONSE:
        return tr( "Time out waiting for response." );
    case INVALID_RESPONSE_FROM_SERVER:
        return tr( "Bad response from server." );
    case INVALID_RESPONSE_FORMAT:
        return tr( "Invalid response format from server." );

    // server error codes
    case SUCCESS:
        return tr( "Success." );
    case INVALID_REQUEST:
        return tr( "Server responded, but the request was invalid." );
    case LICENSE_NOT_INSTALLED:
        return tr( "Server is not licensed. Install a license on the server." );
    case INVALID_QUEUE_SETUP:
        return tr( "Queue is not setup properly. Please check your configuration in supervisor menu." );
    case NO_CUSTOMERS_QUEUED:
        return tr( "There are no customers queued at this time." );
    case INVALID_PRINTER_TEMPLATE:
        return tr( "Server is not setup properly, please install a ticket template." );
    case NO_PRINTER_ATTACHED:
        return tr( "No printer attached to server." );
    case INVALID_NUMBER:
        return tr( "Server unable to use the number you entered." );
    default:
        break;
    }

    return tr( "An unknown error has occured." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serverComm::incrementServingQueue( int& customer )
{
    static const char cmd[] = "increment_serving_queue";

    QStringList resp;

    // perform comm
    if ( !sendRecv( resp, cmd ) )
        return false;

    // validate response
    else if ( resp.size() < 2 )
    {
        lastError_ = INVALID_RESPONSE_FROM_SERVER;
        return false;
    }

    customer = resp[1].toInt();
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serverComm::alloc( const QString& server, int port, const QString& queue, int updatePort, modeType mode, QObject *parent )
{
   if ( instance_ )
       return;

   QMutexLocker guard( &instanceMutex_ );

   if ( !instance_ )
       instance_ = new serverComm( server, port, queue, updatePort, mode, parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serverComm::sendRecv( QStringList& resp, const QString& cmd, const QStringList& param )
{
    QTcpSocket sock;

    // format request
    QString request;
    request.append( cmd );
    request.append( "|" );
    request.append( queue_ );

    foreach ( const QString& p, param )
    {
        request.append( "|" );
        request.append( p );
    }

    request.append( "\n" );

    // connect to server
    sock.connectToHost( server_, port_ );

    if ( !sock.waitForConnected( 3000 ) )
    {
        lastError_ = TIMEOUT_WAITING_FOR_CONNECTION;
        return false;
    }

    // write request
    else if ( request.length() != sock.write( request.toLatin1() ) )
    {
        lastError_ = ERROR_SENDING_REQUEST;
        return false;
    }

    // wait for response
    else if ( !sock.waitForReadyRead( 2000 ) )
    {
        lastError_ = TIMEOUT_WAITING_FOR_RESPONSE;
        return false;
    }

    // read data
    QString buffer( sock.readAll() );

    // disconnect
    sock.disconnectFromHost();

    // parse response!
    resp = buffer.split( "|" );

    // validate
    if ( resp.size() <= 0 )
    {
        lastError_ = INVALID_RESPONSE_FROM_SERVER;
        return false;
    }
    else if ( "success" == resp[0] )
    {
        lastError_ = SUCCESS;
        return true;
    }

    // check for error code
    if (( "error" == resp[0] ) && ( 3 <= resp.size() ))
        lastError_ = resp[2].toInt();
    else
        lastError_ = INVALID_RESPONSE_FORMAT;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serverComm::onDisconnect( QObject *sock )
{
    sock->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serverComm::onNewConnection()
{
    // process all pending connections
    while ( tcpUpdates_->hasPendingConnections() )
    {
        bool valid( false );

        // get connection
        QTcpSocket *sock( tcpUpdates_->nextPendingConnection() );

        // validate state
        if ( QTcpSocket::ConnectedState == sock->state() )
        {
            QString senderAddress(  sock->peerAddress().toString() );

            // only process connections from our server
            if ( server_ == senderAddress )
            {
                disconnectSignal_->setMapping( sock, sock );
                connect( sock, SIGNAL( disconnected() ), disconnectSignal_, SLOT( map() ) );

                readyReadSignal_->setMapping( sock, sock );
                connect( sock, SIGNAL( readyRead() ), readyReadSignal_, SLOT( map() ) );

                valid = true;
            }
        }

        // invalid socket
        if ( !valid )
        {
            sock->disconnectFromHost();
            sock->deleteLater();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serverComm::onReadDatagram()
{
    // process all pending comm
    while ( udpUpdates_->hasPendingDatagrams() )
    {
        QHostAddress sender;

        QByteArray buffer;
        buffer.resize( udpUpdates_->pendingDatagramSize() );

        // read datagram
        if ( buffer.size() == udpUpdates_->readDatagram( buffer.data(), buffer.size(), &sender, NULL ) )
        {
            QString senderAddress( sender.toString() );

            // only process datagrams from our server
            if ( server_ == senderAddress )
            {
                buffer_.append( buffer );
                processBuffer();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serverComm::onReadyRead( QObject *obj )
{
    QTcpSocket *sock( dynamic_cast<QTcpSocket*>( obj ) );

    if ( !sock )
        return;

    // process buffer
    buffer_.append( sock->readAll() );
    processBuffer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serverComm::initialize()
{
    disconnectSignal_ = new QSignalMapper( this );
    connect( disconnectSignal_, SIGNAL( mapped(QObject*) ), SLOT( onDisconnect(QObject*) ) );

    readyReadSignal_ = new QSignalMapper( this );
    connect( readyReadSignal_, SIGNAL( mapped(QObject*) ), SLOT( onReadyRead(QObject*) ) );

    // setup server socket
    if ( tcpServerMode == mode_ )
    {
        tcpUpdates_ = new QTcpServer( this );

#if ( QT_VERSION < 0x050000 )
        tcpUpdates_->listen( QHostAddress::Any, updatePort_ );
#else
        tcpUpdates_->listen( QHostAddress::AnyIPv4, updatePort_ );
#endif

        connect( tcpUpdates_, SIGNAL( newConnection() ), SLOT( onNewConnection() ) );
    }
    else if ( udpServerMode == mode_ )
    {
        udpUpdates_ = new QUdpSocket( this );
        udpUpdates_->bind( updatePort_ );

        connect( udpUpdates_, SIGNAL( readyRead() ), SLOT( onReadDatagram() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serverComm::processBuffer()
{
    int i;

    // process messages on buffer
    while ( 0 < (i = buffer_.indexOf( '\n' )) )
    {
        QByteArray buffer( buffer_.left( i ) );

        // remove message from buffer
        buffer_ = buffer_.mid( i+1 );

        // parse message!
        QStringList msg( QString( buffer ).split( "|" ) );

        // validate
        if (( 4 <= msg.size() ) &&
            ( "queue_status" == msg[0] ) &&
            ( queue_ == msg[1] ))
        {
            QString currentlyServing( msg[2] );
            QString queueSize( msg[3] );

            // emit update!
            Q_EMIT updateCurrentlyServing( currentlyServing.toInt(), queueSize.toInt() );
        }
    }
}
