/**
 * @file scaleperipherals.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "scaleperipherals.h"
#include "stringsjson.h"

#include <QCoreApplication>
#include <QList>
#include <QTimer>

QMutex scalePeripherals::instanceMutex_;
scalePeripherals *scalePeripherals::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
scalePeripherals::scalePeripherals( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive )
{
    // move to application thread
    moveToThread( QCoreApplication::instance()->thread() );

    // setup device descriptions
    deviceDescriptions_[1000*WEIGHER+0] = "First Weigher";
    deviceDescriptions_[1000*WEIGHER+1] = "Second Weigher";
    deviceDescriptions_[1000*LABEL_APPLIER+0] = "Label Applier";
    deviceDescriptions_[1000*PRINTER+0] = "First Printer";
    deviceDescriptions_[1000*PRINTER+1] = "Second Printer";
    deviceDescriptions_[1000*PRINTER+2] = "Third Printer";
    deviceDescriptions_[1000*WRAPPER+0] = "Wrapper Camera";
    deviceDescriptions_[1000*WRAPPER+1] = "Wrapper Node 1";
    deviceDescriptions_[1000*WRAPPER+2] = "Wrapper Node 2";
    deviceDescriptions_[1000*WRAPPER+3] = "Wrapper Node 3";
    deviceDescriptions_[1000*WRAPPER+4] = "Wrapper Node 4";
    deviceDescriptions_[1000*WRAPPER+5] = "Wrapper Node 5";
    deviceDescriptions_[1000*NUMERIC+0] = "Numeric";
    deviceDescriptions_[1000*MARQUEE+0] = "Marquee";
    deviceDescriptions_[1000*GCD+0] = "Graphical Customer Display";

    // setup device kinds
    deviceKinds_[1000*WEIGHER+2] = "USB";
    deviceKinds_[1000*WEIGHER+3] = "APC";
    deviceKinds_[1000*WEIGHER+4] = "AWS2";
    deviceKinds_[1000*WEIGHER+5] = "KPC";

    deviceKinds_[1000*LABEL_APPLIER+3] = "Access";
    deviceKinds_[1000*LABEL_APPLIER+5] = "CLAS";

    deviceKinds_[1000*PRINTER+1] = "EPP";
    deviceKinds_[1000*PRINTER+2] = "ESS";
    deviceKinds_[1000*PRINTER+3] = "ESC";
    deviceKinds_[1000*PRINTER+4] = "APC";
    deviceKinds_[1000*PRINTER+5] = "EPP3";
    deviceKinds_[1000*PRINTER+6] = "EPP4";
    deviceKinds_[1000*PRINTER+9] = "KPC";

    deviceKinds_[1000*WRAPPER+1] = "Ultima";
    deviceKinds_[1000*WRAPPER+2] = "Access";

    deviceKinds_[1000*NUMERIC+1] = "ESC";
    deviceKinds_[1000*NUMERIC+2] = "APC";

    deviceKinds_[1000*MARQUEE+1] = "ESC";
    deviceKinds_[1000*MARQUEE+2] = "APC";

    deviceKinds_[1000*GCD+1] = "USB";

    // setup timer
    requestTimer_ = new QTimer( this );
    requestTimer_->setSingleShot( false );
    requestTimer_->setInterval( REQUEST_TIME_INITIAL );

    connect( requestTimer_, SIGNAL(timeout()), SLOT(requestPeripheralVersions()) );

    requestTimer_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scalePeripherals::~scalePeripherals()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scalePeripherals::updatePeripheralList( int dev, int devex, int kind, const QString& swVer, const QString& hwVer )
{
    QMutexLocker guard( &m_ );

    const int key( 1000*dev + devex );

    if ( !deviceDescriptions_.contains( key ) )
    {
        LOG_WARN << "unknown device " << dev << " " << devex << " " << kind;
        return;
    }

    QJsonObject obj;
    obj[JSON_DESCRIPTION] = deviceDescription( (PeripheralType) dev, devex );
    obj[JSON_TYPE] = deviceKind( (PeripheralType) dev, kind );
    obj[JSON_SW_VER] = swVer;
    obj[JSON_HW_VER] = hwVer;

    devices_[key] = obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonArray scalePeripherals::toJsonArray() const
{
    QMutexLocker guard( &m_ );

    // populate result
    QJsonArray result;

    foreach ( const QJsonObject& obj, devices_ )
        result.append( obj );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scalePeripherals *scalePeripherals::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scalePeripherals::requestPeripheralVersions()
{
    LOG_DEBUG << "request peripheral versions...";

    // perform request
    if ( !hook_request( "GetAttachedPeripherals", true, DELAY_TIME ) )
        LOG_WARN << "hook request failed!";

    // update timer interval
    requestTimer_->setInterval( REQUEST_TIME );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scalePeripherals::deviceDescription( PeripheralType dev, int devex ) const
{
    static const QString unknown( "Unknown" );

    const int key( 1000 * (int)( dev ) + devex );

    if ( deviceDescriptions_.contains( key ) )
        return deviceDescriptions_[key];

    return unknown;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scalePeripherals::deviceKind( PeripheralType dev, int kind ) const
{
    static const QString unknown( "Unknown" );

    const int key( 1000 * (int)( dev ) + kind );

    if ( deviceKinds_.contains( key ) )
        return deviceKinds_[key];

    return unknown;
}
