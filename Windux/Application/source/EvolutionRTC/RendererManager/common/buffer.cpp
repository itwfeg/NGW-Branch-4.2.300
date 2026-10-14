/**
 * @file buffer.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "bitstring.h"
#include "buffer.h"

#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
buffer::buffer() :
    _Mybase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
buffer::buffer( const value_type *b, size_t bs ) :
    _Mybase( b, bs )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
buffer::buffer( size_t n, value_type val ) :
    _Mybase( n, val )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
buffer::buffer( const _Myt &other ) :
    _Mybase( other )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
buffer::buffer( const _Mybase &other ) :
    _Mybase( other )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
buffer::buffer( const bitstring &s, size_t len ) :
    _Mybase()
{
    // add data to buffer
    value_type codeword( 0 );

    for ( size_t i( 0 ); i < s.length(); )
    {
        codeword <<= 1;
        codeword |= (s[i++] ? 1 : 0);

        if ( 0 == (i % 8) )
        {
            push_back( codeword );
            codeword = 0;
        }
    }

    size_t remainder( 0x07 & (8 - (s.length() % 8)) );

    if ( remainder )
        push_back( codeword <<= remainder );

    // add padding to buffer
    while ( length() < len )
        push_back( 0x00 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
buffer::~buffer()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool buffer::save( const std::string &filename ) const
{
    std::ofstream f( filename, std::fstream::out | std::fstream::binary );

    if ( !f.is_open() )
        return false;

    f.write( (char*) c_str(), length() );
    f.close();

    return true;
}

