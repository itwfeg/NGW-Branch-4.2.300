/**
 * @file hobartusbprinter.h
 * Hobart USB Printer Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTUSBPRINTER_H
#define HOBARTUSBPRINTER_H

#include "../hobartprinter.h"

#include <cstddef>

class hobartUsbDev;

class usbDevEndpoint;

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart USB Printer Device.
class hobartUsbPrinter : public hobartPrinter
{
    typedef hobartUsbPrinter _Myt;
    typedef hobartPrinter _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartUsbPrinter();

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] t  printer type
     */
    hobartUsbPrinter( printerType t );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device logical address.
    /**
     * @return  logical address
     */
    virtual unsigned short logicalAddrPrinter() const {return LOGICAL_ADDR_PRINTER;}

    /// Retrieve printer bulk out.
    /**
     * @return  printer bulk out
     */
    virtual usbDevEndpoint *printerBulkOut() const = 0;

    /// Retrieve printer interrupt in.
    /**
     * @return  printer interrupt in
     */
    virtual usbDevEndpoint *printerInterruptIn() const = 0;

    /// Retrieve printer interrupt out.
    /**
     * @return  printer interrupt out
     */
    virtual usbDevEndpoint *printerInterruptOut() const = 0;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send data to device.
    /**
     * @param[in] e  endpoint
     * @param[in] logicalAddr  device logical address
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @param[in] timeout  timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0
     * @return  number of bytes transferred
     */
    virtual size_t sendData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen, unsigned int timeout ) = 0;

    /// Receive message from device.
    /**
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     */
    virtual void recvPrinterMessage( const unsigned char *buffer, size_t bufferLen );

    /// Send message to device.
    /**
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @param[in] timeout  timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0
     * @return  number of bytes transferred
     */
    virtual size_t sendPrinterMessage( const unsigned char *buffer, size_t bufferLen, unsigned int timeout = XFER_TIMEOUT );

    /// Send bulk message to device.
    /**
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @param[in] timeout  timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0
     * @return  number of bytes transferred
     */
    virtual size_t sendPrinterMessageBulk( const unsigned char *buffer, size_t bufferLen, unsigned int timeout = XFER_TIMEOUT );

private:

    enum {LOGICAL_ADDR_PRINTER = 0x0024};

    enum {XFER_TIMEOUT = 500};

    // not implemented
    hobartUsbPrinter( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTUSBPRINTER_H
