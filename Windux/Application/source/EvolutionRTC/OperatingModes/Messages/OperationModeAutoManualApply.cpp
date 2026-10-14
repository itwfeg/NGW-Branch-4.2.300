//#include "vxWorks.h"
#include "OperationModeAutoManualApply.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeAutoManualApply::dataType = 0;
std::string OperationModeAutoManualApply::dataTypeName("OperationModeAutoManualApply"); 
#endif

OperationModeAutoManualApply::OperationModeAutoManualApply(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeAutoManualApply, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeAutoManualApply::OperationModeAutoManualApply(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeAutoManualApply::~OperationModeAutoManualApply()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
