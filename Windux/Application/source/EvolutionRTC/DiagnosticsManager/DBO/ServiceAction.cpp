/**
 * @file ServiceAction.cpp
 * @brief Service action table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "ServiceAction.h"
#include "ServiceActionTable.h"
#include "ServiceCatTable.h"
#include "ServiceMethodTable.h"
#include "ServicePartTable.h"

#if defined ( __linux__ )
#include <Util/Dict.h>
#else
#include "../Util/Dict.h"
#endif // __linux__


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
int ServiceCat::dataType_ = 0;
std::string ServiceCat::dataTypeName_( "rtc.diagnosticsmanager.dbo.serviceaction" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.serviceaction", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceActionRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 1, category )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, method )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, part )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, order )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceActionRow::extract( sqlite3_stmt *stmt )
{
	category = sqlite3_column_int( stmt, 0 );
	method = sqlite3_column_int( stmt, 1 );
	part = sqlite3_column_int( stmt, 2 );
	order = sqlite3_column_int( stmt, 3 );

	if ( 5 <= sqlite3_column_count( stmt ) )
		categoryName = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 4 ) );

	if ( 6 <= sqlite3_column_count( stmt ) )
		methodName = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 5 ) );

	if ( 7 <= sqlite3_column_count( stmt ) )
		partName = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 6 ) );

	if ( 8 <= sqlite3_column_count( stmt ) )
		notesRequired = (0 != sqlite3_column_int( stmt, 7 ));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceActionRow::retrieve( util::Dictionary &d ) const
{
	d.append( "category", util_string::longtostring( category ) );
	d.append( "method", util_string::longtostring( method ) );
	d.append( "part", util_string::longtostring( part ) );
	d.append( "order", util_string::longtostring( order ) );
	d.appendString( "categoryName", categoryName );
	d.appendString( "methodName", methodName );
	d.appendString( "partName", partName );
	d.append( "notesRequired", util_string::longtostring( notesRequired ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceAction::ServiceAction( sqlite3 *db ) :
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
ServiceAction::~ServiceAction()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceAction::queryByCategory( int category, rows &results )
{
	// setup query
	const std::string s(
		"SELECT a." COLUMN_CAT ", a." COLUMN_METHOD ", a." COLUMN_PART ", a." COLUMN_ORDERING ", "
			"c." COLUMN_DESCRIPTION ", "
			"m." COLUMN_DESCRIPTION ", "
			"p." COLUMN_DESCRIPTION ", p." COLUMN_NOTES_REQUIRED " "
		"FROM "	TABLE_SERVICEACTION " a "
			"INNER JOIN " TABLE_SERVICECAT " c "
				"ON a." COLUMN_CAT "=c." COLUMN_ID " "
			"INNER JOIN " TABLE_SERVICEMETHOD " m "
				"ON a." COLUMN_METHOD "=m." COLUMN_ID " "
			"INNER JOIN " TABLE_SERVICEPART " p "
				"ON a." COLUMN_PART "=p." COLUMN_ID " "
		"WHERE a." COLUMN_CAT "=" + util_string::longtostring( category ) + " "
		"ORDER BY "
			"a." COLUMN_ORDERING " ASC, "
			"c." COLUMN_DESCRIPTION " ASC, "
			"m." COLUMN_DESCRIPTION " ASC, "
			"p." COLUMN_DESCRIPTION " ASC;" );

	// find record
	return select( s, results );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceActionRow>::createTableStmt()
{
	return CREATE_TABLE_SERVICEACTION;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceActionRow>::insertStmt()
{
	return INSERT_SERVICEACTION;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceActionRow>::populateTableStmt()
{
	return POPULATE_TABLE_SERVICEACTION;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceActionRow>::updateStmt()
{
	return UPDATE_SERVICEACTION;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::ServiceActionRow>;


