/**
 * @file devicestate.h
 * HFPD Device State Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICESTATE_H
#define HFPD_DEVICESTATE_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device State Message.
class deviceState : public message
{
    typedef deviceState _Myt;
    typedef message _Mybase;

public:

    enum type
    {
        STATE_DISCONNECTED                  = 0,    ///< Not connected.
        STATE_CONNECTING                    = 1,    ///< Connecting.
        STATE_CONNECTED                     = 2,    ///< Connected.
        STATE_READY                         = 3,    ///< Ready.
    };

    std::string address_;                           ///< Device address.
    type state_;                                    ///< Device state.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceState() :
        _Mybase( HFPD_DEV_STATE, 1 ),
        state_( STATE_DISCONNECTED ) {}

    /// Constructor.
    /**
     * @param[in] address
     * @param[in] state
     */
    deviceState( const std::string& address, type state ) :
        _Mybase( HFPD_DEV_STATE, 1 ),
        address_( address ),
        state_( state ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceState( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ),
        state_( other.state_ ) {}

    /// Destructor.
    virtual ~deviceState() {}

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

        // state
        if ( !parseValue( &s[idx], n-idx, state_, MSG_STATE_LEN ) )
            return false;
        idx += MSG_STATE_LEN;

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
        serializeValue( msg, state_, MSG_STATE_LEN );

        return msg;
    }

private:

    enum {MSG_STATE_LEN = 1};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICESTATE_H

