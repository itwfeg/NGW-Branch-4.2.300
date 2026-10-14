#include "HfcEvolWrapperEventCapture.h"
#include "HobTokenizerCSV.h"
#include "HobString.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperEventCapture::_dataType_ = 0;
std::string HfcEvolWrapperEventCapture::_dataTypeName_("HfcEvolWrapperEventCapture");
#endif


HfcEvolWrapperEventCapture::HfcEvolWrapperEventCapture(const char *data) : EvolWrapperEventCapture(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


HfcEvolWrapperEventCapture::~HfcEvolWrapperEventCapture()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperEventCapture::serialize(int format)const
{
	HfcObject::serializedData = "";
	HobString result;
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < fieldOffsetEvolWrapperEventCaptureEnd)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetCaptureType:
					{
						HfcObject::serializedData.append(Integer::toString(getEventCaptureType()));
						break;
					}					
					case fieldOffsetPackageId:
					{
						HfcObject::serializedData.append(Integer::toString(getPackageId()));
						break;
					}
					case fieldOffsetValue:
					{
						HfcObject::serializedData.append(Long::toString(getValue()));
						break;
					}
					case fieldOffsetErrorNumber:
					{
						HfcObject::serializedData.append(Bool::toString(getErrorNumber()));
						break;
					}
					case fieldOffsetEventMonth:
					{
						HfcObject::serializedData.append(Integer::toString(getEventMonth()));
						break;
					}
					case fieldOffsetEventDay:
					{
						HfcObject::serializedData.append(Integer::toString(getEventDay()));
						break;
					}
					case fieldOffsetEventYear:
					{
						HfcObject::serializedData.append(Integer::toString(getEventYear()));
						break;
					}
					case fieldOffsetEventHour:
					{
						HfcObject::serializedData.append(Integer::toString(getEventHour()));
						break;
					}
					case fieldOffsetEventMinutes:
					{
						HfcObject::serializedData.append(Integer::toString(getEventMinutes()));
						break;
					}
					case fieldOffsetEventSeconds:
					{
						HfcObject::serializedData.append(Integer::toString(getEventSeconds()));
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
