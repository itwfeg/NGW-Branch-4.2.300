/**
 * @file commclient.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "commclient.h"
#include "hobarttag.h"

#include <deviceclearallmessages.h>
#include <devicecmdresult.h>
#include <deviceconfig.h>
#include <deviceconnect.h>
#include <devicedisconnect.h>
#include <deviceerror.h>
#include <devicefirmwareprogress.h>
#include <devicefirmwarerequest.h>
#include <deviceflashfirmwareota.h>
#include <devicelist.h>
#include <devicelistreq.h>
#include <deviceopnamelist.h>
#include <devicereadallconfig.h>
#include <devicereadconfig.h>
#include <devicesendmessage.h>
#include <devicewriteconfig.h>
#include <devicestate.h>
#include <messagefactory.h>
#include <ping.h>
#include <pong.h>

#include <QTimer>

#include <arpa/inet.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
commClient::commClient( QTcpSocket *sock, QObject *parent ) :
    _Mybase( parent ),
    tagManager_( hobartTagManager::instance() ),
    upgradeManager_( otaUpgradeManager::instance() ),
    sock_( sock ),
    keepAlive_( new QTimer( this ) ),
    timeout_( 0 )
{
    sock_->setParent( this );

    // socket signals
    connect( sock_, SIGNAL(disconnected()), SLOT(onDisconnected()) );
    connect( sock_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(onError(QAbstractSocket::SocketError)) );
    connect( sock_, SIGNAL(readyRead()), SLOT(onReadyRead()) );
    connect( sock_, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(onStateChanged(QAbstractSocket::SocketState)) );

    // upgrade manager
    connect( upgradeManager_, SIGNAL(firmwareRequested(QBluetoothAddress)), SLOT(onFirmwareRequested(QBluetoothAddress)) );
    connect( upgradeManager_, SIGNAL(updateProgress(QBluetoothAddress,bool,bool,qreal)), SLOT(onUpdateProgress(QBluetoothAddress,bool,bool,qreal)) );

    // setup keep alive
    keepAlive_->start( KEEP_ALIVE_TIMEOUT );

    connect( keepAlive_, SIGNAL(timeout()), SLOT(onTimeout()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
commClient::~commClient()
{
    // disconnect active connections
    foreach ( const hobartTagManager::device_ptr_t& dev, connections_ )
        dev->disconnectFromDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::close()
{
    sock_->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onDeviceConfigValueChanged( int conf, const QByteArray& value )
{
    device_t *tag( qobject_cast<device_t*>( sender() ) );

    if ( !tag )
        return;

    const QBluetoothAddress tagAddress( tag->address() );
    const QString tagAddressStr( tagAddress.toString() );

    qDebug() << this << "config value changed" << tagAddress << conf << value;

    // send response
    deviceConfig response( tagAddressStr.toStdString(), conf, value.toStdString(), 0 );
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onDeviceConnected()
{
    device_t *tag( qobject_cast<device_t*>( sender() ) );

    if ( !tag )
        return;

    const QBluetoothAddress tagAddress( tag->address() );
    const QString tagAddressStr( tagAddress.toString() );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    if ( !ptr )
    {
        ptr = findDeviceByAddress( tagManager_->devices(), tagAddress );

        if ( !ptr )
            qWarning() << this << "could not locate connected device" << tagAddressStr;
        else
        {
            // add device to connection list
            connections_.append( ptr );

            qInfo() << this << "active connections" << connections_.size();
        }
    }

    // send response
    deviceState response( tagAddressStr.toStdString(), deviceState::STATE_CONNECTED );
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onDeviceConnecting()
{
    device_t *tag( qobject_cast<device_t*>( sender() ) );

    if ( !tag )
        return;

    const QBluetoothAddress tagAddress( tag->address() );
    const QString tagAddressStr( tagAddress.toString() );

    // send response
    deviceState response( tagAddressStr.toStdString(), deviceState::STATE_CONNECTING );
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onDeviceDisconnected()
{
    device_t *tag( qobject_cast<device_t*>( sender() ) );

    if ( !tag )
        return;

    const QBluetoothAddress tagAddress( tag->address() );
    const QString tagAddressStr( tagAddress.toString() );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    if ( !ptr )
        qWarning() << this << "could not locate disconnected device" << tagAddressStr;
    else
    {
        // disconnect signals
        disconnect( tag, 0, this, 0 );

        // remove device from connections list
        connections_.removeOne( ptr );

        qInfo() << this << "active connections" << connections_.size();
    }

    // send response
    deviceState response( tagAddressStr.toStdString(), deviceState::STATE_DISCONNECTED );
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onDeviceError( int num, const QString& description )
{
    device_t *tag( qobject_cast<device_t*>( sender() ) );

    if ( !tag )
        return;

    const QBluetoothAddress tagAddress( tag->address() );
    const QString tagAddressStr( tagAddress.toString() );

    // send response
    deviceError response( tagAddressStr.toStdString(), num, description.toStdString() );
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onDeviceReady()
{
    device_t *tag( qobject_cast<device_t*>( sender() ) );

    if ( !tag )
        return;

    const QBluetoothAddress tagAddress( tag->address() );
    const QString tagAddressStr( tagAddress.toString() );

    // send response
    deviceState response( tagAddressStr.toStdString(), deviceState::STATE_READY );
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onDisconnected()
{
    qInfo() << this << "socket disconnect";
    deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onError( QAbstractSocket::SocketError socketError )
{
    qWarning() << this << "socket error" << socketError << sock_->errorString();
    deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onFirmwareRequested( const QBluetoothAddress& address )
{
    const QString tagAddressStr( address.toString() );

    // send message
    deviceFirmwareRequest response( tagAddressStr.toStdString() );
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onReadyRead()
{
    a_.append( sock_->readAll() );

    for ( ;; )
    {
        size_t len( 0 );

        const message *msg( messageFactory::create( a_.constData(), a_.length(), &len ) );

        // nothing processed
        if ( !len )
            break;

        // process!
        else if ( msg )
        {
            recvMessage( msg );
            delete msg;
        }

        if ( a_.length() == (int) len )
        {
            a_.clear();
            break;
        }

        a_ = a_.right( a_.length() - len );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onStateChanged( QAbstractSocket::SocketState socketState )
{
    qWarning() << this << "socket state changed to" << socketState;

    if ( QAbstractSocket::UnconnectedState == socketState )
        deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onTimeout()
{
    if ( keepAlive_ == sender() )
    {
        // socket timeout
        if ( 5 < timeout_ )
        {
            qWarning() << this << "socket timeout";
            deleteLater();
        }

        // ping remote socket
        else
        {
            const ping msg;
            sendMessage( &msg );

            ++timeout_;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::onUpdateProgress( const QBluetoothAddress& address, bool start, bool finish, qreal progress )
{
    // only send update to who requested it
    if ( !pendingFirmware_.contains( address ) )
        return;

    if ( finish )
        pendingFirmware_.removeAll( address );

    const QString tagAddressStr( address.toString() );

    qDebug() << "update progress" << start << finish << progress;

    // send message
    deviceFirmwareProgress response( tagAddressStr.toStdString(), start, finish, (unsigned int)(progress * 100.0) );
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void commClient::handleDeviceMessage<deviceClearAllMessages>( const deviceClearAllMessages *request )
{
    deviceCommandResult response( request->address_, request->type_ );

    const QBluetoothAddress tagAddress( QString::fromStdString( request->address_ ) );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    // clear all messages
    if ( ptr )
    {
        qDebug() << this << "clear all messages" << tagAddress;
        ptr->clearMessages();
    }
    else
    {
        ptr = findDeviceByAddress( tagManager_->devices(), tagAddress );

        if ( !ptr )
            response.result_ = message::HFPD_ERROR_UNKNOWN_DEVICE;
        else
            response.result_ = message::HFPD_ERROR_NOT_CONNECTED;
    }

    // send response
    sendMessage( &response );
}

template <>
void commClient::handleDeviceMessage<deviceConnect>( const deviceConnect *request )
{
    deviceCommandResult response( request->address_, request->type_ );

    const QBluetoothAddress tagAddress( QString::fromStdString( request->address_ ) );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    if ( !ptr )
    {
        ptr = findDeviceByAddress( tagManager_->devices(), tagAddress );

        if ( !ptr )
            response.result_ = message::HFPD_ERROR_UNKNOWN_DEVICE;
        else
        {
            device_t *tag( ptr.data() );

            // connect signals
            connect( tag, SIGNAL(configValueChanged(int,QByteArray)), SLOT(onDeviceConfigValueChanged(int,QByteArray)) );
            connect( tag, SIGNAL(connected()), SLOT(onDeviceConnected()) );
            connect( tag, SIGNAL(connecting()), SLOT(onDeviceConnecting()) );
            connect( tag, SIGNAL(disconnected()), SLOT(onDeviceDisconnected()) );
            connect( tag, SIGNAL(error(int,QString)), SLOT(onDeviceError(int,QString)) );
            connect( tag, SIGNAL(ready()), SLOT(onDeviceReady()) );

            // connect
            qDebug() << this << "connect to device" << tagAddress;
            ptr->connectToDevice();
        }
    }

    // send response
    sendMessage( &response );
}

template <>
void commClient::handleDeviceMessage<deviceDisconnect>( const deviceDisconnect *request )
{
    deviceCommandResult response( request->address_, request->type_ );

    const QBluetoothAddress tagAddress( QString::fromStdString( request->address_ ) );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    // disconnect
    if ( ptr )
    {
        qDebug() << this << "disconnect from device" << tagAddress;
        ptr->disconnectFromDevice();
    }
    else
    {
        ptr = findDeviceByAddress( tagManager_->devices(), tagAddress );

        if ( !ptr )
            response.result_ = message::HFPD_ERROR_UNKNOWN_DEVICE;
        else
            response.result_ = message::HFPD_ERROR_NOT_CONNECTED;
    }

    // send response
    sendMessage( &response );
}

template <>
void commClient::handleDeviceMessage<deviceFlashFirmwareOTA>( const deviceFlashFirmwareOTA *request )
{
    deviceCommandResult response( request->address_, request->type_ );

    const QBluetoothAddress tagAddress( QString::fromStdString( request->address_ ) );

    const unsigned int otaIdent( request->id_ );
    const QString filename( QString::fromStdString( request->filename_ ) );

    qInfo() << this << "ota firmware upgrade initiated" << tagAddress << otaIdent << filename;

    // prepare uprade
    if ( !upgradeManager_->prepareUpgrade( tagAddress, filename, otaIdent ) )
    {
        qWarning() << this << "bad firmware!" << filename;
        response.result_ = message::HFPD_ERROR_INVALID_FIRMWARE;
    }
    else
    {
        qDebug() << this << "valid firmware" << filename;

        // locate device
        device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

        // enter OTA mode
        if ( ptr )
            ptr->otaMode();

        // device pending
        pendingFirmware_.append( tagAddress );
    }

    // send response
    sendMessage( &response );
}

template <>
void commClient::handleDeviceMessage<deviceListRequest>( const deviceListRequest *request )
{
    const QDateTime now( QDateTime::currentDateTime() );

    deviceList response;

    // determine power level
    hobartTag::powerLevel txpow( hobartTag::POWER_LEVEL_HIGH );

    if ( deviceListRequest::HFPD_LOW_POWER_ADVERT == request->strength_ )
        txpow = hobartTag::POWER_LEVEL_LOW;

    response.strength_ = request->strength_;

    // process devices
    QList<hobartTagManager::device_ptr_t> devices( tagManager_->devices() );

    qDebug() << this << "processing" << devices.size() << "devices...";

    foreach ( const hobartTagManager::device_ptr_t& dev, devices )
    {
        QDateTime lastAdvert( dev->lastAdvert( txpow ) );

        if ( lastAdvert.isValid() )
            if (( !request->age_ ) || ( lastAdvert.secsTo( now ) <= request->age_ ))
            {
                const QString address( dev->address().toString() );
                const QString operatorName( dev->operatorName() );

                response.address_.push_back( address.toStdString() );
                response.rssi_.push_back( dev->rssi( txpow ) );

                response.batteryLevel_.push_back( dev->batteryLevel() );
                response.firmwareVersion_.push_back( dev->firmwareVersion() );
                response.messages_.push_back( dev->messages() );

                response.operatorName_.push_back( operatorName.toStdString() );

                qDebug() << this << address << response.rssi_.back() << operatorName;
            }
    }

    // send response
    sendMessage( &response );
}

template <>
void commClient::handleDeviceMessage<deviceOpNameList>( const deviceOpNameList *request )
{
    const size_t opers( std::min( request->address_.size(), request->opName_.size() ) );

    // generate mapping of operator names
    hobartTagManager::operator_name_map_t operNames;

    for ( size_t i( opers ); i--; )
    {
        const QBluetoothAddress tagAddress( QString::fromStdString( request->address_[i] ) );

        operNames[tagAddress] = QString::fromStdString( request->opName_[i] );
    }

    // set operator names
    tagManager_->setOperatorNames( operNames );
}

template <>
void commClient::handleDeviceMessage<deviceReadAllConfig>( const deviceReadAllConfig *request )
{
    deviceCommandResult response( request->address_, request->type_ );

    const QBluetoothAddress tagAddress( QString::fromStdString( request->address_ ) );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    // read all config
    if ( ptr )
    {
        qDebug() << this << "read all config" << tagAddress;
        ptr->readAllConfig();
    }
    else
    {
        ptr = findDeviceByAddress( tagManager_->devices(), tagAddress );

        if ( !ptr )
            response.result_ = message::HFPD_ERROR_UNKNOWN_DEVICE;
        else
            response.result_ = message::HFPD_ERROR_NOT_CONNECTED;
    }

    // send response
    sendMessage( &response );
}

template <>
void commClient::handleDeviceMessage<deviceReadConfig>( const deviceReadConfig *request )
{
    deviceCommandResult response( request->address_, request->type_ );

    const QBluetoothAddress tagAddress( QString::fromStdString( request->address_ ) );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    // read config
    if ( ptr )
    {
        QByteArray value;

        if ( ptr->readConfig( request->id_, value, request->useCachedValue_ ? true : false ) )
        {
            // send config
            deviceConfig msg( request->address_, request->id_, value.toStdString(), request->useCachedValue_ );
            sendMessage( &msg );
        }
        else if ( request->useCachedValue_ )
            response.result_ = message::HFPD_ERROR_CONFIG_UNKNOWN;
        else
            response.result_ = message::HFPD_ERROR_CONFIG_REQUESTED;
    }
    else
    {
        ptr = findDeviceByAddress( tagManager_->devices(), tagAddress );

        if ( !ptr )
            response.result_ = message::HFPD_ERROR_UNKNOWN_DEVICE;
        else
            response.result_ = message::HFPD_ERROR_NOT_CONNECTED;
    }

    // send response
    sendMessage( &response );
}

template <>
void commClient::handleDeviceMessage<deviceSendMessage>( const deviceSendMessage *request )
{
    deviceCommandResult response( request->address_, request->type_ );

    const QBluetoothAddress tagAddress( QString::fromStdString( request->address_ ) );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    // disconnect
    if ( ptr )
    {
        QTime time;
        time.setHMS( request->hour_, request->min_, request->sec_ );

        qDebug() << this << "send message" << tagAddress;
        ptr->sendMessage( request->id_, time, QByteArray::fromStdString( request->message_ ) );
    }
    else
    {
        ptr = findDeviceByAddress( tagManager_->devices(), tagAddress );

        if ( !ptr )
            response.result_ = message::HFPD_ERROR_UNKNOWN_DEVICE;
        else
            response.result_ = message::HFPD_ERROR_NOT_CONNECTED;
    }

    // send response
    sendMessage( &response );
}

template <>
void commClient::handleDeviceMessage<deviceWriteConfig>( const deviceWriteConfig *request )
{
    deviceCommandResult response( request->address_, request->type_ );

    const QBluetoothAddress tagAddress( QString::fromStdString( request->address_ ) );

    // locate device
    device_ptr_t ptr( findDeviceByAddress( connections_, tagAddress ) );

    // read config
    if ( ptr )
    {
        const QByteArray value( QByteArray::fromStdString( request->value_ ) );

        if ( ptr->writeConfig( request->id_, value ) )
            response.result_ = message::HFPD_ERROR_CONFIG_REQUESTED;
        else
            response.result_ = message::HFPD_ERROR_CONFIG_UNKNOWN;
    }
    else
    {
        ptr = findDeviceByAddress( tagManager_->devices(), tagAddress );

        if ( !ptr )
            response.result_ = message::HFPD_ERROR_UNKNOWN_DEVICE;
        else
            response.result_ = message::HFPD_ERROR_NOT_CONNECTED;
    }

    // send response
    sendMessage( &response );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::recvMessage( const message *msg )
{
    bool handled( true );

    switch ( msg->type_ )
    {
    case message::HFPD_DEV_CLEAR_ALL_MESSAGES: {
        qInfo() << this << "handle device clear all messages...";

        // process request
        const deviceClearAllMessages *request( dynamic_cast<const deviceClearAllMessages*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_CONNECT: {
        qInfo() << this << "handle device connect...";

        // process request
        const deviceConnect *request( dynamic_cast<const deviceConnect*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_DISCONNECT: {
        qInfo() << this << "handle device disconnect...";

        // process request
        const deviceDisconnect *request( dynamic_cast<const deviceDisconnect*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_FLASH_FIRMWARE_OTA: {
        qInfo() << this << "handle device flash firmware ota...";

        // process request
        const deviceFlashFirmwareOTA *request( dynamic_cast<const deviceFlashFirmwareOTA*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_LIST_REQ: {
        qInfo() << this << "handle device list request...";

        // process request
        const deviceListRequest *request( dynamic_cast<const deviceListRequest*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_OPNAME_LIST: {
        qInfo() << this << "handle device operator names...";

        // process request
        const deviceOpNameList *request( dynamic_cast<const deviceOpNameList*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_READ_ALL_CONFIG: {
        qInfo() << this << "handle device read all config...";

        // process request
        const deviceReadAllConfig *request( dynamic_cast<const deviceReadAllConfig*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_READ_CONFIG: {
        qInfo() << this << "handle device read config...";

        // process request
        const deviceReadConfig *request( dynamic_cast<const deviceReadConfig*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_SEND_MESSAGE: {
        qInfo() << this << "handle device send message...";

        // process request
        const deviceSendMessage *request( dynamic_cast<const deviceSendMessage*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_DEV_WRITE_CONFIG: {
        qInfo() << this << "handle device write config...";

        // process request
        const deviceWriteConfig *request( dynamic_cast<const deviceWriteConfig*>( msg ) );

        if ( request )
            handleDeviceMessage( request );
        else
            handled = false;

        break;
    }

    case message::HFPD_PING: {
        qDebug() << this << "handle ping...";

        pong response;
        sendMessage( &response );

        break;
    }

    case message::HFPD_PONG: {
        qDebug() << this << "handle pong...";

        timeout_ = 0;
        break;
    }

    default:
        qWarning() << this << "unknown message type" << msg->type_ << msg->version_;

        handled = false;
        break;
    }

    // unhandled message
    if ( !handled )
    {
        deviceCommandResult response( msg->type_, message::HFPD_ERROR_BAD_REQUEST );
        sendMessage( &response );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void commClient::sendMessage( const message *msg ) const
{
    const std::string buffer( msg->serialize() );
    const uint32_t bufferLen( htonl( buffer.size() ) );

    QByteArray a;
    a.append( reinterpret_cast<const char*>( &bufferLen ), sizeof(bufferLen) );
    a.append( buffer.c_str(), buffer.size() );

    sock_->write( a );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
commClient::device_ptr_t commClient::findDeviceByAddress( const device_ptr_list_t& devices, const QBluetoothAddress& address )
{
    // locate device
    foreach ( const hobartTagManager::device_ptr_t& dev, devices )
        if ( address == dev->address() )
            return dev;

    return device_ptr_t( NULL );
}

