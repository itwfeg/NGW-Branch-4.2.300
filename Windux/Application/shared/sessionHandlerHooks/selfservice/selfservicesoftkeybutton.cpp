/**
 * @file selfservicesoftkeybutton.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfservicesoftkeybutton.h"

#include <backgroundwidgettypes.h>
#include <pixmap.h>

#include <QPainter>

/// Hobart namespace.
namespace hobart {

const Qt::Alignment selfServiceSoftkeyButton::buttonTextAlignment_( Qt::AlignBottom | Qt::AlignHCenter );
const QColor selfServiceSoftkeyButton::buttonTextColor_( hobart::colorPalette::black );

const int selfServiceSoftkeyButton::buttonFontPointSize_ = 11;

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeyButton::selfServiceSoftkeyButton( QWidget *parent ) :
    _Mybase( parent )
{
    // set object name
    setObjectName( "selfServiceSoftkeyButton" );

    // set button properties
    _Mybase::setTextAlignment( buttonTextAlignment_ );
    _Mybase::setTextMargins( QMargins( 5, 5, 5, 5 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeyButton::~selfServiceSoftkeyButton()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButton::setBackgroundImage( const QString& value )
{
    // save off image for overlaying later...
    backgroundImageOrig_ = value;

    if( value.contains( "more.png" ) || value.contains( "less.png" ) )
        backgroundImageOrigSize_ = 1.0;
    else
        backgroundImageOrigSize_ = 0.70;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButton::setTextAlignment( Qt::Alignment value )
{
    Q_UNUSED( value );

    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButton::paintEvent( QPaintEvent *e )
{
    // set button properties
    setTextColor( buttonTextColor_ );

    // invoke base method
    _Mybase::paintEvent( e );

    // load product action should render product code
    if ( 8 == action() )
    {
        QPainter p( this );
        p.setRenderHint( QPainter::Antialiasing, true );

        // text font point size
        QFont f( p.font() );
            f.setPointSize( buttonFontPointSize_ );
            p.setFont( f );

            QMargins margins( textMargins() );
            QRect r( rect().adjusted( margins.left(), margins.top(), -margins.right(), -margins.bottom() ) );

            // draw text
            p.setPen( Qt::black );
        p.drawText( r, Qt::AlignTop | Qt::AlignLeft, actionParam() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButton::updatePixmaps()
{
    QPixmap pixmap( size() );
    pixmap.fill( Qt::transparent );

    QPainter p( &pixmap );
    p.setRenderHint( QPainter::Antialiasing, true );

    p.setBrush( hobart::colorPalette::white );

    QPen pen( p.pen() );
    pen.setColor( hobart::colorPalette::gray );
    pen.setWidth( 2 );
    p.setPen( pen );

    // create button
    p.drawRoundedRect( rect().adjusted( 1, 1, -1, -1 ), 20, 20 );

    // load background image
    if ( backgroundImageOrig_.length() )
    {
        QString thumb;
        QString img = makeSoftkeyImage( backgroundImageOrig_, &thumb );

        QPixmap background = thumb.length() ? thumb : img;

        // overlay original background image
        if ( !background.isNull() )
        {
            QMargins margins( textMargins() );

            QSize sourceSize( width() - margins.left() - margins.right(), height() - margins.top() - margins.bottom() );
            sourceSize.setHeight( sourceSize.height() * backgroundImageOrigSize_ );

            sizeToFitPixmap stretcher( sourceSize );

            // original background image is stretched to fit button
            p.drawPixmap( margins.left(), margins.top(), stretcher( background ) );

            if ( background.height() < background.width() )
                updateThumbnailHint( backgroundImageOrig_, QSize( sourceSize.width(), 0 ) );
            else
                updateThumbnailHint( backgroundImageOrig_, QSize( 0, sourceSize.height() ) );
        }
    }

    invertPixmap inverter;

    // set new button images
    releasedImage_ = pixmap;
    pressedImage_ = inverter( pixmap );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *selfServiceSoftkeyButtonFactory::create( QWidget *parent ) const
{
    // create new button
    return new selfServiceSoftkeyButton( parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


