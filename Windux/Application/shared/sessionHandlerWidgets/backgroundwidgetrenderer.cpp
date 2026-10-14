/**
 * @file backgroundwidgetrenderer.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "backgroundwidgetrenderer.h"
#include "backgroundwidgetstyles.h"
#include "backgroundwidgettypes.h"

#include <QPainter>
#include <QPixmap>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////
backgroundWidgetRenderer::backgroundWidgetRenderer( _WidgetType *tr, _Appearance *a ) :
    traits_( tr ),
    appearance_( a )
{
}

///////////////////////////////////////////////////////////////////////////////
backgroundWidgetRenderer::~backgroundWidgetRenderer()
{
    if ( traits_ )
        delete traits_;

    if ( appearance_ )
        delete appearance_;
}

///////////////////////////////////////////////////////////////////////////////
QPixmap backgroundWidgetRenderer::drawControl( const QSize &size, rendererState type ) const
{
    QPixmap pixmap( size );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    painter.setBrush( appearance_->brush( type, size ) );
    painter.setPen( appearance_->pen( type ) );

    if ( _WidgetType::PainterPath == traits_->type() )
        painter.drawPath( traits_->painterPath( size ) );
    else if ( _WidgetType::Rectangle == traits_->type() )
        painter.drawRect( QRect( QPoint( 0, 0 ), size ) );
    else if ( _WidgetType::RoundedRect == traits_->type() )
    {
        int a = painter.pen().width() / 2;
        painter.drawRoundedRect( QRect( QPoint( 0, 0 ), size ).adjusted( a, a, -a, -a ), traits_->xRadius(), traits_->yRadius() );
    }

    return pixmap;
}

///////////////////////////////////////////////////////////////////////////////
QPixmap backgroundWidgetRenderer::drawDropShadow( const QSize &size ) const
{
    QPainterPath path( traits_->painterPath( size ) );

    QPixmap pixmap( size );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    int loops = 10;
    int alpha = 6;
    painter.translate( QPoint( 5, 5 ) );

    do
    {
        QColor color( 0, 0, 0, (alpha += 6) );
        painter.setPen( color );
        painter.setBrush( color );

        painter.drawPath( path );
        painter.translate( QPoint( -1, -1 ) );

    } while ( --loops );

    return pixmap;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
