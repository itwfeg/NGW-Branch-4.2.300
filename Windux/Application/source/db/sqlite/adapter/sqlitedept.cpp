#include "sqlitedept.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteDept::NUMBER_FIELD        ("DeptNum");
const string SqliteDept::NAME1_FIELD         ("DeptName1");
const string SqliteDept::NAME2_FIELD         ("DeptName2");


#ifdef USE_ACCOUNTANT
int SqliteDept::_dataType_ = 0;
string SqliteDept::_dataTypeName_("SqliteDept");
#endif



SqliteDept::SqliteDept(SqliteRecord* r, DBDeptI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteDept::~SqliteDept()
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
