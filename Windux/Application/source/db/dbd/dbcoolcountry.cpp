#include "edb.h"
#include DBCOOLCOUNTRY_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCountry::DBCoolCountry(edb_adapter::DBCoolCountryI* d)
		: decorated_(d)
{
}


DBCoolCountry::DBCoolCountry(const DBCoolCountry& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolCountry::~DBCoolCountry()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolCountry& rec)
{
	os << "DBCoolCountry Key " << rec.getCountryKey() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";

	if (!rec.isDeptNumberNull())
		os << ", Department: " << rec.getDeptNumber();
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();
	if ( !rec.isAllCategoriesNull() )
		os << ", AllCategories: " << rec.getAllCategories();
	if ( !rec.isCountryNumberNull() )
		os << ", CountryNumber: " << rec.getCountryNumber();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
