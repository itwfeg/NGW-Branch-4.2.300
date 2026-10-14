/**
 * @file externalvideoplayer.h
 * Video player based on command line launch.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef EXTERNALVIDEOPLAYER_H
#define EXTERNALVIDEOPLAYER_H

#include "appconfig.h"

#include "../videoplayer.h"

#include <process.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Video player class that uses command line driven arguments.
class externalVideoPlayer : public videoPlayer
{
    Q_OBJECT

    typedef externalVideoPlayer _Myt;
    typedef videoPlayer _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] config  configuration
     * @param[in] parent  parent widget
     */
    externalVideoPlayer( appConfig *config, QWidget *parent = NULL );

    /// Constructor.
    /**
     * @param[in] config  configuration
     * @param[in] deviceType  device type
     * @param[in] parent  parent widget
     */
    externalVideoPlayer( appConfig *config, videoPlayerDevice::type deviceType, QWidget *parent = NULL );

    /// Destructor.
    virtual ~externalVideoPlayer();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if player active.
    /**
     * @return  @c true if player is active, @c false otherwise
     */
    virtual bool active() const;

    /// Check if stream can be paused.
    /**
     * @return  @c true if stream may be paused, @c false otherwise
     */
    virtual bool canPause() const;

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

    process process_;                               ///< external video player process

    // ========================================================================
    // Methods
    // ========================================================================

    /// Terminate any active process.
    virtual void terminate();

private Q_SLOTS:

    /// Slot to handle process finished.
    void onProcessFinished();

private:

    // not implemented
    externalVideoPlayer( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    enum {START_COMMAND_TIMEOUT_MS = 5000};

    enum {STOP_COMMAND_TIMEOUT_MS = 2000};
    enum {TERM_COMMAND_TIMEOUT_MS = 1000};

    appConfig *config_;

    QString launchCommand_;
    QString playCommand_;
    QString pauseCommand_;
    QString stopCommand_;
    QString killCommand_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // EXTERNALVIDEOPLAYER_H
