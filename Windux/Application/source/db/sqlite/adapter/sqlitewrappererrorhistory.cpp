#include "sqlitewrappererrorhistory.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteWrapperErrorHistory::ERRORID_FIELD    ("WEID");
const string SqliteWrapperErrorHistory::ERROR_FIELD      ("WECode");
const string SqliteWrapperErrorHistory::DATETIME_FIELD   ("WEDtTm");
const string SqliteWrapperErrorHistory::PKGLENGTH_FIELD  ("WEPLen");
const string SqliteWrapperErrorHistory::PKGWIDTH_FIELD   ("WEPWdth");
const string SqliteWrapperErrorHistory::PKGHEIGHT_FIELD  ("WEPHght");
const string SqliteWrapperErrorHistory::FILMUSED_FIELD   ("WEFlmSl");
const string SqliteWrapperErrorHistory::TRAYNAME_FIELD   ("WETrType");
const string SqliteWrapperErrorHistory::BESTFIT_FIELD    ("WEBeFit");
const string SqliteWrapperErrorHistory::PERNET_FIELD     ("WEPerNet");


#ifdef USE_ACCOUNTANT
int SqliteWrapperErrorHistory::_dataType_ = 0;
string SqliteWrapperErrorHistory::_dataTypeName_("SqliteWrapperErrorHistory");
#endif



SqliteWrapperErrorHistory::SqliteWrapperErrorHistory(SqliteRecord* r, DBWrapperErrorHistoryI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteWrapperErrorHistory::~SqliteWrapperErrorHistory()
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
