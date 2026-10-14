/**
 * @file dmapi.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "dmapi.h"
#include "dmecc.h"
#include "dmmetrics.h"
#include "dmmode.h"
#include "dmmodedata.h"
#include "dmmodule.h"

#include <rbitmap.h>
#include <bitstring.h>

#include <sstream>

using namespace dm;

// FNC1
char dmapi::fnc1_( -1 );

///////////////////////////////////////////////////////////////////////////////////////////////////
void dmapi::clearFNC1()
{
    fnc1_ = -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
symbolSize dmapi::encode( const std::string &s, eccLevel level, bitmap &b )
{
    symbolSize size( (symbolSize) -1 );

    // generate matrix
    moduleMatrix *matrix( generate( s, level, size ) );

    // render
    if ( matrix )
    {
        matrix->render( (b = bitmap( matrix->width(), matrix->height() )), 0, 0, matrix->width(), matrix->height() );
        delete matrix;
    }

    return size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
symbolSize dmapi::encode( const std::string &s, dm::eccLevel level, bitmap &b, size_t x, size_t y, size_t width, size_t height )
{
    symbolSize size( (symbolSize) -1 );

    // generate matrix
    moduleMatrix *matrix( generate( s, level, size ) );

    // render
    if ( matrix )
    {
        matrix->render( b, x, y, width, height );
        delete matrix;
    }

    return size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dmapi::setFNC1( char c )
{
    fnc1_ = c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix *dmapi::generate( const std::string &s, eccLevel level, symbolSize &size )
{
    moduleMatrix *matrix( NULL );

    // split strings by FNC1
    typedef std::list<std::string>stringList;
    stringList strings;

    if ( -1 == fnc1_ )
        strings.push_back( s );
    else
    {
        std::istringstream f( s );
        std::string substring;

        while ( std::getline( f, substring, fnc1_ ) )
            strings.push_back( substring );
    }

    // generate mode data
    typedef std::list<modeData*> modeDataList;
    modeDataList data;

    for ( stringList::const_iterator i( strings.begin() ); i != strings.end(); ++i )
    {
        if ( !data.empty() )
            data.push_back( mode::create( mode::FNC1 ) );

        data.push_back( mode::create( mode::Auto, *i ) );
    }

    // generate message and calculate symbol size
    size_t ss( SymbolSize_10x10 );
    bitstring message;

    for ( modeDataList::iterator d( data.begin() ); d != data.end(); ++d )
        message.append( (*d)->generate() );

    do
    {
        const eccCharacteristicsInfo ecci( metrics::eccCharacteristics( level, (symbolSize) ss ) );

        // check message fits within this symbology
        const size_t dataLength( 8 * ecci.totalDataCodewords );

        if ( message.length() <= dataLength )
        {
            // pad message to total length
            if ( message.length() < dataLength )
            {
                static const paddingModeData padding0( 0 );
                message.append( padding0.generate() );

                while ( message.length() < dataLength )
                {
                    const paddingModeData padding( (unsigned int) ((message.length() / 8) + 1) );
                    message.append( padding.generate() );
                }
            }

            break;
        }

    } while ( ++ss <= SymbolSize_144x144 );

    // validate symbology
    buffer result;

    if ( SymbolSize_144x144 < ss )
        goto done;

    // encode
    if ( !ecc::encode( message, level, result ) )
        goto done;
    else
    {
        size = (symbolSize) ss;

        // generate matrix
        matrix = new moduleMatrix( size, result );
    }

done:

    // free memory
    for ( modeDataList::iterator d( data.begin() ); d != data.end(); ++d )
        if ( *d )
            delete (*d);

    return matrix;
}
