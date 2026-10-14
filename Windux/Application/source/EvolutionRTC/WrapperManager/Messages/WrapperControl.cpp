//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperControl.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperControl::dataType = 0;
std::string WrapperControl::dataTypeName("WrapperControl"); 
#endif

WrapperControl::WrapperControl(Hobart::CEvolMsgReceiver *pHobThread,
							   UINT16 heaterSetting,
							   bool heaterEnable,
							   WrFilmTension filmTension,
							   WrPackWidth filmWidth,
							   WrapperIntakeMode::IntakeMode intakeMode) : EvolMessage(Wrapper, WrControl, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->heaterSetting = heaterSetting;
	this->heaterEnable = heaterEnable;
	this->filmTension = filmTension;
	this->filmWidth = filmWidth;
	this->intakeMode = intakeMode;
}

WrapperControl::WrapperControl(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("Clare");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "WrapperControl(*buffer):  ";

	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &filmWidth;
	for (unsigned int i = 0; i < sizeof(filmWidth); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("filmWidth = ").append(Integer::toString(filmWidth));
		msgDebug2.append("  sizeof(filmWidth) = ").append(Integer::toString(sizeof(filmWidth)));
		Debug::trace("Clare", msgDebug2);
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
		Debug::trace("Clare", msgDebug2);
	}

	pChar = (char *) &heaterSetting;
	for (unsigned int i = 0; i < sizeof(heaterSetting); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("heaterSetting = ").append(Integer::toString(heaterSetting));
		msgDebug2.append("  sizeof(heaterSetting) = ").append(Integer::toString(sizeof(heaterSetting)));
		Debug::trace("Clare", msgDebug2);
	}

	pChar = (char *) &heaterEnable;
	for (unsigned int i = 0; i < sizeof(heaterEnable); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("heaterEnable = ").append(Integer::toString(heaterEnable));
		msgDebug2.append("  sizeof(heaterEnable) = ").append(Integer::toString(sizeof(heaterEnable)));
		Debug::trace("Clare", msgDebug2);
	}

	pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		*pChar++ = buffer[j++];
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("intakeMode = ").append(Integer::toString(intakeMode));
		msgDebug2.append("  sizeof(intakeMode) = ").append(Integer::toString(sizeof(intakeMode)));
		Debug::trace("Clare", msgDebug2);
	}
}

WrapperControl::~WrapperControl()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *WrapperControl::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &filmWidth;
	for (unsigned int i = 0; i < sizeof(filmWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTension;
	for (unsigned int i = 0; i < sizeof(filmTension); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &heaterSetting;
	for (unsigned int i = 0; i < sizeof(heaterSetting); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &heaterEnable;
	for (unsigned int i = 0; i < sizeof(heaterEnable); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperControl::show()
{
	bool isTracing = Debug::isTracing("WrapperControl");

	if (isTracing == false)
	{
		Debug::addTraceCategory("WrapperControl");
	}

	Debug::trace("WrapperControl", "WrapperControl");

	EvolMessage::show();

	if (filmWidth == NarrowPackage)
	{
		Debug::trace("WrapperControl", "filmWidth: NarrowPackage");
	}
	else if (filmWidth == WidePackage)
	{
		Debug::trace("WrapperControl", "filmWidth: WidePackage");
	}
	else if (filmWidth == FilmWidthIdle)
	{
		Debug::trace("WrapperControl", "filmWidth: FilmWidthIdle");
	}
	else if (filmWidth == AutoWidthSelect)
	{
		Debug::trace("WrapperControl", "filmWidth: AutoWidthSelect");
	}
	else
	{
		Debug::trace("WrapperControl", HobString("filmWidth: Unknown ") + HobString(Integer::toString((UINT16) filmWidth)));
	}

	if (filmTension == LowFilmTension)
	{
		Debug::trace("WrapperControl", "filmTension: LowFilmTension!");
	}
	else if (filmTension == MediumFilmTension)
	{
		Debug::trace("WrapperControl", "filmTension: MediumFilmTension!");
	}
	else if (filmTension == HighFilmTension)
	{
		Debug::trace("WrapperControl", "filmTension: HighFilmTension!");
	}
	else
	{
		Debug::trace("WrapperControl", HobString("filmTension: Unknown ") +
									   HobString(Integer::toString((UINT16) filmTension)));
	}

	Debug::trace("WrapperControl", HobString("heaterSetting: ") +
								   HobString(Integer::toString((UINT16) heaterSetting)));

	Debug::trace("WrapperControl", HobString("heaterEnable: ") + HobString(Bool::toString(heaterEnable)));

	
	if (intakeMode == WrapperIntakeMode::DefaultIntake)
	{
		Debug::trace("WrapperControl", "intakeMode: DefaultIntake");
	}
	else if (intakeMode == WrapperIntakeMode::GentleIntake)
	{
		Debug::trace("WrapperControl", "intakeMode: GentleIntake");
	}
	else if (intakeMode == WrapperIntakeMode::NormalIntake)
	{
		Debug::trace("WrapperControl", "intakeMode: NormalIntake");
	}
	else if (intakeMode == WrapperIntakeMode::FastIntake)
	{
		Debug::trace("WrapperControl", "intakeMode: FastIntake");
	}
	else
	{
		Debug::trace("WrapperControl", HobString("intakeMode: Unknown ") +
								   	   HobString(Integer::toString((UINT16) intakeMode)));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory("WrapperControl");
	}
}

WrPackWidth WrapperControl::getFilmWidth() const
{
	return filmWidth;
}

void WrapperControl::setFilmWidth(WrPackWidth filmWidth)
{
	this->filmWidth = filmWidth;
}

WrFilmTension WrapperControl::getFilmTension() const
{
	return filmTension;
}

void WrapperControl::setFilmTension(WrFilmTension filmTension)
{
	this->filmTension = filmTension;
}

UINT16 WrapperControl::getHeaterSetting() const
{
	return heaterSetting;
}

void WrapperControl::setHeaterSetting(UINT16 heaterSetting)
{
	this->heaterSetting = heaterSetting;
}

bool WrapperControl::isHeaterEnable() const
{
	return heaterEnable;
}

void WrapperControl::setHeaterEnable(bool value)
{
	heaterEnable = value;
}

void WrapperControl::clearHeaterEnable()
{
	heaterEnable = false;
}

WrapperIntakeMode::IntakeMode WrapperControl::getIntakeMode() const
{
	return intakeMode;
}

void WrapperControl::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	this->intakeMode = intakeMode;
}
