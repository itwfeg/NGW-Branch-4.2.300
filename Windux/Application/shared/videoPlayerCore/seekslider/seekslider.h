/**
 * @file seekslider.h
 * Slider to handle seeking for multimedia playback.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef SEEKSLIDER_H
#define SEEKSLIDER_H

#include "../videoplayer/videoplayerconfig.h"

#include <QWidget>

class QSlider;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Slider to handle seeking for multimedia playback.
class seekSlider : public QWidget
{
    Q_OBJECT

    typedef seekSlider _Myt;
    typedef QWidget _Mybase;

    typedef videoPlayerConfig<PLATFORM>::videoPlayerType _Player;

public:

    /// Source type.
    typedef _Player::sourceType sourceType;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in]  parent  parent widget
     */
    seekSlider( QWidget *parent = NULL );

    /// Destructor.
    virtual ~seekSlider();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve media object associated with slider.
    /**
     * @return  pointer to media object
     */
    virtual sourceType *mediaObject() const {return mediaObject_;}

    /// Set the media object associated with slider.
    /**
     * @param[in] value  pointer to media object
     */
    virtual void setMediaObject( void *value );

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in] obj  pointer to object
     * @param[in] e  pointer to event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

private Q_SLOTS:

    /// Slot to handle device polling
    void onPoll();

    /// Slot to handle user seeking to new position.
    void onSliderMoved( int pos );

    /// Slot to handle slider pressed.
    void onSliderPressed();

    /// Slot to handle slider released.
    void onSliderReleased();

    /// Slot to handle media duration changes.
    void updateDuration( qint64 duration );

    /// Slot to handle media position changes.
    void updatePosition( qint64 pos );

    /// Slot to handle seekability changes.
    void updateSeekable( bool canSeek );

private:

    sourceType *mediaObject_;

    QSlider *slider_;

    qint64 duration_;
    qint64 pos_;

    bool blocking_;

    QTimer *pollTimer_;

    // ========================================================================

    // not implemented
    seekSlider( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update slider range.
    void updateSliderRange();

    /// Update slider position.
    void updateSliderPosition();

    /// Set media object sink device.
    template <class T>
    void setSinkImpl( T *sink );

    /// Set position.
    template <class T>
    void setPositionImpl( T *sink, qreal value );

    /// Poll sink device.
    template <class T>
    void pollImpl( T *sink );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SEEKSLIDER_H
