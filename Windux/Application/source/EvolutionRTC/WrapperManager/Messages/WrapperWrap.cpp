//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperWrap.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperWrap::dataType = 0;
std::string WrapperWrap::dataTypeName("WrapperWrap"); 
#endif

WrapperWrap::WrapperWrap(Hobart::CEvolMsgReceiver	*pHobThread,
						 WrapperIntakeMode::IntakeMode intakeMode,
						 UINT8 packageId,
						 bool pullFilm,
						 UINT16 packageWeight,
						 WrPackCode packCode) : WrapperCycle(pHobThread, packageId, pullFilm, packCode)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->intakeMode = intakeMode;
	this->packageWeight = packageWeight;

	setEventType(WrWrap);
}

WrapperWrap::WrapperWrap(const char *buffer) : WrapperCycle(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	WrapperCycle::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageWeight;
	for (unsigned int i = 0; i < sizeof(packageWeight); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperWrap::~WrapperWrap()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
 
const char *WrapperWrap::serialize() const
{
	WrapperCycle::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageWeight;
	for (unsigned int i = 0; i < sizeof(packageWeight); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperWrap::show()
{
	bool isTracing = Debug::isTracing("WrapperWrap");

	if (isTracing == false)
	{
		Debug::addTraceCategory("WrapperWrap");
	}

	Debug::trace("WrapperWrap", "WrapperWrap: ");

	WrapperCycle::show();

	Debug::trace("WrapperWrap", HobString("packageWeight") + HobString(Integer::toString(packageWeight)));

	if (intakeMode == WrapperIntakeMode::DefaultIntake)
	{
		Debug::trace("WrapperWrap", "intakeMode: DefaultIntake");
	}
	else if (intakeMode == WrapperIntakeMode::GentleIntake)
	{
		Debug::trace("WrapperWrap", "intakeMode: GentleIntake");
	}
	else if (intakeMode == WrapperIntakeMode::NormalIntake)
	{
		Debug::trace("WrapperWrap", "intakeMode: NormalIntake");
	}
	else if (intakeMode == WrapperIntakeMode::FastIntake)
	{
		Debug::trace("WrapperWrap", "intakeMode: FastIntake");
	}
	else
	{
		Debug::trace("WrapperWrap", HobString("intakeMode: Unknown ") +
									HobString(Integer::toString((UINT16) intakeMode)));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory("WrapperWrap");
	}
}

UINT16 WrapperWrap::getPackageWeight()
{
	return packageWeight;
}

void WrapperWrap::setPackageWeight(UINT16 packageWeight)
{
	this->packageWeight = packageWeight;
}
WrapperIntakeMode::IntakeMode WrapperWrap::getIntakeMode()
{
	return intakeMode;
}

void WrapperWrap::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	this->intakeMode = intakeMode;
}

WrapperIntakeMode::UltimaIntakeMode WrapperWrap::getUltimaIntakeMode()
{
	 WrapperIntakeMode::UltimaIntakeMode rv = WrapperIntakeMode::UltimaNormalIntake;

	if (intakeMode == WrapperIntakeMode::GentleIntake)
	{
		rv = WrapperIntakeMode::UltimaGentleIntake;
	}

	return rv;
}
