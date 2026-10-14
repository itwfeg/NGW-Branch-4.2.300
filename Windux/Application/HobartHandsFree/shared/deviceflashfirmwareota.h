/**
 * @file deviceflashfirmwareota.h
 * HFPD Device Flash Firmware using OTA.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICEFLASHFIRMWAREOTA_H
#define HFPD_DEVICEFLASHFIRMWAREOTA_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Flah Firmware using OTA.
class deviceFlashFirmwareOTA : public message
{
    typedef deviceFlashFirmwareOTA _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    unsigned int id_;                               ///< OTA Identifier.
    std::string filename_;                          ///< Firmware File.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceFlashFirmwareOTA() :
        _Mybase( HFPD_DEV_FLASH_FIRMWARE_OTA, 1 ) {}

    /// Constructor.
    /**
     * @param[in] address  device address
     * @param[in] id  ota identifier
     * @param[in] filename  firmware file
     */
    deviceFlashFirmwareOTA( const std::string& address, unsigned int id, const std::string& filename ) :
        _Mybase( HFPD_DEV_FLASH_FIRMWARE_OTA, 1 ),
        address_( address ),
        id_( id ),
        filename_( filename ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceFlashFirmwareOTA( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ),
        id_( other.id_ ),
        filename_( other.filename_ ) {}

    /// Destructor.
    virtual ~deviceFlashFirmwareOTA() {}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Parse message.
    /**
     * @param[in] s  pointer to array of characters
     * @param[in] n  number of characters in array
     * @param[out] len  characters used
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool parse( const char *s, size_t n, size_t *len )
    {
        size_t idx( 0 );

        if ( !_Mybase::parse( s, n, &idx ) )
            return false;

        // address
        if ( !parseString( &s[idx], n-idx, address_ ) )
            return false;
        idx += address_.length()+1;

        // id
        if ( !parseValue( &s[idx], n-idx, id_, MSG_ID_LEN ) )
            return false;
        idx += MSG_ID_LEN;

        // filename
        if ( !parseString( &s[idx], n-idx, filename_ ) )
            return false;
        idx += filename_.length()+1;

        (*len) = idx;
        return true;
    }

    /// Serialize message.
    /**
     * @return  serialized message
     */
    virtual std::string serialize() const
    {
        std::string msg( _Mybase::serialize() );
        serializeString( msg, address_ );
        serializeValue( msg, id_, MSG_ID_LEN );
        serializeString( msg, filename_ );

        return msg;
    }

private:

    enum {MSG_ID_LEN = 4};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICEFLASHFIRMWAREOTA_H

