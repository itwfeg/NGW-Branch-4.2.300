//#include "vxWorks.h"
#include "WrapperAbort.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperAbort::dataType = 0;
std::string WrapperAbort::dataTypeName("WrapperAbort"); 
#endif

WrapperAbort::WrapperAbort(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrAbort, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperAbort::WrapperAbort(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperAbort::~WrapperAbort()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

