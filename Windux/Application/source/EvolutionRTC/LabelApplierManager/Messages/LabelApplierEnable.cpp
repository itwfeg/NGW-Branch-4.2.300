//#include "vxWorks.h"
#include "LabelApplierEnable.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierEnable::dataType = 0;
std::string LabelApplierEnable::dataTypeName("LabelApplierEnable"); 
#endif

LabelApplierEnable::LabelApplierEnable(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaEnable, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

LabelApplierEnable::LabelApplierEnable(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierEnable::~LabelApplierEnable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
