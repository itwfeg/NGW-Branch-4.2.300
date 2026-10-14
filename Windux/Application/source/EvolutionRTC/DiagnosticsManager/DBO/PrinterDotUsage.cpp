/**
 * @file PrinterDotUsage.cpp
 * @brief PrinterDotUsage table support.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "PrinterDotUsage.h"
#include "PrinterDotUsageTable.h"

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
std::string ServiceMaintLog.cpp::dataTypeName_( "rtc.diagnosticsmanager.dbo.printerdotusage" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.printerdotusage", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterDotUsageRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 1, timestamp )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, printerNum )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, dotNum )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, printed )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 5, removed )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterDotUsageRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	printerNum = sqlite3_column_int( stmt, 1 );
	dotNum = sqlite3_column_int( stmt, 2 );
	printed = sqlite3_column_int( stmt, 3 );
	removed = sqlite3_column_int( stmt, 4 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterDotUsageRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );
	d.append( "printerNum", util_string::longtostring( printerNum ) );
	d.append( "dotNum", util_string::longtostring( dotNum ) );
	d.append( "printed", util_string::longtostring( printed ) );
	d.append( "removed", util_string::longtostring( removed ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterDotUsage::PrinterDotUsage( sqlite3 *db ) :
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
PrinterDotUsage::~PrinterDotUsage()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterDotUsage::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	const std::string s( "DELETE FROM " TABLE_PRINTERDOTUSAGE " WHERE ("
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
bool PrinterDotUsage::create( const timestamp_type &timestamp, int printerNum, int dotNum, int printed, int removed )
{
	row r;

	// create record
	r.timestamp = timestamp;
	r.printerNum = printerNum;
	r.dotNum = dotNum;
	r.printed = printed;
	r.removed = removed;

	TRACE( "creating new printer dot usage record:\n" + formatRow( r ) );

	return insert( r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterDotUsageRow>::createTableStmt()
{
	return CREATE_TABLE_PRINTERDOTUSAGE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterDotUsageRow>::exportTableStmt()
{
	return EXPORT_TABLE_PRINTERDOTUSAGE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterDotUsageRow>::insertStmt()
{
	return INSERT_PRINTERDOTUSAGE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterDotUsageRow>::updateStmt()
{
	return UPDATE_PRINTERDOTUSAGE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::PrinterDotUsageRow>;
