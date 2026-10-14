//#include "vxWorks.h"
#include "OperationModeManualWeigh.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeManualWeigh::dataType = 0;
std::string OperationModeManualWeigh::dataTypeName("OperationModeManualWeigh"); 
#endif

OperationModeManualWeigh::OperationModeManualWeigh(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeManualWeigh, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeManualWeigh::OperationModeManualWeigh(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeManualWeigh::~OperationModeManualWeigh()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
