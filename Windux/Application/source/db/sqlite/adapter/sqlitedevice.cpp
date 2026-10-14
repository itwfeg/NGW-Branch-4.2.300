#include "sqlitedevice.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteDevice::TYPE_FIELD              ("DvType");
const string SqliteDevice::SUBTYPE_FIELD           ("DvSType");
const string SqliteDevice::KIND_FIELD              ("DvKind");
const string SqliteDevice::SERIALNUMBER_FIELD      ("DvSrlNum");
const string SqliteDevice::HARDWAREVERSION_FIELD   ("DvHWVer");
const string SqliteDevice::SOFTWAREVERSION_FIELD   ("DvSWVer");
const string SqliteDevice::CHECKPRESENCE_FIELD     ("DvChkPr");
const string SqliteDevice::INSTALLATIONDATE_FIELD  ("DvInstDt");
const string SqliteDevice::SERVICEHISTORY_FIELD    ("DvSrvHst");
const string SqliteDevice::USERNAME_FIELD          ("DvUserName");
const string SqliteDevice::SYSTEMNAME_FIELD        ("DvSystemName");
const string SqliteDevice::IPADDRESS_FIELD         ("DvIPAddress");


#ifdef USE_ACCOUNTANT
int SqliteDevice::_dataType_ = 0;
string SqliteDevice::_dataTypeName_("SqliteDevice");
#endif



SqliteDevice::SqliteDevice(SqliteRecord* r, DBDeviceI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteDevice::~SqliteDevice()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( rec_ )
		delete rec_;

	if ( decorated_ )
		delete decorated_;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
