#include "edb.h"
#include DBSTATUS_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBStatus::DBStatus(edb_adapter::DBStatusI* d)
		: decorated_(d)
{
}


DBStatus::DBStatus(const DBStatus& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBStatus::~DBStatus()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBStatus& rec)
{
	os << "DBStatus Name " << rec.getName() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isValueNull() )
		os << ", Value: " << rec.getValue();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
