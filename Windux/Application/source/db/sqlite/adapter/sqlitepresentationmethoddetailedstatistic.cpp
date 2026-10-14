#include "sqlitepresentationmethoddetailedstatistic.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD  ("PMS_PMType");
const string SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICLANGUAGE_FIELD    ("MSLang");
const string SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD  ("MSHrNum");
const string SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD                 ("PMType");
const string SqlitePresentationMethodDetailedStatistic::SELECTIONS_FIELD                             ("MDSlctd");


#ifdef USE_ACCOUNTANT
int SqlitePresentationMethodDetailedStatistic::_dataType_ = 0;
string SqlitePresentationMethodDetailedStatistic::_dataTypeName_("SqlitePresentationMethodDetailedStatistic");
#endif



SqlitePresentationMethodDetailedStatistic::SqlitePresentationMethodDetailedStatistic(SqliteRecord* r, DBPresentationMethodDetailedStatisticI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqlitePresentationMethodDetailedStatistic::~SqlitePresentationMethodDetailedStatistic()
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
