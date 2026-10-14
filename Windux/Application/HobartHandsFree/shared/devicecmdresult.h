/**
 * @file devicelist.h
 * HFPD Device Command Result Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICECMDRESULT_H
#define HFPD_DEVICECMDRESULT_H

#include "message.h"

#include <algorithm>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Command Result Message.
class deviceCommandResult : public message
{
    typedef deviceCommandResult _Myt;
    typedef message _Mybase;

public:

    type command_;                                  ///< Command message type.

    commandResult result_;                          ///< Command result.

    std::string address_;                           ///< Device address (if applicable).

    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceCommandResult() :
        _Mybase( HFPD_DEV_CMD_RESULT, 1 ),
        command_( HFPD_UNKNOWN ),
        result_( HFPD_ERROR_SUCCESS ) {}

    /// Constructor.
    deviceCommandResult( type command, commandResult result = HFPD_ERROR_SUCCESS ) :
        _Mybase( HFPD_DEV_CMD_RESULT, 1 ),
        command_( command ),
        result_( result ) {}

    /// Constructor.
    deviceCommandResult( const std::string &address, type command, commandResult result = HFPD_ERROR_SUCCESS ) :
        _Mybase( HFPD_DEV_CMD_RESULT, 1 ),
        command_( command ),
        result_( result ),
        address_( address ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceCommandResult( const _Myt& other ) :
        _Mybase( other ),
        command_( other.command_ ),
        result_( other.result_ ),
        address_( other.address_ ) {}

    /// Destructor.
    virtual ~deviceCommandResult() {}

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

        // command
        if ( !parseValue( &s[idx], n-idx, command_, MSG_COMMAND_LEN ) )
            return false;
        idx += MSG_COMMAND_LEN;

        // result
        if ( !parseValue( &s[idx], n-idx, result_, MSG_RESULT_LEN ) )
            return false;
        idx += MSG_RESULT_LEN;

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
        serializeValue( msg, command_, MSG_COMMAND_LEN );
        serializeValue( msg, result_, MSG_RESULT_LEN );
        serializeString( msg, address_ );

        return msg;
    }

private:

    enum {MSG_COMMAND_LEN = 3};
    enum {MSG_RESULT_LEN = 2};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICECMDRESULT_H

