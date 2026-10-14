/**
 * @file phononvideoplayer.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "phononvideoplayer.h"
#include "phononvideoplayerconfig.h"

// Note:
// Phonon is not supported in Qt5
#if ( QT_VERSION < 0x050000 )

#include <QApplication>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QTime>

typedef phononVideoPlayerConfig<PLATFORM> videoPlayerConfig;

template <>
void phononVideoPlayer::deleteVideoObjectImpl( Phonon::AudioOutput *ptr )
{
    ptr->disconnect();
}

template <>
void phononVideoPlayer::deleteVideoObjectImpl( Phonon::MediaObject *ptr )
{
    ptr->clear();
    ptr->disconnect();
}

template <>
void phononVideoPlayer::deleteVideoObjectImpl( Phonon::VideoWidget *ptr )
{
    ptr->close();
    ptr->disconnect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
phononVideoPlayer::phononVideoPlayer( QWidget *parent ) :
    _Mybase( parent )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
phononVideoPlayer::phononVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent ) :
    _Mybase( deviceType, parent )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
phononVideoPlayer::~phononVideoPlayer()
{
    LOG_CLASS_INFO( "phononVideoPlayer", "~phononVideoPlayer" );

    LOG_TRACE( "disconnecting slots...", NULL );
    disconnect();

    LOG_TRACE( "free player...", NULL );
    freePlayer();

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool phononVideoPlayer::active() const
{
    if ( !source_ )
        return false;

    if (( Phonon::PlayingState != source_->state() ) && ( Phonon::PausedState != source_->state() ))
        return false;

    if ( source_->currentTime() >= source_->totalTime() )
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
phononVideoPlayer::MediaProperty phononVideoPlayer::hasVideo() const
{
    if ( !active() )
        return Unknown;

    return (source_->hasVideo() ? Yes : No);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool phononVideoPlayer::load( const QString& filename, bool startPlaying )
{
    LOG_CLASS_INFO( "phononVideoPlayer", "load" );

    if ( !source_ )
        return false;

    // stop and clear previous source
    source_->clear();

    LOG_INFO( "loading media '%s'...", qPrintable( filename ) );

    // assume media size until we can get one from phonon
    mediaSize_ = size();

    emittedPlaybackError_ = false;

    // set new source
    source_->setCurrentSource( Phonon::MediaSource( filename ) );

    // play!
    if ( startPlaying )
    {
        LOG_DEBUG( "playing...", NULL );

        // play!
        source_->play();

        LOG_TRACE( "waiting for play state...", NULL );

        // wait for player to start playing...
        QTime done( QTime::currentTime().addMSecs( LEAD_TIME_MS ) );

        do
        {
            if ( Phonon::PlayingState == source_->state() )
            {
                LOG_TRACE( "done waiting", NULL );
                return true;
            }

            QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );

        } while ( QTime::currentTime() < done );

        // in some cases media will not play but no error is emitted... if this happens
        // lets just emit a generic error...
        if ( !emittedPlaybackError_ )
        {
            QString desc( QString( tr( "Could not load video:\n%1" ) ).arg( filename ) );

            Q_EMIT playbackError( desc );
        }

        LOG_WARN( "media '%s' did not play!", qPrintable( filename ) );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::play()
{
    if ( source_ )
        source_->play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::pause()
{
    if ( !source_ )
        return;

    // only pause when playing
    if ( Phonon::PlayingState == source_->state() )
        source_->pause();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::stop()
{
    if ( source_ )
        source_->stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::setVolume( float volume )
{
    if ( audioSink_ )
        audioSink_->setVolume( volume );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::allocPlayer()
{
    LOG_CLASS_INFO( "phononVideoPlayer", "allocPlayer" );

    LOG_INFO( "phonon version is '%s'", Phonon::phononVersion() );

    source_ = NULL;
    videoSink_ = NULL;
    audioSink_ = NULL;

    ///////////////////////////////////////////////////////////////////////////
    // create video sink

    videoSink_ = new Phonon::VideoWidget;

    videoSink_->setAspectRatio( Phonon::VideoWidget::AspectRatioWidget );
    videoSink_->setGeometry( QRect( QPoint( 0, 0 ), size() ) );
    videoSink_->setPalette( palette() );
    videoSink_->setScaleMode( Phonon::VideoWidget::FitInView );
    videoSink_->setWindowFlags( Qt::ToolTip | Qt::FramelessWindowHint );

    ///////////////////////////////////////////////////////////////////////////
    // create audio sink

    audioSink_ = new Phonon::AudioOutput( this );

    QList<Phonon::AudioOutputDevice> audioDevices( Phonon::BackendCapabilities::availableAudioOutputDevices() );
    QString audioDeviceName( videoPlayerConfig::audioDeviceName() );

    // audio device config
    if ( audioDeviceName.length() )
        LOG_INFO( "looking for device '%s'...", qPrintable( audioDeviceName ) );

    // list out audio devices
    foreach ( Phonon::AudioOutputDevice aod, audioDevices )
    {
        LOG_DEBUG( "audio device '%s' ('%s') valid %d",
            qPrintable( aod.name() ),
            qPrintable( aod.description() ),
            aod.isValid() );

        // found device!
        if (( audioDeviceName.length() ) && ( audioDeviceName == aod.name() ))
        {
            LOG_DEBUG( "setting device...", NULL );
            audioSink_->setOutputDevice( aod );
        }
    }

    LOG_INFO( "using audio device '%s'", qPrintable( audioSink_->outputDevice().name() ) );

    ///////////////////////////////////////////////////////////////////////////
    // source

    source_ = new Phonon::MediaObject( this );

    source_->setTickInterval( 1000 );

    // create paths
    Phonon::createPath( source_, videoSink_ );
    Phonon::createPath( source_, audioSink_ );

    connect( source_, SIGNAL( finished() ), SLOT( playbackDone() ) );
    connect( source_, SIGNAL( hasVideoChanged( bool ) ), SLOT( mediaHasVideoChanged( bool ) ) );
    connect( source_, SIGNAL( metaDataChanged() ), SLOT( mediaMetaDataChanged() ) );
    connect( source_, SIGNAL( stateChanged( Phonon::State, Phonon::State ) ), SLOT( mediaStateChanged( Phonon::State, Phonon::State ) ));
    connect( source_, SIGNAL( tick( qint64 ) ), SIGNAL( tick( qint64 ) ) );
    connect( source_, SIGNAL( totalTimeChanged( qint64 ) ), SIGNAL( totalTimeChanged( qint64 ) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::freePlayer()
{
    LOG_CLASS_INFO( "phononVideoPlayer", "freePlayer" );
    QMutexLocker lock( &mutex_ );

    LOG_TRACE( "delete source...", NULL );
    deleteVideoObject( &source_ );

    LOG_TRACE( "delete audio sink...", NULL );
    deleteVideoObject( &audioSink_ );

    LOG_TRACE( "delete video sink...", NULL );
    deleteVideoObject( &videoSink_ );

    LOG_TRACE( "done freeing player", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::closeEvent( QCloseEvent *e )
{
    _Mybase::closeEvent( e );

    if ( videoSink_ )
        videoSink_->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::hideEvent( QHideEvent *e )
{
    _Mybase::hideEvent( e );

    if ( videoSink_ )
        videoSink_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::moveEvent( QMoveEvent *e )
{
    _Mybase::moveEvent( e );

    // make sure video is centered in parent
    setVideoSinkGeometry();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // make sure video is centered in parent
    setVideoSinkGeometry();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

    if ( videoSink_ )
        videoSink_->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool phononVideoPlayer::eventFilter( QObject *obj, QEvent *e )
{
    // make sure video is centered in parent
    if ( QEvent::Move == e->type() )
        setVideoSinkGeometry();

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::playbackDone()
{
    LOG_CLASS_INFO( "phononVideoPlayer", "playbackDone" );

    LOG_DEBUG( "playback complete", NULL );
    stop();

    Q_EMIT finished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::mediaHasVideoChanged( bool hasVideo )
{
    LOG_CLASS_INFO( "phononVideoPlayer", "mediaHasVideoChanged" );

    LOG_INFO( "media object %s", (hasVideo ? "has video" : "does NOT have video") );
    Q_EMIT hasVideoChanged( hasVideo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::mediaMetaDataChanged()
{
    LOG_CLASS_INFO( "phononVideoPlayer", "mediaMetaDataChanged" );

    // there is an issue where we destroy the video sink and state changing gets called on an invalid
    // sink... this mutex prevents that.
    if ( !mutex_.tryLock() )
        return;

    LOG_TRACE( "processing meta data change...", NULL );

    if ( !source_ )
        LOG_ERROR( "source is NULL", NULL );
    else if ( !source_->isValid() )
        LOG_WARN( "invalid source", NULL );
    else
    {
        metaDataMap data( source_->metaData() );

        LOG_DEBUG( "media meta data (%d):", data.size() );

        // log out meta data
        for ( metaDataMap::iterator i( data.begin() ); i != data.end(); ++i )
        {
            QString prop( i.key() );
            QString value( i.value() );

            LOG_DEBUG( "['%s', '%s']", qPrintable( prop ), qPrintable( value ) );
        }
    }

    mutex_.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::mediaStateChanged( Phonon::State newState, Phonon::State oldState )
{
    LOG_CLASS_INFO( "phononVideoPlayer", "mediaStateChanged" );

    LOG_DEBUG( "state changed %s -> %s", qPrintable( toString( oldState ) ), qPrintable( toString( newState ) ) );

    // there is an issue where we destroy the video sink and state changing gets called on an invalid
    // sink... this mutex prevents that.
    if ( !mutex_.tryLock() )
        return;

    // log out the error state
    if ( Phonon::ErrorState == newState )
    {
        if ( !source_ )
            LOG_ERROR( "source is NULL", NULL );

        // process error
        else
        {
            QString description( source_->errorString() );

            // we shall ignore these errors
            if ( description.contains( "0x80004005" ) )
            {
                LOG_WARN( "ignoring error '%s'", qPrintable( description ) );
            }
            else
            {
                if ( Phonon::NormalError == source_->errorType() )
                    LOG_WARN( "player normal error '%s'", qPrintable( description ) );
                else if ( Phonon::FatalError == source_->errorType() )
                    LOG_ERROR( "player fatal error '%s'", qPrintable( description ) );
                else
                {
                    LOG_DEBUG( "player error '%s'", qPrintable( description ) );
                }

                Q_EMIT playbackError( description );
                emittedPlaybackError_ = true;
            }
        }
    }

    // handle media related stuff
    else if ( Phonon::LoadingState == oldState )
    {
        if ( !videoSink_ )
            LOG_ERROR( "video sink is NULL", NULL );

        // resize the video widget to match the aspect ratio of the source
        else
        {
            LOG_TRACE( "retrieving size...", NULL );
            QSize mediaSize( videoSink_->sizeHint() );

            if ( mediaSize.isValid() )
            {
                LOG_TRACE( "processing events...", NULL );

                // flush event que so that sizeHint takes the recently shown/hidden videoSink_ into account
                qApp->processEvents();

                LOG_TRACE( "setting geometry...", NULL );

                mediaSize_ = mediaSize;
                setVideoSinkGeometry();
            }
        }
    }

    LOG_TRACE( "done processing state change", NULL );
    mutex_.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::initialize()
{
    LOG_CLASS_INFO( "phononVideoPlayer", "initialize" );

    // set object name
    setObjectName( "phononVideoPlayer" );

    // set environment
    QStringList list( videoPlayerConfig::environment( device() ) );

    foreach( QString env, list )
    {
        LOG_INFO( "setting environment '%s'", qPrintable( env ) );

        int index( env.indexOf( "=" ) );

        if ( 0 < index )
        {
            char s[1024];
            char v[1024];

            // yay, c code!!!
            strcpy( s, qPrintable( env.left( index ) ) );
            strcpy( v, qPrintable( env.right( env.length() - index - 1 ) ) );

#if defined( Q_OS_WIN )
            qputenv( s, v );
#else
            setenv( s, v, 1 );
#endif
        }
    }

    // create player
    allocPlayer();

    // install event filter to top level window
    window()->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void phononVideoPlayer::setVideoSinkGeometry()
{
    LOG_CLASS_INFO( "phononVideoPlayer", "setVideoSinkGeometry" );

    if ( videoSink_ )
    {
        QPoint p( mapToGlobal( QPoint( 0, 0 ) ) );

        LOG_DEBUG( "video size is (%d, %d)", mediaSize_.width(), mediaSize_.height() );
        LOG_DEBUG( "viewport size is (%d, %d)", width(),  height() );

        mediaSize_.scale( size(), Qt::KeepAspectRatio );

        QRect newGeometry(
            p.x() + ((width() - mediaSize_.width()) / 2),
            p.y() + ((height() - mediaSize_.height()) / 2),
            mediaSize_.width(),
            mediaSize_.height() );

        LOG_INFO( "video sink geometry is [%d, %d] (%d, %d)",
            newGeometry.x(), newGeometry.y(), newGeometry.width(), newGeometry.height() );

        // Note:
        // The sizeHint() function is supposed to return the actual video dimensions of
        // the source... however in practice I am only seeing this sometimes. In the bad
        // cases it seems to latch onto a size then stick with it for each call forever
        // after. Ideally we would have some way to query the video dimensions outside of
        // phonon but I don't see anything to do this.  I am hoping this is just some sort
        // of quirk with gstreamer backend that will be resolved in short order. The
        // windows backend seems okay...
        videoSink_->setGeometry( newGeometry );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString phononVideoPlayer::toString( Phonon::State state )
{
    QString result;

    switch ( state )
    {
    case Phonon::LoadingState:
        result = "Loading";
        break;
    case Phonon::StoppedState:
        result = "Stopped";
        break;
    case Phonon::PlayingState:
        result = "Playing";
        break;
    case Phonon::BufferingState:
        result = "Buffering";
        break;
    case Phonon::PausedState:
        result = "Paused";
        break;
    case Phonon::ErrorState:
        result = "Error";
        break;
    default:
        result = "UNKNOWN";
        break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // QT_VERSION < 0x050000
