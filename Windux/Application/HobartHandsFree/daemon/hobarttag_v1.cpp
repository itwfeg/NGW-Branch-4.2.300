/**
 * @file hobarttag_v1.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobarttag_v1.h"

#include <QBluetoothDeviceInfo>

static const QBluetoothUuid vantron( QString( "{0000fff9-0000-1000-8000-00805f9b34fb}" ) );

static const QBluetoothUuid rx( QString( "{0000fff7-0000-1000-8000-00805f9b34fb}" ) );
static const QBluetoothUuid tx( QString( "{0000fff8-0000-1000-8000-00805f9b34fb}" ) );

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTag_V1::hobartTag_V1( const QBluetoothDeviceInfo& info, QObject *parent ) :
    _Mybase( info, parent )
{
    qDebug() << this << "CTOR";

    // setup uuid names
    names_[ rx ] = tr( "Notify" );
    names_[ tx ] = tr( "Read and write" );

    // connect signals
    connect( this, SIGNAL(valueChanged(QBluetoothUuid,QByteArray)), SLOT(onValueChanged(QBluetoothUuid,QByteArray)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTag_V1::~hobartTag_V1()
{
    qDebug() << this << "DTOR";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString hobartTag_V1::characteristicName( const QBluetoothUuid& uuid ) const
{
    if ( names_.contains( uuid ) )
        return names_[uuid];

    return _Mybase::characteristicName( uuid );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartTag_V1::isReadOnly( const QBluetoothUuid& uuid ) const
{
    return (rx == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartTag_V1::isWriteOnly( const QBluetoothUuid& uuid ) const
{
    return (tx == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag_V1::setValue( const QBluetoothUuid& uuid, const QByteArray& value, const QBluetoothUuid& respUuid )
{
    if ( 20 != value.length() )
    {
        qWarning() << this << "bad value" << uuid.toString() << value.toHex();
        return;
    }

    // calculate checksum
    QByteArray valueWithChecksum( value );

    unsigned long checksum( 0 );

    for ( int i( 0 ); i < 19; ++i )
        checksum += value[i];

    valueWithChecksum[19] = (char) (checksum & 0xff);

    // send!
    _Mybase::setValue( uuid, valueWithChecksum, respUuid );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag_V1::clearMessages()
{
    QByteArray buffer( 20, '\0' );
    buffer[0] = CMD_CLEAR_MESSAGES;

    setValue( tx, buffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag_V1::otaMode()
{
    QByteArray buffer( 20, '\0' );
    buffer[0] = CMD_GO_TO_OTA_MODE;

    setValue( tx, buffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag_V1::readAllConfig()
{
    QList<char> commands;
    commands.append( CMD_GET_OTA_NUMBER );
    commands.append( CMD_GET_FW_VERSION );
    commands.append( CMD_GET_ACTV_MSG_TOP );
    commands.append( CMD_GET_ACTV_MSG_BOT );
    commands.append( CMD_GET_SLEP_MSG_TOP );
    commands.append( CMD_GET_SLEP_MSG_BOT );
    commands.append( CMD_GET_USR_NAME );
    commands.append( CMD_GET_BEEPER_CONFIG );

    foreach ( const char &command, commands )
    {
        QByteArray buffer( 20, '\0' );
        buffer[0] = command;

        setValue( tx, buffer, rx );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartTag_V1::readConfig( int conf, QByteArray& value, bool useCached )
{
    // check if we have value in our cache
    if ( _Mybase::readConfig( conf, value, useCached ) )
        return true;

    // map config type to command
    QMap<configType, char> command;
    command[OTA_ID] = CMD_GET_OTA_NUMBER;
    command[FIRMWARE_VER] = CMD_GET_FW_VERSION;
    command[DEVICE_TYPE] = CMD_GET_BEEPER_CONFIG;
    command[MSG_ACTIVE_TOP] = CMD_GET_ACTV_MSG_TOP;
    command[MSG_ACTIVE_BOTTOM] = CMD_GET_ACTV_MSG_BOT;
    command[MSG_INACTIVE_TOP] = CMD_GET_SLEP_MSG_TOP;
    command[MSG_INACTIVE_BOTTOM] = CMD_GET_SLEP_MSG_BOT;
    command[USER_NAME] = CMD_GET_USR_NAME;
    command[SHIFT_DURATION] = CMD_GET_BEEPER_CONFIG;
    command[ADVERT_TX_POWER_LEVEL] = CMD_GET_BEEPER_CONFIG;
    command[ADVERT_TX_COUNT] = CMD_GET_BEEPER_CONFIG;
    command[ADVERT_TX_DELAY] = CMD_GET_BEEPER_CONFIG;
    command[BEEP_TIME] = CMD_GET_BEEPER_CONFIG;

    if ( !command.contains( (configType) conf ) )
    {
        qWarning() << this << "unknown config type" << conf;
        return false;
    }

    // issue request
    QByteArray buffer( 20, '\0' );
    buffer[0] = command[(configType) conf];

    setValue( tx, buffer, rx );
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartTag_V1::writeConfig( int conf, const QByteArray& value )
{
    QByteArray buffer;

    switch ( conf )
    {
    case MSG_ACTIVE_TOP:
        buffer.append( CMD_SET_ACTV_MSG_TOP );
        buffer.append( padValue( value, 10 ) );
        break;

    case MSG_ACTIVE_BOTTOM:
        buffer.append( CMD_SET_ACTV_MSG_BOT );
        buffer.append( padValue( value, 10 ) );
        break;

    case MSG_INACTIVE_TOP:
        buffer.append( CMD_SET_SLEP_MSG_TOP );
        buffer.append( padValue( value, 10 ) );
        break;

    case MSG_INACTIVE_BOTTOM:
        buffer.append( CMD_SET_SLEP_MSG_BOT );
        buffer.append( padValue( value, 10 ) );
        break;

    case USER_NAME:
        buffer.append( CMD_SET_USR_NAME );
        buffer.append( padValue( value, 16 ) );
        break;

    case SHIFT_DURATION:
        if (( 1 != value.length() ) || ( 36 < value[0] ))
        {
            qWarning() << this << "bad config SHIFT_DURATION" << value;
            return false;
        }

        buffer.append( CMD_SET_SHIFT_DURATION );
        buffer.append( value );
        break;

    case ADVERT_TX_POWER_LEVEL:
        if ( 1 != value.length() )
        {
            qWarning() << this << "bad config ADVERT_TX_POWER_LEVEL" << value;
            return false;
        }
        else
        {
            int test( (signed char) value[0] );

            if (( -40 != test ) && ( -20 != test ) && ( -16 != test ) && ( -12 != test ) &&
                ( -8 != test ) && ( -4 != test ) && ( 0 != test ) && ( 4 != test ))
            {
                qWarning() << this << "bad config ADVERT_TX_POWER_LEVEL" << test;
                return false;
            }
        }

        buffer.append( CMD_SET_TX_PWR_LVL );
        buffer.append( value );
        break;

    case ADVERT_TX_COUNT:
        if ( 1 != value.length() )
        {
            qWarning() << this << "bad config ADVERT_TX_COUNT" << value;
            return false;
        }

        buffer.append( CMD_SET_LOW_POWER_COUNTS );
        buffer.append( value );
        break;

    case ADVERT_TX_DELAY:
        if ( 2 != value.length() )
        {
            qWarning() << this << "bad config CMD_SET_ADV_DELAY" << value;
            return false;
        }
        else
        {
            int test( value[0] << 8 | value[1] );

            if (( test < 100 ) || ( 10000 < test ))
            {
                qWarning() << this << "bad config CMD_SET_ADV_DELAY" << test;
                return false;
            }
        }

        buffer.append( CMD_SET_ADV_DELAY );
        buffer.append( value );
        break;

    case BEEP_TIME:
        if ( 1 != value.length() )
        {
            qWarning() << this << "bad config BEEP_TIME" << value;
            return false;
        }

        buffer.append( CMD_SET_BEEP_TIME );
        buffer.append( value );
        break;

    case LOGO:
        if ( 768 != value.length() )
        {
            qWarning() << this << "bad config LOGO" << value;
            return false;
        }

        // send logo in 16 byte packets
        for ( unsigned int i( 0 ); i < 48; ++i )
        {
            buffer.clear();
            buffer.append( CMD_LOGO_UPDATE + i );
            buffer.append( value.mid( i * 16, 16 ) );
            buffer.append( "\0\0\0", 3 );

            setValue( tx, buffer );
        }

        {
            buffer.clear();
            buffer.append( CMD_LOGO_UPDATE_END );
            buffer.append( "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 19 );

            setValue( tx, buffer );
        }

        return true;

    default:
        qWarning() << this << "unknown config type" << conf;
        return false;
    }

    // pad buffer out
    while ( buffer.length() < 20 )
        buffer.append( '\0' );

    setValue( tx, buffer, rx );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag_V1::sendMessage( unsigned char id, const QTime& time, const QByteArray& msg )
{
    QByteArray stamp( 20, '\0' );
    stamp[0] = CMD_SET_IP_AND_TIME;
    stamp[1] = id;
    stamp[2] = time.hour();
    stamp[3] = time.minute();
    stamp[4] = time.second();

    setValue( tx, stamp );

    // ---- //

    QByteArray buffer( 20, '\0' );
    buffer[0] = CMD_RCV_MESSAGE;

    for ( int i( 0 ); i < qMin( 16, msg.length() ); ++i )
        buffer[i+1] = msg.at( i );

    setValue( tx, buffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTag_V1::uuidList hobartTag_V1::characteristics( const QBluetoothUuid& uuid ) const
{
    if ( vantron == uuid )
        return names_.keys();

    return uuidList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartTag_V1::isNotifyEnabled( const QBluetoothUuid& uuid ) const
{
    return (rx == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTag_V1::uuidList hobartTag_V1::services() const
{
    uuidList ids;
    ids.append( vantron );

    return ids;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag_V1::onValueChanged( const QBluetoothUuid& uuid, const QByteArray& value )
{
    qDebug() << this << "value changed" << uuid << value;

    if ( rx != uuid )
        return;

    // validate message
    if ( 20 != value.length() )
    {
        qWarning() << this << "invalid message length" << value;
        return;
    }

    // calculate checksum
    unsigned long checksum( 0 );

    for ( int i( 0 ); i < 19; ++i )
        checksum += value[i];

    // validate checksum
    if ( value[19] != (char) (checksum & 0xff) )
    {
        qWarning() << this << "bad checksum" << value;
        return;
    }

    // check for error result
    if ( 0x99 == value[1] )
    {
        bool zeros( true );

        for ( int i( 2 ); i < 19; ++i )
            if ( value[i] )
            {
                zeros = false;
                break;
            }

        if ( zeros )
        {
            qWarning() << this << "command failed" << value[0];
            return;
        }
    }

    // ---- //

    QByteArray a;

    // process message
    switch ( value[0] )
    {
    case CMD_GET_FW_VERSION:
        _Mybase::writeConfig( FIRMWARE_VER, value.mid( 1, 2 ) );
        break;

    case CMD_GET_OTA_NUMBER:
        _Mybase::writeConfig( OTA_ID, value.mid( 1, 4 ) );
        break;

    case CMD_GO_TO_OTA_MODE:
        qDebug() << this << "device in ota mode";
        break;

    case CMD_RCV_MESSAGE:
        qDebug() << this << "message sent to device";
        break;

    case CMD_SET_IP_AND_TIME:
        qDebug() << this << "ip and time sent to device";
        break;

    case CMD_CLEAR_MESSAGES:
        qDebug() << this << "device messages cleared";
        break;

    case CMD_SET_ACTV_MSG_TOP:
        readConfig( MSG_ACTIVE_TOP, a, false );
        break;

    case CMD_SET_ACTV_MSG_BOT:
        readConfig( MSG_ACTIVE_BOTTOM, a, false );
        break;

    case CMD_SET_SLEP_MSG_TOP:
        readConfig( MSG_INACTIVE_TOP, a, false );
        break;

    case CMD_SET_SLEP_MSG_BOT:
        readConfig( MSG_INACTIVE_BOTTOM, a, false );
        break;

    case CMD_SET_USR_NAME:
        readConfig( USER_NAME, a, false );
        break;

    case CMD_SET_TX_PWR_LVL:
        readConfig( ADVERT_TX_POWER_LEVEL, a, false );
        break;

    case CMD_SET_SHIFT_DURATION:
        readConfig( SHIFT_DURATION, a, false );
        break;

    case CMD_SET_LOW_POWER_COUNTS:
        readConfig( ADVERT_TX_COUNT, a, false );
        break;

    case CMD_SET_ADV_DELAY:
        readConfig( ADVERT_TX_DELAY, a, false );
        break;

    case CMD_SET_BEEP_TIME:
        readConfig( BEEP_TIME, a, false );
        break;

    case CMD_GET_BEEPER_CONFIG:
        _Mybase::writeConfig( SHIFT_DURATION, value.mid( 1, 1 ) );
        _Mybase::writeConfig( ADVERT_TX_COUNT, value.mid( 2, 1 ) );
        _Mybase::writeConfig( ADVERT_TX_DELAY, value.mid( 3, 2 ) );
        _Mybase::writeConfig( BEEP_TIME, value.mid( 5, 1 ) );
        _Mybase::writeConfig( ADVERT_TX_POWER_LEVEL, value.mid( 6, 1 ) );
        _Mybase::writeConfig( DEVICE_TYPE, value.mid( 7, 2 ) );
        break;

    case CMD_GET_ACTV_MSG_TOP:
        _Mybase::writeConfig( MSG_ACTIVE_TOP, value.mid( 1, 10 ) );
        break;

    case CMD_GET_ACTV_MSG_BOT:
        _Mybase::writeConfig( MSG_ACTIVE_BOTTOM, value.mid( 1, 10 ) );
        break;

    case CMD_GET_SLEP_MSG_TOP:
        _Mybase::writeConfig( MSG_INACTIVE_TOP, value.mid( 1, 10 ) );
        break;

    case CMD_GET_SLEP_MSG_BOT:
        _Mybase::writeConfig( MSG_INACTIVE_BOTTOM, value.mid( 1, 10 ) );
        break;

    case CMD_GET_USR_NAME:
        _Mybase::writeConfig( USER_NAME, value.mid( 1, 16 ) );
        break;

    case CMD_LOGO_UPDATE_END:
        qDebug() << this << "logo update complete";
        break;

    default:

        // CMD_LOGO_UPDATE
        if (( CMD_LOGO_UPDATE <= value[0] ) && ( value[0] < CMD_LOGO_UPDATE_END ))
            break;

        qWarning() << this << "unknown cmd type" << value[0];
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray hobartTag_V1::padValue( const QByteArray& value, int len )
{
    QByteArray result;

    for ( int i( 0 ); i < qMin( len, value.length() ); ++i )
        result.append( value[i] );

    while ( result.length() < len )
        result.append( ' ' );

    return result;
}

