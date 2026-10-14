/**
 * @file message.h
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
message::message() :
    type_( HFPD_UNKNOWN ),
    version_( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
message::message( type t, uint8_t v ) :
    type_( t ),
    version_( v )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
message::message( const _Myt& other ) :
    type_( other.type_ ),
    version_( other.version_ )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
message::~message()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool message::parse( const char *s, size_t n, size_t *len )
{
    size_t idx( 0 );

    if ( !parseValue( &s[idx], n-idx, type_, MSG_TYPE_LEN ) )
        return false;
    idx += MSG_TYPE_LEN;

    if ( !parseValue( &s[idx], n-idx, version_, MSG_VERSION_LEN ) )
        return false;
    idx += MSG_VERSION_LEN;

    (*len) = idx;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string message::serialize() const
{
    std::string msg;
    serializeValue( msg, type_, MSG_TYPE_LEN );
    serializeValue( msg, version_, MSG_VERSION_LEN );

    return msg;
}
