//#include "vxWorks.h"
#include "OperationModeService.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeService::dataType = 0;
std::string OperationModeService::dataTypeName("OperationModeService"); 
#endif

OperationModeService::OperationModeService(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeService, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeService::OperationModeService(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeService::~OperationModeService()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
