/**
 * @file eanapi.h
 * EAN API Entry Point.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef EANAPI_H
#define EANAPI_H

#include "eantype.h"

#include <cstdint>
#include <string>

class bitmap;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// EAN API Entry Point.
class eanapi
{
    typedef eanapi _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Encode string.
    /**
     * @param[in] s  string to encode
     * @param[in] t  ean type
     * @param[out] b  bitmap
     * @return  @c true upon success, @c false otherwise
     */
    static bool encode( const std::string &s, ean::type t, bitmap &b );

    /// Encode string.
    /**
     * @param[in] s  string to encode
     * @param[in] t  ean type
     * @param[in,out] b  bitmap to modify
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @param[in] width  width of qr code
     * @param[in] height  height of qr code
     * @return  @c true upon success, @c false otherwise
     */
    static bool encode( const std::string &s, ean::type t, bitmap &b, size_t x, size_t y, size_t width, size_t height );

private:

    // not implemented
    eanapi();

    // not implemented
    _Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EANAPI_H
