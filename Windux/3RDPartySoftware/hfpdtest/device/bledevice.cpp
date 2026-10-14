/**
 * @file bledevice.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "bledevice.h"

#include <QBluetoothDeviceInfo>
#include <QDebug>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////
bleDevice::bleDevice( const QBluetoothDeviceInfo &info, QObject *parent ) :
    _Mybase( parent ),
    controller_( NULL ),
    created_( QDateTime::currentDateTime() ),
    rssi_( info.rssi() )
{
    // create controller
    controller_ = new QLowEnergyController( info, this );
    controller_->setRemoteAddressType( QLowEnergyController::RandomAddress );

    connect( controller_, SIGNAL(error(QLowEnergyController::Error)), SLOT(onControllerError(QLowEnergyController::Error)) );
    connect( controller_, SIGNAL(stateChanged(QLowEnergyController::ControllerState)), SLOT(onControllerStateChanged(QLowEnergyController::ControllerState)) );

    // create reconnect timer
    reconnectTimer_ = new QTimer( this );
    reconnectTimer_->setSingleShot( true );

    connect( reconnectTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // create process comm timer
    processCommTimer_ = new QTimer( this );
    processCommTimer_->setSingleShot( true );

    connect( processCommTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // create timeout timer
    timeoutTimer_ = new QTimer( this );
    timeoutTimer_->setSingleShot( true );

    connect( timeoutTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // create uuid timer
    uuidTimer_ = new QTimer( this );
    uuidTimer_->setSingleShot( true );

    connect( uuidTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    uuidTimer_->start( 0 );

    // ---- //

    const QBluetoothAddress mac( address() );

    qInfo() << address() << "CTOR";
    setObjectName( mac.toString() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bleDevice::~bleDevice()
{
    qInfo() << address() << "DTOR";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QBluetoothAddress bleDevice::address() const
{
    return controller_->remoteAddress();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString bleDevice::characteristicName( const QBluetoothUuid &uuid ) const
{
    Q_UNUSED( uuid )
    return "(Unknown)";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bleDevice::isNotifyEnabled( const QBluetoothUuid &uuid ) const
{
    Q_UNUSED( uuid )
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bleDevice::isReadOnly( const QBluetoothUuid &uuid ) const
{
    Q_UNUSED( uuid )
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bleDevice::isWriteOnly( const QBluetoothUuid &uuid ) const
{
    Q_UNUSED( uuid )
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::setValue( const QBluetoothUuid &uuid, const QByteArray &value )
{
    writeCharacteristic( uuid, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray bleDevice::value( const QBluetoothUuid &uuid ) const
{
    // return value we read from device (if we read it...)
    if ( values_.contains( uuid ) )
        return values_[uuid];

    return QByteArray();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::readAllCharacteristics()
{
    const uuidList ids( uuid_.keys() );

    foreach ( const QBluetoothUuid &id, ids )
        if ( !isWriteOnly( id ) )
            comm_.push_back( commOperation( id ) );

    // connect
    connectToDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::readCharacteristic( const QBluetoothUuid &uuid )
{
    if ( isWriteOnly( uuid ) )
        return;

    comm_.push_back( commOperation( uuid ) );
    connectToDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::writeCharacteristic( const QBluetoothUuid &uuid, const QByteArray &value )
{
    if ( isReadOnly( uuid ) )
        return;

    comm_.push_back( commOperation( uuid, value ) );
    connectToDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::onControllerError( QLowEnergyController::Error newError )
{
    qWarning() << address() << "controller error" << newError << controller_->errorString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::onControllerStateChanged( QLowEnergyController::ControllerState state )
{
    qInfo() << address() << "controller state changed to" << state;

    bool invalidateServices( false );

    // process state
    switch ( state )
    {
    case QLowEnergyController::UnconnectedState:

        // more work to do, reconnect
        if ( !comm_.empty() )
            reconnectTimer_->start( 5000 );

        invalidateServices = true;
        break;

    case QLowEnergyController::ConnectingState:
        timeoutTimer_->start( CONNECT_TIMEOUT );
        break;

    case QLowEnergyController::ConnectedState:
        timeoutTimer_->start( DISCOVER_SERVICES_TIMEOUT );
        controller_->discoverServices();
        break;

    case QLowEnergyController::DiscoveringState:
        break;

    case QLowEnergyController::DiscoveredState:
        processServices( controller_->services() );
        break;

    case QLowEnergyController::ClosingState:
        invalidateServices = true;
        break;

    default:
        break;
    }

    // invalidate services
    if ( invalidateServices )
    {
        // delete all services
        QList<QLowEnergyService*> svcs( findChildren<QLowEnergyService*>() );

        foreach ( QLowEnergyService *s, svcs )
            s->deleteLater();

        // clear discovered characteristics
        characteristic_.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::onServiceCharacteristicUpdated( const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue )
{
    const QBluetoothUuid uuid( characteristic.uuid() );

    qInfo() << address() << uuid.toString() << newValue.toHex();

    // check comms
    if ( !comm_.empty() )
    {
        const commOperation op( comm_.first() );

        // schedule next comm operation
        if ( op.id_ == uuid )
        {
            comm_.pop_front();
            processCommTimer_->start( 0 );
        }
    }

    // update local value
    values_[uuid] = newValue;

    // emit signal
    emit valueChanged( uuid, newValue );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::onServiceError( QLowEnergyService::ServiceError newError )
{
    qWarning() << address() << "service error" << newError;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::onServiceStateChanged( QLowEnergyService::ServiceState state )
{
    qInfo() << address() << "service state changed to" << state;

    // process discovered services
    if ( QLowEnergyService::ServiceDiscovered == state )
    {
        QLowEnergyService *s( qobject_cast<QLowEnergyService*>( sender() ) );

        if ( s )
        {
            processCharacteristics( s->characteristics() );
            processServices( s->includedServices() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::onTimeout()
{
    // connect to device
    if ( reconnectTimer_ == sender() )
    {
        qInfo() << address() << "connect to device";
        controller_->connectToDevice();
    }

    // process comm
    else if ( processCommTimer_ == sender() )
    {
        processComm();
    }

    // timeout
    else if ( timeoutTimer_ == sender() )
    {
        qWarning() << address() << "device timed out";
        controller_->disconnectFromDevice();
    }

    // uuid mapper
    else if ( uuidTimer_ == sender() )
    {
        processUuid();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::connectToDevice()
{
    // connect controller to device
    if ( QLowEnergyController::DiscoveredState != controller_->state() )
        controller_->connectToDevice();

    // ensure all characteristics discovered
    else if ( uuid_.size() == characteristic_.size() )
    {
        // no comm already pending
        if ( !timeoutTimer_->isActive() )
            processCommTimer_->start( 0 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::processUuid()
{
    // nothing to do
    if ( !uuid_.isEmpty() )
        return;

    qInfo() << address() << "process uuid";

    // map all characteristics to their service
    const uuidList ids( services() );

    foreach ( const QBluetoothUuid &id, ids )
    {
        const uuidList chs( characteristics( id ) );

        foreach ( const QBluetoothUuid &ch, chs )
            uuid_[ch] = id;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::processCharacteristics( const characteristicList &chs )
{
    // enumerate characteristics
    foreach ( const QLowEnergyCharacteristic &ch, chs )
    {
        const QBluetoothUuid uuid( ch.uuid() );

        qInfo() << address() << "new characteristic" << uuid.toString() << ch.name() << ch.properties();

        // check discovered characteristic in our uuid list
        const uuidList ids( uuid_.keys() );

        if ( ids.contains( uuid ) )
            characteristic_[uuid] = ch;
    }

    // check if all characteristics discovered
    if ( uuid_.size() == characteristic_.size() )
    {
        // connect services
        QList<QLowEnergyService*> svcs( findChildren<QLowEnergyService*>() );

        foreach ( QLowEnergyService *s, svcs )
        {
            connect( s, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)), SLOT(onServiceCharacteristicUpdated(QLowEnergyCharacteristic,QByteArray)) );
            connect( s, SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)), SLOT(onServiceCharacteristicUpdated(QLowEnergyCharacteristic,QByteArray)) );
            connect( s, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)), SLOT(onServiceCharacteristicUpdated(QLowEnergyCharacteristic,QByteArray)) );

            // enable notification
            const uuidList uuids( characteristics( QBluetoothUuid( s->objectName() ) ) );

            foreach ( const QBluetoothUuid &uuid, uuids )
                if (( isNotifyEnabled( uuid ) ) && ( characteristic_.contains( uuid ) ))
                {
                    const QLowEnergyCharacteristic ch( characteristic_[uuid] );

                    const QList<QLowEnergyDescriptor> descs( ch.descriptors() );

                    foreach ( const QLowEnergyDescriptor &desc, descs )
                        if ( QBluetoothUuid::ClientCharacteristicConfiguration == desc.type() )
                        {
                            qInfo() << address() << "enable notify" << uuid.toString();

                            s->writeDescriptor( desc, QByteArray::fromHex( "0100" ) );
                            break;
                        }
                }

        }

        // process comm
        qInfo() << address() << "READY";
        processCommTimer_->start( 0 );

        // emit write only characteristics
        const uuidList ids( uuid_.keys() );

        foreach( const QBluetoothUuid &id, ids )
            if ( isWriteOnly( id ) )
            {
                qInfo() << address() << "write only" << id.toString();
                emit valueChanged( id, QByteArray() );
            }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::processServices( const uuidList &ids )
{
    // enumerate services
    foreach ( const QBluetoothUuid &id, ids )
    {
        qInfo() << address() << "new service discovered" << id.toString();

        // check discovered service in our uuid list
        const uuidList svcs( services() );

        if ( svcs.contains( id ) )
        {
            // locate existing service
            QLowEnergyService *s( findChild<QLowEnergyService*>( id.toString() ) );

            // did not find existing... create new service object...
            if ( !s )
            {
                if ( (s = controller_->createServiceObject( id, this )) )
                    s->setObjectName( id.toString() );
            }

            if ( !s )
                qWarning() << address() << "failed to create service";
            else if ( QLowEnergyService::ServiceDiscovered == s->state() )
                processServices( s->includedServices() );
            else if ( QLowEnergyService::DiscoveryRequired == s->state() )
            {
                connect( s, SIGNAL(error(QLowEnergyService::ServiceError)), SLOT(onServiceError(QLowEnergyService::ServiceError)) );
                connect( s, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), SLOT(onServiceStateChanged(QLowEnergyService::ServiceState)) );

                timeoutTimer_->start( DISCOVER_SERVICES_TIMEOUT );
                s->discoverDetails();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bleDevice::processComm()
{
    // nothing more to do
    if ( comm_.empty() )
    {
        qInfo() << address() << "process comm complete";

        // stay connected
        //controller_->disconnectFromDevice();

        timeoutTimer_->stop();

        return;
    }

    // perform next operation
    const commOperation op( comm_.first() );

    if (( !uuid_.contains( op.id_ ) ) || ( !characteristic_.contains( op.id_ ) ))
    {
        qWarning() << address() << "unknown characteristic" << op.id_;

        comm_.pop_front();
        processCommTimer_->start( 0 );

        return;
    }

    // locate existing service
    QLowEnergyService *s( findChild<QLowEnergyService*>( uuid_[op.id_].toString() ) );

    if ( !s )
    {
        qWarning() << address() << "failed to locate service" << op.id_;

        comm_.pop_front();
        processCommTimer_->start( 0 );

        return;
    }

    const QLowEnergyCharacteristic ch( characteristic_[op.id_] );

    // read
    if ( op.value_.isEmpty() )
    {
        s->readCharacteristic( ch );

        timeoutTimer_->start( READ_TIMEOUT );
    }

    // write only
    else if ( isWriteOnly( op.id_ ) )
    {
        qInfo() << address() << "write characteristic (wo)" << op.id_.toString() << op.value_.toHex();
        s->writeCharacteristic( ch, op.value_, QLowEnergyService::WriteWithoutResponse );

        comm_.pop_front();
        processCommTimer_->start( 0 );
    }

    // write
    else
    {
        qInfo() << address() << "write characteristic" << op.id_.toString() << op.value_.toHex();
        s->writeCharacteristic( ch, op.value_ );

        timeoutTimer_->start( WRITE_TIMEOUT );
    }
}

