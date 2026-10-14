#include "edb.h"
#include DBEMAILALERT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBEmailAlert::DBEmailAlert(edb_adapter::DBEmailAlertI* d)
		: decorated_(d)
{
}


DBEmailAlert::DBEmailAlert(const DBEmailAlert& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBEmailAlert::~DBEmailAlert()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBEmailAlert& rec)
{
	os << "DBEmailAlert Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isNotifyOnFailureNull() )
		os << "NotifyOnFailure: " << rec.getNotifyOnFailure();
	if ( !rec.isNotifyOnResolutionNull() )
		os << ", NotifyOnResolution: " << rec.getNotifyOnResolution();
	if (!rec.isNotifyHandsFreeDeviceOnFailureNull())
		os << "NotifyHandsFreeDeviceOnFailure: " << rec.getNotifyHandsFreeDeviceOnFailure();
	if (!rec.isNotifyHandsFreeDeviceOnResolutionNull())
		os << ", NotifyHandsFreeDeviceOnResolution: " << rec.getNotifyHandsFreeDeviceOnResolution();
	if ( !rec.isDelayNull() )
		os << ", Delay: " << rec.getDelay();
   if ( !rec.isFailureSentNull() )
   	os << ", FailureSent: " << rec.getFailureSent();
	if ( !rec.isSubject1Null() )
		os << ", Subject1: " << rec.getSubject1();
	if ( !rec.isSubject2Null() )
		os << ", Subject2: " << rec.getSubject2();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
