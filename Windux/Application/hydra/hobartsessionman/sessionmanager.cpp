/**
 * @file sessionmanager.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "session.h"

#include <sessioneventtype.h>
#include <sessionmanager.h>
#include <sessionmanager_adaptor.h>

#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

static const QString DBUS_SERVICE( SESSION_MANAGER_DBUS_SERVICE );
static const QString DBUS_OBJECT_PATH( SESSION_MANAGER_DBUS_OBJECT_PATH );

SessionManager *SessionManager::instance_( nullptr );
QMutex SessionManager::instanceMutex_;

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionManager::SessionManager() :
    _Mybase( DBUS_SERVICE, DBUS_OBJECT_PATH )
{
    // create list of well known services
    wellKnownServices_.append( RESOURCE_MANAGER_DBUS_SERVICE );
    wellKnownServices_.append( SCALE_DATABASE_DBUS_SERVICE );
    wellKnownServices_.append( DIAGNOSTICS_DATABASE_DBUS_SERVICE );

    // create adaptor
    new SessionManagerAdaptor( this );

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerObject( DBUS_OBJECT_PATH, this ) )
    {
        const QDBusError err( connection.lastError() );

        if ( QDBusError::NoError != err.type() )
            qWarning() << "failed to register object" << DBUS_OBJECT_PATH << err.type() << err.name() << err.message();
    }

    // regsiter for events
    registerEvent( DeviceType::SessionManager );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint SessionManager::count() const
{
    const QList<Session*> children( findChildren<Session*>() );

    return children.count();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString SessionManager::createNamedSession( uint type, const QString& name )
{
    qDebug() << "create session" << type << name;

    // check for existing session
    if ( name.length() )
    {
        const QString objPath( resumeNamedSession( name ) );

        if ( objPath.length() )
            return objPath;
    }

    // create new session
    qDebug() << "create new session...";

    Session *s( new Session( name, this ) );
    s->setServices( providerAliases_ );

    return s->objectPath();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString SessionManager::createSession( uint type )
{
    return createNamedSession( type, QString() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionManager::destroySession( const QString& name )
{
    if ( name.isEmpty() )
    {
        qWarning() << "missing session name";
        return;
    }

    // check for existing session
    Session *s( findChild<Session*>( name ) );

    if ( s )
    {
        qDebug() << "destroy session" << name;

        s->deleteLater();
    }
    else
    {
        qWarning() << "could not find session" << name;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString SessionManager::resumeNamedSession( const QString& name )
{
    qDebug() << "resume session" << name;

    // check for existing session
    if ( name.length() )
    {
        const QList<Session*> children( findChildren<Session*>() );

        foreach ( Session *s, children )
            if ( name == s->name() )
                return s->objectPath();
    }

    qWarning() << "failed to locate existing session" << name;

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionManager *SessionManager::instance()
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
void SessionManager::onProcessEvent( const Event *e, bool& destroy )
{
    Q_UNUSED( destroy )

    switch ( e->device() )
    {
    case DeviceType::SessionManager:
        processSessionManagerEvent( e );
        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionManager::processSessionManagerEvent( const Event *e )
{
    qDebug() << "process session manager event" << e->event();

    switch ( (SessionManagerEventType) e->event() )
    {
    case SessionManagerEventType::CreateSession:
        {
            const uint type( e->data<uint>( SESSION_MAN_CREATE_SESSION_TYPE ) );

            // create!
            const QString objPath( createSession( type ) );

            if ( objPath.length() )
            {
                // send response
                SessionManSessionCreated resp;
                resp.setData( SESSION_MAN_SESSION_CREATED_OBJ_PATH, objPath );

                send( e->senderService(), e->senderObjectPath(), &resp );
            }
        }

        break;

    case SessionManagerEventType::CreateNamedSession:
        {
            const uint prevCount( count() );

            const uint type( e->data<uint>( SESSION_MAN_CREATE_NAMED_SESSION_TYPE ) );
            const QString name( e->data<QString>( SESSION_MAN_CREATE_NAMED_SESSION_NAME ) );

            // create!
            const QString objPath( createNamedSession( type, name ) );

            if ( objPath.length() )
            {
                // send response
                if ( prevCount < count() )
                {
                    SessionManSessionCreated resp;
                    resp.setData( SESSION_MAN_SESSION_CREATED_OBJ_PATH, objPath );

                    send( e->senderService(), e->senderObjectPath(), &resp );
                }
                else
                {
                    SessionManSessionResumed resp;
                    resp.setData( SESSION_MAN_SESSION_RESUMED_OBJ_PATH, objPath );

                    send( e->senderService(), e->senderObjectPath(), &resp );
                }
            }
        }

        break;

    case SessionManagerEventType::DestroySession:
        {
            const QString objPath( e->data<QString>( SESSION_MAN_DESTROY_SESSION_OBJ_PATH ) );

            // destroy!
            destroySession( objPath );
        }

        break;

    case SessionManagerEventType::ResumeNamedSession:
        {
            const QString name( e->data<QString>( SESSION_MAN_RESUME_NAMED_SESSION_NAME ) );

            // resume!
            const QString objPath( resumeNamedSession( name ) );

            if ( objPath.length() )
            {
                // send response
                SessionManSessionResumed resp;
                resp.setData( SESSION_MAN_SESSION_RESUMED_OBJ_PATH, objPath );

                send( e->senderService(), e->senderObjectPath(), &resp );
            }
        }

        break;

    case SessionManagerEventType::RegisterServiceAlias:
        {
            const QString alias( e->data<QString>( SESSION_MAN_REG_SERVICE_ALIAS_NAME ) );

            if (( alias.length() ) && ( wellKnownServices_.contains( alias ) ))
            {
                const ServiceObjectPath s( e->senderService(), e->senderObjectPath() );

                if ( !providerAliases_.contains( alias, s ) )
                {
                    providerAliases_.insert( alias, s );
                    qDebug() << "added alias" << alias << "for [" << e->senderService() << e->senderObjectPath() << "]";
                }
            }
        }

        break;

    case SessionManagerEventType::UnregisterServiceAlias:
        {
            const QString alias( e->data<QString>( SESSION_MAN_REG_SERVICE_ALIAS_NAME ) );

            if (( alias.length() ) && ( wellKnownServices_.contains( alias ) ))
            {
                const ServiceObjectPath s( e->senderService(), e->senderObjectPath() );

                if ( providerAliases_.contains( alias, s ) )
                {
                    providerAliases_.remove( alias, s );
                    qDebug() << "removed alias" << alias << "for [" << e->senderService() << e->senderObjectPath() << "]";
                }
            }
        }

        break;

    default:
        break;
    }
}
