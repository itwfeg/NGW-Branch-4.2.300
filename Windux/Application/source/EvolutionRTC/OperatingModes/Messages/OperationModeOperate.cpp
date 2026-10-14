//#include "vxWorks.h"
#include "OperationModeOperate.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeOperate::dataType = 0;
std::string OperationModeOperate::dataTypeName("OperationModeOperate"); 
#endif

OperationModeOperate::OperationModeOperate(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeOperate, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeOperate::OperationModeOperate(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeOperate::~OperationModeOperate()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
