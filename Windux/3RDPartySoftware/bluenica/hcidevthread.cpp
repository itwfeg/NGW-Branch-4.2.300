/**
 * @file hcidevthread.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "leadvertinfo.h"
#include "hcidev.h"
#include "hcidevthread.h"

#include <errno.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <bluetooth/hci_lib.h>

#include <cstdlib>
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
void *startReadLoop( void *ptr )
{
    hcidevThread *obj( reinterpret_cast<hcidevThread*>( ptr ) );

    // go!
    obj->readLoop();

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hcidevThread::hcidevThread( hcidev *dev ) :
    dev_( dev ),
    running_( false )
{
    int rc;

    if ( 0 != (rc = pthread_mutex_init( &m_, NULL )) )
        std::cerr << "Failed to init mutex: " << rc << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hcidevThread::~hcidevThread()
{
    stop();

    pthread_mutex_destroy( &m_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t hcidevThread::eventCount() const
{
    pthread_mutex_lock( &m_ );
    size_t count( le_advert_info_.size() );
    pthread_mutex_unlock( &m_ );

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
leAdvertInfo hcidevThread::event()
{
    pthread_mutex_lock( &m_ );

    leAdvertInfo e( le_advert_info_.front() );
    le_advert_info_.pop_front();

    pthread_mutex_unlock( &m_ );

    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hcidevThread::readLoop()
{
    // sanity check
    if ( !dev_ )
        return;

    running_ = true;

    // main read loop
    do
    {
        unsigned char buf[HCI_MAX_EVENT_SIZE];
        ssize_t len( 0 );

        // read from device
        while ( (len = read( dev_->device_handle_, buf, sizeof(buf) )) < 0 )
        {
            // device interrupt
            done_ |= (EINTR == errno);

            if ( done_ )
                break;

            // nothing to read
            if ( EAGAIN == errno )
            {
                usleep( 100 );
                continue;
            }

            std::cerr << "Read error: " << errno << std::endl;
            error_ = true;
        }

        // process buffer
        if (( !done_ ) && ( !error_ ))
        {
            unsigned char *ptr( buf );

            // handle each event
            while ( ptr < &buf[len] )
            {
                int type( *ptr++ );

                if ( HCI_EVENT_PKT != type )
                    break;

                hci_event_hdr *header( (hci_event_hdr *) ptr );
                ptr += HCI_EVENT_HDR_SIZE;

                switch ( header->evt )
                {
                case EVT_LE_META_EVENT:
                    processEvent( (evt_le_meta_event *) ptr, header->plen );
                    break;

                default:
                    break;
                }

                ptr += header->plen;
            }
        }

    } while (( !done_ ) && ( !error_ ));

    running_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hcidevThread::start()
{
    // we are not running (yet...)
    if ( !running_ )
    {
        int rc;

        // start thread
        done_ = false;
        error_ = false;

        if ( (rc = pthread_create( &thread_, NULL, startReadLoop, this )) )
        {
            std::cerr << "Error creating thread: " << rc << std::endl;
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hcidevThread::stop()
{
    // need to stop thread
    if ( running_ )
    {
        int rc;

        // stop thread
        done_ = true;

        if ( (rc = pthread_join( thread_, NULL )) )
            std::cerr << "Error joining thread: " << rc << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hcidevThread::processEvent( const evt_le_meta_event *e, size_t len )
{
    switch ( e->subevent )
    {
    case EVT_LE_ADVERTISING_REPORT:

        {
            leAdvertInfo info( std::string( (const char *) e->data, len-1 ) );

            // queue up event
            if ( info.isValid() )
            {
                pthread_mutex_lock( &m_ );
                le_advert_info_.push_back( info );
                pthread_mutex_unlock( &m_ );
            }
        }

        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hcidevThread::readRSSI( const bdaddr_t *bdaddr, int &rssi )
{
    bool result( false );

    // get device info
    struct hci_dev_info di;

    if ( hci_devinfo( dev_->device_id_, &di ) < 0 )
    {
        std::cerr << "Can't get device info" << std::endl;
        return false;
    }

    // create a connection to device
    uint16_t handle;

    if ( hci_create_connection( dev_->device_handle_, bdaddr, htobs(di.pkt_type & ACL_PTYPE_MASK), 0, 0x01, &handle, 25000 ) < 0 )
    {
        std::cerr << "Can't create connection" << std::endl;
        return false;
    }

    sleep( 1 );

    // retrieve connection info
    struct hci_conn_info_req *cr( (struct hci_conn_info_req *) malloc( sizeof(*cr) + sizeof(struct hci_conn_info) ) );

    bacpy( &cr->bdaddr, bdaddr );
    cr->type = ACL_LINK;

    if ( ioctl( dev_->device_handle_, HCIGETCONNINFO, (unsigned long) cr ) < 0 )
    {
        std::cerr << "Get connection info failed" << std::endl;
        goto done;
    }

    // get the rssi
    int8_t raw_rssi;

    if ( hci_read_rssi( dev_->device_handle_, htobs( cr->conn_info->handle ), &raw_rssi, 1000 ) < 0 )
    {
        std::cerr << "Read RSSI failed" << std::endl;
        goto done;
    }

    result = true;
    rssi = raw_rssi;

    usleep( 10000 );

done:

    free( cr );

    // disconnect
    hci_disconnect( dev_->device_handle_, handle, HCI_OE_USER_ENDED_CONNECTION, 10000 );

    return result;
}
