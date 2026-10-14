#include "sqlitetransactiontype.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteTransactionType::NUMBER_FIELD                         ("TTNum");
const string SqliteTransactionType::NAME1_FIELD                          ("TTName1");
const string SqliteTransactionType::NAME2_FIELD                          ("TTName2");
const string SqliteTransactionType::AFFECTINGPRODUCTION_FIELD            ("TTAffPr");
const string SqliteTransactionType::ALLOWINGBACKDATING_FIELD             ("TTBkDt");
const string SqliteTransactionType::AUTOMATICALLYCHANGINGUPCDIGIT_FIELD  ("TTAutoUPC");
const string SqliteTransactionType::MANUALLYCHANGINGUPCDIGIT_FIELD       ("TTManUPC");
const string SqliteTransactionType::UPCDIGIT_FIELD                       ("TTUPCDigit");
const string SqliteTransactionType::USEBACKGROUNDCOLOR_FIELD             ("TTUseBgClr");
const string SqliteTransactionType::BACKGROUNDCOLOR_FIELD                ("TTBgColor");
const string SqliteTransactionType::TEXTFORLABEL_FIELD                   ("TTTextForLabel");


#ifdef USE_ACCOUNTANT
int SqliteTransactionType::_dataType_ = 0;
string SqliteTransactionType::_dataTypeName_("edb::SqliteTransactionType");
#endif



SqliteTransactionType::SqliteTransactionType(SqliteRecord* r, DBTransactionTypeI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteTransactionType::~SqliteTransactionType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( rec_ )
		delete rec_;

	if ( decorated_ )
		delete decorated_;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
