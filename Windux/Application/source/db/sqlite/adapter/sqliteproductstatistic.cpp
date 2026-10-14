#include "sqliteproductstatistic.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteProductStatistic::PRODUCTKEY_FIELD          ("PrID");
const string SqliteProductStatistic::LANGUAGE_FIELD            ("PSLang");
const string SqliteProductStatistic::HOURNUMBER_FIELD          ("PSHrNum");
const string SqliteProductStatistic::TOTALSELECTIONS_FIELD     ("PSTotSl");
const string SqliteProductStatistic::TIMETOSELECT_FIELD        ("PSTimSl");
const string SqliteProductStatistic::TOTALLABELSPRINTED_FIELD  ("PSTotPr");
const string SqliteProductStatistic::RECORDEDDATETIME_FIELD    ("PSDtTm");


#ifdef USE_ACCOUNTANT
int SqliteProductStatistic::_dataType_ = 0;
string SqliteProductStatistic::_dataTypeName_("SqliteProductStatistic");
#endif



SqliteProductStatistic::SqliteProductStatistic(SqliteRecord* r, DBProductStatisticI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteProductStatistic::~SqliteProductStatistic()
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
