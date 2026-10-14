/**
 * @file qrecc.h
 * QR Error Correction.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRECC_H
#define QRECC_H

#include "qrecclevel.h"
#include "qrversion.h"

class bitstring;

/// QR Barcode Namespace
namespace qr
{

class poly;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Error Correction.
class ecc
{
    typedef ecc _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Encode bitstring.
    /**
     * @param[in] s  bitstring to encode
     * @param[in] ver  version
     * @param[in] level  error correction level
     * @param[out] buffer  encoded buffer
     * @return  @c true upon success, @c false otherwise
     */
    static bool encode( const bitstring &s, version ver, eccLevel level, poly &buffer );

    /// Execute test cases.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    static bool test();

private:

    // not implemented
    ecc();

    // not implemented
    _Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // QRECC_H

