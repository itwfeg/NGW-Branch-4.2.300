/**
 * @file DiagTable.h
 * @brief Diag table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBODIAGTABLE_H
#define DBODIAGTABLE_H

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_DIAG "diag"

// columns
#define COLUMN_EVENTLOG "eventLog"
#define COLUMN_HARDWARE_CYCLES "hardwareCycles"
#define COLUMN_HARDWARE_MODEL "hardwareModel"
#define COLUMN_HARDWARE_SERIAL "hardwareSerial"
#define COLUMN_METHOD "method"
#define COLUMN_TIMESTAMP "timestamp"

// value types

#define METHOD_IGNORED 0
#define METHOD_MANUAL 1
#define METHOD_AUTOMATIC 2
#define METHOD_OPERATOR_INITIATED 3
#define METHOD_SERVICE_INITIATED 4

//
// create table sql
//

#define CREATE_TABLE_DIAG "create table " \
	TABLE_DIAG " ( " \
		COLUMN_TIMESTAMP            " integer primary key not null, " \
		COLUMN_EVENTLOG				" integer, " \
		COLUMN_METHOD				" integer, " \
        COLUMN_HARDWARE_MODEL       " string, " \
        COLUMN_HARDWARE_SERIAL      " string, " \
        COLUMN_HARDWARE_CYCLES      " integer );"

//
// export table sql
//

#define EXPORT_TABLE_DIAG "insert into newdb." \
	TABLE_DIAG " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_EVENTLOG ", " \
		COLUMN_METHOD ", " \
        COLUMN_HARDWARE_MODEL ", " \
        COLUMN_HARDWARE_SERIAL ", " \
        COLUMN_HARDWARE_CYCLES ") SELECT " \
		COLUMN_TIMESTAMP ", " \
		COLUMN_EVENTLOG ", " \
		COLUMN_METHOD ", " \
        COLUMN_HARDWARE_MODEL ", " \
        COLUMN_HARDWARE_SERIAL ", " \
        COLUMN_HARDWARE_CYCLES " FROM " TABLE_DIAG ";"

//
// insert sql
//

#define INSERT_DIAG "insert into " \
	TABLE_DIAG " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_EVENTLOG ", " \
		COLUMN_METHOD ", " \
		COLUMN_HARDWARE_MODEL ", " \
		COLUMN_HARDWARE_SERIAL ", " \
		COLUMN_HARDWARE_CYCLES ") values (?1, ?2, ?3, ?4, ?5, ?6);"

//
// update sql
//

#define UPDATE_DIAG "update " \
	TABLE_DIAG " SET " \
		COLUMN_EVENTLOG "=?2, " \
		COLUMN_METHOD "=?3, " \
		COLUMN_HARDWARE_MODEL "=?4, " \
		COLUMN_HARDWARE_SERIAL "=?5, " \
		COLUMN_HARDWARE_CYCLES "=?6 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBODIAGTABLE_H
