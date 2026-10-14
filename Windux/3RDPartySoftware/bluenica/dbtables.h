/**
 * @file dbtables.h
 * Table and column definitions.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DBTABLES
#define DBTABLES

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

#define SENSORDATA_FILENAME "sensordata.sqlite"
#define SENSORDATA_DB_VERSION 3                                     // if you change this you need to modify dbUploader::upgrade()

// tables
#define TABLE_CONFIG "config"
#define TABLE_PROFILES "profiles"
#define TABLE_SENSORS "sensors"
#define TABLE_SENSORPOINTS "sensorpoints"
#define TABLE_TEMPERATURE "temperature"

// columns
#define COLUMN_ADDRESS "address"
#define COLUMN_BATTERY_LEVEL "battery_level"
#define COLUMN_DESCRIPTION "description"
#define COLUMN_GENERATE_ALERTS "generate_alerts"
#define COLUMN_ID "id"
#define COLUMN_LAST_COMM "last_comm"
#define COLUMN_PROFILE "profile"
#define COLUMN_SENSOR "sensor"
#define COLUMN_SENSORPOINT "sensorpoint"
#define COLUMN_STATE "state"
#define COLUMN_TEMPERATURE "temperature"
#define COLUMN_TEMP_NORMAL_FROM "temp_normal_from"
#define COLUMN_TEMP_NORMAL_TO "temp_normal_to"
#define COLUMN_TEMP_STANDARD "temp_standard"
#define COLUMN_TEMP_WARNING_FROM "temp_warning_from"
#define COLUMN_TEMP_WARNING_TO "temp_warning_to"
#define COLUMN_TIMESTAMP "timestamp"
#define COLUMN_TYPE "type"
#define COLUMN_VALUE "value"

// sensor types

#define SENSOR_TYPE_TEMPERATURE 0x0001

#define SENSOR_MANF_BLUENICA 0x010000

// temp uom types

#define TEMP_UOM_FAHRENHEIT 0
#define TEMP_UOM_CELSIUS 1

// sensor point states

#define STATE_OKAY 0x0000
#define STATE_TEMP_WARNING 0x0001
#define STATE_TEMP_ERROR 0x0002
#define STATE_COMM_WARNING 0x0010
#define STATE_COMM_ERROR 0x0020
#define STATE_BATT_WARNING 0x0100
#define STATE_BATT_ERROR 0x0200

// configuration keys
#define CONFIG_DB_VERSION "db_version"                              // integer
#define CONFIG_BATTERY_WARNING_FROM "battery_warning_from"          // percent
#define CONFIG_BATTERY_NORMAL_FROM "battery_normal_from"            // percent
#define CONFIG_COMM_WARNING_FROM "comm_warning_from"                // minutes
#define CONFIG_COMM_NORMAL_FROM "comm_normal_from"                  // minutes
#define CONFIG_MAX_TEMP_AGE "max_temp_age"                          // days
#define CONFIG_TEMP_UNIT_OF_MEASURE "temp_unit_of_measure"          // 0=fahrenheit, 1=celsius

//
// create table sql
//

#define CREATE_TABLE_CONFIG "create table " \
    TABLE_CONFIG " ( " \
        COLUMN_TYPE                 " text primary key not null, " \
        COLUMN_VALUE                " text );"

#define CREATE_TABLE_PROFILES "create table " \
    TABLE_PROFILES " ( " \
        COLUMN_ID                   " integer primary key not null, " \
        COLUMN_DESCRIPTION          " text, " \
        COLUMN_TEMP_WARNING_FROM    " real, " \
        COLUMN_TEMP_NORMAL_FROM     " real, " \
        COLUMN_TEMP_NORMAL_TO       " real, " \
        COLUMN_TEMP_WARNING_TO      " real, " \
        COLUMN_TEMP_STANDARD        " real );"

#define CREATE_TABLE_SENSORPOINTS "create table " \
    TABLE_SENSORPOINTS " ( " \
        COLUMN_ID                   " integer primary key not null, " \
        COLUMN_DESCRIPTION          " text, " \
        COLUMN_PROFILE              " integer, " \
        COLUMN_SENSOR               " text, " \
        COLUMN_GENERATE_ALERTS      " integer, " \
        COLUMN_STATE                " integer default 0);"

#define CREATE_TABLE_SENSORS "create table " \
    TABLE_SENSORS " ( " \
        COLUMN_ID                   " integer primary key not null, " \
        COLUMN_ADDRESS              " text, " \
        COLUMN_TYPE                 " integer, " \
        COLUMN_LAST_COMM            " integer, " \
        COLUMN_TEMPERATURE          " real, " \
        COLUMN_BATTERY_LEVEL        " integer );"

#define CREATE_TABLE_TEMPERATURE "create table " \
    TABLE_TEMPERATURE " ( " \
        COLUMN_SENSOR               " integer not null, " \
        COLUMN_TIMESTAMP            " integer not null, " \
        COLUMN_TEMPERATURE          " real, " \
        COLUMN_SENSORPOINT          " integer, " \
        "primary key (" COLUMN_SENSOR "," COLUMN_TIMESTAMP ") );"

//
// insert table sql
//

#define INSERT_INTO_CONFIG "insert into " TABLE_CONFIG " " \
    "(" COLUMN_TYPE "," COLUMN_VALUE ") values " \
    "( '" CONFIG_DB_VERSION "', '" STRINGIFY(SENSORDATA_DB_VERSION) "' ), " \
    "( '" CONFIG_TEMP_UNIT_OF_MEASURE "', '0' ), " \
    "( '" CONFIG_MAX_TEMP_AGE "', '28' ), " \
    "( '" CONFIG_BATTERY_WARNING_FROM "', '10' ), " \
    "( '" CONFIG_BATTERY_NORMAL_FROM "', '25' ), " \
    "( '" CONFIG_COMM_WARNING_FROM "', '240' ), " \
    "( '" CONFIG_COMM_NORMAL_FROM "', '60' );"

#define INSERT_INTO_PROFILES "insert into " TABLE_PROFILES " " \
    "(" COLUMN_ID "," COLUMN_DESCRIPTION "," COLUMN_TEMP_WARNING_FROM "," COLUMN_TEMP_NORMAL_FROM "," COLUMN_TEMP_NORMAL_TO "," COLUMN_TEMP_WARNING_TO "," COLUMN_TEMP_STANDARD ") values " \
    "( 0, 'Room', -51.1111, -40.0, 37.7778, 48.8889, 23.8889 ), " \
    "( 1, 'Frozen', -30.0, -25.0, -16.6667, -10.0, -17.7778 ), " \
    "( 2, 'Refrigerator', -1.11111, 0.0, 5, 7.22222, 4.44444 ), " \
    "( 3, 'Door', -51.1111, -40.0, 37.7778, 48.8889, 23.8889 ), " \
    "( 4, 'Produce', 0.0, 1.11111, 12.2222, 20.0, 6.11111 ), " \
    "( 5, 'Hot Holding', 57.2222, 60.0, 73.8889, 76.6667, 65.5556 ), " \
    "( 6, 'Live Seafood', 0.0, 1.11111, 7.22222, 8.88889, 1.66667 ), " \
    "( 7, 'Ice Cream', -35.0, -32.2222, -27.7778, -23.2778, -28.8889 ), " \
    "( 8, 'Dairy', -1.11111, 0.0, 5.0, 7.22222, 4.388889 ), " \
    "( 9, 'Eggs', -1.11111, 0.0, 7.77778, 8.88889, 7.22222 );"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBTABLES
