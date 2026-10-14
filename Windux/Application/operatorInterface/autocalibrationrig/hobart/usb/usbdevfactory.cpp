/**
 * @file usbdevfactory.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "atmelsamba.h"
#include "fpc.h"
#include "prepack.h"
#include "usbdevfactory.h"

#include <iostream>
#include <iomanip>

#include <libusb.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
usbDev *usbDevFactory::create( libusb_device *device )
{
    libusb_device_descriptor deviceDescriptor;
    int result;

    if ( LIBUSB_SUCCESS != (result = libusb_get_device_descriptor( device, &deviceDescriptor )) )
        std::cerr << "lib_usb_device_descriptor failed rc=" << result << std::endl;
    else
    {
        const unsigned int vid( deviceDescriptor.idVendor );
        const unsigned int pid( deviceDescriptor.idProduct );

        std::cout << "have device" <<
            " vid=0x" << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << vid <<
            " pid=0x" << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << pid << std::endl << std::dec;

        usbDev *d( NULL );

        // atmel samba bootloader
        if (( 0x03eb == vid ) && ( 0x6124 == pid ))
        {
            std::cout << "atmel samba bootloader" << std::endl;
            d = new atmelSamba( device );
        }

        // hobart fpc
        else if (( 0x128c == vid ) && ( 0x0204 == pid ))
        {
            std::cout << "hobart fpc" << std::endl;
            d = new fpc( device );
        }

        // hobart prepack
        else if (( 0x128c == vid ) && ( 0x0401 == pid ))
        {
            std::cout << "hobart prepack" << std::endl;
            d = new prepack( device );
        }

        // valid device!
        if ( d )
        {
            if ( d->isOpen() )
                return d;

            std::cerr << "device failed to open... destroying..." << std::endl;
            delete d;
        }
    }

    return NULL;
}
