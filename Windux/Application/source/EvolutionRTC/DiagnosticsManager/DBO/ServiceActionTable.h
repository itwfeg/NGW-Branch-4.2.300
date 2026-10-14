/**
 * @file ServiceActionTable.h
 * @brief Service action table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOSERVICEACTIONTABLE_H
#define DBOSERVICEACTIONTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_SERVICEACTION "serviceaction"

// columns
#define COLUMN_CAT "cat"
#define COLUMN_METHOD "method"
#define COLUMN_ORDERING "ordering"
#define COLUMN_PART "part"


//
// create table sql
//

#define CREATE_TABLE_SERVICEACTION "create table " \
    TABLE_SERVICEACTION " ( " \
        COLUMN_CAT                  " integer not null, " \
        COLUMN_METHOD               " integer not null, " \
        COLUMN_PART           		" integer not null, " \
		COLUMN_ORDERING             " integer, " \
        "primary key (" COLUMN_CAT ", " COLUMN_METHOD ", " COLUMN_PART "));"

//
// populate table sql
//

#define POPULATE_TABLE_SERVICEACTION "insert into " TABLE_SERVICEACTION " " \
    "(" COLUMN_CAT "," COLUMN_METHOD "," COLUMN_PART "," COLUMN_ORDERING ") values " \
    "( 300,  1,   4,   0 ), " \
    "( 300,  1,   2,   0 ), " \
    "( 300,  1,   7,   0 ), " \
    "( 300,  1,   9,   0 ), " \
    "( 300,  1,  10,   0 ), " \
    "( 300,  1,  13,   0 ), " \
    "( 300,  1,  28,   0 ), " \
    "( 300,  1,  37,   0 ), " \
    "( 300,  1,  40,   0 ), " \
    "( 300,  1,  48,   0 ), " \
    "( 300,  1,  61,   0 ), " \
    "( 300,  1,  70,   0 ), " \
    "( 300,  1,  79,   0 ), " \
    "( 300,  1,  81,   0 ), " \
    "( 300,  1,  83,   0 ), " \
    "( 300,  1,  87,   0 ), " \
    "( 300,  1,  93,   0 ), " \
    "( 300,  1,  96,   0 ), " \
    "( 300,  3,  13,   0 ), " \
    "( 300,  3,  37,   0 ), " \
    "( 300,  3,  42,   0 ), " \
    "( 300,  3,  65,   0 ), " \
    "( 300,  3,  73,   0 ), " \
    "( 300,  3,  93,   0 ), " \
    "( 300,  6,  21,   0 ), " \
    "( 300,  6,  26,   0 ), " \
    "( 300,  6,  28,   0 ), " \
    "( 300,  6,  37,   0 ), " \
    "( 300,  6,  40,   0 ), " \
    "( 300,  6,  47,   0 ), " \
    "( 300,  6,  49,   0 ), " \
    "( 300,  6,  54,   0 ), " \
    "( 300,  6,  63,   0 ), " \
    "( 300,  6,  66,   0 ), " \
    "( 300,  6,  71,   0 ), " \
    "( 300,  6,  73,   0 ), " \
    "( 300,  6,  76,   0 ), " \
    "( 300,  6,  68,   0 ), " \
    "( 300,  6,  87,   0 ), " \
    "( 300,  6,  93,   0 ), " \
    "( 300,  6,  96,   0 ), " \
    "( 300,  8,  78,   0 ), " \
    "( 300,  8,  91,   0 ), " \
    "( 300,  9,  86,   0 ), " \
    "( 300,  7,  24,   0 ), " \
    "( 300,  7,  27,   0 ), " \
    "( 300,  7,  41,   0 ), " \
    "( 300,  7,  89,   0 ), " \
    "( 300,  7,  95,   0 ), " \
    "( 300, 10,  28,   0 ), " \
    "( 300, 10,  73,   0 ), " \
    "( 300, 11,   6,   0 ), " \
    "( 300, 11,  24,   0 ), " \
    "( 300, 11,  36,   0 ), " \
    "( 300, 11,  46,   0 ), " \
    "( 300, 11,  48,   0 ), " \
    "( 300, 11,  51,   0 ), " \
    "( 300, 11,  54,   0 ), " \
    "( 300, 11,  72,   0 ), " \
    "( 300, 13,  19,   0 ), " \
    "( 300, 13,  30,   0 ), " \
    "( 300, 15,  98,   0 ), " \
    "( 300, 15,  41,   0 ), " \
    "( 300, 16,  56,   0 ), " \
    "( 300, 18,   9,   0 ), " \
    "( 300, 18,  10,   0 ), " \
    "( 300, 20,   4,   0 ), " \
    "( 300, 20,   5,   0 ), " \
    "( 300, 20,   7,   0 ), " \
    "( 300, 20,   9,   0 ), " \
    "( 300, 20,  20,   0 ), " \
    "( 300, 20,  33,   0 ), " \
    "( 300, 20,  40,   0 ), " \
    "( 300, 20,  51,   0 ), " \
    "( 300, 20,  75,   0 ), " \
    "( 300, 20,  79,   0 ), " \
    "( 300, 20,  83,   0 ), " \
    "( 300, 21,  32,   0 ), " \
    "( 300, 21,  25,   0 ), " \
    "( 300, 21,  97,   0 ), " \
    "( 300, 23,  99,   0 ); "

//
// insert sql
//

#define INSERT_SERVICEACTION "insert into " \
	TABLE_SERVICEACTION " (" \
		COLUMN_CAT "," \
		COLUMN_METHOD "," \
		COLUMN_PART "," \
		COLUMN_ORDERING ") " \
		"values ( ?1, ?2, ?3, ?4 );"

//
// update sql
//

#define UPDATE_SERVICEACTION "update " \
	TABLE_SERVICEACTION " set " \
		COLUMN_ORDERING "=?4 " \
		"where (" COLUMN_CAT "=?1 and " COLUMN_METHOD "=?2 and " COLUMN_PART "=?3 );"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOSERVICEACTIONTABLE_H
