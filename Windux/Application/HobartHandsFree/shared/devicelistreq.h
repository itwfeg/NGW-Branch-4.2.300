/**
 * @file devicelistreq.h
 * HFPD Device List Request Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICELISTREQ_H
#define HFPD_DEVICELISTREQ_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device List Request Message.
class deviceListRequest : public message
{
    typedef deviceListRequest _Myt;
    typedef message _Mybase;

public:

    /// Advert transmit power strength types.
    message::advertStrength strength_;                       ///< Advert tx strength.
    size_t age_;                                    ///< Device age (seconds).

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceListRequest( advertStrength strength = HFPD_HIGH_POWER_ADVERT, size_t age = 0 ) :
        _Mybase( HFPD_DEV_LIST_REQ, 1 ),
        strength_( strength ),
        age_( age ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceListRequest( const _Myt& other ) :
        _Mybase( other ),
        strength_( other.strength_ ),
        age_( other.age_ ) {}

    /// Destructor.
    virtual ~deviceListRequest() {}

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

        if ( !parseValue( &s[idx], n-idx, strength_, MSG_TX_POWER_LEN ) )
            return false;
        idx += MSG_TX_POWER_LEN;

        if ( !parseValue( &s[idx], n-idx, age_, MSG_AGE_LEN ) )
            return false;
        idx += MSG_AGE_LEN;

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
        serializeValue( msg, strength_, MSG_TX_POWER_LEN );
        serializeValue( msg, age_, MSG_AGE_LEN );

        return msg;
    }

private:

    enum {MSG_TX_POWER_LEN = 1};
    enum {MSG_AGE_LEN = 4};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICELISTREQ_H

