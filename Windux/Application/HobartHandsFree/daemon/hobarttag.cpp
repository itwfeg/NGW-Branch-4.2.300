/**
 * @file hobarttag.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobarttag.h"
#include "scanneragent.h"

#include <QFile>

extern bool _rssiLogging;

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTag::hobartTag( const QBluetoothDeviceInfo& info, QObject *parent ) :
    _Mybase( info, parent ),
    batteryLevel_( 0 ),
    firmwareVersion_( 0 ),
    messages_( 0 ),
    rssiHighPower_( 0 )
{
    connect( this, SIGNAL(valueChanged(QBluetoothUuid,QByteArray)), SLOT(onValueChanged(QBluetoothUuid,QByteArray)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartTag::~hobartTag()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int hobartTag::batteryLevel() const
{
    QReadLocker guard( &m_ );
    return batteryLevel_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime hobartTag::created() const
{
    QReadLocker guard( &m_ );
    return created_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int hobartTag::firmwareVersion() const
{
    QReadLocker guard( &m_ );
    return firmwareVersion_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime hobartTag::lastAdvert( powerLevel txpower ) const
{
    QReadLocker guard( &m_ );

    if ( lastAdvert_.contains( txpower ) )
        return lastAdvert_[txpower];

    return QDateTime();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime hobartTag::lastComm() const
{
    QReadLocker guard( &m_ );
    return lastComm_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int hobartTag::messages() const
{
    QReadLocker guard( &m_ );
    return messages_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString hobartTag::operatorName() const
{
    QReadLocker guard( &m_ );
    return operatorName_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int hobartTag::rssi( powerLevel txpower ) const
{
    QReadLocker guard( &m_ );

    // give filtered rssi for low power tx
    if ( POWER_LEVEL_LOW == txpower )
        return rssiLowPowerFiltered();

    return rssiHighPower_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag::setOperatorName( const QString& value )
{
    QWriteLocker guard( &m_ );
    operatorName_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int hobartTag::transmitPowerLevel( powerLevel txpower ) const
{
    QReadLocker guard( &m_ );

    if ( powerLevel_.contains( txpower ) )
        return powerLevel_[txpower];

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag::otaMode()
{
    // do nothing, needs overwritten
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartTag::readConfig( int conf, QByteArray& value, bool useCached )
{
    if ( useCached )
    {
        QReadLocker guard( &m_ );

        // check if we have value in our cache
        if ( configValue_.contains( conf ) )
        {
            value = configValue_[conf];
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag::update( const scannerAgentDeviceInfo& info )
{
    const QDateTime now( QDateTime::currentDateTime() );

    QWriteLocker guard( &m_ );

    if ( info.lowPowerAdvert )
    {
        qDebug() << this << "processing low power advert...";

        lastAdvert_[POWER_LEVEL_LOW] = now;
        powerLevel_[POWER_LEVEL_LOW] = info.transmitPowerLevel;

        rssi_ = info.info.rssi();

        // save history
        const rssiDataPoint tx( now, rssi_ );

        history_.append( tx );

        while ( HISTORY_MAX_SIZE < history_.size() )
            history_.pop_front();
    }
    else if ( info.highPowerAdvert )
    {
        qDebug() << this << "processing high power advert...";

        lastAdvert_[POWER_LEVEL_HIGH] = now;
        powerLevel_[POWER_LEVEL_HIGH] = info.transmitPowerLevel;

        batteryLevel_ = info.batteryLevel;
        messages_ = info.messages;

        rssiHighPower_ = info.info.rssi();
    }

    if ( info.firmwareVersion )
        firmwareVersion_ = info.firmwareVersion;

    // ---- //

    // generate log file of measured rssi
    if (( _rssiLogging ) && ( info.lowPowerAdvert ) && ( 2 == powerLevel_.size() ))
    {
        const QBluetoothAddress address( info.info.address() );

        QString tagId( address.toString() );
        tagId.replace( ":", "" );

        QString powerLevel( QString::number( info.transmitPowerLevel ) );
        powerLevel.replace( "-", "m" );

        QFile f( tagId + "_" + powerLevel );

        // open file and append data
        if ( f.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) )
        {
            QString logline;
            logline += QString::number( now.toMSecsSinceEpoch() ) + ",";
            logline += QString::number( rssiHighPower_ ) + ",";
            logline += QString::number( rssi_ ) + ",";
            logline += QString::number( rssiLowPowerFiltered() ) + "\n";

            f.write( logline.toLatin1() );
            f.close();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartTag::writeConfig( int conf, const QByteArray& value )
{
    {
        QWriteLocker guard( &m_ );
        configValue_[conf] = value;
    }

    emit configValueChanged( conf, value );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartTag::onValueChanged( const QBluetoothUuid& /*uuid*/, const QByteArray& /*value*/ )
{
    QWriteLocker guard( &m_ );
    lastComm_ = QDateTime::currentDateTime();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int hobartTag::rssiLowPowerFiltered() const
{
    const QDateTime now( QDateTime::currentDateTime() );

    // compute average rssi
    //
    // the hobart tag is (default) configured to send at least 3 low power adverts per
    // second... so here we make sure we have the expected transmit history and we average it
    // to give a filtered rssi value
    //
    int rssi( 0 );
    int rssiCount( 0 );

    for ( rssiHistoryList::const_iterator tx( history_.constBegin() ); tx != history_.constEnd(); ++tx )
        if ( tx->first.msecsTo( now ) <= HISTORY_AMOUNT_MS )
        {
            rssi += tx->second;
            ++rssiCount;
        }

    if (( !rssiCount ) || ( rssiCount < HISTORY_MIN_SIZE ))
    {
        qDebug() << "rssi low power history" << history_.size() << rssiCount;
        return -127;
    }

    const int result( rssi / rssiCount );

    qDebug() << "rssi low power history" << history_.size() << rssiCount << result;
    return result;
}

