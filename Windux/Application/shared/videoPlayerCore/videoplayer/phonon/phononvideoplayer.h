/**
 * @file phononvideoplayer.h
 * Video player based on Qt Phonon.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef PHONONVIDEOPLAYER_H
#define PHONONVIDEOPLAYER_H

#include <QtGlobal>

// Note:
// Phonon is not supported in Qt5
#if ( QT_VERSION < 0x050000 )

#include "../videoplayer.h"

#include <phonon/MediaObject>
#include <phonon/VideoWidget>
#include <phonon/AudioOutput>

#include <phonon/BackendCapabilities>

#include <QMutex>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Video player that uses the Qt Phonon framework/plugin.
class phononVideoPlayer : public videoPlayer
{
    Q_OBJECT

    typedef phononVideoPlayer _Myt;
    typedef videoPlayer _Mybase;

public:

    /// Source type.
    typedef Phonon::MediaObject sourceType;

    /// Video sink type.
    typedef Phonon::VideoWidget videoSinkType;

    /// Audio sink type.
    typedef Phonon::AudioOutput audioSinkType;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    phononVideoPlayer( QWidget *parent = NULL );

    /// Constructor.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  parent widget
     */
    phononVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent = NULL );

    /// Destructor.
    virtual ~phononVideoPlayer();

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
    virtual void *audioSink() {return audioSink_;}

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
    virtual void *source() {return source_;}

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

    /// Amount of time to give phonon to start playing.
    enum {LEAD_TIME_MS = 5000};

    Phonon::MediaObject *source_;                       ///< the media source
    Phonon::VideoWidget *videoSink_;                    ///< pointer to video sink
    Phonon::AudioOutput *audioSink_;                    ///< pointer to audio sink

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

    /// Create new phonon player.
    virtual void allocPlayer();

    /// Event filter.
    /**
     * @param[in] obj  object being filtered
     * @param[in] e  event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Destroy phonon player.
    virtual void freePlayer();

private Q_SLOTS:

    /// Slot to receive playback done signal.
    void playbackDone();

    /// Slot to recieve if source object has video or not.
    void mediaHasVideoChanged( bool hasVideo );

    /// Slot to receive when media data has changed.
    void mediaMetaDataChanged();

    /// Slot to receieve when media state changes.
    void mediaStateChanged( Phonon::State newState, Phonon::State oldState );

private:

    typedef QMultiMap<QString, QString> metaDataMap;

    bool emittedPlaybackError_;

    QMutex mutex_;

    // ========================================================================

    // not implemented
    phononVideoPlayer( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize.
    void initialize();

    /// Set player geometry such that it is centered in the parent.
    void setVideoSinkGeometry();

    /**
     * Delete video player object.
     */
    template <class T>
    static void deleteVideoObject( T **ptr )
    {
        if (( ptr ) && ( *ptr ))
        {
            T *doomed( *ptr );
            (*ptr) = NULL;

            deleteVideoObjectImpl( doomed );

            delete doomed;
        }
    }

    /**
     * Delete video player object.
     */
    template <class T>
    static void deleteVideoObjectImpl( T* /*ptr*/ ) {}

    /**
     * Convert phonon state into human readable string.
     */
    static QString toString( Phonon::State state );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // QT_VERSION < 0x050000

#endif // PHONONVIDEOPLAYER_H
