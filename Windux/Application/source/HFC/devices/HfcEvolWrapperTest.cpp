/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperTest.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperTest::_dataType_ = 0;
std::string HfcEvolWrapperTest::_dataTypeName_("HfcEvolWrapperTest"); 
#endif

HfcEvolWrapperTest::HfcEvolWrapperTest(Hobart::CEvolMsgReceiver *pHobThread, WrEvolTestCode wrTestCode) : EvolWrapperTest(pHobThread, wrTestCode)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcEvolWrapperTest::HfcEvolWrapperTest(const char *data) : EvolWrapperTest(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcEvolWrapperTest::HfcEvolWrapperTest(const char *data, int len, int formatType)	: EvolWrapperTest()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	bool traceOn = Debug::isTracing("HfcEvolWrapperTest");
	HobString msgDebug = "HfcEvolWrapperTest()";
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  formatType = ").append(Integer::toString(formatType));
		Debug::trace("HfcEvolWrapperTest", msgDebug2);
		msgDebug.append(":  ");
	}

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
			HobString gotWhat = "";
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;

			while (tok.next(&token) != -1)
			{
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  token = ").append(token);
					Debug::trace("HfcEvolWrapperTest", msgDebug2);
				}
				setReceived(loopCnt, (token.length() == 0) ? false : true);

				switch (loopCnt)
				{
					case fieldOffsetEvolWrTestTest:
					{
						setTestCode((WrEvolTestCode) Integer::valueOf(token.c_str()));
						if (traceOn)
						{
							gotWhat = "Test = ";
							gotWhat.append(token);
						}
						if (getTestCode() == WrapperCycle)		//	This test takes a PLU# as a parameter.
							setUseLongParam();
						break;
					}
					case fieldOffsetEvolWrTestData1:
					{
						if (doUseIntParams())
						{
							token.stripChar(DECIMAL_POINT);
							setDataInt1(Integer::valueOf(token.c_str()));
							if (traceOn)
							{
								gotWhat = "Data1 = ";
								gotWhat.append(token);
							}
						}
						else
						{
							setDataLong((INT32) Long::valueOf(token.c_str()));
							if (traceOn)
							{
								gotWhat = "DataLong = ";
								gotWhat.append(token);
							}
						}
						break;
					}
					case fieldOffsetEvolWrTestData2:
					{
						token.stripChar(DECIMAL_POINT);
						setDataInt2(Integer::valueOf(token.c_str()));
						if (traceOn)
						{
							gotWhat = "Data2 = ";
							gotWhat.append(token);
						}
						break;
					}
					default:
					{
						if (traceOn)
						{
							gotWhat = "Nothing valid: ";
							gotWhat.append(token);
						}
						break;
					}
				}
				if (traceOn)
				{
					Debug::trace("HfcEvolWrapperTest", msgDebug + gotWhat);
				}
				++loopCnt;
			}
		}
		break;


		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcEvolWrapperTest::~HfcEvolWrapperTest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperTest::serialize(int format)const
{
	switch (format)
	{
		case InternalFormat:
		{
			EvolWrapperTest::serialize();
			EvolWrapperTest::getSerializedLength();
			HfcObject::serializedData = EvolWrapperTest::serializedData;
		}
		break;

		default:
			if (Debug::isTracing("HfcEvolWrapperTest"))
			{
				HobString msgDebug = "serialize() - invalid format: ";
				msgDebug.append(Integer::toString(format));
				Debug::trace("HfcEvolWrapperTest", msgDebug);
			}
			// todo - throw exception("");
			break;
	}

	return HfcObject::serializedData.data();
}
