//#include "vxWorks.h"
#include "OperationModeWrapOnly.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeWrapOnly::dataType = 0;
std::string OperationModeWrapOnly::dataTypeName("OperationModeWrapOnly"); 
#endif

OperationModeWrapOnly::OperationModeWrapOnly(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeWrapOnly, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeWrapOnly::OperationModeWrapOnly(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeWrapOnly::~OperationModeWrapOnly()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
