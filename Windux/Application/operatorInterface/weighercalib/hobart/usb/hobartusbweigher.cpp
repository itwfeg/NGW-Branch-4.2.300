/**
 * @file hobartusbweigher.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobartusbweigher.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartUsbWeigher::hobartUsbWeigher() :
    _Mybase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartUsbWeigher::~hobartUsbWeigher()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartUsbWeigher::setWeigherMode( unsigned int mode )
{
    static const size_t msgLen( 4 );

    unsigned char msg[16];
    msg[0] = wgMode;
    msg[1] = 0;
    msg[2] = mode;
    msg[3] = 0;

    if ( msgLen != sendWeigherMessage( msg, msgLen, XFER_TIMEOUT ) )
        std::cout << "failed to set weigher mode!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartUsbWeigher::weigherEnable()
{
    if ( !sendWeigherMessageBasic( wgEnable ) )
        std::cout << "failed to enable weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartUsbWeigher::weigherWakeup()
{
    if ( !sendWeigherMessageBasic( wgReqWakeup ) )
        std::cout << "failed to wakeup weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartUsbWeigher::weigherZero()
{
    if ( !sendWeigherMessageBasic( wgRezero ) )
        std::cout << "failed to zero weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartUsbWeigher::recvWeigherMessage( const unsigned char *buffer, size_t bufferLen )
{
    if ( bufferLen < 2 )
        return;

    const weigherMessage msgType( (weigherMessage) buffer[0] );

    std::cout << "recv weigher message " << msgType << std::endl;

    // handle message
    switch ( msgType )
    {
    case wgStatus:
        if ( 12 <= bufferLen )
        {
            const uint16_t flags( be16toh( *reinterpret_cast<const uint16_t*>( &buffer[2] ) ) );
            const uint32_t counts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[4] ) ) );
            const uint32_t rawCounts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[8] ) ) );

            // handle message
            handleWeigherStatus( flags, counts, rawCounts );
        }

        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t hobartUsbWeigher::sendWeigherMessage( const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    return sendData( weigherInterruptOut(), logicalAddrWeigher(), buffer, bufferLen, timeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartUsbWeigher::sendWeigherMessageBasic( weigherMessage msgType )
{
    static const size_t msgLen( 2 );

    unsigned char msg[16];
    msg[0] = msgType;
    msg[1] = 0;

    return (msgLen == sendWeigherMessage( msg, msgLen, XFER_TIMEOUT ));
}
