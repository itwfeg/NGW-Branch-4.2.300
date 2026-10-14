/**
 * @file commmanager.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "commmanager.h"
#include "connection.h"

#include <devicetype.h>
#include <commmanager_adaptor.h>

#include <QDebug>
#include <QTcpServer>

static const QString DBUS_SERVICE( COMM_MANAGER_DBUS_SERVICE );
static const QString DBUS_OBJECT_PATH( COMM_MANAGER_DBUS_OBJECT_PATH );

CommManager *CommManager::instance_( nullptr );
QMutex CommManager::instanceMutex_;

///////////////////////////////////////////////////////////////////////////////////////////////////
CommManager::CommManager() :
    _Mybase( DBUS_SERVICE, DBUS_OBJECT_PATH ),
    s_( new QTcpServer( this ) )
{
    connect( s_, &QTcpServer::acceptError, this, &_Myt::onAcceptError );
    connect( s_, &QTcpServer::newConnection, this, &_Myt::onNewConnection );

    // open server
    open();

    // create adaptor
    new CommManagerAdaptor( this );

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerObject( DBUS_OBJECT_PATH, this ) )
    {
        const QDBusError err( connection.lastError() );

        if ( QDBusError::NoError != err.type() )
            qWarning() << "failed to register object" << DBUS_OBJECT_PATH << err.type() << err.name() << err.message();
    }

    // register for events
    registerEvent( DeviceType::CommManager );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint CommManager::count() const
{
    const QList<Connection*> children( findChildren<Connection*>() );

    return children.count();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CommManager::isOpen() const
{
    return s_->isListening();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint CommManager::serverPort() const
{
    return s_->serverPort();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CommManager::close()
{
    qInfo() << "close server...";

    s_->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CommManager::open()
{
    if ( isOpen() )
    {
        qDebug() << "server already open";
        return true;
    }

    qDebug() << "open server...";

    if ( !s_->listen( QHostAddress::Any, SERVER_PORT ) )
    {
        qWarning() << "failed to listen";
        return false;
    }
    else if ( !isOpen() )
    {
        qWarning() << "failed to open server";
        return false;
    }

    qInfo() << "server is open and listening";
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CommManager::terminateConnection( const QString& name )
{
    if ( name.isEmpty() )
    {
        qWarning() << "missing connection name";
        return;
    }

    // check for existing session
    Connection *conn( findChild<Connection*>( name ) );

    if ( conn )
    {
        qDebug() << "destroy connection" << name;

        conn->deleteLater();
    }
    else
    {
        qWarning() << "could not find connection" << name;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CommManager *CommManager::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt;
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CommManager::onProcessEvent( const Event *e, bool& destroy )
{
    Q_UNUSED( e )
    Q_UNUSED( destroy )

    // TODO
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CommManager::onAcceptError( QAbstractSocket::SocketError socketError )
{
    qWarning() << "accept error" << socketError;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CommManager::onNewConnection()
{
    while ( s_->hasPendingConnections() )
    {
        qInfo() << "new connection...";
        new Connection( s_->nextPendingConnection(), this );
    }
}
