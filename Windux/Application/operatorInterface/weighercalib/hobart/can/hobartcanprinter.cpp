/**
 * @file hobartcanprinter.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobartcandev.h"
#include "hobartcanprinter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartCanPrinter::hobartCanPrinter( printerType t ) :
    _Mybase( t ),
    id_( CAN_ID_FIRST )
{
    if ( second == t )
        id_ = CAN_ID_SECOND;
    else if ( third == t )
        id_ = CAN_ID_THIRD;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartCanPrinter::~hobartCanPrinter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartCanPrinter::rangeType hobartCanPrinter::printerDeviceRange() const
{
    return rangeType( id_, CAN_RANGE );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanPrinter::recvPrinterMessage( unsigned int id, const unsigned char *buffer, size_t bufferLen )
{
    Q_UNUSED( id );
    Q_UNUSED( buffer );
    Q_UNUSED( bufferLen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartCanPrinter::sendPrinterMessage( unsigned int id, const unsigned char *buffer, size_t bufferLen )
{
    return sendData( id, buffer, bufferLen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanPrinter::registerPrinterMessages( hobartCanDev *dev )
{
    Q_UNUSED( dev );
}
