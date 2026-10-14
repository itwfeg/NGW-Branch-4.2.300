//#include "vxWorks.h"
#include "OperationModePowerDown.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModePowerDown::dataType = 0;
std::string OperationModePowerDown::dataTypeName("OperationModePowerDown"); 
#endif

OperationModePowerDown::OperationModePowerDown(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModePowerDown, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModePowerDown::OperationModePowerDown(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModePowerDown::~OperationModePowerDown()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
