/**
 * @file messagecenter.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "messagecenter.h"

#include <messagecenter_adaptor.h>

#include <QDBusConnection>
#include <QDebug>

static const QString DBUS_OBJECT_PATH( "/Hobart/MessageCenter" );

MessageCenter *MessageCenter::instance_( nullptr );
QMutex MessageCenter::instanceMutex_;

///////////////////////////////////////////////////////////////////////////////////////////////////
MessageCenter::MessageCenter() :
    _Mybase()
{
    // create adaptor
    new MessageCenterAdaptor( this );

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerObject( DBUS_OBJECT_PATH, this ) )
    {
        const QDBusError err( connection.lastError() );

        if ( QDBusError::NoError != err.type() )
            qWarning() << "failed to register object" << DBUS_OBJECT_PATH << err.type() << err.name() << err.message();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MessageCenter *MessageCenter::instance()
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
void MessageCenter::post( const QByteArray& a )
{
    uint device;
    uint event;

    // no data (or not enough data)
    if ( a.size() < 20 )
        return;

    // extract device and event
    const char *data( a.constData() );

    memcpy( &device, &data[0], sizeof(uint) );
    memcpy( &event, &data[sizeof(uint)], sizeof(uint) );

    // validate
    if (( !device ) || ( !event ))
        return;

    qDebug() << "POST" << device << event << a.size();

    emit processEvent( a );
}
