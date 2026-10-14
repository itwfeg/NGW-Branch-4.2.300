//#include "vxWorks.h"
#include "WrapperTiming.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperTiming::dataType = 0;
std::string WrapperTiming::dataTypeName("WrapperTiming"); 
#endif

WrapperTiming::WrapperTiming(UINT8 onMin, UINT8 onValue, UINT8 onMax,
							 UINT8 offMin, UINT8 offValue, UINT8 offMax)
{
	timeOn.setMin(onMin);
	timeOn.setValue(onValue);
	timeOn.setMax(onMax);
	timeOff.setMin(offMin);
	timeOff.setValue(offValue);
	timeOff.setMax(offMax);
}

WrapperTiming::WrapperTiming(const char *buffer)
{
	UINT8 onMin, onValue, onMax;
	UINT8 offMin, offValue, offMax;

	int j = 0;

	char *pChar = (char *) &onMin;
	for (unsigned int i = 0; i < sizeof(onMin); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &onValue;
	for (unsigned int i = 0; i < sizeof(onValue); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &onMax;
	for (unsigned int i = 0; i < sizeof(onMax); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &offMin;
	for (unsigned int i = 0; i < sizeof(offMin); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &offValue;
	for (unsigned int i = 0; i < sizeof(offValue); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &offMax;
	for (unsigned int i = 0; i < sizeof(offMax); i++)
	{
		*pChar++ = buffer[j++];
	}

	timeOn.setMin(onMin);
	timeOn.setValue(onValue);
	timeOn.setMax(onMax);
	timeOff.setMin(offMin);
	timeOff.setValue(offValue);
	timeOff.setMax(offMax);
}

WrapperTiming::~WrapperTiming()
{
}

const char *WrapperTiming::serialize() const
{
	serializedData = "";

	const char *pCC = timeOn.serialize();
	int mLength = timeOn.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = timeOff.serialize();
	mLength = timeOff.getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

UINT32 WrapperTiming::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperTiming::show()
{
	Debug::addTraceCategory("WrapperTiming");
	Debug::trace("WrapperTiming", "WrapperTiming");

	Debug::trace("WrapperTiming", "timeOn: ");
	timeOn.show();
	Debug::trace("WrapperTiming", "timeOff: ");
	timeOff.show();

	Debug::removeTraceCategory("WrapperTiming");
}

UINT8 WrapperTiming::getOnMin() const
{
	return timeOn.getMin();
}

void WrapperTiming::setOnMin(UINT8 onMin)
{
	timeOn.setMin(onMin);
}

UINT8 WrapperTiming::getOnValue() const
{
	return timeOn.getValue();
}

void WrapperTiming::setOnValue(UINT8 onValue)
{
	timeOn.setValue(onValue);
}

UINT8 WrapperTiming::getOnMax() const
{
	return timeOn.getMax();
}

void WrapperTiming::setOnMax(UINT8 onMax)
{
	timeOn.setMax(onMax);
}

UINT8 WrapperTiming::getOffMin() const
{
	return timeOff.getMin();
}

void WrapperTiming::setOffMin(UINT8 offMin)
{
	timeOff.setMin(offMin);
}

UINT8 WrapperTiming::getOffValue() const
{
	return timeOff.getValue();
}

void WrapperTiming::setOffValue(UINT8 offValue)
{
	timeOff.setValue(offValue);
}

UINT8 WrapperTiming::getOffMax() const
{
	return timeOff.getMax();
}

void WrapperTiming::setOffMax(UINT8 offMax)
{
	timeOff.setMax(offMax);
}
