/**
 * @file dmsymbolsize.h
 * Data Matrix Symbol Sizes.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DMSYMBOLSIZE_H
#define DMSYMBOLSIZE_H

/// Data Matrix Barcode Namespace
namespace dm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Symbol Sizes.
enum symbolSize
{
    // Square Symbols
    SymbolSize_10x10 = 0,                           // Data Region 8x8; 1 Data Region
    SymbolSize_12x12,                               // Data Region 10x10; 1 Data Region
    SymbolSize_14x14,                               // Data Region 12x12; 1 Data Region
    SymbolSize_16x16,                               // Data Region 14x14; 1 Data Region
    SymbolSize_18x18,                               // Data Region 16x16; 1 Data Region
    SymbolSize_20x20,                               // Data Region 18x18; 1 Data Region

    SymbolSize_22x22,                               // Data Region 20x20; 1 Data Region
    SymbolSize_24x24,                               // Data Region 22x22; 1 Data Region
    SymbolSize_26x26,                               // Data Region 24x24; 1 Data Region
    SymbolSize_32x32,                               // Data Region 14x14; 4 Data Regions
    SymbolSize_36x36,                               // Data Region 16x16; 4 Data Regions
    SymbolSize_40x40,                               // Data Region 18x18; 4 Data Regions

    SymbolSize_44x44,                               // Data Region 20x20; 4 Data Regions
    SymbolSize_48x48,                               // Data Region 22x22; 4 Data Regions
    SymbolSize_52x52,                               // Data Region 24x24; 4 Data Regions
    SymbolSize_64x64,                               // Data Region 14x14; 16 Data Regions
    SymbolSize_72x72,                               // Data Region 16x16; 16 Data Regions
    SymbolSize_80x80,                               // Data Region 18x18; 16 Data Regions

    SymbolSize_88x88,                               // Data Region 20x20; 16 Data Regions
    SymbolSize_96x96,                               // Data Region 22x22; 16 Data Regions
    SymbolSize_104x104,                             // Data Region 24x24; 16 Data Regions
    SymbolSize_120x120,                             // Data Region 18x18; 36 Data Regions
    SymbolSize_132x132,                             // Data Region 20x20; 36 Data Regions
    SymbolSize_144x144,                             // Data Region 22x22; 36 Data Regions

    // Rectangular Symbols
    SymbolSize_8x18,                                // Data Region 6x16; 1 Data Region
    SymbolSize_8x32,                                // Data Region 6x14; 2 Data Regions
    SymbolSize_12x26,                               // Data Region 10x24; 1 Data Region
    SymbolSize_12x36,                               // Data Region 10x16; 2 Data Regions
    SymbolSize_16x36,                               // Data Region 14x16; 2 Data Regions
    SymbolSize_16x48                                // Data Region 14x22; 2 Data Regions
};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DMATRIXSYMBOLSIZE_H

