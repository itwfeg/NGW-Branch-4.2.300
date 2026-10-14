/**
 * @file PrinterDotUsageTable.h
 * @brief PrinterDotUsage table and column definitions.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOPRINTERDOTUSAGETABLE_H
#define DBOPRINTERDOTUSAGETABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_PRINTERDOTUSAGE "printerdotusage"

// columns
#define COLUMN_DOT_NUM "dotNum"
#define COLUMN_PRINTED "printed"
#define COLUMN_PRINTER_NUM "printerNum"
#define COLUMN_REMOVED "removed"
#define COLUMN_TIMESTAMP "timestamp"

//
// create table sql
//

#define CREATE_TABLE_PRINTERDOTUSAGE "create table " \
    TABLE_PRINTERDOTUSAGE " ( " \
        COLUMN_TIMESTAMP            " integer not null, " \
        COLUMN_PRINTER_NUM          " integer not null, " \
        COLUMN_DOT_NUM              " integer not null, " \
        COLUMN_PRINTED              " integer, " \
        COLUMN_REMOVED              " integer, " \
        "primary key (" COLUMN_TIMESTAMP ", " COLUMN_PRINTER_NUM ", " COLUMN_DOT_NUM "));"

//
// export table sql
//

#define EXPORT_TABLE_PRINTERDOTUSAGE "insert into newdb." \
	TABLE_PRINTERDOTUSAGE " (" \
        COLUMN_TIMESTAMP ", " \
        COLUMN_PRINTER_NUM ", " \
        COLUMN_DOT_NUM ", " \
        COLUMN_PRINTED ", " \
        COLUMN_REMOVED ") SELECT " \
        COLUMN_TIMESTAMP ", " \
        COLUMN_PRINTER_NUM ", " \
        COLUMN_DOT_NUM ", " \
        COLUMN_PRINTED ", " \
        COLUMN_REMOVED " FROM " TABLE_PRINTERDOTUSAGE ";"

//
// insert sql
//

#define INSERT_PRINTERDOTUSAGE "insert into " \
	TABLE_PRINTERDOTUSAGE " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_PRINTER_NUM ", " \
		COLUMN_DOT_NUM ", " \
		COLUMN_PRINTED ", " \
		COLUMN_REMOVED ") " \
		"values (?1, ?2, ?3, ?4, ?5);"

//
// update sql
//

#define UPDATE_PRINTERDOTUSAGE "update " \
	TABLE_PRINTERDOTUSAGE " set " \
		COLUMN_PRINTER_NUM "=?2, " \
		COLUMN_DOT_NUM "=?3, " \
		COLUMN_PRINTED "=?4, " \
		COLUMN_REMOVED "=?5 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOPRINTERDOTUSAGETABLE_H
