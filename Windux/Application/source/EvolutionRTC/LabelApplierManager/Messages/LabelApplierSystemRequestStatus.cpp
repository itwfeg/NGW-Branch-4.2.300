//#include "vxWorks.h"
#include "LabelApplierSystemRequestStatus.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSystemRequestStatus::dataType = 0;
std::string LabelApplierSystemRequestStatus::dataTypeName("LabelApplierSystemRequestStatus"); 
#endif

LabelApplierSystemRequestStatus::LabelApplierSystemRequestStatus(Hobart::CEvolMsgReceiver *pHobThread) : SystemRequestStatus(LabelApplierSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemRequestStatus::LabelApplierSystemRequestStatus(const char *buffer) : SystemRequestStatus(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierSystemRequestStatus::~LabelApplierSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

