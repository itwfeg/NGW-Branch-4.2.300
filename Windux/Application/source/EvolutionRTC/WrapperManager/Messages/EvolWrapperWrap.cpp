#include "MetSig.h"
#include "WrapperEvent.h"
#include "EvolWrapperWrap.h"
#include "Rounding.h"

#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperWrap::dataType = 0;
std::string EvolWrapperWrap::dataTypeName("EvolWrapperWrap"); 
#endif

EvolWrapperWrap::EvolWrapperWrap(Hobart::CEvolMsgReceiver	*pHobThread,
								 WrapperIntakeMode::IntakeMode intakeMode,
								 UINT16 packageId,
								 bool pullFilm,
								 UINT16 packageWeight,
								 UINT32	pluNumber) : EvolMessage(Wrapper, WrEvolWrap, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->intakeMode = intakeMode;
	this->packageId = packageId;
	this->pullFilm = pullFilm;

	/*this needs to be done differently but for now.... ats
	this->packageWeight = packageWeight;
	*/
	/* For MetSig, the next line was the previous code (it just truncated the 3rd digit off of the weight, but it should round it to 2 implied decimal places, as was already done in AutoWeighEvolWrapperMode.cpp, so I'm doing that here as well - lap
	this->packageWeight = (packageWeight / 10);	//WeightPrecision 3, AWS wrapper check the weight and has a precision of 2 -- ats
	*/
	this->packageWeight = Rounding::roundWeightToTwoImpliedDecimalPlaces(packageWeight);

	this->pluNumber = pluNumber;
}

EvolWrapperWrap::EvolWrapperWrap(const char *buffer) : EvolMessage(buffer)
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

	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pullFilm;
	for (unsigned int i = 0; i < sizeof(pullFilm); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageWeight;
	for (unsigned int i = 0; i < sizeof(packageWeight); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pluNumber;
	for (unsigned int i = 0; i < sizeof(pluNumber); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperWrap::~EvolWrapperWrap()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *EvolWrapperWrap::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &intakeMode;
	for (unsigned int i = 0; i < sizeof(intakeMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pullFilm;
	for (unsigned int i = 0; i < sizeof(pullFilm); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageWeight;
	for (unsigned int i = 0; i < sizeof(packageWeight); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pluNumber;
	for (unsigned int i = 0; i < sizeof(pluNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperWrap::show()
{
	bool isTracing = Debug::isTracing("EvolWrapperWrap");

	if (isTracing == false)
	{
		Debug::addTraceCategory("EvolWrapperWrap");
	}

	Debug::trace("EvolWrapperWrap", "EvolWrapperWrap: ");

	EvolMessage::show();

	if (intakeMode == WrapperIntakeMode::DefaultIntake)
	{
		Debug::trace("EvolWrapperWrap", "intakeMode: DefaultIntake");
	}
	else if (intakeMode == WrapperIntakeMode::GentleIntake)
	{
		Debug::trace("EvolWrapperWrap", "intakeMode: GentleIntake");
	}
	else if (intakeMode == WrapperIntakeMode::NormalIntake)
	{
		Debug::trace("EvolWrapperWrap", "intakeMode: NormalIntake");
	}
	else if (intakeMode == WrapperIntakeMode::FastIntake)
	{
		Debug::trace("EvolWrapperWrap", "intakeMode: FastIntake");
	}
	else
	{
		Debug::trace("EvolWrapperWrap", HobString("intakeMode: Unknown ") +
										HobString(Integer::toString((UINT16) intakeMode)));
	}

	Debug::trace("EvolWrapperWrap", HobString("packageId: ") + HobString(Integer::toString(packageId)));
	Debug::trace("EvolWrapperWrap", HobString("pullFilm: ") + HobString(Bool::toString(pullFilm)));
	Debug::trace("EvolWrapperWrap", HobString("packageWeight: ") + HobString(Integer::toString(packageWeight)));
	Debug::trace("EvolWrapperWrap", HobString("pluNumber: ") + HobString(Long::toString(pluNumber)));

	if (isTracing == false)
	{
		Debug::removeTraceCategory("EvolWrapperWrap");
	}
}

WrapperIntakeMode::IntakeMode EvolWrapperWrap::getIntakeMode() const
{
	return intakeMode;
}

void EvolWrapperWrap::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	this->intakeMode = intakeMode;
}

UINT16 EvolWrapperWrap::getPackageId() const
{
	return packageId;
}

void EvolWrapperWrap::setPackageId(UINT16 packageId)
{
	this->packageId = packageId;
}

bool EvolWrapperWrap::isPullFilm() const
{
	return pullFilm;
}

void EvolWrapperWrap::setPullFilm(bool value)
{
	pullFilm = value;
}

void EvolWrapperWrap::clearPullFilm()
{
	pullFilm = false;
}

UINT16 EvolWrapperWrap::getPackageWeight() const
{
	return packageWeight;
}

void EvolWrapperWrap::setPackageWeight(UINT16 packageWeight)
{
	this->packageWeight = packageWeight;
}

UINT32 EvolWrapperWrap::getPluNumber() const
{
	return pluNumber;
}

void EvolWrapperWrap::setPluNumber(UINT32 pluNumber)
{
	this->pluNumber = pluNumber;
}
