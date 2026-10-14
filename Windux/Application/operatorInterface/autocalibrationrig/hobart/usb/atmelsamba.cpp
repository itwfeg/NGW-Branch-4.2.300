/**
 * @file atmelsamba.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "atmelsamba.h"

#include <cstdio>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
atmelSamba::atmelSamba( libusb_device *device ) :
    _Mybase( device, INTERFACE_BULK ),
    bulkIn_( NULL ),
    bulkOut_( NULL )
{
    // save off endpoints
    QList<usbDevEndpoint*> endpoints( endpoints_.values() );

    foreach ( usbDevEndpoint *e, endpoints )
        if ( LIBUSB_TRANSFER_TYPE_BULK == e->type() )
        {
            if ( LIBUSB_ENDPOINT_IN == e->direction() )
                bulkIn_ = e;
            else if ( LIBUSB_ENDPOINT_OUT == e->direction() )
                bulkOut_ = e;
        }

    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
atmelSamba::~atmelSamba()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void atmelSamba::boot()
{
    std::cout << "booting atmel device..." << std::endl;

    unsigned char buffer[32];
    size_t bufferLen( snprintf( (char*) buffer, sizeof(buffer), "G%08x#\n", 0x00108200 ) );

    if ( !bulkOut_ )
        std::cout << "invalid bulk out" << std::endl;
    else if ( bufferLen != bulkOut_->transfer( buffer, bufferLen, XFER_TIMEOUT ) )
        std::cout << "error sending boot command" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void atmelSamba::initialize()
{
    unsigned char buffer[32];
    size_t bufferLen( snprintf( (char*) buffer, sizeof(buffer), "N#\n" ) );

    // switch to non-interactive mode
    if ( !bulkOut_ )
        std::cout << "invalid bulk out" << std::endl;
    else if ( bufferLen != bulkOut_->transfer( buffer, bufferLen, XFER_TIMEOUT ) )
        std::cout << "error sending init command" << std::endl;

    // wait for response
    else if ( !bulkIn_ )
        std::cout << "invalid bulk in" << std::endl;
    else if ( !bulkIn_->waitForReadyRead( XFER_TIMEOUT ) )
        std::cout << "error receiving init command response" << std::endl;

    // read response
    else
    {
        const QByteArray a( bulkIn_->readAll() );

        // validate response
        if (( 2 != a.size() ) || ( 0 != memcmp( "\n\r", a.data(), 2 ) ))
            std::cout << "invalid init command response" << std::endl;
    }
}
