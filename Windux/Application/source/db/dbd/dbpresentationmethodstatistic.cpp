#include "edb.h"
#include DBPRESENTATIONMETHODSTATISTIC_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBPresentationMethodStatistic::DBPresentationMethodStatistic(edb_adapter::DBPresentationMethodStatisticI* d)
		: decorated_(d)
{
}


DBPresentationMethodStatistic::DBPresentationMethodStatistic(const DBPresentationMethodStatistic& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBPresentationMethodStatistic::~DBPresentationMethodStatistic()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBPresentationMethodStatistic& rec)
{
	os << "DBPresentationMethodStatistic PresentationMethodType " << rec.getPresentationMethodType() <<
			", Language " << rec.getLanguage() <<
			", HourNumber " << rec.getHourNumber() <<
			" (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isTotalMethodSelectionsNull() )
		os << ", MethodSelections: " << rec.getTotalMethodSelections();
	if ( !rec.isTotalCategorySelectionsNull() )
		os << ", CategorySelections: " << rec.getTotalCategorySelections();
	if ( !rec.isTotalProductSelectionsNull() )
		os << ", ProductSelections: " << rec.getTotalProductSelections();
	if ( !rec.isTotalLabelsPrintedNull() )
		os << ", LabelsPrinted: " << rec.getTotalLabelsPrinted();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
