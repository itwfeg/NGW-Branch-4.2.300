/**
 * @file gstmp.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <QtGlobal>

// Linux only!
#ifndef Q_OS_WIN

#include "gstmp.h"

#include <logmanager.h>

#include <gst/gst.h>

#include <QCoreApplication>
#include <QTime>

#if defined( ARM_MXC_FREESCALE )
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>

#include <linux/mxcfb.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
static gboolean gst_decodebin_autoplug_continue( GstElement *object, GstPad *pad, GstCaps *caps, gpointer user_data )
{
    LOG_CLASS_INFO( "", "gst_decodebin_autoplug_continue" );

    Q_UNUSED( object );
    Q_UNUSED( pad );

    // convert caps to string
    gchar *s( gst_caps_to_string( caps ) );

    if ( s )
    {
        const QString desc( s );

        gstmp *ptr( static_cast<gstmp*>( user_data ) );

        // audio stream
        if ( desc.contains( "audio/" ) )
        {
            LOG_DEBUG( "found audio stream '%s'", s );
            ptr->queueAudioStream();
        }
        // video stream
        else if ( desc.contains( "video/" ) )
        {
            LOG_DEBUG( "found video stream '%s'", s );
            ptr->queueVideoStream();
        }

        g_free( s );
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void gst_decodebin_no_more_pads( GstElement *object, gpointer user_data )
{
    LOG_CLASS_INFO( "", "gst_decodebin_no_more_pads" );

    Q_UNUSED( object );

    gstmp *ptr( static_cast<gstmp*>( user_data ) );

    LOG_DEBUG( "removing unlinked elements...", NULL );
    ptr->removeUnlinkedElements();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void gst_decodebin_pad_added( GstElement *object, GstPad *pad, gpointer user_data )
{
    LOG_CLASS_INFO( "", "gst_decodebin_pad_added" );

    Q_UNUSED( object );

    // retrieve caps
    GstCaps *caps( gst_pad_get_current_caps( pad ) );

    if ( caps )
    {
        // convert caps to string
        gchar *s( gst_caps_to_string( caps ) );

        if ( s )
        {
            const QString desc( s );

            gstmp *ptr( static_cast<gstmp*>( user_data ) );

            // audio stream
            if ( desc.contains( "audio/" ) )
            {
                LOG_DEBUG( "have audio pad", NULL );
                ptr->linkAudioPad( pad );
            }
            // video stream
            else if ( desc.contains( "video/" ) )
            {
                LOG_DEBUG( "have video pad", NULL );
                ptr->linkVideoPad( pad );
            }

            g_free( s );
        }

        gst_caps_unref( caps );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void gst_decodebin_unknown_type( GstElement *object, GstPad *pad, GstCaps *caps, gpointer user_data )
{
    LOG_CLASS_INFO( "", "gst_decodebin_unknown_type" );

    Q_UNUSED( object );
    Q_UNUSED( pad );

    // convert caps to string
    gchar *s( gst_caps_to_string( caps ) );

    if ( s )
    {
        QString desc( s );

        // caps string may contain a lot of information that is not important... remove it
        int i( desc.indexOf( ',' ) );

        if ( 0 < i )
            desc.truncate( i );

        // queue the decoder error
        gstmp *ptr( static_cast<gstmp*>( user_data ) );

        LOG_DEBUG( "stream '%s' is not supported", s );
        ptr->queueDecoderError( QString( "Media format '%0' is not supported." ).arg( desc ) );

        g_free( s );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstmp::gstmp( QObject *parent ) :
    _Mybase( parent ),
    pipeline_( NULL ),
    audioSinkBin_( NULL ),
    audioPad_( NULL ),
    videoSinkBin_( NULL ),
    videoPad_( NULL ),
    bus_( NULL ),
    source_( NULL ),
    sound_( NULL ),
    videoSink_( NULL ),
    volume_( 1.0 ),
    muted_( false ),
    visible_( true ),
    processBusEventsTimer_( this )
{
    initialize();
    reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstmp::~gstmp()
{
    finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::setGeometry( const QRect &rect )
{
    LOG_CLASS_INFO( "gstmp", "setGeometry" );

    // nothing to do
    if ( geometry_ == rect )
        return;

    // set geometry
    if (( videoSink_ ) && ( !setVideoSinkGeometry( rect ) ))
         LOG_WARN( "failed to set geometry!", NULL );
    else
    {
        geometry_ = rect;

        LOG_DEBUG( "geometry changed to %d,%d+%d+%d", geometry_.left(), geometry_.top(), geometry_.width(), geometry_.height() );
        Q_EMIT geometryChanged( geometry_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::setMuted( bool muted )
{    
    LOG_CLASS_INFO( "gstmp", "setMuted" );

    // nothing to do
    if ( muted_ == muted )
        return;

    // set muted
    if (( sound_ ) && ( !setSoundMuted( muted ) ))
        LOG_WARN( "failed to set muted!", NULL );
    else
    {
        muted_ = muted;

        LOG_INFO( "muted changed to %d", muted_ );
        Q_EMIT mutedChanged( muted_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::setPosition( qint64 position )
{
    LOG_CLASS_INFO( "gstmp", "setPosition" );

    if ( pipeline_ )
        if (( isSeekable() ) && ( duration() ) && ( 0 <= position ) && ( position <= duration() ))
        {
            static const GstSeekFlags flags( (GstSeekFlags)( GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT ) );

            // ignore seeks while stopped
            if ( StoppedState == requestedState_ )
                LOG_DEBUG( "ignoring seek while stopped", NULL );

            // seek!
            else if ( !gst_element_seek_simple( pipeline_, GST_FORMAT_TIME, flags, position * GST_MSECOND ) )
                LOG_WARN( "seek failed!", NULL );
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::setVisible( bool visible )
{
    LOG_CLASS_INFO( "gstmp", "setVisible" );

    // nothing to do
    if ( visible_ == visible )
        return;

#if defined( ARM_MXC_FREESCALE )

    // open framebuffer
    QString fb_dev( fb() );
    int fb_fd;

    if (( fb_dev.length() ) && ( 0 <= (fb_fd = open( qPrintable( fb_dev ), O_RDWR, 0 )) ))
    {
        // we need to use freescale ioctl to show/hide overlay
        struct mxcfb_gbl_alpha gbl_alpha;
        gbl_alpha.enable = (visible ? 1 : 0);
        gbl_alpha.alpha = (visible ? 0 : 255);

        // set overlay alpha
        int rc( ioctl( fb_fd, MXCFB_SET_GBL_ALPHA, &gbl_alpha ) );

        close( fb_fd );

        // check rc
        if ( rc < 0 )
        {
            LOG_WARN( "mxcfb gbl alpha failed: %d", rc );
            return;
        }
    }
#endif

    visible_ = visible;

    LOG_INFO( "visible changed to %d", visible_ );
    Q_EMIT visibleChanged( visible_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::setVolume( int int_volume )
{
    LOG_CLASS_INFO( "gstmp", "setVolume" );

    // convert to double [0.0, 1.0]
    qreal volume( (qreal) int_volume / (qreal) 100.0 );

    if ( volume < 0.0 )
        volume = 0.0;
    else if ( 1.0 < volume )
        volume = 1.0;

    // nothing to do
    if ( volume_ == volume )
        return;

    // set volume
    if (( sound_ ) && ( !setSoundVolume( volume ) ))
        LOG_WARN( "failed to set volume!", NULL );
    else
    {
        volume_ = volume;

        LOG_INFO( "volume changed to %f", volume_ );
        Q_EMIT volumeChanged( (int)(volume_ * 100.0) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::hide()
{
    setVisible( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::linkAudioPad( GstPad *pad )
{
    LOG_CLASS_INFO( "gstmp", "linkAudioPad" );

    QMutexLocker guard( &m_ );

    // make sure we have a pad
    if ( !audioPad_ )
        return;

    // check if already linked
    else if ( gst_pad_is_linked( audioPad_ ) )
    {
        LOG_DEBUG( "already linked", NULL );
        audioPadLinked_ = true;
        return;
    }

    // link!
    GstPadLinkReturn rc( gst_pad_link( pad, audioPad_ ) );

    if ( GST_PAD_LINK_FAILED( rc ) )
        LOG_WARN( "link failed %d", rc );
    else
    {
        audioPadLinked_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::linkVideoPad( GstPad *pad )
{
    LOG_CLASS_INFO( "gstmp", "linkVideoPad" );

    QMutexLocker guard( &m_ );

    // make sure we have a pad
    if ( !videoPad_ )
        return;

    // check if already linked
    else if ( gst_pad_is_linked( videoPad_ ) )
    {
        LOG_DEBUG( "already linked", NULL );
        videoPadLinked_ = true;
        return;
    }

    // link!
    GstPadLinkReturn rc( gst_pad_link( pad, videoPad_ ) );

    if ( GST_PAD_LINK_FAILED( rc ) )
        LOG_WARN( "link failed %d", rc );
    else
    {
        videoPadLinked_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::load( const QString &filename, abstractMediaPlayer::MediaType type )
{
    LOG_CLASS_INFO( "gstmp", "load" );

    // sanity check
    if ( filename.isEmpty() )
        return;
    else if (( Audio != type ) && ( Video != type ))
        return;

    // reset object
    finalize();
    reset();

    updateMediaStatus( NoMedia );

    // create pipeline
    if (( !createPipeline() ) || ( !createAudioSink() ) || ( !createVideoSink() ))
    {
        LOG_WARN( "could not install pipeline!", NULL );
        return;
    }

    // validate
    else if ( !source_ )
    {
        LOG_WARN( "source object missing!", NULL );
        return;
    }

    // set geometry
    // two cases at this point:
    // 1) we have been given a geometry to set
    //    okay so set the geometry
    // 2) no geometry has been given to us
    //    okay so lets read current geometry
    if ( geometry_.isValid() )
        setVideoSinkGeometry( geometry_ );
    else
        geometry_ = videoSinkGeometry();

    // set muted
    setSoundMuted( muted_ );

    // set volume
    setSoundVolume( volume_ );

    // load new media
    media_ = filename;
    mediaType_ = type;

    LOG_INFO( "loading '%s' of type %d", qPrintable( filename ), type );
    updateMediaStatus( LoadingMedia );

    g_object_set( source_, "location", qPrintable( filename ), NULL );

    // preroll pipeline
    LOG_TRACE( "prerolling pipeline...", NULL );
    int rc( gst_element_set_state( pipeline_, GST_STATE_PAUSED ) );

    switch ( rc )
    {
    case GST_STATE_CHANGE_SUCCESS:
        LOG_DEBUG( "pipeline ready", NULL );
        break;

    case GST_STATE_CHANGE_FAILURE:
        LOG_WARN( "failed to preroll pipeline!", NULL );
        return;

    case GST_STATE_CHANGE_NO_PREROLL:
        updateMediaStatus( LoadedMedia );
        live_ = true;
        break;

    default:
        LOG_DEBUG( "prerolling pipeline complete %d", rc );
        break;
    }

    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::pause()
{
    LOG_CLASS_INFO( "gstmp", "pause" );

    if ( pipeline_ )
    {
        GstStateChangeReturn rc;

        if ( GST_STATE_CHANGE_FAILURE == (rc = gst_element_set_state( pipeline_, GST_STATE_PAUSED )) )
            LOG_WARN( "failed to pause pipeline!", NULL );
        else
        {
            LOG_DEBUG( "set state complete %d", rc );
            requestedState_ = PausedState;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::play()
{
    LOG_CLASS_INFO( "gstmp", "play" );

    if ( pipeline_ )
    {
        GstStateChangeReturn rc;

        if ( GST_STATE_CHANGE_FAILURE == (rc = gst_element_set_state( pipeline_, GST_STATE_PLAYING )) )
            LOG_WARN( "failed to play pipeline!", NULL );
        else
        {
            LOG_DEBUG( "set state complete %d", rc );
            requestedState_ = PlayingState;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::queueDecoderError( const QString &error )
{
    QMutexLocker guard( &m_ );

    // append error to queue
    decoderErrorQueued_.append( error );

    // we are no longer loading
    updateMediaStatus( InvalidMedia );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::removeUnlinkedElements()
{
    LOG_CLASS_INFO( "gstmp", "removeUnlinkedElements" );

    QMutexLocker guard( &m_ );

    if ( !audioPadLinked_ )
    {
        // dont need sound control
        sound_ = NULL;

        LOG_DEBUG( "removing audio bin", NULL );
        gst_bin_remove( GST_BIN(pipeline_), audioSinkBin_ );
    }

    if ( !videoPadLinked_ )
    {
        // dont need video control
        videoSink_ = NULL;

        LOG_DEBUG( "removing video bin", NULL );
        gst_bin_remove( GST_BIN(pipeline_), videoSinkBin_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::show()
{
    setVisible( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::stop()
{
    LOG_CLASS_INFO( "gstmp", "stop" );

    if ( pipeline_ )
    {
        static const GstSeekFlags flags( (GstSeekFlags)( GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT ) );

        GstStateChangeReturn rc;

        // pause video and seek to beginning
        if ( GST_STATE_CHANGE_FAILURE == (rc = gst_element_set_state( pipeline_, GST_STATE_PAUSED )) )
            LOG_WARN( "failed to pause pipeline!", NULL );
        else if ( !gst_element_seek_simple( pipeline_, GST_FORMAT_TIME, flags, 0 ) )
            LOG_WARN( "seek failed!", NULL );
        else
        {
            LOG_DEBUG( "set state complete %d", rc );
            requestedState_ = StoppedState;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstmp::isSoundMuted() const
{
    bool result( muted_ );

    if ( sound_ )
    {
        gboolean value;

        // retrieve volume
        g_object_get( sound_, "mute", &value, NULL );

        result = (value ? true : false);
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstmp::setSoundMuted( bool value )
{
    if ( !sound_ )
        return false;

    // set volume
    g_object_set( sound_, "mute", value ? TRUE : FALSE, NULL );

    return (value == isSoundMuted());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstmp::setVideoSinkGeometry( const QRect &value )
{
    LOG_CLASS_INFO( "gstmp", "setVideoSinkGeometry" );

    if ( !videoSink_ )
        return false;

    // retrieve factory
    GstElementFactory *factory;

    if ( !(factory = gst_element_get_factory( videoSink_ )) )
        return false;

    // handle video sink
    static const QString imxv4l2sink( "imxv4l2sink" );

    // freescale
    if ( imxv4l2sink == GST_OBJECT_NAME( factory ) )
    {
        LOG_TRACE( "set freescale geometry %d,%d+%d+%d...", value.left(), value.top(), value.width(), value.height() );

        // set geometry
        g_object_set( G_OBJECT( videoSink_ ),
            "overlay-left", value.left(),
            "overlay-top", value.top(),
            "overlay-width", value.width(),
            "overlay-height", value.height(),
            NULL );

        // update!
        g_object_set( G_OBJECT( videoSink_ ),
            "reconfig", 1,
            NULL );

        return (value == videoSinkGeometry());
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstmp::setSoundVolume( qreal value )
{
    static const qreal toler( 0.05 );

    if ( !sound_ )
        return false;

    // set volume
    g_object_set( sound_, "volume", (gdouble) value, NULL );

    // retrieve volume
    qreal polledValue( soundVolume() );

    // allow error tolerance
    return (( (value - toler) <= polledValue ) && ( polledValue <= (value + toler) ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qreal gstmp::soundVolume() const
{
    LOG_CLASS_INFO( "gstmp", "soundVolume" );

    qreal result( volume_ );

    if ( sound_ )
    {
        gdouble value;

        LOG_TRACE( "retrieve volume...", NULL );

        // retrieve volume
        g_object_get( sound_, "volume", &value, NULL );

        result = value;
    }

    LOG_DEBUG( "volume %f", result );
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect gstmp::videoSinkGeometry() const
{
    LOG_CLASS_INFO( "gstmp", "videoSinkGeometry" );

    QRect result( geometry_ );

    if ( videoSink_ )
    {
        // retrieve factory
        GstElementFactory *factory;

        if ( (factory = gst_element_get_factory( videoSink_ )) )
        {
            // handle video sink
            static const QString imxv4l2sink( "imxv4l2sink" );

            // freescale
            if ( imxv4l2sink == GST_OBJECT_NAME( factory ) )
            {
                gint overlay_left;
                gint overlay_top;
                gint overlay_width;
                gint overlay_height;

                LOG_TRACE( "retrieve freescale geometry...", NULL );

                // retrieve geometry
                g_object_get( G_OBJECT( videoSink_ ),
                    "overlay-left", &overlay_left,
                    "overlay-top", &overlay_top,
                    "overlay-width", &overlay_width,
                    "overlay-height", &overlay_height,
                    NULL );

                result = QRect( QPoint( overlay_left, overlay_top ), QSize( overlay_width, overlay_height ) );
            }
        }
    }

    LOG_DEBUG( "geometry %d,%d+%d+%d", result.left(), result.top(), result.width(), result.height() );
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::reset()
{
    QMutexLocker guard( &m_ );

    // reset state
    state_ = requestedState_ = StoppedState;

    media_.clear();
    mediaType_ = UnknownMediaType;
    mediaStatus_ = UnknownMediaStatus;

    duration_ = position_ = 0;

    audioStreams_ = videoStreams_ = 0;

    buffering_ = live_ = seekable_ = false;

    // reset internal variables
    internalState_ = GST_STATE_NULL;

    audioStreamsQueued_ = videoStreamsQueued_ = 0;
    audioPadLinked_ = videoPadLinked_ = false;

    decoderErrorQueued_.clear();

    updatePositionStamp_ = QTime::currentTime();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::updateDuration( qint64 value )
{
    if ( duration_ == value )
        return;

    Q_EMIT durationChanged( duration_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::updateMediaStatus( MediaStatus value )
{
    if ( mediaStatus_ == value )
        return;

    Q_EMIT mediaStatusChanged( mediaStatus_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::updatePosition( qint64 value )
{
    if ( position_ == value )
        return;

    // set position
    position_ = value;

    // only emit position signals every 1000 msec
    const QTime stamp( QTime::currentTime() );

    if ( updatePositionStamp_ <= stamp.addMSecs( -EMIT_POSITION_TIMER ) )
    {
        Q_EMIT positionChanged( position_ );
        updatePositionStamp_ = stamp;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::updateSeekable( bool value )
{
    if ( seekable_ == value )
        return;

    Q_EMIT seekableChanged( seekable_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::updateState( State value )
{
    if ( state_ == value )
        return;

    Q_EMIT stateChanged( state_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::processBusEvents()
{
    LOG_CLASS_INFO( "gstmp", "processBusEvents" );

    if ( !bus_ )
        return;

    bool queryDuration( false );

    // process all messages on bus
    for ( unsigned int maxEvents( 16 ); maxEvents--; )
    {
        static const GstMessageType filter( (GstMessageType)( GST_MESSAGE_ASYNC_START | GST_MESSAGE_ASYNC_DONE | GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_WARNING | GST_MESSAGE_INFO | GST_MESSAGE_EOS | GST_MESSAGE_DURATION | GST_MESSAGE_BUFFERING ) );

        // pop message
        GstMessage *msg( gst_bus_pop_filtered( bus_, filter ) );

        // no message, break loop
        if ( !msg )
            break;

        // handle message
        switch ( GST_MESSAGE_TYPE( msg ) )
        {
        case GST_MESSAGE_ERROR:
            {
                GError *err;
                gchar *debug_info;

                // parse message
                gst_message_parse_error( msg, &err, &debug_info );
                LOG_ERROR( "error received from element '%s': %s", GST_OBJECT_NAME( msg->src ), err->message );
                LOG_DEBUG( "debugging information: %s", debug_info ? debug_info : "none" );

                Q_EMIT error( QString( "Playback error: %1" ).arg( err->message ) );
                g_clear_error( &err );
                g_free( debug_info );
            }

            break;

        case GST_MESSAGE_WARNING:
            {
                GError *err;
                gchar *debug_info;

                // parse message
                gst_message_parse_warning( msg, &err, &debug_info );
                LOG_WARN( "warning received from element '%s': %s", GST_OBJECT_NAME( msg->src ), err->message );
                LOG_DEBUG( "debugging information: %s", debug_info ? debug_info : "none" );

                g_clear_error( &err );
                g_free( debug_info );
            }

            break;

        case GST_MESSAGE_INFO:
            {
                GError *err;
                gchar *debug_info;

                // parse message
                gst_message_parse_info( msg, &err, &debug_info );
                LOG_WARN( "info received from element '%s': %s", GST_OBJECT_NAME( msg->src ), err->message );
                LOG_DEBUG( "debugging information: %s", debug_info ? debug_info : "none" );

                g_clear_error( &err );
                g_free( debug_info );
            }

            break;

        case GST_MESSAGE_EOS:
            LOG_DEBUG( "stream finished", NULL );
            Q_EMIT finished();

            stop();
            break;

        case GST_MESSAGE_DURATION:
            queryDuration = true;
            break;

        case GST_MESSAGE_BUFFERING:

            // no state management needed for live pipelines
            if ( live_ )
            {
                gint percent;

                // parse message
                gst_message_parse_buffering( msg, &percent );
                LOG_DEBUG( "media buffer status: %d", percent );

                // done buffering
                if ( 100 == percent )
                {
                    buffering_ = false;

                    // update media status
                    updateMediaStatus( LoadedMedia );

                    // resume playing
                    if ( PlayingState == requestedState_ )
                        if ( !gst_element_set_state( pipeline_, GST_STATE_PLAYING ) )
                            LOG_WARN( "failed to play pipeline!", NULL );
                }

                // media is buffering...
                else if ( !buffering_ )
                {
                    buffering_ = true;

                    // update media status
                    updateMediaStatus( LoadingMedia );

                    // pause media (until buffering completes)
                    if ( PlayingState == requestedState_ )
                        if ( !gst_element_set_state( pipeline_, GST_STATE_PAUSED ) )
                            LOG_WARN( "failed to pause pipeline!", NULL );
                }
            }

            break;

        case GST_MESSAGE_STATE_CHANGED:
            {
                GstState old_state;
                GstState new_state;
                GstState pending_state;

                // parse message
                gst_message_parse_state_changed( msg, &old_state, &new_state, &pending_state );

                // only process when coming from source object
                if ( GST_OBJECT( source_ ) == GST_MESSAGE_SRC( msg ) )
                {
                    LOG_INFO( "pipeline state changed from %s to %s", gst_element_state_get_name( old_state ), gst_element_state_get_name( new_state ) );
                    internalState_ = new_state;

                    // update state
                    if ( GST_STATE_PAUSED == internalState_ )
                    {
                        // when stopping do not move through paused first
                        if ( StoppedState == requestedState_ )
                            ;
                        // duration of 0 means stopped (which is checked below)
                        else if ( 0 != duration_ )
                            updateState( PausedState );
                    }
                    else if ( GST_STATE_PLAYING == internalState_ )
                        updateState( PlayingState );
                    else
                        updateState( StoppedState );

                    // when playing, check if seeking is possible
                    if (( GST_STATE_PAUSED == internalState_ ) || ( GST_STATE_PLAYING == internalState_ ))
                    {
                        GstQuery *query( gst_query_new_seeking( GST_FORMAT_TIME ) );

                        if ( !gst_element_query( pipeline_, query ) )
                            LOG_WARN( "seeking query failed", NULL );
                        else
                        {
                            gboolean seek_enabled;
                            gint64 start;
                            gint64 end;

                            gst_query_parse_seeking( query, NULL, &seek_enabled, &start, &end );

                            if ( seek_enabled )
                            {
                                LOG_DEBUG( "seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT, GST_TIME_ARGS( start ), GST_TIME_ARGS( end ) );
                                updateSeekable( true );
                            }
                            else
                            {
                                LOG_DEBUG( "seeking is DISABLED for this stream.", NULL );
                                updateSeekable( false );
                            }
                        }

                        gst_query_unref( query );
                    }
                }
            }

            break;

        case GST_MESSAGE_ASYNC_START:
            LOG_DEBUG( "async start", NULL );
            break;

        case GST_MESSAGE_ASYNC_DONE:
            LOG_DEBUG( "async done", NULL );

            // when not buffering we are ready
            if ( !buffering_ )
                updateMediaStatus( LoadedMedia );

            break;

        default:
            LOG_WARN( "unknown message received", NULL );
            break;
        }

        gst_message_unref( msg );
    }

    // when playing, update position and duration
    if (( GST_STATE_PAUSED == internalState_ ) || ( GST_STATE_PLAYING == internalState_ ))
    {
        GstFormat fmt( GST_FORMAT_TIME );
        gint64 current( -1 );

        // query the stream duration
        if (( !duration_ ) || ( queryDuration ))
        {
            // retrieve duration
            if ( !gst_element_query_duration( pipeline_, fmt, &current ) )
                LOG_WARN( "could not query duration", NULL );
            else if ( (gint64) GST_CLOCK_TIME_NONE != current )
            {
                LOG_TRACE( "duration %" GST_TIME_FORMAT, GST_TIME_ARGS( current ) );
                updateDuration( GST_TIME_AS_MSECONDS( current ) );
            }
        }

        // query the current position of the stream
        if ( duration_ )
        {
            // retrieve position
            if ( !gst_element_query_position( pipeline_, fmt, &current ) )
                LOG_WARN( "could not query current position", NULL );
            else if ( (gint64) GST_CLOCK_TIME_NONE != current )
            {
                LOG_TRACE( "position %" GST_TIME_FORMAT, GST_TIME_ARGS( current ) );
                updatePosition( GST_TIME_AS_MSECONDS( current ) );

                LOG_DEBUG( "position %lld/%lld", (long long) position_, (long long) duration_ );

                // check for stopped state; it's a special case of PAUSED where position is zero
                if (( GST_STATE_PAUSED == internalState_ ) && ( 0 == position_ ))
                    updateState( StoppedState );
            }
        }
    }

    // check for queued events
    bool audioAvailable( false );
    bool videoAvailable( false );
    QString decoderError;

    m_.lock();

    if (( audioPadLinked_ ) && ( audioStreamsQueued_ != audioStreams_ ))
    {
        audioStreams_ = audioStreamsQueued_;
        audioAvailable = true;
    }

    if (( videoPadLinked_ ) && ( videoStreamsQueued_ != videoStreams_ ))
    {
        videoStreams_ = videoStreamsQueued_;
        videoAvailable = true;
    }

    if ( decoderErrorQueued_.count() )
    {
        decoderError = decoderErrorQueued_.first();
        decoderErrorQueued_.removeFirst();
    }

    m_.unlock();

    // process queued events

    if ( audioAvailable )
        Q_EMIT audioAvailableChanged( isAudioAvailable() );

    if ( videoAvailable )
        Q_EMIT videoAvailableChanged( isVideoAvailable() );

    if ( decoderError.length() )
        Q_EMIT error( decoderError );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::initialize()
{
    LOG_CLASS_INFO( "gstmp", "initialize" );

    // setup bus timer
    connect( &processBusEventsTimer_, SIGNAL(timeout()), SLOT(processBusEvents()) );

    processBusEventsTimer_.setSingleShot( false );
    processBusEventsTimer_.start( PROCESS_BUS_EVENTS_TIMER );

    // init gstreamer
    LOG_TRACE( "init gst...", NULL );
    gst_init( NULL, NULL );

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstmp::finalize()
{
    LOG_CLASS_INFO( "gstmp", "finalize" );

    // free bus
    if ( bus_ )
    {
        // free
        gst_object_unref( bus_ );
        bus_ = NULL;
    }

    // free pipeline
    if ( pipeline_ )
    {
        LOG_DEBUG( "finalizing pipeline...", NULL );

        // stop pipeline
        gst_element_set_state( pipeline_, GST_STATE_NULL );

        audioSinkBin_ = NULL;
        audioPad_ = NULL;

        videoSinkBin_ = NULL;
        videoPad_ = NULL;

        source_ = NULL;
        sound_ = NULL;
        videoSink_ = NULL;

        // free
        gst_object_unref( pipeline_ );
        pipeline_ = NULL;
    }

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstmp::createPipeline()
{
    LOG_CLASS_INFO( "gstmp", "createPipeline" );

    // create pipeline
    if ( !(pipeline_ = gst_pipeline_new( NULL )) )
        LOG_WARN( "failed to create pipeline!", NULL );
    else
    {
        // create source elements
        GstElement *source( gst_element_factory_make( "filesrc", NULL ) );
        GstElement *decoder( gst_element_factory_make( "decodebin", NULL ) );

        // validate
        if (( !source ) || ( !decoder ))
            LOG_WARN( "failed to create one or more elements!", NULL );
        else
        {
            // add elements to pipeline
            gst_bin_add_many( GST_BIN(pipeline_), source, decoder, NULL );

            // setup
            g_object_set( G_OBJECT( decoder ), "use-buffering", TRUE, NULL );

            // link
            if ( !gst_element_link_many( source, decoder, NULL ) )
                LOG_WARN( "failed to link source elements", NULL );
            else
            {
                // retrieve elements
                source_ = source;

                // hook decoder element
                g_signal_connect( decoder, "autoplug-continue", G_CALLBACK( gst_decodebin_autoplug_continue ), this );
                g_signal_connect( decoder, "no-more-pads", G_CALLBACK( gst_decodebin_no_more_pads ), this );
                g_signal_connect( decoder, "pad-added", G_CALLBACK( gst_decodebin_pad_added ), this );
                g_signal_connect( decoder, "unknown-type", G_CALLBACK( gst_decodebin_unknown_type ), this );

                // retrieve bus
                bus_ = gst_element_get_bus( pipeline_ );

                return true;
            }

            // unref
            gst_object_unref( source );
            gst_object_unref( decoder );
        }

        // free
        gst_object_unref( pipeline_ );
        pipeline_ = NULL;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstmp::createAudioSink()
{
    LOG_CLASS_INFO( "gstmp", "createAudioSink" );

    // create bin
    if ( !(audioSinkBin_ = gst_bin_new( NULL )) )
        LOG_WARN( "failed to create audio sink bin!", NULL );
    else
    {
        // create sink elements
        GstElement *queue( gst_element_factory_make( "queue", NULL ) );
        GstElement *audioconvert( gst_element_factory_make( "audioconvert", NULL ) );
        GstElement *audioresample( gst_element_factory_make( "audioresample", NULL ) );
        GstElement *volume( gst_element_factory_make( "volume", "vol" ) );
        GstElement *audioSink( gst_element_factory_make( "alsasink", "asink" ) );

        // validate
        if (( !queue ) || ( !audioconvert ) || ( !audioresample ) || ( !volume ) || ( !audioSink ))
            LOG_WARN( "failed to create one or more audio elements!", NULL );
        else
        {
            // limit queue memory usage
            g_object_set( G_OBJECT( queue ), "max-size-buffers", "0", NULL );
            g_object_set( G_OBJECT( queue ), "max-size-time", "0", NULL );

            // audio device for HDMI
            //g_object_set( G_OBJECT( audioSink ), "device", "plughw:1,0", NULL );

            // add elements to bin
            gst_bin_add_many( GST_BIN(audioSinkBin_), queue, audioconvert, audioresample, volume, audioSink, NULL );

            // link
            if ( !gst_element_link_many( queue, audioconvert, audioresample, volume, audioSink, NULL ) )
                LOG_WARN( "failed to link audio elements", NULL );
            else
            {
                GstPad *pad( gst_element_get_static_pad( queue, "sink" ) );

                // generate pad
                if ( !pad )
                    LOG_WARN( "failed to retrieve pad", NULL );
                else if ( !(audioPad_ = gst_ghost_pad_new( "sink", pad )) )
                    LOG_WARN( "failed to create audio pad", NULL );
                else
                {
                    // add bin to pipeline
                    gst_bin_add( GST_BIN(pipeline_), audioSinkBin_ );

                    // set active
                    if ( !gst_pad_set_active( audioPad_, TRUE ) )
                        LOG_WARN( "failed to set pad active", NULL );

                    // add pad to bin
                    if ( !gst_element_add_pad( audioSinkBin_, audioPad_ ) )
                        LOG_WARN( "failed to add pad to bin", NULL );

                    // save elements
                    sound_ = volume;

                    gst_object_unref( pad );
                    return true;
                }
            }

            // unref
            gst_object_unref( queue );
            gst_object_unref( audioconvert );
            gst_object_unref( audioresample );
            gst_object_unref( volume );
            gst_object_unref( audioSink );
        }

        // free
        gst_object_unref( audioSinkBin_ );
        audioSinkBin_ = NULL;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstmp::createVideoSink()
{
    LOG_CLASS_INFO( "gstmp", "createVideoSink" );

    // create bin
    if ( !(videoSinkBin_ = gst_bin_new( NULL )) )
        LOG_WARN( "failed to create video sink bin!", NULL );
    else
    {
        // create video
        GstElement *queue( gst_element_factory_make( "queue", NULL ) );
        GstElement *videoSink( NULL );

        if ( (videoSink = gst_element_factory_make( "imxv4l2sink", NULL )) )
        {
            const QString device( v4l2dev() );

            g_object_set( G_OBJECT( videoSink ), "force-aspect-ratio", TRUE, NULL );

            if ( device.length() )
                g_object_set( G_OBJECT( videoSink ), "device", qPrintable( device ), NULL );
        }
        else
        {
            videoSink = gst_element_factory_make( "autovideosink", NULL );
        }

        // validate
        if (( !queue ) || ( !videoSink ))
            LOG_WARN( "failed to create one or more video elements!", NULL );
        else
        {
            // limit queue memory usage
            g_object_set( G_OBJECT( queue ), "max-size-buffers", "0", NULL );
            g_object_set( G_OBJECT( queue ), "max-size-time", "0", NULL );

            // add elements to bin
            gst_bin_add_many( GST_BIN(videoSinkBin_), queue, videoSink, NULL );

            // link
            if ( !gst_element_link_many( queue, videoSink, NULL ) )
                LOG_WARN( "failed to link audio elements", NULL );
            else
            {
                GstPad *pad( gst_element_get_static_pad( queue, "sink" ) );

                // generate ghost pad
                if ( !pad )
                    LOG_WARN( "failed to retrieve pad", NULL );
                else if ( !(videoPad_ = gst_ghost_pad_new( "sink", pad )) )
                    LOG_WARN( "failed to create video pad", NULL );
                else
                {
                    // add bin to pipeline
                    gst_bin_add( GST_BIN(pipeline_), videoSinkBin_ );

                    // set active
                    if ( !gst_pad_set_active( videoPad_, TRUE ) )
                        LOG_WARN( "failed to set pad active", NULL );

                    // add ghost pad to bin
                    if ( !gst_element_add_pad( videoSinkBin_, videoPad_ ) )
                        LOG_WARN( "failed to add pad to bin", NULL );

                    // save elements
                    videoSink_ = videoSink;

                    gst_object_unref( pad );
                    return true;
                }
            }

            // unref
            gst_object_unref( queue );
            gst_object_unref( videoSink );
        }

        // free
        gst_object_unref( videoSinkBin_ );
        videoSinkBin_ = NULL;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // linux
