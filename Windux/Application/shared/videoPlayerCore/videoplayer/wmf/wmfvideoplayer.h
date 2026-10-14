/**
 * @file wmfvideoplayer.h
 * Video player based on Windows Media Foundation.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef WMFVIDEOPLAYER_H
#define WMFVIDEOPLAYER_H

#include <QtGlobal>

// Windows only!
#ifdef Q_OS_WIN

#include "../videoplayer.h"

#include <QList>
#include <QMutex>

// Qt5 workaround...
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

// Media Foundation headers
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <evr.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract class for handling WMF events.
class mediaFoundationSessionEventHandler
{
    typedef mediaFoundationSessionEventHandler _Myt;

    friend class mediaFoundationSession;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~mediaFoundationSessionEventHandler() {}

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    mediaFoundationSessionEventHandler() {}

    // ========================================================================
    // Events
    // ========================================================================

    /// Handle WMF session event.
    /**
     * @param[in,out] event  media event
     */
    virtual void mediaFoundationSessionEvent( IMFMediaEvent *event ) = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// WMF session object wrapper.
class mediaFoundationSession : private IMFAsyncCallback
{
    typedef mediaFoundationSession _Myt;
    typedef IMFAsyncCallback _Mybase;

public:

    /// Event handler type.
    typedef mediaFoundationSessionEventHandler eventHandlerType;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    mediaFoundationSession();

    /// Destructor.
    virtual ~mediaFoundationSession();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve event handler.
    /**
     * @return  pointer to event handler
     */
    virtual eventHandlerType *eventHandler() const;

    /// Retrieve session object.
    /**
     * @return  pointer to session
     */
    virtual IMFMediaSession *ptr() const {return session_;}

    /// Set event handler.
    /**
     * @param[in] value  pointer to event handler
     */
    virtual void setEventHandler( eventHandlerType *value );

    /// Check if session valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool valid() const {return (session_ ? true : false);}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Shutdown and release instance.
    virtual void finalize();

private:

    mutable QMutex m_;

    IMFMediaSession *session_;                          ///< pointer to session
    ULONG ref_;                                         ///< reference count

    eventHandlerType *eventHandler_;                    ///< event handler

    bool closing_;                                      ///< flag to track when closing
    HANDLE closeEvent_;                                 ///< event to wait on while closing.

    // ========================================================================

    HRESULT initialize();
    HRESULT shutdown();

    HRESULT allocSession();
    HRESULT freeSession();

    // IUnknown methods
    STDMETHODIMP QueryInterface( REFIID iid, void **ppv );
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IMFAsyncCallback methods
    STDMETHODIMP GetParameters( DWORD*, DWORD* )
    {
        // Implementation of this method is optional.
        return E_NOTIMPL;
    }

    STDMETHODIMP Invoke( IMFAsyncResult *pAsyncResult );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Source wrapper for handling ticks and seeking.
class mediaFoundationSource : public QObject
{
    Q_OBJECT

    typedef mediaFoundationSource _Myt;
    typedef QObject _Mybase;

    friend class mediaFoundationVideoPlayer;

Q_SIGNALS:

    /// This signal is emitted when total time changes.
    /**
     * @param[in] newTotalTime  total time in milliseconds.
     */
    void durationChanged( qint64 newTotalTime );

    /// This signal is emitted when location changes.
    /**
     * @param[in] time  current position in milliseconds
     */
    void positionChanged( qint64 time );

    /// This signal is emitted when seekability changes.
    /**
     * @param[in] seekable  @c true if seekable, @c false otherwise
     */
    void seekableChanged( bool seekable );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] session  pointer to session
     * @param[in] source  pointer to source
     * @param[in] parent  parent
     */
    mediaFoundationSource( IMFMediaSession *session, IMFMediaSource *source, QObject *parent = NULL );

    /// Destructor.
    ~mediaFoundationSource();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve stream duration.
    /**
     * @return  duration in milliseconds
     */
    virtual qint64 duration() const {return duration_;}

    /// Check if stream is seekable.
    /**
     * @return  @c true if seekable, @c false otherwise
     */
    virtual bool isSeekable() const {return (( 0 < duration_ ) && ( seekable_ ));}

    /// Retrieve stream position.
    /**
     * @return  position in milliseconds
     */
    virtual qint64 position() const {return position_;}

    /// Retrieve source object.
    /**
     * @return  pointer to source
     */
    virtual IMFMediaSource *ptr() const {return source_;}

    /// Check if source valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool valid() const {return ((( source_ ) && ( Unknown != state_ )) ? true : false);}

public Q_SLOTS:

    /// Set stream position.
    /**
     * @param[in] value  position in milliseconds
     */
    virtual void setPosition( qint64 value );

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set stream duration.
    /**
     * @param[in] value  duration in milliseconds
     */
    virtual void setDuration( qint64 value );

    /// Set if stream is seekable.
    /**
     * @@param[in] value  @c true if seekable, @c false otherwise
     */
    virtual void setSeekable( bool value );

    // ========================================================================
    // Events
    // ========================================================================

    /// Handle media event.
    /**
     * @param[in,out] e  event
     */
    virtual void mediaEvent( IMFMediaEvent *e );

private Q_SLOTS:

    /// Slot for timeout.
    void onTimeout();

private:

    enum state
    {
        Unknown,
        Stopped,
        Playing,
        Paused,
        Seeking
    };

    QMutex m_;

    IMFMediaSession *session_;                          ///< pointer to session
    IMFMediaSource *source_;                            ///< pointer to source

    IMFRateControl *rateControl_;                       ///< pointer to rate control
    IMFPresentationClock *clock_;                       ///< pointer to clock

    qint64 position_;
    qint64 duration_;
    bool seekable_;

    QTimer *timer_;

    state state_;
    state prevState_;

    bool scrubbing_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Video player based on Windows Media Foundation.
class mediaFoundationVideoPlayer : public videoPlayer, public mediaFoundationSessionEventHandler
{
    Q_OBJECT

    typedef mediaFoundationVideoPlayer _Myt;
    typedef videoPlayer _Mybase;

    typedef mediaFoundationSessionEventHandler _Myimpl;

public:

    /// Source type.
    typedef mediaFoundationSource sourceType;

    /// Video sink type.
    typedef IMFVideoDisplayControl videoSinkType;

    /// Audio sink type.
    typedef IMFSimpleAudioVolume audioSinkType;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    mediaFoundationVideoPlayer( QWidget *parent = NULL );

    /// Constructor.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  parent widget
     */
    mediaFoundationVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent = NULL );

    /// Destructor.
    virtual ~mediaFoundationVideoPlayer();

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

    /// Retrieve widget paint engine.
    /**
     * @return  pointer to paint engine
     */
    virtual QPaintEngine *paintEngine() const;

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

    /// Amount of time to give multimedia to start playing.
    enum {LEAD_TIME_MS = 5000};

    /// Player states.
    enum playerState
    {
        Closed = 0,                                     ///< no session
        Ready,                                          ///< session was created, ready to open a file
        OpenPending,                                    ///< session is opening a file
        Started,                                        ///< session is playing a file
        Paused,                                         ///< session is paused
        Stopped                                         ///< session is stopped (ready to play)
    };

    sourceType *source_;                                ///< media source
    videoSinkType *videoSink_;                          ///< video sink
    audioSinkType *audioSink_;                          ///< audio sink

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check media validity and state.
    /**
     * @return  @c true if media is valid and playable, @c false otherwise
     */
    virtual bool isMediaValid() const;

    /// Retrieve player state.
    /**
     * @return  state
     */
    virtual playerState state() const {return state_;}

    // ========================================================================
    // Events
    // ========================================================================

    /// Event filter.
    /**
     * @param[in,out] e  event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool event( QEvent *e );

    /// Hide event.
    /**
     * @param[in,out] e  hide event
     */
    virtual void hideEvent( QHideEvent *e );

    /// Handle WMF session event.
    /**
     * @param[in,out] e  media event
     */
    virtual void mediaFoundationSessionEvent( IMFMediaEvent *e );

    /// Paint event.
    /**
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  resize event details
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Take snapshot of current frame.
    /**
     * @return  image
     */
    virtual QImage snapshot() const;

private:

    bool emittedPlaybackError_;                         ///< has a playback error been emitted
    bool needToStartPlayback_;                          ///< play media immediately

    bool renderManually_;

    mediaFoundationSession *session_;                   ///< pointer to session object
    playerState state_;                                 ///< player state

    static unsigned int playbacks_;

    // ========================================================================

    // not implemented
    mediaFoundationVideoPlayer( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize.
    void initialize();

    /// Destroy source, attached pipeline, and session.
    void finalize( bool allocNewSession = true );

    /// Start playback.
    HRESULT startPlayback();

    /// Wait for specific state.
    bool waitForState( playerState s, const QTime& maxWait );

    /// Handle topology status event.
    HRESULT handleTopologyStatus( IMFMediaEvent *e );

    /// Handle presentation ended event.
    HRESULT handlePresentationEnded( IMFMediaEvent *e );

    /// Handle new presentation event.
    HRESULT handleNewPresentation( IMFMediaEvent *e );

    /// Handle error event.
    HRESULT handleError( IMFMediaEvent *e, bool fatal );

    /// Create a media source from a URL.
    /**
     * @param[in] url  source url
     * @param[in,out] source  source object
     * @return  result
     */
    static HRESULT createMediaSource( const QString& url, IMFMediaSource **source );

    /// Create an activation object for a renderer, based on the stream media type.
    /**
     * @param[in] pSourceSD  Pointer to the stream descriptor
     * @param[in] hVideoWindow  Handle to the video clipping window
     * @param[in,out] ppActivate  activate object
     * @return  result
     */
    static HRESULT createMediaSinkActivate( IMFStreamDescriptor *pSourceSD, HWND hVideoWindow, IMFActivate **ppActivate );

    /// Add a source node to a topology.
    /**
     * @param[in] pTopology  topology
     * @param[in] pSource  media source
     * @param[in] pPD  presentation descriptor
     * @param[in] pSD  stream descriptor
     * @param[in,out] ppNode  receives the node pointer
     * @return  result
     */
    static HRESULT addSourceNode( IMFTopology *pTopology, IMFMediaSource *pSource, IMFPresentationDescriptor *pPD, IMFStreamDescriptor *pSD, IMFTopologyNode **ppNode );

    /// Add an output node to a topology.
    /**
     * @param[in] pTopology  topology
     * @param[in] pActivate  media sink activation object
     * @param[in] dwId  identifier of the stream sink
     * @param[in,out] ppNode  receives the node pointer
     * @return  result
     */
    static HRESULT addOutputNode( IMFTopology *pTopology, IMFActivate *pActivate, DWORD dwId, IMFTopologyNode **ppNode );

    /// Add a topology branch for one stream.
    /**
     * For each stream, this function does the following:
     * 1. Creates a source node associated with the stream.
     * 2. Creates an output node for the renderer.
     * 3. Connects the two nodes.
     *
     * The media session will add any decoders that are needed.
     *
     * @param[in] pTopology  topology
     * @param[in] pSource  media source
     * @param[in] pPD  presentation descriptor
     * @param[in] iStream  stream index
     * @param[in] hVideoWnd  window for video playback
     * @return  result
     */
    static HRESULT addBranchToPartialTopology( IMFTopology *pTopology, IMFMediaSource *pSource, IMFPresentationDescriptor *pPD, DWORD iStream, HWND hVideoWnd );

    /// Create a playback topology from a media source.
    /**
     * @param[in] pSource  media source
     * @param[in] pPD  presentation descriptor
     * @param[in] hVideoWnd  video window
     * @param[in,out] ppTopology  receives a pointer to the topology
     * @return  result
     */
    static HRESULT createPlaybackTopology( IMFMediaSource *pSource, IMFPresentationDescriptor *pPD, HWND hVideoWnd, IMFTopology **ppTopology );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // Q_OS_WIN

#endif // WMFVIDEOPLAYER_H
