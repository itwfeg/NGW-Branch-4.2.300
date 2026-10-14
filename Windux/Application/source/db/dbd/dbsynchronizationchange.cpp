#include "edb.h"
#include DBSYNCHRONIZATIONCHANGE_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSynchronizationChange::DBSynchronizationChange(edb_adapter::DBSynchronizationChangeI* d)
		: decorated_(d)
{
}


DBSynchronizationChange::DBSynchronizationChange(const DBSynchronizationChange& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBSynchronizationChange::~DBSynchronizationChange()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBSynchronizationChange& rec)
{
	os << "DBSynchronizationChange # " << rec.getIdentifier() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isApplicationDeviceNull() )
		os << ", Applicatoin Device # : " << rec.getApplicationDevice();
	if ( !rec.isApplicationEventNull() )
		os << ", Application Event: " << rec.getApplicationEvent();
	if ( !rec.isApplicationFormatNull() )
		os << ", Application Format: " << rec.getApplicationFormat();
	if ( !rec.isChangeNull() )
		os << ", Change: " << rec.getChange();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
