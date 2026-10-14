/**
 * @file dmmode.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "dmmode.h"
#include "dmmodedata.h"

/// Data Matrix Barcode Namespace
namespace dm
{

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData *mode::create( scheme s )
{
    switch ( s )
    {
    case FNC1:
    case StructuredAppend:
        return new modeData( s );
    default:
        break;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData *mode::create( scheme s, unsigned int value )
{
    switch ( s )
    {
    case Padding:
        return new paddingModeData( value );
    case ECI:
        return new eciModeData( value );
    default:
        break;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
modeData *mode::create( scheme s, const std::string &value )
{
    switch ( s )
    {
    case C40:
    case Base256:
    case X12:
    case Text:
    case EDIFACT:
    case ASCII:
    case Numeric:
    case Auto:
        return new encodedModeData( s, value );
    default:
        break;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mode::test()
{
    modeData *temp;
    bitstring result;

    // ---- //

    static const bool test_541_000099_r[] = {
        1, 1, 1, 1, 0, 0, 0, 1,
        0, 1, 1, 0, 0, 1, 0, 0
    };

    static const bool test_541_015000_r[] = {
        1, 1, 1, 1, 0, 0, 0, 1,
        1, 0, 1, 1, 1, 0, 1, 0,
        1, 0, 0, 0, 1, 1, 1, 0
    };

    static const bool test_541_090000_r[] = {
        1, 1, 1, 1, 0, 0, 0, 1,
        1, 1, 0, 0, 0, 0, 0, 1,
        0, 0, 1, 0, 0, 1, 0, 0,
        1, 1, 0, 1, 0, 1, 0, 0
    };

    // test 5.4.1 015000
    result.clear();

    temp = create( ECI, 15000 );
    result.append( temp->generate() );
    delete temp;

    if ( sizeof( test_541_015000_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_541_015000_r[index++] != (*i) )
                return false;
    }

    // test 5.4.1 090000
    result.clear();

    temp = create( ECI, 90000 );
    result.append( temp->generate() );
    delete temp;

    if ( sizeof( test_541_090000_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_541_090000_r[index++] != (*i) )
                return false;
    }

    // test eci
    result.clear();

    temp = create( ECI, 99 );
    result.append( temp->generate() );
    delete temp;

    if ( sizeof( test_541_000099_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_541_000099_r[index++] != (*i) )
                return false;
    }

    // ---- //

    static const bool test_5252_r[] = {
        1, 1, 1, 0, 0, 1, 1, 0,
        0, 1, 0, 1, 1, 0, 1, 1,
        0, 0, 0, 0, 1, 0, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 0
    };

    // test 5.2.5.2
    result.clear();

    temp = create( C40, "AIM" );
    result.append( temp->generate() );
    delete temp;

    if ( sizeof( test_5252_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_5252_r[index++] != (*i) )
                return false;
    }

    // ---- //

    static const bool test_5282_r[] = {
        1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 1, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 1,
        0, 1, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    // test 5.2.8.2
    result.clear();

    temp = create( EDIFACT, "DATA" );
    result.append( temp->generate() );
    delete temp;

    if ( sizeof( test_5282_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_5282_r[index++] != (*i) )
                return false;
    }

    // ---- //

    static const bool test_O_r[] = {
        1, 0, 0, 0, 1, 1, 1, 0,
        1, 0, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 1, 0, 1, 0
    };

    // test Annex O
    result.clear();

    temp = create( Numeric, "123456" );
    result.append( temp->generate() );
    delete temp;

    if ( sizeof( test_O_r ) != result.length() )
        return false;
    else
    {
        size_t index( 0 );

        for ( bitstring::const_iterator i( result.begin() ); i != result.end(); ++i )
            if ( test_O_r[index++] != (*i) )
                return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

