#include "CuttingTestTags.h"


#include "CuttingTest.h"

#include "TransactionTypeTags.h"

#include "HobOperatorTags.h"
#include "ProductTags.h"
#include "TotalsTags.h"
#include "Debug.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

int CuttingTestTags::_dataType_ = 0;
std::string CuttingTestTags::_dataTypeName_("hfc::CuttingTestTags"); 
#endif


const std::string CuttingTestTags::cuttingTestNum   = "Ct";
const std::string CuttingTestTags::primalProductNum = "Pn";
const std::string CuttingTestTags::startDateTime    = "Ts";
const std::string CuttingTestTags::endDateTime      = "Te";
const std::string CuttingTestTags::lotNumber        = "B#";
const std::string CuttingTestTags::packingDate      = "BC";
const std::string CuttingTestTags::packingHours     = "TH";
const std::string CuttingTestTags::packingMinutes   = "TM";
const std::string CuttingTestTags::comments         = "Ci";
const std::string CuttingTestTags::finalComments    = "Cf";
const std::string CuttingTestTags::status           = "Cs";
const std::string CuttingTestTags::percentTotalShrinkage    = "TS";
const std::string CuttingTestTags::totalWeightOfCutProducts = "TW";
const std::string CuttingTestTags::totalPriceOfCutProducts  = "TP";


bool     CuttingTestTags::_tagsInitialized_ = false;
TagsList CuttingTestTags::cuttingTestTags(CuttingTest::endCuttingTestFieldOffsets);
TagsMap  CuttingTestTags::fieldMap;


using namespace hfc;


CuttingTestTags::CuttingTestTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for the product class
		cuttingTestTags[0] 													= emptyTag;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestNum] 			= cuttingTestNum;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestOperator] 		= HobOperatorTags::opNumber;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestVendorNum]		= ProductTags::prodVendorNumber;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestProductNum] 		= ProductTags::prodProductNumber;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestPrimalWeight] 	= TotalsTags::_totalWeight_;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestPrimalCost] 		= TotalsTags::_totalPrice_;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestStartDateTime]	= startDateTime;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestEndDateTime] 	= endDateTime;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestLotNumber]		= lotNumber;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestPackingDate] 	= packingDate;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestPackingHours] 	= packingHours;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestPackingMinutes] 	= packingMinutes;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestComments]		= comments;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestFinalComments] 	= finalComments;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestStatus] 			= status;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestPercentTotalShrinkage]		= percentTotalShrinkage;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestTotalWeightOfCutProducts]	= totalWeightOfCutProducts;
		cuttingTestTags[CuttingTest::fieldOffsetCuttingTestTotalPriceOfCutProducts] 	= totalPriceOfCutProducts;

		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

CuttingTestTags::~CuttingTestTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
