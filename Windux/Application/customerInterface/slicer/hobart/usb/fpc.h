/**
 * @file fpc.h
 * Hobart FPC USB Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef FPC_H
#define FPC_H

#include "hobartprinter.h"
#include "hobartusbdev.h"
#include "hobartweigher.h"
#include "weigherconfiguration.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart FPC USB Device.
class fpc : public hobartUsbDev, public hobartPrinter, public hobartWeigher
{
    Q_OBJECT

    typedef fpc _Myt;
    typedef hobartUsbDev _Mybase;

Q_SIGNALS:

    /// Signal for weigher state changed.
    /**
     * @param[in] s  state
     * @param[in] weight  weight
     */
    void weigherStateChanged( hobartWeigher::weigherState s, float weight );
    /// Signal for weigher key pressed
    void sig_WeigherKeyPressed();
    void weigherConfigChanged(WeigherConfiguration & config);
    void sig_WeigherStatus(char* buffer, int buffLen);

public Q_SLOTS:
     void slt_WriteConfig(WeigherConfiguration & config);
     void slt_ReqConfig();


public:

    // ========================================================================
    // CTOR/DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] device  device pointer
     */
    fpc( libusb_device *device );

    /// Destructor.
    virtual ~fpc();

protected:

    usbDevEndpoint *interruptIn_;                   ///< Interrupt in endpoint.
    usbDevEndpoint *interruptOut_;                  ///< Interrupt out endpoint.

    usbDevEndpoint *bulkOut_;                       ///< Bulk out endpoint.

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device physical address.
    /**
     * @return  physical address
     */
    virtual unsigned short physicalAddr() const {return PHYSICAL_ADDR_FPC;}

    /// Retrieve printer bulk out.
    /**
     * @return  printer bulk out
     */
    virtual usbDevEndpoint *printerBulkOut() const {return bulkOut_;}

    /// Retrieve printer interrupt in.
    /**
     * @return  printer interrupt in
     */
    virtual usbDevEndpoint *printerInterruptIn() const {return interruptIn_;}

    /// Retrieve printer interrupt out.
    /**
     * @return  printer interrupt out
     */
    virtual usbDevEndpoint *printerInterruptOut() const {return interruptOut_;}

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

    enum {PHYSICAL_ADDR_FPC = 0x4646};

    // not implemented
    fpc();

    // not implemented
    fpc( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FPC_H
