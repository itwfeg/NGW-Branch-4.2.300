//#include "vxWorks.h"
#include "LabelApplierRequestStatus.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierRequestStatus::dataType = 0;
std::string LabelApplierRequestStatus::dataTypeName("LabelApplierRequestStatus"); 
#endif

LabelApplierRequestStatus::LabelApplierRequestStatus(Hobart::CEvolMsgReceiver *pHobThread) : LabelApplierGeneric(LaReqStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestStatus::LabelApplierRequestStatus(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierRequestStatus::~LabelApplierRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

