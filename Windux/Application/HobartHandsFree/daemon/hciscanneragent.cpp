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
#include <QDebug>
#include <QThread>

#define sleep( sec ) QThread::sleep( sec )
#define msleep( ms ) QThread::msleep( ms )
#define usleep( us ) QThread::usleep( us )

///////////////////////////////////////////////////////////////////////////////////////////////////
hciScannerAgent::hciScannerAgent( QObject *parent ) :
    _Mybase( parent ),
    device_id_( 0 ),
    device_handle_( 0 ),
    state_( HCI_STATE_NONE ),
    t_( NULL )
{
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

    if (( HCI_STATE_SCANNING == s ) || ( HCI_STATE_FILTERING == s ))
        return (( t_ ) && ( t_->isRunning() ));

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::reset()
{
    if ( isScanningActive() )
        stopScanning();

    // close device
    close();

    // restart bluetooth daemon
    std::system( "hciconfig hci0 down" );

    sleep( 1 );
    std::system( "hciconfig hci0 up" );

    // open device
    openDefaultDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::startScanning( scanType type )
{
    int rc;

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

    if ( (rc = hci_le_set_scan_parameters( device_handle_, stype, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 1000 )) < 0 )
    {
        qWarning() << "failed to set scan params (1)" << rc << errno;

        QString err( "Failed to set scan parameters: " );
        err.append( strerror( errno ) );

        setErrorString( err );

        // attempt to reset device
        reset();

        // try once more...
        if ( (rc = hci_le_set_scan_parameters( device_handle_, stype, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 1000 )) < 0 )
        {
            qWarning() << "failed to set scan params (2)" << rc << errno;

            // attempt to reset device
            reset();

            // try once more...
            if ( (rc = hci_le_set_scan_parameters( device_handle_, stype, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 1000 )) < 0 )
            {
                qWarning() << "failed to set scan params (3)" << rc << errno;
                return;
            }
        }

        // yay! worked!
        resetErrorString();
    }

    // enable scan
    if ( (rc = hci_le_set_scan_enable( device_handle_, 0x01, 0, 1000 )) < 0 )
    {
        qWarning() << "failed to enable scan" << rc << errno;

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

    if ( (rc = getsockopt( device_handle_, SOL_HCI, HCI_FILTER, &original_filter_, &olen )) < 0 )
    {
        qWarning() << "failed to get sock options" << rc << errno;

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

    if ( (rc = setsockopt( device_handle_, SOL_HCI, HCI_FILTER, &new_filter, sizeof(new_filter) )) < 0 )
    {
        qWarning() << "failed to set sock options" << rc << errno;

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
    int rc;

    if ( !isScanningActive() )
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

        if ( (rc = setsockopt( device_handle_, SOL_HCI, HCI_FILTER, &original_filter_, sizeof(original_filter_))) < 0 )
            qWarning() << "failed to set sock options" << rc << errno;
    }

    // stop scanning
    if ( HCI_STATE_SCANNING == state_ )
    {
        int timeout( 0 );

        // disable scan
        for ( ;; )
            if ( 0 <= (rc = hci_le_set_scan_enable( device_handle_, 0x00, 1, 1000 )) )
                break;
            else
            {
                const QString attempt( "(" + QString::number( ++timeout ) + ")" );

                qWarning() << "failed to disable scan" << qPrintable( attempt ) << rc << errno;

                if (( ETIMEDOUT == errno ) && ( timeout <= 5 ))
                {
                    usleep( 100 );
                    continue;
                }

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

//    info->print();

    const std::string localNameComplete( info->eir( GAP_ADTYPE_LOCAL_NAME_COMPLETE ) );
    const std::string localNameShort( info->eir( GAP_ADTYPE_LOCAL_NAME_SHORT ) );
    const std::string manufacturerSpecific( info->eir( GAP_ADTYPE_MANUFACTURER_SPECIFIC ) );

    // check for hobart tag
    // eir 0x08 = ITWH
    if (( 0 != localNameComplete.compare( 0, 3, "ITW" ) ) &&
        ( 0 != localNameShort.compare( 0, 3, "ITW" ) ))
        return;

    // application (old firmware)
    // eir 0xff = 0xAF 0xFF <MAC ADDR>
    if (( 0 == localNameComplete.compare( 0, 4, "ITWH" ) ) &&
        ( 8 <= manufacturerSpecific.length() ) &&
        ( 0xff == manufacturerSpecific[0] ) &&
        ( 0xff == manufacturerSpecific[1] ))
    {
        const std::string address( info->address() );

        bdaddr_t ba0;
        str2ba( address.c_str(), &ba0 );

        bdaddr_t ba1;
        ba1.b[0] = ba0.b[5];
        ba1.b[1] = ba0.b[4];
        ba1.b[2] = ba0.b[3];
        ba1.b[3] = ba0.b[2];
        ba1.b[4] = ba0.b[1];
        ba1.b[5] = ba0.b[0];

        if (( 0 == memcmp( &manufacturerSpecific[2], &ba0, sizeof(ba0) ) ) ||
            ( 0 == memcmp( &manufacturerSpecific[2], &ba1, sizeof(ba1) ) ))
        {
            qDebug() << "FOUND TAG V0" << address.c_str() << info->rssi();

            // determine device name
            const QString name( localNameComplete.empty() ? address.c_str() : localNameComplete.c_str() );

            // generate bluetooth device info
            QBluetoothDeviceInfo bluetoothDeviceInfo( QBluetoothAddress( QString( address.c_str() ) ), name, QBluetoothDeviceInfo::LowEnergyCoreConfiguration );
            bluetoothDeviceInfo.setRssi( info->rssi() );

            // generate device info
            scannerAgentDeviceInfo deviceInfo;
            deviceInfo.info = bluetoothDeviceInfo;
            deviceInfo.type = ntohs( *reinterpret_cast<const uint16_t*>( &manufacturerSpecific[0] ) );

            deviceInfo.lowPowerAdvert = true;
            deviceInfo.highPowerAdvert = false;

            deviceInfo.transmitPowerLevel = 0;
            deviceInfo.firmwareVersion = 1; // hard coded v0.1
            deviceInfo.batteryLevel = 0;
            deviceInfo.messages = 0;

            // emit signal
            emit deviceDiscovered( deviceInfo );
        }
    }

    // application (new firmware)
    // eir 0xff = 0xAF ? <MAC ADDR> <FLAGS>
    //
    // 0xF1 device with display
    // 0xF0 device without display
    else if (( 0 == localNameShort.compare( 0, 4, "ITWH" ) ) &&
        ( 8 <= manufacturerSpecific.length() ) &&
        ( 0xaf == manufacturerSpecific[0] ))
    {
        if (( 0xf0 == manufacturerSpecific[1] ) || ( 0xf1 == manufacturerSpecific[1] ))
        {
            const std::string address( info->address() );

            bdaddr_t ba0;
            str2ba( address.c_str(), &ba0 );

            bdaddr_t ba1;
            ba1.b[0] = ba0.b[5];
            ba1.b[1] = ba0.b[4];
            ba1.b[2] = ba0.b[3];
            ba1.b[3] = ba0.b[2];
            ba1.b[4] = ba0.b[1];
            ba1.b[5] = ba0.b[0];

            if (( 0 == memcmp( &manufacturerSpecific[2], &ba0, sizeof(ba0) ) ) ||
                ( 0 == memcmp( &manufacturerSpecific[2], &ba1, sizeof(ba1) ) ))
            {
                qDebug() << "FOUND TAG V1" << address.c_str() << info->rssi();

                const std::string powerLevel( info->eir( GAP_ADTYPE_POWER_LEVEL ) );
                const std::string serviceData( info->eir( GAP_ADTYPE_SERVICE_DATA ) );

                // determine device name
                const QString name( localNameShort.empty() ? address.c_str() : localNameShort.c_str() );

                // generate bluetooth device info
                QBluetoothDeviceInfo bluetoothDeviceInfo( QBluetoothAddress( QString( address.c_str() ) ), name, QBluetoothDeviceInfo::LowEnergyCoreConfiguration );
                bluetoothDeviceInfo.setRssi( info->rssi() );

                // generate device info
                scannerAgentDeviceInfo deviceInfo;
                deviceInfo.info = bluetoothDeviceInfo;
                deviceInfo.type = ntohs( *reinterpret_cast<const uint16_t*>( &manufacturerSpecific[0] ) );

                deviceInfo.lowPowerAdvert = (8 == manufacturerSpecific.length());
                deviceInfo.highPowerAdvert = (8 < manufacturerSpecific.length());

                deviceInfo.transmitPowerLevel = 0;
                deviceInfo.firmwareVersion = 0;
                deviceInfo.batteryLevel = 0;
                deviceInfo.messages = 0;

                if ( powerLevel.length() )
                    deviceInfo.transmitPowerLevel = (signed char) powerLevel[0];

                if ( 10 <= manufacturerSpecific.length() )
                    deviceInfo.firmwareVersion = ntohs( *reinterpret_cast<const uint16_t*>( &manufacturerSpecific[8] ) );

                if (( 3 <= serviceData.length() ) && ( 0x180F == ntohs( *reinterpret_cast<const uint16_t*>( &serviceData[0] ) ) ))
                    deviceInfo.batteryLevel = (unsigned char) serviceData[2];

                if ( 11 <= manufacturerSpecific.length() )
                    deviceInfo.messages = (unsigned char) manufacturerSpecific[10];

                // emit signal
                emit deviceDiscovered( deviceInfo );
            }
        }
    }

    // dfu device
    // nordic dfu device that supports OTA upgrades
    else if ( 0 == localNameComplete.compare( 0, 6, "ITWDFU" ) )
    {
        const std::string uuid( info->eir( GAP_ADTYPE_128BIT_MORE ) );

        std::string uuid_expected;
        uuid_expected.push_back( 0x23 );
        uuid_expected.push_back( 0xd1 );
        uuid_expected.push_back( 0xbc );
        uuid_expected.push_back( 0xea );
        uuid_expected.push_back( 0x5f );
        uuid_expected.push_back( 0x78 );
        uuid_expected.push_back( 0x23 );
        uuid_expected.push_back( 0x15 );
        uuid_expected.push_back( 0xde );
        uuid_expected.push_back( 0xef );
        uuid_expected.push_back( 0x12 );
        uuid_expected.push_back( 0x12 );
        uuid_expected.push_back( 0x30 );
        uuid_expected.push_back( 0x15 );
        uuid_expected.push_back( 0x00 );
        uuid_expected.push_back( 0x00 );

        // validate UUID is DFU service
        // {00001530-1212-efde-1523-785feabcd123}
        if ( uuid == uuid_expected )
        {
            const std::string address( info->address() );

            qDebug() << "FOUND DFU DEVICE" << address.c_str() << info->rssi();

            // generate bluetooth device info
            QBluetoothDeviceInfo bluetoothDeviceInfo( QBluetoothAddress( QString( address.c_str() ) ), "DFU", QBluetoothDeviceInfo::LowEnergyCoreConfiguration );
            bluetoothDeviceInfo.setRssi( info->rssi() );

            // generate device info
            scannerAgentDeviceInfo deviceInfo;
            deviceInfo.info = bluetoothDeviceInfo;
            deviceInfo.type = 0xFFFF;

            // emit signal
            emit deviceDiscoveredOTA( deviceInfo );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hciScannerAgent::openDefaultDevice()
{
    device_id_ = hci_get_route( NULL );

    // open device
    if ( (device_handle_ = hci_open_dev( device_id_ )) < 0 )
    {
        qWarning() << "failed to open default device" << errno;

        QString err( "Could not open device: " );
        err.append( strerror( errno ) );

        setErrorString( err );
        return;
    }

    // set fd non-blocking
    int on( 1 );

    if ( ioctl( device_handle_, FIONBIO, (char*) &on ) < 0 )
    {
        qWarning() << "failed to set ioctl" << errno;

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
