//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperIntakeMode.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperIntakeMode::dataType = 0;
std::string WrapperIntakeMode::dataTypeName("WrapperIntakeMode"); 
#endif

WrapperIntakeMode::WrapperIntakeMode(Hobart::CEvolMsgReceiver	*pHobThread,
									 IntakeMode intakeMode) : EvolMessage(Wrapper, WrIntakeMode, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->intakeMode = intakeMode;
}

WrapperIntakeMode::WrapperIntakeMode(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperIntakeMode::~WrapperIntakeMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *WrapperIntakeMode::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperIntakeMode::show()
{
	Debug::addTraceCategory("WrapperIntakeMode");
	Debug::trace("WrapperIntakeMode", "WrapperIntakeMode: ");

	EvolMessage::show();

	if (intakeMode == DefaultIntake)
	{
		Debug::trace("WrapperIntakeMode", "intakeMode: DefaultIntake");
	}
	else if (intakeMode == GentleIntake)
	{
		Debug::trace("WrapperIntakeMode", "intakeMode: GentleIntake");
	}
	else if (intakeMode == NormalIntake)
	{
		Debug::trace("WrapperIntakeMode", "intakeMode: NormalIntake");
	}
	else if (intakeMode == FastIntake)
	{
		Debug::trace("WrapperIntakeMode", "intakeMode: FastIntake");
	}
	else
	{
		Debug::trace("WrapperIntakeMode", HobString("intakeMode: Unknown ") +
										  HobString(Integer::toString((UINT16) intakeMode)));
	}

	Debug::removeTraceCategory("WrapperIntakeMode");
}

bool WrapperIntakeMode::equals(const WrapperIntakeMode &intakeMode) const
{
	bool rv = false;

	if (this->intakeMode == intakeMode.getIntakeMode())
	{
		rv = true;
	}

	return rv;
}

WrapperIntakeMode::IntakeMode WrapperIntakeMode::getIntakeMode() const
{
	return intakeMode;
}

void WrapperIntakeMode::setIntakeMode(IntakeMode intakeMode)
{
	this->intakeMode = intakeMode;
}

WrapperIntakeMode::UltimaIntakeMode WrapperIntakeMode::getUltimaIntakeMode() const
{
	 UltimaIntakeMode rv = UltimaNormalIntake;

	if (intakeMode == GentleIntake)
	{
		rv = UltimaGentleIntake;
	}

	return rv;
}
