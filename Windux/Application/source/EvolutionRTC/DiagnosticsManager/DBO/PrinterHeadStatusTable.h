/**
 * @file PrinterHeadStatusTable.h
 * @brief PrinterHeadStatus table and column definitions.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOPRINTERHEADSTATUSTABLE_H
#define DBOPRINTERHEADSTATUSTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_PRINTERHEADSTATUS "printerheadstatus"

// columns
#define COLUMN_COUNTS "counts"
#define COLUMN_PRINTER_NUM "printerNum"
#define COLUMN_STATUS "status"
#define COLUMN_TIMESTAMP "timestamp"

//
// create table sql
//

#define CREATE_TABLE_PRINTERHEADSTATUS "create table " \
    TABLE_PRINTERHEADSTATUS " ( " \
        COLUMN_TIMESTAMP            " integer primary key not null, " \
        COLUMN_PRINTER_NUM          " integer, " \
        COLUMN_COUNTS               " integer, " \
        COLUMN_STATUS               " integer );"

//
// export table sql
//

#define EXPORT_TABLE_PRINTERHEADSTATUS "insert into newdb." \
	TABLE_PRINTERHEADSTATUS " (" \
        COLUMN_TIMESTAMP ", " \
        COLUMN_PRINTER_NUM ", " \
        COLUMN_COUNTS ", " \
        COLUMN_STATUS ") SELECT " \
        COLUMN_TIMESTAMP ", " \
        COLUMN_PRINTER_NUM ", " \
        COLUMN_COUNTS ", " \
        COLUMN_STATUS " FROM " TABLE_PRINTERHEADSTATUS ";"

//
// insert sql
//

#define INSERT_PRINTERHEADSTATUS "insert into " \
	TABLE_PRINTERHEADSTATUS " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_PRINTER_NUM ", " \
		COLUMN_COUNTS ", " \
		COLUMN_STATUS ") " \
		"values (?1, ?2, ?3, ?4);"

//
// update sql
//

#define UPDATE_PRINTERHEADSTATUS "update " \
	TABLE_PRINTERHEADSTATUS " set " \
		COLUMN_PRINTER_NUM "=?2, " \
		COLUMN_COUNTS "=?3, " \
		COLUMN_STATUS "=?4 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOPRINTERHEADSTATUSTABLE_H
