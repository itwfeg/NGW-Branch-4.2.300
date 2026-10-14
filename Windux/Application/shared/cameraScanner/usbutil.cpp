
#include <QtGlobal>

// linux only
#ifdef Q_OS_LINUX

#include "usbutil.h"

#include <libusb.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
bool usbUtil::isAttachedToSystem( unsigned short vid, unsigned short pid )
{
    bool found( false );

    // init libusb
    if ( 0 <= libusb_init( NULL ) )
    {
        // pull device list
        struct libusb_device **devs;
        ssize_t count( libusb_get_device_list( NULL, &devs ) );

        // enumerate
        if ( 0 <= count )
        {
            for ( int i( 0 ); i < count; ++i )
            {
                struct libusb_device_descriptor info;

                if ( 0 == libusb_get_device_descriptor( devs[i], &info ) )
                    if (( vid == info.idVendor ) && ( pid == info.idProduct ))
                    {
                        found = true;
                        break;
                    }
            }

            libusb_free_device_list( devs, 1 );
        }

        libusb_exit( NULL );
    }

    return found;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // Q_OS_LINUX
