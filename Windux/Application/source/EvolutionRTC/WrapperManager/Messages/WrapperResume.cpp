//#include "vxWorks.h"
#include "WrapperResume.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperResume::dataType = 0;
std::string WrapperResume::dataTypeName("WrapperResume"); 
#endif

WrapperResume::WrapperResume(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrResume, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperResume::WrapperResume(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperResume::~WrapperResume()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

