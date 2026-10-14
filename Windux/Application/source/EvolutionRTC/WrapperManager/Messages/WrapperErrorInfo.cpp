//#include "vxWorks.h"
#include "WrapperErrorInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperErrorInfo::dataType = 0;
std::string WrapperErrorInfo::dataTypeName("WrapperErrorInfo"); 
#endif

WrapperErrorInfo::WrapperErrorInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initClassMembers();
}

WrapperErrorInfo::WrapperErrorInfo(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int j = 0;

	char *pChar = (char *) &errorClass;
	for (unsigned int i = 0; i < sizeof(errorClass); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &errorSource;
	for (unsigned int i = 0; i < sizeof(errorSource); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &eepWriteFailure;
	for (unsigned int i = 0; i < sizeof(eepWriteFailure); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &heaterFailure;
	for (unsigned int i = 0; i < sizeof(heaterFailure); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageTooLarge;
	for (unsigned int i = 0; i < sizeof(packageTooLarge); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &generalFault1;
	for (unsigned int i = 0; i < sizeof(generalFault1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &generalFault2;
	for (unsigned int i = 0; i < sizeof(generalFault2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sealerBeltInterlock;
	for (unsigned int i = 0; i < sizeof(sealerBeltInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &elevatorInterlock;
	for (unsigned int i = 0; i < sizeof(elevatorInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &scalePlatterInterlock;
	for (unsigned int i = 0; i < sizeof(scalePlatterInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &handCrankDoorInterlock;
	for (unsigned int i = 0; i < sizeof(handCrankDoorInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmDoorInterlock;
	for (unsigned int i = 0; i < sizeof(filmDoorInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &tooTallInterlock;
	for (unsigned int i = 0; i < sizeof(tooTallInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &leftCanopyInterlock;
	for (unsigned int i = 0; i < sizeof(leftCanopyInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rightCanopyInterlock;
	for (unsigned int i = 0; i < sizeof(rightCanopyInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &frontCoverInterlock;
	for (unsigned int i = 0; i < sizeof(frontCoverInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &leftSideCoverInterlock;
	for (unsigned int i = 0; i < sizeof(leftSideCoverInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rightSideCoverInterlock;
	for (unsigned int i = 0; i < sizeof(rightSideCoverInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wideFilmSupplyFailure;
	for (unsigned int i = 0; i < sizeof(wideFilmSupplyFailure); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &narrowFilmSupplyFailure;
	for (unsigned int i = 0; i < sizeof(narrowFilmSupplyFailure); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &handCrankDoorInterlock;
	for (unsigned int i = 0; i < sizeof(handCrankDoorInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageJammed;
	for (unsigned int i = 0; i < sizeof(packageJammed); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &mainMotorStalled;
	for (unsigned int i = 0; i < sizeof(mainMotorStalled); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeHomeFailure;
	for (unsigned int i = 0; i < sizeof(intakeHomeFailure); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeWidthFailure;
	for (unsigned int i = 0; i < sizeof(intakeWidthFailure); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeLengthFailure;
	for (unsigned int i = 0; i < sizeof(intakeLengthFailure); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeMotorJammed;
	for (unsigned int i = 0; i < sizeof(intakeMotorJammed); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakePackagePresent;
	for (unsigned int i = 0; i < sizeof(intakePackagePresent); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeOutOfSync;
	for (unsigned int i = 0; i < sizeof(intakeOutOfSync); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &stopSwitchInterlock;
	for (unsigned int i = 0; i < sizeof(stopSwitchInterlock); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperErrorInfo::~WrapperErrorInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *WrapperErrorInfo::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &errorClass;
	for (unsigned int i = 0; i < sizeof(errorClass); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &errorSource;
	for (unsigned int i = 0; i < sizeof(errorSource); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &eepWriteFailure;
	for (unsigned int i = 0; i < sizeof(eepWriteFailure); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &heaterFailure;
	for (unsigned int i = 0; i < sizeof(heaterFailure); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageTooLarge;
	for (unsigned int i = 0; i < sizeof(packageTooLarge); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &generalFault1;
	for (unsigned int i = 0; i < sizeof(generalFault1); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &generalFault2;
	for (unsigned int i = 0; i < sizeof(generalFault2); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sealerBeltInterlock;
	for (unsigned int i = 0; i < sizeof(sealerBeltInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &elevatorInterlock;
	for (unsigned int i = 0; i < sizeof(elevatorInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &scalePlatterInterlock;
	for (unsigned int i = 0; i < sizeof(scalePlatterInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &handCrankDoorInterlock;
	for (unsigned int i = 0; i < sizeof(handCrankDoorInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmDoorInterlock;
	for (unsigned int i = 0; i < sizeof(filmDoorInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &tooTallInterlock;
	for (unsigned int i = 0; i < sizeof(tooTallInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &leftCanopyInterlock;
	for (unsigned int i = 0; i < sizeof(leftCanopyInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rightCanopyInterlock;
	for (unsigned int i = 0; i < sizeof(rightCanopyInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &frontCoverInterlock;
	for (unsigned int i = 0; i < sizeof(frontCoverInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &leftSideCoverInterlock;
	for (unsigned int i = 0; i < sizeof(leftSideCoverInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rightSideCoverInterlock;
	for (unsigned int i = 0; i < sizeof(rightSideCoverInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wideFilmSupplyFailure;
	for (unsigned int i = 0; i < sizeof(wideFilmSupplyFailure); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &narrowFilmSupplyFailure;
	for (unsigned int i = 0; i < sizeof(narrowFilmSupplyFailure); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &handCrankDoorInterlock;
	for (unsigned int i = 0; i < sizeof(handCrankDoorInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageJammed;
	for (unsigned int i = 0; i < sizeof(packageJammed); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &mainMotorStalled;
	for (unsigned int i = 0; i < sizeof(mainMotorStalled); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeHomeFailure;
	for (unsigned int i = 0; i < sizeof(intakeHomeFailure); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeWidthFailure;
	for (unsigned int i = 0; i < sizeof(intakeWidthFailure); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeLengthFailure;
	for (unsigned int i = 0; i < sizeof(intakeLengthFailure); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeMotorJammed;
	for (unsigned int i = 0; i < sizeof(intakeMotorJammed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakePackagePresent;
	for (unsigned int i = 0; i < sizeof(intakePackagePresent); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeOutOfSync;
	for (unsigned int i = 0; i < sizeof(intakeOutOfSync); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &stopSwitchInterlock;
	for (unsigned int i = 0; i < sizeof(stopSwitchInterlock); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 WrapperErrorInfo::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperErrorInfo::show()
{
	Debug::addTraceCategory("WrapperErrorInfo");
	Debug::trace("WrapperErrorInfo", "WrapperErrorInfo: ");

	if (errorSource == NoErrorSource)
	{
		Debug::trace("WrapperErrorInfo", "errorSource: NoErrorSource");
	}

	if (errorClass == NoErrorClass)
	{
		Debug::trace("WrapperErrorInfo", "errorClass: NoErrorClass");
	}
	else if (errorClass == ActiveErrorClass)
	{
		Debug::trace("WrapperErrorInfo", "errorClass: ActiveErrorClass");
	}
	else if (errorClass == IdleErrorClass)
	{
		Debug::trace("WrapperErrorInfo", "errorClass: IdleErrorClass");
	}
	else if (errorClass == OtherErrorClass)
	{
		Debug::trace("WrapperErrorInfo", "errorClass: OtherErrorClass");
	}
	else if (errorClass == UnknownErrorClass)
	{
		Debug::trace("WrapperErrorInfo", "errorClass: UnknownErrorClass");
	}
	else
	{
		Debug::trace("WrapperErrorInfo", HobString("errorClass: Unknown ") +
										 HobString(Integer::toString((UINT16) errorClass)));
	}

	if (eepWriteFailure == true)
	{
		Debug::trace("WrapperErrorInfo", "eepWriteFailure: ON");
	}

	if (heaterFailure == true)
	{
		Debug::trace("WrapperErrorInfo", "heaterFailure: ON");
	}

	if (packageTooLarge == true)
	{
		Debug::trace("WrapperErrorInfo", "packageTooLarge: ON");
	}

	if (generalFault1 == true)
	{
		Debug::trace("WrapperErrorInfo", "generalFault1: ON");
	}

	if (generalFault2 == true)
	{
		Debug::trace("WrapperErrorInfo", "generalFault2: ON");
	}

	if (sealerBeltInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "sealerBeltInterlock: ON");
	}

	if (elevatorInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "elevatorInterlock: ON");
	}

	if (scalePlatterInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "scalePlatterInterlock: ON");
	}

	if (handCrankDoorInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "handCrankDoorInterlock: ON");
	}

	if (filmDoorInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "filmDoorInterlock: ON");
	}

	if (tooTallInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "tooTallInterlock: ON");
	}

	if (leftCanopyInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "leftCanopyInterlock: ON");
	}

	if (rightCanopyInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "rightCanopyInterlock: ON");
	}

	if (frontCoverInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "frontCoverInterlock: ON");
	}

	if (leftSideCoverInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "leftSideCoverInterlock: ON");
	}

	if (rightSideCoverInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "rightSideCoverInterlock: ON");
	}

	if (wideFilmSupplyFailure == true)
	{
		Debug::trace("WrapperErrorInfo", "wideFilmSupplyFailure: ON");
	}

	if (narrowFilmSupplyFailure == true)
	{
		Debug::trace("WrapperErrorInfo", "narrowFilmSupplyFailure: ON");
	}

	if (packageJammed == true)
	{
		Debug::trace("WrapperErrorInfo", "packageJammed: ON");
	}

	if (mainMotorStalled == true)
	{
		Debug::trace("WrapperErrorInfo", "mainMotorStalled: ON");
	}

	if (intakeHomeFailure == true)
	{
		Debug::trace("WrapperErrorInfo", "intakeHomeFailure: ON");
	}

	if (intakeWidthFailure == true)
	{
		Debug::trace("WrapperErrorInfo", "intakeWidthFailure: ON");
	}

	if (intakeLengthFailure == true)
	{
		Debug::trace("WrapperErrorInfo", "intakeLengthFailure: ON");
	}

	if (intakeMotorJammed == true)
	{
		Debug::trace("WrapperErrorInfo", "intakeMotorJammed: ON");
	}

	if (intakePackagePresent == true)
	{
		Debug::trace("WrapperErrorInfo", "intakePackagePresent: ON");
	}

	if (intakeOutOfSync == true)
	{
		Debug::trace("WrapperErrorInfo", "intakeOutOfSync: ON");
	}

	if (stopSwitchInterlock == true)
	{
		Debug::trace("WrapperErrorInfo", "stopSwitchInterlock: ON");
	}

	Debug::removeTraceCategory("WrapperErrorInfo");
}

bool WrapperErrorInfo::isEepWriteFailure() const
{
	return eepWriteFailure;
}

void WrapperErrorInfo::setEepWriteFailure()
{
	eepWriteFailure = true;
	setError(EepWriteFailure, ActiveErrorClass);
}

void WrapperErrorInfo::clearEepWriteFailure()
{
	eepWriteFailure = false;
	setError();
}

bool WrapperErrorInfo::isHeaterFailure() const
{
	return heaterFailure;
}

void WrapperErrorInfo::setHeaterFailure()
{
	heaterFailure = true;
	setError(HeaterFailure, ActiveErrorClass);
}

void WrapperErrorInfo::clearHeaterFailure()
{
	heaterFailure = false;
	setError();
}

bool WrapperErrorInfo::isPackageTooLarge() const
{
	return packageTooLarge;
}

void WrapperErrorInfo::setPackageTooLarge()
{
	packageTooLarge = true;
	setError(PackageTooLarge, ActiveErrorClass);
}

void WrapperErrorInfo::clearPackageTooLarge()
{
	packageTooLarge = false;
	setError();
}

bool WrapperErrorInfo::isGeneralFault1() const
{
	return generalFault1;
}

void WrapperErrorInfo::setGeneralFault1()
{
	generalFault1 = true;
	setError(GeneralFault1, ActiveErrorClass);
}

void WrapperErrorInfo::clearGeneralFault1()
{
	generalFault1 = false;
	setError();
}

bool WrapperErrorInfo::isGeneralFault2() const
{
	return generalFault2;
}

void WrapperErrorInfo::setGeneralFault2()
{
	generalFault2 = true;
	setError(GeneralFault2, ActiveErrorClass);
}

void WrapperErrorInfo::clearGeneralFault2()
{
	generalFault2 = false;
	setError();
}

bool WrapperErrorInfo::isSealerBeltInterlock() const
{
	return sealerBeltInterlock;
}

void WrapperErrorInfo::setSealerBeltInterlock()
{
	sealerBeltInterlock = true;
	setError(SealerBeltInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearSealerBeltInterlock()
{
	sealerBeltInterlock = false;
	setError();
}

bool WrapperErrorInfo::isElevatorInterlock() const
{
	return elevatorInterlock;
}

void WrapperErrorInfo::setElevatorInterlock()
{
	elevatorInterlock = true;
	setError(ElevatorInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearElevatorInterlock()
{
	elevatorInterlock = false;
	setError();
}

bool WrapperErrorInfo::isScalePlatterInterlock() const
{
	return scalePlatterInterlock;
}

void WrapperErrorInfo::setScalePlatterInterlock()
{
	scalePlatterInterlock = true;
	setError(IntakeInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearScalePlatterInterlock()
{
	scalePlatterInterlock = false;
	setError();
}

bool WrapperErrorInfo::isHandCrankDoorInterlock() const
{
	return handCrankDoorInterlock;
}

void WrapperErrorInfo::setHandCrankDoorInterlock()
{
	handCrankDoorInterlock = true;
	setError(HandCrankDoorInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearHandCrankDoorInterlock()
{
	handCrankDoorInterlock = false;
	setError();
}

bool WrapperErrorInfo::isFilmDoorInterlock() const
{
	return filmDoorInterlock;
}

void WrapperErrorInfo::setFilmDoorInterlock()
{
	filmDoorInterlock = true;
	setError(FilmDoorInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearFilmDoorInterlock()
{
	filmDoorInterlock = false;
	setError();
}

bool WrapperErrorInfo::isTooTallInterlock() const
{
	return tooTallInterlock;
}

void WrapperErrorInfo::setTooTallInterlock()
{
	tooTallInterlock = true;
	setError(TooTallInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearTooTallInterlock()
{
	tooTallInterlock = false;
	setError();
}

bool WrapperErrorInfo::isLeftCanopyInterlock() const
{
	return leftCanopyInterlock;
}

void WrapperErrorInfo::setLeftCanopyInterlock()
{
	leftCanopyInterlock = true;
	setError(LeftCanopyInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearLeftCanopyInterlock()
{
	leftCanopyInterlock = false;
	setError();
}

bool WrapperErrorInfo::isRightCanopyInterlock() const
{
	return rightCanopyInterlock;
}

void WrapperErrorInfo::setRightCanopyInterlock()
{
	rightCanopyInterlock = true;
	setError(RightCanopyInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearRightCanopyInterlock()
{
	rightCanopyInterlock = false;
	setError();
}

bool WrapperErrorInfo::isFrontCoverInterlock() const
{
	return frontCoverInterlock;
}

void WrapperErrorInfo::setFrontCoverInterlock()
{
	frontCoverInterlock = true;
	setError(FrontCoverInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearFrontCoverInterlock()
{
	frontCoverInterlock = false;
	setError();
}

bool WrapperErrorInfo::isLeftSideCoverInterlock() const
{
	return leftSideCoverInterlock;
}

void WrapperErrorInfo::setLeftSideCoverInterlock()
{
	leftSideCoverInterlock = true;
	setError(LeftSideCoverInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearLeftSideCoverInterlock()
{
	leftSideCoverInterlock = false;
	setError();
}

bool WrapperErrorInfo::isRightSideCoverInterlock() const
{
	return rightSideCoverInterlock;
}

void WrapperErrorInfo::setRightSideCoverInterlock()
{
	rightSideCoverInterlock = true;
	setError(RightSideCoverInterlock, ActiveErrorClass);
}

void WrapperErrorInfo::clearRightSideCoverInterlock()
{
	rightSideCoverInterlock = false;
	setError();
}

bool WrapperErrorInfo::isWideFilmSupplyFailure() const
{
	return wideFilmSupplyFailure;
}

void WrapperErrorInfo::setWideFilmSupplyFailure()
{
	wideFilmSupplyFailure = true;
	setError(WideFilmFailure, ActiveErrorClass);
}

void WrapperErrorInfo::clearWideFilmSupplyFailure()
{
	wideFilmSupplyFailure = false;
	setError();
}

bool WrapperErrorInfo::isNarrowFilmSupplyFailure() const
{
	return narrowFilmSupplyFailure;
}

void WrapperErrorInfo::setNarrowFilmSupplyFailure()
{
	narrowFilmSupplyFailure = true;
	setError(NarrowFilmFailure, ActiveErrorClass);
}

void WrapperErrorInfo::clearNarrowFilmSupplyFailure()
{
	narrowFilmSupplyFailure = false;
	setError();
}

bool WrapperErrorInfo::isPackageJammed() const
{
	return packageJammed;
}

void WrapperErrorInfo::setPackageJammed()
{
	packageJammed = true;
	setError(PackageJammed, ActiveErrorClass);
}

void WrapperErrorInfo::clearPackageJammed()
{
	packageJammed = false;
	setError();
}

bool WrapperErrorInfo::isMainMotorStalled() const
{
	return mainMotorStalled;
}

void WrapperErrorInfo::setMainMotorStalled()
{
	mainMotorStalled = true;
	setError(MainMotorJammed, ActiveErrorClass);
}

void WrapperErrorInfo::clearMainMotorStalled()
{
	mainMotorStalled = false;
	setError();
}

bool WrapperErrorInfo::isIntakeHomeFailure() const
{
	return intakeHomeFailure;
}

void WrapperErrorInfo::setIntakeHomeFailure()
{
	intakeHomeFailure = true;
	setError(IntakeHomeFailure, ActiveErrorClass);
}

void WrapperErrorInfo::clearIntakeHomeFailure()
{
	intakeHomeFailure = false;
	setError();
}

bool WrapperErrorInfo::isIntakeWidthFailure() const
{
	return intakeWidthFailure;
}

void WrapperErrorInfo::setIntakeWidthFailure()
{
	intakeWidthFailure = true;
	setError(IntakeWidthFailure, ActiveErrorClass);
}

void WrapperErrorInfo::clearIntakeWidthFailure()
{
	intakeWidthFailure = false;
	setError();
}

bool WrapperErrorInfo::isIntakeLengthFailure() const
{
	return intakeLengthFailure;
}

void WrapperErrorInfo::setIntakeLengthFailure()
{
	intakeLengthFailure = true;
	setError(IntakeLengthFailure, ActiveErrorClass);
}

void WrapperErrorInfo::clearIntakeLengthFailure()
{
	intakeLengthFailure = false;
	setError();
}

bool WrapperErrorInfo::isIntakeMotorJammed() const
{
	return intakeMotorJammed;
}

void WrapperErrorInfo::setIntakeMotorJammed()
{
	intakeMotorJammed = true;
	setError(IntakeMotorJammed, ActiveErrorClass);
}

void WrapperErrorInfo::clearIntakeMotorJammed()
{
	intakeMotorJammed = false;
	setError();
}

bool WrapperErrorInfo::isIntakePackagePresent() const
{
	return intakePackagePresent;
}

void WrapperErrorInfo::setIntakePackagePresent()
{
	intakePackagePresent = true;
	setError(IntakePackagePresent, OtherErrorClass);
}

void WrapperErrorInfo::clearIntakePackagePresent()
{
	intakePackagePresent = false;
	setError();
}

bool WrapperErrorInfo::isIntakeOutOfSync() const
{
	return intakeOutOfSync;
}

void WrapperErrorInfo::setIntakeOutOfSync()
{
	intakeOutOfSync = true;
	setError(IntakeOutOfSync, OtherErrorClass);
}

void WrapperErrorInfo::clearIntakeOutOfSync()
{
	intakeOutOfSync = false;
	setError();
}

bool WrapperErrorInfo::isStopSwitchInterlock() const
{
	return stopSwitchInterlock;
}

void WrapperErrorInfo::setStopSwitchInterlock()
{
	stopSwitchInterlock = true;
	setError(StopSwitchInterlock, IdleErrorClass);
}

void WrapperErrorInfo::clearStopSwitchInterlock()
{
	stopSwitchInterlock = false;
}

void WrapperErrorInfo::setErrorBits(WrErrorBits errorBits)
{
	initClassMembers();

	if (errorBits.packageTooLarge == 1)
	{
		packageTooLarge = true;
		setError(PackageTooLarge, ActiveErrorClass);
	}

	if (errorBits.heaterFailure == 1)
	{
		heaterFailure = true;
		setError(HeaterFailure, ActiveErrorClass);
	}

	if (errorBits.eepWriteFailure == 1)
	{
		eepWriteFailure = true;
		setError(EepWriteFailure, ActiveErrorClass);
	}

	if (errorBits.tooTallInterlock == 1)
	{
		tooTallInterlock = true;
		setError(TooTallInterlock, ActiveErrorClass);
	}

	if (errorBits.filmDoorInterlock == 1)
	{
		filmDoorInterlock = true;
		setError(FilmDoorInterlock, ActiveErrorClass);
	}

	if (errorBits.handCrankDoorInterlock == 1)
	{
		handCrankDoorInterlock = true;
		setError(HandCrankDoorInterlock, ActiveErrorClass);
	}

	if (errorBits.scalePlatterInterlock == 1)
	{
		scalePlatterInterlock = true;
		setError(IntakeInterlock, ActiveErrorClass);
	}

	if (errorBits.elevatorInterlock == 1)
	{
		elevatorInterlock = true;
		setError(ElevatorInterlock, ActiveErrorClass);
	}

	if (errorBits.sealerBeltInterlock == 1)
	{
		sealerBeltInterlock = true;
		setError(SealerBeltInterlock, ActiveErrorClass);
	}

	if (errorBits.generalFault2 == 1)
	{
		generalFault2 = true;
		setError(GeneralFault2, ActiveErrorClass);
	}

	if (errorBits.generalFault1 == 1)
	{
		generalFault1 = true;
		setError(GeneralFault1, ActiveErrorClass);
	}

	if (errorBits.leftCanopyInterlock == 1)
	{
		leftCanopyInterlock = true;
		setError(LeftCanopyInterlock, ActiveErrorClass);
	}

	if (errorBits.rightCanopyInterlock == 1)
	{
		rightCanopyInterlock = true;
		setError(RightCanopyInterlock, ActiveErrorClass);
	}

	if (errorBits.frontCoverInterlock == 1)
	{
		frontCoverInterlock = true;
		setError(FrontCoverInterlock, ActiveErrorClass);
	}

	if (errorBits.leftSideCoverInterlock == 1)
	{
		leftSideCoverInterlock = true;
		setError(LeftSideCoverInterlock, ActiveErrorClass);
	}

	if (errorBits.rightSideCoverInterlock == 1)
	{
		rightSideCoverInterlock = true;
		setError(RightSideCoverInterlock, ActiveErrorClass);
	}

	if (errorBits.wideFilmSupplyFailure == 1)
	{
		wideFilmSupplyFailure = true;
		setError(WideFilmFailure, ActiveErrorClass);
	}

	if (errorBits.narrowFilmSupplyFailure == 1)
	{
		narrowFilmSupplyFailure = true;
		setError(NarrowFilmFailure, ActiveErrorClass);
	}

	if (errorBits.packageJammed == 1)
	{
		packageJammed = true;
		setError(PackageJammed, ActiveErrorClass);
	}

	if (errorBits.mainMotorStalled == 1)
	{
		mainMotorStalled = true;
		setError(MainMotorJammed, ActiveErrorClass);
	}

	if (errorBits.intakeHomeFailure == 1)
	{
		intakeHomeFailure = true;
		setError(IntakeHomeFailure, ActiveErrorClass);
	}

	if (errorBits.intakeWidthFailure == 1)
	{
		intakeWidthFailure = true;
		setError(IntakeWidthFailure, ActiveErrorClass);
	}

	if (errorBits.intakeLengthFailure == 1)
	{
		intakeLengthFailure = true;
		setError(IntakeLengthFailure, ActiveErrorClass);
	}

	if (errorBits.intakeMotorJammed == 1)
	{
		intakeMotorJammed = true;
		setError(IntakeMotorJammed, ActiveErrorClass);
	}

	if (errorBits.intakePackagePresent == 1)
	{
		intakePackagePresent = true;
		setError(IntakePackagePresent, OtherErrorClass);
	}
	
	if (errorBits.intakeOutOfSync == 1)
	{
		intakeOutOfSync = true;
		setError(IntakeOutOfSync, OtherErrorClass);
	}

	if (errorBits.stopSwitchInterlock == 1)
	{
		stopSwitchInterlock = true;
		setError(StopSwitchInterlock, IdleErrorClass);
	}
}

WrErrorBits WrapperErrorInfo::getErrorBits() const
{
	WrErrorBits errorBits;

	memset (&errorBits, 0, sizeof (errorBits));

	if (errorSource != NoErrorSource)
	{
		if (packageTooLarge == true)
		{
			errorBits.packageTooLarge = 1;
		}

		if (heaterFailure == true)
		{
			errorBits.heaterFailure = 1;
		}

		if (eepWriteFailure == true)
		{
			errorBits.eepWriteFailure = 1;
		}

		if (tooTallInterlock == true)
		{
			errorBits.tooTallInterlock = 1;
		}

		if (filmDoorInterlock == true)
		{
			errorBits.filmDoorInterlock = 1;
		}

		if (handCrankDoorInterlock == true)
		{
			errorBits.handCrankDoorInterlock = 1;
		}

		if (scalePlatterInterlock == true)
		{
			errorBits.scalePlatterInterlock = 1;
		}

		if (elevatorInterlock == true)
		{
			errorBits.elevatorInterlock = 1;
		}

		if (sealerBeltInterlock == true)
		{
			errorBits.sealerBeltInterlock = 1;
		}

		if (generalFault2 == true)
		{
			errorBits.generalFault2 = 1;
		}

		if (generalFault1 == true)
		{
			errorBits.generalFault1 = 1;
		}

		if (leftCanopyInterlock == true)
		{
			errorBits.leftCanopyInterlock = 1;
		}

		if (rightCanopyInterlock == true)
		{
			errorBits.rightCanopyInterlock = 1;
		}

		if (frontCoverInterlock == true)
		{
			errorBits.frontCoverInterlock = 1;
		}

		if (leftSideCoverInterlock == true)
		{
			errorBits.leftSideCoverInterlock = 1;
		}

		if (rightSideCoverInterlock == true)
		{
			errorBits.rightSideCoverInterlock = 1;
		}

		if (wideFilmSupplyFailure == true)
		{
			errorBits.wideFilmSupplyFailure = 1;
		}

		if (narrowFilmSupplyFailure == true)
		{
			errorBits.narrowFilmSupplyFailure = 1;
		}

		if (packageJammed == true)
		{
			errorBits.packageJammed = 1;
		}

		if (mainMotorStalled == true)
		{
			errorBits.mainMotorStalled = 1;
		}

		if (intakeHomeFailure == true)
		{
			errorBits.intakeHomeFailure = 1;
		}

		if (intakeWidthFailure == true)
		{
			errorBits.intakeWidthFailure = 1;
		}

		if (intakeLengthFailure == true)
		{
			errorBits.intakeLengthFailure = 1;
		}

		if (intakeMotorJammed == true)
		{
			errorBits.intakeMotorJammed = 1;
		}

		if (intakePackagePresent == true)
		{
			errorBits.intakePackagePresent = 1;
		}

		if (intakeOutOfSync == true)
		{
			errorBits.intakeOutOfSync = 1;
		}

		if (stopSwitchInterlock == true)
		{
			errorBits.stopSwitchInterlock = 1;
		}
	}

	return errorBits;
}

bool WrapperErrorInfo::equals(WrapperErrorInfo &wrErrorInfo)
{
	bool rv = false;

	if ((eepWriteFailure == wrErrorInfo.isEepWriteFailure()) &&
		(heaterFailure == wrErrorInfo.isHeaterFailure()) &&
		(packageTooLarge == wrErrorInfo.isPackageTooLarge()) &&
		(generalFault1 == wrErrorInfo.isGeneralFault1()) &&
		(generalFault2 == wrErrorInfo.isGeneralFault2()) &&
		(sealerBeltInterlock == wrErrorInfo.isSealerBeltInterlock()) &&
		(elevatorInterlock == wrErrorInfo.isElevatorInterlock()) &&
		(scalePlatterInterlock == wrErrorInfo.isScalePlatterInterlock()) &&
		(handCrankDoorInterlock == wrErrorInfo.isHandCrankDoorInterlock()) &&
		(filmDoorInterlock == wrErrorInfo.isFilmDoorInterlock()) &&
		(tooTallInterlock == wrErrorInfo.isTooTallInterlock()) &&
		(leftCanopyInterlock == wrErrorInfo.isLeftCanopyInterlock()) &&
		(rightCanopyInterlock == wrErrorInfo.isRightCanopyInterlock()) &&
		(frontCoverInterlock == wrErrorInfo.isFrontCoverInterlock()) &&
		(leftSideCoverInterlock == wrErrorInfo.isLeftSideCoverInterlock()) &&
		(rightSideCoverInterlock == wrErrorInfo.isRightSideCoverInterlock()) &&
		(wideFilmSupplyFailure == wrErrorInfo.isWideFilmSupplyFailure()) &&
		(narrowFilmSupplyFailure == wrErrorInfo.isNarrowFilmSupplyFailure()) &&
		(packageJammed == wrErrorInfo.isPackageJammed()) &&
		(mainMotorStalled == wrErrorInfo.isMainMotorStalled()) &&
		(intakeHomeFailure == wrErrorInfo.isIntakeHomeFailure()) &&
		(intakeWidthFailure == wrErrorInfo.isIntakeWidthFailure()) &&
		(intakeLengthFailure == wrErrorInfo.isIntakeLengthFailure()) &&
		(intakeMotorJammed == wrErrorInfo.isIntakeMotorJammed()) &&
		(intakePackagePresent == wrErrorInfo.isIntakePackagePresent()) &&
		(intakeOutOfSync == wrErrorInfo.isIntakeOutOfSync()) &&
		(stopSwitchInterlock == wrErrorInfo.isStopSwitchInterlock()))
	{
		rv = true;
	}

	return rv;
}

bool WrapperErrorInfo::isWrapperError() const
{
	return (errorSource != NoErrorSource);
}

const WrapperErrorInfo::WrapperErrorClass &WrapperErrorInfo::getErrorClass() const
{
	return (const WrapperErrorInfo::WrapperErrorClass &) errorClass;
}

const WrapperErrorInfo::WrapperErrorSource &WrapperErrorInfo::getErrorSource() const
{
	return (const WrapperErrorInfo::WrapperErrorSource &) errorSource;
}

void WrapperErrorInfo::initClassMembers()
{
	errorSource = NoErrorSource;
	errorClass = NoErrorClass;

	eepWriteFailure = false;
	heaterFailure = false;
	packageTooLarge = false;
	generalFault1 = false;
	generalFault2 = false;
	sealerBeltInterlock = false;
	elevatorInterlock = false;
	scalePlatterInterlock = false;
	handCrankDoorInterlock = false;
	filmDoorInterlock = false;
	tooTallInterlock = false;
	leftCanopyInterlock = false;
	rightCanopyInterlock = false;
	frontCoverInterlock = false;
	leftSideCoverInterlock = false;
	rightSideCoverInterlock = false;
	wideFilmSupplyFailure = false;
	narrowFilmSupplyFailure = false;
	packageJammed = false;
	mainMotorStalled = false;
	intakeHomeFailure = false;
	intakeWidthFailure = false;
	intakeLengthFailure = false;
	intakeMotorJammed = false;
	intakePackagePresent = false;
	intakeOutOfSync = false;
	stopSwitchInterlock = false;
}

void WrapperErrorInfo::setError(WrapperErrorSource errorSource, WrapperErrorClass errorClass)
{
	if (this->errorSource < errorSource)
	{
		this->errorSource = errorSource;
		this->errorClass = errorClass;
	}
}

void WrapperErrorInfo::setError()
{
	if (packageTooLarge == true)
	{
		setError(PackageTooLarge, ActiveErrorClass);
	}

	if (heaterFailure == true)
	{
		setError(HeaterFailure, ActiveErrorClass);
	}

	if (eepWriteFailure == true)
	{
		setError(EepWriteFailure, ActiveErrorClass);
	}

	if (tooTallInterlock == true)
	{
		setError(TooTallInterlock, ActiveErrorClass);
	}

	if (filmDoorInterlock == true)
	{
		setError(FilmDoorInterlock, ActiveErrorClass);
	}

	if (handCrankDoorInterlock == true)
	{
		setError(HandCrankDoorInterlock, ActiveErrorClass);
	}

	if (scalePlatterInterlock == true)
	{
		setError(IntakeInterlock, ActiveErrorClass);
	}

	if (elevatorInterlock == true)
	{
		setError(ElevatorInterlock, ActiveErrorClass);
	}

	if (sealerBeltInterlock == true)
	{
		setError(SealerBeltInterlock, ActiveErrorClass);
	}

	if (generalFault2 == true)
	{
		setError(GeneralFault2, ActiveErrorClass);
	}

	if (generalFault1 == true)
	{
		setError(GeneralFault1, ActiveErrorClass);
	}

	if (leftCanopyInterlock == true)
	{
		setError(LeftCanopyInterlock, ActiveErrorClass);
	}

	if (rightCanopyInterlock == true)
	{
		setError(RightCanopyInterlock, ActiveErrorClass);
	}

	if (frontCoverInterlock == true)
	{
		setError(FrontCoverInterlock, ActiveErrorClass);
	}

	if (leftSideCoverInterlock == true)
	{
		setError(LeftSideCoverInterlock, ActiveErrorClass);
	}

	if (rightSideCoverInterlock == true)
	{
		setError(RightSideCoverInterlock, ActiveErrorClass);
	}

	if (wideFilmSupplyFailure == true)
	{
		setError(WideFilmFailure, ActiveErrorClass);
	}

	if (narrowFilmSupplyFailure == true)
	{
		setError(NarrowFilmFailure, ActiveErrorClass);
	}

	if (packageJammed == true)
	{
		setError(PackageJammed, ActiveErrorClass);
	}

	if (mainMotorStalled == true)
	{
		setError(MainMotorJammed, ActiveErrorClass);
	}

	if (intakeHomeFailure == true)
	{
		setError(IntakeHomeFailure, ActiveErrorClass);
	}

	if (intakeWidthFailure == true)
	{
		setError(IntakeWidthFailure, ActiveErrorClass);
	}

	if (intakeLengthFailure == true)
	{
		setError(IntakeLengthFailure, ActiveErrorClass);
	}

	if (intakeMotorJammed == true)
	{
		setError(IntakeMotorJammed, ActiveErrorClass);
	}

	if (intakePackagePresent == true)
	{
		setError(IntakePackagePresent, OtherErrorClass);
	}
	
	if (intakeOutOfSync == true)
	{
		setError(IntakeOutOfSync, OtherErrorClass);
	}

	if (stopSwitchInterlock == true)
	{
		setError(StopSwitchInterlock, IdleErrorClass);
	}
}
