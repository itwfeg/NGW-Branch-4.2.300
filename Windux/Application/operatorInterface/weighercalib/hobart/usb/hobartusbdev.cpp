/**
 * @file hobartusbdev.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobartusbdev.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartUsbDev::hobartUsbDev( libusb_device *device ) :
    _Mybase( device )
{
    // connect to endpoints
    QList<usbDevEndpoint*> endpoints( endpoints_.values() );

    foreach ( usbDevEndpoint *e, endpoints )
        if ( LIBUSB_ENDPOINT_IN == e->direction() )
            connect( e, SIGNAL(readyRead()), SLOT(onReadyRead()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartUsbDev::~hobartUsbDev()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t hobartUsbDev::sendData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    uint16_t v16;

    // generate header for this message
    unsigned char header[16];
    memcpy( &header[0], &(v16 = htobe16( PHYSICAL_ADDR_HOST )), sizeof( v16 ) );
    memcpy( &header[2], &(v16 = htobe16( LOGICAL_ADDR_HOST )), sizeof( v16 ) );
    memcpy( &header[4], &(v16 = htobe16( physicalAddr() )), sizeof( v16 ) );
    memcpy( &header[6], &(v16 = htobe16( logicalAddr )), sizeof( v16 ) );
    memcpy( &header[8], &(v16 = htobe16( bufferLen )), sizeof( v16 ) );

    QMutexLocker guard( &m_ );

    // send header
    if ( 10 != e->transfer( header, 10, timeout ) )
    {
        std::cout << "failed to send header!" << std::endl;
        return 0;
    }

    // send body
    return e->transfer( const_cast<unsigned char*>( buffer ), bufferLen, timeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartUsbDev::onReadyRead()
{
    usbDevEndpoint *e( dynamic_cast<usbDevEndpoint*>( sender() ) );

    if ( !e )
        return;

    const size_t bytes( e->bytesAvailable() );

    // check for header
    if ( bytes < 10 )
        return;

    const QByteArray header( e->peek( 10 ) );

    size_t bodyLen( be16toh( *reinterpret_cast<const uint16_t*>( &header.data()[8] ) ) );

    // check for body
    if ( bytes < 10+bodyLen )
        return;

    QByteArray body( e->read( 10+bodyLen ) );
    body.remove( 0, 10 );

    // we have data
    recvData( e, be16toh( *reinterpret_cast<const uint16_t*>( &header.data()[2] ) ), (unsigned char*) body.data(), body.size() );
}
