/**
 * @file TableBaseImpl.h
 * @brief Base object for table and row support implementation.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "TableBase.h"

/// Database objects namespace.
namespace dbo
{

static const std::string BASE_TRACE_CAT( "DiagnosticsManager.db.tablebase" );

#define TRACE_BASE( logline ) Debug::trace( BASE_TRACE_CAT, (logline) )
#define TRACE_BASEFUNC( logline ) if (Debug::isTracing( BASE_TRACE_CAT + "." + __func__ )) TRACE( (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
TableBase<_Row>::TableBase( sqlite3 *db ) :
	db_( db )
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
TableBase<_Row>::~TableBase()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
bool TableBase<_Row>::exportTable( sqlite3 *db )
{
	const std::string s( exportTableStmt() );

	sqlite3_stmt *stmt( NULL );

	// make sure we have something to do
	if ( !s.empty() )
	{
		int result;

		// create table
		result = sqlite3_prepare_v2( db, s.c_str(), (int) s.length(), &stmt, NULL );

		if (( SQLITE_OK != result ) || ( !stmt ))
		{
			TRACE_BASE( "error during export prepare: " + util_string::longtostring( result ) );
			TRACE_BASEFUNC( s );

			goto error_handler;
		}
		else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
		{
			TRACE_BASE( "error exporting table: " + util_string::longtostring( result ) );
			TRACE_BASEFUNC( s );

			goto error_handler;
		}

		// finalize
		sqlite3_finalize( stmt );
	}

	return true;

error_handler:

	if ( stmt )
		sqlite3_finalize( stmt );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
bool TableBase<_Row>::createTable( sqlite3 *db )
{
	const std::string s( createTableStmt() );
	const std::string ps( populateTableStmt() );

	sqlite3_stmt *stmt( NULL );
	int result;

	// create table
	result = sqlite3_prepare_v2( db, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE_BASE( "error during create prepare: " + util_string::longtostring( result ) );
		TRACE_BASEFUNC( s );

		goto error_handler;
	}
	else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
	{
		TRACE_BASE( "error creating table: " + util_string::longtostring( result ) );
		TRACE_BASEFUNC( s );

		goto error_handler;
	}

	// finalize
	sqlite3_finalize( stmt );

	// populate table
	if ( ps.length() )
	{
		result = sqlite3_prepare_v2( db, ps.c_str(), (int) ps.length(), &stmt, NULL );

		if (( SQLITE_OK != result ) || ( !stmt ))
		{
			TRACE_BASE( "error during populate prepare: " + util_string::longtostring( result ) );
			TRACE_BASEFUNC( ps );

			goto error_handler;
		}
		else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
		{
			TRACE_BASE( "error populating table: " + util_string::longtostring( result ) );
			TRACE_BASEFUNC( ps );

			goto error_handler;
		}

		// finalize
		sqlite3_finalize( stmt );
	}

	return true;

error_handler:

	if ( stmt )
		sqlite3_finalize( stmt );

	return false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
std::string TableBase<_Row>::formatRow( const row &value, bool csv )
{
	util::Dictionary d;
	d.setCommaSeparated( csv );

	value.retrieve( d );

	return d.format();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
bool TableBase<_Row>::insert( const row &value )
{
	const std::string s( insertStmt() );

	sqlite3_stmt *stmt( NULL );
	int result;

	// prepare
	result = sqlite3_prepare_v2( db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE_BASE( "error during insert prepare: " + util_string::longtostring( result ) );
		TRACE_BASEFUNC( s );

		goto error_handler;
	}

	// bind
	else if ( !value.bind( stmt ) )
	{
		TRACE_BASE( "error during insert bind" );
		TRACE_BASEFUNC( s );

		goto error_handler;
	}

	// go!
	else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
	{
		TRACE_BASE( "error during insert step: " + util_string::longtostring( result ) );
		TRACE_BASEFUNC( s );

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
template <typename _Row>
bool TableBase<_Row>::insert( const rows &values )
{
	bool result( true );

	for ( typename rows::const_iterator i( values.begin() ); i != values.end(); ++i )
		result &= insert( (*i) );

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
bool TableBase<_Row>::select( const std::string &s, row &result ) const
{
	rows results;

	// find record
	if (( select( s, results ) ) && ( 1 == results.size() ))
	{
		result = results[0];
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
bool TableBase<_Row>::select( const std::string &s, rows &results ) const
{
	sqlite3_stmt *stmt( NULL );
	int result;

	// prepare
	result = sqlite3_prepare_v2( db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE_BASE( "error during select prepare: " + util_string::longtostring( result ) );
		TRACE_BASEFUNC( s );

		goto error_handler;
	}

	// go!
	while ( SQLITE_ROW == (result = sqlite3_step( stmt )) )
	{
		row r;

		if ( !r.extract( stmt ) )
			TRACE_BASE( "error extracting row!" );
		else
		{
			results.push_back( r );
		}
	}

	if ( SQLITE_DONE != result )
	{
		TRACE_BASE( "error during select step: " + util_string::longtostring( result ) );
		TRACE_BASEFUNC( s );

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
template <typename _Row>
bool TableBase<_Row>::update( const row &value )
{
	const std::string s( updateStmt() );

	sqlite3_stmt *stmt( NULL );
	int result;

	// prepare
	result = sqlite3_prepare_v2( db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE_BASE( "error during update prepare: " + util_string::longtostring( result ) );
		TRACE_BASEFUNC( s );

		goto error_handler;
	}

	// bind
	else if ( !value.bind( stmt ) )
	{
		TRACE_BASE( "error during update bind" );
		TRACE_BASEFUNC( s );

		goto error_handler;
	}

	// go!
	else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
	{
		TRACE_BASE( "error during write step: " + util_string::longtostring( result ) );
		TRACE_BASEFUNC( s );

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
template <typename _Row>
bool TableBase<_Row>::update( const rows &values )
{
	bool result( true );

	for ( typename rows::const_iterator i( values.begin() ); i != values.end(); ++i )
		result &= update( (*i) );

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
std::string TableBase<_Row>::exportTableStmt()
{
	return std::string();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Row>
std::string TableBase<_Row>::populateTableStmt()
{
	return std::string();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
