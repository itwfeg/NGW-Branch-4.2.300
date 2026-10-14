#include "edb.h"
#include DBOPERATORNOTE_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBOperatorNote::DBOperatorNote(edb_adapter::DBOperatorNoteI* d)
		: decorated_(d)
{
}


DBOperatorNote::DBOperatorNote(const DBOperatorNote& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBOperatorNote::~DBOperatorNote()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBOperatorNote& rec)
{
	os << "DBOperatorNote (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isTextNull() )
		os << ", OperatorNote: " << rec.getText();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
