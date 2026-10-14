//#include "vxWorks.h"
#include "WrapperReset.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperReset::dataType = 0;
std::string WrapperReset::dataTypeName("WrapperReset"); 
#endif

WrapperReset::WrapperReset(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrReset, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperReset::WrapperReset(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperReset::~WrapperReset()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

