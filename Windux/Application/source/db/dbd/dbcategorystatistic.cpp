#include "edb.h"
#include DBCATEGORYSTATISTIC_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCategoryStatistic::DBCategoryStatistic(edb_adapter::DBCategoryStatisticI* d)
		: decorated_(d)
{
}


DBCategoryStatistic::DBCategoryStatistic(const DBCategoryStatistic& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCategoryStatistic::~DBCategoryStatistic()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCategoryStatistic& rec)
{
	os << "DBCategoryStatistic HourKey " << rec.getHourNumber() <<
			", Language " << rec.getLanguage() <<
			", CategoryNumber " << rec.getCategoryNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isTotalSelectionsNull() )
		os << ", TotalSelections: " << rec.getTotalSelections();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
