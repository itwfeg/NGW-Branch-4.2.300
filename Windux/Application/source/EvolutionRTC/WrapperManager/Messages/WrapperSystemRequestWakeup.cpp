//#include "vxWorks.h"
#include "WrapperSystemRequestWakeup.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSystemRequestWakeup::dataType = 0;
std::string WrapperSystemRequestWakeup::dataTypeName("WrapperSystemRequestWakeup"); 
#endif

WrapperSystemRequestWakeup::WrapperSystemRequestWakeup(Hobart::CEvolMsgReceiver *pHobThread) : SystemRequestWakeup(WrapperSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperSystemRequestWakeup::WrapperSystemRequestWakeup(const char *buffer) : SystemRequestWakeup(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperSystemRequestWakeup::~WrapperSystemRequestWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

