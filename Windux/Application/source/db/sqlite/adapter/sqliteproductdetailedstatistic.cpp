#include "sqliteproductdetailedstatistic.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteProductDetailedStatistic::PRODUCTKEY_FIELD                  ("PrID");
const string SqliteProductDetailedStatistic::PRODUCTSTATISTICLANGUAGE_FIELD    ("PSLang");
const string SqliteProductDetailedStatistic::PRODUCTSTATISTICHOURNUMBER_FIELD  ("PSHrNum");
const string SqliteProductDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD      ("PMType");
const string SqliteProductDetailedStatistic::SELECTIONS_FIELD                  ("DSSlctd");
const string SqliteProductDetailedStatistic::LABELSPRINTED_FIELD               ("DSPrntd");


#ifdef USE_ACCOUNTANT
int SqliteProductDetailedStatistic::_dataType_ = 0;
string SqliteProductDetailedStatistic::_dataTypeName_("SqliteProductDetailedStatistic");
#endif



SqliteProductDetailedStatistic::SqliteProductDetailedStatistic(SqliteRecord* r, DBProductDetailedStatisticI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteProductDetailedStatistic::~SqliteProductDetailedStatistic()
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
