//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperSetWrapMode.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSetWrapMode::dataType = 0;
std::string WrapperSetWrapMode::dataTypeName("WrapperSetWrapMode"); 
#endif

WrapperSetWrapMode::WrapperSetWrapMode(Hobart::CEvolMsgReceiver *pHobThread, WrapMode wrapMode) : EvolMessage(Wrapper, WrSetWrapMode, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->wrapMode = wrapMode;
}

WrapperSetWrapMode::WrapperSetWrapMode(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &wrapMode;
	for (unsigned int i = 0; i < sizeof(wrapMode); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperSetWrapMode::~WrapperSetWrapMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *WrapperSetWrapMode::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &wrapMode;
	for (unsigned int i = 0; i < sizeof(wrapMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperSetWrapMode::show()
{
	bool isTracing = Debug::isTracing("WrapperSetWrapMode");

	if (isTracing == false)
	{
		Debug::addTraceCategory("WrapperSetWrapMode");
	}

	Debug::trace("WrapperSetWrapMode", "WrapperSetWrapMode: ");

	EvolMessage::show();

	if (wrapMode == WrapOn)
	{
		Debug::trace("WrapperSetWrapMode", "wrapMode: WrapOn");
	}
	else if (wrapMode == WrapOff)
	{
		Debug::trace("WrapperSetWrapMode", "wrapMode: WrapOff");
	}
	else if (wrapMode == WrapOnly)
	{
		Debug::trace("WrapperSetWrapMode", "wrapMode: WrapOnly");
	}
	else if (wrapMode == ManualWeigh)
	{
		Debug::trace("WrapperSetWrapMode", "wrapMode: ManualWeigh");
	}
	else if (wrapMode == AutoManualApply)
	{
		Debug::trace("WrapperSetWrapMode", "wrapMode: AutoManualApply");
	}
	else
	{
		Debug::trace("WrapperSetWrapMode", HobString("wrapMode: Unknown ") + HobString(Integer::toString((UINT16) wrapMode)));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory("WrapperSetWrapMode");
	}
}

WrapperSetWrapMode::WrapMode WrapperSetWrapMode::getWrapMode()
{
	return wrapMode;
}

void WrapperSetWrapMode::setWrapMode(WrapperSetWrapMode::WrapMode wrapMode)
{
	this->wrapMode = wrapMode;
}

void WrapperSetWrapMode::setWrapOn()
{
	wrapMode = WrapOn;
}

void WrapperSetWrapMode::setWrapOff()
{
	wrapMode = WrapOff;
}

void WrapperSetWrapMode::setWrapOnly()
{
	wrapMode = WrapOnly;
}

void WrapperSetWrapMode::setManualWeigh()
{
	wrapMode = ManualWeigh;
}

void WrapperSetWrapMode::setAutoManualApply()
{
	wrapMode = AutoManualApply;
}
