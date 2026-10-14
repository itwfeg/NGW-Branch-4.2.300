/**
 * @file qrapi.cpp
 * QR API Entry Point.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "poly.h"
#include "qrapi.h"
#include "qrecc.h"
#include "qrmetrics.h"
#include "qrmode.h"
#include "qrmodedata.h"
#include "qrmodule.h"

#include <bitmap.h>
#include <bitstring.h>

using namespace qr;

///////////////////////////////////////////////////////////////////////////////////////////////////
version qrapi::encode( const std::string &s, eccLevel level, bitmap &b )
{
    version ver( (version) -1 );

    // generate matrix
    moduleMatrix *matrix( generate( s, level, ver ) );

    // render
    if ( matrix )
    {
        const size_t mps( matrix->modulesPerSide() );

        matrix->render( (b = bitmap( mps, mps )), 0, 0, mps, mps );
        delete matrix;
    }

    return ver;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
version qrapi::encode( const std::string &s, qr::eccLevel level, bitmap &b, size_t x, size_t y, size_t width, size_t height )
{
    version ver( (version) -1 );

    // generate matrix
    moduleMatrix *matrix( generate( s, level, ver ) );

    // render
    if ( matrix )
    {
        matrix->render( b, x, y, width, height );
        delete matrix;
    }

    return ver;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix *qrapi::generate( const std::string &s, eccLevel level, version &ver )
{
    moduleMatrix *matrix( NULL );

    bitstring message;
    poly p;

    // determine type
    mode::dataType type( mode::Byte );

    if ( encodedModeData::isNumeric( s ) )
        type = mode::Numeric;
    else if ( encodedModeData::isAlphanumeric( s ) )
        type = mode::Alphanumeric;

    // generate mode data
    typedef std::list<modeData*> modeDataList;

    modeDataList data;
    data.push_back( mode::create( type, s ) );
    data.push_back( mode::create( mode::Terminator ) );

    // calculate max length and version
    size_t maxLength( 0 );
    size_t v( Ver1 );

    for ( modeDataList::iterator d( data.begin() ); d != data.end(); ++d )
    {
        if ( !(*d) )
            goto done;

        maxLength += (*d)->maxLength();
    }

    do
    {
        const dataCapacityInfo dci( metrics::dataCapacity( (version) v ) );
        const eccCharacteristicsInfo ecci( metrics::eccCharacteristics( (version) v, level ) );

        if ( maxLength <= (8 * (dci.dataCodewordCapacity - ecci.numCodewords)) )
            break;

    } while ( ++v < Ver40 );

    // generate message and encode
    for ( modeDataList::iterator d( data.begin() ); d != data.end(); ++d )
        message.append( (*d)->generate( (version) v ) );

    if ( !ecc::encode( message, (version) v, level, p ) )
        goto done;
    else
    {
        // generate matrix
        matrix = new moduleMatrix( (ver = (version) v), level, p );
    }

done:

    // free memory
    for ( modeDataList::iterator d( data.begin() ); d != data.end(); ++d )
        if ( *d )
            delete (*d);

    return matrix;
}
