/**
 * @file qrecclevel.h
 * QR Error Correction levels.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRECCLEVEL_H
#define QRECCLEVEL_H

/// QR Barcode Namespace
namespace qr
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Error correction levels.
enum eccLevel
{
    LevelL = 0,                                     ///< Low; Approx. 7% Recovery Capacity.
    LevelM,                                         ///< Medium; Approx. 15% Recovery Capacity.
    LevelQ,                                         ///< Quartile; Approx. 25% Recovery Capacity.
    LevelH                                          ///< High; Approx. 30% Recovery Capacity.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // QRECCLEVEL_H

