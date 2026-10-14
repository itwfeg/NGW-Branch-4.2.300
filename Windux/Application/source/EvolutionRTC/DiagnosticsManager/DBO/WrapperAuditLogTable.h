/**
 * @file WrapperAuditLogTable.h
 * @brief WrapperAuditLog table and column definitions.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOWRAPPERAUDITLOGTABLE_H
#define DBOWRAPPERAUDITLOGTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_WRAPPERAUDITLOG "wrapperauditlog"

// columns
#define COLUMN_DESCRIPTION "description"
#define COLUMN_SEVERITY "severity"
#define COLUMN_SOURCE "source"
#define COLUMN_SOURCE_EXT "sourceExt"
#define COLUMN_TIMESTAMP "timestamp"

// value types

#define SOURCE_GENERIC 0
#define SOURCE_TIMINGS 1
#define SOURCE_FILM 2
#define SOURCE_SERVICE_VISIT 99

#define EVENT_SEVERITY_UNKNOWN 0
#define EVENT_SEVERITY_INFO 1
#define EVENT_SEVERITY_WARNING 2
#define EVENT_SEVERITY_ERROR 3
#define EVENT_SEVERITY_CRITICAL 4

//
// create table sql
//

#define CREATE_TABLE_WRAPPERAUDITLOG "create table " \
    TABLE_WRAPPERAUDITLOG " ( " \
        COLUMN_TIMESTAMP            " integer primary key not null, " \
        COLUMN_SEVERITY             " integer, " \
        COLUMN_SOURCE               " integer, " \
        COLUMN_SOURCE_EXT           " integer, " \
		COLUMN_DESCRIPTION          " string );"

//
// export table sql
//

#define EXPORT_TABLE_WRAPPERAUDITLOG "insert into newdb." \
	TABLE_WRAPPERAUDITLOG " (" \
        COLUMN_TIMESTAMP ", " \
        COLUMN_SEVERITY ", " \
        COLUMN_SOURCE ", " \
        COLUMN_SOURCE_EXT ", " \
		COLUMN_DESCRIPTION ") SELECT " \
        COLUMN_TIMESTAMP ", " \
        COLUMN_SEVERITY ", " \
        COLUMN_SOURCE ", " \
        COLUMN_SOURCE_EXT ", " \
		COLUMN_DESCRIPTION " FROM " TABLE_WRAPPERAUDITLOG ";"

//
// insert sql
//

#define INSERT_WRAPPERAUDITLOG "insert into " \
	TABLE_WRAPPERAUDITLOG " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_SEVERITY ", " \
		COLUMN_SOURCE ", " \
		COLUMN_SOURCE_EXT ", " \
		COLUMN_DESCRIPTION ") " \
		"values (?1, ?2, ?3, ?4, ?5);"

//
// update sql
//

#define UPDATE_WRAPPERAUDITLOG "update " \
	TABLE_WRAPPERAUDITLOG " set " \
		COLUMN_SEVERITY "=?2, " \
		COLUMN_SOURCE "=?3, " \
		COLUMN_SOURCE_EXT "=?4, " \
		COLUMN_DESCRIPTION "=?5 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOWRAPPERAUDITLOGTABLE_H
