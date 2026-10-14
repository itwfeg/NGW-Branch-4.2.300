/**
 * @file hobartcanprinter.h
 * Hobart CAN Printer Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTCANPRINTER_H
#define HOBARTCANPRINTER_H

#include "candev.h"

#include "../hobartprinter.h"

#include <cstddef>

class hobartCanDev;

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart CAN Printer Device.
class hobartCanPrinter : public hobartPrinter
{
    typedef hobartCanPrinter _Myt;
    typedef hobartPrinter _Mybase;

public:

    /// CAN Frame Id Range (min, numFrames)
    typedef canDev::rangeType rangeType;

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartCanPrinter();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device range.
    /**
     * @return  device range
     */
    virtual rangeType printerDeviceRange() const;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] t  printer type
     */
    hobartCanPrinter( printerType t );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send data to device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool sendData( unsigned int id, const unsigned char *buffer, size_t bufferLen ) = 0;

    /// Receive message from device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     */
    virtual void recvPrinterMessage( unsigned int id, const unsigned char *buffer, size_t bufferLen );

    /// Send message to device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool sendPrinterMessage( unsigned int id, const unsigned char *buffer, size_t bufferLen );

    /// Register messages.
    /**
     * @param[in] dev  device
     */
    virtual void registerPrinterMessages( hobartCanDev *dev );

private:

    enum {CAN_ID_FIRST = 0x100};
    enum {CAN_ID_SECOND = 0x200};
    enum {CAN_ID_THIRD = 0x300};

    enum {CAN_RANGE = 0x100};

    unsigned int id_;

    // not implemented
    hobartCanPrinter( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTCANPRINTER_H
