//#include "vxWorks.h"
#include "OperationModeMonitor.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeMonitor::dataType = 0;
std::string OperationModeMonitor::dataTypeName("OperationModeMonitor"); 
#endif

OperationModeMonitor::OperationModeMonitor(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeMonitor, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeMonitor::OperationModeMonitor(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeMonitor::~OperationModeMonitor()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
