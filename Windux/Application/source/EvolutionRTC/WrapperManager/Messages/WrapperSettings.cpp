//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperSettings.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSettings::dataType = 0;
std::string WrapperSettings::dataTypeName("WrapperSettings"); 
#endif

WrapperSettings::WrapperSettings(Hobart::CEvolMsgReceiver *pHobThread,
								 WrapperIntakeMode::IntakeMode intakeMode,
								 WrFilmTension filmTension,
								 WrPackWidth filmWidth) : WrapperGeneric(WrSettings, pHobThread) 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->intakeMode.setIntakeMode(intakeMode);
	this->filmTension = filmTension;
	this->filmWidth = filmWidth;
}

WrapperSettings::WrapperSettings(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	WrapperGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	WrapperIntakeMode::IntakeMode intakeMode;
	char *pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		*pChar++ = buffer[j++];
	}

	this->intakeMode.setIntakeMode(intakeMode);

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmWidth;
	for (unsigned int i = 0; i < sizeof(filmWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	heaterInfo = WrapperHeaterInfo(&buffer[j]);

//	heaterInfo.serialize();
//	j += heaterInfo.getSerializedLength();
}

WrapperSettings::~WrapperSettings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *WrapperSettings::serialize() const
{
	WrapperGeneric::serialize();

	// Now, for the derived class items
	WrapperIntakeMode::IntakeMode intakeMode = this->intakeMode.getIntakeMode();
	char *pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmWidth;
	for (unsigned int i = 0; i < sizeof(filmWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	const char *pCC = heaterInfo.serialize();
	int mLength = heaterInfo.getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

bool WrapperSettings::equals(WrapperSettings &wrSettings)
{
	bool rv = true;

	if ((intakeMode.equals(wrSettings.getIntakeMode()) == false) ||
		(filmTension != wrSettings.getFilmTension()) ||
		(filmWidth != wrSettings.getFilmWidth()) ||
		(heaterInfo.equals(wrSettings.getHeaterInfo()) == false))
	{
		rv = false;
	}
	return rv;
}
void WrapperSettings::show()
{
	bool isTracing = Debug::isTracing("WrapperSettings");

	if (isTracing == false)
	{
		Debug::addTraceCategory("WrapperSettings");
	}

	Debug::trace("WrapperSettings", "WrapperSettings: ");

	WrapperGeneric::show();
	intakeMode.show();

	if (filmTension == LowFilmTension)
	{
		Debug::trace("WrapperSettings", "filmTension: LowFilmTension!");
	}
	else if (filmTension == MediumFilmTension)
	{
		Debug::trace("WrapperSettings", "filmTension: MediumFilmTension!");
	}
	else if (filmTension == HighFilmTension)
	{
		Debug::trace("WrapperSettings", "filmTension: HighFilmTension!");
	}
	else
	{
		Debug::trace("WrapperSettings", HobString("filmTension: Unknown ") +
											HobString(Integer::toString((UINT16) filmTension)));
	}
	
	if (filmWidth == NarrowPackage)
	{
		Debug::trace("WrapperSettings", "filmWidth: NarrowPackage!");
	}
	else if (filmWidth == WidePackage)
	{
		Debug::trace("WrapperSettings", "filmWidth: WidePackage!");
	}
	else if (filmWidth == FilmWidthIdle)
	{
		Debug::trace("WrapperSettings", "filmWidth: FilmWidthIdle!");
	}
	else if (filmWidth == AutoWidthSelect)
	{
		Debug::trace("WrapperSettings", "filmWidth: AutoWidthSelect!");
	}
	else
	{
		Debug::trace("WrapperSettings", HobString("filmWidth: Unknown ") +
										  HobString(Integer::toString((UINT16) filmWidth)));
	}

	Debug::trace("WrapperSettings", "heaterInfo: ");
	heaterInfo.show();

	if (isTracing == false)
	{
		Debug::removeTraceCategory("WrapperSettings");
	}
}

const WrapperIntakeMode &WrapperSettings::getIntakeMode() const
{
	return intakeMode;
}

void WrapperSettings::setIntakeMode(WrapperIntakeMode &intakeMode)
{
	this->intakeMode.setIntakeMode(intakeMode.getIntakeMode());
}

void WrapperSettings::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	this->intakeMode.setIntakeMode(intakeMode);
}

void WrapperSettings::setIntakeMode(WrapperIntakeMode::UltimaIntakeMode intakeMode)
{
	this->intakeMode.setIntakeMode((WrapperIntakeMode::IntakeMode) intakeMode);
}

void WrapperSettings::setFilmTension(WrFilmTension filmTension)
{
	this->filmTension = filmTension;
}

WrFilmTension WrapperSettings::getFilmTension() const
{
	return filmTension;
}

void WrapperSettings::setFilmWidth(WrPackWidth filmWidth)
{
	this->filmWidth = filmWidth;
}

WrPackWidth WrapperSettings::getFilmWidth() const
{
	return filmWidth;
}

void WrapperSettings::setHeaterInfo(WrapperHeaterInfo &heaterInfo)
{
	this->heaterInfo = heaterInfo;
}

WrapperHeaterInfo &WrapperSettings::getHeaterInfo()
{
	return heaterInfo;
}

const WrapperHeaterInfo &WrapperSettings::getHeaterInfo() const
{
	return heaterInfo;
}
