//#include "vxWorks.h"
#include "LabelApplierRequestWakeup.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierRequestWakeup::dataType = 0;
std::string LabelApplierRequestWakeup::dataTypeName("LabelApplierRequestWakeup"); 
#endif

LabelApplierRequestWakeup::LabelApplierRequestWakeup(Hobart::CEvolMsgReceiver *pHobThread,
													 UINT16 physicalAddress) : LabelApplierGeneric(LaReqWakeup, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->physicalAddress = physicalAddress;
}

LabelApplierRequestWakeup::LabelApplierRequestWakeup(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	LabelApplierGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &physicalAddress;
	for (unsigned int i = 0; i < sizeof(physicalAddress); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierRequestWakeup::~LabelApplierRequestWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierRequestWakeup::serialize() const
{
	LabelApplierGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &physicalAddress;
	for (unsigned int i = 0; i < sizeof(physicalAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierRequestWakeup::show()
{
	bool isTracing = Debug::isTracing("LabelApplierRequestWakeup");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("LabelApplierRequestWakeup") );
#else
		Debug::addTraceCategory("LabelApplierRequestWakeup");
#endif
	}

	Debug::trace("LabelApplierRequestWakeup", "LabelApplierRequestWakeup: ");

	EvolMessage::show();

	Debug::trace("LabelApplierRequestWakeup", HobString("physicalAddress: 0x") + HobString(Hex::toString((UINT16) physicalAddress)));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("LabelApplierRequestWakeup") );
#else
		Debug::removeTraceCategory("LabelApplierRequestWakeup");
#endif
	}
}

UINT16 LabelApplierRequestWakeup::getPhysicalAddress()
{
	return physicalAddress;
}

void LabelApplierRequestWakeup::setPhysicalAddress(UINT16 physicalAddress)
{
	this->physicalAddress = physicalAddress;
}
