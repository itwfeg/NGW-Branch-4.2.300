#include "edb.h"
#include DBCOOLCATEGORY_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCategory::DBCoolCategory(edb_adapter::DBCoolCategoryI* d)
		: decorated_(d)
{
}


DBCoolCategory::DBCoolCategory(const DBCoolCategory& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolCategory::~DBCoolCategory()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolCategory& rec)
{
	os << "DBCoolCategory Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << ", Department: " << rec.getDeptNumber();
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
