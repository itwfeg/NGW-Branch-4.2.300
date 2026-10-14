#include "edb.h"
#include DBCOOLCATEGORYCOUNTRY_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCategoryCountry::DBCoolCategoryCountry(edb_adapter::DBCoolCategoryCountryI* d)
		: decorated_(d)
{
}


DBCoolCategoryCountry::DBCoolCategoryCountry(const DBCoolCategoryCountry& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolCategoryCountry::~DBCoolCategoryCountry()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolCategoryCountry& rec)
{
	os << "DBCoolCategoryCountry CategoryNumber " << rec.getCategoryNumber() << ", CountryKey " <<
			rec.getCountryKey() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
