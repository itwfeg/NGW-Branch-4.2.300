/**
 * @file dmmetrics.h
 * Data Matrix Barcode Metrics.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DMMETRICS_H
#define DMMETRICS_H

#include "dmecclevel.h"
#include "dmsymbolsize.h"

#include <cstddef>

/// Data Matrix Barcode Namespace
namespace dm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Data capacity information.
struct dataCapacityInfo
{
    size_t numeric;                                 ///< Maximum numeric characters.
    size_t alphanumeric;                            ///< Maximum alphanumeric characters.
    size_t byte;                                    ///< Maximum byte characters.
};

/// Symbol information.
struct symbolInfo
{
    size_t dataRegionHeight;                        ///< Height of data region.
    size_t dataRegionWidth;                         ///< Width of data region.
    size_t numDataRegions;                          ///< Number of data regions.
    size_t mappingMatrixHeight;                     ///< Mapping matrix height.
    size_t mappingMatrixWidth;                      ///< Mapping matrix width.
};

/// Error correction block information.
struct eccBlockInfo
{
    size_t dataBlocks;                              ///< Number of error correction blocks.
    size_t errorBlocks;                             ///< Total number of codewords.
    size_t interleavedBlocks;                       ///< Number of data codewords.
};

/// Error correction characteristics information.
struct eccCharacteristicsInfo
{
    size_t totalDataCodewords;                      ///< Total number of data codewords.
    size_t totalErrorCodewords;                     ///< Total number of error codewords.
    eccBlockInfo blockInfo[2];                      ///< Error correction block information.
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

    /// Retrieve data capacity information.
    /**
     * @param[in] level  error correction level
     * @param[in] s  symbol size
     * @return  data capacity information
     */
    static dataCapacityInfo dataCapacity( eccLevel level, symbolSize s );

    /// Retrieve symbology information.
    /**
     * @param[in] s  symbol size
     * @return  symbol information
     */
    static symbolInfo symbology( symbolSize s );

    /// Retrieve error correction blocks information.
    /**
     * @param[in] level  error correction level
     * @param[in] s  symbol size
     * @return  error correction characteristics information
     */
    static eccCharacteristicsInfo eccCharacteristics( eccLevel level, symbolSize s );

private:

    // not implemented
    metrics();

    // not implemented
    _Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DMATRIXMETRICS_H

