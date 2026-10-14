/**
 * @file hobartusbweigher.h
 * Hobart USB Weigher Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTUSBWEIGHER_H
#define HOBARTUSBWEIGHER_H

#include "../hobartweigher.h"

#include <cstddef>

class hobartUsbDev;

class usbDevEndpoint;

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart USB Weigher Device.
class hobartUsbWeigher : public hobartWeigher
{
    typedef hobartUsbWeigher _Myt;
    typedef hobartWeigher _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartUsbWeigher();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set weigher mode.
    /**
     * @param[in] mode  mode
     */
    virtual void setWeigherMode( unsigned int mode );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Enable weigher.
    virtual void weigherEnable();

    /// Wakeup weigher.
    virtual void weigherWakeup();

    /// Zero weigher.
    virtual void weigherZero();

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    hobartUsbWeigher();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device logical address.
    /**
     * @return  logical address
     */
    virtual unsigned short logicalAddrWeigher() const {return LOGICAL_ADDR_WEIGHER;}

    /// Retrieve weigher interrupt in.
    /**
     * @return  weigher interrupt in
     */
    virtual usbDevEndpoint *weigherInterruptIn() const = 0;

    /// Retrieve weigher interrupt out.
    /**
     * @return  weigher interrupt out
     */
    virtual usbDevEndpoint *weigherInterruptOut() const = 0;

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
    virtual void recvWeigherMessage( const unsigned char *buffer, size_t bufferLen );

    /// Send message to device.
    /**
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @param[in] timeout  timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0
     * @return  number of bytes transferred
     */
    virtual size_t sendWeigherMessage( const unsigned char *buffer, size_t bufferLen, unsigned int timeout = XFER_TIMEOUT );

private:

    enum {LOGICAL_ADDR_WEIGHER = 0x0006};

    enum {XFER_TIMEOUT = 500};

    enum weigherMessage
    {
        wgWakeup = 0,
        wgStatus = 4,
        wgEnable = 5,
        wgMode = 7,
        wgRezero = 8,
        wgReqWakeup = 34,
        wgReqStatus = 35,
    };

    /// Send basic message to weigher.
    bool sendWeigherMessageBasic( weigherMessage msgType );

    // not implemented
    hobartUsbWeigher( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTUSBWEIGHER_H
