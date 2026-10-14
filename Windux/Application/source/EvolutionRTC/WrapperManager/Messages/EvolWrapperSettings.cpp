//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "EvolWrapperSettings.h"

#include "Debug.h"
#include "Integer.h"
#include "UInteger.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperSettings::dataType = 0;
std::string EvolWrapperSettings::dataTypeName("EvolWrapperSettings"); 
#endif

EvolWrapperSettings::EvolWrapperSettings(Hobart::CEvolMsgReceiver *pHobThread,
										 WrapperIntakeMode::IntakeMode intakeMode,
										 WrFilmTension filmTension,
										INT16 filmTensionFrontToBack,
										INT16 filmTensionRightToLeft,
										 WrFilmWidthSelection filmWidth,
										 UINT16 upperRollWidth,
										 UINT16 lowerRollWidth,
										 bool isCelsiusSet,
										 bool filmSupplyTempPresent) : WrapperGeneric(WrEvolSettings, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->intakeMode.setIntakeMode(intakeMode);
	this->filmTension = filmTension;
	this->filmTensionFrontToBack = filmTensionFrontToBack;
	this->filmTensionRightToLeft = filmTensionRightToLeft;
	this->filmWidth = filmWidth;
	this->upperRollWidth = upperRollWidth;
	this->lowerRollWidth = lowerRollWidth;
	this->isCelsiusSet = isCelsiusSet;
	this->filmSupplyTempPresent = filmSupplyTempPresent;
}

