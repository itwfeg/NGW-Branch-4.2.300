//#include "vxWorks.h"
#include "WrapperOpen.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperOpen::dataType = 0;
std::string WrapperOpen::dataTypeName("WrapperOpen"); 
#endif

WrapperOpen::WrapperOpen(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrOpen, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperOpen::WrapperOpen(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

WrapperOpen::~WrapperOpen()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

