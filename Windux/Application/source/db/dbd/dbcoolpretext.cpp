#include "edb.h"
#include DBCOOLPRETEXT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolPreText::DBCoolPreText(edb_adapter::DBCoolPreTextI* d)
		: decorated_(d)
{
}


DBCoolPreText::DBCoolPreText(const DBCoolPreText& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolPreText::~DBCoolPreText()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolPreText& rec)
{
	os << "DBCoolPreText Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();
	if ( !rec.isAllCategoriesNull() )
		os << ", AllCategories: " << rec.getAllCategories();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
