/**
 * @file bluenicaconfigdatarec.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "bluenicaconfigdatarec.h"
#include "leadvertinfo.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaConfigDataRec::bluenicaConfigDataRec( const leAdvertInfo &other )
{
    // init
    reset();

    // parse data
    if ( isTypeOf( other ) )
    {
        std::string eir( other.eir( GAP_ADTYPE_MANUFACTURER_SPECIFIC ) );

        if ( 22 < eir.length() )
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

            features_ = (feature) ((unsigned char) eir.at( 13 ));

            recordId_ = (unsigned char) eir.at( 16 );
            recordId_ <<= 8;
            recordId_ += (unsigned char) eir.at( 15 );
            recordId_ <<= 8;
            recordId_ += (unsigned char) eir.at( 14 );

            advVisibilityTimeout_ = (unsigned char) eir.at( 17 );
            advVisibilityTimeout_ *= 10;

            tempCheckItv_ = (unsigned char) eir.at( 18 );
            tempCheckItv_ *= 10;

            tempPeriodicWriteItv_ = (unsigned char) eir.at( 19 );
            tempPeriodicWriteItv_ *= 60;

            tempWriteOnDelta_ = (unsigned char) eir.at( 20 );
            tempWriteOnDelta_ /= 10.0;

            versionMajor_ = (unsigned char) eir.at( 21 );
            versionMinor_ = (unsigned char) eir.at( 22 );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaConfigDataRec::bluenicaConfigDataRec( const _Myt &other )
{
    copy( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaConfigDataRec::~bluenicaConfigDataRec()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaConfigDataRec &bluenicaConfigDataRec::operator = ( const _Myt &rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaConfigDataRec::print() const
{
    std::cout << "**************************************************" << std::endl;

    std::cout << address_ << std::endl;
    std::cout << "record: " << recordId_ << " (" << timestamp_ << ")" << std::endl;

    if ( senderAddress_ != address_ )
        std::cout << "sender: " << senderAddress_ << std::endl;

    std::cout << "temp: " << temp_ << " C" << std::endl;

    std::cout << "features: " << features_ << std::endl;

    if ( ALWAYS_VISIBLE & features_ )
        std::cout << "  ALWAYS_VISIBLE" << std::endl;
    if ( ACCELEROMETER & features_ )
        std::cout << "  ACCELEROMETER" << std::endl;
    if ( ACCEL_WAKEUP & features_ )
        std::cout << "  ACCEL_WAKEUP" << std::endl;
    if ( PRIORITY_DEVICE & features_ )
        std::cout << "  PRIORITY_DEVICE" << std::endl;
    if ( ADV_LIVE_TEMP & features_ )
        std::cout << "  ADV_LIVE_TEMP" << std::endl;
    if ( WATCHDOG_DISABLED & features_ )
        std::cout << "  WATCHDOG_DISABLED" << std::endl;

    std::cout << "adv visibility timeout: " << advVisibilityTimeout_ << " sec" << std::endl;
    std::cout << "temp check itv: " << tempCheckItv_ << " sec" << std::endl;
    std::cout << "temp periodic write itv: " << tempPeriodicWriteItv_ << " sec" << std::endl;
    std::cout << "temp write on delta: " << tempWriteOnDelta_ << " C" << std::endl;

    std::cout << "version: " << versionMajor_ << "." << versionMinor_ << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bluenicaConfigDataRec::isTypeOf( const leAdvertInfo &value )
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

    // check for configuration record type
    else if ( 3 != (type & 0x7f) )
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaConfigDataRec::copy( const _Myt &other )
{
    recordId_ = other.recordId_;

    temp_ = other.temp_;

    features_ = other.features_;

    advVisibilityTimeout_ = other.advVisibilityTimeout_;

    tempCheckItv_ = other.tempCheckItv_;
    tempPeriodicWriteItv_ = other.tempPeriodicWriteItv_;
    tempWriteOnDelta_ = other.tempWriteOnDelta_;

    versionMajor_ = other.versionMajor_;
    versionMinor_ = other.versionMinor_;

    _Mybase::copy( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaConfigDataRec::reset()
{
    recordId_ = versionMajor_ = versionMinor_ = 0;
    temp_ = tempWriteOnDelta_ = 0.0;

    features_ = (feature) 0;

    advVisibilityTimeout_ = tempCheckItv_ = tempPeriodicWriteItv_ = 0;

    _Mybase::reset();
}


