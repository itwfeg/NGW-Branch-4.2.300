/**
 * @file dbquery.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "dbquery.h"
#include "dbtables.h"

#include <QtDebug>

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>

/// This is the filename we store sensor information to.
const QString dbQuery::DB_FILENAME( SENSORDATA_FILENAME );

///////////////////////////////////////////////////////////////////////////////////////////////////
dbQuery::dbQuery() :
    db_( NULL )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dbQuery::~dbQuery()
{
    // close database
    if ( db_ )
    {
        delete db_;
        db_ = NULL;
    }

    // shutdown driver
    QSqlDatabase::removeDatabase( "QSQLITE" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbQuery::deleteProfile( unsigned int id )
{
    QSqlQuery query( *db_ );

    query.prepare( "DELETE FROM " TABLE_PROFILES " WHERE " COLUMN_ID "=:id" );
    query.bindValue( ":id", id );

    if ( !query.exec() )
    {
        qDebug() << "failed to delete profile info:" << query.lastError().text();
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbQuery::deleteSensorPoint( unsigned int id )
{
    QSqlQuery query( *db_ );

    query.prepare( "DELETE FROM " TABLE_SENSORPOINTS " WHERE " COLUMN_ID "=:id" );
    query.bindValue( ":id", id );

    if ( !query.exec() )
    {
        qDebug() << "failed to delete sensor point info:" << query.lastError().text();
        return false;
    }

    query.prepare( "UPDATE " TABLE_TEMPERATURE " SET " COLUMN_SENSORPOINT "=NULL WHERE " COLUMN_SENSORPOINT "=:id" );
    query.bindValue( ":id", id );

    if ( !query.exec() )
    {
        qDebug() << "failed to update temperature info:" << query.lastError().text();
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbQuery::deleteSensor( unsigned int id )
{
    QSqlQuery query( *db_ );

    query.prepare( "DELETE FROM " TABLE_SENSORS " WHERE " COLUMN_ID "=:id" );
    query.bindValue( ":id", id );

    if ( !query.exec() )
    {
        qDebug() << "failed to delete sensor info:" << query.lastError().text();
        return false;
    }

    query.prepare( "DELETE FROM " TABLE_TEMPERATURE " WHERE " COLUMN_SENSOR "=:id" );
    query.bindValue( ":id", id );

    if ( !query.exec() )
    {
        qDebug() << "failed to delete temperature info:" << query.lastError().text();
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int dbQuery::insertProfile( const profileInfo &value )
{
    unsigned int id( 0 );

    QSqlQuery query( *db_ );
    query.prepare( "SELECT MAX(" COLUMN_ID ") AS last FROM " TABLE_PROFILES );

    if ( !query.exec() )
        qDebug() << "failed to select on profiles table:" << query.lastError().text();
    else
    {
        const QSqlRecord rec( query.record() );

        id = 1;

        // found the record
        if ( query.next() )
            id += query.value( rec.indexOf( "last" ) ).toUInt();

        if ( !query.prepare( "INSERT INTO " TABLE_PROFILES " (" COLUMN_ID "," COLUMN_DESCRIPTION "," COLUMN_TEMP_WARNING_FROM "," COLUMN_TEMP_NORMAL_FROM "," COLUMN_TEMP_NORMAL_TO "," COLUMN_TEMP_WARNING_TO "," COLUMN_TEMP_STANDARD ") VALUES (:id,:desc,:warnfrom,:normalfrom,:normalto,:warnto,:std)" ) )
        {
            qDebug() << "failed prepare of insert into profiles table:" << query.lastError().text();
            id = 0;
        }
        else
        {
            query.bindValue( ":id", id );
            query.bindValue( ":desc", value.description );
            query.bindValue( ":warnfrom", value.tempWarningFrom );
            query.bindValue( ":normalfrom", value.tempNormalFrom );
            query.bindValue( ":normalto", value.tempNormalTo );
            query.bindValue( ":warnto", value.tempWarningTo );
            query.bindValue( ":std", value.tempStandard );

            if ( !query.exec() )
            {
                qDebug() << "failed to insert into profiles table:" << query.lastError().text();
                id = 0;
            }
        }
    }

    return id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int dbQuery::insertSensorPoint( const sensorPointInfo &value )
{
    unsigned int id( 0 );

    QSqlQuery query( *db_ );
    query.prepare( "SELECT MAX(" COLUMN_ID ") AS last FROM " TABLE_SENSORPOINTS );

    if ( !query.exec() )
        qDebug() << "failed to select on sensor points table:" << query.lastError().text();
    else
    {
        const QSqlRecord rec( query.record() );

        id = 1;

        // found the record
        if ( query.next() )
            id += query.value( rec.indexOf( "last" ) ).toUInt();

        if ( !query.prepare( "INSERT INTO " TABLE_SENSORPOINTS " (" COLUMN_ID "," COLUMN_DESCRIPTION "," COLUMN_PROFILE "," COLUMN_SENSOR "," COLUMN_GENERATE_ALERTS ") VALUES (:id,:desc,:profile,:sensor,:alert)" ) )
        {
            qDebug() << "failed prepare of insert into sensor points table:" << query.lastError().text();
            id = 0;
        }
        else
        {
            query.bindValue( ":id", id );
            query.bindValue( ":desc", value.description );
            query.bindValue( ":profile", value.profile );
            query.bindValue( ":sensor", value.sensorAddress );
            query.bindValue( ":alert", value.generateAlerts );

            if ( !query.exec() )
            {
                qDebug() << "failed to insert into sensor points table:" << query.lastError().text();
                id = 0;
            }
        }
    }

    return id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbQuery::isProfileUsed( unsigned int id )
{
    bool result( true );

    QSqlQuery query( *db_ );

    qDebug() << "checking for use of profile" << id;

    if ( !query.prepare( "SELECT * FROM " TABLE_SENSORPOINTS " WHERE " COLUMN_PROFILE "=:profile" ) )
        qDebug() << "failed to prepare sensor points table query:" << query.lastError().text();
    else
    {
        query.bindValue( ":profile", id );

        if ( !query.exec() )
            qDebug() << "failed to query sensor points table:" << query.lastError().text();
        else
        {
            // check for a record
            result = query.next();
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbQuery::isSensorUsed( const QString &address )
{
    bool result( true );

    QSqlQuery query( *db_ );

    qDebug() << "checking for use of sensor" << address;

    if ( !query.prepare( "SELECT * FROM " TABLE_SENSORPOINTS " WHERE " COLUMN_SENSOR "=:sensor" ) )
        qDebug() << "failed to prepare sensor points table query:" << query.lastError().text();
    else
    {
        query.bindValue( ":sensor", address );

        if ( !query.exec() )
            qDebug() << "failed to query sensor points table:" << query.lastError().text();
        else
        {
            // check for a record
            result = query.next();
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString dbQuery::queryConfigValue( const QString &id )
{
    QString result;

    QSqlQuery query( *db_ );
    query.prepare( "SELECT * FROM " TABLE_CONFIG " WHERE " COLUMN_TYPE "=:type" );
    query.bindValue( ":type", id );

    if ( !query.exec() )
        qDebug() << "failed to query config table:" << query.lastError().text();
    else if ( query.next() )
    {
        const QSqlRecord rec( query.record() );

        result = query.value( rec.indexOf( COLUMN_VALUE ) ).toString();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
profileInfo dbQuery::queryProfile( unsigned int id )
{
    profileInfo result;
    result.id = 0;

    QSqlQuery query( *db_ );
    query.prepare( "SELECT * FROM " TABLE_PROFILES " WHERE " COLUMN_ID "=:id" );
    query.bindValue( ":id", id );

    if ( !query.exec() )
        qDebug() << "failed to query profiles table:" << query.lastError().text();
    else if ( query.next() )
    {
        const QSqlRecord rec( query.record() );

        result.id = query.value( rec.indexOf( COLUMN_ID ) ).toUInt();
        result.description = query.value( rec.indexOf( COLUMN_DESCRIPTION ) ).toString();
        result.tempWarningFrom = query.value( rec.indexOf( COLUMN_TEMP_WARNING_FROM ) ).toReal();
        result.tempWarningTo = query.value( rec.indexOf( COLUMN_TEMP_WARNING_TO ) ).toReal();
        result.tempNormalFrom = query.value( rec.indexOf( COLUMN_TEMP_NORMAL_FROM ) ).toReal();
        result.tempNormalTo = query.value( rec.indexOf( COLUMN_TEMP_NORMAL_TO ) ).toReal();
        result.tempStandard = query.value( rec.indexOf( COLUMN_TEMP_STANDARD ) ).toReal();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<profileInfo> dbQuery::queryProfiles()
{
    QList<profileInfo> results;

    QSqlQuery query( *db_ );
    query.prepare( "SELECT " COLUMN_ID "," COLUMN_DESCRIPTION " FROM " TABLE_PROFILES " ORDER BY " COLUMN_DESCRIPTION );

    if ( !query.exec() )
        qDebug() << "failed to query profiles table:" << query.lastError().text();
    else
    {
        // found the record
        while ( query.next() )
        {
            const QSqlRecord rec( query.record() );

            unsigned int id( query.value( rec.indexOf( COLUMN_ID ) ).toUInt() );
            results.append( queryProfile( id ) );
        }
    }

    return results;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorPointInfo dbQuery::querySensorPoint( unsigned int id )
{
    sensorPointInfo result;
    result.id = 0;

    QSqlQuery query( *db_ );
    query.prepare( "SELECT * FROM " TABLE_SENSORPOINTS " WHERE " COLUMN_ID "=:id" );
    query.bindValue( ":id", id );

    if ( !query.exec() )
        qDebug() << "failed to query sensor points table:" << query.lastError().text();
    else if ( query.next() )
    {
        const QSqlRecord rec( query.record() );

        result.id = query.value( rec.indexOf( COLUMN_ID ) ).toUInt();
        result.description = query.value( rec.indexOf( COLUMN_DESCRIPTION ) ).toString();
        result.profile = query.value( rec.indexOf( COLUMN_PROFILE ) ).toUInt();
        result.sensorAddress = query.value( rec.indexOf( COLUMN_SENSOR ) ).toString();
        result.generateAlerts = (0 < query.value( rec.indexOf( COLUMN_GENERATE_ALERTS ) ).toUInt());
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<sensorPointInfo> dbQuery::querySensorPoints()
{
    QList<sensorPointInfo> results;

    QSqlQuery query( *db_ );
    query.prepare( "SELECT " COLUMN_ID "," COLUMN_DESCRIPTION " FROM " TABLE_SENSORPOINTS " ORDER BY " COLUMN_DESCRIPTION );

    if ( !query.exec() )
        qDebug() << "failed to query sensor points table:" << query.lastError().text();
    else
    {
        // found the record
        while ( query.next() )
        {
            const QSqlRecord rec( query.record() );

            unsigned int id( query.value( rec.indexOf( COLUMN_ID ) ).toUInt() );
            results.append( querySensorPoint( id ) );
        }
    }

    return results;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorPointStateInfo dbQuery::querySensorPointState( unsigned int id )
{
    sensorPointStateInfo result;
    memset( &result, 0, sizeof(result) );

    QSqlQuery sensorPoints( *db_ );

    sensorPoints.prepare( "SELECT * FROM " TABLE_SENSORPOINTS " WHERE " COLUMN_ID "=:id" );
    sensorPoints.bindValue( ":id", id );

    // retrieve sensor point
    if ( !sensorPoints.exec() )
        qDebug() << "error during select of sensor points: " << sensorPoints.lastError().text();
    else if ( sensorPoints.next() )
    {
        const QSqlRecord sensorPoint( sensorPoints.record() );

        const QString name( sensorPoints.value( sensorPoint.indexOf( COLUMN_DESCRIPTION ) ).toString() );
        const QString address( sensorPoints.value( sensorPoint.indexOf( COLUMN_SENSOR ) ).toString() );

        qDebug() << address << " " << name ;

        unsigned int sensorPointState( STATE_OKAY );

        // no sensor bound to this point
        if ( address.isEmpty() )
        {
            qDebug() << "  WARN: No sensor mapped to this point";
            sensorPointState |= STATE_TEMP_WARNING;
            sensorPointState |= STATE_COMM_WARNING;
            sensorPointState |= STATE_BATT_WARNING;
        }

        else
        {
            const qint64 now( QDateTime::currentMSecsSinceEpoch() / 1000 );

            const QString batteryWarning( queryConfigValue( CONFIG_BATTERY_WARNING_FROM ) );
            const QString batteryNormal( queryConfigValue( CONFIG_BATTERY_NORMAL_FROM ) );
            const QString commWarning( queryConfigValue( CONFIG_COMM_WARNING_FROM ) );
            const QString commNormal( queryConfigValue( CONFIG_COMM_NORMAL_FROM ) );

            //
            // BATTERY LEVEL
            //

            // retrieve sensor for this point
            QSqlQuery sensors( *db_ );

            sensors.prepare( "SELECT * FROM " TABLE_SENSORS " WHERE " COLUMN_ADDRESS "=:addr" );
            sensors.bindValue( ":addr", sensorPoints.value( sensorPoint.indexOf( COLUMN_SENSOR ) ).toString() );

            if ( !sensors.exec() )
                qDebug() << "error during select of sensor: " << sensors.lastError().text();
            else if ( sensors.next() )
            {
                const QSqlRecord sensor( sensors.record() );

                // check battery level
                const unsigned int batteryLevel( sensors.value( sensor.indexOf( COLUMN_BATTERY_LEVEL ) ).toUInt() );

                if ( 0 < batteryLevel )
                {
                    result.batteryLevel = batteryLevel;

                    if ( batteryLevel <= batteryWarning.toUInt() )
                    {
                        qDebug() << "  ERROR: Battery level of sensor RED";
                        sensorPointState |= STATE_BATT_ERROR;
                    }
                    else if ( batteryLevel <= batteryNormal.toUInt() )
                    {
                        qDebug() << "  WARN: Battery level of sensor YELLOW";
                        sensorPointState |= STATE_BATT_WARNING;
                    }
                    else
                    {
                        qDebug() << "  Battery level okay (" << batteryLevel << ")";
                    }
                }
            }

            //
            // COMPUTE TEMP AVERAGE
            //

            unsigned int tempAgeValueWarning( commWarning.toUInt() * 60 ); // convert minutes to seconds
            unsigned int tempAgeValueNormal( commNormal.toUInt() * 60 ); // convert minutes to seconds

            // do not allow warning period to be over 4h
            if ( 14400 < tempAgeValueWarning )
                tempAgeValueWarning = 14400;

            // do not allow normal period to be over 2h
            if ( 7200 < tempAgeValueNormal )
                tempAgeValueNormal = 7200;
            else if ( tempAgeValueWarning < tempAgeValueNormal )
                tempAgeValueNormal = tempAgeValueWarning;

            // retrieve temp data for this point
            QSqlQuery temps( *db_ );

            qreal tempWarning( 0.0 );
            unsigned int tempWarningCount( 0 );

            qreal tempNormalHigh( 0.0 );
            qreal tempNormalLow( 0.0 );
            qreal tempNormal( 0.0 );
            unsigned int tempNormalCount( 0 );

            // warning (4h)
            temps.prepare( "SELECT * FROM " TABLE_TEMPERATURE " WHERE " COLUMN_SENSORPOINT "=:sp AND " COLUMN_TIMESTAMP ">:age" );
            temps.bindValue( ":sp", sensorPoints.value( sensorPoint.indexOf( COLUMN_ID ) ).toUInt() );
            temps.bindValue( ":age", (unsigned int) now - tempAgeValueWarning );

            if ( !temps.exec() )
                qDebug() << "error during select of temp data: " << temps.lastError().text();
            else
            {
                while ( temps.next() )
                {
                    const QSqlRecord temp( temps.record() );

                    const unsigned int stamp( temps.value( temp.indexOf( COLUMN_TIMESTAMP ) ).toUInt() );
                    const qreal tempPoint( temps.value( temp.indexOf( COLUMN_TEMPERATURE ) ).toReal() );

                    tempWarning += tempPoint;
                    ++tempWarningCount;

                    if ( result.stamp < stamp )
                    {
                        result.stamp = stamp;
                        result.temp = tempPoint;
                    }
                }
            }

            if ( tempWarningCount )
                tempWarning /= (qreal) tempWarningCount;

            // normal (1h)
            temps.prepare( "SELECT * FROM " TABLE_TEMPERATURE " WHERE " COLUMN_SENSORPOINT "=:sp AND " COLUMN_TIMESTAMP ">:age" );
            temps.bindValue( ":sp", sensorPoints.value( sensorPoint.indexOf( COLUMN_ID ) ).toUInt() );
            temps.bindValue( ":age", (unsigned int) now - tempAgeValueNormal );

            if ( !temps.exec() )
                qDebug() << "error during select of temp data: " << temps.lastError().text();
            else
            {
                bool first( true );

                while ( temps.next() )
                {
                    const QSqlRecord temp( temps.record() );

                    const qreal tempPoint( temps.value( temp.indexOf( COLUMN_TEMPERATURE ) ).toReal() );

                    tempNormal += tempPoint;
                    ++tempNormalCount;

                    if ( first )
                    {
                        tempNormalHigh = tempNormalLow = tempPoint;
                        first = false;
                    }
                    else if ( tempNormalHigh < tempPoint )
                        tempNormalHigh = tempPoint;
                    else if ( tempPoint < tempNormalLow )
                        tempNormalLow = tempPoint;
                }
            }

            if ( tempNormalCount )
                tempNormal /= (qreal) tempNormalCount;

            //
            // TEMP PROFILE
            //

            QSqlQuery profiles( *db_ );

            profiles.prepare( "SELECT * FROM " TABLE_PROFILES " WHERE " COLUMN_ID "=:profile" );
            profiles.bindValue( ":profile", sensorPoints.value( sensorPoint.indexOf( COLUMN_PROFILE ) ).toUInt() );

            if ( !profiles.exec() )
                qDebug() << "error during select of profile data: " << profiles.lastError().text();
            else if ( !profiles.next() )
                qDebug() << "error during seek to profile: " << profiles.lastError().text();
            else
            {
                const QSqlRecord profile( profiles.record() );

                const qreal warningTo( profiles.value( profile.indexOf( COLUMN_TEMP_WARNING_TO ) ).toReal() );
                const qreal normalTo( profiles.value( profile.indexOf( COLUMN_TEMP_NORMAL_TO ) ).toReal() );
                const qreal normalFrom( profiles.value( profile.indexOf( COLUMN_TEMP_NORMAL_FROM ) ).toReal() );
                const qreal warningFrom( profiles.value( profile.indexOf( COLUMN_TEMP_WARNING_FROM ) ).toReal() );

                //               single    (1h)        (4h)
                // TEMPERATURE   point  tempNormal tempWarning
                // ------------- ------ ---------- -----------
                // > warningTo   YELLOW    RED         RED
                // > normalTo             YELLOW       RED
                // standard
                // < normalFrom           YELLOW       RED
                // < warningFrom YELLOW    RED         RED

                if (( tempWarningCount ) && (( tempWarning < normalFrom ) || ( normalTo < tempWarning )))
                {
                    qDebug() << "  ERROR: Temperature RED (" << tempWarning << ")";
                    qDebug() << "         Outside normal range for 4h";
                    sensorPointState |= STATE_TEMP_ERROR;
                }
                else if (( tempNormalCount ) && (( tempNormal < warningFrom ) || ( warningTo < tempNormal )))
                {
                    qDebug() << "  ERROR: Temperature RED (" << tempNormal << ")";
                    qDebug() << "         Outside warning range for 1h";
                    sensorPointState |= STATE_TEMP_ERROR;
                }
                else if (( tempNormalCount ) && (( tempNormal < normalFrom ) || ( normalTo < tempNormal )))
                {
                    qDebug() << "  WARN: Temperature YELLOW (" << tempNormal << ")";
                    qDebug() << "        Outside normal range for 1h";
                    sensorPointState |= STATE_TEMP_WARNING;
                }
                else if (( tempNormalCount ) && (( tempNormalLow < warningFrom ) || ( warningTo < tempNormalHigh )))
                {
                    qDebug() << "  WARN: Temperature YELLOW (" << tempNormalLow << ", " << tempNormalHigh << ")";
                    qDebug() << "        Point outside warning range";
                    sensorPointState |= STATE_TEMP_WARNING;
                }
                else
                {
                    qDebug() << "  Temperature okay (" << tempWarning << ", " << tempNormal << ")";
                }
            }

            //
            // COMM STATE
            //

            // no comm for 4h
            if ( !tempWarningCount )
            {
                qDebug() << "  ERROR: comm RED";
                sensorPointState |= STATE_COMM_ERROR;

                // force other fields error state
                sensorPointState |= STATE_TEMP_ERROR;
                sensorPointState |= STATE_BATT_ERROR;
            }

            // no comm for 1h
            else if ( !tempNormalCount )
            {
                qDebug() << "  WARN: comm YELLOW";
                sensorPointState |= STATE_COMM_WARNING;
            }
        }

        result.state = sensorPointState;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorInfo dbQuery::querySensor( unsigned int id )
{
    sensorInfo result;
    result.id = 0;

    QSqlQuery query( *db_ );
    query.prepare( "SELECT * FROM " TABLE_SENSORS " WHERE " COLUMN_ID "=:id" );
    query.bindValue( ":id", id );

    if ( !query.exec() )
        qDebug() << "failed to query sensors table:" << query.lastError().text();
    else if ( query.next() )
    {
        const QSqlRecord rec( query.record() );

        result.id = query.value( rec.indexOf( COLUMN_ID ) ).toUInt();
        result.address = query.value( rec.indexOf( COLUMN_ADDRESS ) ).toString();
        result.type = query.value( rec.indexOf( COLUMN_TYPE ) ).toUInt();
        result.lastComm = query.value( rec.indexOf( COLUMN_LAST_COMM ) ).toUInt();
        result.temperature = query.value( rec.indexOf( COLUMN_TEMPERATURE ) ).toReal();
        result.batteryLevel = query.value( rec.indexOf( COLUMN_BATTERY_LEVEL ) ).toUInt();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorInfo dbQuery::querySensor( const QString &address )
{
    sensorInfo result;
    result.id = 0;

    QSqlQuery query( *db_ );
    query.prepare( "SELECT * FROM " TABLE_SENSORS " WHERE " COLUMN_ADDRESS "=:id" );
    query.bindValue( ":id", address );

    if ( !query.exec() )
        qDebug() << "failed to query sensors table:" << query.lastError().text();
    else if ( query.next() )
    {
        const QSqlRecord rec( query.record() );

        result.id = query.value( rec.indexOf( COLUMN_ID ) ).toUInt();
        result.address = query.value( rec.indexOf( COLUMN_ADDRESS ) ).toString();
        result.type = query.value( rec.indexOf( COLUMN_TYPE ) ).toUInt();
        result.lastComm = query.value( rec.indexOf( COLUMN_LAST_COMM ) ).toUInt();
        result.temperature = query.value( rec.indexOf( COLUMN_TEMPERATURE ) ).toReal();
        result.batteryLevel = query.value( rec.indexOf( COLUMN_BATTERY_LEVEL ) ).toUInt();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<sensorInfo> dbQuery::querySensors( unsigned int age )
{
    QList<sensorInfo> results;

    QSqlQuery query( *db_ );
    query.prepare( "SELECT " COLUMN_ID "," COLUMN_ADDRESS " FROM " TABLE_SENSORS " WHERE " COLUMN_LAST_COMM ">:age ORDER BY " COLUMN_ADDRESS );
    query.bindValue( ":age", age );

    if ( !query.exec() )
        qDebug() << "failed to query sensors table:" << query.lastError().text();
    else
    {
        // found the record
        while ( query.next() )
        {
            const QSqlRecord rec( query.record() );

            unsigned int id( query.value( rec.indexOf( COLUMN_ID ) ).toUInt() );
            results.append( querySensor( id ) );
        }
    }

    return results;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<tempInfo> dbQuery::queryTemps( unsigned int sensor, unsigned int sensorPoint, unsigned int bottom, unsigned int top )
{
    QList<tempInfo> results;

    QString column;
    unsigned int columnValue( 0 );

    // what column are we filtering on
    if ( 0 < sensorPoint )
    {
        column = COLUMN_SENSORPOINT;
        columnValue = sensorPoint;
    }
    else if ( 0 < sensor )
    {
        column = COLUMN_SENSOR;
        columnValue = sensor;
    }

    if (( column.length() ) && ( 0 < columnValue ))
    {
        QSqlQuery query( *db_ );

        if (( bottom ) && ( top ))
        {
            query.prepare( "SELECT * FROM " TABLE_TEMPERATURE " WHERE (" + column + "=:id AND " COLUMN_TIMESTAMP ">=:bottom AND " COLUMN_TIMESTAMP "<=:top) ORDER BY " COLUMN_TIMESTAMP );
            query.bindValue( ":bottom", bottom );
            query.bindValue( ":top", top );
            query.bindValue( ":id", columnValue );
        }
        else if ( bottom )
        {
            query.prepare( "SELECT * FROM " TABLE_TEMPERATURE " WHERE (" + column + "=:id AND " COLUMN_TIMESTAMP ">=:bottom) ORDER BY " COLUMN_TIMESTAMP );
            query.bindValue( ":bottom", bottom );
            query.bindValue( ":id", columnValue );
        }
        else
        {
            query.prepare( "SELECT * FROM " TABLE_TEMPERATURE " WHERE " + column + "=:id ORDER BY " COLUMN_TIMESTAMP );
            query.bindValue( ":id", columnValue );
        }

        if ( !query.exec() )
            qDebug() << "failed to query sensors table:" << query.lastError().text();
        else
        {
            // found the record
            while ( query.next() )
            {
                const QSqlRecord rec( query.record() );

                tempInfo ti;
                ti.sensor = query.value( rec.indexOf( COLUMN_SENSOR ) ).toUInt();
                ti.stamp = query.value( rec.indexOf( COLUMN_TIMESTAMP ) ).toUInt();
                ti.temperature = query.value( rec.indexOf( COLUMN_TEMPERATURE ) ).toReal();
                ti.sensorPoint = query.value( rec.indexOf( COLUMN_SENSORPOINT ) ).toUInt();

                results.append( ti );
            }
        }
    }

    return results;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbQuery::updateConfigValue( const QString &id, const QString &value )
{
    QSqlQuery query( *db_ );

    if ( !query.prepare( "UPDATE " TABLE_CONFIG " SET " COLUMN_VALUE "=:value WHERE " COLUMN_TYPE "=:id" ) )
        qDebug() << "failed to prepare config table update:" << query.lastError().text();
    else
    {
        query.bindValue( ":id", id );
        query.bindValue( ":value", value );

        if ( !query.exec() )
            qDebug() << "failed to update config table:" << query.lastError().text();
        else
        {
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbQuery::updateProfile( const profileInfo &value )
{
    QSqlQuery query( *db_ );

    if ( !query.prepare( "UPDATE " TABLE_PROFILES " SET " COLUMN_DESCRIPTION "=:desc, " COLUMN_TEMP_WARNING_FROM "=:warnfrom, " COLUMN_TEMP_NORMAL_FROM "=:normalfrom, " COLUMN_TEMP_NORMAL_TO "=:normalto, " COLUMN_TEMP_WARNING_TO "=:warnto, " COLUMN_TEMP_STANDARD "=:std WHERE " COLUMN_ID "=:id" ) )
        qDebug() << "failed to prepare profiles table update: " << query.lastError().text();
    else
    {
        query.bindValue( ":id", value.id );
        query.bindValue( ":desc", value.description );
        query.bindValue( ":warnfrom", value.tempWarningFrom );
        query.bindValue( ":normalfrom", value.tempNormalFrom );
        query.bindValue( ":normalto", value.tempNormalTo );
        query.bindValue( ":warnto", value.tempWarningTo );
        query.bindValue( ":std", value.tempStandard );

        if ( !query.exec() )
            qDebug() << "failed to update profiles table:" << query.lastError().text();
        else
        {
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbQuery::updateSensorPoint( const sensorPointInfo &value )
{
    QSqlQuery query( *db_ );

        if ( !query.prepare( "UPDATE " TABLE_SENSORPOINTS " SET " COLUMN_DESCRIPTION "=:desc, " COLUMN_PROFILE "=:profile, " COLUMN_SENSOR "=:sensor, " COLUMN_GENERATE_ALERTS "=:alerts WHERE " COLUMN_ID "=:id" ) )
        qDebug() << "failed to prepare sensor points table update: " << query.lastError().text();
    else
    {
        query.bindValue( ":id", value.id );
        query.bindValue( ":desc", value.description );
        query.bindValue( ":profile", value.profile );
        query.bindValue( ":sensor", value.sensorAddress );
        query.bindValue( ":alerts", (value.generateAlerts ? 1 : 0) );

        if ( !query.exec() )
            qDebug() << "failed to update sensor points table:" << query.lastError().text();
        else
        {
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbQuery::initialize()
{
    // create driver
    db_ = new QSqlDatabase( QSqlDatabase::addDatabase( "QSQLITE" ) );

    if ( db_ )
    {
        // open database
        db_->setDatabaseName( DB_FILENAME );

        if ( !db_->open() )
        {
            delete db_;
            db_ = NULL;
        }
    }
}
