/**
 * @file ConfigTable.h
 * @brief Config table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOCONFIGTABLE_H
#define DBOCONFIGTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

#define DB_VERSION 11

#define DEFAULT_CONFIG_EDGE_INIT 240
#define DEFAULT_CONFIG_EDGE_POWER_UP 300
#define DEFAULT_CONFIG_EDGE_STATUS_CHECK 3600
#define DEFAULT_CONFIG_HOUSEKEEPING 180
#define DEFAULT_CONFIG_VALUE_MAX_SAVINGS 0.0
#define DEFAULT_CONFIG_WRAPPER_LOWER_FILM_WIDTH 0
#define DEFAULT_CONFIG_WRAPPER_UPPER_FILM_WIDTH 0

// tables
#define TABLE_CONFIG "config"

// columns
#define COLUMN_NAME "name"
#define COLUMN_TYPE "type"
#define COLUMN_VALUE "value"

// value types

#define VALUE_TYPE_INTEGER 0
#define VALUE_TYPE_STRING 1
#define VALUE_TYPE_REAL 2
#define VALUE_TYPE_LONGLONG 3

// configuration keys
#define CONFIG_DB_VERSION "db_version"                                  // integer
#define CONFIG_CREATED "created"                                        // timestamp (ms since epoch)

#define CONFIG_EDGE_INIT "edge_init"                          		    // when to init edge device comms (sec after boot)
#define CONFIG_EDGE_POWER_UP "edge_power_up"                            // when to send power up message (sec after boot)
#define CONFIG_EDGE_STATUS_CHECK "edge_status_check"                    // how often to send status check message (sec)
#define CONFIG_HOUSEKEEPING "housekeeping"                              // how long to keep records (days)
#define CONFIG_VALUE_MAX_SAVINGS "value_max_savings"                    // how much value max technology has saved
#define CONFIG_WRAPPER_LOWER_FILM_WIDTH "wrapper_lower_film_width"  	// lower film width for wrapper (tenth of inch)
#define CONFIG_WRAPPER_UPPER_FILM_WIDTH "wrapper_upper_film_width"  	// upper film width for wrapper (tenth of inch)


//
// create table sql
//

#define CREATE_TABLE_CONFIG "create table " \
    TABLE_CONFIG " ( " \
        COLUMN_NAME                 " text primary key not null, " \
        COLUMN_TYPE                 " integer, " \
        COLUMN_VALUE                " text );"

//
// populate table sql
//

#define POPULATE_TABLE_CONFIG "insert into " TABLE_CONFIG " " \
    "(" COLUMN_NAME "," COLUMN_TYPE "," COLUMN_VALUE ") values " \
    "( '" CONFIG_DB_VERSION "', " STRINGIFY(VALUE_TYPE_INTEGER) ", '" STRINGIFY(DB_VERSION) "' ), " \
    "( '" CONFIG_CREATED "', " STRINGIFY(VALUE_TYPE_LONGLONG) ", '%" CONFIG_CREATED "%' ), " \
    "( '" CONFIG_EDGE_INIT "', " STRINGIFY(VALUE_TYPE_INTEGER) ", '" STRINGIFY(DEFAULT_CONFIG_EDGE_INIT) "' ), " \
    "( '" CONFIG_EDGE_POWER_UP "', " STRINGIFY(VALUE_TYPE_INTEGER) ", '" STRINGIFY(DEFAULT_CONFIG_EDGE_POWER_UP) "' ), " \
    "( '" CONFIG_EDGE_STATUS_CHECK "', " STRINGIFY(VALUE_TYPE_INTEGER) ", '" STRINGIFY(DEFAULT_CONFIG_EDGE_STATUS_CHECK) "' ), " \
    "( '" CONFIG_HOUSEKEEPING "', " STRINGIFY(VALUE_TYPE_INTEGER) ", '" STRINGIFY(DEFAULT_CONFIG_HOUSEKEEPING) "' ), " \
    "( '" CONFIG_VALUE_MAX_SAVINGS "', " STRINGIFY(VALUE_TYPE_REAL) ", '" STRINGIFY(DEFAULT_CONFIG_VALUE_MAX_SAVINGS) "' ), " \
    "( '" CONFIG_WRAPPER_LOWER_FILM_WIDTH "', " STRINGIFY(VALUE_TYPE_INTEGER) ", '" STRINGIFY(DEFAULT_CONFIG_WRAPPER_LOWER_FILM_WIDTH) "' ), " \
    "( '" CONFIG_WRAPPER_UPPER_FILM_WIDTH "', " STRINGIFY(VALUE_TYPE_INTEGER) ", '" STRINGIFY(DEFAULT_CONFIG_WRAPPER_UPPER_FILM_WIDTH) "' ); "

//
// insert sql
//

#define INSERT_CONFIG "insert into " \
	TABLE_CONFIG " (" \
		COLUMN_NAME ", " \
		COLUMN_TYPE ", " \
		COLUMN_VALUE ") " \
		"values (?1, ?2, ?3);"

//
// update sql
//

#define UPDATE_CONFIG "update " \
	TABLE_CONFIG " set " \
		COLUMN_TYPE "=?2, " \
		COLUMN_VALUE "=?3 " \
		"where " COLUMN_NAME "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOCONFIGTABLE_H
