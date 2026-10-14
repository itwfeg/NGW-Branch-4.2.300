//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperMode.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperMode::dataType = 0;
std::string WrapperMode::dataTypeName("WrapperMode"); 
#endif

WrapperMode::WrapperMode(Hobart::CEvolMsgReceiver *pHobThread, WrOpMode wrOpMode) : EvolMessage(Wrapper, WrMode, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->wrOpMode = wrOpMode;
}

WrapperMode::WrapperMode(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &wrOpMode;
	for (unsigned int i = 0; i < sizeof(wrOpMode); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WrapperMode::~WrapperMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperMode::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &wrOpMode;
	for (unsigned int i = 0; i < sizeof(wrOpMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperMode::show()
{
	Debug::addTraceCategory("WrapperMode");
	Debug::trace("WrapperMode", "WrapperMode: ");

	EvolMessage::show();

	if (wrOpMode == WrOperateMode)
	{
		Debug::trace("WrapperMode", "wrOpMode: wrOperateMode");
	}
	else if (wrOpMode == WrServiceMode)
	{
		Debug::trace("WrapperMode", "wrOpMode: WrServiceMode");
	}
	else if (wrOpMode == WrTestMode)
	{
		Debug::trace("WrapperMode", "wrOpMode: WrTestMode");
	}
	else if (wrOpMode == WrMfgMode)
	{
		Debug::trace("WrapperMode", "wrOpMode: WrMfgMode");
	}
	else if (wrOpMode == WrContinuousMode)
	{
		Debug::trace("WrapperMode", "wrOpMode: WrContinuousMode");
	}
	else if (wrOpMode == WrBoardMfgMode)
	{
		Debug::trace("WrapperMode", "wrOpMode: WrBoardMfgMode");
	}
	else
	{
		Debug::trace("WrapperMode", HobString("wrOpMode: Unknown ") +
									HobString(Integer::toString((UINT16) wrOpMode)));
	}

	Debug::removeTraceCategory("WrapperMode");
}

void WrapperMode::setWrOpMode(WrOpMode wrOpMode)
{
	this->wrOpMode = wrOpMode;
}

WrOpMode WrapperMode::getWrOpMode() const
{
	return wrOpMode;
}

