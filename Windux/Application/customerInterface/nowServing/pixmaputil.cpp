/**
 * @file pixmaputil.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "pixmaputil.h"

#include <QPainter>
#include <QPixmap>
#include <QSize>

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap pixmapUtil::scalePixmap( const QPixmap& pixmap, const QSize& size, Qt::AspectRatioMode aspectMode, const QColor& backgroundColor )
{
    QPixmap canvas( size );
    canvas.fill( backgroundColor );

    if ( !pixmap.isNull() )
    {
        QPixmap scaledPixmap( pixmap.scaled( size, aspectMode, Qt::SmoothTransformation ) );
        QSize scaledPixmapSize( scaledPixmap.size() );

        QPoint pos( (size.width() - scaledPixmapSize.width()) / 2, (size.height() - scaledPixmapSize.height()) / 2 );

        QPainter painter( &canvas );
        painter.setRenderHint( QPainter::Antialiasing, true );

        painter.drawPixmap( pos, scaledPixmap );
    }

    return canvas;
}
