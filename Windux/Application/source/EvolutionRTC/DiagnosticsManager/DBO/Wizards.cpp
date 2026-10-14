/**
 * @file Wizards.cpp
 * @brief Wizards table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "Wizards.h"
#include "WizardsTable.h"

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
std::string Wizards::dataTypeName_( "rtc.diagnosticsmanager.dbo.wizards" );
#endif

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.db.wizards", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WizardsRow::bind( sqlite3_stmt *stmt ) const
{
	int result;

	// bind
	if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 1, device )) )
	{
		TRACE( "error during bind (1): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 2, deviceExt )) )
	{
		TRACE( "error during bind (2): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 3, errorCode )) )
	{
		TRACE( "error during bind (3): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 4, severity )) )
	{
		TRACE( "error during bind (4): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 5, serviceCategory )) )
	{
		TRACE( "error during bind (5): " + util_string::longtostring( result ) );
		return false;
	}
	else if ( SQLITE_OK != (result = sqlite3_bind_int( stmt, 6, wizard )) )
	{
		TRACE( "error during bind (6): " + util_string::longtostring( result ) );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WizardsRow::extract( sqlite3_stmt *stmt )
{
	device = sqlite3_column_int( stmt, 0 );
	deviceExt = sqlite3_column_int( stmt, 1 );
	errorCode = sqlite3_column_int( stmt, 2 );
	severity = (severityType ) sqlite3_column_int( stmt, 3 );
	serviceCategory = sqlite3_column_int( stmt, 4 );
	wizard = sqlite3_column_int( stmt, 5 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WizardsRow::retrieve( util::Dictionary &d ) const
{
	d.append( "device", util_string::longtostring( device ) );
	d.append( "deviceExt", util_string::longtostring( deviceExt ) );
	d.append( "errorCode", util_string::longtostring( errorCode ) );
	d.append( "severity", util_string::longtostring( severity ) );
	d.append( "serviceCategory", util_string::longtostring( serviceCategory ) );
	d.append( "wizard", util_string::longtostring( wizard ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Wizards::Wizards( sqlite3 *db ) :
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
Wizards::~Wizards()
{
	TRACE( "DTOR" );

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int Wizards::lookupMaxCount( int device, int deviceExt, int errorCode )
{
	row result;

	if ( retrieve( device, deviceExt, errorCode, result ) )
		return result.wizard;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int Wizards::lookupServiceCategory( int device, int deviceExt, int errorCode )
{
	row result;

	if ( retrieve( device, deviceExt, errorCode, result ) )
		return result.serviceCategory;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WizardsRow::severityType Wizards::lookupSeverity( int device, int deviceExt, int errorCode )
{
	row result;

	if ( retrieve( device, deviceExt, errorCode, result ) )
		return result.severity;

	return row::Unknown;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Wizards::retrieve( int device, int deviceExt, int errorCode, row &result )
{
	std::string s;

	// check for device and deviceExt
	s = "SELECT * FROM " TABLE_WIZARDS " WHERE ("
		"(" COLUMN_DEVICE " = " + util_string::longtostring( device ) + ") AND "
		"(" COLUMN_DEVICE_EXT " = " + util_string::longtostring( deviceExt ) + ") AND "
		"(" COLUMN_ERROR_CODE " = " + util_string::longtostring( errorCode ) + "));";

	if ( select( s, result ) )
		return true;

	// check for device only
	if ( 0 < deviceExt )
	{
		s = "SELECT * FROM " TABLE_WIZARDS " WHERE ("
			"(" COLUMN_DEVICE " = " + util_string::longtostring( device ) + ") AND "
			"(" COLUMN_DEVICE_EXT " = 0) AND "
			"(" COLUMN_ERROR_CODE " = " + util_string::longtostring( errorCode ) + "));";

		if ( select( s, result ) )
			return true;
	}

	// nothing found
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WizardsRow>::createTableStmt()
{
	return CREATE_TABLE_WIZARDS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WizardsRow>::insertStmt()
{
	return INSERT_WIZARDS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WizardsRow>::populateTableStmt()
{
	return POPULATE_TABLE_WIZARDS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string TableBase<WizardsRow>::updateStmt()
{
	return UPDATE_WIZARDS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace


// pull in base implementation
#include "TableBaseImpl.h"

// instantiate base implementation
template class dbo::TableBase<dbo::WizardsRow>;

