/**
 * @file message.h
 * HFPD Message Base Class Object.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_MESSAGE_H
#define HFPD_MESSAGE_H

#include <ostream>
#include <string>

#include <cstring>
#include <limits>

#include <stdint.h>

#if WIN32
#include <WinSock.h>
#else
#include <arpa/inet.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Message.
class message
{
    typedef message _Myt;

public:

    /// Message types.
    enum type
    {
        HFPD_UNKNOWN                    = 0,        ///< Unknown Message.

        HFPD_PING                       = 100,      ///< Ping.
        HFPD_PONG                       = 101,      ///< Ping Response.

        HFPD_DEV_LIST_REQ               = 200,      ///< Device List Request.
        HFPD_DEV_LIST                   = 201,      ///< Device Listing.
		HFPD_DEV_OPNAME_LIST            = 202,      ///< Device Operator Name Listing.

        HFPD_DEV_FLASH_FIRMWARE_OTA     = 220,      ///< Flash Application Firmware via OTA.
        HFPD_DEV_FIRMWARE_REQUEST       = 221,      ///< Device Requesting Firmware Download.
        HFPD_DEV_FIRMWARE_PROGRESS      = 222,      ///< Device Firmware Download Progress.

        HFPD_DEV_CONNECT                = 300,      ///< Connect to Device.
        HFPD_DEV_DISCONNECT             = 301,      ///< Disconnect from Device.
        HFPD_DEV_READ_ALL_CONFIG        = 302,      ///< Read All Device Configuration.
        HFPD_DEV_READ_CONFIG            = 303,      ///< Read Device Configuration.
        HFPD_DEV_WRITE_CONFIG           = 304,      ///< Write Device Configuration.
        HFPD_DEV_CONFIG                 = 305,      ///< Device Configuration.
        HFPD_DEV_SEND_MESSAGE           = 306,      ///< Send Message.
        HFPD_DEV_CLEAR_ALL_MESSAGES     = 307,      ///< Clear All Messages.

        HFPD_DEV_CMD_RESULT             = 350,      ///< Device Command Result.
        HFPD_DEV_STATE                  = 351,      ///< Device State.
        HFPD_DEV_ERROR                  = 352,      ///< Device Error Message.
    };

    /// Command result codes.
    enum commandResult
    {
        HFPD_ERROR_SUCCESS              = 0,        ///< No Error.
        HFPD_ERROR_BAD_REQUEST          = 1,        ///< Bad Request.
        HFPD_ERROR_UNKNOWN_DEVICE       = 2,        ///< Unknown Device.
        HFPD_ERROR_NOT_CONNECTED        = 3,        ///< Not connected to device.
        HFPD_ERROR_UNSUPPORTED          = 4,        ///< Unsupported command.
        HFPD_ERROR_CONFIG_UNKNOWN       = 5,        ///< Unknown Config Value.
        HFPD_ERROR_CONFIG_REQUESTED     = 6,        ///< Configuration was Requested.
        HFPD_ERROR_INVALID_FIRMWARE     = 7,        ///< Firmware is invalid.
    };

    enum advertStrength
    {
        HFPD_HIGH_POWER_ADVERT          = 0,        ///< High tx power advertisment.
        HFPD_LOW_POWER_ADVERT           = 1,        ///< Low tx power advertisment.
    };

    type type_;                                     ///< Message type.
    uint8_t version_;                               ///< Message version.

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~message();

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
    virtual bool parse( const char *s, size_t n, size_t *len );

    /// Serialize message.
    /**
     * @return  serialized message
     */
    virtual std::string serialize() const;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    message();

    /// Constructor.
    /**
     * @param[in] t  message type
     * @param[in] v  message version
     */
    message( type t, uint8_t v );

    /// Constructor.
    /**
     * @param[in] other  message
     */
    message( const _Myt& other );

    // ========================================================================
    // Static Methods
    // ========================================================================

    template <typename T>
    static void serializeValue( std::string& s, const T& val, size_t length = sizeof(T) ) {
        if ( length <= 4 ) {
            const uint32_t temp( htonl( (uint32_t) val ) );
            s.append( &reinterpret_cast<const char*>( &temp )[4-length], length );
        }
    }

    static void serializeString( std::string& s, const std::string& val ) {
        s.append( val );
        s.append( "\0", 1 );
    }

    template <typename T>
    static bool parseValue( const char* s, size_t n, T& val, size_t length = sizeof(T) ) {
        if (( n < length ) || ( sizeof(T) < length ))
            return false;

        const size_t shift_bits( 8 * (sizeof(T) - length) );

        val = (T) 0;
        do
        {
        	val = (T)( val << 8 );
        	val = (T)( val | (*s++ & 0xFF) );
        } while ( --length );

        val = (T)( val << shift_bits );
       	val = (T)( val >> shift_bits );

        return true;
    }

    static bool parseString( const char* s, size_t n, std::string& val ) {
        bool result( false );
        val.clear();
        for ( size_t i( 0 ); i < n; ++i ) {
            if ( !s[i] ) {
                result = true;
                break;
            }
            val.push_back( s[i] );
        }
        return result;
    }

private:

    enum {MSG_TYPE_LEN = 3};
    enum {MSG_VERSION_LEN = 1};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Stream insertion operator.
/**
 * @param[in,out] rhs  output stream
 * @param[in] lhs  object to insert
 * @return  reference to @p lhs
 */
inline std::ostream& operator << ( std::ostream& lhs, const message& rhs )
{
    lhs << rhs.serialize();
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_MESSAGE_H

