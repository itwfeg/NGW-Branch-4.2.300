//#include "vxWorks.h"
#include "LabelApplierRequestSensors.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierRequestSensors::dataType = 0;
std::string LabelApplierRequestSensors::dataTypeName("LabelApplierRequestSensors"); 
#endif

LabelApplierRequestSensors::LabelApplierRequestSensors(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaReqSensors, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestSensors::LabelApplierRequestSensors(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestSensors::~LabelApplierRequestSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

