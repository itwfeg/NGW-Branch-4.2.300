//#include "vxWorks.h"
#include "WeigherSystemReset.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherSystemReset::dataType = 0;
std::string WeigherSystemReset::dataTypeName("WeigherSystemReset"); 
#endif

WeigherSystemReset::WeigherSystemReset(Hobart::CEvolMsgReceiver *pHobThread) : SystemReset(WeigherSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherSystemReset::WeigherSystemReset(const char *buffer) : SystemReset(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherSystemReset::~WeigherSystemReset()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void WeigherSystemReset::show()
{
	bool isTracing = Debug::isTracing("WeigherSystemReset");

	if (isTracing == false)
	{
		Debug::addTraceCategory("WeigherSystemReset");
	}

	Debug::trace("WeigherSystemReset", "WeigherSystemReset: ");

	SystemReset::show();

	if (isTracing == false)
	{
		Debug::removeTraceCategory("WeigherSystemReset");
	}
}

