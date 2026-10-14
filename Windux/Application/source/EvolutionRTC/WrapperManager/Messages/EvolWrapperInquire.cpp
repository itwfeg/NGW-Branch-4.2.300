//#include "vxWorks.h"
#include "EvolWrapperInquire.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperInquire::dataType = 0;
std::string EvolWrapperInquire::dataTypeName("EvolWrapperInquire"); 
#endif

EvolWrapperInquire::EvolWrapperInquire(Hobart::CEvolMsgReceiver *pHobThread) : EvolWrapperConfiguration(pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(WrEvolInquire);
	cfgStatus = WrapperConfigError;
}

EvolWrapperInquire::EvolWrapperInquire(const char *buffer) : EvolWrapperConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolWrapperConfiguration::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &cfgStatus;
	for (unsigned int i = 0; i < sizeof(cfgStatus); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperInquire::~EvolWrapperInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *EvolWrapperInquire::serialize() const
{
	EvolWrapperConfiguration::serialize();

	// Now, for the derived class items
	const char *pCC = (char *) &cfgStatus;
	for (unsigned int i = 0; i < sizeof(cfgStatus); i++)
	{
		serializedData.append(1, pCC[i]);
	}

	return serializedData.data();
}

void EvolWrapperInquire::show()
{
	bool isTracing = Debug::isTracing("EvolWrapperInquire");

	if (isTracing == false)
	{
	//	Debug::addTraceCategory("EvolWrapperInquire");
	}

	Debug::trace("EvolWrapperInquire", "EvolWrapperInquire: ");

	EvolWrapperConfiguration::show();

	if (cfgStatus == WrapperConfigValid)
	{
		Debug::trace("EvolWrapperInquire", "cfgStatus: WrapperConfigValid");
	}
	else if (cfgStatus == WrapperConfigError)
	{
		Debug::trace("EvolWrapperInquire", "cfgStatus: WrapperConfigError");
	}
	else
	{
		Debug::trace("EvolWrapperInquire", HobString("cfgStatus: Unknown ") + HobString(Integer::toString((UINT16) cfgStatus)));
	}

	if (isTracing == false)
	{
	//	Debug::removeTraceCategory("EvolWrapperInquire");
	}
}

WrCfgStatus EvolWrapperInquire::getConfigStatus()
{
	return cfgStatus;
}

void EvolWrapperInquire::setConfigStatus(WrCfgStatus configStatus)
{
	cfgStatus = configStatus;
}


