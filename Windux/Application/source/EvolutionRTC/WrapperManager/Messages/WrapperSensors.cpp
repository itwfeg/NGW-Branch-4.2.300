//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperSensors.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperSensors::dataType = 0;
std::string WrapperSensors::dataTypeName("WrapperSensors"); 
#endif

WrapperSensors::WrapperSensors(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Wrapper, WrSensors, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	heaterReading = 0;
	encoderCount = 0;
	intakeCount = 0;
	stopSwitch = false;
	startSwitch = false;
	packageLengthSwitch = false;
	packageSenseSwitch = false;
	packageWidthSwitch = false;
	pusherHomeSwitch = false;
	packageJamSwitch = false;
	wideFilmSwitch = false;
	narrowFilmSwitch = false;
	slideBlockState = false;
	heaterState = false;
	compressorState = false;
	mainMotorState = false;
	dischargeMotorState = false;
	filmLengthState = FilmLengthIdle;
	filmWidthState = FilmWidthIdle;
	knifeState = false;
	leftSideClampState = false;
	rightSideClampState = false;
	gripperState = false;
}

WrapperSensors::WrapperSensors(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &heaterReading;
	for (unsigned int i = 0; i < sizeof(heaterReading); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &encoderCount;
	for (unsigned int i = 0; i < sizeof(encoderCount); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeCount;
	for (unsigned int i = 0; i < sizeof(intakeCount); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &stopSwitch;
	for (unsigned int i = 0; i < sizeof(stopSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &startSwitch;
	for (unsigned int i = 0; i < sizeof(startSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageLengthSwitch;
	for (unsigned int i = 0; i < sizeof(packageLengthSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageSenseSwitch;
	for (unsigned int i = 0; i < sizeof(packageSenseSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageWidthSwitch;
	for (unsigned int i = 0; i < sizeof(packageWidthSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pusherHomeSwitch;
	for (unsigned int i = 0; i < sizeof(pusherHomeSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageJamSwitch;
	for (unsigned int i = 0; i < sizeof(packageJamSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wideFilmSwitch;
	for (unsigned int i = 0; i < sizeof(wideFilmSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &narrowFilmSwitch;
	for (unsigned int i = 0; i < sizeof(narrowFilmSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &slideBlockState;
	for (unsigned int i = 0; i < sizeof(slideBlockState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &heaterState;
	for (unsigned int i = 0; i < sizeof(heaterState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &compressorState;
	for (unsigned int i = 0; i < sizeof(compressorState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mainMotorState;
	for (unsigned int i = 0; i < sizeof(mainMotorState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &dischargeMotorState;
	for (unsigned int i = 0; i < sizeof(dischargeMotorState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmLengthState;
	for (unsigned int i = 0; i < sizeof(filmLengthState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmWidthState;
	for (unsigned int i = 0; i < sizeof(filmWidthState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &knifeState;
	for (unsigned int i = 0; i < sizeof(knifeState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &leftSideClampState;
	for (unsigned int i = 0; i < sizeof(leftSideClampState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rightSideClampState;
	for (unsigned int i = 0; i < sizeof(rightSideClampState); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &gripperState;
	for (unsigned int i = 0; i < sizeof(gripperState); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperSensors::~WrapperSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperSensors::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &heaterReading;
	for (unsigned int i = 0; i < sizeof(heaterReading); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &encoderCount;
	for (unsigned int i = 0; i < sizeof(encoderCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeCount;
	for (unsigned int i = 0; i < sizeof(intakeCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &stopSwitch;
	for (unsigned int i = 0; i < sizeof(stopSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &startSwitch;
	for (unsigned int i = 0; i < sizeof(startSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageLengthSwitch;
	for (unsigned int i = 0; i < sizeof(packageLengthSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageSenseSwitch;
	for (unsigned int i = 0; i < sizeof(packageSenseSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageWidthSwitch;
	for (unsigned int i = 0; i < sizeof(packageWidthSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pusherHomeSwitch;
	for (unsigned int i = 0; i < sizeof(pusherHomeSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageJamSwitch;
	for (unsigned int i = 0; i < sizeof(packageJamSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wideFilmSwitch;
	for (unsigned int i = 0; i < sizeof(wideFilmSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &narrowFilmSwitch;
	for (unsigned int i = 0; i < sizeof(narrowFilmSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &slideBlockState;
	for (unsigned int i = 0; i < sizeof(slideBlockState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &heaterState;
	for (unsigned int i = 0; i < sizeof(heaterState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &compressorState;
	for (unsigned int i = 0; i < sizeof(compressorState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mainMotorState;
	for (unsigned int i = 0; i < sizeof(mainMotorState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &dischargeMotorState;
	for (unsigned int i = 0; i < sizeof(dischargeMotorState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmLengthState;
	for (unsigned int i = 0; i < sizeof(filmLengthState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmWidthState;
	for (unsigned int i = 0; i < sizeof(filmWidthState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &knifeState;
	for (unsigned int i = 0; i < sizeof(knifeState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &leftSideClampState;
	for (unsigned int i = 0; i < sizeof(leftSideClampState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rightSideClampState;
	for (unsigned int i = 0; i < sizeof(rightSideClampState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &gripperState;
	for (unsigned int i = 0; i < sizeof(gripperState); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperSensors::show()
{
	Debug::addTraceCategory("WrapperSensors");
	Debug::trace("WrapperSensors", "WrapperSensors: ");

	EvolMessage::show();

	Debug::trace("WrapperSensors", HobString("heaterReading: ") +
								 HobString(Integer::toString((UINT16) heaterReading)));
	Debug::trace("WrapperSensors", HobString("encoderCount: ") +
								 HobString(Integer::toString((UINT16) encoderCount)));
	Debug::trace("WrapperSensors", HobString("intakeCount: ") +
								 HobString(Integer::toString((UINT16) intakeCount)));

	if (stopSwitch == true)
	{
		Debug::trace("WrapperSensors", "stopSwitch: OPEN");
	}

	if (startSwitch == true)
	{
		Debug::trace("WrapperSensors", "startSwitch: OPEN");
	}

	if (packageLengthSwitch == true)
	{
		Debug::trace("WrapperSensors", "packageLengthSwitch: OPEN");
	}

	if (packageSenseSwitch == true)
	{
		Debug::trace("WrapperSensors", "packageSenseSwitch: OPEN");
	}

	if (packageWidthSwitch == true)
	{
		Debug::trace("WrapperSensors", "packageWidthSwitch: OPEN");
	}

	if (pusherHomeSwitch == true)
	{
		Debug::trace("WrapperSensors", "pusherHomeSwitch: OPEN");
	}

	if (packageJamSwitch == true)
	{
		Debug::trace("WrapperSensors", "packageJamSwitch: OPEN");
	}

	if (wideFilmSwitch == true)
	{
		Debug::trace("WrapperSensors", "wideFilmSwitch: OPEN");
	}

	if (narrowFilmSwitch == true)
	{
		Debug::trace("WrapperSensors", "narrowFilmSwitch: OPEN");
	}

	if (slideBlockState == true)
	{
		Debug::trace("WrapperSensors", "slideBlockState: OPEN");
	}

	if (heaterState == true)
	{
		Debug::trace("WrapperSensors", "heaterState: OPEN");
	}

	if (compressorState == true)
	{
		Debug::trace("WrapperSensors", "compressorState: OPEN");
	}

	if (mainMotorState == true)
	{
		Debug::trace("WrapperSensors", "mainMotorState: OPEN");
	}

	if (dischargeMotorState == true)
	{
		Debug::trace("WrapperSensors", "dischargeMotorState: OPEN");
	}

	if (filmLengthState == ShortPackage)
	{
		Debug::trace("WrapperSensors", "filmLengthState: ShortPackage");
	}
	else if (filmLengthState == MediumPackage)
	{
		Debug::trace("WrapperSensors", "filmLengthState: MediumPackage");
	}
	else if (filmLengthState == LongPackage)
	{
		Debug::trace("WrapperSensors", "filmLengthState: LongPackage");
	}
	else if (filmLengthState == FilmLengthIdle)
	{
		Debug::trace("WrapperSensors", "filmLengthState: FilmLengthIdle");
	}
	else if (filmLengthState == AutoLengthSelect)
	{
		Debug::trace("WrapperSensors", "filmLengthState: AutoLengthSelect");
	}
	else
	{
		Debug::trace("WrapperSensors", HobString("filmLengthState: Unknown ") +
									 HobString(Integer::toString((UINT16) filmLengthState)));
	}

	if (filmWidthState == NarrowPackage)
	{
		Debug::trace("WrapperSensors", "filmWidthState: NarrowPackage!");
	}
	else if (filmWidthState == WidePackage)
	{
		Debug::trace("WrapperSensors", "filmWidthState: WidePackage!");
	}
	else if (filmWidthState == FilmWidthIdle)
	{
		Debug::trace("WrapperSensors", "filmWidthState: FilmWidthIdle!");
	}
	else if (filmWidthState == AutoWidthSelect)
	{
		Debug::trace("WrapperSensors", "filmWidthState: AutoWidthSelect!");
	}
	else
	{
		Debug::trace("WrapperSensors", HobString("filmWidthState: Unknown ") +
											 HobString(Integer::toString((UINT16) filmWidthState)));
	}

	if (knifeState == true)
	{
		Debug::trace("WrapperSensors", "knifeState: ON");
	}

	if (leftSideClampState == true)
	{
		Debug::trace("WrapperSensors", "leftSideClampState: ON");
	}

	if (rightSideClampState == true)
	{
		Debug::trace("WrapperSensors", "rightSideClampState: ON");
	}

	if (gripperState == true)
	{
		Debug::trace("WrapperSensors", "gripperState: ON");
	}

	Debug::removeTraceCategory("WrapperSensors");
}

void WrapperSensors::setHeaterReading(UINT16 heaterReading)
{
	this->heaterReading = heaterReading;
}

UINT16 WrapperSensors::getHeaterReading() const
{
	return heaterReading;
}

void WrapperSensors::setEncoderCount(UINT16 encoderCount)
{
	this->encoderCount = encoderCount;
}

UINT16 WrapperSensors::getEncoderCount() const
{
	return encoderCount;
}

void WrapperSensors::setIntakeCount(UINT16 intakeCount)
{
	this->intakeCount = intakeCount;
}

UINT16 WrapperSensors::getIntakeCount() const
{
	return intakeCount;
}

bool WrapperSensors::isStopSwitch() const
{
	return stopSwitch;
}

void WrapperSensors::setStopSwitch()
{
	stopSwitch = true;
}

void WrapperSensors::clearStopSwitch()
{
	stopSwitch = false;
}

bool WrapperSensors::isStartSwitch() const
{
	return startSwitch;
}

void WrapperSensors::setStartSwitch()
{
	startSwitch = true;
}

void WrapperSensors::clearStartSwitch()
{
	startSwitch = false;
}

bool WrapperSensors::isPackageLengthSwitch() const
{
	return packageLengthSwitch;
}

void WrapperSensors::setPackageLengthSwitch()
{
	packageLengthSwitch = true;
}

void WrapperSensors::clearPackageLengthSwitch()
{
	packageLengthSwitch = false;
}

bool WrapperSensors::isPackageSenseSwitch() const
{
	return packageSenseSwitch;
}

void WrapperSensors::setPackageSenseSwitch()
{
	packageSenseSwitch = true;
}

void WrapperSensors::clearPackageSenseSwitch()
{
	packageSenseSwitch = false;
}

bool WrapperSensors::isPackageWidthSwitch() const
{
	return packageWidthSwitch;
}

void WrapperSensors::setPackageWidthSwitch()
{
	packageWidthSwitch = true;
}

void WrapperSensors::clearPackageWidthSwitch()
{
	packageWidthSwitch = false;
}

bool WrapperSensors::isPusherHomeSwitch() const
{
	return pusherHomeSwitch;
}

void WrapperSensors::setPusherHomeSwitch()
{
	pusherHomeSwitch = true;
}

void WrapperSensors::clearPusherHomeSwitch()
{
	pusherHomeSwitch = false;
}

bool WrapperSensors::isPackageJamSwitch() const
{
	return packageJamSwitch;
}

void WrapperSensors::setPackageJamSwitch()
{
	packageJamSwitch = true;
}

void WrapperSensors::clearPackageJamSwitch()
{
	packageJamSwitch = false;
}

bool WrapperSensors::isWideFilmSwitch() const
{
	return wideFilmSwitch;
}

void WrapperSensors::setWideFilmSwitch()
{
	wideFilmSwitch = true;
}

void WrapperSensors::clearWideFilmSwitch()
{
	wideFilmSwitch = false;
}

bool WrapperSensors::isNarrowFilmSwitch() const
{
	return narrowFilmSwitch;
}

void WrapperSensors::setNarrowFilmSwitch()
{
	narrowFilmSwitch = true;
}

void WrapperSensors::clearNarrowFilmSwitch()
{
	narrowFilmSwitch = false;
}

bool WrapperSensors::isSlideBlockState() const
{
	return slideBlockState;
}

void WrapperSensors::setSlideBlockState()
{
	slideBlockState = true;
}

void WrapperSensors::clearSlideBlockState()
{
	slideBlockState = false;
}

bool WrapperSensors::isHeaterState() const
{
	return heaterState;
}

void WrapperSensors::setHeaterState()
{
	heaterState = true;
}

void WrapperSensors::clearHeaterState()
{
	heaterState = false;
}

bool WrapperSensors::isCompressorState() const
{
	return compressorState;
}

void WrapperSensors::setCompressorState()
{
	compressorState = true;
}

void WrapperSensors::clearCompressorState()
{
	compressorState = false;
}

bool WrapperSensors::isMainMotorState() const
{
	return mainMotorState;
}

void WrapperSensors::setMainMotorState()
{
	mainMotorState = true;
}

void WrapperSensors::clearMainMotorState()
{
	mainMotorState = false;
}

bool WrapperSensors::isDischargeMotorState() const
{
	return dischargeMotorState;
}

void WrapperSensors::setDischargeMotorState()
{
	dischargeMotorState = true;
}

void WrapperSensors::clearDischargeMotorState()
{
	dischargeMotorState = false;
}

WrPackLength WrapperSensors::getFilmLengthState() const
{
	return filmLengthState;
}

void WrapperSensors::setFilmLengthState(WrPackLength filmLengthState)
{
	this->filmLengthState = filmLengthState;
}

bool WrapperSensors::isFilmLengthStateShortPackage() const
{
	return (filmLengthState == ShortPackage);
}

bool WrapperSensors::isFilmLengthStateMediumPackage() const
{
	return (filmLengthState == MediumPackage);
}

bool WrapperSensors::isFilmLengthStateLongPackage() const
{
	return (filmLengthState == LongPackage);
}

bool WrapperSensors::isFilmLengthStateFilmLengthIdle() const
{
	return (filmLengthState == FilmLengthIdle);
}

bool WrapperSensors::isFilmLengthStateAutoLengthSelect() const
{
	return (filmLengthState == AutoLengthSelect);
}

void WrapperSensors::setFilmLengthStateShortPackage()
{
	filmLengthState = ShortPackage;
}

void WrapperSensors::setFilmLengthStateMediumPackage()
{
	filmLengthState = MediumPackage;
}

void WrapperSensors::setFilmLengthStateLongPackage()
{
	filmLengthState = LongPackage;
}

void WrapperSensors::setFilmLengthStateFilmLengthIdle()
{
	filmLengthState = FilmLengthIdle;
}

void WrapperSensors::setFilmLengthStateAutoLengthSelect()
{
	filmLengthState = AutoLengthSelect;
}

WrPackWidth WrapperSensors::getFilmWidthState() const
{
	return filmWidthState;
}

void WrapperSensors::setFilmWidthState(WrPackWidth filmWidthState)
{
	this->filmWidthState = filmWidthState;
}

bool WrapperSensors::isFilmWidthStateNarrowPackage() const
{
	return (filmWidthState == NarrowPackage);
}

bool WrapperSensors::isFilmWidthStateWidePackage() const
{
	return (filmWidthState == WidePackage);
}

bool WrapperSensors::isFilmWidthStateFilmWidthIdle() const
{
	return (filmWidthState == FilmWidthIdle);
}

bool WrapperSensors::isFilmWidthStateAutoWidthSelect() const
{
	return (filmWidthState == AutoWidthSelect);
}

void WrapperSensors::setFilmWidthStateNarrowPackage()
{
	filmWidthState = NarrowPackage;
}

void WrapperSensors::setFilmWidthStateWidePackage()
{
	filmWidthState = WidePackage;
}

void WrapperSensors::setFilmWidthStateFilmWidthIdle()
{
	filmWidthState = FilmWidthIdle;
}

void WrapperSensors::setFilmWidthStateAutoWidthSelect()
{
	filmWidthState = AutoWidthSelect;
}

bool WrapperSensors::isKnifeState() const
{
	return knifeState;
}

void WrapperSensors::setKnifeState()
{
	knifeState = true;
}

void WrapperSensors::clearKnifeState()
{
	knifeState = false;
}

bool WrapperSensors::isLeftSideClampState() const
{
	return leftSideClampState;
}

void WrapperSensors::setLeftSideClampState()
{
	leftSideClampState = true;
}

void WrapperSensors::clearLeftSideClampState()
{
	leftSideClampState = false;
}

bool WrapperSensors::isRightSideClampState() const
{
	return rightSideClampState;
}

void WrapperSensors::setRightSideClampState()
{
	rightSideClampState = true;
}

void WrapperSensors::clearRightSideClampState()
{
	rightSideClampState = false;
}

bool WrapperSensors::isGripperState() const
{
	return gripperState;
}

void WrapperSensors::setGripperState()
{
	gripperState = true;
}

void WrapperSensors::clearGripperState()
{
	gripperState = false;
}

bool WrapperSensors::equals(WrapperSensors &wrSense)
{
	bool rv = false;

	if	((heaterReading == wrSense.getHeaterReading()) &&
		 (encoderCount == wrSense.getEncoderCount()) &&
		 (intakeCount == wrSense.getIntakeCount()) &&
		 (stopSwitch == wrSense.isStopSwitch()) &&
		 (startSwitch == wrSense.isStartSwitch()) &&
		 (packageLengthSwitch == wrSense.isPackageLengthSwitch()) &&
		 (packageSenseSwitch == wrSense.isPackageSenseSwitch()) &&
		 (packageWidthSwitch == wrSense.isPackageWidthSwitch()) &&
		 (pusherHomeSwitch == wrSense.isPusherHomeSwitch()) &&
		 (packageJamSwitch == wrSense.isPackageJamSwitch()) &&
		 (wideFilmSwitch == wrSense.isWideFilmSwitch()) &&
		 (narrowFilmSwitch == wrSense.isNarrowFilmSwitch()) &&
		 (slideBlockState == wrSense.isSlideBlockState()) &&
		 (heaterState == wrSense.isHeaterState()) &&
		 (compressorState == wrSense.isCompressorState()) &&
		 (mainMotorState == wrSense.isMainMotorState()) &&
		 (dischargeMotorState == wrSense.isDischargeMotorState()) &&
		 (filmLengthState == wrSense.getFilmLengthState()) &&
		 (filmWidthState == wrSense.getFilmWidthState()) &&
		 (knifeState == wrSense.isKnifeState()) &&
		 (leftSideClampState == wrSense.isLeftSideClampState()) &&
		 (rightSideClampState == wrSense.isRightSideClampState()) &&
		 (gripperState == wrSense.isGripperState()))
	{
		rv = true;
	}

	return rv;
}
