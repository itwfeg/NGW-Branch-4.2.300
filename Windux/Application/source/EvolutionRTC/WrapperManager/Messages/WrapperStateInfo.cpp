//#include "vxWorks.h"
#include "WrapperStateInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperStateInfo::dataType = 0;
std::string WrapperStateInfo::dataTypeName("WrapperStateInfo"); 
#endif

WrapperStateInfo::WrapperStateInfo(WrapperState state)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->state = state;
}

WrapperStateInfo::WrapperStateInfo(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	int j = 0;

	char * pChar = (char *) &state;
	for (unsigned int i = 0; i < sizeof(state); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperStateInfo::~WrapperStateInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperStateInfo::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &state;
	for (unsigned int i = 0; i < sizeof(state); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 WrapperStateInfo::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperStateInfo::show()
{
	Debug::addTraceCategory("WrapperStateInfo");
	Debug::trace("WrapperStateInfo", "WrapperStateInfo: ");

	if (state == WrSwitching)
	{
		Debug::trace("WrapperStateInfo", "WrapperStateInfo: WrSwitching");
	}
	else if (state == WrIdle)
	{
		Debug::trace("WrapperStateInfo", "WrapperStateInfo: WrIdle");
	}
	else if (state == WrPaused)
	{
		Debug::trace("WrapperStateInfo", "WrapperStateInfo: WrPaused");
	}
	else if (state == WrRunning)
	{
		Debug::trace("WrapperStateInfo", "WrapperStateInfo: WrRunning");
	}
	else if (state == WrDisabled)
	{
		Debug::trace("WrapperStateInfo", "WrapperStateInfo: WrDisabled");
	}
	else if (state == WrBoardTest)
	{
		Debug::trace("WrapperStateInfo", "WrapperStateInfo: WrBoardTest");
	}
	else if (state == WrStandBy)
	{
		Debug::trace("WrapperStateInfo", "WrapperStateInfo: WrStandBy");
	}
	else if (state == WrUnknown)
	{
		Debug::trace("WrapperStateInfo", "WrapperStateInfo: WrUnknown");
	}
	else
	{
		Debug::trace("WrapperStateInfo", HobString("WrapperStateInfo: Unknown ") +
										  HobString(Integer::toString((UINT16) state)));
	}

	Debug::removeTraceCategory("WrapperStateInfo");
}

// Wrapper State Variants
WrapperState WrapperStateInfo::getWrapperState() const
{
	return state;
}

void WrapperStateInfo::setWrapperState(WrapperState state)
{
	this->state = state;
}

bool WrapperStateInfo::isSwitching()
{
	return (state == WrSwitching);
}

bool WrapperStateInfo::isIdle()
{
	return (state == WrIdle);
}

bool WrapperStateInfo::isPaused()
{
	return (state == WrPaused);
}

bool WrapperStateInfo::isRunning()
{
	return (state == WrRunning);
}

bool WrapperStateInfo::isDisabled()
{
	return (state == WrDisabled);
}

bool WrapperStateInfo::isBoardTest()
{
	return (state == WrBoardTest);
}

bool WrapperStateInfo::isStandBy()
{
	return (state == WrStandBy);
}

bool WrapperStateInfo::isUnknown()
{
	return (state == WrUnknown);
}

void WrapperStateInfo::setSwitching()
{
	state = WrSwitching;
}

void WrapperStateInfo::setIdle()
{
	state = WrIdle;
}

void WrapperStateInfo::setPaused()
{
	state = WrPaused;
}

void WrapperStateInfo::setRunning()
{
	state = WrRunning;
}

void WrapperStateInfo::setDisabled()
{
	state = WrDisabled;
}

void WrapperStateInfo::setBoardTest()
{
	state = WrBoardTest;
}

void WrapperStateInfo::setStandBy()
{
	state = WrStandBy;
}

void WrapperStateInfo::setUnknown()
{
	state = WrUnknown;
}

bool WrapperStateInfo::equals(const WrapperStateInfo &wrStateInfo)
{
	bool rv = false;

	if (state == wrStateInfo.getWrapperState())
	{
		rv = true;
	}

	return rv;
}
