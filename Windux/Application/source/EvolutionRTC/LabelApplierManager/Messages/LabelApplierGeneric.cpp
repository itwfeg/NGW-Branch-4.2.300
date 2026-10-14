//#include "vxWorks.h"
#include "LabelApplierGeneric.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierGeneric::dataType = 0;
std::string LabelApplierGeneric::dataTypeName("LabelApplierGeneric"); 
#endif

LabelApplierGeneric::LabelApplierGeneric(LaEvent laEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(LabelApplier, (UINT8)laEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierGeneric::LabelApplierGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierGeneric::~LabelApplierGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
