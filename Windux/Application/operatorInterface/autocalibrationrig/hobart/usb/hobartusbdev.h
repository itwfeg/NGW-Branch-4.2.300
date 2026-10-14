/**
 * @file hobartusbdev.h
 * Hobart Generic USB Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTUSBDEV_H
#define HOBARTUSBDEV_H

#include "usbdev.h"

#include <QMutex>

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart Generic USB Device.
class hobartUsbDev : public usbDev
{
    Q_OBJECT

    typedef hobartUsbDev _Myt;
    typedef usbDev _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartUsbDev();

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] device  device pointer
     */
    hobartUsbDev( libusb_device *device );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device physical address.
    /**
     * @return  physical address
     */
    virtual unsigned short physicalAddr() const = 0;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Receive data from device.
    /**
     * @param[in] e  endpoint
     * @param[in] logicalAddr  device logical address
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     */
    virtual void recvData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen ) = 0;

    /// Send data to device.
    /**
     * @param[in] e  endpoint
     * @param[in] logicalAddr  device logical address
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @param[in] timeout  timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0
     * @return  number of bytes transferred
     */
    virtual size_t sendData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen, unsigned int timeout );

private Q_SLOTS:

    /// Slot for endpoint ready read.
    void onReadyRead();

private:

    enum {PHYSICAL_ADDR_HOST = 0x0000};
    enum {LOGICAL_ADDR_HOST = 0x0009};

    QMutex m_;

    // not implemented
    hobartUsbDev();

    // not implemented
    hobartUsbDev( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTUSBDEV_H
