/**
 * @file devicereadallconfig.h
 * HFPD Device Read All Configuration Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICEREADALLCONFIG_H
#define HFPD_DEVICEREADALLCONFIG_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Read All Configuration Message.
class deviceReadAllConfig : public message
{
    typedef deviceReadAllConfig _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceReadAllConfig() :
        _Mybase( HFPD_DEV_READ_ALL_CONFIG, 1 ) {}

    /// Constructor.
    /**
     * @param[in] address
     */
    deviceReadAllConfig( const std::string& address ) :
        _Mybase( HFPD_DEV_READ_ALL_CONFIG, 1 ),
        address_( address ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceReadAllConfig( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ) {}

    /// Destructor.
    virtual ~deviceReadAllConfig() {}

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

        return msg;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICEREADALLCONFIG_H

