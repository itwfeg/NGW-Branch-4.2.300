//#include "vxWorks.h"
#include "LabelApplierSystemRequestWakeup.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSystemRequestWakeup::dataType = 0;
std::string LabelApplierSystemRequestWakeup::dataTypeName("LabelApplierSystemRequestWakeup"); 
#endif

LabelApplierSystemRequestWakeup::LabelApplierSystemRequestWakeup(Hobart::CEvolMsgReceiver *pHobThread) : SystemRequestWakeup(LabelApplierSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemRequestWakeup::LabelApplierSystemRequestWakeup(const char *buffer) : SystemRequestWakeup(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemRequestWakeup::~LabelApplierSystemRequestWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

