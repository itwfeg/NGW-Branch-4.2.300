/**
 * @file hobartusbprinter.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobartusbprinter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartUsbPrinter::hobartUsbPrinter( printerType t ) :
    _Mybase( t )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartUsbPrinter::~hobartUsbPrinter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartUsbPrinter::recvPrinterMessage( const unsigned char *buffer, size_t bufferLen )
{
    Q_UNUSED( buffer );
    Q_UNUSED( bufferLen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t hobartUsbPrinter::sendPrinterMessage( const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    return sendData( printerInterruptOut(), logicalAddrPrinter(), buffer, bufferLen, timeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t hobartUsbPrinter::sendPrinterMessageBulk( const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    return sendData( printerBulkOut(), logicalAddrPrinter(), buffer, bufferLen, timeout );
}
