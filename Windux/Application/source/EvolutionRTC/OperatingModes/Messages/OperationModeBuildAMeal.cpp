//#include "vxWorks.h"
#include "OperationModeBuildAMeal.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeBuildAMeal::dataType = 0;
std::string OperationModeBuildAMeal::dataTypeName("OperationModeBuildAMeal"); 
#endif

OperationModeBuildAMeal::OperationModeBuildAMeal(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeBuildAMeal, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeBuildAMeal::OperationModeBuildAMeal(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeBuildAMeal::~OperationModeBuildAMeal()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
