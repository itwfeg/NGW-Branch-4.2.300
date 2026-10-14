/**
 * @file gradientpushbutton.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "gradientpushbutton.h"

#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////
gradientPushButton::gradientPushButton( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gradientPushButton::gradientPushButton( const QString& text, QWidget *parent ) :
    _Mybase( text, parent )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gradientPushButton::~gradientPushButton()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gradientPushButton::resizeEvent( QResizeEvent *e )
{
    // generate new gradient
    QLinearGradient g( 0, 0, 0, height() );
    g.setColorAt( 0.0, QColor( 192, 192, 192 ) );
    g.setColorAt( 0.2121, QColor( 96, 96, 96 ) );
    g.setColorAt( 0.5091, QColor( 64, 64, 64 ) );
    g.setColorAt( 0.5333, QColor( 32, 32, 32 ) );
    g.setColorAt( 1.0, QColor( 64, 64, 64 ) );

    gradient_ = g;

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gradientPushButton::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    QPixmap canvas( size() );
    canvas.fill( Qt::transparent );

    QPainter painter( &canvas );
    painter.setRenderHint( QPainter::Antialiasing, true );

    // draw background
    painter.setBrush( gradient_ );
    painter.setPen( QPen( Qt::white, 2 ) );

    int a( painter.pen().width() / 2 );
    painter.drawRoundedRect( QRect( QPoint( 0, 0 ), size() ).adjusted( a, a, -a, -a ), 4, 4 );

    // draw text
    QPalette p( palette() );

    painter.setBrush( p.brush( QPalette::ButtonText ) );
    painter.setFont( font() );
    painter.setPen( QPen( p.color( QPalette::ButtonText ) ) );

    painter.drawText( rect(), Qt::AlignCenter | Qt::TextWordWrap, text() );

    // render!
    QPainter renderer( this );

    if ( !isDown() )
        renderer.drawPixmap( QPoint( 0, 0 ), canvas );
    else
    {
        QImage img( canvas.toImage() );
        img.invertPixels();

        renderer.drawImage( QPoint( 0, 0 ), img );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gradientPushButton::initialize()
{
}
