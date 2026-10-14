/**
 * @file fontutil.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "fontutil.h"

#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QRect>
#include <QSize>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////
int fontUtil::calcFontPointSize( const QFont& font, const QSize& maxSize, const QString& text, bool useLargeFonts )
{
    QFontDatabase fdb;
    QFont f( font );

    // font sizes to try
    QList<int> fontSizes( fdb.smoothSizes( f.family(), f.styleName() ) );

    if ( !fontSizes.size() )
        fontSizes = fdb.pointSizes( f.family(), f.styleName() );

    if ( !fontSizes.size() )
        fontSizes = fdb.standardSizes();

    // sanity check
    if ( !fontSizes.size() )
        return 8;

    // add some larger fonts too
    if ( useLargeFonts )
    {
        for ( int i( 0 ); i < 8; ++i )
            fontSizes.append( fontSizes.last() + 8 );
    }

    int result( fontSizes.first() );

    // iterate font point sizes
    foreach ( int s, fontSizes )
        if ( result < s )
        {
            // bump point size
            f.setPointSize( s );

            // compute bounding rect
            QFontMetrics metrics( f );
            QRect boundingRect( metrics.boundingRect( QRect( 0, 0, INT_MAX, INT_MAX ), Qt::AlignLeft | Qt::AlignTop, text ) );

            // check bounds
            if (( maxSize.width() < boundingRect.width() ) || ( maxSize.height() < boundingRect.height() ))
                break;

            result = f.pointSize();
        }

    return result;
}
