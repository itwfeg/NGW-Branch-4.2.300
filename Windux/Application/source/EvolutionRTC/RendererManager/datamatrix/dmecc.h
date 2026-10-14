/**
 * @file dmecc.h
 * Data Matrix Error Correction.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DMECC_H
#define DMECC_H

#include "dmecclevel.h"

#include <vector>

class bitstring;
class buffer;

/// Data Matrix Barcode Namespace
namespace dm
{

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
     * @param[in] level  error correction level
     * @param[out] result  encoded buffer
     * @return  @c true upon success, @c false otherwise
     */
    static bool encode( const bitstring &s, eccLevel level, buffer &result );

    /// Execute test cases.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    static bool test();

private:

    // Calculate product.
    static int prod( int x, int y, const std::vector<int> &log, const std::vector<int> &alog, int gf );

    // not implemented
    ecc();

    // not implemented
    _Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DMECC_H

