/**
 * @file multimediavideoplayer.h
 * Video player based on Qt5 Multimedia Framework.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef MULTIMEDIAVIDEOPLAYER_H
#define MULTIMEDIAVIDEOPLAYER_H

#include <QtGlobal>

// Note:
// QtMultimedia Framework is not supported in Qt4
#if ( QT_VERSION >= 0x050000 )

#include "../videoplayer.h"

#include <QMediaPlayer>
#include <QMutex>
#include <QTimer>
#include <QVideoWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Video player that uses the Qt5 Multimedia Framework.
class multimediaVideoPlayer : public videoPlayer
{
    Q_OBJECT

    typedef multimediaVideoPlayer _Myt;
    typedef videoPlayer _Mybase;

public:

    /// Source type.
    typedef QMediaPlayer sourceType;

    /// Video sink type.
    typedef QVideoWidget videoSinkType;

    /// Audio sink type.
    typedef QMediaPlayer audioSinkType;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    multimediaVideoPlayer( QWidget *parent = NULL );

    /// Constructor.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  parent widget
     */
    multimediaVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent = NULL );

    /// Destructor.
    virtual ~multimediaVideoPlayer();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if player active.
    /**
     * @return  @c true if player is active, @c false otherwise
     */
    virtual bool active() const;

    /// Retrieve the player audio sink.
    /**
     * @return  pointer to audio sink
     */
    virtual void *audioSink() {return &source_;}

    /// Check if stream can be paused.
    /**
     * @return  @c true if stream may be paused, @c false otherwise
     */
    virtual bool canPause() const {return true;}

    /// Check if volume can be set.
    /**
     * @return  @c true if volume may be set, @c false otherwise
     */
    virtual bool canSetVolume() const {return true;}

    /// Check if media has video component.
    /**
     * @return  media property
     */
    virtual MediaProperty hasVideo() const;

    /// Set video player volume.
    /**
     * @param[in] volume  volume to set within the range [0.0, 1.0]
     */
    virtual void setVolume( float volume );

    /// Retrieve the player source.
    /**
     * @return  pointer to source
     */
    virtual void *source() {return &source_;}

    /// Retrieve the player video sink.
    /**
     * @return  pointer to video sink
     */
    virtual void *videoSink() {return videoSink_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Load media into video player and start playing.
    /**
     * @param[in] filename  filename of media to load
     * @param[in] startPlaying  start playing video after loading
     * @return  @c true if player loaded and started playing, @c false otherwise
     */
    virtual bool load( const QString& filename, bool startPlaying = true );

    /// Pause video player.
    virtual void pause();

    /// Play video.
    virtual void play();

    /// Stop video player.
    virtual void stop();

protected:

    /// Amount of time to give multimedia to start playing.
    enum {LEAD_TIME_MS = 5000};

    sourceType source_;                                 ///< the media source
    videoSinkType *videoSink_;                          ///< pointer to video sink

    QSize mediaSize_;                                   ///< media size (dimensions)

    // ========================================================================
    // Events
    // ========================================================================

    /// Close event.
    /**
     * @param[in,out] e  close event
     */
    virtual void closeEvent( QCloseEvent *e );

    /// Hide event.
    /**
     * @param[in,out] e  hide event
     */
    virtual void hideEvent( QHideEvent *e );

    /// Move event.
    /**
     * @param[in,out] e  move event
     */
    virtual void moveEvent( QMoveEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  resize event details
     */
    virtual void resizeEvent( QResizeEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  show event
     */
    virtual void showEvent( QShowEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create new multimedia player.
    virtual void allocPlayer();

    /// Event filter.
    /**
     * @param[in] obj  object being filtered
     * @param[in] e  event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Destroy multimedia player.
    virtual void freePlayer();

private Q_SLOTS:

    /// Slot to check playback.
    void onCheckPlaybackTimer();

    /// Slot to receive errors.
    void onError( QMediaPlayer::Error error );

    /// Slot to receive when media status changes.
    void onMediaStatusChanged( QMediaPlayer::MediaStatus status );

    /// Slot to receive when meta data has changed.
    void onMetaDataChanged( const QString& key, const QVariant& value );

    /// Slot to receieve when media state changes.
    void onStateChanged( QMediaPlayer::State newState );

    /// Slot to recieve if source object has video or not.
    void onVideoAvailableChanged( bool hasVideo );

private:

    enum {CHECK_PLAYBACK_TIMER_MS = 50};

    enum playbackState
    {
        WAITING_FOR_NOTHING,
        WAITING_FOR_MEDIA_LOADED,
        WAITING_FOR_PLAY
    };

    QTimer checkPlaybackTimer_;

    unsigned int playbackTimer_;
    playbackState playbackState_;

    bool emittedPlaybackError_;

    QMediaPlayer::MediaStatus currentMediaStatus_;
    QMediaPlayer::State currentState_;

    // ========================================================================

    // not implemented
    multimediaVideoPlayer( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize.
    void initialize();

    /// Set player geometry such that it is centered in the parent.
    void setVideoSinkGeometry();

    /// Check media size and adjust viewport if needed.
    void checkMediaSize();

    /// Convert player error into human readable string.
    static QString toString( QMediaPlayer::Error error );

    /// Convert media status into human readable string.
    static QString toString( QMediaPlayer::MediaStatus status );

    /// Convert player state into human readable string.
    static QString toString( QMediaPlayer::State state );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // QT_VERSION >= 0x050000

#endif // MULTIMEDIAVIDEOPLAYER_H
