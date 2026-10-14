/**
 * @file qrmetrics.h
 * QR Barcode Metrics.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRMETRICS_H
#define QRMETRICS_H

#include "qrecclevel.h"
#include "qrversion.h"

#include <cstddef>

/// QR Barcode Namespace
namespace qr
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Alignment pattern information.
struct alignmentInfo
{
    size_t numPatterns;                             ///< Number of alignment patterns.
    size_t location[7];                             ///< Location of center module.
};

/// Data capacity information.
struct dataCapacityInfo
{
    size_t numModulesPerSide;                       ///< Modules per side (A).
    size_t functionPatternModules;                  ///< Function pattern module count (B).
    size_t formatAndVersionInfoModules;             ///< Format and version information module count (C).
    size_t dataModules;                             ///< Data module count (A^2 - B - C)
    size_t dataCodewordCapacity;                    ///< Data codeword capacity (1 codeword = 8 bits)
    size_t remainderBits;                           ///< Remainder bits.
};

/// Error correction block information.
struct eccCodePerBlockInfo
{
    size_t numBlocks;                               ///< Number of error correction blocks.
    size_t c;                                       ///< Total number of codewords.
    size_t k;                                       ///< Number of data codewords.
    size_t r;                                       ///< Number of error correction capacity.
};

/// Error correction characteristics information.
struct eccCharacteristicsInfo
{
    size_t numCodewords;                            ///< Number of error correction codewords.
    eccCodePerBlockInfo blockInfo[2];               ///< Error correction block information.
};


///////////////////////////////////////////////////////////////////////////////////////////////////

/// Metrics.
class metrics
{
    typedef metrics _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve alignment information.
    /**
     * @param[in] ver  version
     * @return  alignment information
     */
    static alignmentInfo alignment( version ver );

    /// Retrieve data capacity information.
    /**
     * @param[in] ver  version
     * @return  data capacity information
     */
    static dataCapacityInfo dataCapacity( version ver );

    /// Retrieve error correction blocks information.
    /**
     * @param[in] ver  version
     * @param[in] level  error correction level
     * @return  error correction characteristics information
     */
    static eccCharacteristicsInfo eccCharacteristics( version ver, eccLevel level );

private:

    // not implemented
    metrics();

    // not implemented
    _Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // QRMETRICS_H

