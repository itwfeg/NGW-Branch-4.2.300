//#include "vxWorks.h"
#include "WrapperErrorHistoryTracking.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperErrorHistoryTracking::dataType = 0;
std::string WrapperErrorHistoryTracking::dataTypeName("WrapperErrorHistoryTracking"); 
#endif

WrapperErrorHistoryTracking::WrapperErrorHistoryTracking(Hobart::CEvolMsgReceiver *pHobThread,
														 bool errorHistoryTracking) : WrapperGeneric(WrErrorHistoryTracking, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->errorHistoryTracking = errorHistoryTracking;
}

WrapperErrorHistoryTracking::WrapperErrorHistoryTracking(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	WrapperGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &errorHistoryTracking;
	for (unsigned int i = 0; i < sizeof(errorHistoryTracking); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperErrorHistoryTracking::~WrapperErrorHistoryTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperErrorHistoryTracking::serialize() const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &errorHistoryTracking;
	for (unsigned int i = 0; i < sizeof(errorHistoryTracking); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperErrorHistoryTracking::setErrorHistoryTracking()
{
	errorHistoryTracking = true;
}

void WrapperErrorHistoryTracking::clearErrorHistoryTracking()
{
	errorHistoryTracking = false;
}

bool WrapperErrorHistoryTracking::isErrorHistoryTracking()
{
	return (errorHistoryTracking == true);
}

bool WrapperErrorHistoryTracking::getErrorHistoryTracking()
{
	return errorHistoryTracking;
}
