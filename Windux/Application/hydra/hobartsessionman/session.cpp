/**
 * @file session.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "session.h"

#include <databaseeventtype.h>
#include <databasejson.h>
#include <fileresourceevent.h>
#include <jsondocumentevent.h>
#include <sessioneventtype.h>
#include <session_adaptor.h>

#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

static const QString DBUS_SERVICE( SESSION_MANAGER_DBUS_SERVICE );
static const QString DBUS_OBJECT_PATH( SESSION_DBUS_OBJECT_PATH );

// uncomment to debug content data
#define DEBUG_JSON

// define proper constant dependending on qt version
#if QT_VERSION >= 0x050e00
#define KEEP_EMPTY_PARTS Qt::KeepEmptyParts
#define SKIP_EMPTY_PARTS Qt::SkipEmptyParts
#else
#define KEEP_EMPTY_PARTS QString::KeepEmptyParts
#define SKIP_EMPTY_PARTS QString::SkipEmptyParts
#endif

QMutex Session::m_;
uint Session::nextSession_( 1 );

///////////////////////////////////////////////////////////////////////////////////////////////////
Session::Session( const QString& n, QObject *parent ) :
    _Mybase( DBUS_SERVICE, DBUS_OBJECT_PATH.arg( nextSessionId() ), parent ),
    name_( n )
{
    qInfo() << "CTOR" << objectPath();

    // set object name
    setObjectName( objectPath() );

    // parse out session id
    QStringList l( objectName().split( "/", SKIP_EMPTY_PARTS ) );

    if ( 3 == l.size() )
        id_ = l[2];

    // create adaptor
    new SessionAdaptor( this );

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerObject( objectPath(), this ) )
    {
        const QDBusError err( connection.lastError() );

        if ( QDBusError::NoError != err.type() )
            qWarning() << "failed to register object" << objectPath() << err.type() << err.name() << err.message();
    }

    // regsiter for events
    registerEvent( DeviceType::Session );

    // the read/write/delete events of these database devices require a department number
    deptNumRequired_.append( DeviceType::ProductDatabase );
    deptNumRequired_.append( DeviceType::CategoryDatabase );

    deptNumRequired_.append( DeviceType::ProductTextDatabase );
    deptNumRequired_.append( DeviceType::NutritionDatabase );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Session::~Session()
{
    qInfo() << "DTOR" << objectPath();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Session::setServices( const ServiceProviderMap& value )
{
    services_ = value;

    // register resource devices
    if ( services_.contains( RESOURCE_MANAGER_DBUS_SERVICE ) )
    {
        registerEvent( DeviceType::GraphicResource );
        registerEvent( DeviceType::FontResource );
    }

    // register scale database devices
    if ( services_.contains( SCALE_DATABASE_DBUS_SERVICE ) )
    {
        registerEvent( DeviceType::DepartmentDatabase );

        registerEvent( DeviceType::ProductDatabase );
        registerEvent( DeviceType::OperatorDatabase );
        registerEvent( DeviceType::OperatorNotesDatabase );
        registerEvent( DeviceType::CategoryDatabase );

        registerEvent( DeviceType::ProductTextDatabase );
        registerEvent( DeviceType::NutritionDatabase );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Session::ServiceProviderMap Session::services() const
{
    return services_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString Session::id() const
{
    return id_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString Session::name() const
{
    return name_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Session::onProcessEvent( const Event *e, bool& destroy )
{
    Q_UNUSED( destroy )

    qDebug() << "process event" << (uint) e->device() << e->event();

    switch ( e->device() )
    {
    case DeviceType::Session:

        processSessionEvent( e );
        break;

    case DeviceType::GraphicResource:
    case DeviceType::FontResource:

        processResourceEvent( dynamic_cast<const FileResourceEvent*>( e ) );
        break;

    case DeviceType::DepartmentDatabase:

    case DeviceType::ProductDatabase:
    case DeviceType::OperatorDatabase:
    case DeviceType::OperatorNotesDatabase:
    case DeviceType::CategoryDatabase:

    case DeviceType::ProductTextDatabase:
    case DeviceType::NutritionDatabase:

        processScaleDatabaseEvent( dynamic_cast<const JsonDocumentEvent*>( e ) );
        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Session::sendToService( const QString& service, DeviceType device, uint event ) const
{
    const Event e( device, event );
    sendToService( service, &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Session::sendToService( const QString& service, const Event *e ) const
{
    // iterate service providers
    ServiceProviderMap::const_iterator i( services_.find( service ) );

    while (( i != services_.constEnd() ) && ( service == i.key() ))
    {
        const ServiceObjectPath s( i.value() );

        // send!
        send( s.first, s.second, e );

        // next provider
        ++i;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Session::processSessionEvent( const Event *e )
{
    qDebug() << "process session event" << e->event();

    switch ( (SessionEventType) e->event() )
    {
    case SessionEventType::Register:
        {
            const ServiceObjectPath owner( e->senderService(), e->senderObjectPath() );

            if (( !owner.first.isEmpty() ) && ( !owner.second.isEmpty() ) && ( !owners_.contains( owner ) ))
            {
                owners_.append( owner );
                qInfo() << "session" << id_ << "owner added" << owner.first << owner.second;
            }
        }

        break;

    case SessionEventType::Unregister:
        {
            const ServiceObjectPath owner( e->senderService(), e->senderObjectPath() );

            if (( !owner.first.isEmpty() ) && ( !owner.second.isEmpty() ) && ( owners_.contains( owner ) ))
            {
                owners_.removeOne( owner );
                qInfo() << "session" << id_ << "owner removed" << owner.first << owner.second;
            }
        }

        break;

    case SessionEventType::RegisterEvent:
        {
            const DeviceType dt( (DeviceType) e->data<uint>( SESSION_REGISTER_EVENT_DEVICE_TYPE ) );
            const uint event( e->data<uint>( SESSION_REGISTER_EVENT_EVENT_TYPE ) );

            registerEvent( dt, event );
        }

        break;

    case SessionEventType::UnregisterEvent:
        {
            const DeviceType dt( (DeviceType) e->data<uint>( SESSION_UNREGISTER_EVENT_DEVICE_TYPE ) );
            const uint event( e->data<uint>( SESSION_UNREGISTER_EVENT_EVENT_TYPE ) );

            unregisterEvent( dt, event );
        }

        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Session::processScaleDatabaseEvent( const JsonDocumentEvent *e )
{
    if ( !e )
        return;

    qDebug() << "process scale database event" << (uint) e->device() << e->event();

    const ServiceObjectPath sender( e->senderService(), e->senderObjectPath() );

    bool update( false );

    JsonDocumentEvent ev( (*e) );
    ev.setSenderService( service() );
    ev.setSenderService( objectPath() );

    const QJsonDocument doc( ev.document() );

    QJsonObject obj;

    if ( doc.isObject() )
        obj = doc.object();

#ifdef DEBUG_JSON
    qDebug() << "RECV\n" << obj;
#endif

    // check for missing department number
    if ( deptNumRequired_.contains( ev.device() ) )
    {
        if (( DatabaseEventType::Read == (DatabaseEventType) ev.event() ) ||
            ( DatabaseEventType::Write == (DatabaseEventType) ev.event() ) ||
            ( DatabaseEventType::Delete == (DatabaseEventType) ev.event() ))
        {
            // default to department zero (0)
            if ( !obj.contains( JSON_DEPTNUM_FIELD ) )
            {
                qDebug() << "event missing department";

                obj[JSON_DEPTNUM_FIELD] = "0";
                update = true;
            }
        }
    }

    // TODO
    // apply business logic to event

    // update event
    if ( update )
    {
#ifdef DEBUG_JSON
        qDebug() << "UPDATE\n" << obj;
#endif
        ev.setDocument( QJsonDocument( obj ) );
    }

    // came from owner, forward to datbase service
    if ( owners_.contains( sender ) )
        sendToService( SCALE_DATABASE_DBUS_SERVICE, &ev );

    // came from service, forward to owners
    else
    {
        foreach ( const ServiceObjectPath& owner, owners_ )
            send( owner.first, owner.second, &ev );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Session::processResourceEvent( const FileResourceEvent *e )
{
    if ( !e )
        return;

    qDebug() << "process resource event" << (uint) e->device() << e->event();

    const ServiceObjectPath sender( e->senderService(), e->senderObjectPath() );

    FileResourceEvent ev( (*e) );
    ev.setSenderService( service() );
    ev.setSenderService( objectPath() );

    // TODO
    // apply business logic to event

    // came from owner, forward to resource manager service
    if ( owners_.contains( sender ) )
        sendToService( RESOURCE_MANAGER_DBUS_SERVICE, &ev );

    // came from service, forward to owners
    else
    {
        foreach ( const ServiceObjectPath& owner, owners_ )
            send( owner.first, owner.second, &ev );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString Session::nextSessionId()
{
    QMutexLocker guard( &m_ );

    // increment session counter
    QString id( QString::number( nextSession_++, 16 ) );

    // return hex string of length 8 characters
    while ( id.length() < 8 )
        id.prepend( "0" );

    return id;
}
