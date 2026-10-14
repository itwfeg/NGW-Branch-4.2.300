/**
 * @file ServiceMaintLogTable.h
 * @brief ServiceMaintLog table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOSERVICEMAINTLOGTABLE_H
#define DBOSERVICEMAINTLOGTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_SERVICEMAINTLOG "servicemaintlog"

// columns
#define COLUMN_COUNT "count"
#define COLUMN_ERROR_CODE "errorCode"
#define COLUMN_LATEST_DIAG "latestDiag"
#define COLUMN_LATEST_OCCURRENCE "latestOccurrence"
#define COLUMN_RESOLVED "resolved"
#define COLUMN_SERVICE_METHOD1 "serviceMethod1"
#define COLUMN_SERVICE_METHOD2 "serviceMethod2"
#define COLUMN_SERVICE_METHOD3 "serviceMethod3"
#define COLUMN_SERVICE_METHOD4 "serviceMethod4"
#define COLUMN_SERVICE_NOTES "serviceNotes"
#define COLUMN_SERVICE_PART1 "servicePart1"
#define COLUMN_SERVICE_PART2 "servicePart2"
#define COLUMN_SERVICE_PART3 "servicePart3"
#define COLUMN_SERVICE_PART4 "servicePart4"
#define COLUMN_STATE "state"
#define COLUMN_TIMESTAMP "timestamp"

// value types

#define STATE_OPEN 0
#define STATE_IGNORED 1
#define STATE_RESOLVED_SERVICE 2
#define STATE_RESOLVED_DIAG 3
#define STATE_ARCHIVED 4


//
// create table sql
//

#define CREATE_TABLE_SERVICEMAINTLOG "create table " \
    TABLE_SERVICEMAINTLOG " ( " \
        COLUMN_TIMESTAMP            " integer primary key not null, " \
        COLUMN_ERROR_CODE           " integer, " \
        COLUMN_STATE                " integer, " \
        COLUMN_COUNT                " integer, " \
        COLUMN_LATEST_OCCURRENCE    " integer, " \
        COLUMN_LATEST_DIAG			" integer, " \
        COLUMN_RESOLVED             " integer, " \
        COLUMN_SERVICE_METHOD1      " integer, " \
        COLUMN_SERVICE_PART1        " integer, " \
        COLUMN_SERVICE_METHOD2      " integer, " \
        COLUMN_SERVICE_PART2        " integer, " \
        COLUMN_SERVICE_METHOD3      " integer, " \
        COLUMN_SERVICE_PART3        " integer, " \
        COLUMN_SERVICE_METHOD4      " integer, " \
        COLUMN_SERVICE_PART4        " integer, " \
		COLUMN_SERVICE_NOTES        " string );"

//
// export table sql
//

#define EXPORT_TABLE_SERVICEMAINTLOG "insert into newdb." \
	TABLE_SERVICEMAINTLOG " (" \
        COLUMN_TIMESTAMP ", " \
        COLUMN_ERROR_CODE ", " \
        COLUMN_STATE ", " \
        COLUMN_COUNT ", " \
        COLUMN_LATEST_OCCURRENCE ", " \
        COLUMN_LATEST_DIAG ", " \
        COLUMN_RESOLVED ", " \
        COLUMN_SERVICE_METHOD1 ", " \
        COLUMN_SERVICE_PART1 ", " \
        COLUMN_SERVICE_METHOD2 ", " \
        COLUMN_SERVICE_PART2 ", " \
        COLUMN_SERVICE_METHOD3 ", " \
        COLUMN_SERVICE_PART3 ", " \
        COLUMN_SERVICE_METHOD4 ", " \
        COLUMN_SERVICE_PART4 ", " \
		COLUMN_SERVICE_NOTES ") SELECT " \
        COLUMN_TIMESTAMP ", " \
        COLUMN_ERROR_CODE ", " \
        COLUMN_STATE ", " \
        COLUMN_COUNT ", " \
        COLUMN_LATEST_OCCURRENCE ", " \
        COLUMN_LATEST_DIAG ", " \
        COLUMN_RESOLVED ", " \
        COLUMN_SERVICE_METHOD1 ", " \
        COLUMN_SERVICE_PART1 ", " \
        COLUMN_SERVICE_METHOD2 ", " \
        COLUMN_SERVICE_PART2 ", " \
        COLUMN_SERVICE_METHOD3 ", " \
        COLUMN_SERVICE_PART3 ", " \
        COLUMN_SERVICE_METHOD4 ", " \
        COLUMN_SERVICE_PART4 ", " \
		COLUMN_SERVICE_NOTES " FROM " TABLE_SERVICEMAINTLOG ";"

//
// insert sql
//

#define INSERT_SERVICEMAINTLOG "insert into " \
	TABLE_SERVICEMAINTLOG " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_ERROR_CODE ", " \
		COLUMN_STATE ", " \
		COLUMN_COUNT ", " \
		COLUMN_LATEST_OCCURRENCE ", " \
		COLUMN_LATEST_DIAG ", " \
		COLUMN_RESOLVED ", " \
		COLUMN_SERVICE_METHOD1 ", " \
		COLUMN_SERVICE_PART1 ", " \
		COLUMN_SERVICE_METHOD2 ", " \
		COLUMN_SERVICE_PART2 ", " \
		COLUMN_SERVICE_METHOD3 ", " \
		COLUMN_SERVICE_PART3 ", " \
		COLUMN_SERVICE_METHOD4 ", " \
		COLUMN_SERVICE_PART4 ", " \
		COLUMN_SERVICE_NOTES ") " \
		"values (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16);"

//
// update sql
//

#define UPDATE_SERVICEMAINTLOG "update " \
	TABLE_SERVICEMAINTLOG " set " \
		COLUMN_ERROR_CODE "=?2, " \
		COLUMN_STATE "=?3, " \
		COLUMN_COUNT "=?4, " \
		COLUMN_LATEST_OCCURRENCE "=?5, " \
		COLUMN_LATEST_DIAG "=?6, " \
		COLUMN_RESOLVED "=?7, " \
		COLUMN_SERVICE_METHOD1 "=?8, " \
		COLUMN_SERVICE_PART1 "=?9, " \
		COLUMN_SERVICE_METHOD2 "=?10, " \
		COLUMN_SERVICE_PART2 "=?11, " \
		COLUMN_SERVICE_METHOD3 "=?12, " \
		COLUMN_SERVICE_PART3 "=?13, " \
		COLUMN_SERVICE_METHOD4 "=?14, " \
		COLUMN_SERVICE_PART4 "=?15, " \
		COLUMN_SERVICE_NOTES "=?16 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOSERVICEMAINTLOGTABLE_H
