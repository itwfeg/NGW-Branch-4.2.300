//#include "vxWorks.h"
#include "LabelApplierRequestOff.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierRequestOff::dataType = 0;
std::string LabelApplierRequestOff::dataTypeName("LabelApplierRequestOff"); 
#endif

LabelApplierRequestOff::LabelApplierRequestOff(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaReqOff, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestOff::LabelApplierRequestOff(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestOff::~LabelApplierRequestOff()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

