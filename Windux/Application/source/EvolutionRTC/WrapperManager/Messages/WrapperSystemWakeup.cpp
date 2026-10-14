//#include "vxWorks.h"
#include "WrapperSystemWakeup.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSystemWakeup::dataType = 0;
std::string WrapperSystemWakeup::dataTypeName("WrapperSystemWakeup"); 
#endif

WrapperSystemWakeup::WrapperSystemWakeup(Hobart::CEvolMsgReceiver *pHobThread, SysCfgStatus sysCfgStatus) : SystemWakeup(WrapperSystem, pHobThread, sysCfgStatus)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperSystemWakeup::WrapperSystemWakeup(const char *buffer) : SystemWakeup(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperSystemWakeup::~WrapperSystemWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
void WrapperSystemWakeup::show()
{
	Debug::addTraceCategory("WrapperSystemWakeup");
	Debug::trace("WrapperSystemWakeup", "WrapperSystemWakeup: ");

	SystemWakeup::show();

	Debug::removeTraceCategory("WrapperSystemWakeup");
}
