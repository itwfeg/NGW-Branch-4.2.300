/**
 * @file volumeslider.h
 * Slider to handle multimedia playback volumes.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef VOLUMESLIDER_H
#define VOLUMESLIDER_H

#include "../videoplayer/videoplayerconfig.h"

#include <QWidget>

class QSlider;
class QTimer;
class QToolButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Slider to handle seeking for multimedia playback.
class volumeSlider : public QWidget
{
    Q_OBJECT

    typedef volumeSlider _Myt;
    typedef QWidget _Mybase;

    typedef videoPlayerConfig<PLATFORM>::videoPlayerType _Player;

public:

    /// Audio sink type.
    typedef _Player::audioSinkType audioSinkType;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in]  parent  parent widget
     */
    volumeSlider( QWidget *parent = NULL );

    /// Destructor.
    virtual ~volumeSlider();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve audio output device.
    /**
     * @return  pointer to device
     */
    virtual audioSinkType *audioOutput() const {return sink_;}

    /// Retrieve icon size.
    /**
     * @return  icon size
     */
    virtual QSize iconSize() const;

    /// Set audio output device.
    /**
     * @param[in] value  pointer to device
     */
    virtual void setAudioOutput( void *value );

    /// Set icon size.
    /**
     * @param[in] value  icon size
     */
    virtual void setIconSize( const QSize& value );

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    //// Event filter.
    /**
     * @param[in] obj  pointer to object
     * @param[in] e  pointer to event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

private Q_SLOTS:

    /// Slot to handle mute button clicked.
    void onMuteClicked();

    /// Slot to handle device polling.
    void onPoll();

    /// Slot to handle volume slider moved.
    void onSliderMoved( int pos );

    /// Slot to handle slider pressed.
    void onSliderPressed();

    /// Slot to handle slider released.
    void onSliderReleased();

    /// Slot to handle mute changed.
    void updateMuted( bool muted );

    /// Slot to handle volume changed.
    void updateVolume( qreal volume );

    /// Slot to handle volume changed.
    void updateVolume( int volume );

private:

    audioSinkType *sink_;

    QSlider *slider_;
    QToolButton *mute_;

    bool blocking_;

    QTimer *pollTimer_;

    // ========================================================================

    // not implemented
    volumeSlider( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Set audio output sink device.
    template <class T>
    void setSinkImpl( T *sink );

    /// Set volume.
    template <class T>
    void setVolumeImpl( T *sink, qreal value );

    /// Toggle mute on/off.
    template <class T>
    void toggleMuteImpl( T *sink );

    /// Poll sink device.
    template <class T>
    void pollImpl( T *sink );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // VOLUMESLIDER_H
