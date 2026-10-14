#include "edb.h"
#include DBCUTTINGTEST_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCuttingTest::DBCuttingTest(edb_adapter::DBCuttingTestI* d)
		: decorated_(d)
{
}


DBCuttingTest::DBCuttingTest(const DBCuttingTest& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCuttingTest::~DBCuttingTest()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCuttingTest& rec)
{
	os << "DBCuttingTest # " << rec.getTestNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isOperatorNumberNull() )
		os << "Operator Number: " << rec.getOperatorNumber();
	if ( !rec.isProductNumberNull() )
		os << ", Product Number: " << rec.getProductNumber();
	if ( !rec.isVendorNumberNull() )
		os << ", Vendor Number # : " << rec.getVendorNumber();
	if ( !rec.isPrimalWeightNull() )
		os << ", Primal Weight: " << rec.getPrimalWeight();
	if ( !rec.isPrimalCostNull() )
		os << ", Primal Cost: " << rec.getPrimalCost();
	if ( !rec.isStartDateNull() )
		os << ", Start Date: " << rec.getStartDate();
	if ( !rec.isEndDateNull() )
		os << ", End Date: " << rec.getEndDate();
	if ( !rec.isLotNumberNull() )
		os << ", Lot Number: " << rec.getLotNumber();
	if ( !rec.isPackingDateNull() )
		os << ", Packing Date: " << rec.getPackingDate();
	if ( !rec.isPackingHoursNull() )
		os << ", Packing Hours: " << rec.getPackingHours();
	if ( !rec.isPackingMinutesNull() )
		os << ", Packing Minutes: " << rec.getPackingMinutes();
	if ( !rec.isCommentsNull() )
		os << ", Comments: " << rec.getComments();
	if ( !rec.isFinalCommentsNull() )
		os << ", Final Comments: " << rec.getFinalComments();
	if ( !rec.isStatusNull() )
		os << ", Status: " << rec.getStatus();
	if ( !rec.isPercentTotalShrinkageNull() )
		os << ", Percent Total Shrinkage: " << rec.getPercentTotalShrinkage();
	if ( !rec.isTotalWeightOfCutProductsNull() )
		os << ", Total Weight Of Cut Products: " << rec.getTotalWeightOfCutProducts();
	if ( !rec.isTotalPriceOfCutProductsNull() )
		os << ", Total Price Of Cut Products: " << rec.getTotalPriceOfCutProducts();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
