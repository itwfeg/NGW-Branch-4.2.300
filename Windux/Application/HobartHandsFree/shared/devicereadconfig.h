/**
 * @file devicereadconfig.h
 * HFPD Device Read Configuration Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICEREADCONFIG_H
#define HFPD_DEVICEREADCONFIG_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Read Configuration Message.
class deviceReadConfig : public message
{
    typedef deviceReadConfig _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    unsigned int useCachedValue_;                   ///< Use cached value for read.
    unsigned int id_;                               ///< Configuration id.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceReadConfig() :
        _Mybase( HFPD_DEV_READ_CONFIG, 1 ),
        useCachedValue_( 0 ),
        id_( 0 ) {}

    /// Constructor.
    /**
     * @param[in] address  device address
     * @param[in] id  configuration id
     */
    deviceReadConfig( const std::string& address, unsigned int id, unsigned int useCachedValue = 0 ) :
        _Mybase( HFPD_DEV_READ_CONFIG, 1 ),
        address_( address ),
        useCachedValue_( useCachedValue ),
        id_( id ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceReadConfig( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ),
        useCachedValue_( other.useCachedValue_ ),
        id_( other.id_ ) {}

    /// Destructor.
    virtual ~deviceReadConfig() {}

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

        // used cached
        if ( !parseValue( &s[idx], n-idx, useCachedValue_, MSG_USE_CACHED_LEN ) )
            return false;
        idx += MSG_USE_CACHED_LEN;

        // id
        if ( !parseValue( &s[idx], n-idx, id_, MSG_ID_LEN ) )
            return false;
        idx += MSG_ID_LEN;

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
        serializeValue( msg, useCachedValue_, MSG_USE_CACHED_LEN );
        serializeValue( msg, id_, MSG_ID_LEN );

        return msg;
    }

private:

    enum {MSG_USE_CACHED_LEN = 1};
    enum {MSG_ID_LEN = 2};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICEREADCONFIG_H

