//#include "vxWorks.h"
#include "WrapperGripperTiming.h"
#include "WrapperTime.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperGripperTiming::dataType = 0;
std::string WrapperGripperTiming::dataTypeName("WrapperGripperTiming"); 
#endif

WrapperGripperTiming::WrapperGripperTiming(WrapperTime &setback, WrGripperPwm pwm)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->setback = WrapperTime(setback);
	this->pwm = pwm;
}

WrapperGripperTiming::WrapperGripperTiming(const WrapperTime *setback, WrGripperPwm pwm)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( setback != NULL ) 
		this->setback = WrapperTime(setback->getMin(), setback->getValue(), setback->getMax());
	else
		setback = NULL;
	this->pwm = pwm;
}

WrapperGripperTiming::WrapperGripperTiming(WrapperGripperTiming &gripperSetback)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->setback = WrapperTime(gripperSetback.setback.getMin(),
								gripperSetback.setback.getValue(),
								gripperSetback.setback.getMax());
	this->pwm = gripperSetback.pwm;
}

WrapperGripperTiming::WrapperGripperTiming(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	int j = 0;

	// fills in the setback!!
	setback = WrapperTime(&buffer[j]);

	setback.serialize();
	j += setback.getSerializedLength();

	// Now, for the pwm
	char *pChar = (char *) &pwm;
	for (unsigned int i = 0; i < sizeof(pwm); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperGripperTiming::~WrapperGripperTiming()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperGripperTiming::serialize() const
{
	serializedData = "";

	const char *pCC = setback.serialize();
	int mLength = setback.getSerializedLength();

	serializedData.append(pCC, mLength);

	char *pChar = (char *) &pwm;
	for (unsigned int i = 0; i < sizeof(pwm); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	return serializedData.data();
}

UINT32 WrapperGripperTiming::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void WrapperGripperTiming::show()
{
	Debug::addTraceCategory("WrapperGripperTiming");
	Debug::trace("WrapperGripperTiming", "WrapperGripperTiming");

	Debug::trace("WrapperGripperTiming", "setback: ");
	setback.show();

	if (pwm == Pwm100Gripper)
	{
		Debug::trace("WrapperGripperTiming", "pwm: Pwm100Gripper");
	}
	else if (pwm == Pwm074Gripper)
	{
		Debug::trace("WrapperGripperTiming", "pwm: Pwm074Gripper");
	}
	else if (pwm == Pwm055Gripper)
	{
		Debug::trace("WrapperGripperTiming", "pwm: Pwm055Gripper");
	}
	else if (pwm == Pwm038Gripper)
	{
		Debug::trace("WrapperGripperTiming", "pwm: Pwm038Gripper");
	}
	else if (pwm == IgnoreGripperPwm)
	{
		Debug::trace("WrapperGripperTiming", "pwm: IgnoreGripperPwm");
	}
	else
	{
		Debug::trace("WrapperGripperTiming", HobString("pwm: Unknown ") +
											 HobString(Integer::toString(pwm)));
	}

	Debug::removeTraceCategory("WrapperGripperTiming");
}

WrapperTime &WrapperGripperTiming::getSetback()
{
	return setback;
}

void WrapperGripperTiming::setSetback(WrapperTime &setback)
{
	this->setback = WrapperTime(setback);
}

WrGripperPwm WrapperGripperTiming::getPwm()
{
	return pwm;
}

void WrapperGripperTiming::setPwm(WrGripperPwm pwm)
{
	this->pwm = pwm;
}

void WrapperGripperTiming::setPwm100()
{
	pwm = Pwm100Gripper;
}

bool WrapperGripperTiming::isPwm100()
{
	return (pwm == Pwm100Gripper);
}

void WrapperGripperTiming::setPwm074()
{
	pwm = Pwm074Gripper;
}

bool WrapperGripperTiming::isPwm074()
{
	return (pwm == Pwm074Gripper);
}

void WrapperGripperTiming::setPwm055()
{
	pwm = Pwm055Gripper;
}

bool WrapperGripperTiming::isPwm055()
{
	return (pwm == Pwm055Gripper);
}

void WrapperGripperTiming::setPwm038()
{
	pwm = Pwm038Gripper;
}

bool WrapperGripperTiming::isPwm038()
{
	return (pwm == Pwm038Gripper);
}

void WrapperGripperTiming::setIgnorePwm()
{
	pwm = IgnoreGripperPwm;
}

bool WrapperGripperTiming::isIgnorePwm()
{
	return (pwm == IgnoreGripperPwm);
}
