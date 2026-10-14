//#include "vxWorks.h"
#include "OperationModeAccumulateItems.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeAccumulateItems::dataType = 0;
std::string OperationModeAccumulateItems::dataTypeName("OperationModeAccumulateItems"); 
#endif

OperationModeAccumulateItems::OperationModeAccumulateItems(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeAccumulateItems, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeAccumulateItems::OperationModeAccumulateItems(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeAccumulateItems::~OperationModeAccumulateItems()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
