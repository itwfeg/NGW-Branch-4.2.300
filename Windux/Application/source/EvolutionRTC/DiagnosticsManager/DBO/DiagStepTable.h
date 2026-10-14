/**
 * @file DiagStepTable.h
 * @brief DiagStep table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBODIAGSTEPTABLE_H
#define DBODIAGSTEPTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_DIAGSTEP "diagstep"

// columns
#define COLUMN_DIAG "diag"
#define COLUMN_DIAG_COMPLETE_TIMESTAMP "diagCompleteTimestamp"
#define COLUMN_DIAG_TIMESTAMP "diagTimestamp"
#define COLUMN_DIAG_RESULT "diagResult"
#define COLUMN_ERROR_CODE "errorCode"
#define COLUMN_ROUTINE "routine"
#define COLUMN_ROUTINE_ACTUAL_RESULT "routineActualResult"
#define COLUMN_ROUTINE_EXPECTED_RESULT "routineExpectedResult"
#define COLUMN_STATE "state"
#define COLUMN_TIMESTAMP "timestamp"

// value types

#define STATE_NEW 0
#define STATE_IN_PROGRESS 1
#define STATE_ABORTED 2
#define STATE_CANCELLED 3
#define STATE_PASS 4
#define STATE_FAIL 5
#define STATE_FAIL_TIMEOUT 6


//
// create table sql
//

#define CREATE_TABLE_DIAGSTEP "create table " \
	TABLE_DIAGSTEP " ( " \
		COLUMN_TIMESTAMP            	" integer primary key not null, " \
		COLUMN_DIAG			          	" integer, " \
		COLUMN_ERROR_CODE          		" integer, " \
		COLUMN_STATE					" integer, " \
		COLUMN_DIAG_TIMESTAMP			" integer, " \
		COLUMN_DIAG_COMPLETE_TIMESTAMP	" integer, " \
		COLUMN_DIAG_RESULT				" integer, " \
		COLUMN_ROUTINE					" integer, " \
		COLUMN_ROUTINE_EXPECTED_RESULT	" integer, " \
		COLUMN_ROUTINE_ACTUAL_RESULT	" integer );"

//
// export table sql
//

#define EXPORT_TABLE_DIAGSTEP "insert into newdb." \
	TABLE_DIAGSTEP " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_DIAG ", " \
		COLUMN_ERROR_CODE ", " \
		COLUMN_STATE ", " \
		COLUMN_DIAG_TIMESTAMP ", " \
		COLUMN_DIAG_COMPLETE_TIMESTAMP ", " \
		COLUMN_DIAG_RESULT ", " \
		COLUMN_ROUTINE ", " \
		COLUMN_ROUTINE_EXPECTED_RESULT ", " \
		COLUMN_ROUTINE_ACTUAL_RESULT ") SELECT " \
		COLUMN_TIMESTAMP ", " \
		COLUMN_DIAG ", " \
		COLUMN_ERROR_CODE ", " \
		COLUMN_STATE ", " \
		COLUMN_DIAG_TIMESTAMP ", " \
		COLUMN_DIAG_COMPLETE_TIMESTAMP ", " \
		COLUMN_DIAG_RESULT ", " \
		COLUMN_ROUTINE ", " \
		COLUMN_ROUTINE_EXPECTED_RESULT ", " \
		COLUMN_ROUTINE_ACTUAL_RESULT " FROM " TABLE_DIAGSTEP ";"

//
// insert sql
//

#define INSERT_DIAGSTEP "insert into " \
	TABLE_DIAGSTEP " (" \
		COLUMN_TIMESTAMP ", " \
		COLUMN_DIAG ", " \
		COLUMN_ERROR_CODE ", " \
		COLUMN_STATE ", " \
		COLUMN_DIAG_TIMESTAMP ", " \
		COLUMN_DIAG_COMPLETE_TIMESTAMP ", " \
		COLUMN_DIAG_RESULT ", " \
		COLUMN_ROUTINE ", " \
		COLUMN_ROUTINE_EXPECTED_RESULT ", " \
		COLUMN_ROUTINE_ACTUAL_RESULT ") values (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10);"

//
// update sql
//

#define UPDATE_DIAGSTEP "update " \
	TABLE_DIAGSTEP " set " \
		COLUMN_DIAG "=?2, " \
		COLUMN_ERROR_CODE "=?3, " \
		COLUMN_STATE "=?4, " \
		COLUMN_DIAG_TIMESTAMP "=?5, " \
		COLUMN_DIAG_COMPLETE_TIMESTAMP "=?6, " \
		COLUMN_DIAG_RESULT "=?7, " \
		COLUMN_ROUTINE "=?8, " \
		COLUMN_ROUTINE_EXPECTED_RESULT "=?9, " \
		COLUMN_ROUTINE_ACTUAL_RESULT "=?10 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBODIAGSTEPTABLE_H
