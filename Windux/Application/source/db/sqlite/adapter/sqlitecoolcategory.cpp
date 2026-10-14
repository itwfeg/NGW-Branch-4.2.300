#include "sqlitecoolcategory.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCoolCategory::NUMBER_FIELD  ("LCNum");
const string SqliteCoolCategory::DEPTNUM_FIELD("DeptNum");
const string SqliteCoolCategory::NAME1_FIELD   ("LCName1");
const string SqliteCoolCategory::NAME2_FIELD   ("LCName2");


#ifdef USE_ACCOUNTANT
int SqliteCoolCategory::_dataType_ = 0;
string SqliteCoolCategory::_dataTypeName_("SqliteCoolCategory");
#endif



SqliteCoolCategory::SqliteCoolCategory(SqliteRecord* r, DBCoolCategoryI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCoolCategory::~SqliteCoolCategory()
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
