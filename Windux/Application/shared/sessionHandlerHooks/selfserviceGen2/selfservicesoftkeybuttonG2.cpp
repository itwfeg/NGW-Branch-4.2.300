/**
 * @file selfservicesoftkeybuttonG2.cpp
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Randy Blankley
 * @updatedby Joe E. Crew
 */

#include "selfservicesoftkeybuttonG2.h"

#include <backgroundwidgettypes.h>
#include <pixmap.h>

#include <QPainter>

/// Hobart namespace.
namespace hobart {

const Qt::Alignment selfServiceSoftkeyButtonG2::buttonTextAlignment_( Qt::AlignBottom | Qt::AlignHCenter );
const QColor selfServiceSoftkeyButtonG2::buttonTextColor_( hobart::colorPalette::black );

const int selfServiceSoftkeyButtonG2::buttonFontPointSize_ = 11;

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeyButtonG2::selfServiceSoftkeyButtonG2( QWidget *parent ) :
    _Mybase( parent ),
    scaleConfig_(NULL),
    isGen2UI_( false )
{
    // set object name
    setObjectName( "selfServiceSoftkeyButtonG2" );

    // set button properties
    _Mybase::setTextAlignment( buttonTextAlignment_ );
    _Mybase::setTextMargins( QMargins( 5, 5, 5, 5 ) );

    rando_bgs_ << "softkeys/foodicon0.png" << "softkeys/foodicon1.png" << "softkeys/foodicon2.png" << "softkeys/foodicon3.png";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeyButtonG2::~selfServiceSoftkeyButtonG2()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButtonG2::setBackgroundImage( const QString& value )
{
    // save off image for overlaying later...
    if( isGen2UI_ && ( value.contains("Whiteflashkey")  || ( value.length() == 0 ) ) )
    {
        int rando = qrand() % 4;
        backgroundImageOrig_ = rando_bgs_[rando];
    }
    else
    {
        backgroundImageOrig_ = value;
    }

    if( value.contains( "more.png" ) || value.contains( "less.png" ) )
        backgroundImageOrigSize_ = 1.0;
    else
        backgroundImageOrigSize_ = 0.50;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButtonG2::setTextAlignment( Qt::Alignment value )
{
    Q_UNUSED( value );

    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButtonG2::setGen2ActiveUI( bool value )
{
    isGen2UI_ = value;

    _Mybase::setGen2ActiveUI( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButtonG2::paintEvent( QPaintEvent *e )
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

        if(  isGen2UI_ )
        {
            QString test_text = actionParam();

            f.setFamily( "Poppins Medium" );
            f.setPointSize( 14 );
            p.setFont( f );

            QMargins margins( textMargins() );
            QRect r( rect().adjusted( 15, margins.top(), -15, -11 ) );

            p.setPen( QPen( hobart::colorPalette::pluNumGrey, 4 ) );
            p.drawText( r, Qt::AlignBottom, test_text );                 // This is the PLU number in the upper left corner
        }
        else
        {
            f.setPointSize( buttonFontPointSize_ );
            p.setFont( f );

            QMargins margins( textMargins() );
            QRect r( rect().adjusted( margins.left(), margins.top(), -margins.right(), -margins.bottom() ) );

            p.setPen( Qt::black );
            p.drawText( r, Qt::AlignTop | Qt::AlignLeft, actionParam() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeyButtonG2::updatePixmaps()
{
    QPixmap pixmap( size() );
    pixmap.fill( Qt::transparent );

    QPainter p( &pixmap );
    p.setRenderHint( QPainter::Antialiasing, true );

    QPen pen( p.pen() );

    if( isGen2UI_ && ( 1.0 == backgroundImageOrigSize_ ) )
    {
        p.setBrush( hobart::colorPalette::backgroundGrey );         // change background color of container
        pen.setColor( hobart::colorPalette::backgroundGrey );                // turned the border white around background image
    }
    else
    {
        p.setBrush( hobart::colorPalette::white );
        pen.setColor( hobart::colorPalette::white );
    }

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

            int test_width = width();   // of what? THIS softkey
            int test_height = height();

            QSize sourceSize( width() - margins.left() - margins.right(), height() - margins.top() - margins.bottom() );
            sourceSize.setHeight( sourceSize.height() * backgroundImageOrigSize_ );

            sizeToFitPixmap stretcher( sourceSize );

            if( isGen2UI_ && ( 1.0 == backgroundImageOrigSize_ ) )
            {
                // keep the image the same size
                 int pt_x = ( test_width / 2 ) - ( background.width() / 2 );
                 int pt_y = ( test_height / 2 ) - ( background.height() / 2 );
                 p.drawPixmap( pt_x, pt_y, background );
            }
            else
            {
                // original background image is stretched to fit button
                p.drawPixmap( margins.left(), margins.top(), stretcher( background ) );
            }

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
softkeyButton *selfServiceSoftkeyButtonFactoryG2::create( QWidget *parent ) const
{
    // create new button
    return new selfServiceSoftkeyButtonG2( parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


