/**
 * @file leadvertinfo.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "leadvertinfo.h"

#include <bluetooth/hci_lib.h>

#include <sys/time.h>

#include <cstdio>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
leAdvertInfo::leAdvertInfo( const std::string& buffer )
{
    parse( buffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
leAdvertInfo::leAdvertInfo( const _Myt& other )
{
    copy( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
leAdvertInfo::~leAdvertInfo()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
leAdvertInfo& leAdvertInfo::operator = ( const _Myt& rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string leAdvertInfo::address() const
{
    char buffer[32];
    ba2str( &bdaddr_, buffer );

    return std::string( buffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string leAdvertInfo::eir( uint8_t type ) const
{
    for ( std::list<std::string>::const_iterator eir( eirs_.begin() ); eir != eirs_.end(); ++eir )
        if (( 2 <= eir->length() ) && ( type == eir->at( 0 ) ))
            return eir->substr( 1 );

    return std::string();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool leAdvertInfo::compare( const _Myt& rhs, bool checkStamp ) const
{
    if ( raw_ != rhs.raw_ )
        return false;

    if (( checkStamp ) && ( 0 != memcmp( &stamp_, &rhs.stamp_, sizeof(stamp_) ) ))
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void leAdvertInfo::print() const
{
    if ( !isValid() )
        return;

    char buffer[1024];

    std::cout << "**************************************************" << std::endl;

    // timestamp
    std::cout << "time: " << stamp_.tv_sec << std::endl;

    // report count
    std::cout << "report-count: " << (int) reports_count_ << std::endl;

    // BT address & type
    ba2str( &bdaddr_, buffer );

    std::cout << "BT Address: " << buffer << ", Type: " << ((LE_PUBLIC_ADDRESS == bdaddr_type_) ? "public" : "random") << std::endl;

    // range (RSSI)
    sprintf( buffer, "(0x%2.2x)", (uint8_t) rssi_ );

    if (( (uint8_t) rssi_ == 0x99 ) || ( rssi_ == 127 ))
        std::cout << "RSSI: invalid " << buffer << std::endl;
    else
        std::cout << "RSSI: " << (int) rssi_ << " dBm " << buffer << std::endl;

    // eirs
    for ( std::list<std::string>::const_iterator i = eirs_.begin(); i != eirs_.end(); ++i )
    {
        if ( 0 < i->length() )
        {
            sprintf( buffer, "[0x%2.2x] ", i->at( 0 ) );
            std::cout << buffer;

            for ( size_t j( 1 ); j < i->length(); ++j )
            {
                sprintf( buffer, "%2.2x", i->at( j ) );
                std::cout << buffer;
            }

            std::cout << std::endl;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void leAdvertInfo::read( std::istream& is )
{
    size_t len;
    char buffer[256];

    // reset object
    reset();

    // read object length
    is.read( (char *) &len, sizeof(len) );

    // read timestamp
    if (( sizeof(stamp_) <= len ) && ( len <= sizeof(buffer) ))
    {
        is.read( (char *) &stamp_, sizeof(stamp_) );
        len -= sizeof(stamp_);
    }

    // read raw advert data
    if (( 0 < len ) && ( len <= sizeof(buffer) ))
    {
        is.read( buffer, len );
        parse( std::string( buffer, len ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void leAdvertInfo::write( std::ostream& os ) const
{
    size_t len( sizeof(stamp_) + raw_.length() );

    os.write( (char *) &len, sizeof(len) );
    os.write( (char *) &stamp_, sizeof(stamp_) );
    os.write( raw_.c_str(), raw_.length() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool leAdvertInfo::parse( const std::string& buffer )
{
    bool valid( true );

    // reset this object
    reset();

    // get timestamp
    gettimeofday( &stamp_, NULL );

    // retrieve structure
    le_advertising_info *leinfo = (le_advertising_info *) (buffer.c_str() + 1);

    bdaddr_type_ = leinfo->bdaddr_type;
    memcpy( &bdaddr_, &leinfo->bdaddr, sizeof(bdaddr_) );

    reports_count_ = *buffer.c_str();
    rssi_ = leinfo->data[leinfo->length];

    // parse eirs
    for ( int i( 0 ); i < leinfo->length; )
    {
        size_t eir_len( leinfo->data[i] );

        if ( leinfo->length < (eir_len + 1) )
        {
            std::cerr << "EIR data length is longer than EIR packet length." << std::endl;
            valid = false;
            break;
        }

        // append eir
        eirs_.push_back( std::string( (const char *) &leinfo->data[i+1], eir_len ) );

        i += eir_len + 1;
    }

    if ( valid )
        raw_ = buffer;

    return valid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void leAdvertInfo::copy( const _Myt& other )
{
    raw_ = other.raw_;

    memcpy( &stamp_, &other.stamp_, sizeof(stamp_) );

    bdaddr_type_ =  other.bdaddr_type_;
    memcpy( &bdaddr_, &other.bdaddr_, sizeof(bdaddr_) );

    reports_count_ =  other.reports_count_;
    rssi_ =  other.rssi_;

    eirs_ = other.eirs_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void leAdvertInfo::reset()
{
    raw_.clear();

    memset( &stamp_, 0, sizeof(stamp_) );

    bdaddr_type_ = 0;
    memset( &bdaddr_, 0, sizeof(bdaddr_) );

    reports_count_ = 0;
    rssi_ = 0;

    eirs_.clear();
}

