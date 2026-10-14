/**
 * @file usbdev.h
 * USB Device Interface.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef USBDEV_H
#define USBDEV_H

#include <QByteArray>
#include <QMap>
#include <QMutex>
#include <QObject>

#include <libusb.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// USB Device Endpoint.
class usbDevEndpoint : public QObject
{
    Q_OBJECT

    typedef usbDevEndpoint _Myt;
    typedef QObject _Mybase;

    friend void libusb_transfer_callback( libusb_transfer *transfer );

Q_SIGNALS:

    /// Signal for data processing after read.
    void readyRead();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] handle  device handle
     * @param[in] desc  endpoint descriptor
     * @param[in] parent  parent
     */
    usbDevEndpoint( libusb_device_handle *handle, const libusb_endpoint_descriptor *desc, QObject *parent = 0 );

    /// Destructor.
    virtual ~usbDevEndpoint();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve endpoint address.
    /**
     * @return  endpoint address
     */
    virtual unsigned char address() const {return endpoint_;}

    /// Retrieve bytes available for reading.
    /**
     * @return  bytes available
     */
    virtual size_t bytesAvailable() const;

    /// Retrieve endpoint direction.
    /**
     * @return  direction
     */
    virtual unsigned int direction() const {return (0x80 & endpoint_);}

    /// Retrieve endpoint type.
    /**
     * @return  type
     */
    virtual unsigned int type() const {return (0x03 & attr_);}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Peek bytes.
    /**
     * @param[in] maxSize  maximum bytes to peek
     * @return  copy of bytes pending
     */
    virtual QByteArray peek( size_t maxSize ) const;

    /// Read bytes.
    /**
     * @param[in] maxSize  maximum bytes to read
     * @return  bytes pending
     */
    virtual QByteArray read( size_t maxSize );

    /// Read all bytes available.
    /**
     * @return  bytes pending
     */
    virtual QByteArray readAll();

    /// Transfer data.
    /**
     * The direction of the transfer is inferred from the direction bits of the endpoint address.
     *
     * For reads, the @p bufferLen field indicates the maximum length of data you are
     * expecting to receive. If less data arrives than expected, this function will return that
     * data, so be sure to check the return value.
     *
     * @param[in,out] buffer  a suitably-sized data buffer for either input or output (depending on endpoint)
     * @param[in] bufferLen  for writes, the number of bytes from data to be sent; for reads, the maximum number of bytes to receive into the data buffer
     * @param[in] timeout  timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0
     * @return  number of bytes transferred
     */
    virtual size_t transfer( unsigned char *buffer, size_t bufferLen, unsigned int timeout = 0 );

    /// Wait for data to be available.
    /**
     * @param[in] msecs  how long to wait
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool waitForReadyRead( int msecs );

protected:

    libusb_device_handle *handle_;                  ///< Device handle.
    unsigned char endpoint_;                        ///< Endpoint address.
    unsigned char attr_;                            ///< Endpoint attributes.

    QByteArray buffer_;                             ///< Receive buffer.

    libusb_transfer *transfer_;                     ///< Transfer request.

private:

    mutable QMutex m_;

    /// Process data receive.
    void processRecv( const unsigned char *buffer, size_t bufferLen );

    // not implemented
    usbDevEndpoint();

    // not implemented
    usbDevEndpoint( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// USB Device Interface.
class usbDev : public QObject
{
    Q_OBJECT

    typedef usbDev _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~usbDev();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if device is open.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool isOpen() const;

    /// Retrieve product id.
    /**
     * @return  USB PID
     */
    virtual unsigned int pid() const {return deviceDescriptor_.idProduct;}

    /// Retrieve vendor id.
    /**
     * @return  USB VID
     */
    virtual unsigned int vid() const {return deviceDescriptor_.idVendor;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close device.
    virtual void close();

    /// Reset device.
    virtual void reset();

protected:

    /// Map of endpoints.
    typedef QMap<unsigned char, usbDevEndpoint*> endpointMap;

    libusb_device_handle *handle_;                  ///< Handle to usb device.
    int interfaceNum_;                              ///< Interface number.

    libusb_device_descriptor deviceDescriptor_;     /// Device descriptor.

    endpointMap endpoints_;                         /// Map of endpoints.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] device  device pointer
     * @param[in] interface_number  interface
     */
    usbDev( libusb_device *device, int interface_number = 0 );

private:

    bool kernelDriverActive_;

    /// Open device.
    virtual bool open( libusb_device *device );

    /// Enumerate endpoints.
    virtual void enumerateEndpoints();

    // not implemented
    usbDev();

    // not implemented
    usbDev( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // USBDEV_H
