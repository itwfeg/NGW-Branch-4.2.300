/**
 * @file PrinterHeadStatus.cpp
 * @brief PrinterHeadStatus table support.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "PrinterHeadStatus.h"
#include "PrinterHeadStatusTable.h"

#include <Printer.h>

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
std::string ServiceMaintLog.cpp::dataTypeName_( "rtc.diagnosticsmanager.dbo.printerheadstatus" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.printerheadstatus", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterHeadStatusRow::bind( sqlite3_stmt *stmt ) const
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
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, counts )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, status )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterHeadStatusRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	printerNum = sqlite3_column_int( stmt, 1 );
	counts = sqlite3_column_int( stmt, 2 );
	status = sqlite3_column_int( stmt, 3 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterHeadStatusRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );

	d.append( "printerNum", util_string::longtostring( printerNum ) );

	d.append( "counts", util_string::longtostring( counts ) );

	d.append( "status", util_string::longtostring( status ) );

	if ( ALL_DOTS_GOOD == status )
		d.appendString( "statusStr", "All Dots Good" );
	else if ( DOTS_MIXED == status )
		d.appendString( "statusStr", "Dots Mixed" );
	else if ( ALL_DOTS_BAD == status )
		d.appendString( "statusStr", "All Dots Bad" );
	else if ( DOT_FAILED == status )
		d.appendString( "statusStr", "Dot Failed" );
	else
		d.appendString( "statusStr", "Unknown" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterHeadStatus::PrinterHeadStatus( sqlite3 *db ) :
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
PrinterHeadStatus::~PrinterHeadStatus()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterHeadStatus::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	const std::string s( "DELETE FROM " TABLE_PRINTERHEADSTATUS " WHERE ("
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
bool PrinterHeadStatus::create( const timestamp_type &timestamp, int printerNum, int counts, int status )
{
	row r;

	// create record
	r.timestamp = timestamp;
	r.printerNum = printerNum;
	r.counts = counts;
	r.status = status;

	TRACE( "creating new printer head status record:\n" + formatRow( r ) );

	return insert( r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterHeadStatus::queryByAge( unsigned int age, int printerNum, rows &results )
{
	// compute timestamp for query
	timestamp_type timestamp( timestamp_type::now() );

	if ( !age )
		timestamp = 0;
	else
	{
		int value( age );
		value *= -1;

		timestamp.addSeconds( value );
	}

	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_PRINTERHEADSTATUS " WHERE ("
		    "(" COLUMN_PRINTER_NUM "=" + util_string::longtostring( printerNum ) + ") AND "
			"(" COLUMN_TIMESTAMP ">" + timestamp.toString() + "))"
			"ORDER BY " COLUMN_TIMESTAMP " DESC;" );

	// go!
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterHeadStatusRow>::createTableStmt()
{
	return CREATE_TABLE_PRINTERHEADSTATUS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterHeadStatusRow>::exportTableStmt()
{
	return EXPORT_TABLE_PRINTERHEADSTATUS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterHeadStatusRow>::insertStmt()
{
	return INSERT_PRINTERHEADSTATUS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterHeadStatusRow>::updateStmt()
{
	return UPDATE_PRINTERHEADSTATUS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::PrinterHeadStatusRow>;
