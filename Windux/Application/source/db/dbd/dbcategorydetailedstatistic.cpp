#include "edb.h"
#include DBCATEGORYDETAILEDSTATISTIC_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCategoryDetailedStatistic::DBCategoryDetailedStatistic(edb_adapter::DBCategoryDetailedStatisticI* d)
		: decorated_(d)
{
}


DBCategoryDetailedStatistic::DBCategoryDetailedStatistic(const DBCategoryDetailedStatistic& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCategoryDetailedStatistic::~DBCategoryDetailedStatistic()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCategoryDetailedStatistic& rec)
{
	os << "DBCategoryDetailedStatistic PresentationMethodType " << rec.getPresentationMethodType() <<
			", CategoryStatisticLanguage " << rec.getCategoryStatisticLanguage() <<
			", CategoryStatisticHourKey " << rec.getCategoryStatisticHourNumber() <<
			", CategoryNumber " << rec.getCategoryNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isSelectionsNull() )
		os << ", Selections: " << rec.getSelections();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
