/**
 * @file blescanneragent.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "blescanneragent.h"

#include <QBluetoothDeviceInfo>
#include <QDebug>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////
bleScannerAgent::bleScannerAgent( QObject *parent ) :
    _Mybase( parent ),
    errors_( 0 ),
    active_( false ),
    activeEmitted_( false ),
    scanTimer_( NULL ),
    scan_( NULL ),
    localDevice_( NULL )
{
    // create local device
    localDevice_ = new QBluetoothLocalDevice( this );

    connect( localDevice_, SIGNAL(error(QBluetoothLocalDevice::Error)), SLOT(onLocalDeviceError(QBluetoothLocalDevice::Error)) );
    connect( localDevice_, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)), SLOT(onLocalDeviceHostModeStateChanged(QBluetoothLocalDevice::HostMode)) );

    const QBluetoothAddress address( localDevice_->address() );

    qDebug() << "local device address" << address.toString();
    qDebug() << "local device host mode" << localDevice_->hostMode();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bleScannerAgent::~bleScannerAgent()
{
    // destroy scanner
    if ( scanTimer_ )
        delete scanTimer_;

    if ( scan_ )
        delete scan_;

    // destroy local device
    if ( localDevice_ )
        delete localDevice_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::reset()
{
    if ( isScanningActive() )
        stopScanning();

    // power cycle device
    localDevice_->setHostMode( QBluetoothLocalDevice::HostPoweredOff );
    localDevice_->setHostMode( QBluetoothLocalDevice::HostDiscoverableLimitedInquiry );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::startScanning( scanType type )
{
    Q_UNUSED( type )

    // nothing to do
    if ( active_ )
        return;

    qDebug() << "=== START SCAN ===";

    // we are now active
    active_ = true;
    activeEmitted_ = false;

    // power on local device
    if ( QBluetoothLocalDevice::HostPoweredOff == localDevice_->hostMode() )
        localDevice_->setHostMode( QBluetoothLocalDevice::HostDiscoverableLimitedInquiry );
    else
    {
        // start scanning!
        createScanner();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::stopScanning()
{
    // nothing to do
    if ( !active_ )
        return;

    qDebug() << "=== STOP SCAN ===";

    active_ = false;
    destroyScanner();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::createScanner()
{
    // nothing to do
    if ( scan_ )
        return;

    // create scanner
    scan_ = new QBluetoothDeviceDiscoveryAgent( this );

    connect( scan_, SIGNAL(canceled()), SLOT(onScanCanceled()) );
    connect( scan_, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), SLOT(onScanDeviceDiscovered(QBluetoothDeviceInfo)) );
    connect( scan_, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), SLOT(onScanError(QBluetoothDeviceDiscoveryAgent::Error)) );
    connect( scan_, SIGNAL(finished()), SLOT(onScanFinished()) );

    // scan!
    scan_->setInquiryType( QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry );
    scan_->start();

    // create timer
    scanTimer_ = new QTimer( this );

    connect( scanTimer_, SIGNAL(timeout()), scan_, SLOT(start()) );

    // validate we are scanning
    if (( !activeEmitted_ ) && ( scan_->isActive() ))
    {
        // emit signal
        qDebug() << "scan started!";
        emit scanningStarted();

        activeEmitted_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::destroyScanner()
{
    // destroy scan timer
    if ( scanTimer_ )
    {
        scanTimer_->blockSignals( true );
        scanTimer_->deleteLater();

        scanTimer_ = NULL;
    }

    // destroy scanner
    if ( scan_ )
    {
        scan_->blockSignals( true );
        scan_->deleteLater();

        scan_ = NULL;
    }

    // not active anymore
    if ( !active_ )
    {
        // emit signal
        qDebug() << "scan stopped!";
        emit scanningStopped();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::onLocalDeviceError( QBluetoothLocalDevice::Error error )
{
    qDebug() << "local device error" << error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::onLocalDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode state )
{
    qDebug() << "local device host mode changed" << state;

    // check state
    if ( QBluetoothLocalDevice::HostPoweredOff == state )
    {
        // stop scanning
        destroyScanner();

        // try to power back on...
        if ( active_ )
            localDevice_->setHostMode( QBluetoothLocalDevice::HostDiscoverableLimitedInquiry );
    }
    else if ( active_ )
    {
        // start scanning!
        createScanner();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::onScanCanceled()
{
    qDebug() << "scan canceled";

    active_ = false;
    destroyScanner();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::onScanDeviceDiscovered( const QBluetoothDeviceInfo& info )
{
    if ( !info.isValid() )
        return;

    // reset error count (scan found device)
    errors_ = 0;

//    qDebug() << info.address() << info.name() << info.rssi() << info.coreConfigurations();

    const QString name( info.name() );

    // check for hobart tag
    if ( info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration )
    {
        // application
        if ( "ITWH" == name.left( 4 ) )
        {
            const QBluetoothAddress addr( info.address() );

            qDebug() << "FOUND TAG" << addr.toString() << info.rssi();

            // generate device info
            scannerAgentDeviceInfo deviceInfo;
            deviceInfo.info = info;
            deviceInfo.type = 0;

            deviceInfo.transmitPowerLevel = 0;
            deviceInfo.firmwareVersion = 0;
            deviceInfo.batteryLevel = 0;
            deviceInfo.messages = 0;

            // emit signal
            emit deviceDiscovered( deviceInfo );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::onScanError( QBluetoothDeviceDiscoveryAgent::Error error )
{
    qWarning() << "scan error" << error << scan_->errorString();

    // no error
    if ( QBluetoothDeviceDiscoveryAgent::NoError == error )
    {
        errors_ = 0;
    }

    // for these errors try to power cycle local device
    else if (( QBluetoothDeviceDiscoveryAgent::InputOutputError == error ) || ( QBluetoothDeviceDiscoveryAgent::UnknownError == error ))
    {
        if ( ++errors_ <= 3 )
            localDevice_->setHostMode( QBluetoothLocalDevice::HostPoweredOff );
        else
        {
            qDebug() << "too many errors stopping scan";

            active_ = false;
            destroyScanner();
        }
    }

    // any other error disable
    else
    {
        active_ = false;
        destroyScanner();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleScannerAgent::onScanFinished()
{
    qDebug() << "scan finished";

    // reset error count (scan finished successfully)
    errors_ = 0;

    // queue another scan
    if ( active_ )
    {
        scanTimer_->setSingleShot( true );
        scanTimer_->start( SCAN_TIMEOUT );
    }
}
