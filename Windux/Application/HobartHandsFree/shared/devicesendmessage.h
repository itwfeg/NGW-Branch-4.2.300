/**
 * @file devicesendmessage.h
 * HFPD Device Send Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICESENDMESSAGE_H
#define HFPD_DEVICESENDMESSAGE_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Send Message.
class deviceSendMessage : public message
{
    typedef deviceSendMessage _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    unsigned int hour_;                             ///< Message hour.
    unsigned int min_;                              ///< Message minute.
    unsigned int sec_;                              ///< Message second.
    unsigned int id_;                               ///< Sender.

    std::string message_;                           ///< Message content.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceSendMessage() :
        _Mybase( HFPD_DEV_SEND_MESSAGE, 1 ),
        hour_( 0 ),
        min_( 0 ),
        sec_( 0 ),
        id_( 0 ) {}

    /// Constructor.
    /**
     * @param[in] address  device address
     * @param[in] hour  hour
     * @param[in] min  minute
     * @param[in] sec  second
     * @param[in] id  sender
     * @param[in] message  message
     */
    deviceSendMessage( const std::string& address, unsigned int hour, unsigned int min, unsigned int sec, unsigned int id, const std::string& message ) :
        _Mybase( HFPD_DEV_SEND_MESSAGE, 1 ),
        address_( address ),
        hour_( hour ),
        min_( min ),
        sec_( sec ),
        id_( id ),
        message_( message ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceSendMessage( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ),
        hour_( other.hour_ ),
        min_( other.min_ ),
        sec_( other.sec_ ),
        id_( other.id_ ),
        message_( other.message_ ) {}

    /// Destructor.
    virtual ~deviceSendMessage() {}

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

        // hour
        if ( !parseValue( &s[idx], n-idx, hour_, MSG_HOUR_LEN ) )
            return false;
        idx += MSG_HOUR_LEN;

        // min
        if ( !parseValue( &s[idx], n-idx, min_, MSG_MIN_LEN ) )
            return false;
        idx += MSG_MIN_LEN;

        // sec
        if ( !parseValue( &s[idx], n-idx, sec_, MSG_SEC_LEN ) )
            return false;
        idx += MSG_SEC_LEN;

        // id
        if ( !parseValue( &s[idx], n-idx, id_, MSG_ID_LEN ) )
            return false;
        idx += MSG_ID_LEN;

        // message
        if ( !parseString( &s[idx], n-idx, message_ ) )
            return false;
        idx += message_.length()+1;

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
        serializeValue( msg, hour_, MSG_HOUR_LEN );
        serializeValue( msg, min_, MSG_MIN_LEN );
        serializeValue( msg, sec_, MSG_SEC_LEN );
        serializeValue( msg, id_, MSG_ID_LEN );
        serializeString( msg, message_ );

        return msg;
    }

private:

    enum {MSG_HOUR_LEN = 1};
    enum {MSG_MIN_LEN = 1};
    enum {MSG_SEC_LEN = 1};
    enum {MSG_ID_LEN = 1};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICESENDMESSAGE_H

