#include "sqlitenutrition.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteNutrition::NUMBER_FIELD            ("NFNum");
const string SqliteNutrition::DEPTNUM_FIELD           ("DeptNum");
const string SqliteNutrition::NUTRITIONCOLUMN1_FIELD  ("NFCol1");
const string SqliteNutrition::NUTRITIONCOLUMN2_FIELD  ("NFCol2");
const string SqliteNutrition::NUTRITIONCOLUMN3_FIELD  ("NFCol3");
const string SqliteNutrition::NUTRITIONCOLUMN4_FIELD  ("NFCol4");


#ifdef USE_ACCOUNTANT
int SqliteNutrition::_dataType_ = 0;
string SqliteNutrition::_dataTypeName_("SqliteNutrition");
#endif



SqliteNutrition::SqliteNutrition(SqliteRecord* r, DBNutritionI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteNutrition::~SqliteNutrition()
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
