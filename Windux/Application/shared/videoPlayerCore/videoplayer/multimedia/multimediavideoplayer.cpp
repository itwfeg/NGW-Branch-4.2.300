/**
 * @file multimediavideoplayer.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "multimediavideoplayer.h"
#include "multimediavideoplayerconfig.h"

// Note:
// QtMultimedia Framework is not supported in Qt4
#if ( QT_VERSION >= 0x050000 )

#include <QApplication>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QTime>

typedef multimediaVideoPlayerConfig<PLATFORM> videoPlayerConfig;

///////////////////////////////////////////////////////////////////////////////////////////////////
multimediaVideoPlayer::multimediaVideoPlayer( QWidget *parent ) :
    _Mybase( parent ),
    emittedPlaybackError_( false ),
    currentMediaStatus_( QMediaPlayer::UnknownMediaStatus ),
    currentState_( QMediaPlayer::StoppedState )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
multimediaVideoPlayer::multimediaVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent ) :
    _Mybase( deviceType, parent ),
    emittedPlaybackError_( false ),
    currentMediaStatus_( QMediaPlayer::UnknownMediaStatus ),
    currentState_( QMediaPlayer::StoppedState )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
multimediaVideoPlayer::~multimediaVideoPlayer()
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "~multimediaVideoPlayer" );

    LOG_TRACE( "disconnecting slots...", NULL );
    disconnect();

    LOG_TRACE( "free player...", NULL );
    freePlayer();

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool multimediaVideoPlayer::active() const
{
    // check if we are playing or paused
    if (( QMediaPlayer::PlayingState != source_.state() ) && ( QMediaPlayer::PausedState != source_.state() ))
    {
        // media may be buffering or loading... we are considered active in these states
        return (( QMediaPlayer::BufferingMedia == source_.mediaStatus() ) || ( QMediaPlayer::LoadingMedia == source_.mediaStatus() ));
    }

    if ( source_.position() >= source_.duration() )
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
multimediaVideoPlayer::MediaProperty multimediaVideoPlayer::hasVideo() const
{
    if ( !active() )
        return Unknown;

    return (source_.isVideoAvailable() ? Yes : No);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool multimediaVideoPlayer::load( const QString& filename, bool startPlaying )
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "load" );

    // stop and clear previous source
    checkPlaybackTimer_.stop();

    source_.setMedia( QMediaContent() );

    LOG_INFO( "loading media '%s'...", qPrintable( filename ) );

    // reset state
    playbackTimer_ = 0;
    playbackState_ = WAITING_FOR_NOTHING;

    emittedPlaybackError_ = false;

    mediaSize_ = QSize();

    // set new source
    source_.setMedia( QUrl::fromLocalFile( filename ) );

    // play!
    if ( startPlaying )
    {
        playbackState_ = WAITING_FOR_MEDIA_LOADED;
        checkPlaybackTimer_.start();
    }

    LOG_DEBUG( "done", NULL );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::play()
{
    source_.play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::pause()
{
    // only pause when playing
    if ( QMediaPlayer::PlayingState == source_.state() )
        source_.pause();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::stop()
{
    checkPlaybackTimer_.stop();
    source_.stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::setVolume( float volume )
{
    source_.setVolume( 100 * volume );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::allocPlayer()
{
    ///////////////////////////////////////////////////////////////////////////
    // create video sink

    videoSink_ = new QVideoWidget;

    videoSink_->setAspectRatioMode( Qt::IgnoreAspectRatio );
    videoSink_->setGeometry( QRect( mapToGlobal( QPoint( 0, 0 ) ), size() ) );
    videoSink_->setPalette( palette() );
    videoSink_->setWindowFlags( Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus );
    videoSink_->setAttribute( Qt::WA_ShowWithoutActivating );

    ///////////////////////////////////////////////////////////////////////////
    // source

    connect( &source_, SIGNAL( videoAvailableChanged( bool ) ), SLOT( onVideoAvailableChanged( bool ) ) );
    connect( &source_, SIGNAL( mediaStatusChanged( QMediaPlayer::MediaStatus ) ), SLOT( onMediaStatusChanged( QMediaPlayer::MediaStatus ) ) );
    connect( &source_, SIGNAL( metaDataChanged( QString, QVariant ) ), SLOT( onMetaDataChanged( QString, QVariant ) ) );
    connect( &source_, SIGNAL( stateChanged( QMediaPlayer::State ) ), SLOT( onStateChanged( QMediaPlayer::State ) ) );
    connect( &source_, SIGNAL( error( QMediaPlayer::Error ) ), SLOT( onError( QMediaPlayer::Error ) ) );
    connect( &source_, SIGNAL( positionChanged( qint64 ) ), SIGNAL( tick( qint64 ) ) );
    connect( &source_, SIGNAL( durationChanged( qint64 ) ), SIGNAL( totalTimeChanged( qint64 ) ) );

    // set notify interval for position updates
    source_.setNotifyInterval( 1000 );

    // create paths
    source_.setVideoOutput( videoSink_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::freePlayer()
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "freePlayer" );

    QVideoWidget *nullVideoWidget( NULL );

    checkPlaybackTimer_.stop();

    LOG_DEBUG( "free player...", NULL );
    source_.setMedia( QMediaContent() );
    source_.setVideoOutput( nullVideoWidget );

    if ( videoSink_ )
    {
        LOG_TRACE( "close video sink...", NULL );
        videoSink_->close();

        QVideoWidget *doomed( videoSink_ );
        videoSink_ = NULL;

        doomed->disconnect();

        LOG_TRACE( "delete video sink...", NULL );
        delete doomed;
    }

    LOG_TRACE( "done freeing player", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::closeEvent( QCloseEvent *e )
{
    _Mybase::closeEvent( e );

    if ( videoSink_ )
        videoSink_->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::hideEvent( QHideEvent *e )
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "hideEvent" );

    if ( videoSink_ )
    {
        LOG_DEBUG( "hiding video sink...", NULL );
        videoSink_->setVisible( false );
        videoSink_->hide();
    }

    LOG_DEBUG( "invoke base...", NULL );
    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::moveEvent( QMoveEvent *e )
{
    _Mybase::moveEvent( e );

    // make sure video is centered in parent
    setVideoSinkGeometry();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // make sure video is centered in parent
    setVideoSinkGeometry();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "showEvent" );

    LOG_DEBUG( "invoke base...", NULL );
    _Mybase::showEvent( e );

    if ( videoSink_ )
    {
        // Note:
        // The QtMultimedia video widget has some issues with mfw_v4lsink and window repaints. Here
        // we apply the old Qt4 Phonon code to resolve this issue.
        if ( platformType::EmbeddedLinux == PLATFORM )
        {
            QObjectList objs( videoSink_->children() );
            QWidget *w;

            foreach ( QObject *obj, objs )
                if ( (w = dynamic_cast<QWidget*>( obj )) )
                {
                    w->setAttribute( Qt::WA_NoSystemBackground, true );
                    w->setAttribute( Qt::WA_PaintOnScreen, true );
                }
        }

        LOG_DEBUG( "showing video sink...", NULL );
        videoSink_->setVisible( true );
        videoSink_->show();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool multimediaVideoPlayer::eventFilter( QObject *obj, QEvent *e )
{
    // make sure video is centered in parent
    if ( QEvent::Move == e->type() )
        setVideoSinkGeometry();

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::onCheckPlaybackTimer()
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "onCheckPlaybackTimer" );

    // increment timer
    playbackTimer_ += CHECK_PLAYBACK_TIMER_MS;

    LOG_TRACE( "check playback state %d...", playbackTimer_ );

    // check state
    switch ( playbackState_ )
    {
    case WAITING_FOR_MEDIA_LOADED:

        // media buffered or loaded
        if (( QMediaPlayer::BufferedMedia == source_.mediaStatus() ) || ( QMediaPlayer::LoadedMedia == source_.mediaStatus() ))
        {
            LOG_INFO( "media loaded", NULL );
            playbackState_ = WAITING_FOR_PLAY;

            // play!
            source_.play();
        }
        else
        {
            LOG_TRACE( "waiting for loaded", NULL );
            break;
        }

    case WAITING_FOR_PLAY:

        // media playing
        if ( QMediaPlayer::PlayingState == source_.state() )
        {
            LOG_INFO( "media playing", NULL );
            playbackState_ = WAITING_FOR_NOTHING;
        }
        else
        {
            LOG_TRACE( "waiting for play", NULL );
            break;
        }

    case WAITING_FOR_NOTHING:
    default:

        // nothing more to do here
        checkPlaybackTimer_.stop();

        LOG_TRACE( "checking media size...", NULL );
        checkMediaSize();

        return;
    }

    // check for timeout
    if ( LEAD_TIME_MS <= playbackTimer_ )
    {
        const QString filename( source_.media().canonicalUrl().toLocalFile() );

        // in some cases media will not play but no error is emitted... if this happens
        // lets just emit a generic error...
        if ( !emittedPlaybackError_ )
        {
            QString desc( QString( tr( "Could not load video:\n%1" ) ).arg( filename ) );

            Q_EMIT playbackError( desc );
        }

        LOG_WARN( "media '%s' did not play!", qPrintable( filename ) );

        // nothing more to do here
        checkPlaybackTimer_.stop();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::onError( QMediaPlayer::Error error )
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "onError" );

    // nothing to do
    if ( QMediaPlayer::NoError == error )
        return;

    QString description( source_.errorString() );

    // we shall ignore these errors
    if ( description.contains( "0x80004005" ) )
        LOG_WARN( "ignoring error '%s'", qPrintable( description ) );
    else
    {
        LOG_WARN( "player error '%s': '%s'", qPrintable( toString( error ) ), qPrintable( description ) );

        Q_EMIT playbackError( description );
        emittedPlaybackError_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::onMediaStatusChanged( QMediaPlayer::MediaStatus status )
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "onMediaStatusChanged" );

    LOG_DEBUG( "media status changed %s -> %s", qPrintable( toString( currentMediaStatus_ ) ), qPrintable( toString( status ) ) );
    currentMediaStatus_ = status;

    // playback finished
    if ( QMediaPlayer::EndOfMedia == status )
    {
        LOG_DEBUG( "playback complete", NULL );
        stop();

        Q_EMIT finished();
    }

    // media buffered and/or loaded
    else if (( QMediaPlayer::BufferedMedia == status ) || ( QMediaPlayer::LoadedMedia == status ))
    {
        LOG_TRACE( "checking media size...", NULL );
        checkMediaSize();
    }

    // error state
    else if ( QMediaPlayer::InvalidMedia == status )
    {
        LOG_WARN( "media is invalid!", NULL );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::onMetaDataChanged( const QString& key, const QVariant& value )
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "onMetaDataChanged" );

    // nothing to do
    if ( source_.media().isNull() )
        return;

    QString s( value.toString() );

    if ( s.isNull() )
        return;

    LOG_DEBUG( "media meta data ['%s', '%s']", qPrintable( key ), qPrintable( s ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::onStateChanged( QMediaPlayer::State newState )
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "onStateChanged" );

    LOG_DEBUG( "state changed %s -> %s", qPrintable( toString( currentState_ ) ), qPrintable( toString( newState ) ) );
    currentState_ = newState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::onVideoAvailableChanged( bool hasVideo )
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "onVideoAvailableChanged" );

    LOG_INFO( "media object %s", (hasVideo ? "has video" : "does NOT have video") );
    Q_EMIT hasVideoChanged( hasVideo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::initialize()
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "initialize" );

    // set object name
    setObjectName( "multimediaVideoPlayer" );

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

    // setup playback timer
    checkPlaybackTimer_.setInterval( CHECK_PLAYBACK_TIMER_MS );
    checkPlaybackTimer_.setSingleShot( false );

    connect( &checkPlaybackTimer_, SIGNAL( timeout() ), SLOT( onCheckPlaybackTimer() ) );

    // create player
    allocPlayer();

    // install event filter to top level window
    window()->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::setVideoSinkGeometry()
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "setVideoSinkGeometry" );

    // nothing to do
    if ( !videoSink_ )
        return;

    QPoint p( mapToGlobal( QPoint( 0, 0 ) ) );
    QSize s( mediaSize_ );

    // no media size yet... use widget size
    if (( s.width() <= 0 ) && ( s.height() <= 0 ))
        s = size();

    LOG_DEBUG( "video size is %dx%d", s.width(), s.height() );
    LOG_DEBUG( "viewport size is %dx%d", width(),  height() );

    s.scale( size(), Qt::KeepAspectRatio );

    QRect newGeometry(
        p.x() + ((width() - s.width()) / 2),
        p.y() + ((height() - s.height()) / 2),
        s.width(),
        s.height() );

    LOG_INFO( "video sink geometry is %dx%d+%d+%d",
        newGeometry.width(), newGeometry.height(),
        newGeometry.x(), newGeometry.y() );

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

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaVideoPlayer::checkMediaSize()
{
    LOG_CLASS_INFO( "multimediaVideoPlayer", "checkMediaSize" );

    if ( !videoSink_ )
    {
        LOG_ERROR( "video sink is NULL", NULL );
        return;
    }

    // resize the video widget to match the aspect ratio of the source
    LOG_TRACE( "retrieving size...", NULL );
    QSize mediaSize( videoSink_->sizeHint() );

    if (( 0 < mediaSize.width() ) && ( 0 < mediaSize.height() ))
        if ( mediaSize_ != mediaSize )
        {
            LOG_TRACE( "setting geometry...", NULL );

            mediaSize_ = mediaSize;
            setVideoSinkGeometry();
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString multimediaVideoPlayer::toString( QMediaPlayer::Error error )
{
    QString result;

    switch ( error )
    {
    case QMediaPlayer::NoError:
        result = "None";
        break;
    case QMediaPlayer::ResourceError:
        result = "ResourceError";
        break;
    case QMediaPlayer::FormatError:
        result = "FormatError";
        break;
    case QMediaPlayer::NetworkError:
        result = "NetworkError";
        break;
    case QMediaPlayer::AccessDeniedError:
        result = "AccessDeniedError";
        break;
    case QMediaPlayer::ServiceMissingError:
        result = "ServiceMissingError";
        break;
    default:
        result = "UNKNOWN";
        break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString multimediaVideoPlayer::toString( QMediaPlayer::MediaStatus status )
{
    QString result;

    switch ( status )
    {
    case QMediaPlayer::UnknownMediaStatus:
        result = "UnknownMedia";
        break;
    case QMediaPlayer::NoMedia:
        result = "NoMedia";
        break;
    case QMediaPlayer::LoadingMedia:
        result = "LoadingMedia";
        break;
    case QMediaPlayer::LoadedMedia:
        result = "LoadedMedia";
        break;
    case QMediaPlayer::StalledMedia:
        result = "StalledMedia";
        break;
    case QMediaPlayer::BufferingMedia:
        result = "BufferingMedia";
        break;
    case QMediaPlayer::BufferedMedia:
        result = "BufferedMedia";
        break;
    case QMediaPlayer::EndOfMedia:
        result = "EndOfMedia";
        break;
    case QMediaPlayer::InvalidMedia:
        result = "InvalidMedia";
        break;
    default:
        result = "UNKNOWN";
        break;
    }

    return result;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
QString multimediaVideoPlayer::toString( QMediaPlayer::State state )
{
    QString result;

    switch ( state )
    {
    case QMediaPlayer::StoppedState:
        result = "Stopped";
        break;
    case QMediaPlayer::PlayingState:
        result = "Playing";
        break;
    case QMediaPlayer::PausedState:
        result = "Paused";
        break;
    default:
        result = "UNKNOWN";
        break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // QT_VERSION >= 0x050000
