/**
 * @file PrinterDotWear.cpp
 * @brief PrinterDotWear table support.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "PrinterDotWear.h"
#include "PrinterDotWearTable.h"

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
std::string ServiceMaintLog.cpp::dataTypeName_( "rtc.diagnosticsmanager.dbo.printerdotwear" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.printerdotwear", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterDotWearRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 1, timestamp )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, dotNum )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, status )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, value )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterDotWearRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	dotNum = sqlite3_column_int( stmt, 1 );
	status = sqlite3_column_int( stmt, 2 );
	value = sqlite3_column_int( stmt, 3 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterDotWearRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );

	d.append( "dotNum", util_string::longtostring( dotNum ) );

	d.append( "status", util_string::longtostring( status ) );

	if ( DotGood == status )
		d.appendString( "statusStr", "Good" );
	else if ( DotBad == status )
		d.appendString( "statusStr", "Bad" );
	else if ( DotMarginal == status )
		d.appendString( "statusStr", "Marginal" );
	else
		d.appendString( "statusStr", "Unknown" );

	d.append( "value", util_string::longtostring( value ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterDotWear::PrinterDotWear( sqlite3 *db ) :
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
PrinterDotWear::~PrinterDotWear()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterDotWear::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	const std::string s( "DELETE FROM " TABLE_PRINTERDOTWEAR " WHERE ("
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
bool PrinterDotWear::create( const timestamp_type &timestamp, int dotNum, int status, int value )
{
	row r;

	// create record
	r.timestamp = timestamp;
	r.dotNum = dotNum;
	r.status = status;
	r.value = value;

	TRACE( "creating new printer dot wear record:\n" + formatRow( r ) );

	return insert( r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterDotWearRow>::createTableStmt()
{
	return CREATE_TABLE_PRINTERDOTWEAR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterDotWearRow>::exportTableStmt()
{
	return EXPORT_TABLE_PRINTERDOTWEAR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterDotWearRow>::insertStmt()
{
	return INSERT_PRINTERDOTWEAR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<PrinterDotWearRow>::updateStmt()
{
	return UPDATE_PRINTERDOTWEAR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::PrinterDotWearRow>;
