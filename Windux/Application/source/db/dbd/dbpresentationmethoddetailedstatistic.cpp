#include "edb.h"
#include DBPRESENTATIONMETHODDETAILEDSTATISTIC_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBPresentationMethodDetailedStatistic::DBPresentationMethodDetailedStatistic(edb_adapter::DBPresentationMethodDetailedStatisticI* d)
		: decorated_(d)
{
}


DBPresentationMethodDetailedStatistic::DBPresentationMethodDetailedStatistic(const DBPresentationMethodDetailedStatistic& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBPresentationMethodDetailedStatistic::~DBPresentationMethodDetailedStatistic()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBPresentationMethodDetailedStatistic& rec)
{
	os << "DBPresentationMethodDetailedStatistic PresentationMethodType " << rec.getPresentationMethodType() <<
			", PresentationMethodStatisticHourNumber " << rec.getPresentationMethodStatisticHourNumber() <<
			", PresentationMethodStatisticMethodType " << rec.getPresentationMethodStatisticMethodType() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isSelectionsNull() )
		os << ", Selections: " << rec.getSelections();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
