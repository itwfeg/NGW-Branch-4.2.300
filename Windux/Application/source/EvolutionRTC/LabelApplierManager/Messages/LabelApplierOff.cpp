//#include "vxWorks.h"
#include "LabelApplierOff.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierOff::dataType = 0;
std::string LabelApplierOff::dataTypeName("LabelApplierOff"); 
#endif

LabelApplierOff::LabelApplierOff(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaOff, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierOff::LabelApplierOff(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierOff::~LabelApplierOff()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
