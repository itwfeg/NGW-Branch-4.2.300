//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperWrapperKind.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperWrapperKind::dataType = 0;
std::string WrapperWrapperKind::dataTypeName("WrapperWrapperKind"); 
#endif

WrapperWrapperKind::WrapperWrapperKind(Hobart::CEvolMsgReceiver *pHobThread, WrapperKind wrapperKind) : EvolMessage(Wrapper, WrKind, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->wrapperKind = wrapperKind;
}

WrapperWrapperKind::WrapperWrapperKind(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &wrapperKind;
	for (unsigned int i = 0; i < sizeof(wrapperKind); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperWrapperKind::~WrapperWrapperKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *WrapperWrapperKind::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &wrapperKind;
	for (unsigned int i = 0; i < sizeof(wrapperKind); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperWrapperKind::show()
{
	bool isTracing = Debug::isTracing("WrapperWrapperKind");

	if (isTracing == false)
	{ 
		Debug::addTraceCategory("WrapperWrapperKind");
	}

	Debug::trace("WrapperWrapperKind", "WrapperWrapperKind: ");

	EvolMessage::show();

	if (wrapperKind == WrNoWrapper)
	{
		Debug::trace("WrapperWrapperKind", "wrapperKind: WrNoWrapper");
	}
	else if (wrapperKind == WrUltimaWrapper)
	{
		Debug::trace("WrapperWrapperKind", "wrapperKind: WrUltimaWrapper");
	}
	else if (wrapperKind == WrEvolWrapper)
	{
		Debug::trace("WrapperWrapperKind", "wrapperKind: WrEvolWrapper");
	}
	else
	{
		Debug::trace("WrapperWrapperKind", HobString("wrapperKind: Unknown ") + HobString(Integer::toString((UINT16) wrapperKind)));
	}

	if (isTracing == false)
	{ 
		Debug::removeTraceCategory("WrapperWrapperKind");
	}
}

WrapperKind WrapperWrapperKind::getWrapperKind()
{
	return wrapperKind;
}

void WrapperWrapperKind::setWrapperKind(WrapperKind wrapperKind)
{
	this->wrapperKind = wrapperKind;
}
