//#include "vxWorks.h"

#include "NumericEvent.h"
#include "NumericGeneric.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NumericGeneric::dataType = 0;
std::string NumericGeneric::dataTypeName("NumericGeneric"); 
#endif

NumericGeneric::NumericGeneric(NmEvent nmEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Numeric, (UINT8)nmEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

NumericGeneric::NumericGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

NumericGeneric::~NumericGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void NumericGeneric::show()
{
	bool isTracing = Debug::isTracing("NumericMessages");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("NumericMessages") );
#else
		Debug::addTraceCategory("NumericMessages");
#endif
	}

	Debug::trace("NumericMessages", "NumericGeneric: ");

	EvolMessage::show();

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("NumericMessages") );
#else
		Debug::removeTraceCategory("NumericMessages");
#endif
	}
}

int NumericGeneric::fillPeripheralMessage(NmGenericMsg& nmGenericMsg)
{

	nmGenericMsg.nmEvent = eventType;
	nmGenericMsg.pad0 = 0;

	return sizeof(nmGenericMsg);
}
