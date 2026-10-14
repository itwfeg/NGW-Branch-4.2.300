//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperTest.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperTest::dataType = 0;
std::string WrapperTest::dataTypeName("WrapperTest"); 
#endif

WrapperTest::WrapperTest(Hobart::CEvolMsgReceiver *pHobThread,
						 WrTestCode wrTestCode) : EvolMessage(Wrapper, WrTest, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->wrTestCode = wrTestCode;
}

WrapperTest::WrapperTest(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &wrTestCode;
	for (unsigned int i = 0; i < sizeof(wrTestCode); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperTest::~WrapperTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperTest::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &wrTestCode;
	for (unsigned int i = 0; i < sizeof(wrTestCode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperTest::show()
{
	Debug::addTraceCategory("WrapperTest");
	Debug::trace("WrapperTest", "WrapperTest: ");

	EvolMessage::show();

	if (wrTestCode == WideFilmWidthTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: WideFilmWidthTest");
	}
	else if (wrTestCode == NarrowFilmWidthTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: NarrowFilmWidthTest");
	}
	else if (wrTestCode == ShortFilmLengthTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: ShortFilmLengthTest");
	}
	else if (wrTestCode == MediumFilmLengthTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: MediumFilmLengthTest");
	}
	else if (wrTestCode == LongFilmLengthTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: LongFilmLengthTest");
	}
	else if (wrTestCode == KnifeOnTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: KnifeOnTest");
	}
	else if (wrTestCode == KnifeOffTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: KnifeOffTest");
	}
	else if (wrTestCode == GripperOnTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: GripperOnTest");
	}
	else if (wrTestCode == GripperOffTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: GripperOffTest");
	}
	else if (wrTestCode == LeftSideClampOnTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: LeftSideClampOnTest");
	}
	else if (wrTestCode == LeftSideClampOffTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: LeftSideClampOffTest");
	}
	else if (wrTestCode == RightSideClampOnTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: RightSideClampOnTest");
	}
	else if (wrTestCode == RightSideClampOffTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: RightSideClampOffTest");
	}
	else if (wrTestCode == CompressorOnTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: CompressorOnTest");
	}
	else if (wrTestCode == CompressorOffTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: CompressorOffTest");
	}
	else if (wrTestCode == ResetIntakeTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: ResetIntakeTest");
	}
	else if (wrTestCode == SlowIntakeTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: SlowIntakeTest");
	}
	else if (wrTestCode == QuickIntakeTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: QuickIntakeTest");
	}
	else if (wrTestCode == SpiRead)
	{
		Debug::trace("WrapperTest", "wrTestCode: SpiRead");
	}
	else if (wrTestCode == SpiWrite)
	{
		Debug::trace("WrapperTest", "wrTestCode: SpiWrite");
	}
	else if (wrTestCode == AToDRead)
	{
		Debug::trace("WrapperTest", "wrTestCode: AToDRead");
	}
	else if (wrTestCode == SlideBlockOnTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: SlideBlockOnTest");
	}
	else if (wrTestCode == SlideBlockOffTest)
	{
		Debug::trace("WrapperTest", "wrTestCode: SlideBlockOffTest");
	}
	else
	{
		Debug::trace("WrapperTest", HobString("wrTestCode: Unknown ") +
									HobString(Integer::toString((UINT16) wrTestCode)));
	}

	Debug::removeTraceCategory("WrapperTest");
}

void WrapperTest::setWrTestCode(WrTestCode wrTestCode)
{
	this->wrTestCode = wrTestCode;
}

WrapperTest::WrTestCode WrapperTest::getWrTestCode() const
{
	return wrTestCode;
}
