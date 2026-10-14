
#include "GenericTextReport.h"

#include "ReportManager.h"
#include "CfgAll.h"

#include "MultiLanguageText.h"

#include "util/e_string/utilstring.h"


const std::string GenericTextReport::_contentFontFamily_( "Gravilex" );
const int GenericTextReport::_contentFontSize_ = 7;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GenericTextReport::dataType = 0;
std::string GenericTextReport::dataTypeName( "GenericTextReport" );
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
GenericTextReport::GenericTextReport( ReportManager *reportManager, PrinterType type ) :
	AbstractReport( reportManager, type ),
	name_( "Untitled Report" )
{
#ifdef USE_ACCOUNTANT
	if ( dataType == 0 )
		dataType = util_acc::registerDataType( dataTypeName );

	util_acc::addInstance( dataType );
#endif

#ifndef EVOL_NDEBUG
	Debug::trace(
		"GenericTextReport", 20,
		"Constructing the GenericTextReport for printer " + inttostring(type) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
GenericTextReport::GenericTextReport( const std::string &name, const std::string &content, ReportManager *reportManager, PrinterType type ) :
	AbstractReport( reportManager, type ),
	name_( name )
{
#ifdef USE_ACCOUNTANT
	if ( dataType == 0 )
		dataType = util_acc::registerDataType( dataTypeName );

	util_acc::addInstance( dataType );
#endif

#ifndef EVOL_NDEBUG
	Debug::trace(
		"GenericTextReport", 20,
		"Constructing the GenericTextReport for printer " + inttostring(type) );
#endif

	setContent( content );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
GenericTextReport::~GenericTextReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType );
#endif
#ifndef EVOL_NDEBUG
	Debug::trace(
		"GenericTextReport", 20,
		"Destroying the GenericTextReport");
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GenericTextReport::doInitialize()
{
#ifndef EVOL_NDEBUG
	Debug::trace(
		"GenericTextReport", 20,
		"Initializing the GenericTextReport" );
#endif

	// ready
	setContentReady();
}
