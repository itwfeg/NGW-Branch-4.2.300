//#include "vxWorks.h"
#include "LabelApplierOn.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierOn::dataType = 0;
std::string LabelApplierOn::dataTypeName("LabelApplierOn"); 
#endif

LabelApplierOn::LabelApplierOn(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaOn, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierOn::LabelApplierOn(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierOn::~LabelApplierOn()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
