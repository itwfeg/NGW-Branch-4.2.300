/**
 * @file DiagStep.cpp
 * @brief DiagStep table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "DiagStep.h"
#include "DiagStepTable.h"

#include <Debug.h>

#include <e_string/utilstring.h>

#include <sqlite3.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

/// Database objects namespace.
namespace dbo
{

#ifdef USE_ACCOUNTANT
int DiagStep::dataType_ = 0;
std::string DiagStep::dataTypeName_( "rtc.diagnosticsmanager.dbo.diagstep" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.diagstep", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagStepRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 1, timestamp )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 2, diag )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, errorCode )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, state )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 5, diagTimestamp )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 6, diagCompleteTimestamp )) )
	{
		TRACE( "error during bind (6): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 7, diagResult )) )
	{
		TRACE( "error during bind (7): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 8, routine )) )
	{
		TRACE( "error during bind (8): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 9, routineExpectedResult )) )
	{
		TRACE( "error during bind (9): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 10, routineActualResult )) )
	{
		TRACE( "error during bind (10): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagStepRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	diag = sqlite3_column_int64( stmt, 1 );
	errorCode = sqlite3_column_int( stmt, 2 );
	state = (stateType) sqlite3_column_int( stmt, 3 );
	diagTimestamp = sqlite3_column_int64( stmt, 4 );
	diagCompleteTimestamp = sqlite3_column_int64( stmt, 5 );
	diagResult = sqlite3_column_int( stmt, 6 );
	routine = sqlite3_column_int( stmt, 7 );
	routineExpectedResult = sqlite3_column_int( stmt, 8 );
	routineActualResult = sqlite3_column_int( stmt, 9 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagStepRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );
	d.appendString( "diag", diag.toString() );
	d.appendString( "diagDateTime", diag.toDateTime() );
	d.append( "errorCode", util_string::longtostring( errorCode ) );
	d.append( "state", util_string::longtostring( state ) );
	d.appendString( "diagTimestamp", diagTimestamp.toString() );
	d.appendString( "diagTimestampDateTime", diagTimestamp.toDateTime() );
	d.appendString( "diagCompleteTimestamp", diagCompleteTimestamp.toString() );
	d.appendString( "diagCompleteTimestampDateTime", diagCompleteTimestamp.toDateTime() );
	d.append( "diagResult", util_string::longtostring( diagResult ) );
	d.append( "routine", util_string::longtostring( routine ) );
	d.append( "routineExpectedResult", util_string::longtostring( routineExpectedResult ) );
	d.append( "routineActualResult", util_string::longtostring( routineActualResult ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagStep::DiagStep( sqlite3 *db ) :
	_Mybase( db )
{
	TRACE( "CTOR" );

#ifdef USE_ACCOUNTANT
	if ( !dataType )
		dataType_ = util_acc::registerDataType( dataTypeName_ );

	util_acc::addInstance( dataType_ );
#endif

	TRACE( "CTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagStep::~DiagStep()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagStep::abortUnfinishedSteps()
{
	std::string s;

	sqlite3_stmt *stmt( NULL );
	int result;

	// setup query to cancel new steps
	s = "UPDATE " TABLE_DIAGSTEP " "
		"SET " COLUMN_STATE "=" STRINGIFY(STATE_CANCELLED) " "
		"WHERE (" COLUMN_STATE "=" STRINGIFY(STATE_NEW) " );";

	// update table
	result = sqlite3_prepare_v2( db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE( "error during update prepare: " + util_string::longtostring( result ) );
		goto error_handler;
	}
	else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
	{
		TRACE( "error updating table: " + util_string::longtostring( result ) );
		goto error_handler;
	}

	// finalize
	sqlite3_finalize( stmt );

	// setup query to abort unfinished steps
	s = "UPDATE " TABLE_DIAGSTEP " "
		"SET " COLUMN_STATE "=" STRINGIFY(STATE_ABORTED) " "
		"WHERE (" COLUMN_STATE "=" STRINGIFY(STATE_IN_PROGRESS) ");";

	// update table
	result = sqlite3_prepare_v2( db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE( "error during update prepare: " + util_string::longtostring( result ) );
		goto error_handler;
	}
	else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
	{
		TRACE( "error updating table: " + util_string::longtostring( result ) );
		goto error_handler;
	}

	// finalize
	sqlite3_finalize( stmt );

	return true;

error_handler:

	if ( stmt )
		sqlite3_finalize( stmt );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagStep::timestamp_type DiagStep::create( const timestamp_type &diag, int errorCode )
{
	row r;

	// create record
	r.timestamp = timestamp_type::now();
	r.diag = diag;
	r.errorCode = errorCode;
	r.state = row::New;
	r.diagTimestamp = 0;
	r.diagCompleteTimestamp = 0;
	r.diagResult = 0;
	r.routine = 0;
	r.routineExpectedResult = 0;
	r.routineActualResult = 0;

	TRACE( "creating new diag step:\n" + formatRow( r ) );

	if ( !insert( r ) )
		return 0;

	return r.timestamp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagStep::queryByDiag( const timestamp_type &diag, rows &results )
{
	// setup query
	std::string s(
		"SELECT * FROM " TABLE_DIAGSTEP " WHERE ("
			COLUMN_DIAG "=" + diag.toString() + ") "
			"ORDER BY " COLUMN_TIMESTAMP " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagStep::retrieve( const timestamp_type &timestamp, row &result )
{
	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_DIAGSTEP " WHERE ("
			COLUMN_TIMESTAMP "=" + timestamp.toString() + ");" );

	//go!
	return select( s, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagStep::updateRoutineInfo( const timestamp_type &timestamp, int routine, int routineExpectedResult, int routineActualResult )
{
	row result;

	// find record
	if ( retrieve( timestamp, result ) )
	{
		// set routine information
		result.routine = routine;
		result.routineExpectedResult = routineExpectedResult;
		result.routineActualResult = routineActualResult;

		return update( result );
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagStep::updateState( const timestamp_type &timestamp, row::stateType newState, int diagResult )
{
	row result;

	// find record
	if ( retrieve( timestamp, result ) )
		if ( result.state < newState )
		{
			// set new state
			result.state = newState;

			// update timestamps when switching states
			if ( row::InProgress == newState )
			{
				result.diagTimestamp = timestamp_type::now();
			}
			else if (( row::Pass == newState ) || ( row::Fail == newState ) || ( row::FailTimeout == newState ))
			{
				result.diagCompleteTimestamp = timestamp_type::now();
				result.diagResult = diagResult;
			}

			TRACE( "updating step:\n" + formatRow( result ) );

			return update( result );
		}

	TRACE( "failed to update state diag step " + timestamp.toString() );
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagStep::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	const std::string s( "DELETE FROM " TABLE_DIAGSTEP " WHERE ("
		COLUMN_DIAG "<" + timestamp.toString() + ");" );

	sqlite3_stmt *stmt( NULL );
	int result;

	// setup query
	result = sqlite3_prepare_v2( db, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE( "error during housekeeping prepare: " + util_string::longtostring( result ) );
		goto error_handler;
	}
	else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
	{
		TRACE( "error housekeeping: " + util_string::longtostring( result ) );
		goto error_handler;
	}

	// finalize
	sqlite3_finalize( stmt );

	return true;

error_handler:

	if ( stmt )
		sqlite3_finalize( stmt );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<DiagStepRow>::createTableStmt()
{
	return CREATE_TABLE_DIAGSTEP;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<DiagStepRow>::exportTableStmt()
{
	return EXPORT_TABLE_DIAGSTEP;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<DiagStepRow>::insertStmt()
{
	return INSERT_DIAGSTEP;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<DiagStepRow>::updateStmt()
{
	return UPDATE_DIAGSTEP;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::DiagStepRow>;


