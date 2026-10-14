//#include "vxWorks.h"
#include "WrapperRequestWakeup.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperRequestWakeup::dataType = 0;
std::string WrapperRequestWakeup::dataTypeName("WrapperRequestWakeup"); 
#endif

WrapperRequestWakeup::WrapperRequestWakeup(Hobart::CEvolMsgReceiver *pHobThread,
										   UINT16 physicalAddress) : WrapperGeneric(WrReqWakeup, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->physicalAddress = physicalAddress;
}

WrapperRequestWakeup::WrapperRequestWakeup(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	WrapperGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &physicalAddress;
	for (unsigned int i = 0; i < sizeof(physicalAddress); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperRequestWakeup::~WrapperRequestWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperRequestWakeup::serialize() const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &physicalAddress;
	for (unsigned int i = 0; i < sizeof(physicalAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT16 WrapperRequestWakeup::getPhysicalAddress()
{
	return physicalAddress;
}

void WrapperRequestWakeup::setPhysicalAddress(UINT16 physicalAddress)
{
	this->physicalAddress = physicalAddress;
}



