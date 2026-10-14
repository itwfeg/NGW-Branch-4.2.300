#include "sqliteknifetimehistory.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteKnifeTimeHistory::TYPE_FIELD            ("WKType");
const string SqliteKnifeTimeHistory::KNIFETIME_FIELD       ("WKTmng");
const string SqliteKnifeTimeHistory::KNIFETIMESTAMP_FIELD  ("WKDtTm");


#ifdef USE_ACCOUNTANT
int SqliteKnifeTimeHistory::_dataType_ = 0;
string SqliteKnifeTimeHistory::_dataTypeName_("SqliteKnifeTimeHistory");
#endif



SqliteKnifeTimeHistory::SqliteKnifeTimeHistory(SqliteRecord* r, DBKnifeTimeHistoryI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);
	util_acc::addInstance(_dataType_);
#endif
}


SqliteKnifeTimeHistory::~SqliteKnifeTimeHistory()
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
