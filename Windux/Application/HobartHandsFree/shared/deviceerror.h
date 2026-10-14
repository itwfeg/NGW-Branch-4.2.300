/**
 * @file deviceerror.h
 * HFPD Device Error Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICEERROR_H
#define HFPD_DEVICEERROR_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Error Message.
class deviceError : public message
{
    typedef deviceError _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    unsigned int num_;                              ///< Error number.
    std::string desc_;                              ///< Error description.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceError() :
        _Mybase( HFPD_DEV_ERROR, 1 ),
        num_( 0 ) {}

    /// Constructor.
    /**
     * @param[in] address  device address
     * @param[in] num  error number
     * @param[in] desc  error description
     */
    deviceError( const std::string& address, unsigned int num, const std::string& desc ) :
        _Mybase( HFPD_DEV_ERROR, 1 ),
        address_( address ),
        num_( num ),
        desc_( desc ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceError( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ),
        num_( other.num_ ),
        desc_( other.desc_ ) {}

    /// Destructor.
    virtual ~deviceError() {}

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

        // num
        if ( !parseValue( &s[idx], n-idx, num_, MSG_NUM_LEN ) )
            return false;
        idx += MSG_NUM_LEN;

        // desc
        if ( !parseString( &s[idx], n-idx, desc_ ) )
            return false;
        idx += desc_.length()+1;

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
        serializeValue( msg, num_, MSG_NUM_LEN );
        serializeString( msg, desc_ );

        return msg;
    }

private:

    enum {MSG_NUM_LEN = 2};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICEERROR_H

