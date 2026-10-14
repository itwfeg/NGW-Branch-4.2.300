#include "edb.h"
#include DBPRODUCTDETAILEDSTATISTIC_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBProductDetailedStatistic::DBProductDetailedStatistic(edb_adapter::DBProductDetailedStatisticI* d)
		: decorated_(d)
{
}


DBProductDetailedStatistic::DBProductDetailedStatistic(const DBProductDetailedStatistic& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBProductDetailedStatistic::~DBProductDetailedStatistic()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBProductDetailedStatistic& rec)
{
	os << "DBProductDetailedStatistic PresentationMethodNumber " << rec.getPresentationMethodNumber() <<
			", ProductStatisticLanguage " << rec.getProductStatisticLanguage() <<
			", ProductStatisticHourNumber " << rec.getProductStatisticHourNumber() <<
			", ProductKey " << rec.getProductKey() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isSelectionsNull() )
		os << ", Selections: " << rec.getSelections();
	if ( !rec.isLabelsPrintedNull() )
		os << ", LabelsPrinted: " << rec.getLabelsPrinted();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
