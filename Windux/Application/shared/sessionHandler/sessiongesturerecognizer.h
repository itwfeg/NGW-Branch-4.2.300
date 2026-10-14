/**
 * @file sessiongesturerecognizer.h
 * Filter for converting mouse events into gestures.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SESSIONGESTURERECOGNIZER_H
#define SESSIONGESTURERECOGNIZER_H

#include <QGestureRecognizer>
#include <QList>
#include <QPoint>
#include <QSize>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Filter for converting mouse events into gestures.
class SessionGestureRecognizer : public QGestureRecognizer
{
    typedef SessionGestureRecognizer _Myt;
    typedef QGestureRecognizer _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit SessionGestureRecognizer();

    /// Destructor.
    virtual ~SessionGestureRecognizer();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create gesture object.
    /**
     * This function is called by Qt to create a new @c QGesture object for the given @p target
     * (@c QWidget or @p QGraphicsObject).
     *
     * The application takes ownership of the created gesture object.
     *
     * @param[in,out] target  target object
     * @return  gesture object
     */
    virtual QGesture *create( QObject *target );

    /// Recognize gesture.
    /**
     * Handles the given @p event for the @p watched object, updating the state of the @p gesture
     * object as required, and returns a suitable result for the current recognition step.
     *
     * This function is called by the framework to allow the recognizer to filter input events
     * dispatched to @c QWidget or @c QGraphicsObject instances that it is monitoring.
     *
     * The result reflects how much of the gesture has been recognized. The state of the @p gesture
     * object is set depending on the result.
     *
     * @param[in,out] gesture  gesture object to update
     * @param[in,out] watched  object being watched
     * @param[in,out] event  event to filter
     * @return  gesture result
     */
    virtual Result recognize( QGesture *gesture, QObject *watched, QEvent *event );

    /// Reset gesture.
    /**
     * This function is called by the framework to reset a given @p gesture.
     *
     * @param[in,out] gesture  gesture to reset
     */
    virtual void reset( QGesture *gesture );

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Cancel active gesture.
    virtual void cancel();

private:

    QSize resolution_;

    bool cancel_;

    static const int swipeMilliseconds_;

    static const int swipeSpeedMin_;
    static const int swipeSpeedMax_;

    static const int hotSpotHeight_;
    static const int hotSpotWidth_;

    /// Check distance and speed between start and end points.
    bool isValidSwipe( const QPoint& start, const QPoint& end, int distance, int speed, qreal& angle ) const;

    /// Check for gesture by pressing hot spot.
    bool isValidPress( const QPoint& start, const QPoint& end, int distance, int speed, qreal& angle ) const;

    /// Prepare widget for swipe.
    template <class T>
    void prepareForSwipe( T* /*widget*/ ) {}

    /// Prepare list of widgets for swipe.
    template <class T>
    void prepareForSwipe( QList<T*> widgets ) {
        foreach ( T *w, widgets )
            prepareForSwipe( w );
    }

    /// Prepare for swipe.
    void updateSwipedObject( QObject *watched );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SESSIONGESTURERECOGNIZER_H
