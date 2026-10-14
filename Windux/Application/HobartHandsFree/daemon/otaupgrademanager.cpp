/**
 * @file otaupgrademanager.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "dfudevice.h"
#include "otaupgrademanager.h"
#include "scanneragent.h"

#include <nordicfirmware.h>

#include <QCoreApplication>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QDebug>
#include <QFile>
#include <QMutexLocker>
#include <QReadLocker>
#include <QThread>
#include <QTimer>
#include <QWriteLocker>

QMutex otaUpgradeManager::instanceLock_;
otaUpgradeManager *otaUpgradeManager::instance_( NULL );

///////////////////////////////////////////////////////////////////////////////////////////////////
otaUpgradeManager::otaUpgradeManager() :
    m_( QReadWriteLock::Recursive ),
    active_( 0 ),
    t_( new QTimer( this ) )
{
    // setup next state mapping
    nextState_[START] = INIT;
    nextState_[INIT] = UPDATE;
    nextState_[UPDATE] = VALIDATE;
    nextState_[UPDATE_RECOVERY] = UPDATE;
    nextState_[VALIDATE] = ACTIVATE;
    nextState_[ACTIVATE] = COMPLETE;

    // start timer
    t_->setSingleShot( false );
    t_->start( TIMEOUT_TIME );

    connect( t_, SIGNAL(timeout()), SLOT(onTimeout()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
otaUpgradeManager::~otaUpgradeManager()
{
    qDebug() << "clearing devices...";

    // clear device map
    devices_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
otaUpgradeManager::device_ptr_list_t otaUpgradeManager::devices() const
{
    QReadLocker guard( &m_ );
    return devices_.values();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool otaUpgradeManager::prepareUpgrade( const QBluetoothAddress& address, const QString& filename, unsigned int /*id*/ )
{
    std::string image;
    int imageVer;
    int crc;

    if ( !nordicDeviceFirmware::validate( filename.toStdString(), image, imageVer, crc ) )
    {
        qWarning() << address << "invalid firmware" << filename;
        return false;
    }

    QWriteLocker guard( &m_ );

    if ( deviceInfo_.contains( address ) )
    {
        qWarning() << address << "error firmware already scheduled for device";
        return false;
    }

    // read file
    QFile f( QString::fromStdString( image ) );

    if ( !f.open( QFile::ReadOnly ) )
    {
        qWarning() << address << "error opening firmware";
        return false;
    }

    // create update info
    firmwareUpdateInfo di;
    di.state = PENDING;
    di.processed = false;
    di.timeout = false;
    di.pending = QDateTime::currentDateTime();
    di.firmware = f.readAll();
    di.firmwareIndex = 0;
    di.version = imageVer;
    di.crc = crc;

    deviceInfo_[address] = di;

    // close file
    f.close();

    qInfo() << address << "PENDING firmware upgrade" << QString::fromStdString( image ) << di.firmware.size();
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
otaUpgradeManager *otaUpgradeManager::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceLock_ );

        if ( !instance_ )
            instance_ = new otaUpgradeManager();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::destroy()
{
    if ( instance_ )
    {
        QMutexLocker guard( &instanceLock_ );

        if ( instance_ )
        {
            delete instance_;
            instance_ = NULL;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::onDeviceConnecting()
{
    emit updateActiveConnections( ++active_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::onDeviceDisconnected()
{
    emit updateActiveConnections( --active_ );

    // ---- //

    device_t *device( qobject_cast<device_t*>( sender() ) );

    if ( !device )
        qWarning() << sender() << "error casting device";
    else
    {
        QWriteLocker guard( &m_ );

        const QBluetoothAddress address( device->address() );

        if ( !deviceInfo_.contains( address ) )
        {
            qWarning() << address << "no information regarding this device";
            return;
        }

        qWarning() << address << "DISCONNECTED";

        // ---- //

        firmwareUpdateInfo di( deviceInfo_[address] );

        // device has timed out so we are done
        if ( di.timeout )
            di.state = COMPLETE;

        // pick up where we left off instead of starting all over
        if ( UPDATE == di.state )
            di.state = UPDATE_RECOVERY;

        di.processed = false;

        deviceInfo_[address] = di;

        // reconnect if we are not done with this device
        if ( COMPLETE != di.state )
        {
            // reconnect!
            QTimer::singleShot( RECONNECT_TIMEOUT, device, SLOT(connectToDevice()) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::onDeviceDownloadProgress( qreal percent )
{
    device_t *device( qobject_cast<device_t*>( sender() ) );

    if ( !device )
        qWarning() << sender() << "error casting device";
    else
    {
        const QBluetoothAddress address( device->address() );

        // update progress
        emit updateProgress( address, false, false, percent );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::onDeviceFirmwareSizeResult( dfuDevice::errorCode value, size_t size )
{
    device_t *device( qobject_cast<device_t*>( sender() ) );

    if ( !device )
        qWarning() << sender() << "error casting device";
    else
    {
        QWriteLocker guard( &m_ );

        const QBluetoothAddress address( device->address() );

        if ( !deviceInfo_.contains( address ) )
        {
            qWarning() << address << "no information regarding this device";
            return;
        }

        qDebug() << address << "RESULT" << value << size;

        // ---- //

        firmwareUpdateInfo di( deviceInfo_[address] );

        if (( dfuDevice::SUCCESS != value ) || ( !size ))
        {
            qWarning() << address << "ERROR OCCURRED DURING UPDATE";

            // restart upgrade
            restart( device );
        }
        else
        {
            // update image index so we dont re-send information
            firmwareUpdateInfo di( deviceInfo_[address] );
            di.firmwareIndex = size;

            deviceInfo_[address] = di;

            // process next state
            processNextState( device );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::onDeviceReady()
{
    device_t *device( qobject_cast<device_t*>( sender() ) );

    if ( !device )
        qWarning() << sender() << "error casting device";
    else
    {
        QWriteLocker guard( &m_ );

        const QBluetoothAddress address( device->address() );

        if ( !deviceInfo_.contains( address ) )
        {
            qWarning() << address << "no information regarding this device";
            return;
        }

        qDebug() << address << "READY";

        // ---- //

        // process state
        processCurrentState( device );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::onDeviceResult( dfuDevice::errorCode value )
{
    device_t *device( qobject_cast<device_t*>( sender() ) );

    if ( !device )
        qWarning() << sender() << "error casting device";
    else
    {
        QWriteLocker guard( &m_ );

        const QBluetoothAddress address( device->address() );

        if ( !deviceInfo_.contains( address ) )
        {
            qWarning() << address << "no information regarding this device";
            return;
        }

        qDebug() << address << "RESULT" << value;

        // ---- //

        if ( dfuDevice::SUCCESS != value )
        {
            qWarning() << address << "ERROR OCCURRED DURING UPDATE";

            // restart upgrade
            restart( device );
        }
        else
        {
            // process next state
            processNextState( device );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::onTimeout()
{
    QWriteLocker guard( &m_ );

    const QDateTime now( QDateTime::currentDateTime() );

cull_devices:

    for ( firmwareUpdateInfoMap::iterator i( deviceInfo_.begin() ); i != deviceInfo_.end(); ++i )
    {
        const QBluetoothAddress address( i.key() );

        firmwareUpdateInfo di( i.value() );

        bool removeDevice( false );

        // check if device timeout and/or should be removed
        switch ( di.state )
        {
        case PENDING:
            if ( PENDING_TIMEOUT < di.pending.msecsTo( now ) )
            {
                qWarning() << address << "PENDING timeout";
                removeDevice = true;
            }

            break;

        case START:
        case INIT:
        case UPDATE:
        case UPDATE_RECOVERY:
        case VALIDATE:
        case ACTIVATE:
            if (( UPGRADE_TIMEOUT < di.start.msecsTo( now ) ) && ( !di.timeout ))
            {
                qWarning() << address << "UPGRADE timeout";
                removeDevice = true;

                // flag device as timed out so we can delete it after it disconnects (if connected)
                di.timeout = true;
                deviceInfo_[address] = di;

                // force disconnect
                devices_[address]->disconnectFromDevice();
            }

            break;

        case COMPLETE:
            removeDevice = true;
            break;

        default:
            qWarning() << address << "unknown device state" << di.state;
            break;
        }

        // remove device
        // only remove devices when no active connections
        if (( removeDevice ) && ( !active_ ))
        {
            emit updateProgress( address, false, true, 0.0 );

            deviceInfo_.remove( address );
            devices_.remove( address );

            qDebug() << "tracking" << deviceInfo_.size() << devices_.size() << "devices";

            goto cull_devices;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::updateDevice( const scannerAgentDeviceInfo& info )
{
    QWriteLocker guard( &m_ );

    const QBluetoothDeviceInfo deviceInfo( info.info );
    const QBluetoothAddress address( deviceInfo.address() );

    // we are not aware of this device... nothing to do
    if ( !deviceInfo_.contains( address ) )
    {
        emit firmwareRequested( address );
        return;
    }

    firmwareUpdateInfo di( deviceInfo_[address] );

    // pending update
    // create new device and connect
    if (( PENDING == di.state ) && ( !devices_.contains( address ) ))
    {
        // create device
        device_ptr_t device( new dfuDevice( deviceInfo, this ) );

        // device should exist on main thread
        device->moveToThread( QCoreApplication::instance()->thread() );

        // connect signals
        connect( device.data(), SIGNAL(connecting()), SLOT(onDeviceConnecting()) );
        connect( device.data(), SIGNAL(disconnected()), SLOT(onDeviceDisconnected()) );
        connect( device.data(), SIGNAL(ready()), SLOT(onDeviceReady()) );

        connect( device.data(), SIGNAL(downloadProgress(qreal)), SLOT(onDeviceDownloadProgress(qreal)) );
        connect( device.data(), SIGNAL(firmwareSizeResult(dfuDevice::errorCode,size_t)), SLOT(onDeviceFirmwareSizeResult(dfuDevice::errorCode,size_t)) );
        connect( device.data(), SIGNAL(initResult(dfuDevice::errorCode)), SLOT(onDeviceResult(dfuDevice::errorCode)) );
        connect( device.data(), SIGNAL(sendFirmwareResult(dfuDevice::errorCode)), SLOT(onDeviceResult(dfuDevice::errorCode)) );
        connect( device.data(), SIGNAL(startResult(dfuDevice::errorCode)), SLOT(onDeviceResult(dfuDevice::errorCode)) );
        connect( device.data(), SIGNAL(validateResult(dfuDevice::errorCode)), SLOT(onDeviceResult(dfuDevice::errorCode)) );

        // track device
        devices_[address] = device;

        // ---- //

        // firmware update in progress...
        di.state = START;
        di.start = QDateTime::currentDateTime();

        deviceInfo_[address] = di;

        // ---- //

        qInfo() << address << "connecting...";
        device->connectToDevice();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::restart( device_t *device )
{
    const QBluetoothAddress address( device->address() );

    firmwareUpdateInfo di( deviceInfo_[address] );

    // start over again
    di.state = START;
    di.processed = false;

    deviceInfo_[address] = di;

    // process state
    processCurrentState( device );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::processNextState( device_t *device )
{
    const QBluetoothAddress address( device->address() );

    firmwareUpdateInfo di( deviceInfo_[address] );

    // change states
    di.state = nextState_[di.state];
    di.processed = false;

    deviceInfo_[address] = di;

    // process state
    processCurrentState( device );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void otaUpgradeManager::processCurrentState( device_t *device )
{
    const QBluetoothAddress address( device->address() );

    firmwareUpdateInfo di( deviceInfo_[address] );

    // nothing to do, state already processed
    if ( di.processed )
    {
        // there is no response to the ACTIVATE message
        // after sending ACTIVATE we will recieve a READY message... so lets continue the next state in this situation
        if ( ACTIVATE == di.state )
            di.state = COMPLETE;
        else
        {
            return;
        }
    }

    di.processed = true;

    // handle state
    switch ( di.state )
    {
    case START:
        qDebug() << address << "DFU START";
        device->start( di.firmware.size() );

        emit updateProgress( address, true, false, 0.0 );

        break;

    case INIT:
        qDebug() << address << "DFU INIT";
        device->init( di.version, di.crc );
        break;

    case UPDATE:
        qDebug() << address << "DFU UPDATE";
        device->sendFirmware( di.firmware, di.firmwareIndex );
        break;

    case UPDATE_RECOVERY:
        qDebug() << address << "DFU UPDATE RECOVERY";
        device->firmwareSize();
        break;

    case VALIDATE:
        qDebug() << address << "DFU VALIDATE";
        device->validate();
        break;

    case ACTIVATE:
        qDebug() << address << "DFU ACTIVATE";
        device->activate();
        break;

    case COMPLETE:
        di.finish = QDateTime::currentDateTime();
        qDebug() << address << "DFU COMPLETE" << di.start.msecsTo( di.finish );
        break;

    default:
        qWarning() << address << "DFU UNHANDLED STATE";
        break;
    }

    deviceInfo_[address] = di;
}
