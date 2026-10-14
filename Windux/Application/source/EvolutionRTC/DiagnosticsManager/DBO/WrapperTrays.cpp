/**
 * @file WrapperTrays.cpp
 * @brief Wrapper trays table support.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "WrapperTrays.h"
#include "WrapperTraysTable.h"

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
std::string Wizards::dataTypeName_( "rtc.diagnosticsmanager.dbo.wrappertrays" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.wrappertrays", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTraysRow::bind( sqlite3_stmt *stmt ) const
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
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, lengthMin )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, lengthMax )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 5, widthMin )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 6, widthMax )) )
	{
		TRACE( "error during bind (6): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 7, lip )) )
	{
		TRACE( "error during bind (7): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTraysRow::extract( sqlite3_stmt *stmt )
{
	name = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 0 ) );
	plu = sqlite3_column_int( stmt, 1 );
	lengthMin = sqlite3_column_int( stmt, 2 );
	lengthMax = sqlite3_column_int( stmt, 3 );
	widthMin = sqlite3_column_int( stmt, 4 );
	widthMax = sqlite3_column_int( stmt, 5 );
	lip = sqlite3_column_int( stmt, 6 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperTraysRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "name", name );
	d.append( "plu", util_string::longtostring( plu ) );
	d.append( "lengthMin", util_string::longtostring( lengthMin ) );
	d.append( "lengthMax", util_string::longtostring( lengthMax ) );
	d.append( "widthMin", util_string::longtostring( widthMin ) );
	d.append( "widthMax", util_string::longtostring( widthMax ) );
	d.append( "lip", util_string::longtostring( lip ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTraysRow::operator == ( const WrapperTraysRow &rhs ) const
{
	return (
		( rhs.name == name ) &&
		( rhs.plu == plu ) &&
		( rhs.lengthMin == lengthMin ) &&
		( rhs.lengthMax == lengthMax ) &&
		( rhs.widthMin == widthMin ) &&
		( rhs.widthMax == widthMax ) &&
		( rhs.lip == lip ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperTrays::WrapperTrays( sqlite3 *db ) :
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
WrapperTrays::~WrapperTrays()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTrays::deleteAll()
{
	const std::string s( "DELETE FROM " TABLE_WRAPPERTRAYS ";" );

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
bool WrapperTrays::retrieve( const std::string &name, unsigned int plu, row &result )
{
	std::string s;

	s = "SELECT * FROM " TABLE_WRAPPERTRAYS " WHERE ("
		"(" COLUMN_NAME " = '" + name + "') AND "
		"(" COLUMN_PLU " = " + util_string::longtostring( plu ) + "));";

	if ( select( s, result ) )
		return true;

	// nothing found
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTrays::retrieveAll( rows &result )
{
	const std::string s( "SELECT * FROM " TABLE_WRAPPERTRAYS ";" );

	if ( select( s, result ) )
		return true;

	// nothing found
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperTrays::save( const row &value )
{
	return insert( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperTraysRow>::createTableStmt()
{
	return CREATE_TABLE_WRAPPERTRAYS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperTraysRow>::exportTableStmt()
{
	return EXPORT_TABLE_WRAPPERTRAYS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperTraysRow>::insertStmt()
{
	return INSERT_WRAPPERTRAYS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WrapperTraysRow>::updateStmt()
{
	return UPDATE_WRAPPERTRAYS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::WrapperTraysRow>;

