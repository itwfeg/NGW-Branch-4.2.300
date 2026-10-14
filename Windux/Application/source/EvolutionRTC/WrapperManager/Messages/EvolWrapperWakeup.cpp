//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "EvolWrapperWakeup.h"

#include "Debug.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperWakeup::dataType = 0;
std::string EvolWrapperWakeup::dataTypeName("EvolWrapperWakeup"); 
#endif

EvolWrapperWakeup::EvolWrapperWakeup(Hobart::CEvolMsgReceiver *pHobThread,
									 WrCfgStatus wrCfgStatus,
									 bool allNodeAvailable) : WrapperWakeup(pHobThread, wrCfgStatus)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WrEvolWakeup);

	this->allNodeAvailable = allNodeAvailable;
}

EvolWrapperWakeup::EvolWrapperWakeup(const char *buffer) : WrapperWakeup(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	WrapperWakeup::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &allNodeAvailable;
	for (unsigned int i = 0; i < sizeof(allNodeAvailable); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperWakeup::~EvolWrapperWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *EvolWrapperWakeup::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &allNodeAvailable;
	for (unsigned int i = 0; i < sizeof(allNodeAvailable); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperWakeup::show()
{
	bool isTracing = Debug::isTracing("EvolWrapperWakeup");

	if (isTracing == false)
	{
		Debug::addTraceCategory("EvolWrapperWakeup");
	}

	Debug::trace("EvolWrapperWakeup", "EvolWrapperWakeup: ");

	WrapperWakeup::show();

	Debug::trace("EvolWrapperWakeup", HobString("allNodeAvailable") + HobString(Bool::toString(allNodeAvailable)));

	if (isTracing == false)
	{
		Debug::removeTraceCategory("EvolWrapperWakeup");
	}
}

bool EvolWrapperWakeup::getAllNodeAvailable()
{
	return allNodeAvailable;
}

void EvolWrapperWakeup::clearAllNodeAvailable()
{
	allNodeAvailable = false;
}

void EvolWrapperWakeup::setAllNodeAvailable()
{
	allNodeAvailable = true;
}

void EvolWrapperWakeup::setAllNodeAvailable(bool allNodeAvailable)
{
	this->allNodeAvailable = allNodeAvailable;
}
