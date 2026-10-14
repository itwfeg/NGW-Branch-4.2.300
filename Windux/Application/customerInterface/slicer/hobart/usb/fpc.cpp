/**
 * @file fpc.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "fpc.h"

#include <iostream>
#include <iomanip>

#include <libusb.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
void fpc::slt_WriteConfig(WeigherConfiguration &config)
{
    //TODO::STUBB
}

void fpc::slt_ReqConfig()
{
    this->reqWeigherConfig();
}

fpc::fpc( libusb_device *device ) :
    _Mybase( device ),
    hobartPrinter(),
    hobartWeigher(),
    interruptIn_( 0 ),
    interruptOut_( 0 ),
    bulkOut_( 0 )
{
    // save off endpoints
    QList<usbDevEndpoint*> endpoints( endpoints_.values() );

    foreach ( usbDevEndpoint *e, endpoints )
    {
        if ( LIBUSB_TRANSFER_TYPE_BULK == e->type() )
        {
            if ( LIBUSB_ENDPOINT_OUT == e->direction() )
                bulkOut_ = e;
        }
        else if ( LIBUSB_TRANSFER_TYPE_INTERRUPT == e->type() )
        {
            if ( LIBUSB_ENDPOINT_IN == e->direction() )
                interruptIn_ = e;
            else if ( LIBUSB_ENDPOINT_OUT == e->direction() )
                interruptOut_ = e;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fpc::~fpc()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void fpc::recvData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen )
{
    //std::cout << "device " << logicalAddr << " recv " << bufferLen << " bytes" << std::endl;

    // send to proper device
    if ( logicalAddrPrinter() == logicalAddr )
        recvPrinterMessage( buffer, bufferLen );
    else if ( logicalAddrWeigher() == logicalAddr )
        recvWeigherMessage( buffer, bufferLen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t fpc::sendData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    return _Mybase::sendData( e, logicalAddr, buffer, bufferLen, timeout );
}
