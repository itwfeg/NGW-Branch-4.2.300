//#include "vxWorks.h"
#include "WrapperRequestStatus.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperRequestStatus::dataType = 0;
std::string WrapperRequestStatus::dataTypeName("WrapperRequestStatus"); 
#endif

WrapperRequestStatus::WrapperRequestStatus(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrReqStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestStatus::WrapperRequestStatus(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperRequestStatus::~WrapperRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

