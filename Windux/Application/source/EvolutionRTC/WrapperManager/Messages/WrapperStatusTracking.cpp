//#include "vxWorks.h"
#include "WrapperStatusTracking.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperStatusTracking::dataType = 0;
std::string WrapperStatusTracking::dataTypeName("WrapperStatusTracking"); 
#endif

WrapperStatusTracking::WrapperStatusTracking(Hobart::CEvolMsgReceiver *pHobThread,
											   bool statusTracking) : WrapperGeneric(WrStatusTracking, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->statusTracking = statusTracking;
}

WrapperStatusTracking::WrapperStatusTracking(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	WrapperGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &statusTracking;
	for (unsigned int i = 0; i < sizeof(statusTracking); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperStatusTracking::~WrapperStatusTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperStatusTracking::serialize() const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &statusTracking;
	for (unsigned int i = 0; i < sizeof(statusTracking); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperStatusTracking::setStatusTracking()
{
	statusTracking = true;
}

void WrapperStatusTracking::clearStatusTracking()
{
	statusTracking = false;
}

bool WrapperStatusTracking::isStatusTracking()
{
	return (statusTracking == true);
}

bool WrapperStatusTracking::getStatusTracking()
{
	return statusTracking;
}
