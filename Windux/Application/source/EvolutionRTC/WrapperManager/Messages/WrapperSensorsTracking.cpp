//#include "vxWorks.h"
#include "WrapperSensorsTracking.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSensorsTracking::dataType = 0;
std::string WrapperSensorsTracking::dataTypeName("WrapperSensorsTracking"); 
#endif

WrapperSensorsTracking::WrapperSensorsTracking(Hobart::CEvolMsgReceiver *pHobThread,
											   bool sensorsTracking) : WrapperGeneric(WrSensorsTracking, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->sensorsTracking = sensorsTracking;
}

WrapperSensorsTracking::WrapperSensorsTracking(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	WrapperGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &sensorsTracking;
	for (unsigned int i = 0; i < sizeof(sensorsTracking); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperSensorsTracking::~WrapperSensorsTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperSensorsTracking::serialize() const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sensorsTracking;
	for (unsigned int i = 0; i < sizeof(sensorsTracking); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperSensorsTracking::setSensorsTracking()
{
	sensorsTracking = true;
}

void WrapperSensorsTracking::clearSensorsTracking()
{
	sensorsTracking = false;
}

bool WrapperSensorsTracking::isSensorsTracking()
{
	return (sensorsTracking == true);
}

bool WrapperSensorsTracking::getSensorsTracking()
{
	return sensorsTracking;
}
