//#include "vxWorks.h"
#include "WrapperRequestPackageHistory.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperRequestPackageHistory::dataType = 0;
std::string WrapperRequestPackageHistory::dataTypeName("WrapperRequestPackageHistory"); 
#endif

WrapperRequestPackageHistory::WrapperRequestPackageHistory(Hobart::CEvolMsgReceiver *pHobThread)
		: WrapperGeneric(WrReqPackageHistory, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


WrapperRequestPackageHistory::WrapperRequestPackageHistory(const char *buffer)
		: WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


WrapperRequestPackageHistory::~WrapperRequestPackageHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
