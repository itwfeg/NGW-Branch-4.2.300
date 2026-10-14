/**
 * @file gstmp.h
 * Media player object for gstreamer.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef GSTMP_H
#define GSTMP_H

#include "dbusmediaplayer.h"

#include <QMutex>
#include <QStringList>
#include <QTime>
#include <QTimer>

typedef struct _GstElement GstElement;
typedef struct _GstBus GstBus;
typedef struct _GstPad GstPad;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Media player object for gstreamer.
/**
 * This class implements a media player using gstreamer.
 */
class gstmp : public dbusMediaPlayer
{
    Q_OBJECT

    typedef gstmp _Myt;
    typedef dbusMediaPlayer _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~gstmp();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve media duration.
    /**
     * @return  duration (in msec)
     */
    virtual qint64 duration() const {return duration_;}

    /// Retrieve framebuffer device.
    /**
     * @return  framebuffer
     */
    virtual QString fb() const = 0;

    /// Retrieve video window geometry.
    /**
     * @return  window geometry
     */
    virtual QRect geometry() const {return geometry_;}

    /// Retrieve audio available.
    /**
     * @return  @c true if audio available, @c false otherwise
     */
    virtual bool isAudioAvailable() const {return (0 < audioStreams_);}

    /// Retrieve audio muted.
    /**
     * @return  @c true if audio muted, @c false otherwise
     */
    virtual bool isMuted() const {return muted_;}

    /// Retrieve media seekable.
    /**
     * @return  @c true if media seekable, @c false otherwise
     */
    virtual bool isSeekable() const {return seekable_;}

    /// Retrieve video available.
    /**
     * @return  @c true if video available, @c false otherwise
     */
    virtual bool isVideoAvailable() const {return (0 < videoStreams_);}

    /// Retrieve player visible.
    /**
     * @return  @c true if player visible, @c false otherwise
     */
    virtual bool isVisible() const {return visible_;}

    /// Retrieve media status.
    /**
     * @return  status
     */
    virtual MediaStatus mediaStatus() const {return mediaStatus_;}

    /// Retrieve media type.
    /**
     * @return  type
     */
    virtual MediaType mediaType() const {return mediaType_;}

    /// Retrieve media position.
    /**
     * @return  position (in msec)
     */
    virtual qint64 position() const {return position_;}

    /// Retrieve media state.
    /**
     * @return  state
     */
    virtual State state() const {return state_;}

    /// Retrieve v4l2 device.
    /**
     * @return  v4l2 device
     */
    virtual QString v4l2dev() const = 0;

    /// Retrieve media volume.
    /**
     * @return  volume [0, 100]
     */
    virtual int volume() const {return (int)(volume_ * 100.0);}

public Q_SLOTS:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set video window geometry.
    /**
     * @param[in] rect  window geometry
     */
    virtual void setGeometry( const QRect &rect );

    /// Set audio muted.
    /**
     * @param[in] muted  @c true to mute audio, @c false otherwise
     */
    virtual void setMuted( bool muted );

    /// Set media position.
    /**
     * @param[in]  position (in msec)
     */
    virtual void setPosition( qint64 position );

    /// Set player visible.
    /**
     * @param[in] visible  @c true to show, @c false to hide
     */
    virtual void setVisible( bool visible );

    /// Set media volume.
    /**
     * @return  volume [0, 100]
     */
    virtual void setVolume( int volume );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Hide player.
    virtual void hide();

    /// Link audio pad to source.
    /**
     * @note
     * This method is exposed for gstreamer callback use only.
     *
     * @param[in,out] pad  source pad
     */
    virtual void linkAudioPad( GstPad *pad );

    /// Link video pad to source.
    /**
     * @note
     * This method is exposed for gstreamer callback use only.
     *
     * @param[in,out] pad  source pad
     */
    virtual void linkVideoPad( GstPad *pad );

    /// Load media.
    /**
     * @param[in] filename  media filename
     * @param[in] type  media type
     */
    virtual void load( const QString &filename, abstractMediaPlayer::MediaType type );

    /// Pause media.
    virtual void pause();

    /// Play media.
    virtual void play();

    /// Queue an audio stream for event processing.
    /**
     * @note
     * This method is exposed for gstreamer callback use only.
     */
    virtual void queueAudioStream() {++audioStreamsQueued_;}

    /// Queue a decoder error for event processing.
    /**
     * @note
     * This method is exposed for gstreamer callback use only.
     */
    virtual void queueDecoderError( const QString &error );

