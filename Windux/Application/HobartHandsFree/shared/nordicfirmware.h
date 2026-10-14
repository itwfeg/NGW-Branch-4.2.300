/**
 * @file nordicfirmware.h
 * Nordic DFU Firmware Bundle Validation.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_NORDICFIRMWARE_H
#define HFPD_NORDICFIRMWARE_H

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Nordic DFU Firmware Bundle Validation.
class nordicDeviceFirmware
{
    typedef nordicDeviceFirmware _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Message Factory Method.
    /**
     * @param[in] filename  firmware bundle filename (zip file)
     * @param[out] out  firmware out filename
     * @param[out] version  firmware version
     * @param[out] crc  firmware checksum
     * @return  @c true if valid, @c false otherwise
     */
    static bool validate( const std::string &filename, std::string &out, int &version, int &crc );

private:

    /// Parse JSON value from string.
    static std::string parseValue( const std::string &json, size_t start );

    /// Perform Nordic CRC on file.
    static int nordic_crc16( const std::string &filename );

    // not implemented
    nordicDeviceFirmware();

    // not implemented
    nordicDeviceFirmware( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_NORDICFIRMWARE_H

