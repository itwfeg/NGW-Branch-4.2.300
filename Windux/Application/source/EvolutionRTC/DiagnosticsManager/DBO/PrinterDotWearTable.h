/**
 * @file PrinterDotWearTable.h
 * @brief PrinterDotWear table and column definitions.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOPRINTERDOTWEARTABLE_H
#define DBOPRINTERDOTWEARTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_PRINTERDOTWEAR "printerdotwear"

// columns
#define COLUMN_DOT_NUM "dotNum"
#define COLUMN_STATUS "status"
#define COLUMN_TIMESTAMP "timestamp"
#define COLUMN_VALUE "value"

//
// create table sql
//

#define CREATE_TABLE_PRINTERDOTWEAR "create table " \
    TABLE_PRINTERDOTWEAR " ( " \
        COLUMN_TIMESTAMP            " integer not null, " \
        COLUMN_DOT_NUM              " integer not null, " \
        COLUMN_STATUS               " integer, " \
        COLUMN_VALUE                " integer, " \
        "primary key (" COLUMN_TIMESTAMP ", " COLUMN_DOT_NUM "));"

//
// export table sql
//

#define EXPORT_TABLE_PRINTERDOTWEAR "insert into newdb." \
	TABLE_PRINTERDOTWEAR " (" \
        COLUMN_TIMESTAMP ", " \
        COLUMN_DOT_NUM ", " \
        COLUMN_STATUS ", " \
        COLUMN_VALUE ") SELECT " \
        COLUMN_TIMESTAMP ", " \
        COLUMN_DOT_NUM ", " \
        COLUMN_STATUS ", " \
        COLUMN_VALUE " FROM " TABLE_PRINTERDOTWEAR ";"

//
// insert sql
//

#define INSERT_PRINTERDOTWEAR "insert into " \
	TABLE_PRINTERDOTWEAR " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_DOT_NUM ", " \
		COLUMN_STATUS ", " \
		COLUMN_VALUE ") " \
		"values (?1, ?2, ?3, ?4);"

//
// update sql
//

#define UPDATE_PRINTERDOTWEAR "update " \
	TABLE_PRINTERDOTWEAR " set " \
		COLUMN_DOT_NUM "=?2, " \
		COLUMN_STATUS "=?3, " \
		COLUMN_VALUE "=?4 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOPRINTERDOTWEARTABLE_H
