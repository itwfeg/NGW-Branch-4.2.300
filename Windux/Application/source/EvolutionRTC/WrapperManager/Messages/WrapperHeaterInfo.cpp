//#include "vxWorks.h"
#include "WrapperHeaterInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperHeaterInfo::dataType = 0;
std::string WrapperHeaterInfo::dataTypeName("WrapperHeaterInfo"); 
#endif

WrapperHeaterInfo::WrapperHeaterInfo(bool state, UINT16 setting,
									 bool celsius, UINT16 temperature)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->state = state;
	this->setting = setting;
	this->celsius = celsius;
	this->temperature = temperature;
}

WrapperHeaterInfo::WrapperHeaterInfo(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int j = 0;

	char *pChar = (char *) &state;
	for (unsigned int i = 0; i < sizeof(state); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &setting;
	for (unsigned int i = 0; i < sizeof(setting); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &celsius;
	for (unsigned int i = 0; i < sizeof(celsius); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &temperature;
	for (unsigned int i = 0; i < sizeof(temperature); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperHeaterInfo::~WrapperHeaterInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperHeaterInfo::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &state;
	for (unsigned int i = 0; i < sizeof(state); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &setting;
	for (unsigned int i = 0; i < sizeof(setting); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &celsius;
	for (unsigned int i = 0; i < sizeof(celsius); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &temperature;
	for (unsigned int i = 0; i < sizeof(temperature); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 WrapperHeaterInfo::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperHeaterInfo::show()
{
	Debug::addTraceCategory("WrapperHeaterInfo");
	Debug::trace("WrapperHeaterInfo", "WrapperHeaterInfo: ");

	if (state == true)
	{
		Debug::trace("WrapperHeaterInfo", "Heater ON!");
	}
	else
	{
		Debug::trace("WrapperHeaterInfo", "Heater OFF!");
	}

	HobString scale("");

	if (celsius == true)
	{
		scale = " C";
	}
	else
	{
		scale = " F";
	}

	Debug::trace("WrapperHeaterInfo", HobString("Setting: ") +
									  HobString(Integer::toString(setting)) +
									  scale);
	Debug::trace("WrapperHeaterInfo", HobString("Temperature: ") +
									  HobString(Integer::toString(temperature)) +
									  scale);

	Debug::removeTraceCategory("WrapperHeaterInfo");
}

bool WrapperHeaterInfo::isState() const
{
	return state;
}

void WrapperHeaterInfo::setState(bool value)
{
	state = value;
}

void WrapperHeaterInfo::clearState()
{
	state = false;
}

void WrapperHeaterInfo::setSetting(UINT16 setting)
{
	this->setting = setting;
}

UINT16 WrapperHeaterInfo::getSetting() const
{
	return setting;
}

bool WrapperHeaterInfo::isCelsius() const
{
	return celsius;
}

void WrapperHeaterInfo::setCelsius(bool value)
{
	celsius = value;
}

void WrapperHeaterInfo::clearCelsius()
{
	celsius = false;
}

void WrapperHeaterInfo::setTemperature(UINT16 temperature)
{
	this->temperature= temperature;
}

UINT16 WrapperHeaterInfo::getTemperature() const
{
	return temperature;
}

void WrapperHeaterInfo::setHeaterBits(WrHeaterBits heaterBits)
{
	if (heaterBits.state == 1)
	{
		state = true;
	}
	else
	{
		state = false;
	}

	if (heaterBits.celsius == 1)
	{
		celsius = true;
	}
	else
	{
		celsius = false;
	}

	setting = heaterBits.setting;
	temperature = heaterBits.temperature;
}

WrHeaterBits WrapperHeaterInfo::getHeaterBits() const
{
	WrHeaterBits heaterBits;

	memset (&heaterBits, 0, sizeof (heaterBits));

	if (state == true)
	{
		heaterBits.state = 1;
	}

	if (celsius == true)
	{
		heaterBits.celsius = 1;
	}

	heaterBits.setting = setting;
	heaterBits.temperature = temperature;

	return heaterBits;
}

bool WrapperHeaterInfo::equals(const WrapperHeaterInfo &wrHeaterInfo)
{
	bool rv = false;

	if ((state == wrHeaterInfo.isState()) &&
		(setting == wrHeaterInfo.getSetting()) &&
		(celsius == wrHeaterInfo.isCelsius()) &&
		(temperature == wrHeaterInfo.getTemperature()))
	{
		rv = true;
	
	}

	return rv;
}
