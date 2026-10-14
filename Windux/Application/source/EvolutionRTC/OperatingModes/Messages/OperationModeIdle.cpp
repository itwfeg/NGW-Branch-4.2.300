//#include "vxWorks.h"
#include "OperationModeIdle.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeIdle::dataType = 0;
std::string OperationModeIdle::dataTypeName("OperationModeIdle"); 
#endif

OperationModeIdle::OperationModeIdle(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeIdle, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeIdle::OperationModeIdle(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeIdle::~OperationModeIdle()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
