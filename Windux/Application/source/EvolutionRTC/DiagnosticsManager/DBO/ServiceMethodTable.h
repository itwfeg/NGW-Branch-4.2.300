/**
 * @file ServiceMethodTable.h
 * @brief Service method table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOSERVICEMETHODTABLE_H
#define DBOSERVICEMETHODTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_SERVICEMETHOD "servicemethod"

// columns
#define COLUMN_DESCRIPTION "description"
#define COLUMN_ID "id"


//
// create table sql
//

#define CREATE_TABLE_SERVICEMETHOD "create table " \
    TABLE_SERVICEMETHOD " ( " \
        COLUMN_ID                   " integer primary key not null, " \
		COLUMN_DESCRIPTION          " string );"

//
// populate table sql
//

#define POPULATE_TABLE_SERVICEMETHOD "insert into " TABLE_SERVICEMETHOD " " \
    "(" COLUMN_ID "," COLUMN_DESCRIPTION ") values " \
    "(  1, 'Adjust / Reform (Mech)' ), " \
    "(  2, 'Anchor / Secure' ), " \
    "(  3, 'Calibrate' ), " \
    "(  4, 'Certify / Seal' ), " \
    "(  5, 'Check / Test' ), " \
    "(  6, 'Clean' ), " \
    "(  7, 'Drain Condensate' ), " \
    "(  8, 'Configure / Program' ), " \
    "(  9, 'Cycle Power / Reboot' ), " \
    "( 10, 'Level' ), " \
    "( 11, 'Lubricate' ), " \
    "( 12, 'Modify' ), " \
    "( 13, 'Train Operator' ), " \
    "( 14, 'Reload' ), " \
    "( 15, 'Repaired' ), " \
    "( 16, 'Replace' ), " \
    "( 17, 'Reseat' ), " \
    "( 18, 'Shim' ), " \
    "( 19, 'Solder / Weld' ), " \
    "( 20, 'Tighten' ), " \
    "( 21, 'Update' ), " \
    "( 22, 'Wired' ), " \
    "( 23, 'Other' ); "

//
// insert sql
//

#define INSERT_SERVICEMETHOD "insert into " \
	TABLE_SERVICEMETHOD " (" \
		COLUMN_ID "," \
		COLUMN_DESCRIPTION ") " \
		"values ( ?1, ?2 );"

//
// update sql
//

#define UPDATE_SERVICEMETHOD "update " \
	TABLE_SERVICEMETHOD " set " \
		COLUMN_DESCRIPTION "=?2 " \
		"where " COLUMN_ID "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOSERVICEMETHODTABLE_H
