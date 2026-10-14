/**
 * @file EventLogTable.h
 * @brief EventLog table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOEVENTLOGTABLE_H
#define DBOEVENTLOGTABLE_H

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_EVENTLOG "eventlog"

// columns
#define COLUMN_DEVICE "device"
#define COLUMN_DEVICE_CYCLES "deviceCycles"
#define COLUMN_DEVICE_EXT "deviceExt"
#define COLUMN_ERROR_CODE "errorCode"
#define COLUMN_SEVERITY "severity"
#define COLUMN_TIMESTAMP "timestamp"
#define COLUMN_WIZARD "wizard"

// value types

#define EVENT_SEVERITY_UNKNOWN 0
#define EVENT_SEVERITY_INFO 1
#define EVENT_SEVERITY_WARNING 2
#define EVENT_SEVERITY_ERROR 3
#define EVENT_SEVERITY_CRITICAL 4


//
// create table sql
//

#define CREATE_TABLE_EVENTLOG "create table " \
    TABLE_EVENTLOG " ( " \
        COLUMN_TIMESTAMP            " integer primary key not null, " \
        COLUMN_DEVICE               " integer, " \
        COLUMN_DEVICE_EXT           " integer, " \
        COLUMN_DEVICE_CYCLES        " integer, " \
        COLUMN_ERROR_CODE           " integer, " \
        COLUMN_SEVERITY             " integer, " \
        COLUMN_WIZARD           	" integer );"

//
// export table sql
//

#define EXPORT_TABLE_EVENTLOG "insert into newdb." \
	TABLE_EVENTLOG " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_DEVICE ", " \
		COLUMN_DEVICE_EXT ", " \
		COLUMN_DEVICE_CYCLES ", " \
		COLUMN_ERROR_CODE ", " \
		COLUMN_SEVERITY ", " \
		COLUMN_WIZARD ") SELECT " \
		COLUMN_TIMESTAMP ", " \
		COLUMN_DEVICE ", " \
		COLUMN_DEVICE_EXT ", " \
		COLUMN_DEVICE_CYCLES ", " \
		COLUMN_ERROR_CODE ", " \
		COLUMN_SEVERITY ", " \
		COLUMN_WIZARD " FROM " TABLE_EVENTLOG ";"

//
// insert sql
//

#define INSERT_EVENTLOG "insert into " \
	TABLE_EVENTLOG " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_DEVICE ", " \
		COLUMN_DEVICE_EXT ", " \
		COLUMN_DEVICE_CYCLES ", " \
		COLUMN_ERROR_CODE ", " \
		COLUMN_SEVERITY ", " \
		COLUMN_WIZARD ") " \
		"values (?1, ?2, ?3, ?4, ?5, ?6, ?7);"

//
// update sql
//

#define UPDATE_EVENTLOG "update " \
	TABLE_EVENTLOG " set " \
		COLUMN_DEVICE "=?2, " \
		COLUMN_DEVICE_EXT "=?3, " \
		COLUMN_DEVICE_CYCLES "=?4, " \
		COLUMN_ERROR_CODE "=?5, " \
		COLUMN_SEVERITY "=?6, " \
		COLUMN_WIZARD "=?7 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOEVENTLOGTABLE_H
