//#include "vxWorks.h"

#include "WeigherEvent.h"
#include "WeigherGeneric.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherGeneric::dataType = 0;
std::string WeigherGeneric::dataTypeName("WeigherGeneric"); 
#endif

WeigherGeneric::WeigherGeneric(WgEvent wgEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Weigher, (UINT8)wgEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherGeneric::WeigherGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherGeneric::~WeigherGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void WeigherGeneric::show()
{
	bool isTracing = Debug::isTracing("WeigherGeneric");

	if (isTracing == false)
	{
		Debug::addTraceCategory("WeigherGeneric");
	}

	Debug::trace("WeigherGeneric", "WeigherGeneric: ");

	EvolMessage::show();

	if (isTracing == false)
	{
		Debug::removeTraceCategory("WeigherGeneric");
	}
}
