/**
 * @file qrecc.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "poly.h"
#include "qrecc.h"
#include "qreccblock.h"
#include "qrmetrics.h"
#include "qrmode.h"
#include "qrmodedata.h"

#include <bitstring.h>

/// QR Barcode Namespace
namespace qr
{

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ecc::encode( const bitstring &s, version ver, eccLevel level, poly &buffer )
{
    // calculate codewords required
    const size_t numDataCodewordsReq( (s.length() / 8) + ((s.length() % 8) ? 1 : 0) );

    const dataCapacityInfo dci( metrics::dataCapacity( ver ) );
    const eccCharacteristicsInfo ecci( metrics::eccCharacteristics( ver, level ) );

    const size_t numDataCodewords( dci.dataCodewordCapacity - ecci.numCodewords );

    // check we have enough room
    if ( numDataCodewords < numDataCodewordsReq )
        return false;

    const size_t numBlocks( ecci.blockInfo[0].numBlocks + ecci.blockInfo[1].numBlocks );

    // generate blocks
    const eccBlockArray blocks( poly( s, numDataCodewords ), ecci );

    if ( numBlocks != blocks.size() )
        return false;

    // save buffer
    buffer = blocks.generate();

    return ( dci.dataCodewordCapacity == buffer.size() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ecc::test()
{
    static const std::string test_annexg( "01234567" );

    static const uint8_t test_annexg_r[] = {
        0x10, 0x20, 0x0c, 0x56, 0x61, 0x80, 0xec, 0x11,
        0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11,
        0xa5, 0x24, 0xd4, 0xc1, 0xed, 0x36, 0xc7, 0x87,
        0x2c, 0x55,
    };

    const version ver( Ver1 );
    const eccLevel level( LevelM );

    modeData *data( mode::create( mode::Numeric, test_annexg ) );
    modeData *term( mode::create( mode::Terminator ) );

    bitstring message;
    message.append( data->generate( ver ) );
    message.append( term->generate( ver ) );

    poly result;

    // encode
    if ( !encode( message, ver, level, result ) )
        return false;
    else if ( sizeof( test_annexg_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( poly::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_annexg_r[index++] != (*i) )
                return false;
    }

    delete term;
    delete data;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

