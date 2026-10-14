/**
 * @file qreccblock.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "qreccblock.h"
#include "qrmetrics.h"

/// QR Barcode Namespace
namespace qr
{

///////////////////////////////////////////////////////////////////////////////////////////////////
eccBlockArray::eccBlockArray() :
    _Mybase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eccBlockArray::eccBlockArray( const _Mybase &other ) :
    _Mybase( other )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eccBlockArray::eccBlockArray( const _Myt &other ) :
    _Mybase( other )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eccBlockArray::eccBlockArray( const poly &buffer, const eccCharacteristicsInfo &ecci )
{
    size_t bi( 0 );
    size_t i( 0 );

    // process each ecc block
    do
    {
        const eccCodePerBlockInfo *blockInfo( &ecci.blockInfo[bi++] );

        size_t numBlocks( blockInfo->numBlocks );

        if ( !numBlocks )
            continue;

        const size_t dataLength( blockInfo->k );
        const size_t eccLength( blockInfo->c - blockInfo->k );

        // create generator poly
        const poly gen( poly::reedSolomonGenerator( eccLength ) );

        // create blocks
        do
        {
            const poly data( buffer.substr( i, dataLength ) );

            poly divisor;
            divisor.append( data );
            divisor.append( poly( eccLength, 0 ) );

            // handle division
            for ( size_t d( 0 ); d < dataLength; ++d )
            {
                const uint8_t coef( divisor[d] );

                if ( coef )
                    for ( size_t g( 1 ); g < gen.length(); ++g )
                        divisor[d+g] ^= poly::mul( gen[g], coef );
            }

            // append block
            const poly eccData( divisor.substr( dataLength, eccLength ) );

            push_back( value_type( data, eccData ) );

            // update index
            i += dataLength;

        } while ( --numBlocks );

    } while ( bi < 2 ); // for each block info

}

///////////////////////////////////////////////////////////////////////////////////////////////////
eccBlockArray::~eccBlockArray()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
poly eccBlockArray::generate() const
{
    poly data;
    poly ecc;

    size_t i( 0 );

    // append data
    for ( ;; )
    {
        const size_t length( data.length() + ecc.length() );

        // interleave blocks
        for ( const_iterator b( begin() ); b != end(); ++b )
        {
            if ( i < b->first.length() )
                data.push_back( b->first[i] );

            if ( i < b->second.length() )
                ecc.push_back( b->second[i] );
        }

        if ( length == (data.length() + ecc.length()) )
            break;

        ++i;
    }

    data.append( ecc );

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

