#include "edb.h"
#include DBLABELTEXT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBLabelText::DBLabelText(edb_adapter::DBLabelTextI* d)
		: decorated_(d)
{
}


DBLabelText::DBLabelText(const DBLabelText& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBLabelText::~DBLabelText()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBLabelText& rec)
{
	os << "DBLabelText # " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isTextNull() )
		os << ", Text: " << rec.getText();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
