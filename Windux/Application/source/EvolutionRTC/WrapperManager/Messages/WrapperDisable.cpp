//#include "vxWorks.h"
#include "WrapperDisable.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperDisable::dataType = 0;
std::string WrapperDisable::dataTypeName("WrapperDisable"); 
#endif

WrapperDisable::WrapperDisable(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrDisable, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperDisable::WrapperDisable(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperDisable::~WrapperDisable()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

