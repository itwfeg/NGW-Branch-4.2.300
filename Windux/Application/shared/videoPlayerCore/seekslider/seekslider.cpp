/**
 * @file seekslider.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "seekslider.h"
#include "seeksliderimpl.h"

#include <QEvent>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
seekSlider::seekSlider( QWidget *parent ) :
    _Mybase( parent ),
    mediaObject_( NULL ),
    duration_( 0 ),
    pos_( 0 ),
    blocking_( false )
{
    setObjectName( "seekSlider" );

    // initialize
    initialize();

    // create layouts
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
seekSlider::~seekSlider()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::setMediaObject( void *value )
{
    // nothing to do
    if ( mediaObject_ == value )
        return;

    setSinkImpl( reinterpret_cast<sourceType*>( value ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool seekSlider::eventFilter( QObject *obj, QEvent *e )
{
    if (( slider_ == obj ) && ( slider_->isEnabled() ))
        if ( QEvent::MouseButtonPress == e->type() )
        {
            QMouseEvent *event( dynamic_cast<QMouseEvent*>( e ) );

            if (( event ) && ( Qt::LeftButton == event->button() ))
                if ( mediaObject_ )
                {
                    QStyleOptionSlider opt;
                    opt.initFrom( slider_ );

                    QRect sr( style()->subControlRect( QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, slider_ ) );

                    qreal halfHandleWidth( (0.5 * sr.width()) + 0.5 );
                    qreal value( 0.0 );

                    if ( halfHandleWidth < event->x() )
                    {
                        if ( event->x() <= (slider_->width() - halfHandleWidth) )
                            value = ((qreal)(event->x() - halfHandleWidth) / (qreal)(slider_->width() - sr.width()));
                        else
                            value = 1.0;
                    }

                    setPositionImpl( mediaObject_, value );
                    event->accept();
                }
        }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::onPoll()
{
    pollImpl( mediaObject_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::onSliderMoved( int pos )
{
    qreal location( (qreal)(pos - slider_->minimum()) / (qreal)(slider_->maximum() - slider_->minimum()) );

    setPositionImpl( mediaObject_, location );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::onSliderPressed()
{
    blocking_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::onSliderReleased()
{
    blocking_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::updateDuration( qint64 duration )
{
    duration_ = duration;
    updateSliderRange();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::updatePosition( qint64 pos )
{
    if ( blocking_ )
        return;

    pos_ = pos;
    updateSliderPosition();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::updateSeekable( bool canSeek )
{
    slider_->setEnabled( canSeek );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::initialize()
{
    slider_ = new QSlider( Qt::Horizontal, this );
    slider_->setEnabled( false );
    slider_->setPageStep( 0 );
    slider_->setSingleStep( 0 );
    slider_->setTickPosition( QSlider::NoTicks );

    slider_->installEventFilter( this );

    // connect signals
    connect( slider_, SIGNAL( sliderMoved(int) ), SLOT( onSliderMoved(int) ) );
    connect( slider_, SIGNAL( sliderPressed() ), SLOT( onSliderPressed() ) );
    connect( slider_, SIGNAL( sliderReleased() ), SLOT( onSliderReleased() ) );

    // create timer
    pollTimer_ = new QTimer( this );

    connect( pollTimer_, SIGNAL( timeout() ), SLOT( onPoll() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::createLayout()
{
    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->setContentsMargins( QMargins() );
    layout->setSpacing( 0 );

    layout->addStretch();
    layout->addWidget( slider_, 1 );
    layout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::updateSliderRange()
{
    slider_->setRange( 0, (int) qMin( (qint64) INT_MAX, duration_ ) );
    updateSliderPosition();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void seekSlider::updateSliderPosition()
{
    int newPos;

    if ( duration_ <= (qint64) INT_MAX )
        newPos = (int) pos_;
    else
    {
        qreal temp( (qreal) pos_ );
        temp /= (qreal) duration_;
        temp *= (qreal) INT_MAX;

        newPos = (int) temp;
    }

    slider_->setValue( newPos );
    update();
}
