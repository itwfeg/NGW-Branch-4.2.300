#include "sqlitepresentationmethodstatistic.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqlitePresentationMethodStatistic::PRESENTATIONMETHODTYPE_FIELD   ("PMType");
const string SqlitePresentationMethodStatistic::LANGUAGE_FIELD                 ("MSLang");
const string SqlitePresentationMethodStatistic::HOURNUMBER_FIELD               ("MSHrNum");
const string SqlitePresentationMethodStatistic::TOTALMETHODSELECTIONS_FIELD    ("MSTtMSl");
const string SqlitePresentationMethodStatistic::TOTALCATEGORYSELECTIONS_FIELD  ("MSTtCSl");
const string SqlitePresentationMethodStatistic::TOTALPRODUCTSELECTIONS_FIELD   ("MSTtPSl");
const string SqlitePresentationMethodStatistic::TOTALLABELSPRINTED_FIELD       ("MSTtLPr");


#ifdef USE_ACCOUNTANT
int SqlitePresentationMethodStatistic::_dataType_ = 0;
string SqlitePresentationMethodStatistic::_dataTypeName_("SqlitePresentationMethodStatistic");
#endif



SqlitePresentationMethodStatistic::SqlitePresentationMethodStatistic(SqliteRecord* r, DBPresentationMethodStatisticI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqlitePresentationMethodStatistic::~SqlitePresentationMethodStatistic()
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
