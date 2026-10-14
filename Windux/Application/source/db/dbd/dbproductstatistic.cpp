#include "edb.h"
#include DBPRODUCTSTATISTIC_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBProductStatistic::DBProductStatistic(edb_adapter::DBProductStatisticI* d)
		: decorated_(d)
{
}


DBProductStatistic::DBProductStatistic(const DBProductStatistic& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBProductStatistic::~DBProductStatistic()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBProductStatistic& rec)
{
	os << "DBProductStatistic Language " << rec.getLanguage() <<
			", HourKey " << rec.getHourNumber() <<
			", ProductKey " << rec.getProductKey() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isTotalSelectionsNull() )
		os << ", TotalSelections: " << rec.getTotalSelections();
	if ( !rec.isTotalLabelsPrintedNull() )
		os << ", TotalLabelsPrinted: " << rec.getTotalLabelsPrinted();
	if ( !rec.isTimeToSelectNull() )
		os << ", TimeToSelect: " << rec.getTimeToSelect();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
