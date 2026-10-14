/**
 * @file dmecclevel.h
 * Data Matrix Error Correction levels.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DMECCLEVEL_H
#define DMECCLEVEL_H

/// Data Matrix Barcode Namespace
namespace dm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Error correction levels.
/**
 * @warning Only @c ECC200 format is supported!
 */
enum eccLevel
{
    ECC000 = 0,                                     ///< Convolutional Error Correction; 0% Error Recovery
    ECC050,                                         ///< Convolutional Error Correction; 3% Error Recovery
    ECC080,                                         ///< Convolutional Error Correction; 5% Error Recovery
    ECC100,                                         ///< Convolutional Error Correction; 12.5% Error Recovery
    ECC140,                                         ///< Convolutional Error Correction; 25% Error Recovery
    ECC200                                          ///< Reed-Solomon Error Correction; Approx 25% Error Recovery (varies based on size)
};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DMATRIXECCLEVEL_H

