/**
 * @file Config.cpp
 * @brief Config table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "Config.h"
#include "ConfigTable.h"

#include <Debug.h>

#include <e_string/utilstring.h>

#include <sqlite3.h>

#include <sstream>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

/// Database objects namespace.
namespace dbo
{

#ifdef USE_ACCOUNTANT
int Config::dataType_ = 0;
std::string Config::dataTypeName_( "rtc.diagnosticsmanager.dbo.config" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.config", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ConfigRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_text( stmt, 1, name.c_str(), -1, NULL )) )
	{
		TRACE( "error during write bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, type )) )
	{
		TRACE( "error during write bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if (( vtInteger == type ) && ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, value_integer )) ))
	{
		TRACE( "error during write bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if (( vtString == type ) && ( SQLITE_OK != (result = sqlite3_bind_text( stmt, 3, value_string.c_str(), -1, NULL )) ))
	{
		TRACE( "error during write bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if (( vtReal == type ) && ( SQLITE_OK != (result = sqlite3_bind_double( stmt, 3, value_real )) ))
	{
		TRACE( "error during write bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if (( vtLongLong == type ) && ( SQLITE_OK != (result = sqlite3_bind_int64( stmt, 3, value_longlong )) ))
	{
		TRACE( "error during write bind (3): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ConfigRow::extract( sqlite3_stmt *stmt )
{
	name = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 0 ) );
	type = (valueType) sqlite3_column_int( stmt, 1 );

	if ( vtInteger == type )
		value_integer = sqlite3_column_int( stmt, 2 );
	else if ( vtString == type )
		value_string = reinterpret_cast<const char*>( sqlite3_column_text( stmt, 2 ) );
	else if ( vtReal == type )
		value_real = sqlite3_column_double( stmt, 2 );
	else if ( vtLongLong == type )
		value_longlong = sqlite3_column_int64( stmt, 2 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigRow::retrieve( util::Dictionary &d ) const
{
	d.appendString( "name", name );
	d.append( "type", util_string::longtostring( type ) );

	if ( vtInteger == type )
		d.append( "value", util_string::longtostring( value_integer ) );
	else if ( vtString == type )
		d.appendString( "value", value_string );
	else if ( vtReal == type )
		d.append( "value", util_string::doubletostring( value_real, 1, 3 ) );
	else if ( vtLongLong == type )
		d.append( "value", util_string::longlongtostring( value_longlong ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Config::Config( sqlite3 *db ) :
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
Config::~Config()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Config::timestamp_type Config::created() const
{
	row result;

	// go!
	if ( !retrieve( CONFIG_CREATED, result ) )
		return 0;

	return timestamp_type( result.value_longlong );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int Config::edgeInit() const
{
	row result;

	// go!
	if ( !retrieve( CONFIG_EDGE_INIT, result ) )
		return DEFAULT_CONFIG_EDGE_INIT;

	return result.value_integer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int Config::edgePowerUp() const
{
	row result;

	// go!
	if ( !retrieve( CONFIG_EDGE_POWER_UP, result ) )
		return DEFAULT_CONFIG_EDGE_POWER_UP;

	return result.value_integer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int Config::edgeStatusCheckTimer() const
{
	row result;

	// go!
	if ( !retrieve( CONFIG_EDGE_STATUS_CHECK, result ) )
		return DEFAULT_CONFIG_EDGE_STATUS_CHECK;

	return result.value_integer;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int Config::housekeeping() const
{
	row result;

	// go!
	if ( !retrieve( CONFIG_HOUSEKEEPING, result ) )
		return DEFAULT_CONFIG_HOUSEKEEPING;

	return result.value_integer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Config::isLatestVersion() const
{
	return (DB_VERSION == version());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Config::setWrapperLowerFilmWidth( unsigned int value )
{
	row r;
	r.name = CONFIG_WRAPPER_LOWER_FILM_WIDTH;
	r.type = row::vtInteger;
	r.value_integer = value;

	update( r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Config::setWrapperUpperFilmWidth( unsigned int value )
{
	row r;
	r.name = CONFIG_WRAPPER_UPPER_FILM_WIDTH;
	r.type = row::vtInteger;
	r.value_integer = value;

	update( r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int Config::version() const
{
	row result;

	// go!
	if ( !retrieve( CONFIG_DB_VERSION, result ) )
		return 0;

	return result.value_integer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int Config::wrapperLowerFilmWidth() const
{
	row result;

	// go!
	if ( !retrieve( CONFIG_WRAPPER_LOWER_FILM_WIDTH, result ) )
		return 0;

	return result.value_integer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int Config::wrapperUpperFilmWidth() const
{
	row result;

	// go!
	if ( !retrieve( CONFIG_WRAPPER_UPPER_FILM_WIDTH, result ) )
		return 0;

	return result.value_integer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Config::addValueMaxSavings( double savings )
{
	row result;

	// load value
	if ( !retrieve( CONFIG_VALUE_MAX_SAVINGS, result ) )
		return false;

	result.value_real += savings;

	// store value
	return update( result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Config::retrieve( const std::string &name, row &result ) const
{
	const std::string s(
		"SELECT * FROM " TABLE_CONFIG " WHERE ("
			COLUMN_NAME " = '" + name + "'" ");" );

	// go!
	return select( s, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ConfigRow>::createTableStmt()
{
	return CREATE_TABLE_CONFIG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ConfigRow>::insertStmt()
{
	return INSERT_CONFIG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ConfigRow>::populateTableStmt()
{
	std::string stmt( POPULATE_TABLE_CONFIG );

	std::string created( "%" CONFIG_CREATED "%" );
	size_t createdPos( stmt.find( created ) );

	if ( std::string::npos != createdPos )
	{
		util::Timestamp now( util::Timestamp::now() );
		stmt.replace( createdPos, created.length(), now.toString() );
	}

	return stmt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<ConfigRow>::updateStmt()
{
	return UPDATE_CONFIG;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::ConfigRow>;


