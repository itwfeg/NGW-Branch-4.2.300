//#include "vxWorks.h"
#include "WrapperSystemInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSystemInquire::dataType = 0;
std::string WrapperSystemInquire::dataTypeName("WrapperSystemInquire"); 
#endif

WrapperSystemInquire::WrapperSystemInquire(Hobart::CEvolMsgReceiver *pHobThread, SysConfigType sysConfigType,
										   UINT8 *deviceSignature, DeviceAddress stationAddress,
										   DeviceAddress controllerAddress) : SystemInquire(WrapperSystem,
										   													pHobThread,
																							sysConfigType,
										   													deviceSignature,
										   													stationAddress,
										   													controllerAddress)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemInquire::WrapperSystemInquire(const char *buffer) : SystemInquire(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemInquire::~WrapperSystemInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

