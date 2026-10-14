/**
 * @file dfudevice.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "dfudevice.h"

#include <QBluetoothDeviceInfo>

static const QBluetoothUuid nordic( QString( "{00001530-1212-efde-1523-785feabcd123}" ) );

static const QBluetoothUuid dfuPacket( QString( "{00001532-1212-efde-1523-785feabcd123}" ) );
static const QBluetoothUuid dfuControlPoint( QString( "{00001531-1212-efde-1523-785feabcd123}" ) );
static const QBluetoothUuid dfuVersion( QString( "{00001534-1212-efde-1523-785feabcd123}" ) );

///////////////////////////////////////////////////////////////////////////////////////////////////
dfuDevice::dfuDevice( const QBluetoothDeviceInfo& info, QObject *parent ) :
    _Mybase( info, parent )
{
    qDebug() << this << "CTOR";

    // setup uuid names
    names_[ dfuPacket ] = tr( "DFU Packet" );
    names_[ dfuControlPoint ] = tr( "DFU Control Point" );
    names_[ dfuVersion ] = tr( "DFU Version" );

    connect( this, SIGNAL(valueChanged(QBluetoothUuid,QByteArray)), SLOT(onValueChanged(QBluetoothUuid,QByteArray)) );

    qRegisterMetaType<errorCode>( "errorCode" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dfuDevice::~dfuDevice()
{
    qDebug() << this << "DTOR";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString dfuDevice::characteristicName( const QBluetoothUuid& uuid ) const
{
    if ( names_.contains( uuid ) )
        return names_[uuid];

    return _Mybase::characteristicName( uuid );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dfuDevice::isReadOnly( const QBluetoothUuid& uuid ) const
{
    return (dfuVersion == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dfuDevice::isWriteOnly( const QBluetoothUuid& uuid ) const
{
    return (dfuPacket == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::activate()
{
    sendControlData( ACTIVATE_FIRMWARE_AND_RESET );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::firmwareSize()
{
    sendControlData( REPORT_RECEIVED_IMAGE_SIZE );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::init( size_t appVersion, size_t crc )
{
    static const uint8_t INIT_START( 0 );
    static const uint8_t INIT_END( 1 );

    QList<uint16_t> softDevices;
    softDevices.append( 0xFFFE ); // development/any soft device

    sendControlData( INITIALIZE_DFU, toByteArray( INIT_START ) );
    sendPacketData( createInitPacket( 0xFFFF, 0xFFFF, appVersion, softDevices, crc ) );
    sendControlData( INITIALIZE_DFU, toByteArray( INIT_END ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::sendFirmware( const QByteArray& image, size_t idx )
{
    if ( !idx )
    {
        static const uint16_t PACKET_COUNT( 1 );

        // turn on packet notification
        sendControlData( REQ_PKT_RCPT_NOTIFICATION, toByteArray( PACKET_COUNT ) );

        // send firmware
        sendControlData( RECEIVE_FIRMWARE_IMAGE );
    }

    // send firmware packet
    imagePending_ = image;
    imagePendingIndex_ = idx;

    sendFirmwarePacket();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::start( size_t appSize )
{
    sendControlData( START_DFU, toByteArray( (uint8_t) APPLICATION ) );
    sendPacketData( createImageSizePacket( 0, 0, appSize ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::validate()
{
    sendControlData( VALIDATE_FIRMWARE_IMAGE );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dfuDevice::uuidList dfuDevice::characteristics( const QBluetoothUuid& uuid ) const
{
    if ( nordic == uuid )
        return names_.keys();

    return uuidList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dfuDevice::isNotifyEnabled( const QBluetoothUuid& uuid ) const
{
    return (dfuControlPoint == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dfuDevice::uuidList dfuDevice::services() const
{
    uuidList ids;
    ids.append( nordic );

    return ids;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::sendControlData( opcode op, const QByteArray& data )
{
    QByteArray a;
    a.append( toByteArray( (uint8_t) op ) );
    a.append( data );

    setValue( dfuControlPoint, a );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::sendPacketData( const QByteArray& data )
{
    setValue( dfuPacket, data );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray dfuDevice::createImageSizePacket( size_t softdeviceSize, size_t bootloaderSize, size_t appSize )
{
    QByteArray a;
    a.append( toByteArray( softdeviceSize ) );
    a.append( toByteArray( bootloaderSize ) );
    a.append( toByteArray( appSize ) );

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray dfuDevice::createInitPacket( uint16_t dt, uint16_t drev, uint32_t appVersion, const QList<uint16_t>& softDevices, uint16_t crc )
{
    QByteArray a;
    a.append( toByteArray( dt ) );
    a.append( toByteArray( drev ) );
    a.append( toByteArray( appVersion ) );
    a.append( toByteArray( (uint16_t) softDevices.size() ) );

    foreach ( const uint16_t& dev, softDevices )
        a.append( toByteArray( dev ) );

    a.append( toByteArray( crc ) );

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::onValueChanged( const QBluetoothUuid& uuid, const QByteArray& value )
{
    if (( dfuControlPoint == uuid ) && ( 1 <= value.size() ))
    {
        const opcode v( (opcode) value[0] );

        if (( RESPONSE == v ) && ( 3 <= value.size() ))
        {
            const opcode op( (opcode) value[1] );
            const errorCode result( (errorCode) value[2] );

            switch ( op )
            {
            case START_DFU:
                emit startResult( result );
                break;
            case INITIALIZE_DFU:
                emit initResult( result );
                break;
            case RECEIVE_FIRMWARE_IMAGE:
                emit sendFirmwareResult( result );
                break;
            case VALIDATE_FIRMWARE_IMAGE:
                emit validateResult( result );
                break;
            case REPORT_RECEIVED_IMAGE_SIZE: {
                size_t size( 0 );

                if ( 7 <= value.size() )
                {
                    size |= value[6] << 24;
                    size |= value[5] << 16;
                    size |= value[4] << 8;
                    size |= value[3];
                }

                emit firmwareSizeResult( result, size );
                break;
            }

            default:
                break;
            }
        }
        else if (( PKT_RCPT_NOTIF == v ) && ( 5 <= value.size() ))
        {
            size_t received( 0 );
            received |= value[4] << 24;
            received |= value[3] << 16;
            received |= value[2] << 8;
            received |= value[1];

            imagePendingIndex_ = received;
            sendFirmwarePacket();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dfuDevice::sendFirmwarePacket()
{
    // nothing to do
    if ( imagePending_.size() <= imagePendingIndex_ )
        return;

    // send packet
    sendPacketData( imagePending_.mid( imagePendingIndex_, FIRMARE_IMAGE_PACKET_SIZE ) );

    // report progress
    qreal percent( (qreal) imagePendingIndex_ );
    percent /= (qreal) imagePending_.size();

    emit downloadProgress( percent );
}
