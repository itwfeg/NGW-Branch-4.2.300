#include "edb.h"
#include DBNETWORKERROR_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBNetworkError::DBNetworkError(edb_adapter::DBNetworkErrorI* d)
		: decorated_(d)
{
}


DBNetworkError::DBNetworkError(const DBNetworkError& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBNetworkError::~DBNetworkError()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBNetworkError& rec)
{
	os << "DBNetworkError # " << rec.getNetworkErrorKey() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isErrorCodeNull() )
		os << ", Error Code: " << rec.getErrorCode();
	if ( !rec.isRecordTypeNull() )
		os << ", Record Type # : " << rec.getRecordType();
	if ( !rec.isDateTimeNull() )
		os << "Date/Time: " << rec.getDateTime();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
