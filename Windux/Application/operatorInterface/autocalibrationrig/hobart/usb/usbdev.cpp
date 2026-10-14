/**
 * @file usbdev.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "usbdev.h"

#include <iomanip>
#include <iostream>

#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////
void libusb_transfer_callback( libusb_transfer *transfer )
{
    int result;

    // validate transfer
    if ( LIBUSB_TRANSFER_COMPLETED != transfer->status )
    {
        std::cerr << "transfer on endpoint " << (unsigned int) transfer->endpoint << " failed status " << transfer->status << std::endl;
        return;
    }
#if 0
    // print receive buffer
    std::cout << "endpoint " << (unsigned int) transfer->endpoint << ": ";

    for ( int i( 0 ); i < transfer->actual_length; ++i )
        std::cout << std::uppercase << std::setfill('0') << std::setw( 2 ) << std::hex << (unsigned int) transfer->buffer[i];

    std::cout << std::endl << std::dec;
#endif
    // process buffer
    usbDevEndpoint *instance( (usbDevEndpoint*) transfer->user_data );
    instance->processRecv( transfer->buffer, transfer->actual_length );

    // resubmit transfer request
    if ( LIBUSB_SUCCESS != (result = libusb_submit_transfer( transfer )) )
        std::cerr << "libusb_submit_transfer failed rc=" << result << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
usbDevEndpoint::usbDevEndpoint( libusb_device_handle *handle, const libusb_endpoint_descriptor *desc, QObject *parent ) :
    _Mybase( parent ),
    handle_( handle ),
    endpoint_( desc->bEndpointAddress ),
    attr_( desc->bmAttributes ),
    transfer_( NULL )
{
    // print endpoint info
    std::string typeStr( "unknown" );
    std::string directionStr( "unknown" );

    if ( LIBUSB_TRANSFER_TYPE_BULK == type() )
        typeStr = "BULK";
    else if ( LIBUSB_TRANSFER_TYPE_INTERRUPT == type() )
        typeStr = "INTERRUPT";

    if ( LIBUSB_ENDPOINT_IN == direction() )
        directionStr = "IN";
    else if ( LIBUSB_ENDPOINT_OUT == direction() )
        directionStr = "OUT";

    std::cout << "endpoint " << (unsigned int) endpoint_ << " " << typeStr << " " << directionStr << std::endl;

    // setup async transfer
    if ( LIBUSB_ENDPOINT_IN == direction() )
    {
        libusb_transfer *transfer( libusb_alloc_transfer( 0 ) );

        if ( !transfer )
            std::cerr << "libusb_alloc_transfer failed" << std::endl;
        else
        {
            int result;

            // allocate buffer for transfers
            unsigned char *buffer( (unsigned char*) malloc( desc->wMaxPacketSize ) );

            // setup request
            if ( LIBUSB_TRANSFER_TYPE_BULK == type() )
                libusb_fill_bulk_transfer( transfer, handle_, endpoint_, buffer, desc->wMaxPacketSize, libusb_transfer_callback, this, 0 );
            else if ( LIBUSB_TRANSFER_TYPE_INTERRUPT == type() )
                libusb_fill_interrupt_transfer( transfer, handle_, endpoint_, buffer, desc->wMaxPacketSize, libusb_transfer_callback, this, 0 );

            transfer->flags = LIBUSB_TRANSFER_FREE_BUFFER;

            // submit request
            if ( LIBUSB_SUCCESS == (result = libusb_submit_transfer( transfer )) )
                transfer_ = transfer;
            else
            {
                std::cerr << "libusb_submit_transfer failed rc=" << result << std::endl;
                libusb_free_transfer( transfer );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
usbDevEndpoint::~usbDevEndpoint()
{
    // cleanup async transfer request
    if ( transfer_ )
    {
        libusb_free_transfer( transfer_ );
        transfer_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t usbDevEndpoint::bytesAvailable() const
{
    QMutexLocker guard( &m_ );

    return buffer_.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray usbDevEndpoint::peek( size_t maxSize ) const
{
    m_.lock();

    const QByteArray a( buffer_.left( maxSize ) );

    m_.unlock();

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray usbDevEndpoint::read( size_t maxSize )
{
    QMutexLocker guard( &m_ );

    const QByteArray a( buffer_.left( maxSize ) );

    buffer_.remove( 0, maxSize );

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray usbDevEndpoint::readAll()
{
    QMutexLocker guard( &m_ );

    const QByteArray a( buffer_ );

    buffer_.clear();

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t usbDevEndpoint::transfer( unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    int result( LIBUSB_ERROR_OTHER );

    std::cout << "transfer to endpoint " << (unsigned int) endpoint_ << "..." << std::endl;
    int transferred;

    // perform transfer
    if ( LIBUSB_TRANSFER_TYPE_INTERRUPT == type() )
        result = libusb_interrupt_transfer( handle_, endpoint_, buffer, bufferLen, &transferred, timeout );
    else if ( LIBUSB_TRANSFER_TYPE_BULK == type() )
        result = libusb_bulk_transfer( handle_, endpoint_, buffer, bufferLen, &transferred, timeout );

    if ( LIBUSB_SUCCESS == result )
    {
        std::cout << "transfer complete" << std::endl;
        return transferred;
    }
    else if ( LIBUSB_ERROR_TIMEOUT == result )
    {
        std::cout << "partial transfer complete" << std::endl;
        return transferred;
    }

    std::cerr << "transfer to endpoint failed rc=" << result << std::endl;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool usbDevEndpoint::waitForReadyRead( int msecs )
{
    do
    {
        if ( bytesAvailable() )
            return true;

        QThread::msleep( 50 );
        msecs -= 50;

    } while ( 0 < msecs );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDevEndpoint::processRecv( const unsigned char *buffer, size_t bufferLen )
{
    {
        QMutexLocker guard( &m_ );
        buffer_.append( (const char*) buffer, bufferLen );
    }

    Q_EMIT readyRead();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
usbDev::usbDev( libusb_device *device, int interface_number ) :
    _Mybase(),
    handle_( NULL ),
    interfaceNum_( interface_number ),
    kernelDriverActive_( false )
{
    std::cout << "usbDev CTOR..." << std::endl;

    // open device and enumerate endpoints
    if ( open( device ) )
        enumerateEndpoints();

    std::cout << "usbDev CTOR complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
usbDev::~usbDev()
{
    std::cout << "usbDev DTOR..." << std::endl;
    close();

    std::cout << "usbDev DTOR complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool usbDev::isOpen() const
{
    return (handle_ ? true : false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDev::close()
{
    int result;

    if ( !handle_ )
        return;

    std::cout << "free endpoints interface..." << std::endl;

    // delete all endpoints
    QList<usbDevEndpoint*> endpoints( endpoints_.values() );

    foreach ( usbDevEndpoint *e, endpoints )
        delete e;

    std::cout << "release interface..." << std::endl;

    // release interface
    if ( LIBUSB_SUCCESS != (result = libusb_release_interface( handle_, interfaceNum_ )) )
        std::cerr << "libusb_release_interface failed rc=" << result << std::endl;

    // attach kernel driver (if previously active)
    if ( kernelDriverActive_ )
    {
        std::cout << "attach kernel driver..." << std::endl;

        if ( LIBUSB_SUCCESS != (result = libusb_attach_kernel_driver( handle_, interfaceNum_ )) )
            std::cerr << "libusb_attach_kernel_driver failed rc=" << result << std::endl;
    }

    std::cout << "close device..." << std::endl;

    // close device
    libusb_close( handle_ );
    handle_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDev::reset()
{
    int result;

    std::cout << "reset device..." << std::endl;

    if ( LIBUSB_SUCCESS != (result = libusb_reset_device( handle_ )) )
        std::cerr << "libusb_reset_device failed rc=" << result << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool usbDev::open( libusb_device *device )
{
    int result;

    // retrieve device descriptor
    if ( LIBUSB_SUCCESS != (result = libusb_get_device_descriptor( device, &deviceDescriptor_ )) )
    {
        std::cerr << "lib_usb_device_descriptor failed rc=" << result << std::endl;
        return false;
    }

    std::cout << "open device..." << std::endl;

    // open device
    if (( LIBUSB_SUCCESS != (result = libusb_open( device, &handle_ )) ) || ( !handle_ ))
    {
        std::cerr << "libusb_open failed rc=" << result << std::endl;
        return false;
    }

    std::cout << "checking for kernel driver..." << std::endl;

    // detach kernel driver (if active)
    if ( 1 == (result = libusb_kernel_driver_active( handle_, interfaceNum_ )) )
    {
        std::cout << "detaching kernel driver..." << std::endl;

        if ( LIBUSB_SUCCESS == (result = libusb_detach_kernel_driver( handle_, interfaceNum_ )) )
            kernelDriverActive_ = true;
        else
        {
            std::cerr << "libusb_detach_kernel_driver failed rc=" << result << std::endl;
        }
    }
    else if ( 0 != result )
    {
        std::cerr << "libusb_kernel_driver_active failed rc=" << result << std::endl;
        goto error;
    }

    // claim interface
    std::cout << "claiming interface " << interfaceNum_ << "..." << std::endl;

    if ( LIBUSB_SUCCESS != (result = libusb_claim_interface( handle_, interfaceNum_ )) )
    {
        std::cerr << "libusb_claim_interface failed rc=" << result << std::endl;
        goto error;
    }

    return true;

error:

    std::cout << "open failed!" << std::endl;

    libusb_close( handle_ );
    handle_ = NULL;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDev::enumerateEndpoints()
{
    int result;

    // read device configuration
    libusb_config_descriptor *config( NULL );

    if ( LIBUSB_SUCCESS != (result = libusb_get_active_config_descriptor( libusb_get_device( handle_ ), &config )) )
        std::cerr << "libusb_get_active_config_descriptor failed rc=" << result << std::endl;
    else if ( config->bNumInterfaces <= interfaceNum_ )
        std::cout << "interface is missing from config!" << std::endl;
    else
    {
        const libusb_interface *interface( &config->interface[interfaceNum_] );

        if ( !interface->num_altsetting )
            std::cout << "interface descriptor is missing from config!" << std::endl;
        else
        {
            const libusb_interface_descriptor *interfaceDescriptor( &interface->altsetting[0] );

            std::cout << "enumerating " << (size_t) interfaceDescriptor->bNumEndpoints << " endpoints..." << std::endl;

            // loop through endpoints
            for ( size_t e( interfaceDescriptor->bNumEndpoints ); e--; )
            {
                const libusb_endpoint_descriptor *endpointDescriptor( &interfaceDescriptor->endpoint[e] );

                std::cout << "create endpoint " << (unsigned int) endpointDescriptor->bEndpointAddress << std::endl;

                // add endpoint to list
                endpoints_[endpointDescriptor->bEndpointAddress] = new usbDevEndpoint( handle_, endpointDescriptor, this );
            }
        }
    }

    if ( config )
        libusb_free_config_descriptor( config );
}
