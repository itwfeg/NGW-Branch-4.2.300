/**
 * @file ServiceCatTable.h
 * @brief Service categories table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOSERVICECATTABLE_H
#define DBOSERVICECATTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_SERVICECAT "servicecat"

// columns
#define COLUMN_DESCRIPTION "description"
#define COLUMN_ID "id"


//
// create table sql
//

#define CREATE_TABLE_SERVICECAT "create table " \
    TABLE_SERVICECAT " ( " \
        COLUMN_ID                   " integer primary key not null, " \
		COLUMN_DESCRIPTION          " string );"

//
// populate table sql
//

#define POPULATE_TABLE_SERVICECAT "insert into " TABLE_SERVICECAT " " \
    "(" COLUMN_ID "," COLUMN_DESCRIPTION ") values " \
    "( 300, 'Prepack' ), " \
    "( 400, 'Service Counter' ); "

//
// insert sql
//

#define INSERT_SERVICECAT "insert into " \
	TABLE_SERVICECAT " (" \
		COLUMN_ID "," \
		COLUMN_DESCRIPTION ") " \
		"values ( ?1, ?2 );"

//
// update sql
//

#define UPDATE_SERVICECAT "update " \
	TABLE_SERVICECAT " set " \
		COLUMN_DESCRIPTION "=?2 " \
		"where " COLUMN_ID "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOSERVICECATTABLE_H
