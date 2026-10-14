/**
 * @file volumesliderimpl.h
 * Slider to handle multimedia playback volumes.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef VOLUMESLIDERIMPL_H
#define VOLUMESLIDERIMPL_H

#include "volumeslider.h"

#include <logmanager.h>

#include <QSlider>
#include <QTimer>
#include <QToolButton>

#if ( QT_VERSION >= 0x050000 )
// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
// ================================================================================================
//
// Qt5 QtMultimedia Framework Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

template <>
void volumeSlider::setSinkImpl( QMediaPlayer *sink )
{
    QMediaPlayer *oldSink( dynamic_cast<QMediaPlayer*>( sink_ ) );

    // disconnect from old sink
    if ( oldSink )
        disconnect( oldSink, 0, this, 0 );

    // setup new sink
    if ( (sink_ = dynamic_cast<audioSinkType*>( sink )) )
    {
        // connect to new object
        connect( sink, SIGNAL( audioAvailableChanged(bool) ), slider_, SLOT( setEnabled(bool) ) );
        connect( sink, SIGNAL( audioAvailableChanged(bool) ), mute_, SLOT( setEnabled(bool) ) );
        connect( sink, SIGNAL( mutedChanged(bool) ), SLOT( updateMuted(bool) ) );
        connect( sink, SIGNAL( volumeChanged(int) ), SLOT( updateVolume(int) ) );

        slider_->setEnabled( sink->isAudioAvailable() );
        mute_->setEnabled( sink->isAudioAvailable() );
        updateMuted( sink->isMuted() );
        updateVolume( sink->volume() );
    }
}

template <>
void volumeSlider::setVolumeImpl( QMediaPlayer *sink, qreal value )
{
    if ( sink )
        sink->setVolume( (int)(100.0 * value) );
}

template <>
void volumeSlider::toggleMuteImpl( QMediaPlayer *sink )
{
    if ( sink )
        sink->setMuted( sink->isMuted() ? false : true );
}

template <>
void volumeSlider::pollImpl( QMediaPlayer *sink )
{
    Q_UNUSED( sink );

    // nothing to do, handle by signals/slots
}

#else
// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
// ================================================================================================
//
// Qt4 Phonon Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

template <>
void volumeSlider::setSinkImpl( Phonon::AudioOutput *sink )
{
    Phonon::AudioOutput *oldSink( dynamic_cast<Phonon::AudioOutput*>( sink_ ) );

    // disconnect from old sink
    if ( oldSink )
        disconnect( oldSink, 0, this, 0 );

    // setup new sink
    if ( (sink_ = dynamic_cast<audioSinkType*>( sink )) )
    {
        // connect to new object
        connect( sink, SIGNAL( mutedChanged(bool) ), SLOT( updateMuted(bool) ) );
        connect( sink, SIGNAL( volumeChanged(qreal) ), SLOT( updateVolume(qreal) ) );

        slider_->setEnabled( sink->isValid() );
        mute_->setEnabled( sink->isValid() );
        updateMuted( sink->isMuted() );
        updateVolume( sink->volume() );
    }
}

template <>
void volumeSlider::setVolumeImpl( Phonon::AudioOutput *sink, qreal value )
{
    if ( sink )
        sink->setVolume( value );
}

template <>
void volumeSlider::toggleMuteImpl( Phonon::AudioOutput *sink )
{
    if ( sink )
        sink->setMuted( sink->isMuted() ? false : true );
}

template <>
void volumeSlider::pollImpl( Phonon::AudioOutput *sink )
{
    Q_UNUSED( sink );

    // nothing to do, handle by signals/slots
}

#endif

#ifdef Q_OS_WIN
// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
// ================================================================================================
//
// Qt4 WMF Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

template <>
void volumeSlider::setSinkImpl( IMFSimpleAudioVolume *sink )
{
    pollTimer_->stop();

    if ( (sink_ = dynamic_cast<audioSinkType*>( sink )) )
    {
        slider_->setEnabled( true );
        mute_->setEnabled( true );

        pollTimer_->setInterval( 100 );
        pollTimer_->start();
    }
    else
    {
        slider_->setEnabled( false );
        mute_->setEnabled( false );
    }
}

template <>
void volumeSlider::setVolumeImpl( IMFSimpleAudioVolume *sink, qreal value )
{
    LOG_CLASS_INFO( "volumeSlider", "setVolumeImpl" );

    if ( !sink )
        return;

    HRESULT hr( sink->SetMasterVolume( (float) value ) );

    if ( FAILED( hr ) )
        LOG_WARN( "failed to set volume 0x%08x", hr );
}

template <>
void volumeSlider::toggleMuteImpl( IMFSimpleAudioVolume *sink )
{
    LOG_CLASS_INFO( "volumeSlider", "toggleMuteImpl" );

    if ( !sink )
        return;

    BOOL mute;
    HRESULT hr;

    if ( FAILED( (hr = sink->GetMute( &mute )) ) )
        LOG_WARN( "failed to get mute 0x%08x", hr );
    else if ( FAILED( (hr = sink->SetMute( mute ? FALSE : TRUE )) ) )
        LOG_WARN( "failed to set mute 0x%08x", hr );
}

template <>
void volumeSlider::pollImpl( IMFSimpleAudioVolume *sink )
{
    LOG_CLASS_INFO( "volumeSlider", "pollImpl" );

    if ( !sink )
        return;

    float volume;
    BOOL mute;

    HRESULT hr;

    // set volume
    if ( FAILED( (hr = sink->GetMasterVolume( &volume )) ) )
        LOG_WARN( "failed to get volume 0x%08x", hr );
    else
    {
        updateVolume( (qreal) volume );
    }

    // set muted
    if ( FAILED( (hr = sink->GetMute( &mute )) ) )
        LOG_WARN( "failed to get mute 0x%08x", hr );
    else
    {
        updateMuted( mute );
    }
}

#endif

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
// ================================================================================================
//
// GST Process Daemon Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

template <>
void volumeSlider::setSinkImpl( dbusMediaPlayerClient *sink )
{
    dbusMediaPlayerClient *oldSink( dynamic_cast<dbusMediaPlayerClient*>( sink_ ) );

    // disconnect from old sink
    if ( oldSink )
        disconnect( oldSink, 0, this, 0 );

    // setup new sink
    if ( (sink_ = dynamic_cast<audioSinkType*>( sink )) )
    {
        // connect to new object
        connect( sink, SIGNAL( audioAvailableChanged(bool) ), slider_, SLOT( setEnabled(bool) ) );
        connect( sink, SIGNAL( audioAvailableChanged(bool) ), mute_, SLOT( setEnabled(bool) ) );
        connect( sink, SIGNAL( mutedChanged(bool) ), SLOT( updateMuted(bool) ) );
        connect( sink, SIGNAL( volumeChanged(int) ), SLOT( updateVolume(int) ) );

        slider_->setEnabled( sink->isAudioAvailable() );
        mute_->setEnabled( sink->isAudioAvailable() );
        updateMuted( sink->isMuted() );
        updateVolume( sink->volume() );
    }
}

template <>
void volumeSlider::setVolumeImpl( dbusMediaPlayerClient *sink, qreal value )
{
    if ( sink )
        sink->setVolume( (int)(100.0 * value) );
}

template <>
void volumeSlider::toggleMuteImpl( dbusMediaPlayerClient *sink )
{
    if ( sink )
        sink->setMuted( sink->isMuted() ? false : true );
}

template <>
void volumeSlider::pollImpl( dbusMediaPlayerClient *sink )
{
    Q_UNUSED( sink );

    // nothing to do, handle by signals/slots
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // VOLUMESLIDERIMPL_H
