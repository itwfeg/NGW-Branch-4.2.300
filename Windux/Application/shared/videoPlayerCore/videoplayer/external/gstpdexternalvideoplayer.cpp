/**
 * @file gstpdexternalvideoplayer.cpp
 * Video player based on gstpd (gst player daemon).
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "gstpdappconfig.h"
#include "gstpdexternalvideoplayer.h"

#include <custgstmp.h>
#include <logmanager.h>
#include <opergstmp.h>

#include <QEvent>
#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////
gstpdExternalVideoPlayer::gstpdExternalVideoPlayer( QWidget *parent ) :
    _Mybase( new gstpdAppConfig(), parent ),
    client_( NULL )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstpdExternalVideoPlayer::gstpdExternalVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent ) :
    _Mybase( new gstpdAppConfig(), deviceType, parent ),
    client_( NULL )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstpdExternalVideoPlayer::~gstpdExternalVideoPlayer()
{
    // terminate
    terminate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstpdExternalVideoPlayer::active() const
{
    // validate
    if ( !_Mybase::active() )
        return false;

    // client state
    if ( client_ )
    {
        const abstractMediaPlayer::State state( client_->state() );

        // check if we are playing or paused
        if (( abstractMediaPlayer::PlayingState != state ) && ( abstractMediaPlayer::PausedState != state ))
        {
            const abstractMediaPlayer::MediaStatus mediaStatus( client_->mediaStatus() );

            // media may be buffering or loading... we are considered active in these states
            return (( abstractMediaPlayer::BufferingMedia == mediaStatus ) || ( abstractMediaPlayer::LoadingMedia == mediaStatus ));
        }

        if ( client_->position() >= client_->duration() )
            return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstpdExternalVideoPlayer::MediaProperty gstpdExternalVideoPlayer::hasAudio() const
{
    // validate
    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
        return (client_->isAudioAvailable() ? Yes : No);

    return Unknown;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstpdExternalVideoPlayer::MediaProperty gstpdExternalVideoPlayer::hasVideo() const
{
    // validate
    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
        return (client_->isVideoAvailable() ? Yes : No);

    return Unknown;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::setVolume( float volume )
{
    // validate
    if (( !_Mybase::active() ) || ( !client_ ) || ( !client_->isValid() ))
        return;

    client_->setVolume( (int)(100.0 * volume) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstpdExternalVideoPlayer::load( const QString &filename, bool startPlaying )
{
    LOG_CLASS_INFO( "gstpdExternalVideoPlayer", "load" );

    // start server daemon
    if ( _Mybase::load( filename, false ) )
        _Mybase::play();
    else
    {
        LOG_WARN( "external player failed load!", NULL );

        Q_EMIT playbackError( "Media '" + filename + "' failed to load." );
        return false;
    }

    if ( !_Mybase::active() )
    {
        LOG_WARN( "failed to start daemon", NULL );

        Q_EMIT playbackError( "Player failed to start." );
        return false;
    }

    // create client
    // give process up to DBUS_CONNECTION_TIMEOUT to insantiate daemon
    const int maxAttempts( DBUS_CONNECTION_TIMEOUT / 250 );

    const QString service( (videoPlayerDevice::Customer == device()) ? custgstmp::DBUS_SERVICE_NAME : opergstmp::DBUS_SERVICE_NAME );

    for ( int attempts( 0 ); attempts < maxAttempts; ++attempts )
    {
        QThread::msleep( 250 );

        client_ = new dbusMediaPlayerClient( service, this );

        // Note:
        // This is kind of hackish but there is a timing issue where DBUS reports everything is
        // fine but the interface is not fully available. So here we ask DBUS if everything is okay
        // and we also check an object property. Default volume is 100 so if we get a zero back
        // the interface is not ready.
        if ( client_->isValid() )
            if ( 0 < client_->volume() )
                break;

        delete client_;
        client_ = NULL;
    }

    if ( !client_ )
    {
        LOG_WARN( "failed to create client", NULL );

        Q_EMIT playbackError( "Error connecting to media services." );
        return false;
    }

    // connect slots
    connect( client_, SIGNAL(finished()), SIGNAL(finished()) );
    connect( client_, SIGNAL(audioAvailableChanged(bool)), SIGNAL(hasAudioChanged(bool)) );
    connect( client_, SIGNAL(videoAvailableChanged(bool)), SIGNAL(hasVideoChanged(bool)) );
    connect( client_, SIGNAL(error(QString)), SIGNAL(playbackError(QString)) );
    connect( client_, SIGNAL(positionChanged(qint64)), SIGNAL(tick(qint64)) );
    connect( client_, SIGNAL(durationChanged(qint64)), SIGNAL(totalTimeChanged(qint64)) );

    // set client properties
    client_->setGeometry( QRect( mapToGlobal( QPoint( 0, 0 ) ), size() ) );
    client_->setVisible( isVisible() );

    // load
    client_->load( filename, abstractMediaPlayer::Video );

    // play!
    if ( startPlaying )
        play();

    LOG_DEBUG( "done", NULL );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::pause()
{
    LOG_CLASS_INFO( "gstpdExternalVideoPlayer", "pause" );

    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
    {
        LOG_TRACE( "pausing...", NULL );
        client_->pause();

        LOG_DEBUG( "done", NULL );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::play()
{
    LOG_CLASS_INFO( "gstpdExternalVideoPlayer", "play" );

    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
    {
        LOG_TRACE( "playing...", NULL );
        client_->play();

        LOG_DEBUG( "done", NULL );
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::stop()
{
    LOG_CLASS_INFO( "gstpdExternalVideoPlayer", "stop" );

    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
    {
        LOG_TRACE( "stopping...", NULL );
        client_->stop();

        LOG_DEBUG( "done", NULL );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::hideEvent( QHideEvent *e )
{
    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
        client_->hide();

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::moveEvent( QMoveEvent *e )
{
    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
    {
        const QPoint p( mapToGlobal( QPoint( 0, 0 ) ) );

        client_->setGeometry( QRect( p, size() ) );
    }

    _Mybase::moveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::resizeEvent( QResizeEvent *e )
{
    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
    {
        const QPoint p( mapToGlobal( QPoint( 0, 0 ) ) );

        client_->setGeometry( QRect( p, size() ) );
    }

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::showEvent( QShowEvent *e )
{
    if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
        if ( client_->isVideoAvailable() )
            client_->show();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstpdExternalVideoPlayer::eventFilter( QObject *obj, QEvent *e )
{
    // hook move events so we can adjust geometry
    if ( QEvent::Move == e->type() )
        if (( _Mybase::active() ) && ( client_ ) && ( client_->isValid() ))
        {
            const QPoint p( mapToGlobal( QPoint( 0, 0 ) ) );

            client_->setGeometry( QRect( p, size() ) );
        }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::terminate()
{
    LOG_CLASS_INFO( "gstpdExternalVideoPlayer", "terminate" );

    // destroy existing player
    if ( client_ )
    {
        LOG_DEBUG( "destroying client...", NULL );
        client_->disconnect();

        delete client_;
        client_ = NULL;
    }

    LOG_DEBUG( "destroying client...", NULL );

    // block signals while we terminate process
    process_.blockSignals( true );
    _Mybase::terminate();

    process_.blockSignals( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstpdExternalVideoPlayer::initialize()
{
    // set object name
    setObjectName( "gstpdExternalVideoPlayer" );

    // hook events
    QWidget *w( this );

    while ( (w = w->parentWidget()) )
        w->installEventFilter( this );
}
