/**
 * @file wmfvideoplayer.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "wmfvideoplayer.h"

// Windows only!
#ifdef Q_OS_WIN

#include <new>
#include <windows.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <assert.h>
#include <strsafe.h>

#include <QApplication>
#include <QFileInfo>
#include <QMutexLocker>
#include <QPainter>
#include <QTime>
#include <QTimer>

// Note:
// For some reason, the very first playback is only showing a blank screen. With this defined we
// manaully render each frame based on a timer.
#define ENABLE_FIRST_PLAYBACK_HACK

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void SafeRelease( T **ppT )
{
    if ( *ppT )
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Q>
HRESULT GetEventObject( IMFMediaEvent *pEvent, Q **ppObject )
{
    *ppObject = NULL;   // zero output

    PROPVARIANT var;
    HRESULT hr = pEvent->GetValue( &var );

    if ( SUCCEEDED( hr ) )
    {
        if ( VT_UNKNOWN == var.vt )
            hr = var.punkVal->QueryInterface( ppObject );
        else
            hr = MF_E_INVALIDTYPE;

        PropVariantClear( &var );
    }

    return hr;
}

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
// ================================================================================================
//
// mediaFoundationSession Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationSession::mediaFoundationSession() :
    session_( NULL ),
    ref_( 1 ),
    eventHandler_( NULL ),
    closing_( false ),
    closeEvent_( NULL )
{
    initialize();
    allocSession();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationSession::~mediaFoundationSession()
{
    shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationSession::finalize()
{
    LOG_CLASS_INFO( "mediaFoundationSession", "finalize" );

    HRESULT hr;

    if ( FAILED( (hr = shutdown()) ) )
        LOG_WARN( "failed shutdown 0x%08x", hr );

    // dont do anything after this release... if we are the last instance we are now deleted!
    LOG_DEBUG( "releasing...", NULL );
    Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationSession::eventHandlerType *mediaFoundationSession::eventHandler() const
{
    QMutexLocker guard( &m_ );
    return eventHandler_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationSession::setEventHandler( eventHandlerType *value )
{
    QMutexLocker guard( &m_ );
    eventHandler_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationSession::initialize()
{
    LOG_CLASS_INFO( "mediaFoundationSession", "initialize" );

    HRESULT hr;

    LOG_DEBUG( "initialize...", NULL );

    // Start up Media Foundation platform.
    if ( FAILED( (hr = MFStartup( MF_VERSION )) ) )
    {
        LOG_WARN( "failed WMF startup 0x%08x", hr );
        goto done;
    }

    // Create event for shutdown
    if ( !(closeEvent_ = CreateEvent( NULL, FALSE, FALSE, NULL )) )
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );

        LOG_WARN( "failed to create event!", NULL );
        goto done;
    }

done:

    LOG_DEBUG( "done 0x%08x", hr );
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationSession::shutdown()
{
    LOG_CLASS_INFO( "mediaFoundationSession", "shutdown" );

    // Close the session
    HRESULT hr( freeSession() );

    LOG_DEBUG( "shutdown...", NULL );

    // Shutdown the Media Foundation platform
    MFShutdown();

    if ( closeEvent_ )
    {
        CloseHandle( closeEvent_ );
        closeEvent_ = NULL;
    }

    LOG_DEBUG( "done 0x%08x", hr );
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationSession::allocSession()
{
    LOG_CLASS_INFO( "mediaFoundationSession", "allocSession" );

    // Close the old session, if any.
    HRESULT hr( freeSession() );

    if ( FAILED( hr ) )
        goto done;

    LOG_DEBUG( "creating session...", NULL );

    closing_ = false;

    // Create the media session.
    if ( FAILED( (hr = MFCreateMediaSession( NULL, &session_ )) ) )
    {
        LOG_WARN( "failed WMF create media session 0x%08x", hr );
        goto done;
    }

    // Start pulling events from the media session
    if ( FAILED( (hr = session_->BeginGetEvent( this, NULL )) ) )
    {
        LOG_WARN( "failed session begin event 0x%08x", hr );
        goto done;
    }

done:

    LOG_DEBUG( "done 0x%08x", hr );
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationSession::freeSession()
{
    LOG_CLASS_INFO( "mediaFoundationSession", "freeSession" );

    //  The IMFMediaSession::Close method is asynchronous, but the
    //  CPlayer::CloseSession method waits on the MESessionClosed event.
    //
    //  MESessionClosed is guaranteed to be the last event that the
    //  media session fires.

    HRESULT hr( S_OK );

    // First close the media session.
    if ( session_ )
    {
        closing_ = true;

        LOG_DEBUG( "closing session...", NULL );
        hr = session_->Close();

        // Wait for the close operation to complete
        if ( SUCCEEDED(hr) )
        {
            LOG_DEBUG( "waiting for close...", NULL );
            DWORD dwWaitResult( WaitForSingleObject( closeEvent_, 5000 ) );

            if ( dwWaitResult == WAIT_TIMEOUT )
                LOG_WARN( "wait failed!", NULL );

            // Now there will be no more events from this session.

            LOG_DEBUG( "shutdown session...", NULL );

            // Shut down the media session. (Synchronous operation, no events.)
            session_->Shutdown();
        }

        SafeRelease( &session_ );
    }

    LOG_DEBUG( "done 0x%08x", hr );
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationSession::QueryInterface( REFIID riid, void **ppv )
{
    static const QITAB qit[] = {
        QITABENT( mediaFoundationSession, IMFAsyncCallback ),
        { 0 }
    };

    return QISearch( this, qit, riid, ppv );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ULONG mediaFoundationSession::AddRef()
{
    LOG_CLASS_INFO( "mediaFoundationSession", "AddRef" );

    ULONG uCount( InterlockedIncrement( &ref_ ) );

    LOG_DEBUG( "session ref count %d", uCount );
    return uCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ULONG mediaFoundationSession::Release()
{
    LOG_CLASS_INFO( "mediaFoundationSession", "Release" );

    ULONG uCount( InterlockedDecrement( &ref_ ) );

    if ( !uCount )
    {
        LOG_DEBUG( "delete session object...", NULL );
        delete this;
    }

    LOG_DEBUG( "session ref count %d", uCount );
    return uCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationSession::Invoke( IMFAsyncResult *pResult )
{
    LOG_CLASS_INFO( "mediaFoundationSession", "Invoke" );

    MediaEventType meType = MEUnknown;  // Event type
    IMFMediaEvent *pEvent = NULL;

    HRESULT hr;

    // Get the event from the event queue.
    if ( FAILED( (hr = session_->EndGetEvent( pResult, &pEvent )) ) )
    {
        LOG_WARN( "failed session end event 0x%08x", hr );
        goto done;
    }

    // Get the event type.
    if ( FAILED( (hr = pEvent->GetType( &meType )) ) )
    {
        LOG_WARN( "failed to get event type 0x%08x", hr );
        goto done;
    }

    if ( MESessionClosed == meType )
    {
        LOG_INFO( "session closed!", NULL );

        // The session was closed.
        // The application is waiting on the m_hCloseEvent event handle.
        SetEvent( closeEvent_ );
    }
    else
    {
        // For all other events, get the next event in the queue.
        if ( FAILED( (hr = session_->BeginGetEvent( this, NULL )) ) )
        {
            LOG_WARN( "failed session begin event 0x%08x", hr );
            goto done;
        }
    }

    // Check the application state.

    // If a call to IMFMediaSession::Close is pending, it means the
    // application is waiting on the m_hCloseEvent event and
    // the application's message loop is blocked.

    // Otherwise, process the event.

    if (( !closing_ ) && ( eventHandler_ ))
    {
        QMutexLocker guard( &m_ );

        if ( eventHandler_ )
        {
            LOG_TRACE( "processing event of type %d...", meType );
            eventHandler_->mediaFoundationSessionEvent( pEvent );
        }
    }
    else
    {
        LOG_TRACE( "ignore event of type %d...", meType );

        // log out any errors
        if (( MEError == meType ) || ( MENonFatalError == meType ))
        {
            PROPVARIANT var;
            PropVariantInit( &var );

            // try to get error code
            if ( FAILED( (hr = pEvent->GetValue( &var )) ) )
                LOG_WARN( "failed to get error value 0x%08x", hr );
            else
            {
                LOG_ERROR( "session error %d 0x%08x", meType, var.ulVal );
            }

            PropVariantClear( &var );
        }
    }

done:

    SafeRelease( &pEvent );
    return S_OK;
}

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
// ================================================================================================
//
// mediaFoundationSource Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationSource::mediaFoundationSource( IMFMediaSession *session, IMFMediaSource *source, QObject *parent ) :
    _Mybase( parent ),
    session_( session ),
    source_( source ),
    rateControl_( NULL ),
    clock_( NULL ),
    position_( 0 ),
    duration_( 0 ),
    seekable_( false ),
    state_( Unknown ),
    scrubbing_( false )
{
    LOG_CLASS_INFO( "mediaFoundationSource", "mediaFoundationSource" );

    IMFClock *pClock = NULL;
    HRESULT hr;

    // Get the rate control service.
    if ( FAILED( (hr = MFGetService( session_, MF_RATE_CONTROL_SERVICE, IID_PPV_ARGS( &rateControl_ ) )) ) )
        LOG_WARN( "failed to retrieve rate control 0x%08x", hr );

    // Get the presentation clock (optional)
    if ( FAILED( (hr = session_->GetClock( &pClock )) ) )
        LOG_WARN( "failed to retrieve session clock 0x%08x", hr );
    else if ( FAILED( (hr = pClock->QueryInterface( IID_PPV_ARGS( &clock_ ) )) ) )
        LOG_WARN( "failed to query clock interface 0x%08x", hr );

    // start timer
    timer_ = new QTimer( this );
    timer_->setInterval( 100 );

    connect( timer_, SIGNAL( timeout() ), SLOT( onTimeout() ) );
    timer_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationSource::~mediaFoundationSource()
{
    LOG_CLASS_INFO( "mediaFoundationSource", "~mediaFoundationSource" );

    // cease operations
    timer_->stop();

    QMutexLocker guard( &m_ );

    // release rate control
    if ( rateControl_ )
    {
        LOG_TRACE( "free rate control...", NULL );
        SafeRelease( &rateControl_ );
    }

    // release clock
    if ( clock_ )
    {
        LOG_TRACE( "free clock...", NULL );
        SafeRelease( &clock_ );
    }

    // release source
    if ( source_ )
    {
        LOG_TRACE( "shudown source...", NULL );
        source_->Shutdown();

        LOG_TRACE( "free source...", NULL );
        SafeRelease( &source_ );
    }

    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationSource::setPosition( qint64 value )
{
    LOG_CLASS_INFO( "mediaFoundationSource", "setPosition" );

    QMutexLocker guard( &m_ );

    // nothing to do
    if ( !isSeekable() )
        return;
    // already seeking
    else if ( Seeking == state_ )
        return;

    PROPVARIANT varStart;
    PropVariantInit( &varStart );

    varStart.vt = VT_I8;
    varStart.hVal.QuadPart = value * 10000; // convert milliseconds to 100-nanoseconds

    HRESULT hr;

    // Stop playback when paused or stopped.
    if (( rateControl_ ) && ( Playing != state_ ))
    {
        if ( FAILED( (hr = rateControl_->SetRate( FALSE, 0.0 )) ) )
            LOG_WARN( "failed to set rate 0x%08x", hr );
        else
        {
            scrubbing_ = true;
        }
    }

    // Perform seek.
    if ( FAILED( (hr = session_->Start( NULL, &varStart )) ) )
        LOG_WARN( "failed to set position 0x%08x", hr );
    else
    {
        LOG_DEBUG( "seek to position %d", value );
        prevState_ = state_;
        state_ = Seeking;
    }

    PropVariantClear( &varStart );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationSource::setDuration( qint64 value )
{
    qint64 newValue( value / 10000 ); // convert 100-nanoseconds to milliseconds

    // nothing to do
    if ( duration_ == newValue )
        return;

    Q_EMIT durationChanged( duration_ = newValue );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationSource::setSeekable( bool value )
{
    // nothing to do
    if ( seekable_ == value )
        return;

    Q_EMIT seekableChanged( seekable_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationSource::mediaEvent( IMFMediaEvent *e )
{
    LOG_CLASS_INFO( "mediaFoundationSource", "mediaEvent" );

    // sanity check
    if ( !e )
        return;

    MediaEventType meType( MEUnknown );

    HRESULT hr;

    // Get the event type.
    if ( FAILED( (hr = e->GetType( &meType )) ) )
    {
        LOG_WARN( "failed to get event type 0x%08x", hr );
        goto done;
    }

    LOG_DEBUG( "handling event type %d...", meType );

    DWORD dwCaps( 0 );

    switch ( meType )
    {
    case MESessionCapabilitiesChanged:

        // The session capabilities changed. Get the updated capabilities.
        if ( (dwCaps = MFGetAttributeUINT32( e, MF_EVENT_SESSIONCAPS, dwCaps )) )
            setSeekable( MFSESSIONCAP_SEEK == (dwCaps & MFSESSIONCAP_SEEK) );

        break;

    case MESessionScrubSampleComplete:

        // Mark scrub complete.
        if (( rateControl_ ) && ( scrubbing_ ))
        {
            // Pause stream.
            if ( FAILED( (hr = session_->Pause()) ) )
                LOG_WARN( "failed to pause session 0x%08x", hr );
            else if ( FAILED( (hr = rateControl_->SetRate( FALSE, 1.0 )) ) )
                LOG_WARN( "failed to set rate 0x%08x", hr );
            else
            {
                state_ = Paused;
                scrubbing_ = false;

                // Resume stream.
                if ( Playing == prevState_ )
                {
                    PROPVARIANT var;
                    PropVariantInit( &var );

                    var.vt = VT_EMPTY;

                    if ( FAILED( (hr = session_->Start( NULL, &var )) ) )
                        LOG_WARN( "failed to start session 0x%08x", hr );

                    PropVariantClear( &var );
                }
            }
        }

        break;

    case MESessionStopped:
        if ( !scrubbing_ )
            state_ = Stopped;
        break;

    case MESessionStarted:
        if ( !scrubbing_ )
            state_ = Playing;
        break;

    case MESessionPaused:
        if ( !scrubbing_ )
            state_ = Paused;
        break;

    default:
        break;
    }

done:

    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationSource::onTimeout()
{
    LOG_CLASS_INFO( "mediaFoundationSource", "onTimeout" );

    QMutexLocker guard( &m_ );

    if ( !clock_ )
        return;
    else if ( Seeking == state_ )
        return;

    MFTIME pos;
    HRESULT hr;

    // Get current position and update.
    if ( FAILED( (hr = clock_->GetTime( &pos )) ) )
        LOG_WARN( "failed to get postion 0x%08x", hr );
    else
    {
        qint64 newPos( pos / 10000 ); // convert 100-nanoseconds to milliseconds

        if ( position_ != newPos )
            Q_EMIT positionChanged( position_ = newPos );
    }

}

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
// ================================================================================================
//
// mediaFoundationVideoPlayer Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

unsigned int mediaFoundationVideoPlayer::playbacks_( 0 );

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationVideoPlayer::mediaFoundationVideoPlayer( QWidget *parent ) :
    _Mybase( parent ),
    _Myimpl(),
    source_( NULL ),
    videoSink_( NULL ),
    audioSink_( NULL ),
    emittedPlaybackError_( false ),
    needToStartPlayback_( false ),
    renderManually_( false ),
    session_( NULL ),
    state_( Closed )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationVideoPlayer::mediaFoundationVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent ) :
    _Mybase( deviceType, parent ),
    _Myimpl(),
    source_( NULL ),
    videoSink_( NULL ),
    audioSink_( NULL ),
    emittedPlaybackError_( false ),
    needToStartPlayback_( false ),
    renderManually_( false ),
    session_( NULL ),
    state_( Closed )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationVideoPlayer::~mediaFoundationVideoPlayer()
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "~mediaFoundationVideoPlayer" );

    // cleanup
    finalize( false );

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mediaFoundationVideoPlayer::active() const
{
    if (( Started != state() ) && ( Paused != state() ))
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationVideoPlayer::MediaProperty mediaFoundationVideoPlayer::hasAudio() const
{
    if ( !isMediaValid() )
        return Unknown;

    return (audioSink_ ? Yes : No);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaFoundationVideoPlayer::MediaProperty mediaFoundationVideoPlayer::hasVideo() const
{
    if ( !isMediaValid() )
        return Unknown;

    return (videoSink_ ? Yes : No);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPaintEngine *mediaFoundationVideoPlayer::paintEngine() const
{
    if (( isMediaValid() ) && ( videoSink_ ) && ( !renderManually_ ))
        return NULL;

    return _Mybase::paintEngine();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::setVolume( float volume )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "setVolume" );

    // sanity check
    if (( !session_ ) || ( !source_ ))
        return;

    // no sink
    if ( !audioSink_ )
        return;

    HRESULT hr( audioSink_->SetMasterVolume( volume ) );

    if ( SUCCEEDED( hr ) )
        LOG_TRACE( "volume set to %.2f", volume );
    else
    {
        LOG_WARN( "failed to set volume 0x%08x!", hr );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mediaFoundationVideoPlayer::load( const QString& filename, bool startPlaying )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "load" );

    // sanity check
    if ( Closed == state_ )
        return false;

    // cleanup previous state
    finalize();

    // validate session object
    if ( !session_ )
    {
        LOG_WARN( "invalid session!", NULL );
        return false;
    }

    needToStartPlayback_ = startPlaying;

    // 1. Create the media source.
    // 2. Create the topology.
    // 3. Queue the topology [asynchronous]
    // 4. Start playback [asynchronous - does not happen in this method.]

    IMFTopology *pTopology( NULL );
    IMFPresentationDescriptor *pSourcePD( NULL );

    UINT64 nDuration;

    IMFMediaSource *pSource( NULL );

    HRESULT hr;

    // Create the media source.
    if ( FAILED( (hr = createMediaSource( filename, &pSource ) ) ) )
    {
        LOG_WARN( "failed to create media source 0x%08x", hr );
        goto done;
    }

    // setup source object
    source_ = new mediaFoundationSource( session_->ptr(), pSource );

    connect( source_, SIGNAL( positionChanged(qint64) ), SIGNAL( tick(qint64) ) );
    connect( source_, SIGNAL( durationChanged(qint64) ), SIGNAL( totalTimeChanged(qint64) ) );

    // Create the presentation descriptor for the media source.
    if ( FAILED( (hr = source_->ptr()->CreatePresentationDescriptor( &pSourcePD )) ) )
    {
        LOG_WARN( "failed source create presentation descriptor 0x%08x", hr );
        goto done;
    }

    // Create a partial topology.
    if ( FAILED( (hr = createPlaybackTopology( source_->ptr(), pSourcePD, (HWND) winId(), &pTopology )) ) )
    {
        LOG_WARN( "failed to create playback topology 0x%08x", hr );
        goto done;
    }

    // Set the topology on the media session.
    if ( FAILED( (hr = session_->ptr()->SetTopology( 0, pTopology )) ) )
    {
        LOG_WARN( "failed session set topology 0x%08x", hr );
        goto done;
    }

    // Grab the duration from the presentation descriptor.
    if ( FAILED( (hr = pSourcePD->GetUINT64( MF_PD_DURATION, &nDuration )) ) )
        LOG_WARN( "failed to retrieve duration 0x%08x", hr );
    else
    {
        source_->setDuration( nDuration );
    }

    LOG_INFO( "open pending", NULL );
    state_ = OpenPending;

    // If SetTopology succeeds, the media session will queue an
    // MESessionTopologySet event.

    if ( needToStartPlayback_ )
    {
        QTime done( QTime::currentTime().addMSecs( LEAD_TIME_MS ) );

        LOG_TRACE( "waiting for play state...", NULL );

        if ( waitForState( Started, done ) )
        {
            LOG_DEBUG( "done [playing]", NULL );
            goto done;
        }

        // in some cases media will not play but no error is emitted... if this happens
        // lets just emit a generic error...
        if ( !emittedPlaybackError_ )
        {
            QString desc = QString( tr( "Could not load video:\n%1" ) ).arg( filename );

            Q_EMIT playbackError( desc );
        }

        LOG_WARN( "media '%s' did not play!", qPrintable( filename ) );
        hr = E_FAIL;
    }

done:

    if ( FAILED( hr ) )
    {
        LOG_WARN( "closed", NULL );
        state_ = Closed;
    }

    needToStartPlayback_ = false;

    SafeRelease( &pSourcePD );
    SafeRelease( &pTopology );

    LOG_DEBUG( "done 0x%08x", hr );
    return (S_OK == hr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::pause()
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "pause" );

    // sanity check
    if (( !session_ ) || ( !source_ ))
        return;

    // only pause when playing
    if ( Started != state() )
        return;

    HRESULT hr( session_->ptr()->Pause() );

    if ( SUCCEEDED( hr ) )
        state_ = Paused;
    else
    {
        LOG_WARN( "failed to pause media 0x%08x!", hr );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::play()
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "play" );

    // sanity check
    if (( !session_ ) || ( !source_ ))
        return;

    // we still loading media... wait...
    if ( OpenPending == state() )
    {
        QTime done( QTime::currentTime().addMSecs( LEAD_TIME_MS ) );

        LOG_TRACE( "waiting for stopped state...", NULL );

        if ( !waitForState( Stopped, done ) )
            LOG_WARN( "media did not load", NULL );
    }

    // nothing to do
    if (( Paused != state() ) && ( Stopped != state() ))
        return;

    startPlayback();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::stop()
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "stop" );

    // sanity check
    if (( !session_ ) || ( !source_ ))
        return;

    // nothing to do
    if (( Started != state() ) && ( Paused != state() ))
        return;

    HRESULT hr( session_->ptr()->Stop() );

    if ( SUCCEEDED( hr ) )
        state_ = Stopped;
    else
    {
        LOG_WARN( "failed to stop media 0x%08x!", hr );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mediaFoundationVideoPlayer::isMediaValid() const
{
    // check player state
    if (( Stopped != state() ) && ( Started != state() ) && ( Paused != state() ))
        return false;

    // check source validitity
    return (( source_ ) && ( source_->valid() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mediaFoundationVideoPlayer::event( QEvent *e )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "event" );

    LOG_TRACE( "event 0x%08x of type %d", e, e->type() );

    // handle HWND change
    if ( QEvent::WinIdChange == e->type() )
        if ( videoSink_ )
        {
            HRESULT hr;

            HWND hWnd;

            if ( FAILED( (hr = videoSink_->GetVideoWindow( &hWnd )) ) )
                LOG_WARN( "failed to get video window 0x%08x", hr );
            else if ( hWnd != (HWND) winId() )
            {
                if ( FAILED( (hr = videoSink_->SetVideoWindow( (HWND) winId() )) ) )
                    LOG_WARN( "failed to set video window 0x%08x", hr );
            }
        }

    return _Mybase::event( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::hideEvent( QHideEvent *e )
{
    _Mybase::hideEvent( e );

    // when rendering manually there is an issue where you hide the window but the screen
    // does not refresh correctly... here we find the top level window and queue a paint event
    if ( renderManually_ )
        window()->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::mediaFoundationSessionEvent( IMFMediaEvent *e )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "mediaFoundationSessionEvent" );

    // sanity check
    if ( !e )
        return;

    HRESULT hrStatus( S_OK );
    MediaEventType meType( MEUnknown );

    HRESULT hr;

    // Get the event type.
    if ( FAILED( (hr = e->GetType( &meType )) ) )
    {
        LOG_WARN( "failed to get event type 0x%08x", hr );
        goto done;
    }

    // Get the event status. If the operation that triggered the event
    // did not succeed, the status is a failure code.
    hr = e->GetStatus( &hrStatus );

    // Check if the async operation succeeded.
    if (( SUCCEEDED( hr ) ) && ( FAILED( hrStatus ) ))
        hr = hrStatus;

    if ( FAILED( hr ) )
    {
        LOG_WARN( "failed to get status 0x%08x", hr );
        goto done;
    }

    LOG_TRACE( "handling event type %d...", meType );

    switch ( meType )
    {
    case MESessionTopologyStatus:
        hr = handleTopologyStatus( e );
        break;

    case MEEndOfPresentation:
        hr = handlePresentationEnded( e );
        break;

    case MENewPresentation:
        hr = handleNewPresentation( e );
        break;

    case MEError:
    case MENonFatalError:
        hr = handleError( e, (MEError == meType) );
        break;

    case MESessionCapabilitiesChanged:
    case MESessionScrubSampleComplete:
    case MESessionStopped:
    case MESessionStarted:
    case MESessionPaused:

        if ( source_ )
            source_->mediaEvent( e );

        hr = S_OK;
        break;

    default:
        hr = S_OK;
        break;
    }

done:

    LOG_TRACE( "done 0x%08x", hr );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::paintEvent( QPaintEvent *e )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "paintEvent" );

    Q_UNUSED( e );

    // if we have video draw it
    if (( isMediaValid() ) && ( videoSink_ ) && ( !renderManually_ ))
    {
        HRESULT hr;

        // repaint
        if ( FAILED( (hr = videoSink_->RepaintVideo()) ) )
            LOG_WARN( "failed to repaint video 0x%08x", hr );

        return;
    }

    QPainter painter( this );

    // capture a frame
    QImage frame( snapshot() );

    // no frame, draw blackness...
    if ( frame.isNull() )
    {
        painter.setBrush( Qt::black );
        painter.setPen( Qt::black );

        painter.drawRect( rect() );
    }
    else
    {
        painter.drawImage( rect(), frame );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    if ( videoSink_ )
    {
        // Set the destination rectangle.
        // Leave the default source rectangle (0,0,1,1).

        RECT rcDest = { 0, 0, width(), height() };

        videoSink_->SetVideoPosition( NULL, &rcDest );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QImage mediaFoundationVideoPlayer::snapshot() const
{
    if ( videoSink_ )
    {
        BITMAPINFOHEADER bmi;
        BYTE *buffer = 0;
        DWORD bufferSize;
        LONGLONG timeStamp;

        bmi.biSize = sizeof(BITMAPINFOHEADER);

        HRESULT hr = videoSink_->GetCurrentImage( &bmi, &buffer, &bufferSize, &timeStamp );

        if ( SUCCEEDED( hr ) )
        {
            const int w = qAbs( bmi.biWidth ),
            h = qAbs( bmi.biHeight );

            // Create image and copy data into image.
            QImage ret( w, h, QImage::Format_RGB32 );

            if ( !ret.isNull() )
            {
                uchar *data = buffer;
                const int bytes_per_line = w * sizeof(QRgb);

                for ( int y = h - 1; y >= 0; --y )
                {
                    memcpy(
                        ret.scanLine( y ),  //destination
                        data,               //source
                        bytes_per_line );

                    data += bytes_per_line;
                }
            }

            ::CoTaskMemFree (buffer );

            return ret;
        }
    }

    return QImage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::initialize()
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "initialize" );

    // set object properties
    setAttribute( Qt::WA_PaintOnScreen, true );
    setAutoFillBackground( false );
    setFocusPolicy( Qt::NoFocus );
    setObjectName( "mediaFoundationVideoPlayer" );

    // create session
    session_ = new mediaFoundationSession();

    if ( session_->valid() )
    {
        session_->setEventHandler( this );
        state_ = Ready;
    }
    else
    {
        LOG_ERROR( "failed to create WMF session!", NULL );

        session_->finalize();
        session_ = NULL;
    }

    // increment playbacks
    ++playbacks_;

#if defined( ENABLE_FIRST_PLAYBACK_HACK )

    // check for first playback and manual rendering
    renderManually_ = (1 == playbacks_);

    QString appName( QFileInfo( QCoreApplication::applicationFilePath() ).fileName() );

    // this hack only appears to be needed when there are overlays or transparent windows... at the
    // moment this is only scaleui...
    if ( "scaleui.exe" != appName )
        renderManually_ = false;

#endif

    // manual rendering mode
    if ( renderManually_ )
    {
        LOG_INFO( "manual render mode!", NULL );

        QTimer *t( new QTimer( this ) );
        connect( t, SIGNAL( timeout() ), SLOT( repaint() ) );

        t->start( 30 );
    }

    // wmf rendering
    else
    {
        setAttribute( Qt::WA_OpaquePaintEvent, true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaFoundationVideoPlayer::finalize( bool allocNewSession )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "finalize" );

    // stop any active media
    stop();

    // release video sink
    if ( videoSink_ )
    {
        LOG_TRACE( "free video sink...", NULL );
        SafeRelease( &videoSink_ );

        QCoreApplication::processEvents();
    }

    // release audio sink
    if ( audioSink_ )
    {
        LOG_TRACE( "free audio sink...", NULL );
        SafeRelease( &audioSink_ );

        QCoreApplication::processEvents();
    }

    // mark source for deletion
    // do not delete source as controls still may be referencing interfaces
    if ( source_ )
    {
        LOG_TRACE( "free source...", NULL );
        source_->deleteLater();
        source_ = NULL;
    }

    // HW-7
    // WMF player cannot play two videos using the same session object... so lets delete out the
    // old one and create a new one. This seems wrong to me, I should be able to use the same
    // session object. All the documentation says I can. But in practice it is not working.

    // release session
    if ( session_ )
    {
        // Note:
        // Do not delete session!  Just finalize it (it will delete itself)...

        LOG_TRACE( "finalize session...", NULL );
        session_->setEventHandler( NULL );
        session_->finalize();

        session_ = NULL;
        state_ = Closed;
    }

    LOG_TRACE( "process events...", NULL );
    QCoreApplication::processEvents();

    // create session
    if ( allocNewSession )
    {
        session_ = new mediaFoundationSession();

        if ( session_->valid() )
        {
            session_->setEventHandler( this );
            state_ = Ready;
        }
        else
        {
            LOG_ERROR( "failed to create WMF session!", NULL );

            session_->finalize();
            session_ = NULL;
        }
    }

    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::startPlayback()
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "startPlayback" );

    PROPVARIANT varStart;
    PropVariantInit( &varStart );

    HRESULT hr;

    // Note: Start is an asynchronous operation. However, we
    // can treat our state as being already started. If Start
    // fails later, we'll get an MESessionStarted event with
    // an error code, and we will update our state then.
    if ( FAILED( (hr = session_->ptr()->Start( &GUID_NULL, &varStart )) ) )
        LOG_WARN( "failed to start playback 0x%08x!", hr );
    else
    {
        state_ = Started;
    }

    PropVariantClear( &varStart );

    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mediaFoundationVideoPlayer::waitForState( playerState s, const QTime& maxWait )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "waitForState" );

    do
    {
        if ( s == state() )
        {
            LOG_TRACE( "done waiting for state %d", s );
            return true;
        }

        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );

    } while ( QTime::currentTime() < maxWait );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::handleTopologyStatus( IMFMediaEvent *e )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "handleTopologyStatus" );
    UINT32 status;

    HRESULT hr;

    // get topology status
    if ( FAILED( (hr = e->GetUINT32( MF_EVENT_TOPOLOGY_STATUS, &status )) ) )
        LOG_WARN( "failed to get topology status 0x%08x", hr );

    // ready so lets process
    else if ( MF_TOPOSTATUS_READY == status )
    {
        LOG_INFO( "stopped", NULL );
        state_ = Stopped;

        // ---- //

        SafeRelease( &videoSink_ );

        // Get the IMFVideoDisplayControl interface from EVR. This call is
        // expected to fail if the media file does not have a video stream.

        MFGetService( session_->ptr(), MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS( &videoSink_ ) );

        // letterbox the video
        if ( videoSink_ )
        {
            // Set the destination rectangle.
            // Leave the default source rectangle (0,0,1,1).

            RECT rcDest = { 0, 0, width(), height() };

            if ( FAILED( (hr = videoSink_->SetVideoPosition( NULL, &rcDest )) ) )
                LOG_WARN( "failed to set video position 0x%08x", hr );

            if ( FAILED( (hr = videoSink_->SetAspectRatioMode( MFVideoARMode_PreservePicture )) ) )
                LOG_WARN( "failed to set aspect ratio mode 0x%08x", hr );
        }

        // ---- //

        SafeRelease( &audioSink_ );

        // Get the IMFVideoDisplayControl interface from EVR. This call is
        // expected to fail if the media file does not have a video stream.

        MFGetService( session_->ptr(), MR_POLICY_VOLUME_SERVICE, IID_PPV_ARGS( &audioSink_ ) );

        // ---- //

        LOG_DEBUG( "topology status ready: v 0x%08x a 0x%08x", videoSink_, audioSink_ );
        Q_EMIT hasVideoChanged( (Yes == hasVideo()) );
        Q_EMIT hasAudioChanged( (Yes == hasAudio()) );

        // start video playback
        if ( needToStartPlayback_ )
        {
            LOG_DEBUG( "starting playback...", NULL );
            startPlayback();

            needToStartPlayback_ = false;
        }
    }

    LOG_DEBUG( "done 0x%08x", hr );
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::handlePresentationEnded( IMFMediaEvent *e )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "handlePresentationEnded" );

    Q_UNUSED( e );

    // The session puts itself into the stopped state automatically.
    LOG_INFO( "stopped", NULL );
    state_ = Stopped;

    Q_EMIT finished();

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::handleNewPresentation( IMFMediaEvent *e )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "handleNewPresentation" );

    IMFPresentationDescriptor *pPD( NULL );
    IMFTopology *pTopology( NULL );

    UINT64 nDuration;

    HRESULT hr;

    // Get the presentation descriptor from the event.
    if ( FAILED( (hr = GetEventObject( e, &pPD )) ) )
    {
        LOG_WARN( "failed to get event type 0x%08x", hr );
        goto done;
    }

    LOG_DEBUG( "processing new presentation...", NULL );

    // Create a partial topology.
    if ( FAILED( (hr = createPlaybackTopology( source_->ptr(), pPD, (HWND) winId() ,&pTopology )) ) )
    {
        LOG_WARN( "failed to create playback topology 0x%08x", hr );
        goto done;
    }

    // Set the topology on the media session.
    if ( FAILED( (hr = session_->ptr()->SetTopology( 0, pTopology )) ) )
    {
        LOG_WARN( "failed to set session topology 0x%08x", hr );
        goto done;
    }

    // Grab the duration from the presentation descriptor.
    if ( FAILED( (hr = pPD->GetUINT64( MF_PD_DURATION, &nDuration )) ) )
        LOG_WARN( "failed to retrieve duration 0x%08x", hr );
    else if ( source_ )
    {
        source_->setDuration( nDuration );
    }

    LOG_INFO( "open pending", NULL );
    state_ = OpenPending;

done:

    SafeRelease( &pTopology );
    SafeRelease( &pPD );
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::handleError( IMFMediaEvent *e, bool fatal )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "handleError" );

    HRESULT hr;

    PROPVARIANT var;
    PropVariantInit( &var );

    QString description;

    // setup description
    if ( !fatal )
        description = tr( "Media session non-fatal error." );
    else
    {
        description = tr( "Media session serious error." );
        state_ = Closed;
    }

    // try to get error code
    if ( FAILED( (hr = e->GetValue( &var )) ) )
        LOG_WARN( "failed to get error value 0x%08x", hr );
    else
    {
        hr = var.ulVal;

        if ( FAILED( hr ) )
            description.append( " (0x" + QString::number( hr, 16 ) + ")" );

        LOG_ERROR( "session error 0x%08x", hr );
    }

    // emit!
    Q_EMIT playbackError( description );
    emittedPlaybackError_ = true;

    PropVariantClear( &var );
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::createMediaSource( const QString& url, IMFMediaSource **source )
{
    LOG_CLASS_INFO( "mediaFoundationVideoPlayer", "createMediaSource" );

    MF_OBJECT_TYPE ObjectType( MF_OBJECT_INVALID );

    IMFSourceResolver *pSourceResolver( NULL );
    IUnknown *pSource( NULL );

    HRESULT hr;

    // Create the source resolver.
    if ( FAILED( (hr = MFCreateSourceResolver( &pSourceResolver )) ) )
    {
        LOG_WARN( "failed to create source resolver 0x%08x", hr );
        goto done;
    }

    LOG_DEBUG( "loading media '%s'...", qPrintable( url ) );

    // Use the source resolver to create the media source.

    // Note: For simplicity this sample uses the synchronous method to create
    // the media source. However, creating a media source can take a noticeable
    // amount of time, especially for a network source. For a more responsive
    // UI, use the asynchronous BeginCreateObjectFromURL method.

    wchar_t *w_url = new wchar_t[url.length() + 1];
    url.toWCharArray( w_url );
    w_url[url.length()] = NULL;

    hr = pSourceResolver->CreateObjectFromURL(
        w_url,                          // URL of the source.
        MF_RESOLUTION_MEDIASOURCE,      // Create a source object.
        NULL,                           // Optional property store.
        &ObjectType,                    // Receives the created object type.
        &pSource );                     // Receives a pointer to the media source.

    delete w_url;

    if ( FAILED( hr ) )
    {
        LOG_WARN( "failed to create source object 0x%08x", hr );
        goto done;
    }

    // Get the IMFMediaSource interface from the media source.
    if ( FAILED( (hr = pSource->QueryInterface( IID_PPV_ARGS( source ) )) ) )
    {
        LOG_WARN( "failed query interface source object 0x%08x", hr );
        goto done;
    }

done:

    SafeRelease( &pSourceResolver );
    SafeRelease( &pSource );

    LOG_DEBUG( "done 0x%08x", hr );
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::createMediaSinkActivate(
    IMFStreamDescriptor *pSourceSD,
    HWND hVideoWindow,
    IMFActivate **ppActivate )
{
    IMFMediaTypeHandler *pHandler = NULL;
    IMFActivate *pActivate = NULL;

    // Get the media type handler for the stream.
    HRESULT hr = pSourceSD->GetMediaTypeHandler(&pHandler);
    if (FAILED(hr))
    {
        goto done;
    }

    // Get the major media type.
    GUID guidMajorType;
    hr = pHandler->GetMajorType(&guidMajorType);
    if (FAILED(hr))
    {
        goto done;
    }

    // Create an IMFActivate object for the renderer, based on the media type.
    if (MFMediaType_Audio == guidMajorType)
    {
        // Create the audio renderer.
        hr = MFCreateAudioRendererActivate(&pActivate);
    }
    else if (MFMediaType_Video == guidMajorType)
    {
        // Create the video renderer.
        hr = MFCreateVideoRendererActivate(hVideoWindow, &pActivate);
    }
    else
    {
        // Unknown stream type.
        hr = E_FAIL;
        // Optionally, you could deselect this stream instead of failing.
    }
    if (FAILED(hr))
    {
        goto done;
    }

    // Return IMFActivate pointer to caller.
    *ppActivate = pActivate;
    (*ppActivate)->AddRef();

done:

    SafeRelease(&pHandler);
    SafeRelease(&pActivate);
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::addSourceNode(
    IMFTopology *pTopology,
    IMFMediaSource *pSource,
    IMFPresentationDescriptor *pPD,
    IMFStreamDescriptor *pSD,
    IMFTopologyNode **ppNode )
{
    IMFTopologyNode *pNode = NULL;

    // Create the node.
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode);
    if (FAILED(hr))
    {
        goto done;
    }

    // Set the attributes.
    hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the node to the topology.
    hr = pTopology->AddNode(pNode);
    if (FAILED(hr))
    {
        goto done;
    }

    // Return the pointer to the caller.
    *ppNode = pNode;
    (*ppNode)->AddRef();

done:

    SafeRelease(&pNode);
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::addOutputNode(
    IMFTopology *pTopology,
    IMFActivate *pActivate,
    DWORD dwId,
    IMFTopologyNode **ppNode )
{
    IMFTopologyNode *pNode = NULL;

    // Create the node.
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode);
    if (FAILED(hr))
    {
        goto done;
    }

    // Set the object pointer.
    hr = pNode->SetObject(pActivate);
    if (FAILED(hr))
    {
        goto done;
    }

    // Set the stream sink ID attribute.
    hr = pNode->SetUINT32(MF_TOPONODE_STREAMID, dwId);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the node to the topology.
    hr = pTopology->AddNode(pNode);
    if (FAILED(hr))
    {
        goto done;
    }

    // Return the pointer to the caller.
    *ppNode = pNode;
    (*ppNode)->AddRef();

done:
    SafeRelease(&pNode);
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::addBranchToPartialTopology(
    IMFTopology *pTopology,
    IMFMediaSource *pSource,
    IMFPresentationDescriptor *pPD,
    DWORD iStream,
    HWND hVideoWnd )
{
    IMFStreamDescriptor *pSD = NULL;
    IMFActivate         *pSinkActivate = NULL;
    IMFTopologyNode     *pSourceNode = NULL;
    IMFTopologyNode     *pOutputNode = NULL;

    BOOL fSelected = FALSE;

    HRESULT hr = pPD->GetStreamDescriptorByIndex(iStream, &fSelected, &pSD);
    if (FAILED(hr))
    {
        goto done;
    }

    if (fSelected)
    {
        // Create the media sink activation object.
        hr = createMediaSinkActivate(pSD, hVideoWnd, &pSinkActivate);
        if (FAILED(hr))
        {
            goto done;
        }

        // Add a source node for this stream.
        hr = addSourceNode(pTopology, pSource, pPD, pSD, &pSourceNode);
        if (FAILED(hr))
        {
            goto done;
        }

        // Create the output node for the renderer.
        hr = addOutputNode(pTopology, pSinkActivate, 0, &pOutputNode);
        if (FAILED(hr))
        {
            goto done;
        }

        // Connect the source node to the output node.
        hr = pSourceNode->ConnectOutput(0, pOutputNode, 0);
    }
    // else: If not selected, don't add the branch.

done:

    SafeRelease(&pSD);
    SafeRelease(&pSinkActivate);
    SafeRelease(&pSourceNode);
    SafeRelease(&pOutputNode);
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT mediaFoundationVideoPlayer::createPlaybackTopology(
    IMFMediaSource *pSource,
    IMFPresentationDescriptor *pPD,
    HWND hVideoWnd,
    IMFTopology **ppTopology )
{
    IMFTopology *pTopology = NULL;
    DWORD cSourceStreams = 0;

    // Create a new topology.
    HRESULT hr = MFCreateTopology(&pTopology);
    if (FAILED(hr))
    {
        goto done;
    }

    // Get the number of streams in the media source.
    hr = pPD->GetStreamDescriptorCount(&cSourceStreams);
    if (FAILED(hr))
    {
        goto done;
    }

    // For each stream, create the topology nodes and add them to the topology.
    for (DWORD i = 0; i < cSourceStreams; i++)
    {
        hr = addBranchToPartialTopology(pTopology, pSource, pPD, i, hVideoWnd);
        if (FAILED(hr))
        {
            goto done;
        }
    }

    // Return the IMFTopology pointer to the caller.
    *ppTopology = pTopology;
    (*ppTopology)->AddRef();

done:

    SafeRelease(&pTopology);
    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // Q_OS_WIN

