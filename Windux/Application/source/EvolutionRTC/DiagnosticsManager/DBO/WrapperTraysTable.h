/**
 * @file WrapperTraysTable.h
 * @brief Wrapper trays table and column definitions.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOWRAPPERTRAYSTABLE_H
#define DBOWRAPPERTRAYSTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_WRAPPERTRAYS "wrappertrays"

// columns
#define COLUMN_LENGTH_MAX "lengthMax"
#define COLUMN_LENGTH_MIN "lengthMin"
#define COLUMN_LIP "lip"
#define COLUMN_NAME "name"
#define COLUMN_PLU "plu"
#define COLUMN_WIDTH_MAX "widthMax"
#define COLUMN_WIDTH_MIN "widthMin"

//
// create table sql
//

#define CREATE_TABLE_WRAPPERTRAYS "create table " \
    TABLE_WRAPPERTRAYS " ( " \
        COLUMN_NAME				" string not null, " \
        COLUMN_PLU				" integer not null, " \
        COLUMN_LENGTH_MIN		" integer, " \
        COLUMN_LENGTH_MAX		" integer, " \
		COLUMN_WIDTH_MIN		" integer, " \
        COLUMN_WIDTH_MAX		" integer, " \
        COLUMN_LIP				" integer, " \
        "primary key (" COLUMN_NAME ", " COLUMN_PLU "));"

//
// export table sql
//

#define EXPORT_TABLE_WRAPPERTRAYS "insert into newdb." \
	TABLE_WRAPPERTRAYS " (" \
        COLUMN_NAME ", " \
        COLUMN_PLU ", " \
        COLUMN_LENGTH_MIN ", " \
        COLUMN_LENGTH_MAX ", " \
		COLUMN_WIDTH_MIN ", " \
        COLUMN_WIDTH_MAX ", " \
        COLUMN_LIP ") SELECT " \
        COLUMN_NAME ", " \
        COLUMN_PLU ", " \
        COLUMN_LENGTH_MIN ", " \
        COLUMN_LENGTH_MAX ", " \
		COLUMN_WIDTH_MIN ", " \
        COLUMN_WIDTH_MAX ", " \
        COLUMN_LIP " FROM " TABLE_WRAPPERTRAYS ";"

//
// insert sql
//

#define INSERT_WRAPPERTRAYS "insert into " \
	TABLE_WRAPPERTRAYS " (" \
		COLUMN_NAME ", " \
		COLUMN_PLU ", " \
		COLUMN_LENGTH_MIN ", " \
		COLUMN_LENGTH_MAX ", " \
		COLUMN_WIDTH_MIN ", " \
		COLUMN_WIDTH_MAX ", " \
		COLUMN_LIP ") " \
		"values (?1, ?2, ?3, ?4, ?5, ?6, ?7);"

//
// update sql
//

#define UPDATE_WRAPPERTRAYS "update " \
	TABLE_WRAPPERTRAYS " set " \
		COLUMN_LENGTH_MIN "=?3, " \
		COLUMN_LENGTH_MAX "=?4, " \
		COLUMN_WIDTH_MIN "=?5 " \
		COLUMN_WIDTH_MAX "=?6 " \
		COLUMN_LIP "=?7 " \
		"where (" COLUMN_NAME "=?1 and " COLUMN_PLU "=?2);"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOWRAPPERTRAYSTABLE_H
