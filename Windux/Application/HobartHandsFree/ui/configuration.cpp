/**
 * @file configuration.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "commclient.h"
#include "configuration.h"

#include <deviceclearallmessages.h>
#include <devicecmdresult.h>
#include <deviceconfig.h>
#include <deviceconnect.h>
#include <devicedisconnect.h>
#include <deviceerror.h>
#include <deviceflashfirmwareota.h>
#include <devicereadallconfig.h>
#include <devicesendmessage.h>
#include <devicestate.h>
#include <devicewriteconfig.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
configurationWidget::configurationWidget( QWidget *parent ) :
    _Mybase( parent ),
    client_( commClient::instance() ),
    timeout_( new QTimer( this ) ),
    error_( false )
{
    // init
    initialize();
    createLayout();
    translate();

    // create timeout timer
    timeout_->setSingleShot( true );
    timeout_->setInterval( CONNECT_TIMEOUT );

    connect( timeout_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // connect client
    connect( client_, SIGNAL(recvMessage(const message*)), SLOT(onRecvMessage(const message*)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
configurationWidget::~configurationWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::connectToDevice( const QString& address )
{
    address_ = address;

    tagAddress_->setText( address_ );
    firmware_->clear();

    savePending_.clear();
    error_ = false;

    // update state
    updateConnectionState( Qt::white, Qt::black, tr( "Connection initiated..." ) );
    timeout_->start();

    // connect
    deviceConnect request( address_.toStdString() );
    client_->sendMessage( &request );

    state_ = STATE_CONNECTING;

    // clear configuration
    configuration_->setEnabled( true );
    configuration_->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::disconnectFromDevice()
{
    if ( STATE_DISCONNECTED == state_ )
        emit disconnected();
    else if ( STATE_DISCONNECTING != state_ )
    {
        // update state
        updateConnectionState( Qt::white, Qt::black, tr( "Disconnecting..." ) );
        timeout_->start();

        // disconnect
        deviceDisconnect request( address_.toStdString() );
        client_->sendMessage( &request );

        state_ = STATE_DISCONNECTING;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool configurationWidget::save()
{
    savePending_ = configuration_->pending();

    if ( savePending_.isEmpty() )
        return false;

    // send config updates
    for ( configMap::const_iterator i( savePending_.begin() ); i != savePending_.end(); ++i )
    {
        const QByteArray a( i.value() );

        deviceWriteConfig request( address_.toStdString(), i.key(), a.toStdString() );
        client_->sendMessage( &request );
    }

    // disable configuration while pending
    configuration_->setEnabled( false );

    // update state
    updateConnectionState( Qt::white, Qt::black, tr( "Sending device configuration..." ) );
    timeout_->start();

    qDebug() << "waiting on config changes" << savePending_.size();
    state_ = STATE_WAIT_CONFIG;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::translate()
{
    tagAddressLabel_->setText( tr( "Address:" ) );
    firmwareLabel_->setText( tr( "Firmware Version:" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::onRecvMessage( const message *msg )
{
    // device command result
    if ( message::HFPD_DEV_CMD_RESULT == msg->type_ )
    {
        const deviceCommandResult *request( dynamic_cast<const deviceCommandResult*>( msg ) );

        if ( !request )
            return;

        processMessage( request );
    }

    // device configuration
    else if ( message::HFPD_DEV_CONFIG == msg->type_ )
    {
        const deviceConfig *request( dynamic_cast<const deviceConfig*>( msg ) );

        if ( !request )
            return;

        processMessage( request );
    }

    // device state
    else if ( message::HFPD_DEV_STATE == msg->type_ )
    {
        const deviceState *request( dynamic_cast<const deviceState*>( msg ) );

        if ( !request )
            return;

        processMessage( request );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::onTimeout()
{
    // update state
    updateConnectionState( Qt::white, RED, tr( "DEVICE TIMEOUT" ) );
    timeout_->stop();

    // disable configuration and cancel anything pending
    configuration_->setEnabled( false );
    configuration_->clearPending();

    // set error flag
    error_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::onClearAllMessages()
{
    deviceClearAllMessages request( address_.toStdString() );
    client_->sendMessage( &request );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::onPendingConfigUpdated( int count )
{
    // check all pending config complete
    if ( !savePending() )
        if (( 0 == count ) && ( !error_ ))
            configuration_->setEnabled( true );

    emit pendingConfigUpdated( count );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::onSendLogo( const QByteArray& logo )
{
    deviceWriteConfig request( address_.toStdString(), 80, logo.toStdString() );
    client_->sendMessage( &request );

    // disable configuration while pending
    configuration_->setEnabled( false );

    // update state
    updateConnectionState( Qt::white, Qt::black, tr( "Sending device logo..." ) );
    timeout_->start();

    state_ = STATE_WAIT_LOGO;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::onSendMessage( const QString& message )
{
    const QTime now( QTime::currentTime() );

    deviceSendMessage request( address_.toStdString(), now.hour(), now.minute(), now.second(), 0, message.toStdString() );
    client_->sendMessage( &request );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::onUpdateFirmware( unsigned int id, const QString &filename )
{
    deviceFlashFirmwareOTA request( address_.toStdString(), id, filename.toStdString() );
    client_->sendMessage( &request );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::initialize()
{
    // tag address
    tagAddressLabel_ = new QLabel( this );
    tagAddressLabel_->setObjectName( "tagAddressLabel" );

    tagAddress_ = new QLabel( this );
    tagAddress_->setObjectName( "tagAddress" );

    // firmware
    firmwareLabel_ = new QLabel( this );
    firmwareLabel_->setObjectName( "firmwareLabel" );

    firmware_ = new QLabel( this );
    firmware_->setObjectName( "firmware" );

    // configuration
    configuration_ = new configEditorWidget( this );
    configuration_->setObjectName( "configuration" );

    connect( configuration_, SIGNAL(clearAllMessages()), SLOT(onClearAllMessages()) );
    connect( configuration_, SIGNAL(pendingConfigUpdated(int)), SLOT(onPendingConfigUpdated(int)) );
    connect( configuration_, SIGNAL(sendLogo(QByteArray)), SLOT(onSendLogo(QByteArray)) );
    connect( configuration_, SIGNAL(sendMessage(QString)), SLOT(onSendMessage(QString)) );
    connect( configuration_, SIGNAL(updateFirmware(unsigned int, QString)), SLOT(onUpdateFirmware(unsigned int, QString)) );

    // connection state
    connectionState_ = new QLabel( this );
    connectionState_->setAlignment( Qt::AlignCenter );
    connectionState_->setAutoFillBackground( true );
    connectionState_->setObjectName( "connectionState" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::createLayout()
{
    QFormLayout *configLayout( new QFormLayout() );
    configLayout->addRow( tagAddressLabel_, tagAddress_ );
    configLayout->addRow( firmwareLabel_, firmware_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addLayout( configLayout );
    layout->addWidget( configuration_, 1 );
    layout->addWidget( connectionState_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::updateConnectionState( const QColor& fg, const QColor& bg, const QString& text )
{
    QPalette p( connectionState_->palette() );
    p.setColor( connectionState_->foregroundRole(), fg );
    p.setColor( connectionState_->backgroundRole(), bg );

    connectionState_->setText( text );
    connectionState_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::processMessage( const deviceCommandResult *msg )
{
    qDebug() << "device cmd result" << msg->command_ << msg->result_ << QString::fromStdString( msg->address_ );

    // handle error disconnecting when we never actually connected in the first place
    if ( STATE_DISCONNECTING == state_ )
        if (( message::HFPD_DEV_DISCONNECT == msg->command_ ) && ( message::HFPD_ERROR_NOT_CONNECTED == msg->result_ ))
        {
            // update state
            updateConnectionState( Qt::white, RED, tr( "DEVICE NOT RESPONDING" ) );
            timeout_->stop();

            state_ = STATE_DISCONNECTED;

            // disable config
            configuration_->setEnabled( false );
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::processMessage( const deviceConfig *msg )
{
    const QByteArray value( msg->value_.c_str(), msg->value_.size() );

    // check config against any pending config changes
    if ( savePending() )
    {
        if ( !savePending_.contains( msg->id_ ) )
            qDebug() << "config not pending" << msg->id_ << value;
        else if ( value != savePending_[msg->id_] )
            qDebug() << "pending config mismatch" << msg->id_ << value << "expected" << savePending_[msg->id_];
        else
        {
            savePending_.remove( msg->id_ );
        }
    }

    // save config value
    qDebug() << "device config" << msg->id_ << value;
    configuration_->setValue( msg->id_, value );

    // process config
    if ( STATE_WAIT_CONFIG == state_ )
    {
        // still have pending config
        if ( !savePending_.isEmpty() )
            qDebug() << "pending config" << savePending_.size();

        // process config
        else if ( configuration_->process() )
        {
            firmware_->setText( configuration_->firmwareVersion() );

            // update state
            updateConnectionState( Qt::white, GREEN, tr( "CONNECTED" ) );
            timeout_->stop();

            state_ = STATE_READY;

            // enable configuration
            configuration_->setEnabled( true );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::processMessage( const deviceError *msg )
{
    qDebug() << "device error" << msg->num_ << QString::fromStdString( msg->desc_ ) << QString::fromStdString( msg->address_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::processMessage( const deviceState *msg )
{
    qDebug() << "device state" << msg->state_ << "config state" << state_;

    if ( deviceState::STATE_DISCONNECTED == msg->state_ )
    {
        if ( STATE_DISCONNECTING == state_ )
            emit disconnected();
        else
        {
            // update state
            updateConnectionState( Qt::white, RED, tr( "CONNECTION LOST" ) );
            timeout_->stop();
        }

        state_ = STATE_DISCONNECTED;

        // disable config
        configuration_->setEnabled( false );
    }

    else if ( deviceState::STATE_CONNECTING == msg->state_ )
    {
        // update state
        updateConnectionState( Qt::white, Qt::black, tr( "Connecting to device..." ) );
        timeout_->start();
    }

    else if ( deviceState::STATE_CONNECTED == msg->state_ )
    {
        // update state
        updateConnectionState( Qt::white, Qt::black, tr( "Handshaking..." ) );
        timeout_->start();
    }

    else if ( deviceState::STATE_READY == msg->state_ )
    {
        if ( STATE_CONNECTING == state_ )
        {
            // update state
            updateConnectionState( Qt::white, Qt::black, tr( "Retrieving device configuration..." ) );
            timeout_->start();

            state_ = STATE_READ_ALL_CONFIG;

            // request config
            deviceReadAllConfig request( address_.toStdString() );
            client_->sendMessage( &request );

            // update state
            updateConnectionState( Qt::white, Qt::black, tr( "Wait for device configuration..." ) );
            timeout_->start();

            state_ = STATE_WAIT_CONFIG;
        }
        else if ( STATE_WAIT_CONFIG == state_ )
        {
            // nothing to do
        }
        else if ( STATE_WAIT_LOGO == state_ )
        {
            // update state
            updateConnectionState( Qt::white, GREEN, tr( "CONNECTED" ) );
            timeout_->stop();

            state_ = STATE_READY;

            // enable configuration
            configuration_->setEnabled( true );
        }
        else
        {
            qWarning() << "unhandled state" << state_;
        }
    }

    else
    {
        qWarning() << "unhandled device state";
    }

}

