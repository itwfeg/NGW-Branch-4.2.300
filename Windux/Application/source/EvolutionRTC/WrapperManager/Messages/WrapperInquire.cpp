//#include "vxWorks.h"
#include "WrapperInquire.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperInquire::dataType = 0;
std::string WrapperInquire::dataTypeName("WrapperInquire"); 
#endif

WrapperInquire::WrapperInquire(Hobart::CEvolMsgReceiver *pHobThread) : WrapperConfiguration(pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	setEventType(WrInquire);
}

WrapperInquire::WrapperInquire(const char *buffer) : WrapperConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WrapperInquire::~WrapperInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void WrapperInquire::show()
{
	Debug::addTraceCategory("WrapperInquire");
	Debug::trace("WrapperInquire", "WrapperInquire: ");

	WrapperConfiguration::show();

	Debug::removeTraceCategory("WrapperInquire");
}
