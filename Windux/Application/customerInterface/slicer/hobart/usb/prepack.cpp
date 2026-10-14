/**
 * @file prepack.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "prepack.h"

#include <iostream>
#include <iomanip>

#include <libusb.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

void prepack::slt_WriteConfig(WeigherConfiguration &config)
{
    unsigned char * buffer = config.toWgConfigMessage();
    this->sendWeigherMessage(buffer,config.getBufferLen());
}

void prepack::slt_ReqConfig()
{
    this->reqWeigherConfig();
}


prepack::prepack( libusb_device *device ) :
    _Mybase( device ),
    hobartWeigher(),
    interruptIn_( 0 ),
    interruptOut_( 0 )
{
    // save off endpoints
    QList<usbDevEndpoint*> endpoints( endpoints_.values() );

    foreach ( usbDevEndpoint *e, endpoints )
    {
        if ( LIBUSB_TRANSFER_TYPE_INTERRUPT == e->type() )
        {
            if ( LIBUSB_ENDPOINT_IN == e->direction() )
                interruptIn_ = e;
            else if ( LIBUSB_ENDPOINT_OUT == e->direction() )
                interruptOut_ = e;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
prepack::~prepack()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void prepack::recvData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen )
{
    //std::cout << "device " << logicalAddr << " recv " << bufferLen << " bytes" << std::endl;

    // send to proper device
    if ( logicalAddrWeigher() == logicalAddr )
        recvWeigherMessage( buffer, bufferLen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t prepack::sendData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    return _Mybase::sendData( e, logicalAddr, buffer, bufferLen, timeout );
}

