#include "sqliteoperator.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteOperator::NUMBER_FIELD          ("OpNum");
const string SqliteOperator::RFID_FIELD            ("OpRFID");
const string SqliteOperator::CLASSIFICATION_FIELD  ("OpAccLev");
const string SqliteOperator::LANGUAGE_FIELD        ("OpPLang");
const string SqliteOperator::SAWNOTE_FIELD         ("OpSawNote");
const string SqliteOperator::NAME_FIELD            ("OpName");


#ifdef USE_ACCOUNTANT
int SqliteOperator::_dataType_ = 0;
string SqliteOperator::_dataTypeName_("SqliteOperator");
#endif



SqliteOperator::SqliteOperator(SqliteRecord* r, DBOperatorI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteOperator::~SqliteOperator()
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
