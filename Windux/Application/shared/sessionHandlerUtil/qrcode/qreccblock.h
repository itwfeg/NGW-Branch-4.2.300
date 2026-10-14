/**
 * @file qreccblock.h
 * QR Error Correction block.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRECCBLOCK_H
#define QRECCBLOCK_H

#include "poly.h"

#include <vector>

/// QR Barcode Namespace
namespace qr
{

struct eccCharacteristicsInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Error correction block.
typedef std::pair<poly, poly> eccBlock; // data, ecc

/// Error correction block array
class eccBlockArray : public std::vector<eccBlock>
{
    typedef eccBlockArray _Myt;
    typedef std::vector<eccBlock> _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    eccBlockArray();

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    eccBlockArray( const _Mybase &other );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    eccBlockArray( const _Myt &other );

    /// Constructor.
    /**
     * @param[in] buffer  buffer
     * @param[in] ecci  ecc characteristics info
     */
    eccBlockArray( const poly &buffer, const eccCharacteristicsInfo &ecci );

    /// Destructor.
    virtual ~eccBlockArray();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Generate encoded poly.
    /**
     * This method interleaves data and ecc blocks.
     * @return  encoded poly
     */
    virtual poly generate() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // QRECCBLOCK_H

