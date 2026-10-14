/**
 * @file gcdmsg.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "gcdmsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdMsg::valid( const QByteArray& data, gcdMsgType::type& type )
{
    uint32_t len = data.length();

    if ( MSG_HEADER_LENGTH <= len )
    {
        type = parseType( data );

        if ( parseLength( data ) <= len )
            return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int gcdMsg::parseLength( const QByteArray& data )
{
    // length in message is only payload length!
    // so we need to add the header length in our calculation...
    return (readArray<uint32_t>( data, MSG_LENGTH_OFFSET ) + MSG_HEADER_LENGTH);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gcdMsgType::type gcdMsg::parseType( const QByteArray& data )
{
    return (gcdMsgType::type) readArray<uint32_t>( data, MSG_TYPE_OFFSET );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdMsg::copy( const _Myt& rhs )
{
    // empty list
    // do _not_ call clear() here, results in segment fault! more mingw32 workarounds...
    while ( !empty() )
        pop_front();

    // copy list
    // do _not_ use a "foreach" loop here, results in segment fault! more mingw32 workarounds...
    for ( const_iterator i = rhs.begin(); i != rhs.end(); ++i )
        push_back( *i );

}
