#include "edb.h"
#include DBWRAPPERERRORHISTORY_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBWrapperErrorHistory::DBWrapperErrorHistory(edb_adapter::DBWrapperErrorHistoryI* d)
		: decorated_(d)
{
}


DBWrapperErrorHistory::DBWrapperErrorHistory(const DBWrapperErrorHistory& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBWrapperErrorHistory::~DBWrapperErrorHistory()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os,  DBWrapperErrorHistory& rec)
{

	os << "DBWrapperErrorHistory errorCode " << rec.getErrorCode() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
/* to do -- ats
	if ( !rec.isKnifeTimeNull() )
		os << "KnifeTime: " << rec.getKnifeTime();
	if ( !rec.isTimeStampDateTimeNull() )
		os << ", Date Time: " << rec.getTimeStampDateTime();
*/
	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
