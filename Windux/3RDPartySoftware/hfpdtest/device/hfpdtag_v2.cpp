/**
 * @file hfpdtag.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hfpdtag_v2.h"

#include <QBluetoothDeviceInfo>

static const QBluetoothUuid vantron( QString( "{0000fff9-0000-1000-8000-00805f9b34fb}" ) );

static const QBluetoothUuid rx( QString( "{0000fff7-0000-1000-8000-00805f9b34fb}" ) );
static const QBluetoothUuid tx( QString( "{0000fff8-0000-1000-8000-00805f9b34fb}" ) );

///////////////////////////////////////////////////////////////////////////////////////////////////
hfpdTagV2::hfpdTagV2( const QBluetoothDeviceInfo &info, QObject *parent ) :
    _Mybase( info, parent )
{
    // setup uuid names
    names_[ rx ] = tr( "Notify" );
    names_[ tx ] = tr( "Read and write" );

    // test device!
    firmware();
    writeMessage( 1, QTime::currentTime(), "Test Message!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hfpdTagV2::~hfpdTagV2()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString hfpdTagV2::characteristicName( const QBluetoothUuid &uuid ) const
{
    if ( names_.contains( uuid ) )
        return names_[uuid];

    return _Mybase::characteristicName( uuid );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hfpdTagV2::isReadOnly( const QBluetoothUuid &uuid ) const
{
    return (rx == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hfpdTagV2::isWriteOnly( const QBluetoothUuid &uuid ) const
{
    return (tx == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hfpdTagV2::setValue( const QBluetoothUuid &uuid, const QByteArray &value )
{
    if ( 20 != value.length() )
    {
        qWarning() << "bad value" << uuid.toString() << value.toHex();
        return;
    }

    // calculate checksum
    QByteArray valueWithChecksum( value );

    unsigned long checksum( 0 );

    for ( int i( 0 ); i < 19; ++i )
        checksum += value[i];

    valueWithChecksum[19] = (char) (checksum & 0xff);

    // send!
    _Mybase::setValue( uuid, valueWithChecksum );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hfpdTagV2::firmware()
{
    QByteArray buffer( 20, '\0' );
    buffer[0] = 1;

    setValue( tx, buffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hfpdTagV2::writeMessage( unsigned char id, const QTime &time, const QByteArray &msg )
{
    QByteArray stamp( 20, '\0' );
    stamp[0] = 5;
    stamp[1] = id;
    stamp[2] = time.hour();
    stamp[3] = time.minute();
    stamp[4] = time.second();

    setValue( tx, stamp );

    // ---- //

    QByteArray buffer( 20, '\0' );
    buffer[0] = 4;

    for ( int i( 0 ); i < 16; ++i )
        if ( i < msg.length() )
            buffer[1+i] = msg.at( i );

    setValue( tx, buffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hfpdTagV2::uuidList hfpdTagV2::characteristics( const QBluetoothUuid &uuid ) const
{
    if ( vantron == uuid )
        return names_.keys();

    return uuidList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hfpdTagV2::isNotifyEnabled( const QBluetoothUuid &uuid ) const
{
    return (rx == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hfpdTagV2::uuidList hfpdTagV2::services() const
{
    uuidList ids;
    ids.append( vantron );

    return ids;
}
