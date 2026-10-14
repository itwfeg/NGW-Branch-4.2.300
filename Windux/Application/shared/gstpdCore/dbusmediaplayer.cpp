/**
 * @file dbusmediaplayer.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <QtGlobal>

// Linux only!
#ifndef Q_OS_WIN

#include "dbusmediaplayer.h"

#include <logmanager.h>

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>

const QString dbusMediaPlayer::DBUS_OBJECT_INTERFACE( "local.gstpd.abstractMediaPlayer" );
const QString dbusMediaPlayer::DBUS_OBJECT_PATH( "/mediaPlayer" );

///////////////////////////////////////////////////////////////////////////////////////////////////
dbusMediaPlayer::dbusMediaPlayer( QObject *parent ) :
    _Mybase( parent )
{
    initialize( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dbusMediaPlayer::dbusMediaPlayer( dbus_client_t, QObject *parent ) :
    _Mybase( parent )
{
    initialize( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dbusMediaPlayer::~dbusMediaPlayer()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::emitSignal( const QString &name )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "emitSignal" );

    QDBusConnection dbus( QDBusConnection::sessionBus() );

    // generate signal
    QDBusMessage msg( QDBusMessage::createSignal( dbusObjectPath(), dbusObjectInterface(), name ) );

    // emit!
    LOG_TRACE( "emitting signal %s...", qPrintable( name ) );

    if ( dbus.send( msg ) )
        LOG_TRACE( "done", NULL );
    else
    {
        LOG_ERROR( "failed to emit signal '%s': %s", qPrintable( name ), qPrintable( dbus.lastError().message() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void dbusMediaPlayer::emitSignal( const QString &name, const T &value )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "emitSignal" );

    QDBusConnection dbus( QDBusConnection::sessionBus() );

    // generate signal
    QDBusMessage msg( QDBusMessage::createSignal( dbusObjectPath(), dbusObjectInterface(), name ) );
    msg << QVariant::fromValue( value );

    // emit!
    LOG_TRACE( "emitting signal %s...", qPrintable( name ) );

    if ( dbus.send( msg ) )
        LOG_TRACE( "done", NULL );
    else
    {
        LOG_ERROR( "failed to emit signal '%s': %s", qPrintable( name ), qPrintable( dbus.lastError().message() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onAudioAvailableChanged( bool available )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onAudioAvailableChanged" );

    LOG_DEBUG( "available %d", available );

    emitSignal( "audioAvailableChanged", available );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onDurationChanged( qint64 duration )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onDurationChanged" );

    LOG_DEBUG( "duration %lld", (long long) duration );

    emitSignal( "durationChanged", duration );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onError( const QString &err )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onError" );

    LOG_ERROR( qPrintable( err ), NULL );

    emitSignal( "error", err );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onFinished()
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onFinished" );

    LOG_INFO( "finished", NULL );

    emitSignal( "finished" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onGeometryChanged( const QRect &rect )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onGeometryChanged" );

    LOG_DEBUG( "geometry %d,%d+%d+%d", rect.left(), rect.top(), rect.width(), rect.height() );

    emitSignal( "geometryChanged", rect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onMediaStatusChanged( abstractMediaPlayer::MediaStatus status )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onMediaStatusChanged" );

    LOG_DEBUG( "media status %d", status );

    emitSignal( "mediaStatusChanged", status );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onMutedChanged( bool muted )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onMutedChanged" );

    LOG_DEBUG( "muted %d", muted );

    emitSignal( "mutedChanged", muted );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onPositionChanged( qint64 position )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onPositionChanged" );

    LOG_DEBUG( "position %lld", (long long) position );

    emitSignal( "positionChanged", position );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onSeekableChanged( bool seekable )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onSeekableChanged" );

    LOG_DEBUG( "seekable %d", seekable );

    emitSignal( "seekableChanged", seekable );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onStateChanged( abstractMediaPlayer::State newState )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onStateChanged" );

    LOG_DEBUG( "state %d", newState );

    emitSignal( "stateChanged", newState );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onVisibleChanged( bool visible )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onVisibleChanged" );

    LOG_DEBUG( "visible %d", visible );

    emitSignal( "visibleChanged", visible );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onVideoAvailableChanged( bool videoAvailable )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onVideoAvailableChanged" );

    LOG_DEBUG( "available %d", videoAvailable );

    emitSignal( "videoAvailableChanged", videoAvailable );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::onVolumeChanged( int volume )
{
    LOG_CLASS_INFO( "dbusMediaPlayer", "onVolumeChanged" );

    LOG_DEBUG( "volume %d", volume );

    emitSignal( "volumeChanged", volume );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbusMediaPlayer::initialize( bool isClient )
{
    // register meta types within DBUS
    qDBusRegisterMetaType<abstractMediaPlayer::State>();
    qDBusRegisterMetaType<abstractMediaPlayer::MediaStatus>();
    qDBusRegisterMetaType<abstractMediaPlayer::MediaType>();

    if ( isClient )
        return;

    // connect signals
    connect( this, SIGNAL(audioAvailableChanged(bool)), SLOT(onAudioAvailableChanged(bool)) );
    connect( this, SIGNAL(durationChanged(qint64)), SLOT(onDurationChanged(qint64)) );
    connect( this, SIGNAL(error(QString)), SLOT(onError(QString)) );
    connect( this, SIGNAL(finished()), SLOT(onFinished()) );
    connect( this, SIGNAL(geometryChanged(QRect)), SLOT(onGeometryChanged(QRect)) );
    connect( this, SIGNAL(mediaStatusChanged(abstractMediaPlayer::MediaStatus)), SLOT(onMediaStatusChanged(abstractMediaPlayer::MediaStatus)) );
    connect( this, SIGNAL(mutedChanged(bool)), SLOT(onMutedChanged(bool)) );
    connect( this, SIGNAL(positionChanged(qint64)), SLOT(onPositionChanged(qint64)) );
    connect( this, SIGNAL(seekableChanged(bool)), SLOT(onSeekableChanged(bool)) );
    connect( this, SIGNAL(stateChanged(abstractMediaPlayer::State)), SLOT(onStateChanged(abstractMediaPlayer::State)) );
    connect( this, SIGNAL(videoAvailableChanged(bool)), SLOT(onVideoAvailableChanged(bool)) );
    connect( this, SIGNAL(visibleChanged(bool)), SLOT(onVisibleChanged(bool)) );
    connect( this, SIGNAL(volumeChanged(int)), SLOT(onVolumeChanged(int)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDBusArgument& operator << ( QDBusArgument &lhs, const abstractMediaPlayer::State &rhs )
{
    const int newVal( (int) rhs );

    lhs.beginStructure();
    lhs << newVal;
    lhs.endStructure();

    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const QDBusArgument& operator >> ( const QDBusArgument &lhs, abstractMediaPlayer::State &rhs )
{
    int result( 0 );

    lhs.beginStructure();
    lhs >> result;
    lhs.endStructure();

    rhs = (abstractMediaPlayer::State) result;

    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDBusArgument& operator << ( QDBusArgument &lhs, const abstractMediaPlayer::MediaStatus &rhs )
{
    const int newVal( (int) rhs );

    lhs.beginStructure();
    lhs << newVal;
    lhs.endStructure();

    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const QDBusArgument& operator >> ( const QDBusArgument &lhs, abstractMediaPlayer::MediaStatus &rhs )
{
    int result( 0 );

    lhs.beginStructure();
    lhs >> result;
    lhs.endStructure();

    rhs = (abstractMediaPlayer::MediaStatus) result;

    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDBusArgument& operator << ( QDBusArgument &lhs, const abstractMediaPlayer::MediaType &rhs )
{
    const int newVal( (int) rhs );

    lhs.beginStructure();
    lhs << newVal;
    lhs.endStructure();

    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const QDBusArgument& operator >> ( const QDBusArgument &lhs, abstractMediaPlayer::MediaType &rhs )
{
    int result( 0 );

    lhs.beginStructure();
    lhs >> result;
    lhs.endStructure();

    rhs = (abstractMediaPlayer::MediaType) result;

    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // linux
