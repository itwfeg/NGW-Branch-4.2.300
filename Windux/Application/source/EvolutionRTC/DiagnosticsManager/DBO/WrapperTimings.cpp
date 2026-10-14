/**
 * @file WrapperTimings.cpp
 * @brief Wrapper height box and timings table support.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "WrapperTimings.h"
#include "WrapperTimingsTable.h"

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
int Wizards::dataType_ = 0;
std::string Wizards::dataTypeName_( "rtc.diagnosticsmanager.dbo.wrappertimings" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.wrappertimings", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTimingsRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_text( stmt, 1, name.c_str(), -1, NULL )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, plu )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, height )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, param )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 5, paramValue )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTimingsRow::extract( sqlite3_stmt *stmt )
{
	name = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 0 ) );
	plu = sqlite3_column_int( stmt, 1 );
	height = sqlite3_column_int( stmt, 2 );
	param = sqlite3_column_int( stmt, 3 );
	paramValue = sqlite3_column_int( stmt, 4 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperTimingsRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "name", name );
	d.append( "plu", util_string::longtostring( plu ) );
	d.append( "height", util_string::longtostring( height ) );
	d.append( "param", util_string::longtostring( param ) );
	d.append( "paramValue", util_string::longtostring( paramValue ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTimingsRow::operator == ( const WrapperTimingsRow &rhs ) const
{
	return (
		( rhs.name == name ) &&
		( rhs.plu == plu ) &&
		( rhs.height == height ) &&
		( rhs.param == param ) &&
		( rhs.paramValue == paramValue ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperTimings::WrapperTimings( sqlite3 *db ) :
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
WrapperTimings::~WrapperTimings()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTimings::deleteAll()
{
	const std::string s( "DELETE FROM " TABLE_WRAPPERTIMINGS ";" );

	sqlite3_stmt *stmt( NULL );
	int result;

	// setup query
	result = sqlite3_prepare_v2( db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE( "error during delete all prepare: " + util_string::longtostring( result ) );
		goto error_handler;
	}
	else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
	{
		TRACE( "error delete all: " + util_string::longtostring( result ) );
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
bool WrapperTimings::retrieve( const std::string &name, unsigned int plu, rows &result )
{
	std::string s;

	s = "SELECT * FROM " TABLE_WRAPPERTIMINGS " WHERE ("
		"(" COLUMN_NAME " = '" + name + "') AND "
		"(" COLUMN_PLU " = " + util_string::longtostring( plu ) + ")) "
		"ORDER BY "
			COLUMN_HEIGHT " ASC, "
			COLUMN_PARAM " ASC;";

	if ( select( s, result ) )
		return true;

	// nothing found
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTimings::save( const rows &values )
{
	return insert( values );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperTimingsRow>::createTableStmt()
{
	return CREATE_TABLE_WRAPPERTIMINGS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperTimingsRow>::exportTableStmt()
{
	return EXPORT_TABLE_WRAPPERTIMINGS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperTimingsRow>::insertStmt()
{
	return INSERT_WRAPPERTIMINGS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperTimingsRow>::updateStmt()
{
	return UPDATE_WRAPPERTIMINGS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::WrapperTimingsRow>;

