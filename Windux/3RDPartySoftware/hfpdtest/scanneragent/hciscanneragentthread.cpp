/**
 * @file hcidevthread.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "hciscanneragent.h"
#include "hciscanneragentthread.h"
#include "leadvertinfo.h"

#include <errno.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <bluetooth/hci_lib.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include <QDebug>

///////////////////////////////////////////////////////////////////////////////////////////////////
hciScannerAgentThread::hciScannerAgentThread( QObject *parent ) :
    _Mybase( parent ),
    dev_( qobject_cast<hciScannerAgent*>( parent ) )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hciScannerAgentThread::~hciScannerAgentThread()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgentThread::stop()
{
    requestInterruption();

    do
    {
        // sleep a bit...
        msleep( 100 );

    } while ( isRunning() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgentThread::run()
{
    // sanity check
    if ( !dev_ )
        return;

    bool done( false );
    bool error( false );

    // main read loop
    do
    {
        unsigned char buf[HCI_MAX_EVENT_SIZE];
        ssize_t len( 0 );

        // read from device
        while ( (len = read( dev_->device_handle_, buf, sizeof(buf) )) < 0 )
        {
            // device interrupt
            done |= (EINTR == errno);

            if ( done )
                break;

            // nothing to read
            if ( EAGAIN == errno )
            {
                usleep( 100 );
                continue;
            }

            qDebug() << "Read error: " << errno;
            error = true;
        }

        // process buffer
        if (( !done ) && ( !error ))
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

    } while (( !done ) && ( !error ) && ( !isInterruptionRequested() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgentThread::processEvent( const evt_le_meta_event *e, size_t len )
{
    switch ( e->subevent )
    {
    case EVT_LE_ADVERTISING_REPORT:

        {
            leAdvertInfo info( std::string( (const char *) e->data, len-1 ) );

            // queue up event
            if ( info.isValid() )
                dev_->processAdvert( &info );
        }

        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hciScannerAgentThread::readRSSI( const bdaddr_t *bdaddr, int &rssi )
{
    bool result( false );

    // get device info
    struct hci_dev_info di;

    if ( hci_devinfo( dev_->device_id_, &di ) < 0 )
    {
        qDebug() << "Can't get device info";
        return false;
    }

    // create a connection to device
    uint16_t handle;

    if ( hci_create_connection( dev_->device_handle_, bdaddr, htobs(di.pkt_type & ACL_PTYPE_MASK), 0, 0x01, &handle, 25000 ) < 0 )
    {
        qDebug() << "Can't create connection";
        return false;
    }

    sleep( 1 );

    // retrieve connection info
    struct hci_conn_info_req *cr( (struct hci_conn_info_req *) malloc( sizeof(*cr) + sizeof(struct hci_conn_info) ) );

    bacpy( &cr->bdaddr, bdaddr );
    cr->type = ACL_LINK;

    if ( ioctl( dev_->device_handle_, HCIGETCONNINFO, (unsigned long) cr ) < 0 )
    {
        qDebug() << "Get connection info failed";
        goto done;
    }

    // get the rssi
    int8_t raw_rssi;

    if ( hci_read_rssi( dev_->device_handle_, htobs( cr->conn_info->handle ), &raw_rssi, 1000 ) < 0 )
    {
        qDebug() << "Read RSSI failed";
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
