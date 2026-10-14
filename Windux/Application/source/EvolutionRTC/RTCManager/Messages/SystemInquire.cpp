//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemInquire.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemInquire::_dataType_ = 0;
std::string SystemInquire::_dataTypeName_("SystemInquire"); 
#endif

SystemInquire::SystemInquire(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread,
							   SysConfigType sysConfigType, UINT8 *deviceSignature,
							   DeviceAddress stationAddress, DeviceAddress controllerAddress) : SystemConfiguration(evolDevType, pHobThread, 
							   																						sysConfigType, deviceSignature,
							   																						stationAddress, controllerAddress)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	setEventType(SysInquire);
}

SystemInquire::SystemInquire(const char *buffer) : SystemConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemInquire::~SystemInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

void SystemInquire::show()
{
	Debug::addTraceCategory( std::string("SystemInquire") );
	Debug::trace("SystemInquire", "SystemInquire: ");

	SystemConfiguration::show();

	Debug::removeTraceCategory( std::string("SystemInquire") );
}
