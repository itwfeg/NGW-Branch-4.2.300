#include "sqliterecall.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteRecall::NUMBER_FIELD            ("RecallNum");
const string SqliteRecall::NAME_FIELD              ("RecallName");
const string SqliteRecall::TEXT_FIELD              ("RecallText");
const string SqliteRecall::RECEIVEDDATETIME_FIELD  ("ReceivedDtTm");
const string SqliteRecall::ACKNOWLEDGED_FIELD      ("Acknowledged");


#ifdef USE_ACCOUNTANT
int SqliteRecall::_dataType_ = 0;
string SqliteRecall::_dataTypeName_("SqliteRecall");
#endif



SqliteRecall::SqliteRecall(SqliteRecord* r, DBRecallI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteRecall::~SqliteRecall()
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
