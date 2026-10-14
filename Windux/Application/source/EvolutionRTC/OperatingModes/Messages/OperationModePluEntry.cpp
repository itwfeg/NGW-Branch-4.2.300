//#include "vxWorks.h"
#include "OperationModePluEntry.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperationModePluEntry::dataType = 0;
std::string OperationModePluEntry::dataTypeName("OperationModePluEntry"); 
#endif

OperationModePluEntry::OperationModePluEntry(Hobart::CEvolMsgReceiver *pHobThread) : OperationModeGeneric(OpModePluEntry, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModePluEntry::OperationModePluEntry(const char *buffer) : OperationModeGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

OperationModePluEntry::~OperationModePluEntry()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
