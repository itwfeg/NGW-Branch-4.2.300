#include "sqlitecoolcategoryproductionstep.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCoolCategoryProductionStep::PRODUCTIONSTEPKEY_FIELD  ("LSID");
const string SqliteCoolCategoryProductionStep::CATEGORYNUMBER_FIELD     ("LCNum");
const string SqliteCoolCategoryProductionStep::DEPTNUM_FIELD            ("DeptNum");


#ifdef USE_ACCOUNTANT
int SqliteCoolCategoryProductionStep::_dataType_ = 0;
string SqliteCoolCategoryProductionStep::_dataTypeName_("SqliteCoolCategoryProductionStep");
#endif



SqliteCoolCategoryProductionStep::SqliteCoolCategoryProductionStep(SqliteRecord* r, DBCoolCategoryProductionStepI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCoolCategoryProductionStep::~SqliteCoolCategoryProductionStep()
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
