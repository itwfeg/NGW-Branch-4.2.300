/**
 * @file kpc.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "kpc.h"

#include <iostream>
#include <iomanip>

///////////////////////////////////////////////////////////////////////////////////////////////////
kpc::kpc( canSocket *s ) :
    _Mybase( s ),
    hobartCanPrinter( hobartPrinter::first ),
    hobartCanWeigher()
{
    // setup device ranges
    addDeviceRange( printerDeviceRange() );
    addDeviceRange( weigherDeviceRange() );

    // register messages
    registerPrinterMessages( this );
    registerWeigherMessages( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
kpc::~kpc()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void kpc::recvData( unsigned int id, const unsigned char *buffer, size_t bufferLen )
{
    // this range use to include all 0x400 messages, but I narrowed it to 420-437 in the range setup
    if ( validateRange( id, weigherDeviceRange() ) )
    {
        //std::cout << "RX STEP 4: recvData(). Call Step 5 hobartcanweigher::recvWeigherMessage()" << std::endl;
        recvWeigherMessage( id, buffer, bufferLen );
    }
    else
    {
        std::cout << "RX STEP 4: recvData() processed NOTHING." << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool kpc::sendData( unsigned int id, const unsigned char *buffer, size_t bufferLen )
{
    return _Mybase::sendData( id, buffer, bufferLen );
}
