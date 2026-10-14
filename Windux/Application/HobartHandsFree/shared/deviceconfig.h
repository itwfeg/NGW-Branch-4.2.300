/**
 * @file deviceconfig.h
 * HFPD Device Configuration Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICECONFIG_H
#define HFPD_DEVICECONFIG_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Configuration Message.
class deviceConfig : public message
{
    typedef deviceConfig _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    unsigned int cachedValue_;                      ///< Configuration is cached value.
    unsigned int id_;                               ///< Configuration id.

    std::string value_;                             ///< Configuration value.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceConfig() :
        _Mybase( HFPD_DEV_CONFIG, 1 ),
        cachedValue_( 0 ),
        id_( 0 ) {}

    /// Constructor.
    /**
     * @param[in] address  device address
     * @param[in] id  configuration id
     */
    deviceConfig( const std::string& address, unsigned int id, const std::string& value, unsigned int cachedValue = 0 ) :
        _Mybase( HFPD_DEV_CONFIG, 1 ),
        address_( address ),
        cachedValue_( cachedValue ),
        id_( id ),
        value_( value ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceConfig( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ),
        cachedValue_( other.cachedValue_ ),
        id_( other.id_ ),
        value_( other.value_ ) {}

    /// Destructor.
    virtual ~deviceConfig() {}

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

        // cached value
        if ( !parseValue( &s[idx], n-idx, cachedValue_, MSG_CACHED_LEN ) )
            return false;
        idx += MSG_CACHED_LEN;

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
        serializeValue( msg, cachedValue_, MSG_CACHED_LEN );
        serializeValue( msg, id_, MSG_ID_LEN );
        serializeValue( msg, value_.size(), MSG_VALUE_SIZE_LEN );
        msg += value_;

        return msg;
    }

private:

    enum {MSG_CACHED_LEN = 1};
    enum {MSG_ID_LEN = 2};

    enum {MSG_VALUE_SIZE_LEN = 2};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICEREADCONFIG_H

