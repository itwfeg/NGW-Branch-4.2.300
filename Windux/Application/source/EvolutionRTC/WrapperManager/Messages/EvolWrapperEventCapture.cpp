#include "EvolWrapperEventCapture.h"
#include "WrapperEvent.h"

#include "Debug.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperKnifeTime::_dataType_ = 0;
std::string EvolWrapperKnifeTime::_dataTypeName_("EvolWrapperKnifeTime"); 
#endif


EvolWrapperEventCapture::EvolWrapperEventCapture(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolEventCapture, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	captureType = KnifeUpEvent;
	packageId = 0;
	value = 0;
	errorNum = 0;
	
	eventMonth = 0;
	eventDay = 0;
	eventYear = 0;
	eventHour = 0;
	eventMinutes = 0;
	eventSeconds = 0;
}

EvolWrapperEventCapture::EvolWrapperEventCapture(const char *buffer) : WrapperGeneric(buffer)
{
	WrapperGeneric::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &captureType;
	for (unsigned int i = 0; i < sizeof(captureType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &value;
	for (unsigned int i = 0; i < sizeof(value); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &errorNum;
	for (unsigned int i = 0; i < sizeof(errorNum); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &eventMonth;
	for (unsigned int i = 0; i < sizeof(eventMonth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &eventDay;
	for (unsigned int i = 0; i < sizeof(eventDay); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &eventYear;
	for (unsigned int i = 0; i < sizeof(eventYear); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &eventHour;
	for (unsigned int i = 0; i < sizeof(eventHour); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &eventMinutes;
	for (unsigned int i = 0; i < sizeof(eventMinutes); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &eventSeconds;
	for (unsigned int i = 0; i < sizeof(eventSeconds); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperEventCapture::~EvolWrapperEventCapture()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperEventCapture::serialize()const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &captureType;
	for (unsigned int i = 0; i < sizeof(captureType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &value;
	for (unsigned int i = 0; i < sizeof(value); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &errorNum;
	for (unsigned int i = 0; i < sizeof(errorNum); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &eventMonth;
	for (unsigned int i = 0; i < sizeof(eventMonth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &eventDay;
	for (unsigned int i = 0; i < sizeof(eventDay); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &eventYear;
	for (unsigned int i = 0; i < sizeof(eventYear); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &eventHour;
	for (unsigned int i = 0; i < sizeof(eventHour); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &eventMinutes;
	for (unsigned int i = 0; i < sizeof(eventMinutes); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &eventSeconds;
	for (unsigned int i = 0; i < sizeof(eventSeconds); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

WrEventCaptureType EvolWrapperEventCapture::getEventCaptureType() const
{
	return captureType;
}
void EvolWrapperEventCapture::setEventCaptureType(WrEventCaptureType eventCaptureType)
{
	captureType = eventCaptureType;
}

UINT16 EvolWrapperEventCapture::getPackageId() const
{
	return packageId;
}

void EvolWrapperEventCapture::setPackageId(UINT16 id)
{
	packageId = id;
}
		
UINT32 EvolWrapperEventCapture::getValue() const
{
	return value;
}

void EvolWrapperEventCapture::setValue(UINT32 v)
{
	value = v;
}
		
UINT16 EvolWrapperEventCapture::getErrorNumber() const
{
	return errorNum;
}

void EvolWrapperEventCapture::setErrorNumber(UINT16 num)
{
	errorNum = num;
}

INT16 EvolWrapperEventCapture::getEventMonth() const
{
	return eventMonth;
}

void EvolWrapperEventCapture::setEventMonth(INT16 month)
{
	eventMonth = month;
}

INT16 EvolWrapperEventCapture::getEventDay() const
{
	return eventDay;
}

void EvolWrapperEventCapture::setEventDay(INT16 day)
{
	eventDay = day;
}

INT16 EvolWrapperEventCapture::getEventYear() const
{
	return eventYear;
}

void EvolWrapperEventCapture::setEventYear(INT16 year)
{
	eventYear = year;
}

INT16 EvolWrapperEventCapture::getEventHour() const
{
	return eventHour;
}

void EvolWrapperEventCapture::setEventHour(INT16 hour)
{
	eventHour = hour;
}

INT16 EvolWrapperEventCapture::getEventMinutes() const
{
	return eventMinutes;
}

void EvolWrapperEventCapture::setEventMinutes(INT16 minutes)
{
	eventMinutes = minutes;
}

INT16 EvolWrapperEventCapture::getEventSeconds() const
{
	return eventSeconds;
}

void EvolWrapperEventCapture::setEventSeconds(INT16 seconds)
{
	eventSeconds = seconds;
}

void EvolWrapperEventCapture::show()
{
	Debug::trace("EvolWrapperEventCapture", "EvolWrapperEventCapture: ");
	Debug::trace("EvolWrapperEventCapture", "captureType: " + Integer::toString(captureType));
	Debug::trace("EvolWrapperEventCapture", "packageId: " + Integer::toString(packageId));
	Debug::trace("EvolWrapperEventCapture", "value: " + Integer::toString(value));
	Debug::trace("EvolWrapperEventCapture", "errorNum: " + Integer::toString(errorNum));
}
