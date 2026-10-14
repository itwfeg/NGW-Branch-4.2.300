/**
 * @file commserver.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "commclient.h"
#include "commserver.h"
#include "hobarttagmanager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
commServer::commServer( QObject *parent ) :
    _Mybase( parent ),
    tagManager_( hobartTagManager::instance() ),
    server_( new QTcpServer( this ) )
{
    // tcp server signals
    connect( server_, SIGNAL(acceptError(QAbstractSocket::SocketError)), SLOT(onAcceptError(QAbstractSocket::SocketError)) );
    connect( server_, SIGNAL(newConnection()), SLOT(onNewConnection()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
commServer::~commServer()
{
    // close server
    close();

    // disconnect all clients
    disconnectAll();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool commServer::isListening() const
{
    return server_->isListening();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::close()
{
    server_->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::disconnectAll()
{
    qDebug() << "disconnecting all clients...";

    // connected clients
    QList<commClient*> clients( findChildren<commClient*>() );

    foreach ( commClient *client, clients )
        client->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool commServer::listen( const QHostAddress& address, quint16 port )
{
    return server_->listen( address, port );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::onAcceptError( QAbstractSocket::SocketError socketError )
{
    qWarning() << "server accept error" << socketError << server_->errorString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::onClientDestroyed( QObject *obj )
{
    qInfo() << "client disconnect" << obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commServer::onNewConnection()
{
    QTcpSocket *sock;

    while ( (sock = server_->nextPendingConnection()) )
    {
        // create client
        commClient *client( new commClient( sock, this ) );

        connect( client, SIGNAL(destroyed(QObject*)), SLOT(onClientDestroyed(QObject*)) );

        qInfo() << "have new client" << client;
    }
}
