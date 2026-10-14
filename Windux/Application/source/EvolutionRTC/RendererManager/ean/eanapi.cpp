/**
 * @file eanapi.cpp
 * EAN API Entry Point.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "eanapi.h"
#include "eanstring.h"

#include <rbitmap.h>
#include <bitstring.h>

using namespace ean;

///////////////////////////////////////////////////////////////////////////////////////////////////
bool eanapi::encode( const std::string &s, ean::type t, bitmap &b )
{
    string barcode( s, t );

    // validate barcode
    if ( !barcode.length() )
        return false;

    // render
    const size_t width( barcode.length() );
    const size_t height( (width * 3) / 4 );

    barcode.render( (b = bitmap( width, height )), 0, 0, width, height );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool eanapi::encode( const std::string &s, ean::type t, bitmap &b, size_t x, size_t y, size_t width, size_t height )
{
    string barcode( s, t );

    // validate barcode
    if ( !barcode.length() )
        return false;

    // render
    barcode.render( b, x, y, width, height );

    return true;
}
