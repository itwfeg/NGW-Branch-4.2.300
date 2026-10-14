//#include "vxWorks.h"

#include "WeigherEvent.h"
#include "MarqueeGeneric.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MarqueeGeneric::dataType = 0;
std::string MarqueeGeneric::dataTypeName("MarqueeGeneric"); 
#endif

MarqueeGeneric::MarqueeGeneric(MqEvent mqEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Marquee, (UINT8)mqEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

MarqueeGeneric::MarqueeGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

MarqueeGeneric::~MarqueeGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void MarqueeGeneric::show()
{
	bool isTracing = Debug::isTracing("MarqueeMessages");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("MarqueeMessages") );
#else
		Debug::addTraceCategory("MarqueeMessages");
#endif
	}

	Debug::trace("MarqueeMessages", "MarqueeGeneric: ");

	EvolMessage::show();


	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("MarqueeMessages") );
#else
		Debug::removeTraceCategory("MarqueeMessages");
#endif
	}
}

int MarqueeGeneric::fillPeripheralMessage(MqGenericMsg& mqGenericMsg)
{

	mqGenericMsg.mqEvent = (UINT8)eventType;
	mqGenericMsg.pad0 = 0;

	return sizeof (mqGenericMsg);
}
