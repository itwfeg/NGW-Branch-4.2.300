/**
 * @file bluenicadata.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "bluenicaconfigdatarec.h"
#include "bluenicadatarec.h"
#include "bluenicatempdatarec.h"
#include "leadvertinfo.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaDataRec::bluenicaDataRec()
{
    reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaDataRec::~bluenicaDataRec()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaDataRec::print() const
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bluenicaDataRec::isBluenicaSensor( const leAdvertInfo &value )
{
    int valid( 0 );

    // validate advert
    if ( value.isValid() )
    {
        std::list<std::string> eirs( value.eirs() );

        // evaluate eirs
        // looking for 0x08 and 0xFF sections
        for ( std::list<std::string>::const_iterator eir( eirs.begin() ); eir != eirs.end(); ++eir )
        {
            // validate short name
            // should be in the form B#a or b#a for sensors
            if ( GAP_ADTYPE_LOCAL_NAME_SHORT == eir->at( 0 ) )
                if ( 4 == eir->length() )
                    if (( 'B' == eir->at( 1 ) ) || ( 'b' == eir->at( 1 ) ))
                        if ( '#' == eir->at( 2 ) )
                            ++valid;

            // validate manf data
            // should be in the form 0d 00 be eX XX XX XX XX
            if ( GAP_ADTYPE_MANUFACTURER_SPECIFIC == eir->at( 0 ) )
                if ( 21 <= eir->length() )
                    if (( 0x0d == eir->at( 1 ) ) && ( 0x00 == eir->at( 2 ) ) &&
                        ( 0xbe == eir->at( 3 ) ) && ( 0xe0 == (0xf0 & eir->at( 4 )) ))
                            ++valid;
        }
    }

    return (2 == valid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaDataRec *bluenicaDataRec::decode( const leAdvertInfo &value )
{
    bluenicaDataRec *record( NULL );

    // check for configuration broadcast
    if ( bluenicaConfigDataRec::isTypeOf( value ) )
        record = new bluenicaConfigDataRec( value );

    // check for temperature broadcast
    else if ( bluenicaTempDataRec::isTypeOf( value ) )
        record = new bluenicaTempDataRec( value );

    return record;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaDataRec::copy( const _Myt &other )
{
    timestamp_ = other.timestamp_;

    senderAddress_ = other.senderAddress_;
    address_ = other.address_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaDataRec::reset()
{
    timestamp_ = 0;

    senderAddress_.clear();
    address_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaDataRec::decrypt( std::string &eir )
{
    static const size_t bufferOffset( 11 );
    static const size_t seedOffset( 17 );

    // sanity check
    if ( eir.length() < seedOffset )
        return;

    unsigned char seed( (unsigned char) eir.at( seedOffset ) );

    // process each byte (except seed byte)
    for ( size_t i( bufferOffset ); i < eir.length(); ++i )
        if ( seedOffset != i )
        {
            unsigned char ofs( i - bufferOffset );
            unsigned char mod( (ofs + 7) ^ seed );

            eir[i] = (char) (((unsigned char) eir[i]) + ofs - mod);
        }
}
