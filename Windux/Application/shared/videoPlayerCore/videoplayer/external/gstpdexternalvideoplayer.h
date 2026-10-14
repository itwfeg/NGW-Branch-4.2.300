/**
 * @file gstpdexternalvideoplayer.h
 * Video player based on gstpd (gst player daemon).
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GSTPDEXTERNALVIDEOPLAYER_H
#define GSTPDEXTERNALVIDEOPLAYER_H

#include "externalvideoplayer.h"

#include <dbusmediaplayerclient.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Video player based on gstpd (gst player daemon).
class gstpdExternalVideoPlayer : public externalVideoPlayer
{
    Q_OBJECT

    typedef gstpdExternalVideoPlayer _Myt;
    typedef externalVideoPlayer _Mybase;

public:

    /// Source type.
    typedef dbusMediaPlayerClient sourceType;

    /// Video sink type.
    typedef dbusMediaPlayerClient videoSinkType;

    /// Audio sink type.
    typedef dbusMediaPlayerClient audioSinkType;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    gstpdExternalVideoPlayer( QWidget *parent = NULL );

    /// Constructor.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  parent widget
     */
    gstpdExternalVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent = NULL );

    /// Destructor.
    virtual ~gstpdExternalVideoPlayer();

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
    virtual void *audioSink() {return client_;}

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

    /// Check if media has audio component.
    /**
     * @return  media property
     */
    virtual MediaProperty hasAudio() const;

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
    virtual void *source() {return client_;}

    /// Retrieve the player video sink.
    /**
     * @return  pointer to video sink
     */
    virtual void *videoSink() {return client_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Load media into video player and start playing.
    /**
     * @param[in] filename  filename of media to load
     * @param[in] startPlaying  start playing video after loading
     * @return  @c true if player loaded and started playing, @c false otherwise
     */
    virtual bool load( const QString &filename, bool startPlaying = true );

    /// Pause video player.
    virtual void pause();

    /// Play video.
    virtual void play();

    /// Stop video player.
    virtual void stop();

protected:

    dbusMediaPlayerClient *client_;                 ///< media source

    // ========================================================================
    // Events
    // ========================================================================

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

    /// Event filter.
    /**
     * @param[in,out] obj  object
     * @param[in,out] e  event
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Terminate any active process.
    virtual void terminate();

private:

    enum {DBUS_CONNECTION_TIMEOUT = 8000};

    // not implemented
    gstpdExternalVideoPlayer( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GSTPDEXTERNALVIDEOPLAYER_H
