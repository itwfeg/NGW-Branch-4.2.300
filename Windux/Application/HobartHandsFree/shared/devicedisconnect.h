/**
 * @file devicedisconnect.h
 * HFPD Device Disconnect Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICEDISCONNECT_H
#define HFPD_DEVICEDISCONNECT_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Disconnect Message.
class deviceDisconnect : public message
{
    typedef deviceDisconnect _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceDisconnect() :
        _Mybase( HFPD_DEV_DISCONNECT, 1 ) {}

    /// Constructor.
    /**
     * @param[in] address
     */
    deviceDisconnect( const std::string& address ) :
        _Mybase( HFPD_DEV_DISCONNECT, 1 ),
        address_( address ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceDisconnect( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ) {}

    /// Destructor.
    virtual ~deviceDisconnect() {}

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

#endif // HFPD_DEVICEDISCONNECT_H

