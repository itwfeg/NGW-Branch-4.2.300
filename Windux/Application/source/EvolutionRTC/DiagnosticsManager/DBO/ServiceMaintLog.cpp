/**
 * @file ServiceMaintLog.cpp
 * @brief ServiceMaintLog table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "ServiceMaintLog.h"
#include "ServiceMaintLogTable.h"
#include "Wizards.h"

#include <Debug.h>
#include <HobString.h>

#include <e_string/utilstring.h>

#include <sqlite3.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

/// Database objects namespace.
namespace dbo
{

#ifdef USE_ACCOUNTANT
int ServiceMaintLog.cpp::dataType_ = 0;
std::string ServiceMaintLog.cpp::dataTypeName_( "rtc.diagnosticsmanager.dbo.servicemaintlog" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.servicemaintlog", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLogRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 1, timestamp )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, errorCode )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, state )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, count )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 5, latestOccurrence )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 6, latestDiag )) )
	{
		TRACE( "error during bind (6): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 7, resolved )) )
	{
		TRACE( "error during bind (7): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 8, serviceMethod1 )) )
	{
		TRACE( "error during bind (8): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 9, servicePart1 )) )
	{
		TRACE( "error during bind (9): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 10, serviceMethod2 )) )
	{
		TRACE( "error during bind (10): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 11, servicePart2 )) )
	{
		TRACE( "error during bind (11): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 12, serviceMethod3 )) )
	{
		TRACE( "error during bind (12): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 13, servicePart3 )) )
	{
		TRACE( "error during bind (13): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 14, serviceMethod4 )) )
	{
		TRACE( "error during bind (14): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 15, servicePart4 )) )
	{
		TRACE( "error during bind (15): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_text( stmt, 16, serviceNotes.c_str(), -1, NULL )) )
	{
		TRACE( "error during bind (16): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLogRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	errorCode = sqlite3_column_int( stmt, 1 );
	state = (stateType) sqlite3_column_int( stmt, 2 );
	count = sqlite3_column_int( stmt, 3 );
	latestOccurrence = sqlite3_column_int64( stmt, 4 );
	latestDiag = sqlite3_column_int64( stmt, 5 );
	resolved = sqlite3_column_int64( stmt, 6 );
	serviceMethod1 = sqlite3_column_int( stmt, 7 );
	servicePart1 = sqlite3_column_int( stmt, 8 );
	serviceMethod2 = sqlite3_column_int( stmt, 9 );
	servicePart2 = sqlite3_column_int( stmt, 10 );
	serviceMethod3 = sqlite3_column_int( stmt, 11 );
	servicePart3 = sqlite3_column_int( stmt, 12 );
	serviceMethod4 = sqlite3_column_int( stmt, 13 );
	servicePart4 = sqlite3_column_int( stmt, 14 );
	serviceNotes = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 15 ) );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceMaintLogRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );
	d.append( "errorCode", util_string::longtostring( errorCode ) );
	d.append( "state", util_string::longtostring( state ) );
	d.append( "count", util_string::longtostring( count ) );
	d.appendString( "latestOccurrence", latestOccurrence.toString() );
	d.appendString( "latestOccurrenceDateTime", latestOccurrence.toDateTime() );
	d.appendString( "latestDiag", latestDiag.toString() );
	d.appendString( "latestDiagDateTime", latestDiag.toDateTime() );
	d.appendString( "resolved", resolved.toString() );
	d.appendString( "resolvedDateTime", resolved.toDateTime() );
	d.append( "serviceMethod1", util_string::longtostring( serviceMethod1 ) );
	d.append( "servicePart1", util_string::longtostring( servicePart1 ) );
	d.append( "serviceMethod2", util_string::longtostring( serviceMethod2 ) );
	d.append( "servicePart2", util_string::longtostring( servicePart2 ) );
	d.append( "serviceMethod3", util_string::longtostring( serviceMethod3 ) );
	d.append( "servicePart3", util_string::longtostring( servicePart3 ) );
	d.append( "serviceMethod4", util_string::longtostring( serviceMethod4 ) );
	d.append( "servicePart4", util_string::longtostring( servicePart4 ) );
	d.appendString( "serviceNotes", serviceNotes );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceMaintLog::ServiceMaintLog( sqlite3 *db ) :
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
ServiceMaintLog::~ServiceMaintLog()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::create( const timestamp_type &timestamp, int errorCode, const timestamp_type &diagStep )
{
	rows records;

	// check existing records before creating new
	if ( !queryByErrorCode( errorCode, records ) )
		return false;

	// open service record already exists, update it with this entry
	if (( 0 < records.size() ) && ( row::Open == records[0].state ))
	{
		++records[0].count;
		records[0].latestOccurrence = timestamp;

		if ( diagStep )
			records[0].latestDiag = diagStep;

		return update( records[0] );
	}

	row r;

	// create record
	r.timestamp = timestamp;
	r.errorCode = errorCode;
	r.state = row::Open;
	r.count = 1;
	r.latestOccurrence = timestamp;
	r.latestDiag = diagStep;
	r.resolved = 0;
	r.serviceMethod1 = r.servicePart1 = 0;
	r.serviceMethod2 = r.servicePart2 = 0;
	r.serviceMethod3 = r.servicePart3 = 0;
	r.serviceMethod4 = r.servicePart4 = 0;

	TRACE( "creating new maint record:\n" + formatRow( r ) );

	return insert( r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::queryByAge_OpenRecordsOnly( unsigned int age, rows &results )
{
	// compute timestamp for query
	timestamp_type timestamp( timestamp_type::now() );

	if ( !age )
		timestamp = 0;
	else
	{
		int value( age );
		value *= -1;

		timestamp.addDays( value );
	}

	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_SERVICEMAINTLOG " WHERE ("
		    "(" COLUMN_STATE "=" STRINGIFY(STATE_OPEN) ") AND ("
			"(" COLUMN_TIMESTAMP ">" + timestamp.toString() + ") OR "
			"(" COLUMN_LATEST_OCCURRENCE ">" + timestamp.toString() + "))) "
			"ORDER BY " COLUMN_LATEST_OCCURRENCE " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::queryByAge_ResolvedRecordsOnly( unsigned int age, rows &results )
{
	// compute timestamp for query
	timestamp_type timestamp( timestamp_type::now() );

	if ( !age )
		timestamp = 0;
	else
	{
		int value( age );
		value *= -1;

		timestamp.addDays( value );
	}

	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_SERVICEMAINTLOG " WHERE (("
		    "(" COLUMN_STATE "=" STRINGIFY(STATE_RESOLVED_SERVICE) ") OR "
    		"(" COLUMN_STATE "=" STRINGIFY(STATE_RESOLVED_DIAG) ")) AND "
			"(" COLUMN_RESOLVED ">" + timestamp.toString() + ")) "
			"ORDER BY " COLUMN_RESOLVED " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::queryByAge_AllRecords( unsigned int age, rows &results )
{
	// compute timestamp for query
	timestamp_type timestamp( timestamp_type::now() );

	if ( !age )
		timestamp = 0;
	else
	{
		int value( age );
		value *= -1;

		timestamp.addDays( value );
	}

	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_SERVICEMAINTLOG " WHERE ("
			COLUMN_TIMESTAMP ">" + timestamp.toString() + ") "
			"ORDER BY " COLUMN_LATEST_OCCURRENCE " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::queryByErrorCode( int errorCode, rows &results )
{
	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_SERVICEMAINTLOG " WHERE ("
			COLUMN_ERROR_CODE "=" + util_string::longtostring( errorCode ) + ") "
			"ORDER BY " COLUMN_TIMESTAMP " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::retrieve( const timestamp_type &timestamp, row &result )
{
	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_SERVICEMAINTLOG " WHERE ("
			COLUMN_TIMESTAMP "=" + timestamp.toString() + ");" );

	// go!
	return select( s, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::updateServiceInfo( const timestamp_type &timestamp, const serviceMethodPartList &serviceActions, const std::string &serviceNotes )
{
	row result;

	// find record
	if ( retrieve( timestamp, result ) )
	{
		unsigned int index( 0 );

		// set methods and parts
		for ( serviceMethodPartList::const_iterator i( serviceActions.begin() ); i != serviceActions.end(); ++i )
		{
			switch ( index++ )
			{
			case 0:
				result.serviceMethod1 = i->first;
				result.servicePart1 = i->second;
				break;
			case 1:
				result.serviceMethod2 = i->first;
				result.servicePart2 = i->second;
				break;
			case 2:
				result.serviceMethod3 = i->first;
				result.servicePart3 = i->second;
				break;
			case 3:
				result.serviceMethod4 = i->first;
				result.servicePart4 = i->second;
				break;
			default:
				break;
			}
		}

		// set notes
		result.serviceNotes = serviceNotes;

		// update record!
		return update( result );
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::updateState( const timestamp_type &timestamp, row::stateType newState )
{
	row result;

	// find record
	if ( retrieve( timestamp, result ) )
		if ( result.state < newState )
		{
			// set new state
			result.state = newState;
			result.resolved = timestamp_type::now();

			TRACE( "updating maint record:\n" + formatRow( result ) );

			return update( result );
		}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMaintLog::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	// setup query to archive off old records
	const std::string s( "UPDATE " TABLE_SERVICEMAINTLOG " "
		"SET " COLUMN_STATE "=" STRINGIFY(STATE_ARCHIVED) " WHERE ("
		"(" COLUMN_STATE "=" STRINGIFY(STATE_OPEN) ") AND "
		"(" COLUMN_TIMESTAMP "<=" + timestamp.toString() + ") AND "
		"(" COLUMN_LATEST_OCCURRENCE "<=" + timestamp.toString() + "));" );

	sqlite3_stmt *stmt( NULL );
	int result;

	// update table
	result = sqlite3_prepare_v2( db, s.c_str(), (int) s.length(), &stmt, NULL );

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
template <>
std::string TableBase<ServiceMaintLogRow>::createTableStmt()
{
	return CREATE_TABLE_SERVICEMAINTLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceMaintLogRow>::exportTableStmt()
{
	return EXPORT_TABLE_SERVICEMAINTLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceMaintLogRow>::insertStmt()
{
	return INSERT_SERVICEMAINTLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceMaintLogRow>::updateStmt()
{
	return UPDATE_SERVICEMAINTLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::ServiceMaintLogRow>;
