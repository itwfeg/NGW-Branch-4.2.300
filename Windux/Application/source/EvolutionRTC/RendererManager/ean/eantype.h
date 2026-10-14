/**
 * @file eantype.h
 * EAN Barcode Type.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef EANTYPE_H
#define EANTYPE_H

/// EAN Barcode Namespace
namespace ean
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Types.
enum type
{
    EAN8,                                           ///< EAN 8 Barcode.
    EAN13                                           ///< EAN 13 Barcode.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EANTYPE_H
