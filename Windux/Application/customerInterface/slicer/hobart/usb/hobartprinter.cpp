/**
 * @file hobartprinter.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobartprinter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartPrinter::hobartPrinter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartPrinter::~hobartPrinter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartPrinter::recvPrinterMessage( const unsigned char *buffer, size_t bufferLen )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t hobartPrinter::sendPrinterMessage( const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    return sendData( printerInterruptOut(), logicalAddrPrinter(), buffer, bufferLen, timeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t hobartPrinter::sendPrinterMessageBulk( const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    return sendData( printerBulkOut(), logicalAddrPrinter(), buffer, bufferLen, timeout );
}
