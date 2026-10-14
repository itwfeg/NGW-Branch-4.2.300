/**
 * @file qrmode.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "qrmode.h"
#include "qrmodedata.h"

/// QR Barcode Namespace
namespace qr
{

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData *mode::create( dataType mdt )
{
    switch ( mdt )
    {
    case Terminator:
    case StructuredAppend:
    case FNC1_FirstPosition:
    case FNC1_SecondPosition:
        return new modeData( mdt );
    default:
        break;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData *mode::create( dataType mdt, unsigned int value )
{
    switch ( mdt )
    {
    case ECI:
        return new eciModeData( value );
    default:
        break;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData *mode::create( dataType mdt, const std::string &value )
{
    switch ( mdt )
    {
    case Numeric:
    case Alphanumeric:
    case Byte:
    case Kanji:
        return new encodedModeData( mdt, value );
    default:
        break;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mode::test()
{
    static const char test_8411[] = {'\xa1', '\xa2', '\xa3', '\xa4', '\xa5'};
    static const char test_842_1[] = {'0', '1', '2', '3', '4', '5', '6', '7'};
    static const char test_842_2[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5'};
    static const char test_843[] = {'A', 'C', '-', '4', '2'};

    static const bool test_8411_r[] = {
        0, 1, 1, 1,
        0, 0, 0, 0, 1, 0, 0, 1,
        0, 1, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0,
        1, 0, 1, 0, 0, 0, 1, 1,
        1, 0, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 0, 0, 1, 0, 1
    };

    static const bool test_842_1_r[] = {
        0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
        0, 1, 0, 1, 0, 1, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 1
    };

    static const bool test_842_2_r[] = {
        0, 0, 0, 1,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
        0, 1, 0, 1, 0, 1, 1, 0, 0, 1,
        1, 0, 1, 0, 1, 0, 0, 1, 1, 0,
        1, 1, 1, 0, 0, 0, 0, 1, 0, 1,
        0, 0, 1, 1, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1
    };

    static const bool test_843_r[] = {
        0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 1,
        0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0,
        1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1,
        0, 0, 0, 0, 1, 0
    };

    modeData *temp;
    bitstring result;

    // test 8.4.1.1
    result.clear();

    temp = create( ECI, 9 );
    result.append( temp->generate( Ver1 ) );
    delete temp;
    temp = create( Byte, test_8411 );
    result.append( temp->generate( Ver1 ) );
    delete temp;

    if ( sizeof( test_8411_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_8411_r[index++] != (*i) )
                return false;
    }

    // test 8.4.2 (1)
    result.clear();

    temp = create( Numeric, test_842_1 );
    result.append( temp->generate( Ver1 ) );
    delete temp;

    if ( sizeof( test_842_1_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_842_1_r[index++] != (*i) )
                return false;
    }

    // test 8.4.2 (2)
    result.clear();

    temp = create( Numeric, test_842_2 );
    result.append( temp->generate( Ver1 ) );
    delete temp;

    if ( sizeof( test_842_2_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_842_2_r[index++] != (*i) )
                return false;
    }

    // test 8.4.3
    result.clear();

    temp = create( Alphanumeric, test_843 );
    result.append( temp->generate( Ver1 ) );
    delete temp;

    if ( sizeof( test_843_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_843_r[index++] != (*i) )
                return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

