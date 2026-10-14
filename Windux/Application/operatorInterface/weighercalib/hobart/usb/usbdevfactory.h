/**
 * @file usbdevfactory.h
 * USB Device Factory.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef USBDEVFACTORY_H
#define USBDEVFACTORY_H

class usbDev;

class libusb_device;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// USB Device Factory.
class usbDevFactory
{
    typedef usbDevFactory _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Factory object creation.
    /**
     * @param[in] device  device
     * @return  object pointer upon success, NULL otherwise
     */
    static usbDev *create( libusb_device *device );

private:

    // not implemented
    usbDevFactory();

    // not implemented
    usbDevFactory( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // USBDEVFACTORY_H
