//#include "vxWorks.h"
#include "SystemStatusInfo.h"
#include "WrapperSystemStatus.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSystemStatus::dataType = 0;
std::string WrapperSystemStatus::dataTypeName("WrapperSystemStatus"); 
#endif

WrapperSystemStatus::WrapperSystemStatus(Hobart::CEvolMsgReceiver *pHobThread, SystemStatusInfo *systemStatus) : SystemStatus(WrapperSystem, pHobThread, systemStatus)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemStatus::WrapperSystemStatus(const char *buffer) : SystemStatus(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemStatus::~WrapperSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
void WrapperSystemStatus::show()
{
	Debug::addTraceCategory("WrapperSystemStatus");
	Debug::trace("WrapperSystemStatus", "WrapperSystemStatus: ");

	SystemStatus::show();

	Debug::removeTraceCategory("WrapperSystemStatus");
}
