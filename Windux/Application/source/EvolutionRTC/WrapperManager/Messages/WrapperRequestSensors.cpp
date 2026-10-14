//#include "vxWorks.h"
#include "WrapperRequestSensors.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperRequestSensors::dataType = 0;
std::string WrapperRequestSensors::dataTypeName("WrapperRequestSensors"); 
#endif

WrapperRequestSensors::WrapperRequestSensors(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrReqSensors, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestSensors::WrapperRequestSensors(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestSensors::~WrapperRequestSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

