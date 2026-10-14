//#include "vxWorks.h"
#include "LabelApplierSystemWakeup.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSystemWakeup::dataType = 0;
std::string LabelApplierSystemWakeup::dataTypeName("LabelApplierSystemWakeup"); 
#endif

LabelApplierSystemWakeup::LabelApplierSystemWakeup(Hobart::CEvolMsgReceiver *pHobThread, SysCfgStatus sysCfgStatus) : SystemWakeup(WrapperSystem, pHobThread, sysCfgStatus)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemWakeup::LabelApplierSystemWakeup(const char *buffer) : SystemWakeup(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemWakeup::~LabelApplierSystemWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void LabelApplierSystemWakeup::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("LabelApplierSystemWakeup") );
#else
	Debug::addTraceCategory("LabelApplierSystemWakeup");
#endif
	Debug::trace("LabelApplierSystemWakeup", "LabelApplierSystemWakeup: ");

	SystemWakeup::show();

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("LabelApplierSystemWakeup") );
#else
	Debug::removeTraceCategory("LabelApplierSystemWakeup");
#endif
}
