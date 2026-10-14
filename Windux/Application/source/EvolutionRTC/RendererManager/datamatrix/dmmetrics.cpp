/**
 * @file dmmetrics.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "dmmetrics.h"

/// Data Matrix Barcode Namespace
namespace dm
{

///////////////////////////////////////////////////////////////////////////////////////////////////
dataCapacityInfo metrics::dataCapacity( eccLevel level, symbolSize s )
{
    // reed-soloman ecc
    if ( ECC200 == level )
    {
        static const dataCapacityInfo data[] = {

            // Square Symbols
            {    6,    3,    1 }, // SymbolSize_10x10
            {   10,    6,    3 }, // SymbolSize_12x12
            {   16,   10,    6 }, // SymbolSize_14x14
            {   24,   16,   10 }, // SymbolSize_16x16
            {   36,   25,   16 }, // SymbolSize_18x18
            {   44,   31,   20 }, // SymbolSize_20x20

            {   60,   43,   28 }, // SymbolSize_22x22
            {   72,   52,   34 }, // SymbolSize_24x24
            {   88,   64,   42 }, // SymbolSize_26x26
            {  124,   91,   60 }, // SymbolSize_32x32
            {  172,  127,   84 }, // SymbolSize_36x36
            {  228,  169,  112 }, // SymbolSize_40x40

            {  288,  214,  142 }, // SymbolSize_44x44
            {  348,  259,  172 }, // SymbolSize_48x48
            {  408,  304,  202 }, // SymbolSize_52x52
            {  560,  418,  278 }, // SymbolSize_64x64
            {  736,  550,  366 }, // SymbolSize_72x72
            {  912,  682,  454 }, // SymbolSize_80x80

            { 1152,  862,  574 }, // SymbolSize_88x88
            { 1392, 1042,  694 }, // SymbolSize_96x96
            { 1632, 1222,  814 }, // SymbolSize_104x104
            { 2100, 1573, 1048 }, // SymbolSize_120x120
            { 2608, 1954, 1302 }, // SymbolSize_132x132
            { 3116, 2335, 1556 }, // SymbolSize_144x144

            // Rectangular Symbols
            {   10,    6,    3 }, // SymbolSize_8x18
            {   20,   13,    8 }, // SymbolSize_8x32
            {   32,   22,   14 }, // SymbolSize_12x26
            {   44,   31,   20 }, // SymbolSize_12x36
            {   64,   46,   30 }, // SymbolSize_16x36
            {   98,   72,   47 }, // SymbolSize_16x48
        };

        return data[s];
    }

    // other ecc not supported
    static const dataCapacityInfo unsupported = {0};
    return unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
symbolInfo metrics::symbology( symbolSize s )
{
    static const symbolInfo data[] = {

        // Square Symbols
        {  8,  8,  1,   8,   8 }, // SymbolSize_10x10
        { 10, 10,  1,  10,  10 }, // SymbolSize_12x12
        { 12, 12,  1,  12,  12 }, // SymbolSize_14x14
        { 14, 14,  1,  14,  14 }, // SymbolSize_16x16
        { 16, 16,  1,  16,  16 }, // SymbolSize_18x18
        { 18, 18,  1,  18,  18 }, // SymbolSize_20x20

        { 20, 20,  1,  20,  20 }, // SymbolSize_22x22
        { 22, 22,  1,  22,  22 }, // SymbolSize_24x24
        { 24, 24,  1,  24,  24 }, // SymbolSize_26x26
        { 14, 14,  4,  28,  28 }, // SymbolSize_32x32
        { 16, 16,  4,  32,  32 }, // SymbolSize_36x36
        { 18, 18,  4,  36,  36 }, // SymbolSize_40x40

        { 20, 20,  4,  40,  40 }, // SymbolSize_44x44
        { 22, 22,  4,  44,  44 }, // SymbolSize_48x48
        { 24, 24,  4,  48,  48 }, // SymbolSize_52x52
        { 14, 14, 16,  56,  56 }, // SymbolSize_64x64
        { 16, 16, 16,  64,  64 }, // SymbolSize_72x72
        { 18, 18, 16,  72,  72 }, // SymbolSize_80x80

        { 20, 20, 16,  80,  80 }, // SymbolSize_88x88
        { 22, 22, 16,  88,  88 }, // SymbolSize_96x96
        { 24, 24, 16,  96,  96 }, // SymbolSize_104x104
        { 18, 18, 36, 108, 108 }, // SymbolSize_120x120
        { 20, 20, 36, 120, 120 }, // SymbolSize_132x132
        { 22, 22, 36, 132, 132 }, // SymbolSize_144x144

        // Rectangular Symbols
        {  6, 16,  1,   6,  16 }, // SymbolSize_8x18
        {  6, 14,  2,   6,  28 }, // SymbolSize_8x32
        { 10, 24,  1,  10,  24 }, // SymbolSize_12x26
        { 10, 16,  2,  10,  32 }, // SymbolSize_12x36
        { 14, 16,  2,  14,  32 }, // SymbolSize_16x36
        { 14, 22,  2,  14,  44 }, // SymbolSize_16x48
    };

    return data[s];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eccCharacteristicsInfo metrics::eccCharacteristics( eccLevel level, symbolSize s )
{
    // reed-soloman ecc
    if ( ECC200 == level )
    {
        static const eccCharacteristicsInfo data[] = {

            // Square Symbols
            {    3,   5, /*{*/   3,  5, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_10x10
            {    5,   7, /*{*/   5,  7, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_12x12
            {    8,  10, /*{*/   8, 10, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_14x14
            {   12,  12, /*{*/  12, 12, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_16x16
            {   18,  14, /*{*/  18, 14, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_18x18
            {   22,  18, /*{*/  22, 18, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_20x20

            {   30,  20, /*{*/  30, 20, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_22x22
            {   36,  24, /*{*/  36, 24, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_24x24
            {   44,  28, /*{*/  44, 28, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_26x26
            {   62,  36, /*{*/  62, 36, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_32x32
            {   86,  42, /*{*/  86, 42, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_36x36
            {  114,  48, /*{*/ 114, 48, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_40x40

            {  144,  56, /*{*/ 144, 56, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_44x44
            {  174,  68, /*{*/ 174, 68, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_48x48
            {  204,  84, /*{*/ 102, 42, 2 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_52x52
            {  280, 112, /*{*/ 140, 56, 2 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_64x64
            {  368, 144, /*{*/  92, 36, 4 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_72x72
            {  456, 192, /*{*/ 114, 48, 4 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_80x80

            {  576, 224, /*{*/ 144, 56, 4 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_88x88
            {  696, 272, /*{*/ 174, 68, 4 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_96x96
            {  816, 336, /*{*/ 136, 56, 6 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_104x104
            { 1050, 408, /*{*/ 175, 68, 6 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_120x120
            { 1304, 496, /*{*/ 163, 62, 8 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_132x132
            { 1558, 620, /*{*/ 156, 62, 8 /*}*/, /*{*/ 155, 62, 2 /*}*/ }, // SymbolSize_144x144

            // Rectangular Symbols
            {    5,   7, /*{*/   5,  7, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_8x18
            {   10,  11, /*{*/  10, 11, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_8x32
            {   16,  14, /*{*/  16, 14, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_12x26
            {   22,  18, /*{*/  22, 18, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_12x36
            {   32,  24, /*{*/  32, 24, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_16x36
            {   49,  28, /*{*/  49, 28, 1 /*}*/, /*{*/   0,  0, 0 /*}*/ }, // SymbolSize_16x48
        };

        return data[s];
    }

    // other ecc not supported
    static const eccCharacteristicsInfo unsupported = {0};
    return unsupported;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

