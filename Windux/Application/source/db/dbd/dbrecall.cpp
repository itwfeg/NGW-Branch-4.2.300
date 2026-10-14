#include "edb.h"
#include DBRECALL_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBRecall::DBRecall(edb_adapter::DBRecallI* d)
		: decorated_(d)
{
}


DBRecall::DBRecall(const DBRecall& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBRecall::~DBRecall()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBRecall& rec)
{
	os << "DBRecall # " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isNameNull() )
		os << ", Name: " << rec.getName();
	if ( !rec.isTextNull() )
		os << ", Text: " << rec.getText();
	if ( !rec.isReceivedDateTimeNull() )
		os << ", ReceivedDateTime: " << rec.getReceivedDateTime();
	if ( !rec.isAcknowledgedNull() )
		os << ", Acknowledged: " << rec.getAcknowledged();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
