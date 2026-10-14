//#include "vxWorks.h"
#include "WrapperRequestInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperRequestInquire::dataType = 0;
std::string WrapperRequestInquire::dataTypeName("WrapperRequestInquire"); 
#endif

WrapperRequestInquire::WrapperRequestInquire(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrReqInquire, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestInquire::WrapperRequestInquire(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestInquire::~WrapperRequestInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

