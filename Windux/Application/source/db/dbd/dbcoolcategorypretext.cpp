#include "edb.h"
#include DBCOOLCATEGORYPRETEXT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCategoryPreText::DBCoolCategoryPreText(edb_adapter::DBCoolCategoryPreTextI* d)
		: decorated_(d)
{
}


DBCoolCategoryPreText::DBCoolCategoryPreText(const DBCoolCategoryPreText& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolCategoryPreText::~DBCoolCategoryPreText()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolCategoryPreText& rec)
{
	os << "DBCoolCategoryPreText CategoryNumber " << rec.getCategoryNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isPreTextNumberNull() )
		os << ", PreTextNumber: " << rec.getPreTextNumber();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
