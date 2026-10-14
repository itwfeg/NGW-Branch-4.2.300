/**
 * @file seeksliderimpl.h
 * Slider to handle seeking for multimedia playback.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef SEEKSLIDERIMPL_H
#define SEEKSLIDERIMPL_H

#include "seekslider.h"

#include <QSlider>
#include <QTimer>

#if ( QT_VERSION >= 0x050000 )
// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
// ================================================================================================
//
// Qt5 QtMultimedia Framework Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

template <>
void seekSlider::setSinkImpl( QMediaPlayer *sink )
{
    QMediaPlayer *oldSink( dynamic_cast<QMediaPlayer*>( mediaObject_ ) );

    // disconnect from old sink
    if ( oldSink )
        disconnect( oldSink, 0, this, 0 );

    // connect to new object
    if ( (mediaObject_ = dynamic_cast<sourceType*>( sink )) )
    {
        connect( sink, SIGNAL( durationChanged(qint64) ), SLOT( updateDuration(qint64) ) );
        connect( sink, SIGNAL( positionChanged(qint64) ), SLOT( updatePosition(qint64) ) );
        connect( sink, SIGNAL( seekableChanged(bool) ), SLOT( updateSeekable(bool) ) );

        duration_ = sink->duration();
        pos_ = sink->position();
        updateSeekable( sink->isSeekable() );
        updateSliderRange();
    }
}

template <>
void seekSlider::setPositionImpl( QMediaPlayer *sink, qreal value )
{
    if ( sink )
        sink->setPosition( (qint64)(value * (qreal) sink->duration()) );
}

template <>
void seekSlider::pollImpl( QMediaPlayer *sink )
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
void seekSlider::setSinkImpl( Phonon::MediaObject *sink )
{
    Phonon::MediaObject *oldSink( dynamic_cast<Phonon::MediaObject*>( mediaObject_ ) );

    // disconnect from old sink
    if ( oldSink )
        disconnect( oldSink, 0, this, 0 );

    // connect to new object
    if ( (mediaObject_ = dynamic_cast<sourceType*>( sink )) )
    {
        connect( sink, SIGNAL( totalTimeChanged(qint64) ), SLOT( updateDuration(qint64) ) );
        connect( sink, SIGNAL( tick(qint64) ), SLOT( updatePosition(qint64) ) );
        connect( sink, SIGNAL( seekableChanged(bool) ), SLOT( updateSeekable(bool) ) );

        duration_ = sink->totalTime();
        pos_ = sink->currentTime();
        updateSeekable( sink->isSeekable() );
        updateSliderRange();
    }
}

template <>
void seekSlider::setPositionImpl( Phonon::MediaObject *sink, qreal value )
{
    if ( !sink )
        return;

    qint64 pos( (qint64)(value * (qreal) sink->totalTime()) );

    sink->seek( pos );

    // update slider when in stopped state
    if ( Phonon::StoppedState == sink->state() )
        updatePosition( pos );
}

template <>
void seekSlider::pollImpl( Phonon::MediaObject *sink )
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
void seekSlider::setSinkImpl( mediaFoundationSource *sink )
{
    mediaFoundationSource *oldSink( dynamic_cast<mediaFoundationSource*>( mediaObject_ ) );

    // disconnect from old sink
    if ( oldSink )
        disconnect( oldSink, 0, this, 0 );

    // connect to new object
    if ( (mediaObject_ = dynamic_cast<sourceType*>( sink )) )
    {
        connect( sink, SIGNAL( durationChanged(qint64) ), SLOT( updateDuration(qint64) ) );
        connect( sink, SIGNAL( positionChanged(qint64) ), SLOT( updatePosition(qint64) ) );
        connect( sink, SIGNAL( seekableChanged(bool) ), SLOT( updateSeekable(bool) ) );

        duration_ = sink->duration();
        pos_ = sink->position();
        updateSeekable( sink->isSeekable() );
        updateSliderRange();
    }
}

template <>
void seekSlider::setPositionImpl( mediaFoundationSource *sink, qreal value )
{
    if ( sink )
        sink->setPosition( (qint64)(value * (qreal) sink->duration()) );
}

template <>
void seekSlider::pollImpl( mediaFoundationSource *sink )
{
    Q_UNUSED( sink );

    // nothing to do, handle by signals/slots
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
void seekSlider::setSinkImpl( dbusMediaPlayerClient *sink )
{
    dbusMediaPlayerClient *oldSink( dynamic_cast<dbusMediaPlayerClient*>( mediaObject_ ) );

    // disconnect from old sink
    if ( oldSink )
        disconnect( oldSink, 0, this, 0 );

    // connect to new object
    if ( (mediaObject_ = dynamic_cast<sourceType*>( sink )) )
    {
        // connect to new object
        connect( sink, SIGNAL( durationChanged(qint64) ), SLOT( updateDuration(qint64) ) );
        connect( sink, SIGNAL( positionChanged(qint64) ), SLOT( updatePosition(qint64) ) );
        connect( sink, SIGNAL( seekableChanged(bool) ), SLOT( updateSeekable(bool) ) );

        duration_ = sink->duration();
        pos_ = sink->position();
        updateSeekable( sink->isSeekable() );
        updateSliderRange();
    }
}

template <>
void seekSlider::setPositionImpl( dbusMediaPlayerClient *sink, qreal value )
{
    if ( sink )
        sink->setPosition( (qint64)(value * (qreal) sink->duration()) );
}

template <>
void seekSlider::pollImpl( dbusMediaPlayerClient *sink )
{
    Q_UNUSED( sink );

    // nothing to do, handle by signals/slots
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SEEKSLIDERIMPL_H
