#include "edb.h"
#include DBCOOLTEXT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolText::DBCoolText(edb_adapter::DBCoolTextI* d)
		: decorated_(d)
{
}


DBCoolText::DBCoolText(const DBCoolText& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCoolText::~DBCoolText()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCoolText& rec)
{
	os << "DBCoolText Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isTextNull() )
		os << ", Text: " << rec.getText();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
