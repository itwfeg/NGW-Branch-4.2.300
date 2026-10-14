/**
 * @file dmapi.h
 * Data Matrix API Entry Point.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DMAPI_H
#define DMAPI_H

#include "dmecclevel.h"
#include "dmsymbolsize.h"

#include <cstdint>
#include <string>

namespace dm
{
class moduleMatrix;
}

class bitmap;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Data Matrix API Entry Point.
class dmapi
{
    typedef dmapi _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Clear FNC1 character.
    static void clearFNC1();

    /// Encode string.
    /**
     * @param[in] s  string to encode
     * @param[in] level  error correction level
     * @param[out] b  data matrix barcode bitmap
     * @return  data matrix symbol size upon success, -1 otherwise
     */
    static dm::symbolSize encode( const std::string &s, dm::eccLevel level, bitmap &b );

    /// Encode string.
    /**
     * @param[in] s  string to encode
     * @param[in] level  error correction level
     * @param[in,out] b  bitmap to modify
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @param[in] width  width of data matrix barcode
     * @param[in] height  height of data matrix barcode
     * @return  data matrix symbol size upon success, -1 otherwise
     */
    static dm::symbolSize encode( const std::string &s, dm::eccLevel level, bitmap &b, size_t x, size_t y, size_t width, size_t height );

    /// Set FNC1 character.
    /**
     * @param[in] c  character to use for FNC1 encodation
     */
    static void setFNC1( char c );

private:

    static char fnc1_;

    /// Generate matrix.
    static dm::moduleMatrix *generate( const std::string &s, dm::eccLevel level, dm::symbolSize &size );

    // not implemented
    dmapi();

    // not implemented
    _Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DMAPI_H
