//#include "vxWorks.h"
#include "WrapperGeneric.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperGeneric::dataType = 0;
std::string WrapperGeneric::dataTypeName("WrapperGeneric"); 
#endif

WrapperGeneric::WrapperGeneric(WrEvent wrEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Wrapper, (UINT8)wrEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperGeneric::WrapperGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperGeneric::~WrapperGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
