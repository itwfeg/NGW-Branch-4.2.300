//#include "vxWorks.h"
#include "LabelApplierSystemInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSystemInquire::dataType = 0;
std::string LabelApplierSystemInquire::dataTypeName("LabelApplierSystemInquire"); 
#endif

LabelApplierSystemInquire::LabelApplierSystemInquire(Hobart::CEvolMsgReceiver *pHobThread, SysConfigType sysConfigType,
													 UINT8 *deviceSignature, DeviceAddress stationAddress,
													 DeviceAddress controllerAddress) : SystemInquire(LabelApplierSystem,
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

LabelApplierSystemInquire::LabelApplierSystemInquire(const char *buffer) : SystemInquire(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemInquire::~LabelApplierSystemInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

