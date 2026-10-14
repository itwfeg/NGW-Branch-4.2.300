/**
 * @file dmecc.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "dmecc.h"
#include "dmmetrics.h"
#include "dmmode.h"
#include "dmmodedata.h"

#include <bitstring.h>
#include <buffer.h>

/// Data Matrix Barcode Namespace
namespace dm
{

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ecc::encode( const bitstring &s, eccLevel level, buffer &result )
{
    static const int gf_value[] = { 0, 0, 0, 0, 0, 256 };
    static const int pp_value[] = { 0, 0, 0, 0, 0, 301 };

    const size_t dataLength( s.length() / 8 );

    // find proper ecci
    eccCharacteristicsInfo ecci;

    size_t ss( SymbolSize_10x10 );

    do
    {
        ecci = metrics::eccCharacteristics( level, (symbolSize) ss );

        // check message fits within this symbology
        if ( dataLength == ecci.totalDataCodewords )
            break;

    } while ( ++ss <= SymbolSize_144x144 );

    if ( SymbolSize_144x144 < ss )
        return false;

    // convert bitstring to data buffer
    buffer data( dataLength, 0 );

    for ( size_t i( 0 ); i < s.length(); ++i )
    {
        const size_t di( i/8 );

        data[di] <<= 1;

        if ( s[i] )
            data[di] |= 1;
    }

    // generate interleaved blocks
    const size_t numBlocks( ecci.blockInfo[0].interleavedBlocks + ecci.blockInfo[1].interleavedBlocks );

    std::vector<buffer> blocks;
    blocks.resize( numBlocks );

    for ( size_t i( 0 ); i < data.size(); ++i )
        blocks[ i % numBlocks ].push_back( data[i] );

    // --- //

    // set proper galois field size and prime modulus
    int gf( gf_value[level] );
    int pp( pp_value[level] );

    // generate log and anti-log tables
    std::vector<int> log( gf, 0 );
    std::vector<int> alog( gf, 0 );

    log[0] = 1-gf;
    alog[0] = 1;

    for ( int i( 1 ); i < gf; ++i )
    {
        alog[i] = alog[i-1] * 2;

        if ( alog[i] >= gf )
            alog[i] ^= pp;

        log[alog[i]] = i;
    }

    // generate ecc data for each block
    size_t block( 0 );

    for ( size_t bi( 0 ); bi < 2; ++bi )
    {
        size_t interleavedBlocks( ecci.blockInfo[bi].interleavedBlocks );

        if ( !interleavedBlocks )
            continue;

        const size_t nd( ecci.blockInfo[bi].dataBlocks );
        const size_t nc( ecci.blockInfo[bi].errorBlocks );

        // compute the generator polynomial coefficients
        std::vector<int> c( nc+1, 0 );

        c[0] = 1;

        for ( size_t i( 1 ); i <= nc; ++i )
        {
            c[i] = c[i-1];

            for ( size_t j( i-1 ); j >= 1; --j )
                c[j] = c[j-1] ^ prod( c[j], alog[i], log, alog, gf );

            c[0] = prod( c[0], alog[i], log, alog, gf );
        }

        do
        {
            // load block
            buffer wd( blocks[block] );

            if ( nd != wd.size() )
                return false;

            // generate ecc codewords
            wd.resize( nd+nc, 0 );

            for ( size_t i( 0 ); i < nd; ++i )
            {
                const int k( wd[nd] ^ wd[i] );

                for ( size_t j( 0 ); j < nc; ++j )
                    wd[nd+j] = wd[nd+j+1] ^ prod( k, c[nc-j-1], log, alog, gf );
            }

            // save block
            blocks[block++] = wd;

        } while ( --interleavedBlocks );

    }

    // --- //

    // generate result buffer
    const size_t totalCodewords( ecci.totalDataCodewords + ecci.totalErrorCodewords );

    result.clear();

    for ( size_t i( 0 ); i < ecci.totalDataCodewords; ++i )
        result.push_back( blocks[i % numBlocks].at( i / numBlocks ) );

    for ( size_t i( 0 ); i < ecci.totalErrorCodewords; ++i )
    {
        const size_t block( i % numBlocks );
        const size_t bi( block < ecci.blockInfo[0].interleavedBlocks ? 0 : 1 );

        result.push_back( blocks[block].at( ecci.blockInfo[bi].dataBlocks + (i / numBlocks) ) );
    }

    return (totalCodewords == result.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int ecc::prod( int x, int y, const std::vector<int> &log, const std::vector<int> &alog, int gf )
{
    if (( !x ) || ( !y ))
        return 0;

    return alog[(log[x] + log[y]) % (gf-1)];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ecc::test()
{
    const eccLevel level( ECC200 );

    bitstring message;
    buffer result;

    modeData *temp;
    modeData *padding;

    // ---- //

    static const std::string test_annexo( "123456" );

    static const uint8_t test_annexo_r[] = {
        0x8e, 0xa4, 0xba, 0x72, 0x19, 0x05, 0x58, 0x66
    };

    // test Annex 0
    message.clear();

    temp = mode::create( mode::Numeric, test_annexo );
    message.append( temp->generate() );
    delete temp;

    // encode
    if ( !encode( message, level, result ) )
        return false;
    else if ( sizeof( test_annexo_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( buffer::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_annexo_r[index++] != (*i) )
                return false;
    }

    // ---- //

    static const std::string test_wiki( "Wikipedia" );

    static const uint8_t test_wiki_r[] = {
         88, 106, 108, 106, 113, 102, 101, 106,
         98, 129, 251, 147, 104, 216,  88,  39,
        233, 202,  71, 217,  26,  92,  25, 232
    };

    // test Wiki
    message.clear();

    temp = mode::create( mode::ASCII, test_wiki );
    message.append( temp->generate() );
    delete temp;

    padding = mode::create( mode::Padding, 0 );
    message.append( padding->generate() );
    delete padding;

    padding = mode::create( mode::Padding, (unsigned int) ((message.length()/8) + 1) );
    message.append( padding->generate() );
    delete padding;

    padding = mode::create( mode::Padding, (unsigned int) ((message.length()/8) + 1) );
    message.append( padding->generate() );
    delete padding;

    // encode
    if ( !encode( message, level, result ) )
        return false;
    else if ( sizeof( test_wiki_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( buffer::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_wiki_r[index++] != (*i) )
                return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

