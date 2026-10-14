/**
 * @file ServiceCat.cpp
 * @brief Service category table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "ServiceCat.h"
#include "ServiceCatTable.h"

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
std::string ServiceCat::dataTypeName_( "rtc.diagnosticsmanager.dbo.servicecat" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.servicecat", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceCatRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 1, id )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_text( stmt, 2, description.c_str(), -1, NULL )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceCatRow::extract( sqlite3_stmt *stmt )
{
	id = sqlite3_column_int( stmt, 0 );
	description = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 1 ) );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceCatRow::retrieve( util::Dictionary &d ) const
{
	d.append( "id", util_string::longtostring( id ) );
	d.appendString( "description", description );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceCat::ServiceCat( sqlite3 *db ) :
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
ServiceCat::~ServiceCat()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceCat::retrieve( int id, row &result )
{
	// setup query
	const std::string s(
		"SELECT * FROM " TABLE_SERVICECAT " WHERE ("
			COLUMN_ID " = " + util_string::longtostring( id ) + ");" );

	// find record
	return select( s, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceCatRow>::createTableStmt()
{
	return CREATE_TABLE_SERVICECAT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceCatRow>::insertStmt()
{
	return INSERT_SERVICECAT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceCatRow>::populateTableStmt()
{
	return POPULATE_TABLE_SERVICECAT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ServiceCatRow>::updateStmt()
{
	return UPDATE_SERVICECAT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::ServiceCatRow>;


