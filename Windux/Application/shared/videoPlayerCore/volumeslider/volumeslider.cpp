/**
 * @file volumeslider.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "volumeslider.h"
#include "volumesliderimpl.h"

#include <logmanager.h>

#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
volumeSlider::volumeSlider( QWidget *parent) :
    _Mybase( parent ),
    sink_( NULL ),
    blocking_( false )
{
    setObjectName( "volumeSlider" );

    // initialize
    initialize();

    // create layouts
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
volumeSlider::~volumeSlider()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize volumeSlider::iconSize() const
{
    return mute_->iconSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::setAudioOutput( void *value )
{
    // nothing to do
    if ( sink_ == value )
        return;

    setSinkImpl( reinterpret_cast<audioSinkType*>( value ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::setIconSize( const QSize& value )
{
    mute_->setIconSize( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool volumeSlider::eventFilter( QObject *obj, QEvent *e )
{
    if (( slider_ == obj ) && ( slider_->isEnabled() ))
        if ( QEvent::MouseButtonPress == e->type() )
        {
            QMouseEvent *event( dynamic_cast<QMouseEvent*>( e ) );

            if (( event ) && ( Qt::LeftButton == event->button() ))
                if ( sink_ )
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

                    setVolumeImpl( sink_, value );
                    event->accept();
                }
        }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::onMuteClicked()
{
    toggleMuteImpl( sink_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::onPoll()
{
    pollImpl( sink_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::onSliderMoved( int pos )
{
    qreal volume( (qreal)(pos - slider_->minimum()) / (qreal)(slider_->maximum() - slider_->minimum()) );

    setVolumeImpl( sink_, volume );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::onSliderPressed()
{
    blocking_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::onSliderReleased()
{
    blocking_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::updateMuted( bool muted )
{
    mute_->setIcon( style()->standardIcon( muted ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::updateVolume( qreal volume )
{
    if ( blocking_ )
        return;

    int pos( slider_->minimum() + (volume * (qreal)(slider_->maximum() - slider_->minimum())) );

    slider_->setValue( pos );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::updateVolume( int volume )
{
    updateVolume( (qreal)( volume ) / (qreal)( 100.0 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::initialize()
{
    slider_ = new QSlider( Qt::Horizontal, this );
    slider_->setEnabled( false );
    slider_->setPageStep( 0 );
    slider_->setRange( 0, 100 );
    slider_->setSingleStep( 0 );
    slider_->setTickPosition( QSlider::NoTicks );

    slider_->installEventFilter( this );

    mute_ = new QToolButton( this );
    mute_->setEnabled( false );

    // connect signals
    connect( slider_, SIGNAL( sliderMoved(int) ), SLOT( onSliderMoved(int) ) );
    connect( slider_, SIGNAL( sliderPressed() ), SLOT( onSliderPressed() ) );
    connect( slider_, SIGNAL( sliderReleased() ), SLOT( onSliderReleased() ) );
    connect( mute_, SIGNAL( clicked() ), SLOT( onMuteClicked() ) );

    // create timer
    pollTimer_ = new QTimer( this );

    connect( pollTimer_, SIGNAL( timeout() ), SLOT( onPoll() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void volumeSlider::createLayout()
{
    QHBoxLayout *widgets( new QHBoxLayout() );
    widgets->setContentsMargins( QMargins() );

    widgets->addWidget( mute_ );
    widgets->addWidget( slider_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->setContentsMargins( QMargins() );
    layout->setSpacing( 0 );

    layout->addStretch();
    layout->addLayout( widgets, 1 );
    layout->addStretch();
}
