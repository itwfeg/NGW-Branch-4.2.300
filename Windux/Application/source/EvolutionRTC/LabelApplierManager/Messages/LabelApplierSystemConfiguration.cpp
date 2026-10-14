//#include "vxWorks.h"
#include "LabelApplierSystemConfiguration.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSystemConfiguration::dataType = 0;
std::string LabelApplierSystemConfiguration::dataTypeName("LabelApplierSystemConfiguration"); 
#endif

LabelApplierSystemConfiguration::LabelApplierSystemConfiguration(Hobart::CEvolMsgReceiver *pHobThread, SysConfigType sysConfigType,
																 UINT8 *deviceSignature, DeviceAddress stationAddress,
																 DeviceAddress controllerAddress) : SystemConfiguration(LabelApplierSystem, 
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

LabelApplierSystemConfiguration::LabelApplierSystemConfiguration(const char *buffer) : SystemConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemConfiguration::~LabelApplierSystemConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

