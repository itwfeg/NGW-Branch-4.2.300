/**
 * @file pixmaputil.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PIXMAPUTIL_H
#define PIXMAPUTIL_H

#include <QColor>

class QPixmap;
class QSize;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Pixmap utilities.
class pixmapUtil
{
    typedef pixmapUtil _Myt;

public:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Scale pixmap to specified size.
    /**
     * @param[in] pixmap  pixmap to scale
     * @param[in] size  scale size
     * @param[in] aspectMode  scale aspect ratio
     * @param[in] backgroundColor  background color
     * @return  scaled pixmap
     */
    static QPixmap scalePixmap( const QPixmap& pixmap, const QSize& size, Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio, const QColor& backgroundColor = Qt::white );

private:

    // not implemented
    pixmapUtil();

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PIXMAPUTIL_H
