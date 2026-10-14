/**
 * @file WrapperTimingsTable.h
 * @brief Wrapper height box and timings table and column definitions.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOWRAPPERTIMINGSTABLE_H
#define DBOWRAPPERTIMINGSTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_WRAPPERTIMINGS "wrappertimings"

// columns
#define COLUMN_HEIGHT "height"
#define COLUMN_NAME "name"
#define COLUMN_PARAM "param"
#define COLUMN_PARAM_VALUE "paramValue"
#define COLUMN_PLU "plu"

//
// create table sql
//

#define CREATE_TABLE_WRAPPERTIMINGS "create table " \
    TABLE_WRAPPERTIMINGS " ( " \
        COLUMN_NAME				" string not null, " \
        COLUMN_PLU				" integer not null, " \
        COLUMN_HEIGHT			" integer not null, " \
        COLUMN_PARAM			" integer not null, " \
		COLUMN_PARAM_VALUE		" integer, " \
        "primary key (" COLUMN_NAME ", " COLUMN_PLU ", " COLUMN_HEIGHT ", " COLUMN_PARAM "));"

//
// export table sql
//

#define EXPORT_TABLE_WRAPPERTIMINGS "insert into newdb." \
	TABLE_WRAPPERTIMINGS " (" \
        COLUMN_NAME ", " \
        COLUMN_PLU ", " \
        COLUMN_HEIGHT ", " \
        COLUMN_PARAM ", " \
		COLUMN_PARAM_VALUE ") SELECT " \
        COLUMN_NAME ", " \
        COLUMN_PLU ", " \
        COLUMN_HEIGHT ", " \
        COLUMN_PARAM ", " \
		COLUMN_PARAM_VALUE " FROM " TABLE_WRAPPERTIMINGS ";"

//
// insert sql
//

#define INSERT_WRAPPERTIMINGS "insert into " \
	TABLE_WRAPPERTIMINGS " (" \
		COLUMN_NAME ", " \
		COLUMN_PLU ", " \
		COLUMN_HEIGHT ", " \
		COLUMN_PARAM ", " \
		COLUMN_PARAM_VALUE ") " \
		"values (?1, ?2, ?3, ?4, ?5);"

//
// update sql
//

#define UPDATE_WRAPPERTIMINGS "update " \
	TABLE_WRAPPERTIMINGS " set " \
		COLUMN_PARAM_VALUE "=?5 " \
		"where (" COLUMN_NAME "=?1 and " COLUMN_PLU "=?2 and " COLUMN_HEIGHT "=?3 and " COLUMN_PARAM "=?4);"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOWRAPPERTIMINGSTABLE_H
