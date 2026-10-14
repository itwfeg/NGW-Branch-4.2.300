/**
 * @file eanstring.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "eanstring.h"

#include <bitmap.h>

#include <iostream>

/// EAN Barcode Namespace
namespace ean
{

const unsigned int string::leftEven_[10] = {0x27, 0x33, 0x1b, 0x21, 0x1d, 0x39, 0x05, 0x11, 0x09, 0x17};
const unsigned int string::leftOdd_[10] = {0x0d, 0x19, 0x13, 0x3d, 0x23, 0x31, 0x2f, 0x3b, 0x37, 0x0b};
const unsigned int string::right_[10] = {0x72, 0x66, 0x6c, 0x42, 0x5c, 0x4e, 0x50, 0x44, 0x48, 0x74};

const string::parity string::parity_[10][6] = {
    {string::leftOdd,  string::leftOdd,  string::leftOdd,  string::leftOdd,  string::leftOdd,  string::leftOdd},
    {string::leftOdd,  string::leftOdd,  string::leftEven, string::leftOdd,  string::leftEven, string::leftEven},
    {string::leftOdd,  string::leftOdd,  string::leftEven, string::leftEven, string::leftOdd,  string::leftEven},
    {string::leftOdd,  string::leftOdd,  string::leftEven, string::leftEven, string::leftEven, string::leftOdd},
    {string::leftOdd,  string::leftEven, string::leftOdd,  string::leftOdd,  string::leftEven, string::leftEven},
    {string::leftOdd,  string::leftEven, string::leftEven, string::leftOdd,  string::leftOdd,  string::leftEven},
    {string::leftOdd,  string::leftEven, string::leftEven, string::leftEven, string::leftOdd,  string::leftOdd},
    {string::leftOdd,  string::leftEven, string::leftOdd,  string::leftEven, string::leftOdd,  string::leftEven},
    {string::leftOdd,  string::leftEven, string::leftOdd,  string::leftEven, string::leftEven, string::leftOdd},
    {string::leftOdd,  string::leftEven, string::leftEven, string::leftOdd,  string::leftEven, string::leftOdd}};

const unsigned int string::ean8_checkDigitWeight_[7] = {3, 1, 3, 1, 3, 1, 3};
const unsigned int string::ean13_checkDigitWeight_[12] = {1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3};

///////////////////////////////////////////////////////////////////////////////////////////////////
string::string()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
string::string( const std::string &value, type t )
{
    generate( value, t );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
string::string( const _Myt &other )
{
    assign( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
string::~string()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
string &string::operator = ( const _Myt &rhs )
{
    assign( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void string::setData( const std::string &value, type t )
{
    generate( value, t );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void string::assign( const _Myt &other )
{
    s_ = other.s_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void string::print() const
{
    size_t height( (length() * 3) / 4 );

    while ( height-- )
    {
        for ( bitstring::const_iterator i( s_.begin() ); i != s_.end(); ++i )
            std::cout << ((*i) ? "X" : " ");

        std::cout << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void string::render( bitmap &canvas, size_t x, size_t y, size_t width, size_t height ) const
{
    // nothing to do
    if ( !s_.length() )
        return;

    size_t penWidth( width / s_.length() );

    // width is not big enough to hold barcode
    if ( !penWidth )
        return;

    // center barcode
    x += (width - (penWidth * s_.length())) / 2;

    // draw
    canvas.drawRect( x, y, width, height, canvas.white() );

    for ( bitstring::const_iterator i( s_.begin() ); i != s_.end(); ++i )
    {
        if ( *i )
            canvas.drawRect( x, y, penWidth, height, canvas.black() );

        x+= penWidth;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void string::generate( const std::string &value, type t )
{
    std::string code( value );

    // erase current barcode string
    s_.clear();

    // validate string length
    unsigned int length;

    if ( EAN13 == t )
        length = 12;
    else if ( EAN8 == t )
        length = 7;
    else
    {
        return;
    }

    while ( code.length() < length )
        code.insert( 0, 1, '0' );

    if ( length < code.length() )
        code.erase( 12 );

    // validate string contents
    for ( std::string::const_iterator i( code.begin() ); i != code.end(); ++i )
        if (( *i < '0' ) || ( '9' < *i ))
            return;

    // calc check digit
    code.push_back( calcCheckDigit( code, t ) );

    s_ = encode( code, t );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char string::calcCheckDigit( const std::string &value, type t )
{
    const unsigned int *multiplier( NULL );

    // determine multiplier table to use
    if ( EAN13 == t )
        multiplier = ean13_checkDigitWeight_;
    else if ( EAN8 == t )
        multiplier = ean8_checkDigitWeight_;

    // calculate checksum
    unsigned int checksum( 0 );

    for ( size_t i( 0 ); i < value.length(); ++i )
    {
        const unsigned int d( value[i] - '0' );

        checksum += d * multiplier[i];
    }

    if ( 0 == (checksum %= 10) )
        return '0';

    return ('0' + (10 - checksum));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitstring string::encode( const std::string &value, type t )
{
    bitstring s;

    // determine parity table to use
    const parity *parityTable( NULL );

    std::string left;
    std::string right;

    if ( EAN13 == t )
    {
        parityTable = parity_[ value[0] - '0' ];

        left = value.substr( 1, 6 );
        right = value.substr( 7, 6 );
    }
    else if ( EAN8 == t )
    {
        parityTable = parity_[0];

        left = value.substr( 0, 4 );
        right = value.substr( 4, 4 );
    }

    // add prefix
    s.append( 0x05, 3 );

    // add left side
    for ( size_t i( 0 ); i < left.length(); ++i )
    {
        const unsigned int v( left[i] - '0' );

        if ( leftEven == parityTable[i] )
            s.append( leftEven_[v], 7 );
        else if ( leftOdd == parityTable[i] )
            s.append( leftOdd_[v], 7 );
    }

    // add center marker
    s.append( 0x0a, 5 );

    // add right side
    for ( std::string::const_iterator i( right.begin() ); i < right.end(); ++i )
    {
        const unsigned int v( *i - '0' );

        s.append( right_[v], 7 );
    }

    // add suffix
    s.append( 0x05, 3 );

    return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
