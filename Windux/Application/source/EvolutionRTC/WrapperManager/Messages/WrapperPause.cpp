//#include "vxWorks.h"
#include "WrapperPause.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperPause::dataType = 0;
std::string WrapperPause::dataTypeName("WrapperPause"); 
#endif

WrapperPause::WrapperPause(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrPause, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperPause::WrapperPause(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperPause::~WrapperPause()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

