#include "sqlitesoftkeygroup.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteSoftKeyGroup::NUMBER_FIELD              ("SGNum");
const string SqliteSoftKeyGroup::DEPTNUM_FIELD             ("DeptNum");
const string SqliteSoftKeyGroup::HORIZONTALGRIDSIZE_FIELD  ("SGHGrid");
const string SqliteSoftKeyGroup::VERTICALGRIDSIZE_FIELD    ("SGVGrid");
const string SqliteSoftKeyGroup::HORIZONTALSPACING_FIELD   ("SGHSpc");
const string SqliteSoftKeyGroup::VERTICALSPACING_FIELD     ("SGVSpc");


#ifdef USE_ACCOUNTANT
int SqliteSoftKeyGroup::_dataType_ = 0;
string SqliteSoftKeyGroup::_dataTypeName_("SqliteSoftKeyGroup");
#endif



SqliteSoftKeyGroup::SqliteSoftKeyGroup(SqliteRecord* r, DBSoftKeyGroupI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteSoftKeyGroup::~SqliteSoftKeyGroup()
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
