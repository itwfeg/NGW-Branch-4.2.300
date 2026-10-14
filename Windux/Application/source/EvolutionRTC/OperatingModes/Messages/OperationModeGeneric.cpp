//#include "vxWorks.h"
#include "OperationModeGeneric.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeGeneric::dataType = 0;
std::string OperationModeGeneric::dataTypeName("OperationModeGeneric"); 
#endif

OperationModeGeneric::OperationModeGeneric(OpModeEvent opModeEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(OperationMode, (UINT8)opModeEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeGeneric::OperationModeGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeGeneric::~OperationModeGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
