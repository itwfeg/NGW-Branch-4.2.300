/**
 * @file WrapperAuditLog.cpp
 * @brief WrapperAuditLog table support.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "WrapperAuditLog.h"
#include "WrapperAuditLogTable.h"

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
std::string ServiceMaintLog.cpp::dataTypeName_( "rtc.diagnosticsmanager.dbo.wrapperauditlog" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.wrapperauditlog", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperAuditLogRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 1, timestamp )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, severity )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, source )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, sourceExt )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_text( stmt, 5, description.c_str(), -1, NULL )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperAuditLogRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	severity = (severityType) sqlite3_column_int( stmt, 1 );
	source = (sourceType) sqlite3_column_int( stmt, 2 );
	sourceExt = sqlite3_column_int( stmt, 3 );
	description = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 4 ) );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperAuditLogRow::retrieve( util::Dictionary &d ) const
{
	std::string sourceExtStr( "Unknown" );

	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );
	d.append( "severity", util_string::longtostring( severity ) );

	switch ( severity )
	{
	case Critical:
		d.appendString( "severityStr", "CRITICAL" );
		break;
	case Error:
		d.appendString( "severityStr", "ERROR" );
		break;
	case Warning:
		d.appendString( "severityStr", "WARNING" );
		break;
	case Info:
		d.appendString( "severityStr", "INFO" );
		break;
	default:
		break;
	}

	d.append( "source", util_string::longtostring( source ) );

	switch ( source )
	{
	case Unknown:
		d.appendString( "sourceStr", "Unknown" );
		break;
	case Timings:
		d.appendString( "sourceStr", "Timings" );
		if ( Timings_Reset == sourceExt )
			sourceExtStr = "Timings Reset";
		else if ( Timings_Modified == sourceExt )
			sourceExtStr = "Timings Modified";
		break;
	case Film:
		d.appendString( "sourceStr", "Film" );
		if ( Film_RollWidthModified == sourceExt )
			sourceExtStr = "Film Width(s) Modified";
		else if ( Film_RollChanged == sourceExt )
			sourceExtStr = "Film Roll Change";
		break;
	case ServiceVisit:
		d.appendString( "sourceStr", "Service" );
		if ( ServiceVisit_ResolveError == sourceExt )
			sourceExtStr = "Error Resolved";
		else if ( ServiceVisit_Start == sourceExt )
			sourceExtStr = "Service Visit Start";
		else if ( ServiceVisit_Stop == sourceExt )
			sourceExtStr = "Service Visit Complete";
		break;
	default:
		break;
	}

	d.append( "sourceExt", util_string::longtostring( sourceExt ) );

	d.appendString( "sourceExtStr", sourceExtStr );

	d.appendString( "description", description );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperAuditLog::WrapperAuditLog( sqlite3 *db ) :
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
WrapperAuditLog::~WrapperAuditLog()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperAuditLog::create( const timestamp_type &timestamp, int severity, int source, int sourceExt, const std::string &description )
{
	row r;

	// create record
	r.timestamp = timestamp;
	r.severity = (row::severityType) severity;
	r.source = (row::sourceType) source;
	r.sourceExt = sourceExt;
	r.description = description;

	TRACE( "creating new audit log record:\n" + formatRow( r ) );

	return insert( r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperAuditLog::queryByAge_FromSourceOnly( unsigned int age, int source, rows &results )
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
		"SELECT * FROM " TABLE_WRAPPERAUDITLOG " WHERE ("
		    "(" COLUMN_SOURCE "=" + util_string::longtostring( source ) + ") AND "
			"(" COLUMN_TIMESTAMP ">" + timestamp.toString() + "))"
			"ORDER BY " COLUMN_TIMESTAMP " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperAuditLog::queryByAge_AllRecords( unsigned int age, rows &results )
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
		"SELECT * FROM " TABLE_WRAPPERAUDITLOG " WHERE ("
			COLUMN_TIMESTAMP ">" + timestamp.toString() + ") "
			"ORDER BY " COLUMN_TIMESTAMP " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperAuditLog::retrieve( const timestamp_type &timestamp, row &result )
{
	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_WRAPPERAUDITLOG " WHERE ("
			COLUMN_TIMESTAMP "=" + timestamp.toString() + ");" );

	// go!
	return select( s, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperAuditLog::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	bool result( true );

	WrapperAuditLog auditLog( db );
	WrapperAuditLog::rows auditLogRows;

	// forcefully close out any service visit older than 1 day
	if ( auditLog.queryByAge_FromSourceOnly( 2, WrapperAuditLogRow::ServiceVisit, auditLogRows ) )
		for ( size_t i( 0 ); i < auditLogRows.size(); ++i )
		{
			// check for start event older than one day
			if ( WrapperAuditLogRow::ServiceVisit_Stop == auditLogRows[i].sourceExt )
			{
				// nothing to do, last event is stop event
				break;
			}
			else if ( WrapperAuditLogRow::ServiceVisit_Start == auditLogRows[i].sourceExt )
			{
				timestamp_type timestamp( timestamp_type::now() );
				timestamp.addDays( -1 );

				// this start event is older than one day and was not closed by a tech, lets close it
				if ( auditLogRows[i].timestamp < timestamp )
				{
					result = auditLog.create(
						timestamp_type::now(),
						dbo::WrapperAuditLogRow::Warning,
						dbo::WrapperAuditLogRow::ServiceVisit,
						dbo::WrapperAuditLogRow::ServiceVisit_Stop,
						"Service visit terminated.\n\nTechnician did not close out visit properly!" );
				}

				break;
			}
		}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperAuditLogRow>::createTableStmt()
{
	return CREATE_TABLE_WRAPPERAUDITLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperAuditLogRow>::exportTableStmt()
{
	return EXPORT_TABLE_WRAPPERAUDITLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperAuditLogRow>::insertStmt()
{
	return INSERT_WRAPPERAUDITLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperAuditLogRow>::updateStmt()
{
	return UPDATE_WRAPPERAUDITLOG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::WrapperAuditLogRow>;
