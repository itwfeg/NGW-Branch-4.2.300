/**
 * @file marqueelabelwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "marqueelabelwidget.h"

#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeLabelWidget::MarqueeLabelWidget( QWidget *parent ) :
    QLabel( parent ),
    textListIndex_( 0 ),
    direction_( (Direction) DEFAULT_DIRECTION ),
    scrollType_( (ScrollType) DEFAULT_SCROLL_TYPE ),
    scrollState_( Entry ),
    scrollSpeed_( marqueeText::SCROLL_SPEED_NORMAL ),
    blinkTimer_( 0 ),
    holdTimer_( 0 )
{
    if ( objectName().isEmpty() )
        setObjectName( "MarqueeLabelWidget" );

    connect(
        &timer_,
        SIGNAL( timeout() ),
        this,
        SLOT( onTimeout() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeLabelWidget::~MarqueeLabelWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::setAlignment( Qt::Alignment value )
{
    _Mybase::setAlignment( value );
    resetPosition();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::setFont( const QFont& value )
{
    _Mybase::setFont( value );

    // reset text list due to font change
    if ( textWithoutMarkup_.length() )
    {
        textList_ = marqueeTextList::create( textWithoutMarkup_, fontMetrics() );
        textListIndex_ = 0;

        displayNextMarquee();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::setText( const QString& value )
{
    const QString trimmedValue( value.trimmed() );

    QString realValue;

    if ( trimmedValue.length() )
        realValue = value;

    if ( realValue != textWithoutMarkup_ )
    {
        textWithoutMarkup_ = realValue;

        if ( textWithoutMarkup_.isEmpty() )
            timer_.stop();
        else
            timer_.start( TIMEOUT );

        textList_ = marqueeTextList::create( realValue, fontMetrics() );
        textListIndex_ = 0;

        displayNextMarquee();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::setDirection( Direction value )
{
    direction_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::setScrollSpeed( int value )
{
    scrollSpeed_ = abs( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::setScrollType( ScrollType value )
{
    if ( scrollType_ != value )
    {
        scrollType_ = value;
        resetPosition();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::resetPosition()
{
    if ( textWithoutMarkup_.isEmpty() )
        return;

    // calculate locations
    // the frame location is first because other locations may use it
    frameLocation_ = calculateFrameLocation();
    beginLocation_ = calculateBeginLocation();
    endLocation_ = calculateEndLocation();

    position_ = beginLocation();
    scrollState_ = Entry;

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::paintEvent( QPaintEvent *evt )
{
    Q_UNUSED( evt );

    if ( textWithoutMarkup_.isEmpty() )
        return;

    if ( DoneHide != scrollState_ )
    {
        QPainter p( this );

        text_.paint( &p, position_, blinkTextVisible() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::resizeEvent( QResizeEvent *evt )
{
    _Mybase::resizeEvent( evt );
    resetPosition();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::displayNextMarquee()
{
    if ( textList_.size() )
    {
        setMarquee( textList_.at( textListIndex_ ) );

        if ( ++textListIndex_ >= textList_.size() )
            textListIndex_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::setMarquee( const marqueeText& value )
{
    text_ = value;
    _Mybase::setText( text_.text() );

    // update hold timer
    holdTimer_ = HOLD_TIME * text_.holdTime( marqueeText::BeforeEntry );

    // update speed
    setScrollSpeed( text_.scrollSpeed() );

    // update position
    resetPosition();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
QPoint MarqueeLabelWidget::calculateBeginLocationT<MarqueeLabelWidget::Always, MarqueeLabelWidget::RightToLeft>() const
{
    int x = width();
    int y = height() - text_.textSize().height();

    if ( Qt::AlignVCenter & alignment() )
        y /= 2;
    else if ( Qt::AlignTop & alignment() )
        y = 0;

    return QPoint( x, y );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
QPoint MarqueeLabelWidget::calculateBeginLocationT<MarqueeLabelWidget::Always, MarqueeLabelWidget::LeftToRight>() const
{
    int x = -text_.textSize().width();
    int y = height() - text_.textSize().height();

    if ( Qt::AlignVCenter & alignment() )
        y /= 2;
    else if ( Qt::AlignTop & alignment() )
        y = 0;

    return QPoint( x, y );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
QPoint MarqueeLabelWidget::calculateBeginLocationT<MarqueeLabelWidget::Always, MarqueeLabelWidget::BottomToTop>() const
{
    int x = width() - text_.textSize().width();
    int y = height();

    if ( Qt::AlignHCenter & alignment() )
        x /= 2;
    else if ( Qt::AlignLeft & alignment() )
        x = 0;

    return QPoint( x, y );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
QPoint MarqueeLabelWidget::calculateBeginLocationT<MarqueeLabelWidget::Always, MarqueeLabelWidget::TopToBottom>() const
{
    int x = width() - text_.textSize().width();
    int y = -text_.textSize().height();

    if ( Qt::AlignHCenter & alignment() )
        x /= 2;
    else if ( Qt::AlignLeft & alignment() )
        x = 0;

    return QPoint( x, y );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
QPoint MarqueeLabelWidget::calculateBeginLocationT<MarqueeLabelWidget::OnlyWhenLarger, MarqueeLabelWidget::RightToLeft>() const
{
    QPoint point = frameLocation();

    if ( text_.textSize().width() > width() )
        point.setX( 0 );

    return point;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
QPoint MarqueeLabelWidget::calculateBeginLocationT<MarqueeLabelWidget::OnlyWhenLarger, MarqueeLabelWidget::LeftToRight>() const
{
    QPoint point = frameLocation();

    if ( text_.textSize().width() > width() )
        point.setX( width() - text_.textSize().width() );

    return point;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
QPoint MarqueeLabelWidget::calculateBeginLocationT<MarqueeLabelWidget::OnlyWhenLarger, MarqueeLabelWidget::BottomToTop>() const
{
    QPoint point = frameLocation();

    if ( text_.textSize().height() > height() )
        point.setY( 0 );

    return point;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
QPoint MarqueeLabelWidget::calculateBeginLocationT<MarqueeLabelWidget::OnlyWhenLarger, MarqueeLabelWidget::TopToBottom>() const
{
    QPoint point = frameLocation();

    if ( text_.textSize().height() > height() )
        point.setY( height() - text_.textSize().height() );

    return point;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPoint MarqueeLabelWidget::calculateBeginLocation() const
{
    if ( Always == scrollType() )
    {
        if ( marqueeText::Frame == text_.entryBehavior() )
            return calculateFrameLocation();

        if ( RightToLeft == direction() )
            return calculateBeginLocationT<Always, RightToLeft>();
        else if ( LeftToRight == direction() )
            return calculateBeginLocationT<Always, LeftToRight>();
        else if ( BottomToTop == direction() )
            return calculateBeginLocationT<Always, BottomToTop>();
        else
            return calculateBeginLocationT<Always, TopToBottom>();
    }
    else if ( OnlyWhenLarger == scrollType() )
    {
        if ( RightToLeft == direction() )
            return calculateBeginLocationT<OnlyWhenLarger, RightToLeft>();
        else if ( LeftToRight == direction() )
            return calculateBeginLocationT<OnlyWhenLarger, LeftToRight>();
        else if ( BottomToTop == direction() )
            return calculateBeginLocationT<OnlyWhenLarger, BottomToTop>();
        else
            return calculateBeginLocationT<OnlyWhenLarger, TopToBottom>();
    }

    return QPoint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPoint MarqueeLabelWidget::calculateFrameLocation() const
{
    int x = width() - text_.textSize().width();
    int y = height() - text_.textSize().height();

    if ( Qt::AlignHCenter & alignment() )
        x /= 2;
    else if ( Qt::AlignLeft & alignment() )
        x = 0;

    if ( Qt::AlignVCenter & alignment() )
        y /= 2;
    else if ( Qt::AlignTop & alignment() )
        y = 0;

    return QPoint( x, y );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPoint MarqueeLabelWidget::calculateEndLocation() const
{
    if ( Always == scrollType() )
    {
        if ( marqueeText::Clear == text_.exitBehavior() )
            return calculateFrameLocation();

        if ( RightToLeft == direction() )
            return calculateBeginLocationT<Always, LeftToRight>();
        else if ( LeftToRight == direction() )
            return calculateBeginLocationT<Always, RightToLeft>();
        else if ( BottomToTop == direction() )
            return calculateBeginLocationT<Always, TopToBottom>();
        else
            return calculateBeginLocationT<Always, BottomToTop>();
    }
    else if ( OnlyWhenLarger == scrollType() )
    {
        if ( RightToLeft == direction() )
            return calculateBeginLocationT<OnlyWhenLarger, LeftToRight>();
        else if ( LeftToRight == direction() )
            return calculateBeginLocationT<OnlyWhenLarger, RightToLeft>();
        else if ( BottomToTop == direction() )
            return calculateBeginLocationT<OnlyWhenLarger, TopToBottom>();
        else
            return calculateBeginLocationT<OnlyWhenLarger, BottomToTop>();
    }

    return QPoint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MarqueeLabelWidget::checkPositionPast( const QPoint& point ) const
{
    bool result;

    int center;
    int location;

    if (( RightToLeft == direction() ) || ( LeftToRight == direction() ))
    {
        center = point.x();
        location = position_.x();
    }
    else
    {
        center = point.y();
        location = position_.y();
    }

    if (( RightToLeft == direction() ) || ( BottomToTop == direction() ))
        result = (location <= center);
    else
        result = (location >= center);

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeLabelWidget::onTimeout()
{
    // update blink timer
    blinkTimer_ += TIMEOUT;

    if ( blinkTimer_ >= BLINK_TIME )
        blinkTimer_ = -BLINK_TIME;

    // nothing to do when no text
    if ( textWithoutMarkup_.isEmpty() )
        return;

    // update hold timer
    if ( holdTimer_ > 0 )
    {
        holdTimer_ -= TIMEOUT;
        return;
    }

    // repaint the screen
    update();

    // check for frame position
    if (( Entry == scrollState_ ) && ( checkPositionPast( frameLocation() ) ))
    {
        holdTimer_ = HOLD_TIME * text_.holdTime( marqueeText::DuringFrame );
        scrollState_ = Exit;

        if ( holdTimer_ )
            return;
    }

    // check for end position
    if ( checkPositionPast( endLocation() ) )
    {
        if ( Exit == scrollState_ )
        {
            holdTimer_ = HOLD_TIME * text_.holdTime( marqueeText::AfterExit );

            if ( marqueeText::Clear == text_.exitBehavior() )
                scrollState_ = DoneHide;
            else
                scrollState_ = Done;

            if ( holdTimer_ )
                return;
        }

        if (( Done == scrollState_ ) || ( DoneHide == scrollState_ ))
        {
            // grab next marquee
            displayNextMarquee();
            return;
        }
    }

    // update position
    if ( RightToLeft == direction() )
        position_.setX( position_.x() - scrollSpeed() );
    else if ( LeftToRight == direction() )
        position_.setX( position_.x() + scrollSpeed() );
    else if ( BottomToTop == direction() )
        position_.setY( position_.y() - scrollSpeed() );
    else if ( TopToBottom == direction() )
        position_.setY( position_.y() + scrollSpeed() );
}
