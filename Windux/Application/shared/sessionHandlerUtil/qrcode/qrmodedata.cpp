/**
 * @file qrmodedata.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "qrmodedata.h"

#include <cstdlib>

/// QR Barcode Namespace
namespace qr
{

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData::modeData( mode::dataType mdt ) :
    type_( mdt )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData::modeData( const _Myt &other )
{
    assign( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData::~modeData()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData &modeData::operator = ( const _Myt &rhs )
{
    assign( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t modeData::maxLength() const
{
    return 4;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mode::dataType modeData::modeDataType() const
{
    return type_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void modeData::assign( const _Myt &other )
{
    type_ = other.type_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitstring modeData::generate( version /*ver*/ ) const
{
    bitstring result;
    result.append( type_, 4 );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eciModeData::eciModeData( unsigned int eci ) :
    _Mybase( mode::ECI ),
    eci_( eci )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eciModeData::eciModeData( const _Myt &other ) :
    _Mybase( mode::ECI )
{
    assign( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eciModeData::~eciModeData()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t eciModeData::maxLength() const
{
    return _Mybase::maxLength() + 24;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void eciModeData::assign( const _Myt &other )
{
    _Mybase::assign( other );

    eci_ = other.eci_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitstring eciModeData::generate( version ver ) const
{
    bitstring result( _Mybase::generate( ver ) );

    if ( eci_ <= 127 )
        result.append( eci_, 8 );
    else if ( eci_ <= 16383 )
        result.append( eci_ | 0x8000, 16 );
    else if ( eci_ <= 999999 )
        result.append( eci_ | 0xc00000, 24 );
    else
    {
        return bitstring();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
encodedModeData::encodedModeData( mode::dataType mdt ) :
    _Mybase( mdt ),
    characterCount_( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
encodedModeData::encodedModeData( mode::dataType mdt, const std::string &data ) :
    _Mybase( mdt ),
    characterCount_( 0 )
{
    setData( data );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
encodedModeData::encodedModeData( const _Myt &other ) :
    _Mybase( other ),
    characterCount_( 0 )
{
    assign( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
encodedModeData::~encodedModeData()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t encodedModeData::characterCountLen( version ver ) const
{
    static const size_t data[][16] = {
        {  0, 10,  9,  0,  8,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0 },
        {  0, 12, 11,  0, 16,  0,  0,  0, 10,  0,  0,  0,  0,  0,  0,  0 },
        {  0, 14, 13,  0, 16,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0 },
    };

    // compute number of bits needed for length
    if ( ver <= Ver9 )
        return data[0][type_];
    else if ( ver <= Ver26 )
        return data[1][type_];
    else if ( ver <= Ver40 )
        return data[2][type_];

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t encodedModeData::maxLength() const
{
    return _Mybase::maxLength() + characterCountLen( Ver40 ) + data_.length();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void encodedModeData::encode<mode::Numeric>( const std::string &data )
{
    size_t pos( 0 );
    size_t nextPos( 3 );

    while ( pos < data.length() )
    {
        size_t len( 3 );

        // compute length
        if ( data.length() < nextPos )
            len -= nextPos - data.length();

        // convert to value
        const std::string s( data.substr( pos, len ) );
        const unsigned int val( std::strtoul( s.c_str(), NULL, 10 ) );

        // append
        switch ( len )
        {
        case 3: data_.append( val, 10 ); break;
        case 2: data_.append( val, 7 ); break;
        case 1: data_.append( val, 4 ); break;
        default:
            break;
        }

        // increment pos
        pos = nextPos;
        nextPos += 3;

    }
}

template <>
void encodedModeData::encode<mode::Alphanumeric>( const std::string &data )
{
    static const unsigned int values[] = {
         0,  0,  0,  0,  0,  0,  0,  0, // 0
         0,  0,  0,  0,  0,  0,  0,  0, // 8
         0,  0,  0,  0,  0,  0,  0,  0, // 16
         0,  0,  0,  0,  0,  0,  0,  0, // 24
        36,  0,  0,  0, 37, 38,  0,  0, // 32
         0,  0, 39, 40,  0, 41, 42, 43, // 40
         0,  1,  2,  3,  4,  5,  6,  7, // 48
         8,  9, 44,  0,  0,  0,  0,  0, // 56
         0, 10, 11, 12, 13, 14, 15, 16, // 64
        17, 18, 19, 20, 21, 22, 23, 24, // 72
        25, 26, 27, 28, 29, 30, 31, 32, // 80
        33, 34, 35,  0,  0,  0,  0,  0, // 88
         0,  0,  0,  0,  0,  0,  0,  0, // 96
         0,  0,  0,  0,  0,  0,  0,  0, // 104
         0,  0,  0,  0,  0,  0,  0,  0, // 112
         0,  0,  0,  0,  0,  0,  0,  0, // 120
    };

    const char *s( data.c_str() );

    size_t pos( 0 );
    size_t nextPos( 2 );

    while ( pos < data.length() )
    {
        unsigned int len( 2 );

        // compute length
        if ( data.length() < nextPos )
            len = 1;

        // append
        switch ( len )
        {
        case 2: data_.append( 45 * values[s[pos]] + values[s[pos+1]], 11 ); break;
        case 1: data_.append( values[s[pos]], 6 ); break;
        default:
            break;
        }

        // increment pos
        pos = nextPos;
        nextPos += 2;

    }
}

template <>
void encodedModeData::encode<mode::Byte>( const std::string &data )
{
    data_.append( data );
}

bool encodedModeData::setData( const std::string &data )
{
    // encode data
    switch ( type_ )
    {
    case mode::Numeric:
        encode<mode::Numeric>( data );
        break;
    case mode::Alphanumeric:
        encode<mode::Alphanumeric>( data );
        break;
    case mode::Byte:
        encode<mode::Byte>( data );
        break;
/*
    case mode::Kanji:
        encode<mode::Kanji>( data );
        break;
*/
    default:
        return false;
    }

    // store character count
    characterCount_ = data.length();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void encodedModeData::assign( const _Myt &other )
{
    _Mybase::assign( other );

    characterCount_ = other.characterCount_;
    data_ = other.data_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitstring encodedModeData::generate( version ver ) const
{
    bitstring result( _Mybase::generate( ver ) );
    result.append( characterCount_, characterCountLen( ver ) );
    result.append( data_ );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool encodedModeData::isAlphanumeric( const std::string &data )
{
    static const std::string allowed( "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:" );

    for ( std::string::const_iterator d( data.begin() ); d != data.end(); ++d )
        if ( std::string::npos == allowed.find( *d ) )
            return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool encodedModeData::isNumeric( const std::string &data )
{
    static const std::string allowed( "0123456789" );

    for ( std::string::const_iterator d( data.begin() ); d != data.end(); ++d )
        if ( std::string::npos == allowed.find( *d ) )
            return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