    /// Queue an audio stream for event processing.
    /**
     * @note
     * This method is exposed for gstreamer callback use only.
     */
    virtual void queueVideoStream() {++videoStreamsQueued_;}

    /// Remove unlinked elements.
    /**
     * @note
     * This method is exposed for gstreamer callback use only.
     */
    virtual void removeUnlinkedElements();

    /// Show player.
    virtual void show();

    /// Stop media.
    virtual void stop();

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    gstmp( QObject *parent = NULL );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve sound muted.
    /**
     * @return  @c true if muted, @c false otherwise
     */
    virtual bool isSoundMuted() const;

    /// Set sound muted.
    /**
     * @param[in] value  @c true to mute, @c false otherwise
     * @return  @c true on success, @c false otherwise
     */
    virtual bool setSoundMuted( bool value );

    /// Set sound volume.
    /**
     * @param[in] value  volume [0.0, 1.0]
     * @return  @c true on success, @c false otherwise
     */
    virtual bool setSoundVolume( qreal value );

    /// Set video sink geometry.
    /**
     * @param[in] value  geomery
     * @return  @c true on success, @c false on failure
     */
    virtual bool setVideoSinkGeometry( const QRect &value );

    /// Retrieve sound volume.
    /**
     * @return  volume [0.0, 1.0]
     */
    virtual qreal soundVolume() const;

    /// Retrieve video sink geometry.
    /**
     * @return  geomery
     */
    virtual QRect videoSinkGeometry() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset media player.
    virtual void reset();

    /// Update media duration.
    /**
     * @param[in] value  duration (in msec)
     */
    virtual void updateDuration( qint64 value );

    /// Update media status.
    /**
     * @param[in] value  media status
     */
    virtual void updateMediaStatus( MediaStatus value );

    /// Update media position.
    /**
     * @param[in] value  position (in msec)
     */
    virtual void updatePosition( qint64 value );

    /// Update seekable.
    /**
     * @param[in] value  @c true if media seekable, @c false otherwise
     */
    virtual void updateSeekable( bool value );

    /// Update media state.
    /**
     * @param[in] value  media state
     */
    virtual void updateState( State value );

private Q_SLOTS:

    /// Process bus events.
    void processBusEvents();

private:

    enum {PROCESS_BUS_EVENTS_TIMER = 50};           ///< How often bus events are checked in milliseconds.

    enum {EMIT_POSITION_TIMER = 1000};              ///< How often position updates are emitted in milliseconds.

    //
    // gst
    //

    GstElement *pipeline_;                          ///< GST Pipeline.

    GstElement *audioSinkBin_;                      ///< Bin for audio sink pipeline.
    GstPad *audioPad_;                              ///< Audio sink pad.

    GstElement *videoSinkBin_;                      ///< Bin for video sink pipeline.
    GstPad *videoPad_;                              ///< Video sink pad.
    
	GstBus *bus_;                                   ///< GST Bus.

    GstElement *source_;                            ///< Source object.
    GstElement *sound_;                             ///< Sound object (for volume and mute).
    GstElement *videoSink_;                         ///< Video sink object.

    //
    // object state information
    //

    State state_;                                   ///< Player state.
    State requestedState_;                          ///< Requested player state.

    QString media_;                                 ///< Media filename.
    MediaType mediaType_;                           ///< Media type.
    MediaStatus mediaStatus_;                       ///< Media status.

    qint64 duration_;                               ///< Duration in milliseconds.
    qint64 position_;                               ///< Position in milliseconds.

    int audioStreams_;                              ///< Number of audio streams.
    int videoStreams_;                              ///< Number of video streams.

    QRect geometry_;                                ///< Display geometry.

    qreal volume_;                                  ///< Volume.

    bool buffering_;                                ///< Media is buffering.
    bool live_;                                     ///< Media is live stream.
    bool muted_;                                    ///< Media muted flag.
    bool seekable_;                                 ///< Media seekable flag.
    bool visible_;                                  ///< Player visible flag.

    //
    // internal
    //

    QMutex m_;

    int internalState_;

    int audioStreamsQueued_;
    int videoStreamsQueued_;

    bool audioPadLinked_;
    bool videoPadLinked_;

    QStringList decoderErrorQueued_;

    QTimer processBusEventsTimer_;

    QTime updatePositionStamp_;

    // not implemented
    _Myt& operator = ( const _Myt &value );

    /// Initialize.
    void initialize();

    /// Finalize.
    void finalize();

    /// Create pipeline.
    bool createPipeline();

    /// Create audio sink.
    bool createAudioSink();

    /// Create video sink.
    bool createVideoSink();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // GSTMP_H

