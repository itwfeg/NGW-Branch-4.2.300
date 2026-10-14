#include "edb.h"
#include DBCOOLLISTTEXT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolListText::DBCoolListText(edb_adapter::DBCoolListTextI* d)
		: decorated_(d)
{
}


DBCoolListText::DBCoolListText(const DBCoolListText& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolListText::~DBCoolListText()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolListText& rec)
{
	os << "DBCoolListText CoolTextNumber " << rec.getCoolTextNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isOrderNull() )
		os << ", Order: " << rec.getOrder();
	if ( !rec.isLinkNumberNull() )
		os << ", LinkNumber: " << rec.getLinkNumber();
	if ( !rec.isNextLinkNumberNull() )
		os << ", NextLinkNumber: " << rec.getNextLinkNumber();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
