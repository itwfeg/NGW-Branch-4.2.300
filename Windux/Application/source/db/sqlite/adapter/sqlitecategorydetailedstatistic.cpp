#include "sqlitecategorydetailedstatistic.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCategoryDetailedStatistic::CATEGORYNUMBER_FIELD               ("CaNum");
const string SqliteCategoryDetailedStatistic::CATEGORYSTATISTICLANGUAGE_FIELD    ("CSLang");
const string SqliteCategoryDetailedStatistic::CATEGORYSTATISTICHOURNUMBER_FIELD  ("CSHrNum");
const string SqliteCategoryDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD       ("PMType");
const string SqliteCategoryDetailedStatistic::SELECTIONS_FIELD                   ("CDSlctd");


#ifdef USE_ACCOUNTANT
int SqliteCategoryDetailedStatistic::_dataType_ = 0;
string SqliteCategoryDetailedStatistic::_dataTypeName_("SqliteCategoryDetailedStatistic");
#endif



SqliteCategoryDetailedStatistic::SqliteCategoryDetailedStatistic(SqliteRecord* r, DBCategoryDetailedStatisticI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCategoryDetailedStatistic::~SqliteCategoryDetailedStatistic()
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
