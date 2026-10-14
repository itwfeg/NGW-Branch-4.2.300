#include "sqlitecategorystatistic.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCategoryStatistic::CATEGORYNUMBER_FIELD   ("CaNum");
const string SqliteCategoryStatistic::LANGUAGE_FIELD         ("CSLang");
const string SqliteCategoryStatistic::HOURNUMBER_FIELD       ("CSHrNum");
const string SqliteCategoryStatistic::TOTALSELECTIONS_FIELD  ("CSTotSl");


#ifdef USE_ACCOUNTANT
int SqliteCategoryStatistic::_dataType_ = 0;
string SqliteCategoryStatistic::_dataTypeName_("SqliteCategoryStatistic");
#endif



SqliteCategoryStatistic::SqliteCategoryStatistic(SqliteRecord* r, DBCategoryStatisticI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCategoryStatistic::~SqliteCategoryStatistic()
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
