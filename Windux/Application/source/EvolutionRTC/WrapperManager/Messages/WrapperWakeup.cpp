//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperWakeup.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperWakeup::dataType = 0;
std::string WrapperWakeup::dataTypeName("WrapperWakeup"); 
#endif

WrapperWakeup::WrapperWakeup(Hobart::CEvolMsgReceiver *pHobThread, WrCfgStatus wrCfgStatus) : EvolMessage(Wrapper, WrWakeup, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->wrCfgStatus = wrCfgStatus;
}

WrapperWakeup::WrapperWakeup(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &wrCfgStatus;
	for (unsigned int i = 0; i < sizeof(wrCfgStatus); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperWakeup::~WrapperWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
 
const char *WrapperWakeup::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &wrCfgStatus;
	for (unsigned int i = 0; i < sizeof(wrCfgStatus); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperWakeup::show()
{
	Debug::addTraceCategory("WrapperWakeup");
	Debug::trace("WrapperWakeup", "WrapperWakeup: ");

	EvolMessage::show();

	if (wrCfgStatus == WrapperConfigValid)
	{
		Debug::trace("WrapperWakeup", "wrOpMode: WrapperConfigValid");
	}
	else if (wrCfgStatus == WrapperConfigError)
	{
		Debug::trace("WrapperWakeup", "wrOpMode: WrapperConfigError");
	}
	else
	{
		Debug::trace("WrapperWakeup", HobString("wrCfgStatus: Unknown ") +
									HobString(Integer::toString((UINT16) wrCfgStatus)));
	}

	Debug::removeTraceCategory("WrapperWakeup");
}

WrCfgStatus WrapperWakeup::getWrCfgStatus()
{
	return wrCfgStatus;
}

void WrapperWakeup::setWrCfgStatus(WrCfgStatus wrCfgStatus)
{
	this->wrCfgStatus = wrCfgStatus;
}
