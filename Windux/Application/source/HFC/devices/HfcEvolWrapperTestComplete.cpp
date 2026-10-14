/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperTestComplete.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperTestComplete::_dataType_ = 0;
std::string HfcEvolWrapperTestComplete::_dataTypeName_("HfcEvolWrapperTestComplete"); 
#endif

HfcEvolWrapperTestComplete::HfcEvolWrapperTestComplete(const char *data) : EvolWrapperTestComplete(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcEvolWrapperTestComplete::~HfcEvolWrapperTestComplete()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperTestComplete::serialize(int format)const
{
	bool traceOn = Debug::isTracing("HfcEvolWrapperTestComplete");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "serialize() ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" format = ").append(Integer::toString(format));
		Debug::trace("HfcEvolWrapperTestComplete", msgDebug2);
	}
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcEvolWrapperTestCompleteFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetEvolWrTestCompTest:
					{
						HobString temp = Integer::toString(getTestCode());
						HfcObject::serializedData.append(temp);
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append(" fieldOffsetEvolWrTestCompTest: ").append(temp);
							Debug::trace("HfcEvolWrapperTestComplete", msgDebug2);
						}
						break;
					}
					case fieldOffsetEvolWrTestCompSubTest:
					{
						HobString temp = Integer::toString(getData1());
						HfcObject::serializedData.append(temp);
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append(" fieldOffsetEvolWrTestCompSubTest: ").append(temp);
							Debug::trace("HfcEvolWrapperTestComplete", msgDebug2);
						}
						break;
					}
					case fieldOffsetEvolWrTestCompTestPassed:
					{
						HobString temp = Bool::toString(getTestPassed());
						HfcObject::serializedData.append(temp);
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append(" fieldOffsetEvolWrTestCompTestPassed: ").append(temp);
							Debug::trace("HfcEvolWrapperTestComplete", msgDebug2);
						}
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append(" - invalid format: ").append(Integer::toString(format));
				Debug::trace("HfcEvolWrapperTestComplete", msgDebug2);
			}
			// todo - throw exception("");
			break;
	}

	return HfcObject::serializedData.data();
}
