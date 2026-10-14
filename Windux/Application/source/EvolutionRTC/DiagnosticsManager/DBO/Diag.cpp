/**
 * @file Diag.cpp
 * @brief Diag table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "Diag.h"
#include "DiagStep.h"
#include "DiagTable.h"

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
int Diag::dataType_ = 0;
std::string Diag::dataTypeName_( "rtc.diagnosticsmanager.dbo.diag" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.diag", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 1, timestamp )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 2, eventLog )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, method )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_text( stmt, 4, hardwareModel.c_str(), -1, NULL )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_text( stmt, 5, hardwareSerial.c_str(), -1, NULL )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 6, hardwareCycles )) )
	{
		TRACE( "error during bind (6): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	eventLog = sqlite3_column_int64( stmt, 1 );
	method = (methodType) sqlite3_column_int( stmt, 2 );
	hardwareModel = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 3 ) );
	hardwareSerial = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 4 ) );
	hardwareCycles = sqlite3_column_int( stmt, 5 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );
	d.appendString( "eventLog", eventLog.toString() );
	d.appendString( "eventLogDateTime", eventLog.toDateTime() );
	d.append( "method", util_string::longtostring( method ) );
	d.appendString( "hardwareModel", hardwareModel );
	d.appendString( "hardwareSerial", hardwareSerial );
	d.append( "hardwareCycles", util_string::longtostring( hardwareCycles ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Diag::Diag( sqlite3 *db ) :
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
Diag::~Diag()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Diag::timestamp_type Diag::create( const timestamp_type &eventLog, row::methodType method )
{
	row r;

	// create record
	r.timestamp = timestamp_type::now();
	r.eventLog = eventLog;
	r.method = method;
	r.hardwareCycles = 0;

	TRACE( "creating new diag:\n" + formatRow( r ) );

	if ( !insert( r ) )
		return 0;

	return r.timestamp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Diag::queryByEventLog( const timestamp_type &eventLog, rows &results )
{
	// setup query
	std::string s(
		"SELECT * FROM " TABLE_DIAG " WHERE ("
			COLUMN_EVENTLOG "=" + eventLog.toString() + ") "
			"ORDER BY " COLUMN_TIMESTAMP " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Diag::retrieve( const timestamp_type &timestamp, row &result )
{
	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_DIAG " WHERE ("
			COLUMN_TIMESTAMP "=" + timestamp.toString() + ");" );

	// go!
	return select( s, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Diag::updateMethod( const timestamp_type &timestamp, row::methodType method, int errorCode, timestamp_type &diagStep )
{
	row result;

	// find record
	if ( retrieve( timestamp, result ) )
		if ( result.method < method )
		{
			// set new method
			result.method = method;

			TRACE( "updating method:\n" + formatRow( result ) );

			if ( update( result ) )
			{
				// generate initial diagnostic step
				DiagStep diagStepTable( db_ );

				if ( (diagStep = diagStepTable.create( timestamp, errorCode )) )
					return true;
			}
		}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Diag::updateHardwareInfo( const timestamp_type &timestamp, const std::string &hardwareModel, const std::string &hardwareSerial, unsigned int hardwareCycles )
{
	row result;

	// find record
	if ( retrieve( timestamp, result ) )
	{
		result.hardwareModel = hardwareModel;
		result.hardwareSerial = hardwareSerial;
		result.hardwareCycles = hardwareCycles;

		TRACE( "update hardware info:\n" + formatRow( result ) );

		return update( result );
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Diag::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	const std::string s( "DELETE FROM " TABLE_DIAG " WHERE ("
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
std::string TableBase<DiagRow>::createTableStmt()
{
	return CREATE_TABLE_DIAG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<DiagRow>::exportTableStmt()
{
	return EXPORT_TABLE_DIAG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<DiagRow>::insertStmt()
{
	return INSERT_DIAG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<DiagRow>::updateStmt()
{
	return UPDATE_DIAG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::DiagRow>;


