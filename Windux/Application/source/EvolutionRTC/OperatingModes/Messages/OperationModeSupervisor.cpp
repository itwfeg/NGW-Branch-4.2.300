//#include "vxWorks.h"
#include "OperationModeSupervisor.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeSupervisor::dataType = 0;
std::string OperationModeSupervisor::dataTypeName("OperationModeSupervisor"); 
#endif

OperationModeSupervisor::OperationModeSupervisor(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeSupervisor, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeSupervisor::OperationModeSupervisor(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeSupervisor::~OperationModeSupervisor()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
