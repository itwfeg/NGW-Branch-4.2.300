/**
 * @file dmmodedata.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "dmmodedata.h"

#include <list>
#include <map>
#include <vector>

/// Data Matrix Barcode Namespace
namespace dm
{

// ================================================================================================
//
// modeData
//
// ================================================================================================
modeData::modeData( mode::scheme s ) :
    scheme_( s )
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
mode::scheme modeData::modeScheme() const
{
    return scheme_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void modeData::assign( const _Myt &other )
{
    scheme_ = other.scheme_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitstring modeData::generate() const
{
    bitstring result;
    result.append( scheme_, 8 );

    return result;
}

// ================================================================================================
//
// eciModeData
//
// ================================================================================================
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
void eciModeData::assign( const _Myt &other )
{
    _Mybase::assign( other );

    eci_ = other.eci_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitstring eciModeData::generate() const
{
    bitstring result( _Mybase::generate() );

    if ( eci_ <= 126 )
        result.append( eci_ + 1, 8 );
    else if ( eci_ <= 16382 )
    {
        result.append( ((eci_ - 127) / 254) + 128, 8 );
        result.append( ((eci_ -127) % 254) + 1, 8 );
    }
    else if ( eci_ <= 999999 )
    {
        result.append( ((eci_ - 16383) / 64516) + 192, 8 );
        result.append( (((eci_ - 16383) / 254) % 254) + 1, 8 );
        result.append( ((eci_ - 16383) % 254) + 1, 8 );
    }
    else
    {
        return bitstring();
    }

    return result;
}

// ================================================================================================
//
// paddingModeData
//
// ================================================================================================
paddingModeData::paddingModeData( unsigned int pos ) :
    _Mybase( mode::Padding ),
    pos_( pos )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
paddingModeData::paddingModeData( const _Myt &other ) :
    _Mybase( mode::Padding )
{
    assign( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
paddingModeData::~paddingModeData()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void paddingModeData::assign( const _Myt &other )
{
    _Mybase::assign( other );

    pos_ = other.pos_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitstring paddingModeData::generate() const
{
    bitstring result;

    if ( !pos_ )
        result.append( scheme_, 8 );

    // use 253-state algorithm for pattern randomization
    else
    {
        unsigned int value( scheme_ + ((149 * pos_) % 253) + 1 );

        if ( 254 < value )
            value -= 254;

        result.append( value, 8 );
    }

    return result;
}

// ================================================================================================
//
// encodedModeData
//
// ================================================================================================
encodedModeData::encodedModeData( mode::scheme s ) :
    _Mybase( s )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
encodedModeData::encodedModeData( mode::scheme s, const std::string &data ) :
    _Mybase( s )
{
    setData( data );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
encodedModeData::encodedModeData( const _Myt &other ) :
    _Mybase( other )
{
    assign( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
encodedModeData::~encodedModeData()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
bitstring encodedModeData::encode<mode::C40>( const std::string &data ) const
{
    bitstring result;

    static const char basic_set[40] = {
        -1, -1, -1, 32, 48, 49, 50, 51, 52, 53,
        54, 55, 56, 57, 65, 66, 67, 68, 69, 70,
        71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90
    };

    static const char shift1_set[40] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, -1, -1, -1, -1, -1, -1, -1, -1
    };

    static const char shift2_set[40] = {
        33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
        43, 44, 45, 46, 47, 58, 59, 60, 61, 62,
        63, 64, 91, 92, 93, 94, 95, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };

    static const char shift3_set[40] = {
        96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
        106, 107, 108, 109, 110, 111, 112, 113, 114, 115,
        116, 117, 118, 119, 120, 121, 122, 123, 124, 125,
        126, 127, -1, -1, -1, -1, -1, -1, -1, -1
    };

    // generate encoded values
    std::vector<unsigned int> encodedValues;

    for ( std::string::const_iterator i( data.begin() ); i != data.end(); ++i )
    {
        unsigned char c( (unsigned char)(*i) );

        // check for upper shift
        if ( 128 <= c )
        {
            encodedValues.push_back( 1 );
            encodedValues.push_back( 30 );
            c -= 128;
        }

        // find encoded value
        for ( unsigned int j( 40 ); j--; )
        {
            if ( basic_set[j] == c )
            {
                encodedValues.push_back( j );
                break;
            }
            else if ( shift1_set[j] == c )
            {
                encodedValues.push_back( 0 );
                encodedValues.push_back( j );
                break;
            }
            else if ( shift2_set[j] == c )
            {
                encodedValues.push_back( 1 );
                encodedValues.push_back( j );
                break;
            }
            else if ( shift3_set[j] == c )
            {
                encodedValues.push_back( 2 );
                encodedValues.push_back( j );
                break;
            }
        }
    }

    if ( encodedValues.size() )
    {
        while ( 0 != (encodedValues.size() % 3) )
            encodedValues.push_back( 0 );

        // latch to encoding
        result.append( mode::C40, 8 );

        // encode string
        for ( size_t pos( 0 ); pos < encodedValues.size(); pos += 3 )
            result.append( 1600*encodedValues[pos] + 40*encodedValues[pos+1] + encodedValues[pos+2] + 1, 16 );

        // unlatch
        result.append( 254, 8 );
    }

    return result;
}

template <>
bitstring encodedModeData::encode<mode::Text>( const std::string &data ) const
{
    bitstring result;

    static const char basic_set[40] = {
        -1, -1, -1, 32, 48, 49, 50, 51, 52, 53,
        54, 55, 56, 57, 97, 98, 99, 100, 101, 102,
        103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
        113, 114, 115, 116, 117, 118, 119, 120, 121, 122
    };

    static const char shift1_set[40] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, -1, -1, -1, -1, -1, -1, -1, -1
    };


    static const char shift2_set[40] = {
        33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
        43, 44, 45, 46, 47, 58, 59, 60, 61, 62,
        63, 64, 91, 92, 93, 94, 95, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };

    static const char shift3_set[40] = {
        96, 65, 66, 67, 68, 69, 70, 71, 72, 73,
        74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
        84, 85, 86, 87, 88, 89, 90, 123, 124, 125,
        126, 127, -1, -1, -1, -1, -1, -1
    };

    // generate encoded values
    std::vector<unsigned int> encodedValues;

    for ( std::string::const_iterator i( data.begin() ); i != data.end(); ++i )
    {
        unsigned char c( (unsigned char)(*i) );

        // check for upper shift
        if ( 128 <= c )
        {
            encodedValues.push_back( 1 );
            encodedValues.push_back( 30 );
            c -= 128;
        }

        // find encoded value
        for ( unsigned int j( 40 ); j--; )
        {
            if ( basic_set[j] == c )
            {
                encodedValues.push_back( j );
                break;
            }
            else if ( shift1_set[j] == c )
            {
                encodedValues.push_back( 0 );
                encodedValues.push_back( j );
                break;
            }
            else if ( shift2_set[j] == c )
            {
                encodedValues.push_back( 1 );
                encodedValues.push_back( j );
                break;
            }
            else if ( shift3_set[j] == c )
            {
                encodedValues.push_back( 2 );
                encodedValues.push_back( j );
                break;
            }
        }
    }

    if ( encodedValues.size() )
    {
        while ( 0 != (encodedValues.size() % 3) )
            encodedValues.push_back( 0 );

        // latch to encoding
        result.append( mode::Text, 8 );

        // encode string
        for ( size_t pos( 0 ); pos < encodedValues.size(); pos += 3 )
            result.append( 1600*encodedValues[pos] + 40*encodedValues[pos+1] + encodedValues[pos+2] + 1, 16 );

        // unlatch
        result.append( 254, 8 );
    }

    return result;
}

template <>
bitstring encodedModeData::encode<mode::X12>( const std::string &data ) const
{
    bitstring result;

    static const char basic_set[40] = {
        13, 42, 62, 32, 48, 49, 50, 51, 52, 53,
        54, 55, 56, 57, 65, 66, 67, 68, 69, 70,
        71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90
    };

    // generate encoded values
    std::vector<unsigned int> encodedValues;

    for ( std::string::const_iterator i( data.begin() ); i != data.end(); ++i )
    {
        unsigned char c( (unsigned char)(*i) );

        // find encoded value
        for ( unsigned int j( 40 ); j--; )
            if ( basic_set[j] == c )
            {
                encodedValues.push_back( j );
                break;
            }

    }

    if ( encodedValues.size() )
    {
        size_t extra( 0 );

        while ( 0 != (encodedValues.size() % 3) )
        {
            encodedValues.pop_back();
            ++extra;
        }

        // latch to encoding
        result.append( mode::X12, 8 );

        // encode string
        for ( size_t pos( 0 ); pos < encodedValues.size(); pos += 3 )
            result.append( 1600*encodedValues[pos] + 40*encodedValues[pos+1] + encodedValues[pos+2] + 1, 16 );

        // unlatch
        result.append( 254, 8 );

        // append extra characters in ASCII encoding
        while ( extra )
        {
            result.append( data[data.length() - extra] + 1, 8 );
            --extra;
        }
    }

    return result;
}

template <>
bitstring encodedModeData::encode<mode::EDIFACT>( const std::string &data ) const
{
    bitstring result;

    // generate encoded values
    std::vector<unsigned int> encodedValues;

    for ( std::string::const_iterator i( data.begin() ); i != data.end(); ++i )
        encodedValues.push_back( 0x3f & (unsigned char)(*i) );

    if ( encodedValues.size() )
    {
        // unlatch character
        encodedValues.push_back( 31 );

        while ( 0 != (encodedValues.size() % 4) )
            encodedValues.push_back( 0 );

        // latch to encoding
        result.append( mode::EDIFACT, 8 );

        // encode string
        for ( size_t pos( 0 ); pos < encodedValues.size(); pos += 4 )
        {
            unsigned int value( 0 );
            value += encodedValues[pos];
            value <<= 6;
            value += encodedValues[pos+1];
            value <<= 6;
            value += encodedValues[pos+2];
            value <<= 6;
            value += encodedValues[pos+3];

            result.append( value, 24 );
        }
    }

    // remove ending null byte(s)
    for ( ;; )
    {
        const size_t len( result.length() );

        if (( len < 8 ) ||
            ( result[len-8] ) || ( result[len-7] ) ||( result[len-6] ) ||( result[len-5] ) ||
            ( result[len-4] ) || ( result[len-3] ) ||( result[len-2] ) ||( result[len-1] ))
            break;

        result.resize( len-8 );
    }

    return result;
}

template <>
bitstring encodedModeData::encode<mode::Base256>( const std::string &data ) const
{
    bitstring result;

    if (( 0 < data.size() ) && ( data.size() <= 1555 ))
    {
        const unsigned int length( (unsigned int) data.size() );

        // latch to encoding
        result.append( mode::Base256, 8 );

        // encode length
        if ( data.size() <= 249 )
            result.append( length, 8 );
        else
        {
            result.append( (length / 250) + 249, 8 );
            result.append( length % 250, 8 );
        }

        for ( std::string::const_iterator i( data.begin() ); i != data.end(); ++i )
            result.append( (unsigned char)(*i), 8 );
    }

    return result;
}

template <>
bitstring encodedModeData::encode<mode::ASCII>( const std::string &data ) const
{
    bitstring result;

    for ( std::string::const_iterator i( data.begin() ); i != data.end(); ++i )
    {
        unsigned char c( (unsigned char)(*i) );

        if ( 127 < c )
        {
            result.append( 235, 8 );
            c -= 127;
        }

        result.append( c + 1, 8 );
    }

    return result;
}

template <>
bitstring encodedModeData::encode<mode::Numeric>( const std::string &data ) const
{
    bitstring result;

    // encode 2-digit data
    for ( size_t pos( 2 ); pos <= data.length(); pos += 2 )
    {
        const unsigned int value( (10 * (data[pos-2] - '0')) + (data[pos-1] - '0') );

        result.append( value + 130, 8 );
    }

    // encode remainder digit
    if ( 0 != (data.length()%2) )
    {
        unsigned char c( (unsigned char) data.back() );

        result.append( c + 1, 8 );
    }

    return result;
}

bool encodedModeData::setData( const std::string &data )
{
    std::list<bitstring> encodations;

    // encode
    if (( mode::C40 == scheme_ ) || ( mode::Auto == scheme_ ))
        encodations.push_back( encode<mode::C40>( data ) );

    if (( mode::Base256 == scheme_ ) || ( mode::Auto == scheme_ ))
        encodations.push_back( encode<mode::Base256>( data ) );

    if ( isX12( data ) )
        if (( mode::X12 == scheme_ ) || ( mode::Auto == scheme_ ))
            encodations.push_back( encode<mode::X12>( data ) );

    if (( mode::Text == scheme_ ) || ( mode::Auto == scheme_ ))
        encodations.push_back( encode<mode::Text>( data ) );

    if ( isEDIFACT( data ) )
        if (( mode::EDIFACT == scheme_ ) || ( mode::Auto == scheme_ ))
            encodations.push_back( encode<mode::EDIFACT>( data ) );

    if (( mode::ASCII == scheme_ ) || ( mode::Auto == scheme_ ))
        encodations.push_back( encode<mode::ASCII>( data ) );

    if ( isNumeric( data ) )
        if (( mode::Numeric == scheme_ ) || ( mode::Auto == scheme_ ))
            encodations.push_back( encode<mode::Numeric>( data ) );

    // choose smallest encodation
    data_.clear();

    for ( std::list<bitstring>::const_iterator i( encodations.begin() ); i != encodations.end(); ++i )
        if ( i->length() )
            if (( !data_.length() ) || ( i->length() < data_.length() ))
                data_ = (*i);

    return (data_.length() ? true : false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void encodedModeData::assign( const _Myt &other )
{
    _Mybase::assign( other );

    data_ = other.data_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitstring encodedModeData::generate() const
{
    return data_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool encodedModeData::isEDIFACT( const std::string &data )
{
    static const std::string allowed( "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^ !\"#$%&'()*+,-./0123456789:;<=>?" );

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
bool encodedModeData::isX12( const std::string &data )
{
    static const std::string allowed( "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\x13\x42\x62" );

    for ( std::string::const_iterator d( data.begin() ); d != data.end(); ++d )
        if ( std::string::npos == allowed.find( *d ) )
            return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
