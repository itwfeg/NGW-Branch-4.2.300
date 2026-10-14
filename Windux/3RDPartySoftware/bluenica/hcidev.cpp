/**
 * @file hcidev.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "hcidev.h"

#include <errno.h>

#include <sys/ioctl.h>

#include <bluetooth/hci_lib.h>

#include <sstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
hcidev::hcidev()
{
    reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hcidev::hcidev( const hcidev &rhs )
{
    copy( rhs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hcidev::~hcidev()
{
    close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hcidev& hcidev::operator = ( const hcidev &rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hcidev::adapterAddress( bdaddr_t *bdaddr ) const
{
    if ( !isValid() )
        return false;
    else if ( !bdaddr )
        return false;

    // retrieve address
    return ( 0 <= hci_devba( device_id_, bdaddr ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hcidev::startScanning( scanType type )
{
    // sanity check
    if ( !isValid() )
        return false;
    else if ( HCI_STATE_SCANNING == state() )
        return true;
    else if ( HCI_STATE_FILTERING == state() )
        return true;

    // set scan parameters
    int stype( 0x01 );

    if ( SCAN_TYPE_PASSIVE == type )
        stype = 0x00;

    if ( hci_le_set_scan_parameters( device_handle_, stype, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 1000 ) < 0 )
    {
        std::ostringstream os;
        os << "Failed to set scan parameters: " << strerror( errno );

        setErrorString( os.str() );
        return false;
    }

    // enable scan
    if ( hci_le_set_scan_enable( device_handle_, 0x01, 0, 1000 ) < 0 )
    {
        std::ostringstream os;
        os << "Failed to enable scan: " << strerror( errno );

        setErrorString( os.str() );
        return false;
    }

    // we are scanning!
    state_ = HCI_STATE_SCANNING;

    // save the current HCI filter
    socklen_t olen( sizeof( original_filter_ ) );

    if ( getsockopt( device_handle_, SOL_HCI, HCI_FILTER, &original_filter_, &olen ) < 0 )
    {
        std::ostringstream os;
        os << "Could not get socket options: " << strerror( errno );

        setErrorString( os.str() );
        return false;
    }

    // create and set the new filter
    struct hci_filter new_filter;

    hci_filter_clear( &new_filter );
    hci_filter_set_ptype( HCI_EVENT_PKT, &new_filter );
    hci_filter_all_events( &new_filter );

    if ( setsockopt( device_handle_, SOL_HCI, HCI_FILTER, &new_filter, sizeof(new_filter) ) < 0 )
    {
        std::ostringstream os;
        os << "Could not set socket options: " << strerror( errno );

        setErrorString( os.str() );
        return false;
    }

    // we are filtering!
    state_ = HCI_STATE_FILTERING;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hcidev::stopScanning()
{
    // stop filtering
    if ( HCI_STATE_FILTERING == state_ )
    {
        state_ = HCI_STATE_SCANNING;
        setsockopt( device_handle_, SOL_HCI, HCI_FILTER, &original_filter_, sizeof(original_filter_) );
    }

    // stop scanning
    if ( HCI_STATE_SCANNING == state_ )
    {
        // disable scan
        if ( hci_le_set_scan_enable( device_handle_, 0x00, 1, 1000 ) < 0 )
        {
            std::ostringstream os;
            os << "Disable scan failed: " << strerror( errno );

            setErrorString( os.str() );
            return false;
        }

        state_ = HCI_STATE_OPEN;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hcidev hcidev::openDefaultDevice()
{
    hcidev dev;

    dev.device_id_ = hci_get_route( NULL );

    // open device
    if ( (dev.device_handle_ = hci_open_dev( dev.device_id_ )) < 0 )
    {
        std::ostringstream os;
        os << "Could not open device: " << strerror( errno );

        dev.setErrorString( os.str() );

        return dev;
    }

    // set fd non-blocking
    int on( 1 );

    if ( ioctl( dev.device_handle_, FIONBIO, (char*) &on ) < 0 )
    {
        std::ostringstream os;
        os << "Could set device to non-blocking: " << strerror( errno );

        dev.setErrorString( os.str() );

        return dev;
    }

    // open
    dev.state_ = HCI_STATE_OPEN;

    return dev;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hcidev::setErrorString( const std::string &value )
{
    has_error_ = true;
    error_message_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hcidev::close()
{
    // stop scanning
    stopScanning();

    // close device
    if ( HCI_STATE_OPEN == state_ )
    {
        hci_close_dev( device_handle_ );
        reset();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hcidev::resetErrorString()
{
    has_error_ = false;
    error_message_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hcidev::copy( const _Myt &rhs )
{
    device_id_ = rhs.device_id_;
    device_handle_ = rhs.device_id_;

    memcpy( &original_filter_, &rhs.original_filter_, sizeof(original_filter_) );

    state_ = rhs.state_;

    has_error_ = rhs.has_error_;
    error_message_ = rhs.error_message_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hcidev::reset()
{
    device_id_ = 0;
    device_handle_ = 0;

    memset( &original_filter_, 0, sizeof(original_filter_) );

    state_ = HCI_STATE_NONE;

    has_error_ = false;
    error_message_.clear();
}
