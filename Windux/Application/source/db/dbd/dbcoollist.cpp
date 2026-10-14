#include "edb.h"
#include DBCOOLLIST_INC
#include DBCOOLLISTTEXT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolList::DBCoolList(edb_adapter::DBCoolListI* d)
		: decorated_(d)
{
}


DBCoolList::DBCoolList(const DBCoolList& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolList::~DBCoolList()
{
	if ( decorated_ )
		delete decorated_;
}


bool DBCoolList::containsCoolListText(const DBCoolListText* text) const
{
	if ( text && decorated_ )
		return decorated_->containsCoolListText(text->decorated_);

	return false;
}


std::ostream& operator<<(std::ostream& os, const DBCoolList& rec)
{
	os << "DBCoolList (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isNumberNull() )
		os << ", Number: " << rec.getNumber();
	if ( !rec.isFirstLinkNumberNull() )
		os << ", FirstLinkNumber: " << rec.getFirstLinkNumber();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
