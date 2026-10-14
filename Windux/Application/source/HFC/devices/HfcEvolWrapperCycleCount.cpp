#include "HfcEvolWrapperCycleCount.h"
#include "HobString.h"
#include "Long.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperCycleCount::_dataType_ = 0;
std::string HfcEvolWrapperCycleCount::_dataTypeName_("HfcEvolWrapperCycleCount"); 
#endif

HfcEvolWrapperCycleCount::HfcEvolWrapperCycleCount(const char *data) : EvolWrapperCycleCount(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperCycleCount", "Constructor()");
}

HfcEvolWrapperCycleCount::~HfcEvolWrapperCycleCount()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}



const char *HfcEvolWrapperCycleCount::serialize(int format)const
{
	HfcObject::serializedData = "";

	Debug::trace("HfcEvolWrapperCycleCount", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcEvolCycleCountFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetHfcEvolCycleCount:
					{
						HfcObject::serializedData.append(Long::toString(getWrapperCycleCount()));
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
			// todo - throw exception("invalid format");
			break;
	}
	Debug::trace("HfcEvolWrapperCycleCount", "serialize():  " + 	HfcObject::serializedData);
	return 	HfcObject::serializedData.c_str();
}


void HfcEvolWrapperCycleCount::show()
{
	if (Debug::isTracing("HfcEvolWrapperCycleCount"))
	{
		Debug::trace("HfcEvolWrapperCycleCount", "HfcEvolWrapperCycleCount: ");
		Debug::trace("HfcEvolWrapperCycleCount", HobString("wrapperCycleCount: ") + HobString(Long::toString(getWrapperCycleCount())));
	}
}
