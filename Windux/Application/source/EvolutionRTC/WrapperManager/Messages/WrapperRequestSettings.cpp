//#include "vxWorks.h"
#include "WrapperRequestSettings.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperRequestSettings::dataType = 0;
std::string WrapperRequestSettings::dataTypeName("WrapperRequestSettings"); 
#endif

WrapperRequestSettings::WrapperRequestSettings(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrReqSettings, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestSettings::WrapperRequestSettings(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperRequestSettings::~WrapperRequestSettings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

