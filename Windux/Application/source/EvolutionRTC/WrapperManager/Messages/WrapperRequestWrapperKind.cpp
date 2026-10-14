//#include "vxWorks.h"
#include "WrapperRequestWrapperKind.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperRequestWrapperKind::dataType = 0;
std::string WrapperRequestWrapperKind::dataTypeName("WrapperRequestWrapperKind"); 
#endif

WrapperRequestWrapperKind::WrapperRequestWrapperKind(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrReqKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestWrapperKind::WrapperRequestWrapperKind(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestWrapperKind::~WrapperRequestWrapperKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

