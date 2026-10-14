/**
 * @file selfservicetoggledsoftkeybutton.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfservicetoggledsoftkeybutton.h"

#include <backgroundwidgettypes.h>
#include <pixmap.h>

#include <QPainter>

/// Hobart namespace.
namespace hobart {

const Qt::Alignment selfServiceToggledSoftkeyButton::buttonTextAlignment_( Qt::AlignCenter );

const QColor selfServiceToggledSoftkeyButton::buttonTextColor_( hobart::colorPalette::white );
const QColor selfServiceToggledSoftkeyButton::buttonTextColorToggled_( hobart::colorPalette::black );

const int selfServiceToggledSoftkeyButton::buttonFontPointSize_ = 14;

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceToggledSoftkeyButton::selfServiceToggledSoftkeyButton( QWidget *parent ) :
    _Mybase( parent )
{
    // set object name
    setObjectName( "selfServiceToggledSoftkeyButton" );

    // set button properties
    _Mybase::setTextAlignment( buttonTextAlignment_ );
    _Mybase::setTextMargins( QMargins( 5, 5, 5, 5 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceToggledSoftkeyButton::~selfServiceToggledSoftkeyButton()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceToggledSoftkeyButton::setTextAlignment( Qt::Alignment value )
{
    Q_UNUSED( value );

    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceToggledSoftkeyButton::paintEvent( QPaintEvent *e )
{
    // set button properties
    setTextColor( isToggled() ? buttonTextColorToggled_ : buttonTextColor_ );

    // invoke base method
    _Mybase::paintEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceToggledSoftkeyButton::updatePixmaps()
{
    invertPixmap inverter;

    QPixmap pixmap( size() );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    if ( isToggled() )
    {
        QLinearGradient g( 0, 0, 0, height() );

        g.setColorAt( 0.0, gradientColors::lightButton_L0 );
        g.setColorAt( 1.0, gradientColors::lightButton_L1 );

        painter.setBrush( QBrush( g ) );
        painter.setPen( QPen( colorPalette::blue, 4 ) );
    }
    else
    {
        QLinearGradient g( 0, 0, 0, height() );

        g.setColorAt( 0.0, gradientColors::darkButton_L0 );
        g.setColorAt( 0.2121, gradientColors::darkButton_L1 );
        g.setColorAt( 0.5091, gradientColors::darkButton_L2 );
        g.setColorAt( 0.5333, gradientColors::darkButton_L3 );
        g.setColorAt( 1.0, gradientColors::darkButton_L4 );

        painter.setBrush( QBrush( g ) );
        painter.setPen( QPen( colorPalette::white, 2 ) );
    }

    int a = painter.pen().width() / 2;
    painter.drawRoundedRect( QRect( QPoint( 0, 0 ), size() ).adjusted( a, a, -a, -a ), 4, 4 );

    // set new button images
    releasedImage_ = pixmap;
    pressedImage_ = (isToggled() ? pixmap : inverter( pixmap ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *selfServiceToggledSoftkeyButtonFactory::create( QWidget *parent ) const
{
    // create new button
    return new selfServiceToggledSoftkeyButton( parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


