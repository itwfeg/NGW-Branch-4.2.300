/**
 * @file qrapi.h
 * QR API Entry Point.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRAPI_H
#define QRAPI_H

#include "qrecclevel.h"
#include "qrversion.h"

#include <string>

#include <stdint.h>

namespace qr
{
class moduleMatrix;
}

class bitmap;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// QR API Entry Point.
class qrapi
{
    typedef qrapi _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Encode string.
    /**
     * @param[in] s  string to encode
     * @param[in] level  error correction level
     * @param[out] b  qr code bitmap
     * @return  qr version upon success, -1 otherwise
     */
    static qr::version encode( const std::string &s, qr::eccLevel level, bitmap &b );

    /// Encode string.
    /**
     * @param[in] s  string to encode
     * @param[in] level  error correction level
     * @param[in,out] b  bitmap to modify
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @param[in] width  width of qr code
     * @param[in] height  height of qr code
     * @return  qr version upon success, -1 otherwise
     */
    static qr::version encode( const std::string &s, qr::eccLevel level, bitmap &b, size_t x, size_t y, size_t width, size_t height );

private:

    /// Generate matrix.
    static qr::moduleMatrix *generate( const std::string &s, qr::eccLevel level, qr::version &version );

    // not implemented
    qrapi();

    // not implemented
    _Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // QRAPI_H
