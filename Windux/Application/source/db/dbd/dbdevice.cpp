#include "edb.h"
#include DBDEVICE_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBDevice::DBDevice(edb_adapter::DBDeviceI* d)
		: decorated_(d)
{
}


DBDevice::DBDevice(const DBDevice& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBDevice::~DBDevice()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBDevice& rec)
{
	os << "DBDevice Type " << rec.getType() << ", Subtype " << rec.getSubtype() <<
			" (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isKindNull() )
		os << "Kind: " << rec.getKind();
	if ( !rec.isSerialNumberNull() )
		os << ", SerialNumber: " << rec.getSerialNumber();
	if ( !rec.isHardwareVersionNull() )
		os << ", HardwareVersion: " << rec.getHardwareVersion();
	if ( !rec.isSoftwareVersionNull() )
		os << ", SoftwareVersion: " << rec.getSoftwareVersion();
	if ( !rec.isCheckPresenceNull() )
		os << ", CheckPresence: " << rec.getCheckPresence();
	if ( !rec.isInstallationDateNull() )
		os << ", InstallationDate: " << rec.getInstallationDate();
	if ( !rec.isServiceHistoryNull() )
		os << ", ServiceHistory: " << rec.getServiceHistory();
	if ( !rec.isUserNameNull() )
		os << ", UserName: " << rec.getUserName();
	if ( !rec.isSystemNameNull() )
		os << ", SystemName: " << rec.getSystemName();
	if ( !rec.isIPAddressNull() )
		os << ", IPAddress: " << rec.getIPAddress();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
