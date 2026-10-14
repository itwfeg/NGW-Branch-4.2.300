/**
 * @file atmelsamba.h
 * Atmel Samba Bootloader USB Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef ATMELSAMBA_H
#define ATMELSAMBA_H

#include "usbdev.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

// Atmel Samba Bootloader USB Device.
class atmelSamba : public usbDev
{
    Q_OBJECT

    typedef atmelSamba _Myt;
    typedef usbDev _Mybase;

public:

    static const unsigned int ATMEL_VID = 0x03eb;
    static const unsigned int ATMEL_PID = 0x6124;

    // ========================================================================
    // CTOR/DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] device  device pointer
     */
    atmelSamba( libusb_device *device );

    /// Destructor.
    virtual ~atmelSamba();

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Boot device.
    virtual void boot();

private:

    enum {XFER_TIMEOUT = 500};

    enum {INTERFACE_INTERRUPT = 0};
    enum {INTERFACE_BULK = 1};

    usbDevEndpoint *bulkIn_;
    usbDevEndpoint *bulkOut_;

    /// Initialize device.
    void initialize();

    // not implemented
    atmelSamba();

    // not implemented
    atmelSamba( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ATMELSAMBA_H
