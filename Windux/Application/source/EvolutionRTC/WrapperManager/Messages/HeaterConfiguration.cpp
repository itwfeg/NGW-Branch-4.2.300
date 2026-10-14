//#include "vxWorks.h"
#include "HeaterConfiguration.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HeaterConfiguration::_dataType_ = 0;
std::string HeaterConfiguration::_dataTypeName_("HeaterConfiguration"); 
#endif

HeaterConfiguration::HeaterConfiguration(bool isUsed, INT16 maxTemp, INT16 minTemp, UINT16 defaultTempSetting)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->isUsed = isUsed;
	this->maxTemp = maxTemp;
	this->minTemp = minTemp;
	this->defaultTempSetting = defaultTempSetting;
}

HeaterConfiguration::HeaterConfiguration(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	int j = 0;

	char *pChar = (char *) &isUsed;
	for (unsigned int i = 0; i < sizeof(isUsed); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &maxTemp;
	for (unsigned int i = 0; i < sizeof(maxTemp); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &minTemp;
	for (unsigned int i = 0; i < sizeof(minTemp); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &defaultTempSetting;
	for (unsigned int i = 0; i < sizeof(defaultTempSetting); i++)
	{
		*pChar++ = buffer[j++];
	}
}

HeaterConfiguration::HeaterConfiguration(const HeaterConfiguration &hc)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	isUsed = hc.isItUsed();
	maxTemp = hc.getMaxTemp();
	minTemp = hc.getMinTemp();
	defaultTempSetting = hc.getDefaultTempSetting();
}


HeaterConfiguration::~HeaterConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HeaterConfiguration::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &isUsed;
	for (unsigned int i = 0; i < sizeof(isUsed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &maxTemp;
	for (unsigned int i = 0; i < sizeof(maxTemp); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &minTemp;
	for (unsigned int i = 0; i < sizeof(minTemp); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &defaultTempSetting;
	for (unsigned int i = 0; i < sizeof(defaultTempSetting); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 HeaterConfiguration::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void HeaterConfiguration::show()
{
	if (Debug::isTracing("HeaterConfiguration"))
	{
		HobString msgDebug("            isUsed = ");
		msgDebug.append(Bool::toString(isUsed));
		Debug::trace("HeaterConfiguration", msgDebug);

		msgDebug = "           maxTemp = ";
		msgDebug.append(Integer::toString(maxTemp));
		Debug::trace("HeaterConfiguration", msgDebug);

		msgDebug = "           minTemp = ";
		msgDebug.append(Integer::toString(minTemp));
		Debug::trace("HeaterConfiguration", msgDebug);

		msgDebug = "defaultTempSetting = ";
		msgDebug.append(Integer::toString(defaultTempSetting));
		Debug::trace("HeaterConfiguration", msgDebug);
	}
}

bool HeaterConfiguration::isItUsed() const
{
	HobString msgDebug("            isUsed = ");
	msgDebug.append(Bool::toString(isUsed));
	Debug::trace("HeaterConfiguration", msgDebug);

	return isUsed;
}

void HeaterConfiguration::setIsUsed(bool value)
{
	HobString msgDebug("            setIsUsed = ");
	msgDebug.append(Bool::toString(value));
	Debug::trace("HeaterConfiguration", msgDebug);

	isUsed = value;
}

void HeaterConfiguration::clearIsUsed()
{
	isUsed = false;
}

INT16 HeaterConfiguration::getMaxTemp() const
{
	return maxTemp;
}

void HeaterConfiguration::setMaxTemp(INT16 temp)
{
	maxTemp = temp;
}

INT16 HeaterConfiguration::getMinTemp() const
{
	return minTemp;
}

void HeaterConfiguration::setMinTemp(INT16 temp)
{
	minTemp = temp;
}

UINT16 HeaterConfiguration::getDefaultTempSetting() const
{
	return defaultTempSetting;
}

void HeaterConfiguration::setDefaultTempSetting(UINT16 temp)
{
	defaultTempSetting= temp;
}

void HeaterConfiguration::operator=(const HeaterConfiguration &hc)
{
	isUsed = hc.isItUsed();
	maxTemp = hc.getMaxTemp();
	minTemp = hc.getMinTemp();
	defaultTempSetting = hc.getDefaultTempSetting();
}
