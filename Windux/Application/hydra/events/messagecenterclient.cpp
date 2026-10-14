/**
 * @file messagecenterclient.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "event.h"
#include "eventfactory.h"
#include "messagecenterclient.h"

#include <messagecenterclient_adaptor.h>
#include <messagecenterclient_interface.h>
#include <messagecenter_interface.h>

#include <QDBusConnection>

///////////////////////////////////////////////////////////////////////////////////////////////////
MessageCenterClient::MessageCenterClient( const QString& s, const QString& path, QObject *parent ) :
    _Mybase( parent ),
    service_( s ),
    objectPath_( path ),
    all_( false ),
    self_( false )
{
    // create adaptor
    new MessageCenterClientAdaptor( this );

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerObject( objectPath(), this ) )
    {
        const QDBusError err( connection.lastError() );

        if ( QDBusError::NoError != err.type() )
            qWarning() << "failed to register object" << objectPath() << err.type() << err.name() << err.message();
    }

    // create interface to message center
    mc_ = new com::hobartcorp::MessageCenter(
        MESSAGE_CENTER_DBUS_SERVICE,
        MESSAGE_CENTER_DBUS_OBJECT_PATH,
        QDBusConnection::sessionBus(),
        this );

    if ( !mc_ )
        qWarning() << "failed to allocate message center";
    else
    {
        connect( mc_, &com::hobartcorp::MessageCenter::processEvent, this, &_Myt::processEvent );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString MessageCenterClient::service() const
{
    return service_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString MessageCenterClient::objectPath() const
{
    return objectPath_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::processEvent( const QByteArray& a )
{
    bool destroy( true );

    // create event
    Event *e( EventFactory::create( a ) );

    if ( !e )
    {
        qWarning() << "failed to create event!";
        return;
    }

    if ( !e->isValid() )
        qWarning() << "invalid event" << (uint) e->device() << e->event();
    else
    {
        bool process( true );

        qDebug() << "RECV" << (uint) e->device() << e->event() << a.size() << "[" << e->senderService() << e->senderObjectPath() << "]";

        // check for our own messages
        if (( !enableSelfProcessing() ) && ( service() == e->senderService() ) && ( objectPath() == e->senderObjectPath() ))
            process = false;

        // check we are registered for this event
        if ( !all_ )
        {
            const EventType dev( e->device(), 0 );

            if ( !events_.contains( dev ) )
            {
                const EventType t( e->device(), e->event() );

                if ( !events_.contains( t ) )
                    process = false;
            }
        }

        // process event!
        if ( process )
        {
            qInfo() << "process" << (uint) e->device() << e->event() << "...";
            onProcessEvent( e, destroy );
        }
    }

    if ( destroy )
        destroyEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::destroyEvent( const Event *doomed ) const
{
    if ( doomed )
        EventFactory::destroy( doomed );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MessageCenterClient::enableSelfProcessing() const
{
    return self_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::setEnableSelfProcessing( bool value )
{
    self_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::setRegisterAllEvents( bool value )
{
    all_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::onProcessEvent( const Event *e, bool& destroy )
{
    Q_UNUSED( e );
    Q_UNUSED( destroy )

    // implement this in derived object
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::post( DeviceType device, uint event ) const
{
    Event e( device, event );
    post( &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::post( const Event *e ) const
{
    if (( !e ) || ( !e->isValid() ))
        qWarning() << "invalid message";
    else if (( !mc_ ) || ( !mc_->isValid() ))
        qWarning() << "invalid message center";
    else
    {
        Event *temp( nullptr );

        // set sender information
        if (( e->senderService() != service() ) || ( e->senderObjectPath() != objectPath() ))
        {
            temp = new Event( (*e) );
            temp->setSenderService( service() );
            temp->setSenderObjectPath( objectPath() );

            e = temp;
        }

        // post!
        mc_->post( e->serialize() );

        if ( temp )
            delete temp;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::registerEvent( DeviceType device, uint event )
{
    const EventType t( device, event );

    if ( events_.contains( t ) )
        return;

    events_.append( t );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::send( const QString& s, const QString& path, DeviceType device, uint event ) const
{
    Event e( device, event );
    send( s, path, &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::send( const QString& s, const QString& path, const Event *e ) const
{
    if ( s.isEmpty() )
        qWarning() << "service missing from send";
    else if ( path.isEmpty() )
        qWarning() << "object path missing from send";
    else if (( !e ) || ( !e->isValid() ))
        qWarning() << "invalid message";
    else
    {
        com::hobartcorp::MessageCenterClient *client =
            new com::hobartcorp::MessageCenterClient(
                s,
                path,
                QDBusConnection::sessionBus() );

        if ( !client )
            qWarning() << "failed to allocate message center client";
        else
        {
            if ( !client->isValid() )
                qWarning() << "invalid message center client";
            else
            {
                Event *temp( nullptr );

                // set sender information
                if (( e->senderService() != service() ) || ( e->senderObjectPath() != objectPath() ))
                {
                    temp = new Event( (*e) );
                    temp->setSenderService( service() );
                    temp->setSenderObjectPath( objectPath() );

                    e = temp;
                }

                // send!
                const QByteArray a( e->serialize() );

                qDebug() << "SEND" << (uint) e->device() << e->event() << a.size() << "[" << e->senderService() << e->senderObjectPath() << "->" << s << path << "]";
                client->processEvent( a );

                if ( temp )
                    delete temp;
            }

            delete client;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageCenterClient::unregisterEvent( DeviceType device, uint event )
{
    // remove specific device and event
    if ( 0 < event )
    {
        const EventType t( device, event );

        if ( !events_.contains( t ) )
            return;

        events_.removeOne( t );
    }

    // remove all events for device
    else
    {
        EventTypeList doomed;

        // make list of events to remove
        foreach ( const EventType& t, events_ )
            if ( t.first == device )
                doomed.append( t );

        // remove them
        foreach ( const EventType& t, doomed )
            events_.removeOne( t );
    }
}
