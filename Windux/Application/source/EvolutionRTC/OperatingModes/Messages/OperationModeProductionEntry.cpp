//#include "vxWorks.h"
#include "OperationModeProductionEntry.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModeProductionEntry::dataType = 0;
std::string OperationModeProductionEntry::dataTypeName("OperationModeProductionEntry"); 
#endif

OperationModeProductionEntry::OperationModeProductionEntry(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModeProductionEntry, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeProductionEntry::OperationModeProductionEntry(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModeProductionEntry::~OperationModeProductionEntry()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
