//#include "vxWorks.h"
#include "WeigherEvent.h"
#include "WeigherInquire.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherInquire::dataType = 0;
std::string WeigherInquire::dataTypeName("WeigherInquire"); 
#endif

WeigherInquire::WeigherInquire(Hobart::CEvolMsgReceiver *pHobThread) : WeigherConfiguration(pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	setEventType(WgInquire);
}

WeigherInquire::WeigherInquire(const char *buffer) : WeigherConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherInquire::~WeigherInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void WeigherInquire::show()
{

	Debug::trace("WeigherInquire", "WeigherInquire: ");

	WeigherConfiguration::show();

	Debug::removeTraceCategory("WeigherInquire");
}
