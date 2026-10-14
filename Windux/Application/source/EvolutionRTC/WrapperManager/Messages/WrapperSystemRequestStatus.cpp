//#include "vxWorks.h"
#include "WrapperSystemRequestStatus.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSystemRequestStatus::dataType = 0;
std::string WrapperSystemRequestStatus::dataTypeName("WrapperSystemRequestStatus"); 
#endif

WrapperSystemRequestStatus::WrapperSystemRequestStatus(Hobart::CEvolMsgReceiver *pHobThread) : SystemRequestStatus(WrapperSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemRequestStatus::WrapperSystemRequestStatus(const char *buffer) : SystemRequestStatus(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemRequestStatus::~WrapperSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

