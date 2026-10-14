/**
 * @file dbuploader.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "bluenicaconfigdatarec.h"
#include "bluenicatempdatarec.h"
#include "dbtables.h"
#include "dbuploader.h"

#include <iostream>

#include <sys/time.h>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>

/// This is the filename we store sensor information to.
const std::string dbUploader::DB_FILENAME( SENSORDATA_FILENAME );

///////////////////////////////////////////////////////////////////////////////////////////////////
dbUploader::dbUploader() :
    db_( NULL )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dbUploader::~dbUploader()
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

template <>
void dbUploader::processSensorData<bluenicaConfigDataRec>( const bluenicaConfigDataRec &data )
{
    // only interested in new records
    if ( isDuplicateRecord( data.address(), data.recordId() ) )
        return;

    // insert record into db
    unsigned int type( SENSOR_MANF_BLUENICA | SENSOR_TYPE_TEMPERATURE );
	
    unsigned int sensorPoint( 0 );
    unsigned int sensor;

    if ( 0 < (sensor = updateSensor( data.address(), type, data.timestamp(), data.temperature(), 0, sensorPoint )) )
        insertTemperature( sensor, data.timestamp(), data.temperature(), sensorPoint );
}

template <>
void dbUploader::processSensorData<bluenicaTempDataRec>( const bluenicaTempDataRec &data )
{
    // only interested in new records
    if ( isDuplicateRecord( data.address(), data.recordId() ) )
        return;

    // insert record into db
    unsigned int type( SENSOR_MANF_BLUENICA | SENSOR_TYPE_TEMPERATURE );
	
    unsigned int sensorPoint( 0 );
	unsigned int sensor;

    if ( 0 < (sensor = updateSensor( data.address(), type, data.timestamp(), data.temperature(), data.batteryLevel(), sensorPoint )) )
    {
        // timestamp we recieved the tx may be different from when it was created... so here we
        // offset the tx timestamp by the record age
        unsigned int timestamp( data.timestamp() );
        timestamp -= data.age();

        insertTemperature( sensor, timestamp, data.temperature(), sensorPoint );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbUploader::postEvent( const leAdvertInfo &e )
{
    // bluenica sensors
    if ( bluenicaDataRec::isBluenicaSensor( e ) )
    {
        bluenicaDataRec *record( bluenicaDataRec::decode( e ) );

        if ( record )
        {
            bluenicaConfigDataRec *configData;
            bluenicaTempDataRec *tempData;

            // handle record
            if ( (configData = dynamic_cast<bluenicaConfigDataRec*>( record )) )
                processSensorData( *configData );
            else if ( (tempData = dynamic_cast<bluenicaTempDataRec*>( record )) )
                processSensorData( *tempData );
            else
                std::cerr << "could not decode sensor data" << std::endl;

            // delete record
            delete record;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbUploader::housekeeping()
{
    // get timestamp
    timeval now;
    gettimeofday( &now, NULL );

    std::cout << "performing housekeeping " << now.tv_sec << "..." << std::endl;

    // cull records that are too old
    const QString maxTempAge( queryConfigValue( CONFIG_MAX_TEMP_AGE ).c_str() );

    std::cout << "culling old records..." << std::endl;

    if ( maxTempAge.length() )
    {
        // Note:
        // HACCP (Hazard Analysis Critical Control Point) dictates we *MUST* keep data for a
        // minimum of 14d. Do not allow them to delete records under 14d old.

        unsigned int maxTempAgeValue( std::max( maxTempAge.toUInt(), (unsigned int) 14 ) );
        maxTempAgeValue *= (24 * 60 * 60); // convert days to seconds

        if ( !maxTempAgeValue )
            std::cerr << "invalid max temp age setting" << std::endl;
        else
        {
            const time_t age( now.tv_sec - maxTempAgeValue );

            std::cout << "culling records older than " << age << "..." << std::endl;
            QSqlQuery query( *db_ );

            // remove sensors
            query.prepare( "DELETE FROM " TABLE_SENSORS " WHERE " COLUMN_LAST_COMM "<:age" );
            query.bindValue( ":age", (unsigned int) age );

            if ( !query.exec() )
                std::cerr << "failed cleanup of sensors table: " << qPrintable( query.lastError().text() ) << std::endl;

            // remove temp data
            query.prepare( "DELETE FROM " TABLE_TEMPERATURE " WHERE " COLUMN_TIMESTAMP "<:age" );
            query.bindValue( ":age", (unsigned int) age );

            if ( !query.exec() )
                std::cerr << "failed cleanup of temperature table: " << qPrintable( query.lastError().text() ) << std::endl;
        }
    }

    // generate alerts
    const QString batteryWarning( queryConfigValue( CONFIG_BATTERY_WARNING_FROM ).c_str() );
    const QString batteryNormal( queryConfigValue( CONFIG_BATTERY_NORMAL_FROM ).c_str() );
    const QString commWarning( queryConfigValue( CONFIG_COMM_WARNING_FROM ).c_str() );
    const QString commNormal( queryConfigValue( CONFIG_COMM_NORMAL_FROM ).c_str() );

    std::cout << "processing sensor points..." << std::endl;

    if (( batteryWarning.length() ) && ( batteryNormal.length() ) &&
        ( commWarning.length() ) && ( commNormal.length() ))
    {
        QSqlQuery sensorPoints( *db_ );

        // retrieve sensor points that generate alerts
        if ( !sensorPoints.exec( "SELECT * FROM " TABLE_SENSORPOINTS " WHERE " COLUMN_GENERATE_ALERTS ">0" ) )
            std::cerr << "error during select of sensor points: " << qPrintable( sensorPoints.lastError().text() ) << std::endl;
        else
        {
            // iterate sensor points
            while ( sensorPoints.next() )
            {
                const QSqlRecord sensorPoint( sensorPoints.record() );

                const QString name( sensorPoints.value( sensorPoint.indexOf( COLUMN_DESCRIPTION ) ).toString() );
                const QString address( sensorPoints.value( sensorPoint.indexOf( COLUMN_SENSOR ) ).toString() );

                std::cout << qPrintable( address ) << " " << qPrintable( name ) << std::endl;

                unsigned int sensorPointState( STATE_OKAY );

                // no sensor bound to this point
                if ( address.isEmpty() )
                {
                    std::cout << "  WARN: No sensor mapped to this point" << std::endl;
                    sensorPointState |= STATE_TEMP_WARNING;
                    sensorPointState |= STATE_COMM_WARNING;
                    sensorPointState |= STATE_BATT_WARNING;
                }

                else
                {
                    //
                    // BATTERY LEVEL
                    //

                    // retrieve sensor for this point
                    QSqlQuery sensors( *db_ );

                    sensors.prepare( "SELECT * FROM " TABLE_SENSORS " WHERE " COLUMN_ADDRESS "=:addr" );
                    sensors.bindValue( ":addr", sensorPoints.value( sensorPoint.indexOf( COLUMN_SENSOR ) ).toString() );

                    if ( !sensors.exec() )
                        std::cerr << "error during select of sensor: " << qPrintable( sensors.lastError().text() ) << std::endl;
                    else if ( sensors.next() )
                    {
                        const QSqlRecord sensor( sensors.record() );

                        // check battery level
                        const unsigned int batteryLevel( sensors.value( sensor.indexOf( COLUMN_BATTERY_LEVEL ) ).toUInt() );

                        if ( 0 < batteryLevel )
                        {
                            if ( batteryLevel <= batteryWarning.toUInt() )
                            {
                                std::cout << "  ERROR: Battery level of sensor RED" << std::endl;
                                sensorPointState |= STATE_BATT_ERROR;
                            }
                            else if ( batteryLevel <= batteryNormal.toUInt() )
                            {
                                std::cout << "  WARN: Battery level of sensor YELLOW" << std::endl;
                                sensorPointState |= STATE_BATT_WARNING;
                            }
                            else
                            {
                                std::cout << "  Battery level okay (" << batteryLevel << ")" << std::endl;
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
                    temps.bindValue( ":age", (unsigned int) now.tv_sec - tempAgeValueWarning );

                    if ( !temps.exec() )
                        std::cerr << "error during select of temp data: " << qPrintable( temps.lastError().text() ) << std::endl;
                    else
                    {
                        while ( temps.next() )
                        {
                            const QSqlRecord temp( temps.record() );
                            const qreal tempPoint( temps.value( temp.indexOf( COLUMN_TEMPERATURE ) ).toReal() );

                            tempWarning += tempPoint;
                            ++tempWarningCount;
                        }
                    }

                    if ( tempWarningCount )
                        tempWarning /= (qreal) tempWarningCount;

                    // normal (1h)
                    temps.prepare( "SELECT * FROM " TABLE_TEMPERATURE " WHERE " COLUMN_SENSORPOINT "=:sp AND " COLUMN_TIMESTAMP ">:age" );
                    temps.bindValue( ":sp", sensorPoints.value( sensorPoint.indexOf( COLUMN_ID ) ).toUInt() );
                    temps.bindValue( ":age", (unsigned int) now.tv_sec - tempAgeValueNormal );

                    if ( !temps.exec() )
                        std::cerr << "error during select of temp data: " << qPrintable( temps.lastError().text() ) << std::endl;
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
                        std::cerr << "error during select of profile data: " << qPrintable( profiles.lastError().text() ) << std::endl;
                    else if ( !profiles.next() )
                        std::cerr << "error during seek to profile: " << qPrintable( profiles.lastError().text() ) << std::endl;
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
                            std::cout << "  ERROR: Temperature RED (" << tempWarning << ")" << std::endl;
                            std::cout << "         Outside normal range for 4h" << std::endl;
                            sensorPointState |= STATE_TEMP_ERROR;
                        }
                        else if (( tempNormalCount ) && (( tempNormal < warningFrom ) || ( warningTo < tempNormal )))
                        {
                            std::cout << "  ERROR: Temperature RED (" << tempNormal << ")" << std::endl;
                            std::cout << "         Outside warning range for 1h" << std::endl;
                            sensorPointState |= STATE_TEMP_ERROR;
                        }
                        else if (( tempNormalCount ) && (( tempNormal < normalFrom ) || ( normalTo < tempNormal )))
                        {
                            std::cout << "  WARN: Temperature YELLOW (" << tempNormal << ")" << std::endl;
                            std::cout << "        Outside normal range for 1h" << std::endl;
                            sensorPointState |= STATE_TEMP_WARNING;
                        }
                        else if (( tempNormalCount ) && (( tempNormalLow < warningFrom ) || ( warningTo < tempNormalHigh )))
                        {
                            std::cout << "  WARN: Temperature YELLOW (" << tempNormalLow << ", " << tempNormalHigh << ")" << std::endl;
                            std::cout << "        Point outside warning range" << std::endl;
                            sensorPointState |= STATE_TEMP_WARNING;
                        }
                        else
                        {
                            std::cout << "  Temperature okay (" << tempWarning << ", " << tempNormal << ")" << std::endl;
                        }
                    }

                    //
                    // COMM STATE
                    //

                    // no comm for 4h
                    if ( !tempWarningCount )
                    {
                        std::cout << "  ERROR: comm RED" << std::endl;
                        sensorPointState |= STATE_COMM_ERROR;

                        // force other fields error state
                        sensorPointState |= STATE_TEMP_ERROR;
                        sensorPointState |= STATE_BATT_ERROR;
                    }

                    // no comm for 1h
                    else if ( !tempNormalCount )
                    {
                        std::cout << "  WARN: comm YELLOW" << std::endl;
                        sensorPointState |= STATE_COMM_WARNING;
                    }
                }

                // generate alert
                if ( STATE_OKAY != sensorPointState )
                {
                    std::cout << "  ALERT" << std::endl;

                    // TODO
                }
            }
        }
    }

    std::cout << "housekeeping complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbUploader::initialize()
{
    // create driver
    db_ = new QSqlDatabase( QSqlDatabase::addDatabase( "QSQLITE" ) );

    if ( db_ )
    {
        // open database
        db_->setDatabaseName( DB_FILENAME.c_str() );

        if ( db_->open() )
            createTables();
        else
        {
            std::cerr << "could not open database!" << std::endl;

            delete db_;
            db_ = NULL;
        }

        // check db version
        static const unsigned int currentVersion( SENSORDATA_DB_VERSION );

        const QString version( queryConfigValue( CONFIG_DB_VERSION ).c_str() );
        std::cout << "db version: " << qPrintable( version ) << std::endl;

        if ( currentVersion < version.toUInt() )
            std::cerr << "db version is newer than app supports!" << std::endl;
        else if ( currentVersion == version.toUInt() )
            ; // nothing to do
        else
        {
            // upgrade db
            upgradeTables( version.toUInt() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbUploader::createTables( bool populate )
{
    QStringList tables( db_->tables() );

    QSqlQuery query( *db_ );

    // config table
    if ( !tables.contains( TABLE_CONFIG ) )
    {
        query.exec( CREATE_TABLE_CONFIG );

        if ( populate )
            query.exec( INSERT_INTO_CONFIG );
    }

    // profiles table
    if ( !tables.contains( TABLE_PROFILES ) )
    {
        query.exec( CREATE_TABLE_PROFILES );

        if ( populate )
            query.exec( INSERT_INTO_PROFILES );
    }

    // sensors table
    if ( !tables.contains( TABLE_SENSORS ) )
        query.exec( CREATE_TABLE_SENSORS );

    // sensorpoints table
    if ( !tables.contains( TABLE_SENSORPOINTS ) )
        query.exec( CREATE_TABLE_SENSORPOINTS );

    // temperature data
    if ( !tables.contains( TABLE_TEMPERATURE ) )
        query.exec( CREATE_TABLE_TEMPERATURE );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbUploader::upgradeTables( unsigned int version )
{
    bool error( false );

    QStringList tables;
    tables.append( TABLE_CONFIG );
    tables.append( TABLE_PROFILES );
    tables.append( TABLE_SENSORS );
    tables.append( TABLE_SENSORPOINTS );
    tables.append( TABLE_TEMPERATURE );

    QSqlQuery query( *db_ );

    std::cout << "starting db upgrade..." << std::endl;

    // start transaction
    db_->transaction();

    // rename all tables
    foreach ( const QString &table, tables )
        if ( (error = !query.exec( QString( "ALTER TABLE %0 RENAME TO old_%0" ).arg( table ) )) )
        {
            std::cerr << "error during alter table: " << qPrintable( query.lastError().text() ) << std::endl;
            goto finished;
        }

    // create new tables
    createTables( false );

    // populate config values
    query.exec( INSERT_INTO_CONFIG );

    // pull old data into new schema
    foreach ( const QString &table, tables )
    {
        QStringList columns;

        // create list of column names
        if ( (error = !query.exec( QString( "SELECT * FROM old_%0 LIMIT 1" ).arg( table ) )) )
        {
            std::cerr << "error during select of old data: " << qPrintable( query.lastError().text() ) << std::endl;
            goto finished;
        }
        else if ( query.next() )
        {
            const QSqlRecord rec( query.record() );

            for ( int i( 0 ); i < rec.count(); ++i )
                columns.append( rec.fieldName( i ) );
        }

        // pull in data
        if ( columns.count() )
            if ( (error = !query.exec( QString( "REPLACE INTO %0 (%1) SELECT %1 FROM old_%0" ).arg( table ).arg( columns.join( ',' ) ) )) )
            {
                std::cerr << "error during insert of old data: " << qPrintable( query.lastError().text() ) << std::endl;
                goto finished;
            }

    }

    // perform version dependent functions
    switch ( version )
    {
    case 1:
        // do version 1 -> 2 things
        // no break... drop to next case

    case 2:
        // do version 2 -> 3 things
        // no break... drop to next case

    default:
        // update db version
        query.prepare( "UPDATE " TABLE_CONFIG " SET " COLUMN_VALUE "=:value WHERE " COLUMN_TYPE "=:id" );
        query.bindValue( ":id", CONFIG_DB_VERSION );
        query.bindValue( ":value", STRINGIFY(SENSORDATA_DB_VERSION) );

        if ( (error = !query.exec()) )
        {
            std::cerr << "error updating db version: " << qPrintable( query.lastError().text() ) << std::endl;
            goto finished;
        }

        break;
    }

    // drop old tables
    foreach ( const QString &table, tables )
        if ( (error = !query.exec( QString( "DROP TABLE old_%0" ).arg( table ) )) )
        {
            std::cerr << "error during drop table: " << qPrintable( query.lastError().text() ) << std::endl;
            goto finished;
        }

finished:

    // rollback changes we failed to upgrade
    if ( error )
        db_->rollback();
    else
    {
        std::cout << "upgrade complete" << std::endl;
        db_->commit();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string dbUploader::queryConfigValue( const std::string &id )
{
    std::string result;

    QSqlQuery query( *db_ );

    if ( !query.prepare( "SELECT * FROM " TABLE_CONFIG " WHERE " COLUMN_TYPE "=:type" ) )
        std::cerr << "failed prepare of select on config table: " << qPrintable( query.lastError().text() ) << std::endl;
    else
    {
        query.bindValue( ":type", id.c_str() );

        if ( !query.exec() )
            std::cerr << "failed to query config table: " << qPrintable( query.lastError().text() ) << std::endl;
        else if ( query.next() )
        {
            const QSqlRecord rec( query.record() );

            result = qPrintable( query.value( rec.indexOf( COLUMN_VALUE ) ).toString() );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbUploader::isDuplicateRecord( const std::string &address, unsigned int record )
{
    sensorRecordMap::iterator i( records_.find( address ) );
    bool duplicate( false );

    // check for already known record
    if ( records_.end() != i )
        duplicate = (record == i->second);

    // not known, update map
    if ( !duplicate )
        records_[address] = record;

    return duplicate;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int dbUploader::insertSensor( const std::string &address, unsigned int type )
{
    unsigned int id( 0 );

    QSqlQuery query( *db_ );
    query.prepare( "SELECT MAX(" COLUMN_ID ") AS last FROM " TABLE_SENSORS );

    if ( !query.exec() )
        std::cerr << "failed to select on sensors table: " << qPrintable( query.lastError().text() ) << std::endl;
    else
    {
        const QSqlRecord rec( query.record() );

        id = 1;

        // found the record
        if ( query.next() )
            id += query.value( rec.indexOf( "last" ) ).toUInt();

        if ( !query.prepare( "INSERT INTO " TABLE_SENSORS " (" COLUMN_ID "," COLUMN_ADDRESS "," COLUMN_TYPE ") VALUES (:id,:addr,:type)" ) )
        {
            std::cerr << "failed prepare of insert into sensors table: " << qPrintable( query.lastError().text() ) << std::endl;
            id = 0;
        }
        else
        {
            query.bindValue( ":id", id );
            query.bindValue( ":addr", address.c_str() );
            query.bindValue( ":type", type );

            if ( !query.exec() )
            {
                std::cerr << "failed to insert into sensors table: " << qPrintable( query.lastError().text() ) << std::endl;
                id = 0;
            }
        }
    }

    return id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int dbUploader::updateSensor( const std::string &address, unsigned int type, unsigned int stamp, float temp, unsigned int batteryLevel, unsigned int &sensorPoint )
{
    unsigned int id( 0 );

    QSqlQuery query( *db_ );
    query.prepare( "SELECT " COLUMN_ID " FROM " TABLE_SENSORS " WHERE " COLUMN_ADDRESS "=:addr" );
    query.bindValue( ":addr", address.c_str() );

    if ( !query.exec() )
        std::cerr << "failed to query sensors table: " << qPrintable( query.lastError().text() ) << std::endl;
    else
    {
        const QSqlRecord rec( query.record() );

        // found the record
        if ( query.next() )
            id = query.value( rec.indexOf( COLUMN_ID ) ).toUInt();

        // record does not exist, create one
        else
            id = insertSensor( address, type );

        if ( id )
        {
            if ( batteryLevel )
            {
                query.prepare( "UPDATE " TABLE_SENSORS " SET " COLUMN_LAST_COMM "=:stamp, " COLUMN_TEMPERATURE "=:temp, " COLUMN_BATTERY_LEVEL "=:batt WHERE " COLUMN_ID "=:id" );
                query.bindValue( ":stamp", stamp );
                query.bindValue( ":temp", temp );
                query.bindValue( ":batt", batteryLevel );
                query.bindValue( ":id", id );
            }
            else
            {
                query.prepare( "UPDATE " TABLE_SENSORS " SET " COLUMN_LAST_COMM "=:stamp, " COLUMN_TEMPERATURE "=:temp WHERE " COLUMN_ID "=:id" );
                query.bindValue( ":stamp", stamp );
                query.bindValue( ":temp", temp );
                query.bindValue( ":id", id );
            }

            if ( !query.exec() )
                std::cerr << "failed to update sensors table: " << qPrintable( query.lastError().text() ) << std::endl;
            else
            {
                // check for sensor point bound to this address
                query.prepare( "SELECT " COLUMN_ID " FROM " TABLE_SENSORPOINTS " WHERE " COLUMN_SENSOR "=:addr" );
                query.bindValue( ":addr", address.c_str() );

                if ( !query.exec() )
                    std::cerr << "failed to query sensorpoints table: " << qPrintable( query.lastError().text() ) << std::endl;
                else
                {
                    const QSqlRecord rec( query.record() );

                    // found the record
                    if ( query.next() )
                        sensorPoint = query.value( rec.indexOf( COLUMN_ID ) ).toUInt();
                }
            }
        }
    }

    return id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbUploader::insertTemperature( unsigned int sensor, unsigned int stamp, float temp, unsigned int sensorPoint )
{
    QSqlQuery query( *db_ );

    if ( 0 < sensorPoint )
    {
        query.prepare( "INSERT INTO " TABLE_TEMPERATURE " (" COLUMN_SENSOR "," COLUMN_TIMESTAMP "," COLUMN_TEMPERATURE "," COLUMN_SENSORPOINT ") VALUES (:id,:stamp,:temp,:sp)" );
        query.bindValue( ":id", sensor );
        query.bindValue( ":stamp", stamp );
        query.bindValue( ":temp", temp );
        query.bindValue( ":sp", sensorPoint );
    }
    else
    {
        query.prepare( "INSERT INTO " TABLE_TEMPERATURE " (" COLUMN_SENSOR "," COLUMN_TIMESTAMP "," COLUMN_TEMPERATURE ") VALUES (:id,:stamp,:temp)" );
        query.bindValue( ":id", sensor );
        query.bindValue( ":stamp", stamp );
        query.bindValue( ":temp", temp );
    }

    if ( !query.exec() )
    {
        std::cerr << "failed to insert to temperature table: " << qPrintable( query.lastError().text() ) << std::endl;
        return false;
    }

    return true;
}
