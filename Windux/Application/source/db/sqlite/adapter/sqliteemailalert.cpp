#include "sqliteemailalert.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteEmailAlert::NUMBER_FIELD                ("EANum");
const string SqliteEmailAlert::NOTIFY_ON_FAILURE_FIELD     ("EAFail");
const string SqliteEmailAlert::NOTIFY_ON_RESOLUTION_FIELD  ("EAResol");
const string SqliteEmailAlert::NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD     ("EAHFDFail");
const string SqliteEmailAlert::NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD  ("EAHFDResol");
const string SqliteEmailAlert::DELAY_FIELD                 ("EADelay");
const string SqliteEmailAlert::FAILURESENT_FIELD           ("EAFSent");
const string SqliteEmailAlert::SUBJECT1_FIELD              ("EASubj1");
const string SqliteEmailAlert::SUBJECT2_FIELD              ("EASubj2");


#ifdef USE_ACCOUNTANT
int SqliteEmailAlert::_dataType_ = 0;
string SqliteEmailAlert::_dataTypeName_("SqliteEmailAlert");
#endif



SqliteEmailAlert::SqliteEmailAlert(SqliteRecord* r, DBEmailAlertI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteEmailAlert::~SqliteEmailAlert()
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
