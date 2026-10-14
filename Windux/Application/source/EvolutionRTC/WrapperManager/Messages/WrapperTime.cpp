//#include "vxWorks.h"
#include "WrapperTime.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperTime::dataType = 0;
std::string WrapperTime::dataTypeName("WrapperTime"); 
#endif

WrapperTime::WrapperTime(UINT8 min, UINT8 value, UINT8 max)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->min = min;
	this->value = value;
	this->max = max;
}

WrapperTime::WrapperTime(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int j = 0;

	char *pChar = (char *) &min;
	for (unsigned int i = 0; i < sizeof(min); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &value;
	for (unsigned int i = 0; i < sizeof(value); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &max;
	for (unsigned int i = 0; i < sizeof(max); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperTime::~WrapperTime()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperTime::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &min;
	for (unsigned int i = 0; i < sizeof(min); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &value;
	for (unsigned int i = 0; i < sizeof(value); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &max;
	for (unsigned int i = 0; i < sizeof(max); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 WrapperTime::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperTime::show()
{
	Debug::addTraceCategory("WrapperTime");
	Debug::trace("WrapperTime", "WrapperTime (min, value, max): ");

	Debug::trace("WrapperTime", HobString("(")+
								HobString(Integer::toString(min)) +
								HobString(", ") +
								HobString(Integer::toString(value)) +
								HobString(", ") +
								HobString(Integer::toString(max)) +
								HobString(")"));

	Debug::removeTraceCategory("WrapperTime");
}

UINT8 WrapperTime::getMin() const
{
	return min;
}

void WrapperTime::setMin(UINT8 min)
{
	this->min = min;
}

UINT8 WrapperTime::getValue() const
{
	return value;
}

void WrapperTime::setValue(UINT8 value)
{
	this->value = value;
}

UINT8 WrapperTime::getMax() const
{
	return max;
}

void WrapperTime::setMax(UINT8 max)
{
	this->max = max;
}
