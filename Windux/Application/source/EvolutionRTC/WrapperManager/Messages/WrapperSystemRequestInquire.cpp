//#include "vxWorks.h"
#include "WrapperSystemRequestInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSystemRequestInquire::dataType = 0;
std::string WrapperSystemRequestInquire::dataTypeName("WrapperSystemRequestInquire"); 
#endif

WrapperSystemRequestInquire::WrapperSystemRequestInquire(Hobart::CEvolMsgReceiver *pHobThread) : SystemRequestInquire(WrapperSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperSystemRequestInquire::WrapperSystemRequestInquire(const char *buffer) : SystemRequestInquire(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperSystemRequestInquire::~WrapperSystemRequestInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

