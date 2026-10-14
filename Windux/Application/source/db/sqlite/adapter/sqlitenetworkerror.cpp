#include "sqlitenetworkerror.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteNetworkError::NETWORKERRORID_FIELD  ("NEID");
const string SqliteNetworkError::SCALEID_FIELD         ("ScID");
const string SqliteNetworkError::ERRORCODE_FIELD       ("NEECode");
const string SqliteNetworkError::RECORDTYPE_FIELD      ("NERecType");
const string SqliteNetworkError::DATETIME_FIELD        ("NEDateTm");


#ifdef USE_ACCOUNTANT
int SqliteNetworkError::_dataType_ = 0;
string SqliteNetworkError::_dataTypeName_("SqliteNetworkError");
#endif



SqliteNetworkError::SqliteNetworkError(SqliteRecord* r, DBNetworkErrorI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}


SqliteNetworkError::~SqliteNetworkError()
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
