/**
 * @file EventLog.cpp
 * @brief EventLog table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "EventLog.h"
#include "EventLogTable.h"
#include "Wizards.h"

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
int EventLog::dataType_ = 0;
std::string EventLog::dataTypeName_( "rtc.diagnosticsmanager.dbo.eventlog" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.eventlog", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EventLogRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 1, timestamp )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, device )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, deviceExt )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, deviceCycles )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 5, errorCode )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 6, severity )) )
	{
		TRACE( "error during bind (6): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 7, (wizard ? 1 : 0) )) )
	{
		TRACE( "error during bind (7): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EventLogRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	device = sqlite3_column_int( stmt, 1 );
	deviceExt = sqlite3_column_int( stmt, 2 );
	deviceCycles = sqlite3_column_int( stmt, 3 );
	errorCode = sqlite3_column_int( stmt, 4 );
	severity = (severityType) sqlite3_column_int( stmt, 5 );
	wizard = (0 != sqlite3_column_int( stmt, 6 ));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EventLogRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );
	d.append( "device", util_string::longtostring( device ) );
	d.append( "deviceExt", util_string::longtostring( deviceExt ) );
	d.append( "deviceCycles", util_string::longtostring( deviceCycles ) );
	d.append( "errorCode", util_string::longtostring( errorCode ) );
	d.append( "severity", util_string::longtostring( severity ) );
	d.append( "wizard", util_string::longtostring( wizard ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
EventLog::EventLog( sqlite3 *db ) :
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
EventLog::~EventLog()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
EventLog::timestamp_type EventLog::create( int device, int deviceExt, unsigned int deviceCycles, int errorCode, bool forceWizard, row::severityType severity )
{
	row r;

	// create record
	r.timestamp = timestamp_type::now();
	r.device = device;
	r.deviceExt = deviceExt;
	r.deviceCycles = deviceCycles;
	r.errorCode = errorCode;
	r.severity = severity;
	r.wizard = false;

	// check for wizard available for this event
	Wizards wizardsTable( db_ );
	Wizards::row wizardsRow;

	if ( !wizardsTable.retrieve( device, deviceExt, errorCode, wizardsRow ) )
		TRACE( "no diagnostic wizard available for event" );
	else
	{
		TRACE( "diagnostic wizard available: " + util_string::longtostring( wizardsRow.wizard ) );

		if ( row::Unknown == severity )
			r.severity = (row::severityType) wizardsRow.severity;

		r.wizard = wizardsRow.isWizard();
	}

	// check for forced wizard
	if (( forceWizard ) && ( !r.wizard ))
	{
		TRACE( "forcing wizard for event" );
		r.wizard = true;
	}

	TRACE( "creating new event:\n" + formatRow( r ) );

	if ( !insert( r ) )
		return 0;

	return r.timestamp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EventLog::queryByAge( unsigned int age, rows &results )
{
	// compute timestamp for query
	int value( age );
	value *= -1;

	timestamp_type timestamp( timestamp_type::now() );
	timestamp.addMinutes( value );

	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_EVENTLOG " WHERE ("
			COLUMN_TIMESTAMP ">" + timestamp.toString() + ") "
			"ORDER BY " COLUMN_TIMESTAMP " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EventLog::queryByDeviceCycles( unsigned int min, unsigned int max, rows &results )
{
	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_EVENTLOG " WHERE ("
			"(" COLUMN_DEVICE_CYCLES ">=" + util_string::longtostring( min ) + ") AND"
			"(" COLUMN_DEVICE_CYCLES "<=" + util_string::longtostring( max ) + ")) "
			"ORDER BY " COLUMN_TIMESTAMP " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EventLog::retrieve( const timestamp_type &timestamp, row &result )
{
	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_EVENTLOG " WHERE ("
			COLUMN_TIMESTAMP "=" + timestamp.toString() + ");" );

	// go!
	return select( s, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EventLog::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	const std::string s( "DELETE FROM " TABLE_EVENTLOG " WHERE ("
		COLUMN_TIMESTAMP "<" + timestamp.toString() + ");" );

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
std::string TableBase<EventLogRow>::createTableStmt()
{
	return CREATE_TABLE_EVENTLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<EventLogRow>::exportTableStmt()
{
	return EXPORT_TABLE_EVENTLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<EventLogRow>::insertStmt()
{
	return INSERT_EVENTLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<EventLogRow>::updateStmt()
{
	return UPDATE_EVENTLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::EventLogRow>;
