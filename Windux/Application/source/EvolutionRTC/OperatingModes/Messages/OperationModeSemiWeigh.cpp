//#include "vxWorks.h"
#include "OperationModeSemiWeigh.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeSemiWeigh::dataType = 0;
std::string OperationModeSemiWeigh::dataTypeName("OperationModeSemiWeigh"); 
#endif

OperationModeSemiWeigh::OperationModeSemiWeigh(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeSemiWeigh, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeSemiWeigh::OperationModeSemiWeigh(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeSemiWeigh::~OperationModeSemiWeigh()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
