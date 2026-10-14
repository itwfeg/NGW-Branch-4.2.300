#include "sqlitesynchronizationchange.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteSynchronizationChange::IDENTIFIER_FIELD         ("YCID");
const string SqliteSynchronizationChange::BATCHNUMBER_FIELD        ("BaNum");
const string SqliteSynchronizationChange::APPLICATIONDEVICE_FIELD  ("YCAppDv");
const string SqliteSynchronizationChange::APPLICATIONEVENT_FIELD   ("YCAppEv");
const string SqliteSynchronizationChange::APPLICATIONFORMAT_FIELD  ("YCAppFm");
const string SqliteSynchronizationChange::CHANGE_FIELD             ("YCChange");


#ifdef USE_ACCOUNTANT
int SqliteSynchronizationChange::_dataType_ = 0;
string SqliteSynchronizationChange::_dataTypeName_("SqliteSynchronizationChange");
#endif



SqliteSynchronizationChange::SqliteSynchronizationChange(SqliteRecord* r, DBSynchronizationChangeI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteSynchronizationChange::~SqliteSynchronizationChange()
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
