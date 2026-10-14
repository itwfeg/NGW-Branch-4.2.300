/**
 * @file prepack.h
 * Hobart Prepack Weigher USB Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PREPACK_H
#define PREPACK_H

#include "hobartusbdev.h"
#include "hobartusbweigher.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart Prepack Weigher USB Device.
class prepack : public hobartUsbDev, public hobartUsbWeigher
{
    Q_OBJECT

    typedef prepack _Myt;
    typedef hobartUsbDev _Mybase;

Q_SIGNALS:

    /// Signal for weigher state changed.
    /**
     * @param[in] s  state
     * @param[in] weight  weight
     */
    void weigherStateChanged( hobartWeigher::weigherState s, float weight );

public:

    // ========================================================================
    // CTOR/DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] device  device pointer
     */
    prepack( libusb_device *device );

    /// Destructor.
    virtual ~prepack();

protected:

    usbDevEndpoint *interruptIn_;                   ///< Interrupt in endpoint.
    usbDevEndpoint *interruptOut_;                  ///< Interrupt out endpoint.

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device physical address.
    /**
     * @return  physical address
     */
    virtual unsigned short physicalAddr() const {return PHYSICAL_ADDR_PREPACK;}

    /// Retrieve weigher interrupt in.
    /**
     * @return  weigher interrupt in
     */
    virtual usbDevEndpoint *weigherInterruptIn() const {return interruptIn_;}

    /// Retrieve weigher interrupt out.
    /**
     * @return  weigher interrupt out
     */
    virtual usbDevEndpoint *weigherInterruptOut() const {return interruptOut_;}

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
    virtual void recvData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen );

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

private:

    enum {PHYSICAL_ADDR_PREPACK = 0x5555};

    // not implemented
    prepack();

    // not implemented
    prepack( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PREPACK_H
