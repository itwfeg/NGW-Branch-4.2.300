/**
 * @file hciscanneragent.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "hciscanneragent.h"
#include "hciscanneragentthread.h"

#include <errno.h>

#include <sys/ioctl.h>

#include <bluetooth/hci_lib.h>

#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QDebug>

///////////////////////////////////////////////////////////////////////////////////////////////////
hciScannerAgent::hciScannerAgent( QObject *parent ) :
    _Mybase( parent ),
    device_id_( 0 ),
    device_handle_( 0 ),
    state_( HCI_STATE_NONE ),
    t_( NULL )
{
    qRegisterMetaType<QBluetoothDeviceInfo>( "QBluetoothDeviceInfo" );

    memset( &original_filter_, 0, sizeof(original_filter_) );

    // open default device
    openDefaultDevice();

    // retrieve adapter address
    bdaddr_t addr;

    if ( adapterAddress( &addr ) )
    {
        char buffer[32];
        ba2str( &addr, buffer );

        qInfo() << "local device address" << buffer;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hciScannerAgent::~hciScannerAgent()
{
    // stop scanning
    stopScanning();

    // close device
    close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hciScannerAgent::isScanningActive() const
{
    const deviceState s( state() );

    return (( HCI_STATE_SCANNING == s ) || ( HCI_STATE_FILTERING == s ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::startScanning( scanType type )
{
    // sanity check
    if ( HCI_STATE_NONE == state() )
        return;
    else if ( isScanningActive() )
        return;

    qInfo() << "=== START SCAN ===";

    // set type if default passed in
    if ( SCAN_TYPE_DEFAULT == type )
        type = SCAN_TYPE_PASSIVE;

    // set scan parameters
    int stype( 0x01 );

    if ( SCAN_TYPE_PASSIVE == type )
        stype = 0x00;

    if ( hci_le_set_scan_parameters( device_handle_, stype, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 1000 ) < 0 )
    {
        qWarning() << "failed to set scan params...";

        QString err( "Failed to set scan parameters: " );
        err.append( strerror( errno ) );

        setErrorString( err );

        // attempt to reset device
        reset();

        // try once more...
        if ( hci_le_set_scan_parameters( device_handle_, stype, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 1000 ) < 0 )
            return;

        // yay! worked!
        resetErrorString();
    }

    // enable scan
    if ( hci_le_set_scan_enable( device_handle_, 0x01, 0, 1000 ) < 0 )
    {
        QString err( "Failed to enable scan: " );
        err.append( strerror( errno ) );

        setErrorString( err );
    }

    // we are scanning!
    state_ = HCI_STATE_SCANNING;

    qInfo() << "scan started!";
    emit scanningStarted();

    // save the current HCI filter
    socklen_t olen( sizeof( original_filter_ ) );

    if ( getsockopt( device_handle_, SOL_HCI, HCI_FILTER, &original_filter_, &olen ) < 0 )
    {
        QString err( "Could not get socket options: " );
        err.append( strerror( errno ) );

        setErrorString( err );
        return;
    }

    // create and set the new filter
    struct hci_filter new_filter;

    hci_filter_clear( &new_filter );
    hci_filter_set_ptype( HCI_EVENT_PKT, &new_filter );
    hci_filter_all_events( &new_filter );

    if ( setsockopt( device_handle_, SOL_HCI, HCI_FILTER, &new_filter, sizeof(new_filter) ) < 0 )
    {
        QString err( "Could not set socket options: " );
        err.append( strerror( errno ) );

        setErrorString( err );
    }

    // we are filtering!
    state_ = HCI_STATE_FILTERING;

    // startup io thread
    t_ = new hciScannerAgentThread( this );
    t_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::stopScanning()
{
    if ( isScanningActive() )
        return;

    qInfo() << "=== STOP SCAN ===";

    // shutdown io thread
    if ( t_ )
    {
        t_->stop();
        delete t_;

        t_ = NULL;
    }

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
            QString err( "Disable scan failed: " );
            err.append( strerror( errno ) );

            setErrorString( err );
            return;
        }

        state_ = HCI_STATE_OPEN;
    }

    // emit signal
    qInfo() << "scan stopped!";
    emit scanningStopped();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hciScannerAgent::adapterAddress( bdaddr_t *bdaddr ) const
{
    if ( HCI_STATE_NONE == state() )
        return false;
    else if ( !bdaddr )
        return false;

    // retrieve address
    return ( 0 <= hci_devba( device_id_, bdaddr ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::processAdvert( const leAdvertInfo *info )
{
    if ( !info->isValid() )
        return;

    const std::string localNameShort( info->eir( GAP_ADTYPE_LOCAL_NAME_SHORT ) );
    const std::string localNameComplete( info->eir( GAP_ADTYPE_LOCAL_NAME_COMPLETE ) );

    const QString address( QString::fromStdString( info->address() ) );

    // generate device name
    QString name( "(Unknown)" );

    if ( localNameComplete.length() )
        name = QString::fromStdString( localNameComplete );
    else if ( localNameShort.length() )
        name = QString::fromStdString( localNameShort );

    // generate device info
    QBluetoothDeviceInfo deviceInfo( QBluetoothAddress( address ), name, 0 );
    deviceInfo.setCoreConfigurations( QBluetoothDeviceInfo::LowEnergyCoreConfiguration );
    deviceInfo.setRssi( info->rssi() );
/*
    qInfo() << address << "name:" << name;
    qInfo() << address << "rssi:" << deviceInfo.rssi();
*/
    // TEST TEST TEST
    if ( name.contains( "ITWH" ) )
        info->print();

    // emit signal
    emit deviceDiscovered( deviceInfo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::openDefaultDevice()
{
    device_id_ = hci_get_route( NULL );

    // open device
    if ( (device_handle_ = hci_open_dev( device_id_ )) < 0 )
    {
        QString err( "Could not open device: " );
        err.append( strerror( errno ) );

        setErrorString( err );
        return;
    }

    // set fd non-blocking
    int on( 1 );

    if ( ioctl( device_handle_, FIONBIO, (char*) &on ) < 0 )
    {
        QString err( "Could set device to non-blocking: " );
        err.append( strerror( errno ) );

        setErrorString( err );
        return;
    }

    // open
    state_ = HCI_STATE_OPEN;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::close()
{
    // close device
    if ( HCI_STATE_OPEN == state_ )
    {
        hci_close_dev( device_handle_ );
        state_ = HCI_STATE_NONE;
    }

    device_handle_ = 0;
    device_id_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::reset()
{
    system( "hciconfig hci0 down" );
    system( "hciconfig hci0 up" );
}
