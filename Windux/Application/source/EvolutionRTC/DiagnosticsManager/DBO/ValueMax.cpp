/**
 * @file ValueMax.cpp
 * @brief ValueMax table support.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "ValueMax.h"
#include "ValueMaxTable.h"

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
std::string ServiceMaintLog.cpp::dataTypeName_( "rtc.diagnosticsmanager.dbo.valuemax" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.valuemax", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ValueMaxRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 1, timestamp )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, transNum )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, productNum )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 4, unitPrice )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 5, quantity )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 6, netWeight )) )
	{
		TRACE( "error during bind (6): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 7, tare )) )
	{
		TRACE( "error during bind (7): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 8, totalPrice )) )
	{
		TRACE( "error during bind (8): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 9, counts )) )
	{
		TRACE( "error during bind (9): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 10, scaleFactor )) )
	{
		TRACE( "error during bind (10): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 11, tiltX )) )
	{
		TRACE( "error during bind (11): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 12, tiltY )) )
	{
		TRACE( "error during bind (12): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 13, adjNetWeight )) )
	{
		TRACE( "error during bind (13): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 14, adjTotalPrice )) )
	{
		TRACE( "error during bind (14): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 15, savings )) )
	{
		TRACE( "error during bind (15): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ValueMaxRow::extract( sqlite3_stmt *stmt )
{
	timestamp = sqlite3_column_int64( stmt, 0 );
	transNum = sqlite3_column_int( stmt, 1 );
	productNum = sqlite3_column_int( stmt, 2 );
	unitPrice = sqlite3_column_double( stmt, 3 );
	quantity = sqlite3_column_int( stmt, 4 );
	netWeight = sqlite3_column_double( stmt, 5 );
	tare = sqlite3_column_double( stmt, 6 );
	totalPrice = sqlite3_column_double( stmt, 7 );
	counts = sqlite3_column_int( stmt, 8 );
	scaleFactor = sqlite3_column_int( stmt, 9 );
	tiltX = sqlite3_column_double( stmt, 10 );
	tiltY = sqlite3_column_double( stmt, 11 );
	adjNetWeight = sqlite3_column_double( stmt, 12 );
	adjTotalPrice = sqlite3_column_double( stmt, 13 );
	savings = sqlite3_column_double( stmt, 14 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ValueMaxRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "timestamp", timestamp.toString() );
	d.appendString( "timestampDateTime", timestamp.toDateTime() );
	d.append( "transNum", util_string::longtostring( transNum ) );
	d.append( "productNum", util_string::longtostring( productNum ) );
	d.append( "unitPrice", util_string::doubletostring( unitPrice, 0, 4 ) );
	d.append( "quantity", util_string::longtostring( quantity ) );
	d.append( "netWeight", util_string::doubletostring( netWeight, 0, 4 ) );
	d.append( "tare", util_string::doubletostring( tare, 0, 4 ) );
	d.append( "totalPrice", util_string::doubletostring( totalPrice, 0, 4 ) );
	d.append( "counts", util_string::longtostring( counts ) );
	d.append( "scaleFactor", util_string::longtostring( scaleFactor ) );
	d.append( "tiltX", util_string::doubletostring( tiltX, 0, 4 ) );
	d.append( "tiltY", util_string::doubletostring( tiltY, 0, 4 ) );
	d.append( "adjNetWeight", util_string::doubletostring( adjNetWeight, 0, 4 ) );
	d.append( "adjTotalPrice", util_string::doubletostring( adjTotalPrice, 0, 4 ) );
	d.append( "savings", util_string::doubletostring( savings, 0, 4 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ValueMax::ValueMax( sqlite3 *db ) :
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
ValueMax::~ValueMax()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ValueMax::create( const timestamp_type &timestamp, int transNum, int productNum, double unitPrice, int quantity, double netWeight, double tare, double totalPrice, int counts, int scaleFactor, double tiltX, double tiltY, double adjNetWeight, double adjTotalPrice, double savings )
{
	row r;

	// create record
	r.timestamp = timestamp;
	r.transNum = transNum;
	r.productNum = productNum;
	r.unitPrice = unitPrice;
	r.quantity = quantity;
	r.netWeight = netWeight;
	r.tare = tare;
	r.totalPrice = totalPrice;
	r.counts = counts;
	r.scaleFactor = scaleFactor;
	r.tiltX = tiltX;
	r.tiltY = tiltY;
	r.adjNetWeight = adjNetWeight;
	r.adjTotalPrice = adjTotalPrice;
	r.savings = savings;

	TRACE( "creating new value max record:\n" + formatRow( r ) );

	return insert( r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ValueMax::totalSavings( double& savings ) const
{
	const std::string s(
		"SELECT SUM(" COLUMN_SAVINGS ") "
			"FROM " TABLE_VALUEMAX ";" );

	sqlite3_stmt *stmt( NULL );
	int result;

	// setup query
	result = sqlite3_prepare_v2( db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE( "error during select prepare: " + util_string::longtostring( result ) );
		goto error_handler;
	}
	else if ( SQLITE_ROW != (result = sqlite3_step( stmt )) )
	{
		TRACE( "error select: " + util_string::longtostring( result ) );
		goto error_handler;
	}

	// extract savings amount
	savings = sqlite3_column_double( stmt, 0 );

	// finalize
	sqlite3_finalize( stmt );

	return true;

error_handler:

	if ( stmt )
		sqlite3_finalize( stmt );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ValueMax::housekeeping( sqlite3 *db, const timestamp_type &timestamp )
{
	const std::string s( "DELETE FROM " TABLE_VALUEMAX " WHERE ("
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
std::string TableBase<ValueMaxRow>::createTableStmt()
{
	return CREATE_TABLE_VALUEMAX;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ValueMaxRow>::exportTableStmt()
{
	return EXPORT_TABLE_VALUEMAX;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ValueMaxRow>::insertStmt()
{
	return INSERT_VALUEMAX;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ValueMaxRow>::updateStmt()
{
	return UPDATE_VALUEMAX;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::ValueMaxRow>;
