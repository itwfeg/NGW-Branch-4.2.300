#include "sqlitecuttingtest.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCuttingTest::CUTTINGTESTID_FIELD   ("UTNum");
const string SqliteCuttingTest::OPERATORNUMBER_FIELD  ("OpNum");
const string SqliteCuttingTest::STARTDATE_FIELD       ("UTStrDt");
const string SqliteCuttingTest::ENDDATE_FIELD         ("UTEndDt");
const string SqliteCuttingTest::PRODUCTNUMBER_FIELD   ("UTPmPrN");
const string SqliteCuttingTest::VENDORNUMBER_FIELD    ("UTPmVdN");
const string SqliteCuttingTest::PRIMALWEIGHT_FIELD    ("UTPmWgt");
const string SqliteCuttingTest::PRIMALCOST_FIELD      ("UTPmCst");
const string SqliteCuttingTest::LOTNUMBER_FIELD       ("UTLotNumber");
const string SqliteCuttingTest::PACKINGDATE_FIELD     ("UTPackingDate");
const string SqliteCuttingTest::PACKINGHOURS_FIELD    ("UTPackingHours");
const string SqliteCuttingTest::PACKINGMINUTES_FIELD  ("UTPackingMinutes");
const string SqliteCuttingTest::COMMENTS_FIELD        ("UTComments");
const string SqliteCuttingTest::FINALCOMMENTS_FIELD   ("UTFinalComments");
const string SqliteCuttingTest::STATUS_FIELD          ("UTStatus");
const string SqliteCuttingTest::PERCENTTOTALSHRINKAGE_FIELD     ("UTPercentTotalShrinkage");
const string SqliteCuttingTest::TOTALWEIGHTOFCUTPRODUCTS_FIELD  ("UTTotalWeightOfCutProducts");
const string SqliteCuttingTest::TOTALPRICEOFCUTPRODUCTS_FIELD   ("UTTotalPriceOfCutProducts");


#ifdef USE_ACCOUNTANT
int SqliteCuttingTest::_dataType_ = 0;
string SqliteCuttingTest::_dataTypeName_("SqliteCuttingTest");
#endif



SqliteCuttingTest::SqliteCuttingTest(SqliteRecord* r, DBCuttingTestI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCuttingTest::~SqliteCuttingTest()
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
