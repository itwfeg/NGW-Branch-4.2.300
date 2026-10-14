/**
 * @file bluenicatempdatarec.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "bluenicatempdatarec.h"
#include "leadvertinfo.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaTempDataRec::bluenicaTempDataRec( const leAdvertInfo &other )
{
    // init
    reset();

    // parse data
    if ( isTypeOf( other ) )
    {
        std::string eir( other.eir( GAP_ADTYPE_MANUFACTURER_SPECIFIC ) );

        if ( 20 < eir.length() )
        {
            char buffer[32];

            snprintf( buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X",
                (unsigned char) eir.at( 3 ),
                (unsigned char) eir.at( 4 ),
                (unsigned char) eir.at( 5 ),
                (unsigned char) eir.at( 6 ),
                (unsigned char) eir.at( 7 ),
                (unsigned char) eir.at( 8 ) );

            // decrypt the eir
            decrypt( eir );

            timestamp_ = other.timestamp();

            senderAddress_ = other.address();
            address_ = buffer;

            temp_ = eir.at( 12 );
            temp_ /= 100.0;
            temp_ += eir.at( 11 );

            batteryLevel_ = eir.at( 13 );

            recordId_ = (unsigned char) eir.at( 16 );
            recordId_ <<= 8;
            recordId_ += (unsigned char) eir.at( 15 );
            recordId_ <<= 8;
            recordId_ += (unsigned char) eir.at( 14 );

            clock_ = (unsigned char) eir.at( 19 );
            clock_ <<= 8;
            clock_ += (unsigned char) eir.at( 18 );
            clock_ <<= 8;
            clock_ += (unsigned char) eir.at( 17 );

            age_ = (unsigned char) eir.at( 20 );
            age_ *= 10;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaTempDataRec::bluenicaTempDataRec( const _Myt &other )
{
    copy( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaTempDataRec::~bluenicaTempDataRec()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaTempDataRec &bluenicaTempDataRec::operator = ( const _Myt &rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaTempDataRec::print() const
{
    std::cout << "**************************************************" << std::endl;

    std::cout << address_ << std::endl;
    std::cout << "record: " << recordId_ << " (" << timestamp_ << ")" << std::endl;

    if ( senderAddress_ != address_ )
        std::cout << "sender: " << senderAddress_ << std::endl;

    std::cout << "temp: " << temp_ << " C" << std::endl;
    std::cout << "batt: " << batteryLevel_ << " %" << std::endl;

    std::cout << "clock: " << clock_ << " (age " << age_ << ")" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bluenicaTempDataRec::isTypeOf( const leAdvertInfo &value )
{
    // must be a bluenica sensor
    if ( !_Mybase::isBluenicaSensor( value ) )
        return false;

    std::string eir( value.eir( GAP_ADTYPE_MANUFACTURER_SPECIFIC ) );

    if ( eir.length() < 10 )
        return false;

    // record type
    unsigned int type( eir.at( 10 ) );

    // we can only decode partner sensors
    // check for partner bit
    if ( !(0x80 & type) )
        return false;

    // check for temperature record type
    else if ( 1 != (type & 0x7f) )
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaTempDataRec::copy( const _Myt &other )
{
    recordId_ = other.recordId_;

    temp_ = other.age_;

    batteryLevel_ = other.batteryLevel_;

    clock_ = other.clock_;
    age_ = other.age_;

    _Mybase::copy( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaTempDataRec::reset()
{
    recordId_ = batteryLevel_ = clock_ = age_ = 0;
    temp_ = 0.0;

    _Mybase::reset();
}

