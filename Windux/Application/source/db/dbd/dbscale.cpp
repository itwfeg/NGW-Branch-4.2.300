#include "edb.h"
#include DBSCALE_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBScale::DBScale(edb_adapter::DBScaleI* d)
		: decorated_(d)
{
}


DBScale::DBScale(const DBScale& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBScale::~DBScale()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBScale& rec)
{
	os << "DBScale Identifier " << rec.getIdentifier() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isTypeNull() )
		os << ", Type: " << rec.getType();
	if ( !rec.isHostNameNull() )
		os << ", HostName: " << rec.getHostName();
	if ( !rec.isIPAddressNull() )
		os << ", IPAddress: " << rec.getIPAddress();
	if ( !rec.isCreationDateTimeNull() )
		os << ", CreationDateTime: " << rec.getCreationDateTime();
	if ( !rec.isStateNull() )
		os << ", State: " << rec.getState();
	if ( !rec.isStateChangedDateTimeNull() )
		os << ", StateChangedDateTime: " << rec.getStateChangedDateTime();
	if ( !rec.isRegisteringNull() )
		os << ", Registering: " << rec.isRegistering();
	if ( !rec.isDeregisteringNull() )
		os << ", Deregistering: " << rec.isDeregistering();
	if ( !rec.isNotifiedBatchesNull() )
		os << ", NotifiedBatches: " << rec.isNotifiedBatches();
	if ( !rec.isNotifiedBatchesDateTimeNull() )
		os << ", NotifiedBatchesDateTime: " << rec.getNotifiedBatchesDateTime();
	if ( !rec.isNotifiedOnlineNull() )
		os << ", NotifiedOnline: " << rec.isNotifiedOnline();
	if ( !rec.isNotifiedOnlineDateTimeNull() )
		os << ", NotifiedOnlineDateTime: " << rec.getNotifiedOnlineDateTime();
	if ( !rec.isRetryCountNull() )
		os << ", RetryCount: " << rec.getRetryCount();
	if ( !rec.isVersionNull() )
		os << ", Version: " << rec.getVersion();
	if ( !rec.isVersionReceivedNull() )
		os << ", VersionReceived: " << rec.getVersionReceived();
	if ( !rec.isVersionReceivedDateTimeNull() )
		os << ", VersionReceivedDateTime: " << rec.getVersionReceivedDateTime();
	if ( !rec.isLastSuccessfulCommDateTimeNull() )
		os << ", LastSuccessfulCommDateTime: " << rec.getLastSuccessfulCommDateTime();
	if ( !rec.isFirstFailedCommDateTimeNull() )
		os << ", FirstFailedCommDateTime: " << rec.getFirstFailedCommDateTime();
	if ( !rec.isTotalSuccessfulCommunicationsNull() )
		os << ", TotalSuccessfulCommunications: " << rec.getTotalSuccessfulCommunications();
	if ( !rec.isTotalFailedCommunicationsNull() )
		os << ", TotalFailedCommunications: " << rec.getTotalFailedCommunications();
	if ( !rec.isTotalBusyCommunicationsNull() )
		os << ", TotalBusyCommunications: " << rec.getTotalBusyCommunications();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
