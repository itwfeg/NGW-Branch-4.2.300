/**
 * @file hfpdtag.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hfpdtag.h"

#include <QBluetoothDeviceInfo>

static const QBluetoothUuid vantron( QString( "{8f900000-b457-4761-8f7a-07f1aebcb453}" ) );

static const QBluetoothUuid employeeNameTop( QString( "{8f900001-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid employeeNameBottom( QString( "{8f900002-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid alertTop( QString( "{8f900011-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid alertBottom( QString( "{8f900012-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid alertClear( QString( "{8f900018-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid beeperConfig( QString( "{8f900020-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid graphicImage( QString( "{8f900030-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid shiftDuration( QString( "{8f900040-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid advertisementConfig( QString( "{8f900050-b457-4761-8f7a-07f1aebcb453}" ) );
static const QBluetoothUuid firmwareVersion( QString( "{8f903550-b457-4761-8f7a-07f1aebcb453}" ) );

///////////////////////////////////////////////////////////////////////////////////////////////////
hfpdTag::hfpdTag( const QBluetoothDeviceInfo &info, QObject *parent ) :
    _Mybase( info, parent )
{
    // setup uuid names
    names_[ employeeNameTop ] = tr( "Top line of employee name" );
    names_[ employeeNameBottom ] = tr( "Bottom line of employee name" );
    names_[ alertTop ] = tr( "Top line of alert" );
    names_[ alertBottom ] = tr( "Bottom line of alert" );
    names_[ alertClear ] = tr( "Clear alert" );
    names_[ beeperConfig ] = tr( "Configure beeper" );
    names_[ graphicImage ] = tr( "Configure graphic image" );
    names_[ shiftDuration ] = tr( "Configure shift duration" );
    names_[ advertisementConfig ] = tr( "Configure BLE advertisements" );
    names_[ firmwareVersion ] = tr( "Firmware version" );

    // read all characteristics
    readAllCharacteristics();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hfpdTag::~hfpdTag()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString hfpdTag::characteristicName( const QBluetoothUuid &uuid ) const
{
    if ( names_.contains( uuid ) )
        return names_[uuid];

    return _Mybase::characteristicName( uuid );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hfpdTag::isReadOnly( const QBluetoothUuid &uuid ) const
{
    return (firmwareVersion == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hfpdTag::isWriteOnly( const QBluetoothUuid &uuid ) const
{
    return (alertClear == uuid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hfpdTag::uuidList hfpdTag::characteristics( const QBluetoothUuid &uuid ) const
{
    if ( vantron == uuid )
        return names_.keys();

    return uuidList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hfpdTag::uuidList hfpdTag::services() const
{
    uuidList ids;
    ids.append( vantron );

    return ids;
}
