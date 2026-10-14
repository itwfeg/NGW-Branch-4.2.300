/**
 * @file nordicfirmware.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "nordicfirmware.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>

#include <sys/stat.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
bool nordicDeviceFirmware::validate( const std::string &filename, std::string &image, int &version, int &crc )
{
#if WIN32
    // TODO
    // not sure how we can accomplish this under Win32
    return true;
#else

    struct stat st;

    // check file exists
    if ( 0 != stat( filename.c_str(), &st ) )
        return false;

    // check file too large or too small
    else if (( st.st_size < 1024 ) || ( 262144 < st.st_size ))
        return false;

    // extract manifest
    const std::string manifest( "/tmp/manifest.json" );
    const std::string unzip_manifest_cmd( "unzip -p " + filename + " manifest.json > " + manifest );

    if ( 0 != system( unzip_manifest_cmd.c_str() ) )
        return false;
    else if ( 0 != stat( manifest.c_str(), &st ) )
        return false;

    // read manifest
    std::ifstream manifest_file( manifest.c_str() );
    std::string manifest_buf( (std::istreambuf_iterator<char>( manifest_file )), std::istreambuf_iterator<char>() );

    // locate relevent information
    const size_t bin_file_pos( manifest_buf.find( "\"bin_file\"" ) );
    const size_t application_version_pos( manifest_buf.find( "\"application_version\"" ) );
    const size_t firmware_crc16_pos( manifest_buf.find( "\"firmware_crc16\"" ) );

    if (( std::string::npos == bin_file_pos ) || ( std::string::npos == application_version_pos ) || ( std::string::npos == firmware_crc16_pos ))
        return false;

    const std::string bin_file( parseValue( manifest_buf, bin_file_pos ) );
    const std::string application_version( parseValue( manifest_buf, application_version_pos ) );
    const std::string firmware_crc16( parseValue( manifest_buf, firmware_crc16_pos ) );

    std::cout << "BIN FILE: " << bin_file << std::endl;
    std::cout << "APP VERSION: " << application_version << std::endl;
    std::cout << "CRC: " << firmware_crc16 << std::endl;

    // extract firmware
    const std::string firmware( "/tmp/hfpdfirmware" );
    const std::string unzip_firmware_cmd( "unzip -p " + filename + " " + bin_file + " > " + firmware );

    if ( 0 != system( unzip_firmware_cmd.c_str() ) )
        return false;
    else if ( 0 != stat( firmware.c_str(), &st ) )
        return false;

    // perform crc16 on firmware image
    const int firmware_crc16_calc( nordic_crc16( firmware ) );

    if ( firmware_crc16_calc != std::stoi( firmware_crc16 ) )
    {
        std::cout << "calculated CRC does not match! " << firmware_crc16_calc << std::endl;
        return false;
    }

    // YAY!
    // we made it... we got firmware ready to go...
    image = firmware;
    crc = firmware_crc16_calc;

    try
    {
        version = std::stol( application_version );
    }
    catch ( ... )
    {
        version = -1;
    }

    std::cout << "valid firmware image found" << std::endl;
    return true;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string nordicDeviceFirmware::parseValue( const std::string &json, size_t pos )
{
    std::string result;

    const size_t begin( json.find( ':', pos ) );

    if ( std::string::npos != begin )
    {
        const size_t end( json.find( ',', begin ) );

        if (( std::string::npos != end ) && ( 1 < (end-begin) ))
        {
            result = json.substr( begin+1, end-begin-1 );

            // ltrim
            result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) {
                    return !std::isspace(ch);
                }));

            // rtrim
            result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) {
                    return !std::isspace(ch);
                }).base(), result.end());

            // remove double quote from string values
            if (( 2 <= result.size() ) && ( '\"' == result.front() ) && ( '\"' == result.back() ))
                result = result.substr( 1, result.size() - 2 );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int nordicDeviceFirmware::nordic_crc16( const std::string &filename )
{
    std::ifstream f( filename.c_str(), std::ios::binary | std::ios::ate );

    std::ifstream::pos_type pos( f.tellg() );
    std::vector<char> buf( pos );

    f.seekg( 0, std::ios::beg );
    f.read( &buf[0], pos );

    // ---- //

    int crc( 0xFFFF );

    for ( char b : buf )
    {
        crc = ((crc >> 8) & 0x00FF) | ((crc << 8) & 0xFF00);
        crc ^= (unsigned char) b;
        crc ^= (crc & 0x00FF) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0x00FF) << 4) << 1;
    }

    return (crc & 0xFFFF);
}