EvolWrapperSettings::EvolWrapperSettings(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	bool traceOn = Debug::isTracing("EvolWrapperSettings");
	HobString msgDebug = "";
	if (traceOn)
		HobString msgDebug = "EvolWrapperSettings(buffer):   ";

	// Now, for the derived class items

	WrapperIntakeMode::IntakeMode tempIntakeMode;
	char *pChar = (char *) &tempIntakeMode;
	for (unsigned int i = 0; i < sizeof(tempIntakeMode); i++)
	{
		*pChar++ = buffer[j++];
	}
	this->intakeMode.setIntakeMode(tempIntakeMode);
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("intakeMode = ").append(Integer::toString(tempIntakeMode));
		msgDebug2.append("  sizeof(intakeMode) = ").append(Integer::toString(sizeof(tempIntakeMode)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("filmTension = ").append(Integer::toString(filmTension));
		msgDebug2.append("  sizeof(filmTension) = ").append(Integer::toString(sizeof(filmTension)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	pChar = (char *) &filmTensionFrontToBack;
	for (unsigned int i = 0; i < sizeof(filmTensionFrontToBack); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("filmTensionFrontToBack = ").append(Integer::toString(filmTensionFrontToBack));
		msgDebug2.append("  sizeof(filmTensionFrontToBack) = ").append(Integer::toString(sizeof(filmTensionFrontToBack)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	pChar = (char *) &filmTensionRightToLeft;
	for (unsigned int i = 0; i < sizeof(filmTensionRightToLeft); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("filmTensionRightToLeft = ").append(Integer::toString(filmTensionRightToLeft));
		msgDebug2.append("  sizeof(filmTensionRightToLeft) = ").append(Integer::toString(sizeof(filmTensionRightToLeft)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	pChar = (char *) &filmWidth;
	for (unsigned int i = 0; i < sizeof(filmWidth); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("filmWidth = ").append(Integer::toString(filmWidth));
		msgDebug2.append("  sizeof(filmWidth) = ").append(Integer::toString(sizeof(filmWidth)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	pChar = (char *) &upperRollWidth;
	for (unsigned int i = 0; i < sizeof(upperRollWidth); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("upperRollWidth = ").append(Integer::toString(upperRollWidth));
		msgDebug2.append("  sizeof(upperRollWidth) = ").append(Integer::toString(sizeof(upperRollWidth)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	pChar = (char *) &lowerRollWidth;
	for (unsigned int i = 0; i < sizeof(lowerRollWidth); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("lowerRollWidth = ").append(Integer::toString(lowerRollWidth));
		msgDebug2.append("  sizeof(lowerRollWidth) = ").append(Integer::toString(sizeof(lowerRollWidth)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	temperatureInfo = EvolWrapperTemperatureInfo(&buffer[j]);
	temperatureInfo.serialize();
	j += temperatureInfo.getSerializedLength();
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("EvolWrapperTemperatureInfo handled via that class.");
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	pChar = (char *) &isCelsiusSet;
	for (unsigned int i = 0; i < sizeof(isCelsiusSet); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("isCelsiusSet = ").append(Bool::toString(isCelsiusSet));
		msgDebug2.append("  sizeof(isCelsiusSet) = ").append(Integer::toString(sizeof(isCelsiusSet)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}

	pChar = (char *) &filmSupplyTempPresent;
	for (unsigned int i = 0; i < sizeof(filmSupplyTempPresent); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("filmSupplyTempPresent = ").append(Bool::toString(filmSupplyTempPresent));
		msgDebug2.append("  sizeof(filmSupplyTempPresent) = ").append(Integer::toString(sizeof(filmSupplyTempPresent)));
		Debug::trace("EvolWrapperConfiguration", msgDebug2);
	}
}

EvolWrapperSettings::~EvolWrapperSettings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *EvolWrapperSettings::serialize() const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	WrapperIntakeMode::IntakeMode tempIntakeMode = this->intakeMode.getIntakeMode();
	char *pChar = (char *) &tempIntakeMode;
	for (unsigned int i = 0; i < sizeof(tempIntakeMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTensionFrontToBack;
	for (unsigned int i = 0; i < sizeof(filmTensionFrontToBack); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTensionRightToLeft;
	for (unsigned int i = 0; i < sizeof(filmTensionRightToLeft); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmWidth;
	for (unsigned int i = 0; i < sizeof(filmWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &upperRollWidth;
	for (unsigned int i = 0; i < sizeof(upperRollWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &lowerRollWidth;
	for (unsigned int i = 0; i < sizeof(lowerRollWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	const char *pCC = temperatureInfo.serialize();
	int mLength = temperatureInfo.getSerializedLength();

	serializedData.append(pCC, mLength);

	pChar = (char *) &isCelsiusSet;
	for (unsigned int i = 0; i < sizeof(isCelsiusSet); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmSupplyTempPresent;
	for (unsigned int i = 0; i < sizeof(filmSupplyTempPresent); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperSettings::show()
{
	bool isTracing = Debug::isTracing("EvolWrapperSettings");
	if (isTracing == false)
		return;

	Debug::trace("EvolWrapperSettings", "EvolWrapperSettings: ");

	WrapperGeneric::show();
	intakeMode.show();

	if (filmTension == LowFilmTension)
	{
		Debug::trace("EvolWrapperSettings", "filmTension: LowFilmTension");
	}
	else if (filmTension == MediumFilmTension)
	{
		Debug::trace("EvolWrapperSettings", "filmTension: MediumFilmTension");
	}
	else if (filmTension == HighFilmTension)
	{
		Debug::trace("EvolWrapperSettings", "filmTension: HighFilmTension");
	}
	else
	{
		Debug::trace("EvolWrapperSettings", HobString("filmTension: Unknown ") + HobString(Integer::toString((UINT16) filmTension)));
	}
	
	Debug::trace("EvolWrapperSettings", HobString("filmTensionFrontToBack") + HobString(Integer::toString(filmTensionFrontToBack)));
	Debug::trace("EvolWrapperSettings", HobString("filmTensionRightToLeft") + HobString(Integer::toString(filmTensionRightToLeft)));

	if (filmWidth == UpperRoll)
	{
		Debug::trace("EvolWrapperSettings", "filmWidth: UpperRoll");
	}
	else if (filmWidth == LowerRoll)
	{
		Debug::trace("EvolWrapperSettings", "filmWidth: LowerRoll");
	}
	else if (filmWidth == Automatic)
	{
		Debug::trace("EvolWrapperSettings", "filmWidth: Automatic");
	}
	else if (filmWidth == NoFilmWidthSelected)
	{
		Debug::trace("EvolWrapperSettings", "filmWidth: NoFilmWidthSelected");
	}
	else
	{
		Debug::trace("EvolWrapperSettings", HobString("filmWidth: Unknown ") +
										  HobString(Integer::toString((UINT16) filmWidth)));
	}

	Debug::trace("EvolWrapperSettings", HobString("upperRollWidth: ") + HobString(UInteger::toString(upperRollWidth)));
	Debug::trace("EvolWrapperSettings", HobString("lowerRollWidth: ") + HobString(UInteger::toString(lowerRollWidth)));

	temperatureInfo.show();

	Debug::trace("EvolWrapperSettings", HobString("isCelsiusSet: ") + HobString(Bool::toString(isCelsiusSet)));
	Debug::trace("EvolWrapperSettings", HobString("filmSupplyTempPresent: ") + HobString(Bool::toString(filmSupplyTempPresent)));
}

const WrapperIntakeMode &EvolWrapperSettings::getIntakeMode() const
{
	return intakeMode;
}

void EvolWrapperSettings::setIntakeMode(WrapperIntakeMode &intakeMode)
{
	this->intakeMode.setIntakeMode(intakeMode.getIntakeMode());
}

void EvolWrapperSettings::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	this->intakeMode.setIntakeMode(intakeMode);
}

void EvolWrapperSettings::setIntakeMode(WrapperIntakeMode::UltimaIntakeMode intakeMode)
{
	this->intakeMode.setIntakeMode((WrapperIntakeMode::IntakeMode) intakeMode);
}

void EvolWrapperSettings::setFilmTension(WrFilmTension filmTension)
{
	this->filmTension = filmTension;
}

WrFilmTension EvolWrapperSettings::getFilmTension() const
{
	return filmTension;
}

INT16 EvolWrapperSettings::getFilmTensionFrontToBack() const
{
	return filmTensionFrontToBack;
}
void EvolWrapperSettings::setFilmTensionFrontToBack(INT16 tension)
{
	filmTensionFrontToBack = tension;
}

INT16 EvolWrapperSettings::getFilmTensionRightToLeft() const
{
	return filmTensionRightToLeft;
}
void EvolWrapperSettings::setFilmTensionRightToLeft(INT16 tension)
{
	filmTensionRightToLeft = tension;
}


void EvolWrapperSettings::setFilmWidth(WrFilmWidthSelection filmWidth)
{
	this->filmWidth = filmWidth;
}

WrFilmWidthSelection EvolWrapperSettings::getFilmWidth() const
{
	return filmWidth;
}

void EvolWrapperSettings::setUpperRollWidth(UINT16 upperRollWidth)
{
	this->upperRollWidth = upperRollWidth;
}

UINT16 EvolWrapperSettings::getUpperRollWidth() const
{
	return upperRollWidth;
}

void EvolWrapperSettings::setLowerRollWidth(UINT16 lowerRollWidth)
{
	this->lowerRollWidth = lowerRollWidth;
}

UINT16 EvolWrapperSettings::getLowerRollWidth() const
{
	return lowerRollWidth;
}

void EvolWrapperSettings::setTemperatureInfo(EvolWrapperTemperatureInfo &temperatureInfo)
{
	Debug::trace("EvolWrapperSettings", HobString("setTemperatureInfo() ------------------------------- "));
	this->temperatureInfo = temperatureInfo;
	temperatureInfo.show();
	this->temperatureInfo.show();
	Debug::trace("EvolWrapperSettings", HobString("setTemperatureInfo() ------------------------------- "));
}

const EvolWrapperTemperatureInfo &EvolWrapperSettings::getTemperatureInfo() const
{
	return temperatureInfo;
}

bool EvolWrapperSettings::isCelsius() const
{
	return isCelsiusSet;
}

bool EvolWrapperSettings::getCelsius() const
{
	return isCelsiusSet;
}

void EvolWrapperSettings::setCelsius(bool value)
{
	isCelsiusSet = value;
}

bool EvolWrapperSettings::isFahrenheit() const
{
	return (isCelsiusSet == false);
}

bool EvolWrapperSettings::getFahrenheit() const
{
	return (isCelsiusSet == false);
}

void EvolWrapperSettings::setFahrenheit(bool value)
{
	isCelsiusSet = (value == false);
}

bool EvolWrapperSettings::isFilmSupplyTempPresent() const
{
	return filmSupplyTempPresent;
}

bool EvolWrapperSettings::getFilmSupplyTempPresent() const
{
	return filmSupplyTempPresent;
}

void EvolWrapperSettings::clearFilmSupplyTempPresent()
{
	filmSupplyTempPresent = false;
}

void EvolWrapperSettings::setFilmSupplyTempPresent(bool value)
{
	filmSupplyTempPresent = value;
}

