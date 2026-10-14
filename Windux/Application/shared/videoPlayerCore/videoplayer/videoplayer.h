/**
 * @file videoplayer.h
 * Video player base class.
 *
 * The @c videoPlayer class is the base object abstraction layer for all video players.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Video player device enumeration.
/**
 * Some video players require a device configuration for playback (i.e. gstreamer).  This is an
 * enumeration of those devices.
 */
class videoPlayerDevice
{
public:

    /// Video player device types.
    enum type
    {
        Default             = 0,                        ///< use default device
        Operator            = 1,                        ///< playback performed on operator display/speakers
        Customer            = 2                         ///< playback performed on customer display/speakers
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Video Player Base class.
/**
 * This is the base class interface for all video player objects.  Video player objects are
 * responsible for playing media and controlling playback.
 */
class videoPlayer : public QWidget
{
    Q_OBJECT

    typedef videoPlayer _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// This signal is emitted when video finishes playing.
    void finished();

    /// This signal is emitted when the media object detects audio.
    void hasAudioChanged( bool hasAudio );

    /// This signal is emitted when the media object detects video.
    void hasVideoChanged( bool hasVideo );

    /// This signal is emitted when playback error.
    /**
     * @param[in] desc  description of error
     */
    void playbackError( const QString& desc );

    /// This signal is emitted when location changes.
    /**
     * @param[in] time  current position in milliseconds
     */
    void tick( qint64 time );

    /// This signal is emitted when total time changes.
    /**
     * @param[in] newTotalTime  total time in milliseconds.
     */
    void totalTimeChanged( qint64 newTotalTime );

public:

    /// Media properties.
    enum MediaProperty
    {
        Unknown,                                        ///< not sure (try later?)
        Yes,                                            ///< has the property
        No                                              ///< does NOT have the property
    };

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~videoPlayer();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if player active.
    /**
     * @return  @c true if player is active, @c false otherwise
     */
    virtual bool active() const = 0;

    /// Retrieve the player audio sink.
    /**
     * @return  pointer to audio sink
     */
    virtual void *audioSink() {return NULL;}

    /// Check if stream can be paused.
    /**
     * @return  @c true if stream may be paused, @c false otherwise
     */
    virtual bool canPause() const {return false;}

    /// Check if volume can be set.
    /**
     * @return  @c true if volume may be set, @c false otherwise
     */
    virtual bool canSetVolume() const {return false;}

    /// Get player device.
    /**
     * @return  device playback is performed on
     */
    virtual videoPlayerDevice::type device() const {return device_;}

    /// Check if media has audio component.
    /**
     * @return  media property
     */
    virtual MediaProperty hasAudio() const {return Unknown;}

    /// Check if media has video component.
    /**
     * @return  media property
     */
    virtual MediaProperty hasVideo() const {return Unknown;}

    /// Set player device.
    /**
     * @param[in] value  device playbackis performed on
     */
    virtual void setDevice( videoPlayerDevice::type value ) {device_ = value;}

    /// Set video player volume.
    /**
     * @param[in] volume  volume to set within the range [0.0, 1.0]
     */
    virtual void setVolume( float volume ) {
        Q_UNUSED( volume );
    }

    /// Retrieve the player source.
    /**
     * @return  pointer to source
     */
    virtual void *source() {return NULL;}

    /// Retrieve the player video sink.
    /**
     * @return  pointer to video sink
     */
    virtual void *videoSink() {return NULL;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Load media into video player and optionally start playing.
    /**
     * @param[in] filename  filename of media to load
     * @param[in] startPlaying  start playing video after loading
     * @return  @c true if player loaded and started playing, @c false otherwise
     */
    virtual bool load( const QString& filename, bool startPlaying = true ) = 0;

    /// Pause video player.
    virtual void pause() {}

    /// Play video.
    virtual void play() = 0;

    /// Stop video player.
    virtual void stop() = 0;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Factory method to create video player.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  pointer to parent widget
     * @return  pointer to video player
     */
    static videoPlayer *create( videoPlayerDevice::type deviceType, QWidget *parent = NULL );

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    videoPlayer( QWidget *parent = NULL );

    /// Constructor.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  parent widget
     */
    videoPlayer( videoPlayerDevice::type deviceType, QWidget *parent = NULL );

private:

    videoPlayerDevice::type device_;

    // ========================================================================

    // not implemented
    videoPlayer( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // VIDEOPLAYER_H
