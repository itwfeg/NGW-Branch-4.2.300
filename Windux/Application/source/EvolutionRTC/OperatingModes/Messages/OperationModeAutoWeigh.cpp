//#include "vxWorks.h"
#include "OperationModeAutoWeigh.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeAutoWeigh::dataType = 0;
std::string OperationModeAutoWeigh::dataTypeName("OperationModeAutoWeigh"); 
#endif

OperationModeAutoWeigh::OperationModeAutoWeigh(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeAutoWeigh, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeAutoWeigh::OperationModeAutoWeigh(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeAutoWeigh::~OperationModeAutoWeigh()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
