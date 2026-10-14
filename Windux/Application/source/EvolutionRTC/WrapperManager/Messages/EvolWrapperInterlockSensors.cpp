#include "EvolWrapperInterlockSensors.h"

#include "Debug.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperInterlockSensors::_dataType_ = 0;
std::string EvolWrapperInterlockSensors::_dataTypeName_("EvolWrapperInterlockSensors"); 
#endif

EvolWrapperInterlockSensors::EvolWrapperInterlockSensors()
	: stop(0), rightCanopy(0), rightSideCover(0), filmSupplyDoor(0), 
	leftSideCover(0), intakeCleanout(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperInterlockSensors::EvolWrapperInterlockSensors(const char *buffer)
	: stop(0), rightCanopy(0), rightSideCover(0), filmSupplyDoor(0), 
	leftSideCover(0), intakeCleanout(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	int j = 0;

	char *pChar = (char *) &stop;
	for (unsigned int i = 0; i < sizeof(stop); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rightCanopy;
	for (unsigned int i = 0; i < sizeof(rightCanopy); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rightSideCover;
	for (unsigned int i = 0; i < sizeof(rightSideCover); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmSupplyDoor;
	for (unsigned int i = 0; i < sizeof(filmSupplyDoor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &leftSideCover;
	for (unsigned int i = 0; i < sizeof(leftSideCover); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &intakeCleanout;
	for (unsigned int i = 0; i < sizeof(intakeCleanout); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperInterlockSensors::EvolWrapperInterlockSensors(const EvolWrapperInterlockSensors &sensors)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	stop = sensors.isStopSwitch();
	rightCanopy = sensors.isRightCanopySwitch();
	rightSideCover = sensors.isRightSideCover();
	filmSupplyDoor = sensors.isFilmSupplyDoor();
	leftSideCover = sensors.isLeftSideCover();
	intakeCleanout = sensors.isIntakeCleanout();
}

EvolWrapperInterlockSensors::~EvolWrapperInterlockSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperInterlockSensors::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &stop;
	for (unsigned int i = 0; i < sizeof(stop); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rightCanopy;
	for (unsigned int i = 0; i < sizeof(rightCanopy); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rightSideCover;
	for (unsigned int i = 0; i < sizeof(rightSideCover); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmSupplyDoor;
	for (unsigned int i = 0; i < sizeof(filmSupplyDoor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &leftSideCover;
	for (unsigned int i = 0; i < sizeof(leftSideCover); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &intakeCleanout;
	for (unsigned int i = 0; i < sizeof(intakeCleanout); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 EvolWrapperInterlockSensors::getSerializedLength()const
{
	return (UINT32)serializedData.length();
}

void EvolWrapperInterlockSensors::show()
{
	if (Debug::isTracing("EvolWrapperInterlockSensors"))
	{
		HobString msgDebug = "stop:           ";
		msgDebug.append(Bool::toString(stop));
		Debug::trace("EvolWrapperInterlockSensors", msgDebug);

		msgDebug = "rightCanopy:    ";
		msgDebug.append(Bool::toString(rightCanopy));
		Debug::trace("EvolWrapperInterlockSensors", msgDebug);

		msgDebug = "rightSideCover: ";
		msgDebug.append(Bool::toString(rightSideCover));
		Debug::trace("EvolWrapperInterlockSensors", msgDebug);

		msgDebug = "filmSupplyDoor: ";
		msgDebug.append(Bool::toString(filmSupplyDoor));
		Debug::trace("EvolWrapperInterlockSensors", msgDebug);

		msgDebug = "leftSideCover:  ";
		msgDebug.append(Bool::toString(leftSideCover));
		Debug::trace("EvolWrapperInterlockSensors", msgDebug);

		msgDebug = "intakeCleanout: ";
		msgDebug.append(Bool::toString(intakeCleanout));
		Debug::trace("EvolWrapperInterlockSensors", msgDebug);
	}
}
/*
UINT16 EvolWrapperInterlockSensors::getStatusBits()
{
	UINT16 statusBits;

	memset (&statusBits, 0, sizeof (statusBits));

	if (stop == true)
	{
		statusBits |= StopSwitchMask;
	}

	if (rightCanopy == true)
	{
		statusBits |= RightCanopyMask;
	}

	if (rightSideCover == true)
	{
		statusBits |= RightSideCoverMask;
	}

	if (filmSupplyDoor == true)
	{
		statusBits |= FilmSupplyDoorMask;
	}

	if (leftSideCover == true)
	{
		statusBits |= LeftSideCoverMask;
	}

	if (intakeCleanout == true)
	{
		statusBits |= IntakeCleanoutMask;
	}

	return statusBits;
}
*/

bool EvolWrapperInterlockSensors::isStopSwitch()const
{
	return stop;
}

void EvolWrapperInterlockSensors::setStopSwitch(bool value)
{
	stop = value;
}

bool EvolWrapperInterlockSensors::isRightCanopySwitch()const
{
	return rightCanopy;
}

void EvolWrapperInterlockSensors::setRightCanopySwitch(bool value)
{
	rightCanopy = value;
}

bool EvolWrapperInterlockSensors::isRightSideCover()const
{
	return rightSideCover;
}

void EvolWrapperInterlockSensors::setRightSideCover(bool value)
{
	rightSideCover = value;
}

bool EvolWrapperInterlockSensors::isFilmSupplyDoor()const
{
	return filmSupplyDoor;
}

void EvolWrapperInterlockSensors::setFilmSupplyDoor(bool value)
{
	filmSupplyDoor = value;
}

bool EvolWrapperInterlockSensors::isLeftSideCover()const
{
	return leftSideCover;
}

void EvolWrapperInterlockSensors::setLeftSideCover(bool value)
{
	leftSideCover = value;
}

bool EvolWrapperInterlockSensors::isIntakeCleanout()const
{
	return intakeCleanout;
}

void EvolWrapperInterlockSensors::setIntakeCleanout(bool value)
{
	intakeCleanout = value;
}

void EvolWrapperInterlockSensors::setStatus(UINT16 statusBits)
{
	stop =			(((statusBits & StopSwitchMask) == StopSwitchMask)  ? true : false);
	rightCanopy =	(((statusBits & RightCanopyMask) == RightCanopyMask)  ? true : false);
	rightSideCover =(((statusBits & RightSideCoverMask) == RightSideCoverMask)  ? true : false);
	filmSupplyDoor =(((statusBits & FilmSupplyDoorMask) == FilmSupplyDoorMask)  ? true : false);
	leftSideCover  =(((statusBits & LeftSideCoverMask) == LeftSideCoverMask)  ? true : false);
	intakeCleanout =(((statusBits & IntakeCleanoutMask) == IntakeCleanoutMask)  ? true : false);
}

bool EvolWrapperInterlockSensors::equals(const EvolWrapperInterlockSensors &wrSensors)const
{
	bool rv = false;

	if	((stop == wrSensors.isStopSwitch()) &&
		 (rightCanopy == wrSensors.isRightCanopySwitch()) &&
		 (rightSideCover == wrSensors.isRightSideCover()) &&
		 (filmSupplyDoor == wrSensors.isFilmSupplyDoor()) &&
		 (leftSideCover == wrSensors.isLeftSideCover()))
	{
		rv = true;
	}

	HobString m(Bool::toString(rv));
	Debug::trace("EvolWrapperInterlockSensors", "equals = " + m);
	return rv;
}

void EvolWrapperInterlockSensors::operator=(const EvolWrapperInterlockSensors &sensors)
{
	stop = sensors.isStopSwitch();
	rightCanopy = sensors.isRightCanopySwitch();
	rightSideCover = sensors.isRightSideCover();
	filmSupplyDoor = sensors.isFilmSupplyDoor();
	leftSideCover = sensors.isLeftSideCover();
	intakeCleanout = sensors.isIntakeCleanout();
}
