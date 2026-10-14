/**
 * @file dbusmediaplayerclient.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <QtGlobal>

// Linux only!
#ifndef Q_OS_WIN

#include "dbusmediaplayerclient.h"

#include <logmanager.h>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

///////////////////////////////////////////////////////////////////////////////////////////////////
dbusMediaPlayerClient::dbusMediaPlayerClient( const QString &service, QObject *parent ) :
    _Mybase( dbus_client, parent ),
    serviceName_( service ),
    props_( NULL ),
    object_( NULL )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "dbusMediaPlayerClient" );

    LOG_INFO( "service name: %s", qPrintable( dbusServiceName() ) );
    LOG_INFO( "object interface: %s", qPrintable( dbusObjectInterface() ) );
    LOG_INFO( "object path: %s", qPrintable( dbusObjectPath() ) );

    QDBusConnection dbus( QDBusConnection::sessionBus() );

    // create interface for property introspection
    props_ = new QDBusInterface( dbusServiceName(), dbusObjectPath(), "org.freedesktop.DBus.Properties", dbus, this );

    if ( !props_->isValid() )
    {
        LOG_ERROR( "service '%s' invalid: %s", qPrintable( service ), qPrintable( dbus.lastError().message() ) );
        return;
    }

    // create interface for object
    object_ = new QDBusInterface( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), dbus, this );

    if ( !object_->isValid() )
    {
        LOG_ERROR( "service '%s' invalid: %s", qPrintable( service ), qPrintable( dbus.lastError().message() ) );
        return;
    }

    // connect signals
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "audioAvailableChanged", this, SLOT(onAudioAvailableChanged(bool)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "durationChanged", this, SLOT(onDurationChanged(qint64)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "error", this, SLOT(onError(QString)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "finished", this, SLOT(onFinished()) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "geometryChanged", this, SLOT(onGeometryChanged(QRect)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "mediaStatusChanged", this, SLOT(onMediaStatusChanged(abstractMediaPlayer::MediaStatus)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "mutedChanged", this, SLOT(onMutedChanged(bool)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "positionChanged", this, SLOT(onPositionChanged(qint64)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "seekableChanged", this, SLOT(onSeekableChanged(bool)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "stateChanged", this, SLOT(onStateChanged(abstractMediaPlayer::State)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "videoAvailableChanged", this, SLOT(onVideoAvailableChanged(bool)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "visibleChanged", this, SLOT(onVisibleChanged(bool)) );
    dbus.connect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "volumeChanged", this, SLOT(onVolumeChanged(int)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dbusMediaPlayerClient::~dbusMediaPlayerClient()
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "~dbusMediaPlayerClient" );
	
    QDBusConnection dbus( QDBusConnection::sessionBus() );

    LOG_TRACE( "disconnecting...", NULL );

    // disconnect signals
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "audioAvailableChanged", this, SLOT(onAudioAvailableChanged(bool)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "durationChanged", this, SLOT(onDurationChanged(qint64)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "error", this, SLOT(onError(QString)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "finished", this, SLOT(onFinished()) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "geometryChanged", this, SLOT(onGeometryChanged(QRect)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "mediaStatusChanged", this, SLOT(onMediaStatusChanged(abstractMediaPlayer::MediaStatus)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "mutedChanged", this, SLOT(onMutedChanged(bool)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "positionChanged", this, SLOT(onPositionChanged(qint64)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "seekableChanged", this, SLOT(onSeekableChanged(bool)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "stateChanged", this, SLOT(onStateChanged(abstractMediaPlayer::State)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "videoAvailableChanged", this, SLOT(onVideoAvailableChanged(bool)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "visibleChanged", this, SLOT(onVisibleChanged(bool)) );
    dbus.disconnect( dbusServiceName(), dbusObjectPath(), dbusObjectInterface(), "volumeChanged", this, SLOT(onVolumeChanged(int)) );
	
    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
T dbusMediaPlayerClient::getPropValue( const QString &prop )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "getPropValue" );

    // validate interface
    if (( !props_ ) || ( !props_->isValid() ))
        LOG_WARN( "invalid interface", NULL );

    // invoke!
    else
    {
        LOG_TRACE( "get prop '%s'...", qPrintable( prop ) );
        QDBusReply<QDBusVariant> reply( props_->call( "Get", dbusObjectInterface(), prop ) );

        // validate reply
        if ( reply.isValid() )
        {
            // extract variant
            const QVariant v( reply.value().variant() );

            LOG_TRACE( "done", NULL );
            return qvariant_cast<T>( v );
        }
        else
        {
            const QString message( "call to get '" + prop + "' failed: " + reply.error().message() );

            LOG_WARN( "dbus error: '%s'", qPrintable( message ) );
            Q_EMIT busError( message );
        }
    }

    // bad response
    return T();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void dbusMediaPlayerClient::setPropValue( const QString &prop, const T &value )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "setPropValue" );

    // validate interface
    if (( !props_ ) || ( !props_->isValid() ))
    {
        LOG_WARN( "invalid interface", NULL );
        return;
    }

    // invoke!
    const QDBusVariant v( QVariant::fromValue( value ) );

    LOG_TRACE( "set prop '%s'...", qPrintable( prop ) );
    QDBusReply<void> reply( props_->call( "Set", dbusObjectInterface(), prop, QVariant::fromValue( v ) ) );

    // validate reply
    if ( reply.isValid() )
        LOG_TRACE( "done", NULL );
    else
    {
        const QString message( "call to set '" + prop + "' failed: " + reply.error().message() );

        LOG_WARN( "dbus error: '%s'", qPrintable( message ) );
        Q_EMIT busError( message );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qint64 dbusMediaPlayerClient::duration() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<qint64>( "duration" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect dbusMediaPlayerClient::geometry() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<QRect>( "geometry" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbusMediaPlayerClient::isAudioAvailable() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<bool>( "audioAvailable" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbusMediaPlayerClient::isMuted() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<bool>( "muted" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbusMediaPlayerClient::isSeekable() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<bool>( "seekable" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbusMediaPlayerClient::isValid() const
{
    return (( props_ ) && ( props_->isValid() ) && ( object_ ) && ( object_->isValid() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbusMediaPlayerClient::isVideoAvailable() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<bool>( "videoAvailable" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbusMediaPlayerClient::isVisible() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<bool>( "visible" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractMediaPlayer::MediaStatus dbusMediaPlayerClient::mediaStatus() const
{
    const QDBusArgument value( const_cast<dbusMediaPlayerClient*>( this )->getPropValue<QDBusArgument>( "mediaStatus" ) );

    return qdbus_cast<abstractMediaPlayer::MediaStatus>( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractMediaPlayer::MediaType dbusMediaPlayerClient::mediaType() const
{
    const QDBusArgument value( const_cast<dbusMediaPlayerClient*>( this )->getPropValue<QDBusArgument>( "mediaType" ) );

    return qdbus_cast<abstractMediaPlayer::MediaType>( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qint64 dbusMediaPlayerClient::position() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<qint64>( "position" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractMediaPlayer::State dbusMediaPlayerClient::state() const
{
    const QDBusArgument value( const_cast<dbusMediaPlayerClient*>( this )->getPropValue<QDBusArgument>( "state" ) );

    return qdbus_cast<abstractMediaPlayer::State>( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int dbusMediaPlayerClient::volume() const
{
    return const_cast<dbusMediaPlayerClient*>( this )->getPropValue<int>( "volume" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::setGeometry( const QRect &rect )
{
    setPropValue( "geometry", rect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::setMuted( bool muted )
{
    setPropValue( "muted", muted );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::setPosition( qint64 position )
{
    setPropValue( "position", position );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::setVisible( bool visible )
{
    setPropValue( "visible", visible );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::setVolume( int volume )
{
    setPropValue( "volume", volume );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::hide()
{
    call( "hide" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::load( const QString &filename, MediaType type )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "load" );

    LOG_INFO( "loading '%s' of type %d", qPrintable( filename ), type );

    call( "load", QVariant::fromValue( filename ), QVariant::fromValue( type ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::pause()
{
    call( "pause" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::play()
{
    call( "play" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::show()
{
    call( "show" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::stop()
{
    call( "stop" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::call( const QString &method, const QVariant &arg1, const QVariant &arg2 )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "call" );

    // validate interface
    if (( !object_ ) || ( !object_->isValid() ))
    {
        LOG_WARN( "invalid interface", NULL );
        return;
    }

    // invoke
    LOG_TRACE( "invoking '%s'...", qPrintable( method ) );
    QDBusReply<void> reply( object_->call( method, arg1, arg2 ) );

    if ( reply.isValid() )
        LOG_TRACE( "done", NULL );
    else
    {
        const QString message( "call to '" + method + "' failed: " + reply.error().message() );

        LOG_WARN( "dbus error '%s'", qPrintable( message ) );
        Q_EMIT busError( message );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onAudioAvailableChanged( bool available )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onAudioAvailableChanged" );

    LOG_DEBUG( "available %d", available );

    Q_EMIT audioAvailableChanged( available );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onDurationChanged( qint64 duration )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onDurationChanged" );

    LOG_DEBUG( "duration %lld", (long long) duration );

    Q_EMIT durationChanged( duration );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onError( const QString &err )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onError" );

    LOG_ERROR( qPrintable( err ), NULL );

    Q_EMIT error( err );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onFinished()
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onFinished" );

    LOG_INFO( "finished", NULL );

    Q_EMIT finished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onGeometryChanged( const QRect &rect )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onGeometryChanged" );

    LOG_DEBUG( "geometry %d,%d+%d+%d", rect.left(), rect.top(), rect.width(), rect.height() );

    Q_EMIT geometryChanged( rect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onMediaStatusChanged( dbusMediaPlayerClient::MediaStatus status )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onMediaStatusChanged" );

    LOG_DEBUG( "media status %d", status );

    Q_EMIT mediaStatusChanged( status );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onMutedChanged( bool muted )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onMutedChanged" );

    LOG_DEBUG( "muted %d", muted );

    Q_EMIT mutedChanged( muted );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onPositionChanged( qint64 position )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onPositionChanged" );

    LOG_DEBUG( "position %lld", (long long) position );

    Q_EMIT positionChanged( position );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onSeekableChanged( bool seekable )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onSeekableChanged" );

    LOG_DEBUG( "seekable %d", seekable );

    Q_EMIT seekableChanged( seekable );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onStateChanged( dbusMediaPlayerClient::State newState )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onStateChanged" );

    LOG_DEBUG( "state %d", newState );

    Q_EMIT stateChanged( newState );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onVideoAvailableChanged( bool videoAvailable )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onVideoAvailableChanged" );

    LOG_DEBUG( "available %d", videoAvailable );

    Q_EMIT videoAvailableChanged( videoAvailable );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onVisibleChanged( bool visible )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onVisibleChanged" );

    LOG_DEBUG( "visible %d", visible );

    Q_EMIT visibleChanged( visible );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayerClient::onVolumeChanged( int volume )
{
    LOG_CLASS_INFO( "dbusMediaPlayerClient", "onVolumeChanged" );

    LOG_DEBUG( "volume %d", volume );

    Q_EMIT volumeChanged( volume );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // linux
