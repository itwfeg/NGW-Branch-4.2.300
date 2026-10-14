#include "WrapperEvent.h"
#include "EvolWrapperTest.h"

#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperTest::_dataType_ = 0;
std::string EvolWrapperTest::_dataTypeName_("EvolWrapperTest"); 
#endif


EvolWrapperTest::EvolWrapperTest(Hobart::CEvolMsgReceiver *pHobThread, WrEvolTestCode wrTestCode)
	: EvolMessage(Wrapper, WrEvolTest, pHobThread), testCode(NoServiceTest), data1(0), data2(0), dataLong(0), useIntParams(true)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	testCode = wrTestCode;
}

EvolWrapperTest::EvolWrapperTest(const char *buffer)
	: EvolMessage(buffer), testCode(NoServiceTest), data1(0), data2(0), dataLong(0), useIntParams(true)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &testCode;
	for (unsigned int i = 0; i < sizeof(testCode); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &data1;
	for (unsigned int i = 0; i < sizeof(data1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &data2;
	for (unsigned int i = 0; i < sizeof(data2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &dataLong;
	for (unsigned int i = 0; i < sizeof(dataLong); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &useIntParams;
	for (unsigned int i = 0; i < sizeof(useIntParams); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperTest::~EvolWrapperTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperTest::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &testCode;
	for (unsigned int i = 0; i < sizeof(testCode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &data1;
	for (unsigned int i = 0; i < sizeof(data1); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &data2;
	for (unsigned int i = 0; i < sizeof(data2); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &dataLong;
	for (unsigned int i = 0; i < sizeof(dataLong); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &useIntParams;
	for (unsigned int i = 0; i < sizeof(useIntParams); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperTest::show()
{
	if (Debug::isTracing("EvolWrapperTest"))
	{
		HobString msgDebug = "Test: ";

		EvolMessage::show();

		HobString testName = "";
		getTestName(testCode, testName);
		msgDebug.append(testName);
		
		msgDebug.append("  DataInt1: ").append(Integer::toString(data1));
		Debug::trace("EvolWrapperTest", msgDebug);
		
		msgDebug.append("  DataInt2: ").append(Integer::toString(data2));
		Debug::trace("EvolWrapperTest", msgDebug);
		
		msgDebug.append("  DataLong: ").append(Long::toString(dataLong));
		Debug::trace("EvolWrapperTest", msgDebug);
		
		msgDebug.append("  useIntParams: ").append(Bool::toString(useIntParams));
		Debug::trace("EvolWrapperTest", msgDebug);
	}
}


void EvolWrapperTest::getTestName(WrEvolTestCode test, HobString &result)
{
	result = "";
	switch (test)
	{
		case NoServiceTest:
			result = "NoServiceTest";
			break;
		case PrimaryIntakeTest:
			result = "PrimaryIntakeTest";
			break;
		case SecondaryIntakeTest:
			result = "SecondaryIntakeTest";
			break;
		case CameraTest:
			result = "CameraTest";
			break;
		case ElevatorTest:
			result = "ElevatorTest";
			break;
		case ElevatorWidePackageTest:
			result = "ElevatorWidePackageTest";
			break;
		case FilmSelectTest:
			result = "FilmSelectTest";
			break;
		case KnifeTest:
			result = "KnifeTest";
			break;
		case UpperFilmBrakeTest:
			result = "UpperFilmBrakeTest";
			break;
		case LowerFilmBrakeTest:
			result = "LowerFilmBrakeTest";
			break;
		case FilmSensorTest:
			result = "FilmSensorTest";
			break;
		case GripperNarrowTest:
			result = "GripperNarrowTest";
			break;
		case GripperWideTest:
			result = "GripperWideTest";
			break;
		case GripperTest:
			result = "GripperTest";
			break;
		case SideClampsTest:
			result = "SideClampsTest";
			break;
		case RearFolderTest:
			result = "RearFolderTest";
			break;
		case FrontFolderTest:
			result = "FrontFolderTest";
			break;
		case PusherTest:
			result = "PusherTest";
			break;
		case SealerTest:
			result = "SealerTest";
			break;
		case CycleIntakeTest:
			result = "CycleIntakeTest";
			break;
		case LabelApplierTest:
			result = "LabelApplierTest";
			break;
		case ShifterTest:
			result = "ShifterTest";
			break;
		case AirCompressorTest:
			result = "AirCompressorTest";
			break;
		case SideFolderTest:
			result = "SideFolderTest";
			break;
		case WrapperCycle:
			result = "WrapperCycleTest";
			break;
		case WrapperMechResetTest:
			result = "WrapperMechResetTest";
			break;
		case CalibrateWrapper:
			result = "CalibrateWrapperTest";
			break;
		case SecondLabelApplierTest:
			result = "SecondLabelApplierTest";
			break;
		case WrWrapperCleanTest:
			result = "WrWrapperCleanTest";
			break;
		default:
			result = "Unknown test";
			break;
	}
}

EvolWrapperTest::WrEvolTestCode EvolWrapperTest::getTestCode() const
{
	return testCode;
}

void EvolWrapperTest::setTestCode(WrEvolTestCode wrTestCode)
{
	testCode = wrTestCode;
}

INT16 EvolWrapperTest::getDataInt1() const
{
	return data1;
}

void EvolWrapperTest::setDataInt1(INT16 value)
{
	data1 = value;
}

INT16 EvolWrapperTest::getDataInt2() const
{
	return data2;
}

void EvolWrapperTest::setDataInt2(INT16 value)
{
	data2 = value;
}

UINT32 EvolWrapperTest::getDataLong() const
{
	return dataLong;
}

void EvolWrapperTest::setDataLong(UINT32 value)
{
	dataLong = value;
}

bool EvolWrapperTest::doUseIntParams() const
{
	return useIntParams;
}

void EvolWrapperTest::setUseIntParams()
{
	useIntParams = true;
}

void EvolWrapperTest::setUseLongParam()
{
	useIntParams = false;
}
