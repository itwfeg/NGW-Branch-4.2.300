//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierTest.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierTest::dataType = 0;
std::string LabelApplierTest::dataTypeName("LabelApplierTest"); 
#endif

LabelApplierTest::LabelApplierTest(Hobart::CEvolMsgReceiver *pHobThread,
								   LaStationID stationId,
								   LaTestID testId,
								   UINT16 parm1,
								   UINT16 parm2) : EvolMessage(LabelApplier, LaTest, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->stationId = stationId;
	this->testId = testId;
	this->result = 0;
	this->parm1 = parm1;
	this->parm2 = parm2;
}

LabelApplierTest::LabelApplierTest(const char *buffer)	: EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &stationId;
	for (unsigned int i = 0; i < sizeof(stationId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &testId;
	for (unsigned int i = 0; i < sizeof(testId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &result;
	for (unsigned int i = 0; i < sizeof(result); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &parm1;
	for (unsigned int i = 0; i < sizeof(parm1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &parm2;
	for (unsigned int i = 0; i < sizeof(parm2); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierTest::~LabelApplierTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
 
const char *LabelApplierTest::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &stationId;
	for (unsigned int i = 0; i < sizeof(stationId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &testId;
	for (unsigned int i = 0; i < sizeof(testId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &result;
	for (unsigned int i = 0; i < sizeof(result); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &parm1;
	for (unsigned int i = 0; i < sizeof(parm1); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &parm2;
	for (unsigned int i = 0; i < sizeof(parm2); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierTest::show()
{
	bool isTracing = Debug::isTracing("LabelApplierTest");

	if (isTracing == false)
	{
		Debug::addTraceCategory( std::string("LabelApplierTest") );
	}

	Debug::trace("LabelApplierTest", "LabelApplierTest: ");

	EvolMessage::show();

	if (stationId == WeigherStation)
	{
		Debug::trace("LabelApplierTest", "transferStationType: WeigherStation");
	}
	else if (stationId == PrimaryPrinterStation)
	{
		Debug::trace("LabelApplierTest", "transferStationType: PrimaryPrinterStation");
	}
	else if (stationId == SecondaryPrinterStation)
	{
		Debug::trace("LabelApplierTest", "transferStationType: SecondaryPrinterStation");
	}
	else
	{
		Debug::trace("LabelApplierTest", HobString("stationId: Unknown ") +
										 HobString(Integer::toString((UINT16) stationId)));
	}

	if (testId == EnablePrinterMotor)
	{
		Debug::trace("LabelApplierTest", "testId: EnablePrinterMotor");
	}
	else if (testId == NoService)
	{
		Debug::trace("LabelApplierTest", "testId: NoService");
	}
	else if (testId == EnableWeigherMotor)
	{
		Debug::trace("LabelApplierTest", "testId: EnableWeigherMotor");
	}
/*	else if (testId == DisablePrinterMotor)
	{
		Debug::trace("LabelApplierTest", "testId: DisablePrinterMotor");
	}
	else if (testId == DisableWeigherMotor)
	{
		Debug::trace("LabelApplierTest", "testId: DisableWeigherMotor");
	}
	else if (testId == BrakeWeigherMotor)
	{
		Debug::trace("LabelApplierTest", "testId: BrakeWeigherMotor");
	}
	else if (testId == BrakePrinterMotor)
	{
		Debug::trace("LabelApplierTest", "testId: BrakePrinterMotor");
	}
	else if (testId == ReleaseWeigherMotor)
	{
		Debug::trace("LabelApplierTest", "testId: ReleaseWeigherMotor");
	}
	else if (testId == ReleasePrinterMotor)
	{
		Debug::trace("LabelApplierTest", "testId: ReleasePrinterMotor");
	}
	else if (testId == TorqueToWeigher)
	{
		Debug::trace("LabelApplierTest", "testId: TorqueToWeigher");
	}
	else if (testId == TorqueToPrinter)
	{
		Debug::trace("LabelApplierTest", "testId: TorqueToPrinter");
	}
*/	else if (testId == EnableCompressor)
	{
		Debug::trace("LabelApplierTest", "testId: EnableCompressor");
	}
	else if (testId == DisableCompressor)
	{
		Debug::trace("LabelApplierTest", "testId: DisableCompressor");
	}
	else if (testId == GateUp)
	{
		Debug::trace("LabelApplierTest", "testId: GateUp");
	}
	else if (testId == GateDown)
	{
		Debug::trace("LabelApplierTest", "testId: GateDown");
	}
	else if (testId == ActivateBelts)
	{
		Debug::trace("LabelApplierTest", "testId: ActivateBelts");
	}
	else if (testId == ActivateRollers)
	{
		Debug::trace("LabelApplierTest", "testId: ActivateRollers");
	}
	else if (testId == ActivateWand)
	{
		Debug::trace("LabelApplierTest", "testId: ActivateWand");
	}
	else if (testId == ReleaseWand)
	{
		Debug::trace("LabelApplierTest", "testId: ReleaseWand");
	}
	else if (testId == ActivateApplier)
	{
		Debug::trace("LabelApplierTest", "testId: ActivateApplier");
	}
	else if (testId == ReleaseApplier)
	{
		Debug::trace("LabelApplierTest", "testId: ReleaseApplier");
	}
/*	else if (testId == RotateWand)
	{
		Debug::trace("LabelApplierTest", "testId: RotateWand");
	}
	else if (testId == EnableDriveSignals)
	{
		Debug::trace("LabelApplierTest", "testId: EnableDriveSignals");
	}
	else if (testId == DisableDriveSignals)
	{
		Debug::trace("LabelApplierTest", "testId: DisableDriveSignals");
	}
	else if (testId == SlaveInput)
	{
		Debug::trace("LabelApplierTest", "testId: SlaveInput");
	}
	else if (testId == SlaveOutput)
	{
		Debug::trace("LabelApplierTest", "testId: SlaveOutput");
	}
*/	else if (testId == EnableWandVacuum)
	{
		Debug::trace("LabelApplierTest", "testId: EnableWandVacuum");
	}
	else if (testId == DisableWandVaccum)
	{
		Debug::trace("LabelApplierTest", "testId: DisableWandVaccum");
	}
	else if (testId == ResetMechs)
	{
		Debug::trace("LabelApplierTest", "testId: ResetMechs");
	}
	else if (testId == EnableRunIn)
	{
		Debug::trace("LabelApplierTest", "testId: EnableRunIn");
	}
	else
	{
		Debug::trace("LabelApplierTest", HobString("testId: Unknown ") +
										 HobString(Integer::toString((UINT16) testId)));
	}

	Debug::trace("LabelApplierTest", HobString("result: ") + HobString(Integer::toString(result)));
	Debug::trace("LabelApplierTest", HobString("parm1: ") + HobString(Integer::toString(parm1)));
	Debug::trace("LabelApplierTest", HobString("parm2: ") + HobString(Integer::toString(parm2)));

	if (isTracing == false)
	{
		Debug::removeTraceCategory( std::string("LabelApplierTest") );
	}
}

void LabelApplierTest::setStationId(LaStationID stationId)
{
	this->stationId = stationId;
}

LaStationID LabelApplierTest::getStationId()
{
	return stationId;
}

void LabelApplierTest::setTestId(LaTestID testId)
{
	this->testId = testId;
}

LaTestID LabelApplierTest::getTestId()
{
	return testId;
}

void LabelApplierTest::setResult(UINT16 result)
{
	this->result = result;
}

UINT16 LabelApplierTest::getResult()
{
	return result;
}

void LabelApplierTest::setParm1(UINT16 parm1)
{
	this->parm1 = parm1;
}

UINT16 LabelApplierTest::getParm1()
{
	return parm1;
}

void LabelApplierTest::setParm2(UINT16 parm2)
{
	this->parm2 = parm2;
}

UINT16 LabelApplierTest::getParm2()
{
	return parm2;
}
