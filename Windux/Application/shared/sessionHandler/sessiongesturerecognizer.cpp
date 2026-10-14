/**
 * @file sessiongesturerecognizer.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "sessiongesturerecognizer.h"

#include <pushbutton.h>
#include <toolbutton.h>

#include <QAbstractButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QGesture>
#include <QLineF>
#include <QMouseEvent>

// holding the mouse down longer than this will cancel the swipe
const int SessionGestureRecognizer::swipeMilliseconds_ = 500;

// you must swipe faster than this speed (pixels/second) or it is not a swipe
const int SessionGestureRecognizer::swipeSpeedMin_ = 400;

// Note:
// The purpose of this value is to rule out bad/errant touch screens. During development of
// fastrack we had touch drivers that would jump the mouse to (0,0) while in the middle of a
// swipe. This would cause huge swipe speeds usually at at 45 degree angle.
//
// you must swipe slower than this speed (pixels/second) or it is not a swipe
const int SessionGestureRecognizer::swipeSpeedMax_ = 5000;

// Note:
// Instead of swiping one may press the hot spot to trigger a swipe. The idea is to press
// offscreen and the touch controller will press the last pixel on the edge of the screen. So
// we can detect mouse presses in the very last pixel and check it against our hot spot locations.
//
// height of a hot spot (pixels)
const int SessionGestureRecognizer::hotSpotHeight_ = 2;

// width of a hot spot (pixels)
const int SessionGestureRecognizer::hotSpotWidth_ = 100;

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionGestureRecognizer::SessionGestureRecognizer() :
    _Mybase(),
    cancel_( false )
{
    LOG_CLASS_INFO( "SessionGestureRecognizer", "SessionGestureRecognizer" );

    QDesktopWidget *desktop = QApplication::desktop();

    // retrieve screen resolution
    if ( !desktop )
        LOG_WARN( "no desktop found", NULL );
    else
    {
        int screen = desktop->primaryScreen();

        LOG_DEBUG( "primary screen is %d", screen );

        if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
        {
            QRect g( desktop->screenGeometry( screen ) );

            resolution_ = g.size();
            LOG_DEBUG( "screen resolution is %d %d", resolution_.width(), resolution_.height() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionGestureRecognizer::~SessionGestureRecognizer()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QGesture *SessionGestureRecognizer::create( QObject *target )
{
    LOG_CLASS_INFO( "SessionGestureRecognizer", "create" );

    LOG_DEBUG( "creating swipe gesture...", NULL );

    LOG_DEBUG( "gesture distance is %d pixels", QApplication::startDragDistance() );
    LOG_DEBUG( "gesture max time is %d ms", swipeMilliseconds_ );
    LOG_DEBUG( "gesture speed is (%d, %d)", swipeSpeedMin_, swipeSpeedMax_ );

    QGesture *swipeGesture = new QSwipeGesture( target );
    return swipeGesture;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionGestureRecognizer::Result SessionGestureRecognizer::recognize( QGesture *gesture, QObject *watched, QEvent *event )
{
    Q_UNUSED( watched );

    LOG_CLASS_INFO( "SessionGestureRecognizer", "recognize" );

    QSwipeGesture *swipeGesture = static_cast<QSwipeGesture*>( gesture );
    Result result = Ignore;

    // handle event
    if ( QEvent::MouseButtonPress == event->type() )
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );

        // left button only
        if ( Qt::LeftButton == mouseEvent->button() )
        {
            swipeGesture->setProperty( "startPoint", mouseEvent->globalPos() );
            swipeGesture->setProperty( "startTime", QTime::currentTime() );
            result = MayBeGesture;

            LOG_TRACE( "swipe possibly started...", NULL );
            cancel_ = false;
        }
    }
    else if ( QEvent::MouseMove == event->type() )
    {
        QTime timer( swipeGesture->property( "startTime" ).toTime() );

        // check for cancelled gesture
        if ( cancel_ )
        {
            // took too long, swipes are fast!
            result = CancelGesture;

            LOG_TRACE( "swipe canceled manually", NULL );
        }

        // check swipe time
        else if ( timer.isValid() )
        {
            if ( swipeMilliseconds_ < timer.msecsTo( QTime::currentTime() ) )
            {
                // took too long, swipes are fast!
                result = CancelGesture;

                LOG_TRACE( "swipe canceled", NULL );
            }
        }
    }
    else if ( QEvent::MouseButtonRelease == event->type() )
    {
        QPoint start( swipeGesture->property( "startPoint" ).toPoint() );

        // check for cancelled gesture
        if ( cancel_ )
        {
            // took too long, swipes are fast!
            result = CancelGesture;

            LOG_TRACE( "swipe canceled manually", NULL );
        }

        else if ( !start.isNull() )
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );

            QTime timer( swipeGesture->property( "startTime" ).toTime() );
            QPoint end( mouseEvent->globalPos() );

            int distance = (end - start).manhattanLength();
            int msecs = timer.msecsTo( QTime::currentTime() );

            // compute swipe speed
            int speed = 0;

            if ( msecs )
                speed = (distance * 1000) / msecs;

            // compute angle
            qreal angle;

            // check for valid swipe or hot spot press
            if (( isValidSwipe( start, end, distance, speed, angle ) ) ||
                ( isValidPress( start, end, distance, speed, angle ) ))
            {
                // set swipe angle
                swipeGesture->setSwipeAngle( angle );
                result = FinishGesture | ConsumeEventHint;

                LOG_TRACE( "swipe finished %f", angle );
                updateSwipedObject( watched );

                // ignore the mouse event
                event->ignore();
            }

            // cancel the gesture
            else
            {
                result = CancelGesture;

                LOG_TRACE( "swipe canceled", NULL );
            }
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionGestureRecognizer::reset( QGesture *gesture )
{
    static const QVariant invalid( QVariant::Invalid );

    gesture->setProperty( "startPoint", invalid );
    gesture->setProperty( "startTime", invalid );

    _Mybase::reset( gesture );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionGestureRecognizer::cancel()
{
    cancel_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionGestureRecognizer::isValidSwipe( const QPoint& start, const QPoint& end, int distance, int speed, qreal& angle ) const
{
    LOG_CLASS_INFO( "SessionGestureRecognizer", "isValidSwipe" );

    LOG_TRACE( "swipe distance %d speed %d", distance, speed );

    // must have valid length and speed
    if (( QApplication::startDragDistance() <= distance ) && ( swipeSpeedMin_ <= speed ) && ( speed <= swipeSpeedMax_ ))
    {
        QLineF line( start, end );

        // sent return angle
        angle = line.angle();

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionGestureRecognizer::isValidPress( const QPoint& start, const QPoint& end, int distance, int speed, qreal& angle ) const
{
    Q_UNUSED( distance );
    Q_UNUSED( speed );

    LOG_CLASS_INFO( "SessionGestureRecognizer", "isValidPress" );

    // check for valid resolution
    if ( resolution_.isNull() )
        return false;

    LOG_TRACE( "press start (%d, %d) end (%d, %d)", start.x(), start.y(), end.x(), end.y() );

    int x = end.x();
    int y = end.y();

    // check bottom of screen
    if ( (resolution_.height() - hotSpotHeight_) <= y )
    {
        // check for right swipe
        if ( x < hotSpotWidth_ )
        {
            angle = 0.0; //right
            return true;
        }
        // check for left swipe
        else if ( (resolution_.width() - hotSpotWidth_) <= x )
        {
            angle = 180.0; // left
            return true;
        }
    }

    // check right of screen
    // check left of screen
    else if (( x < hotSpotHeight_ ) || ( (resolution_.width() - hotSpotHeight_) <= x ))
    {
        // check for up swipe
        if ( y < hotSpotWidth_ )
        {
            angle = 90.0; // up
            return true;
        }
        // check for down swipe
        else if ( (resolution_.height() - hotSpotWidth_) <= y )
        {
            angle = 270.0; //down
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
inline void SessionGestureRecognizer::prepareForSwipe<QAbstractButton>( QAbstractButton *widget )
{
	LOG_CLASS_INFO( "SessionGestureRecognizer", "prepareForSwipe" );

	// reset any push button stuck in down state
    if (( widget->isVisible() ) && ( widget->isEnabled() ) && ( !widget->isCheckable() ) && ( widget->isDown() ))
    {
        hobart::pushButton *pushButton;
        hobart::toolButton *toolButton;

        LOG_DEBUG( "restoring button 0x%08x [%s]", widget, qPrintable( widget->objectName() ) );

        if ( (pushButton = qobject_cast<hobart::pushButton*>( widget )) )
            pushButton->setDown( false );
        else if ( (toolButton = qobject_cast<hobart::toolButton*>( widget )) )
            toolButton->setDown( false );
        else
            widget->setDown( false );

        widget->update();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionGestureRecognizer::updateSwipedObject( QObject *watched )
{
    prepareForSwipe( watched->findChildren<QAbstractButton*>() );
}
