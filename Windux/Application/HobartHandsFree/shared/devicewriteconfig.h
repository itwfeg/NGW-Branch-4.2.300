/**
 * @file devicewriteconfig.h
 * HFPD Device Write Configuration Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICEWRITECONFIG_H
#define HFPD_DEVICEWRITECONFIG_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Configuration Message.
class deviceWriteConfig : public message
{
    typedef deviceWriteConfig _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    unsigned int id_;                               ///< Configuration id.
    std::string value_;                             ///< Configuration value.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceWriteConfig() :
        _Mybase( HFPD_DEV_WRITE_CONFIG, 1 ),
        id_( 0 ) {}

    /// Constructor.
    /**
     * @param[in] address  device address
     * @param[in] id  configuration id
     */
    deviceWriteConfig( const std::string& address, unsigned int id, const std::string& value ) :
        _Mybase( HFPD_DEV_WRITE_CONFIG, 1 ),
        address_( address ),
        id_( id ),
        value_( value ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceWriteConfig( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ),
        id_( other.id_ ),
        value_( other.value_ ) {}

    /// Destructor.
    virtual ~deviceWriteConfig() {}

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

        // value length
        size_t valueLen;
        if ( !parseValue( &s[idx], n-idx, valueLen, MSG_VALUE_SIZE_LEN ) )
            return false;
        idx += MSG_VALUE_SIZE_LEN;

        if ( !valueLen )
            value_.clear();
        else if ( valueLen < (n-idx) )
            return false;
        else
        {
            value_ = std::string( &s[idx], valueLen );
            idx += valueLen;
        }

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
        serializeValue( msg, value_.size(), MSG_VALUE_SIZE_LEN );
        msg += value_;

        return msg;
    }

private:

    enum {MSG_ID_LEN = 2};

    enum {MSG_VALUE_SIZE_LEN = 2};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICEWRITECONFIG_H

