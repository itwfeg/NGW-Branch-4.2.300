#include "HfcCLATestComplete.h"
#include "HobString.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcCLATestComplete::_dataType_ = 0;
std::string HfcCLATestComplete::_dataTypeName_("HfcCLATestComplete"); 
#endif


HfcCLATestComplete::HfcCLATestComplete(const char *data) : CLATestComplete(data)
{
	Debug::trace("HfcCLATestComplete", "HfcCLATestComplete(const char *data)");
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


HfcCLATestComplete::~HfcCLATestComplete()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcCLATestComplete::serialize(int format)const
{
	HfcObject::serializedData = "";
	HobString result;
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;
			Debug::trace("HfcCLATestComplete", "serialize(BrowserFormat)");
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < fieldOffsetCLATestCompleteOffsetsEnd)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					/*	EEP93C56Board	*/
					case fieldOffsetTest:
					{
						HfcObject::serializedData.append(Integer::toString(getTest()));
						Debug::trace("HfcCLATestComplete", "Test: " + HobString(Integer::toString(getTest())));
						break;
					}
					case fieldOffsetData1:
					{
						HfcObject::serializedData.append(Integer::toString(getData1()));
						Debug::trace("HfcCLATestComplete", "Data1: " + HobString(Integer::toString(getData1())));
						break;
					}
					case fieldOffsetData2:
					{
						HfcObject::serializedData.append(Integer::toString(getData2()));
						Debug::trace("HfcCLATestComplete", "Data2: " + HobString(Integer::toString(getData2())));
						break;
					}
					case fieldOffsetResult:
					{
						HfcObject::serializedData.append(HobString(Bool::toString(getResult())));
						Debug::trace("HfcCLATestComplete", "Result: " + HobString(Bool::toString(getResult())));
						break;
					}
				}
				loopCnt++;
			}
			break;
		}
		default:
			// to do - throw exception("invalid format");
			break;
	}
	return 	HfcObject::serializedData.c_str();
}