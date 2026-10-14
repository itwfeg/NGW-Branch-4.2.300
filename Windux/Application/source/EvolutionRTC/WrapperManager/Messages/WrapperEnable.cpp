//#include "vxWorks.h"
#include "WrapperEnable.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperEnable::dataType = 0;
std::string WrapperEnable::dataTypeName("WrapperEnable"); 
#endif

WrapperEnable::WrapperEnable(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEnable, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperEnable::WrapperEnable(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperEnable::~WrapperEnable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

