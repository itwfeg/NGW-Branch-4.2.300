#include "edb.h"
#include DBPRODUCTTEXT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBProductText::DBProductText(edb_adapter::DBProductTextI* d)
		: decorated_(d)
{
}


DBProductText::DBProductText(const DBProductText& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBProductText::~DBProductText()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBProductText& rec)
{
	os << "DBProductText Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isTypeNull() )
		os << ", Type: " << rec.getType();
	if ( !rec.isTextNull() )
		os << ", Text: " << rec.getText();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
