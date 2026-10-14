/**
 * @file fontutil.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef FONTUTIL_H
#define FONTUTIL_H

class QFont;
class QSize;
class QString;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Font utilities.
class fontUtil
{
    typedef fontUtil _Myt;

public:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Calculate max font point size for area.
    /**
     * @param[in] font  font to use
     * @param[in] maxSize  maximum boundry size
     * @param[in] text  text
     * @param[in] useLargeFonts  use large fonts
     * @return  font point size
     */
    static int calcFontPointSize( const QFont& font, const QSize& maxSize, const QString& text, bool useLargeFonts = false );

private:

    // not implemented
    fontUtil();

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FONTUTIL_H
