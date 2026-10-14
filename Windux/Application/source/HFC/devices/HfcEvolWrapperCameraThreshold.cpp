#include "HfcEvolWrapperCameraThreshold.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperCameraThreshold::_dataType_ = 0;
std::string HfcEvolWrapperCameraThreshold::_dataTypeName_("HfcEvolWrapperCameraThreshold"); 
#endif

HfcEvolWrapperCameraThreshold::HfcEvolWrapperCameraThreshold(const char *data) : EvolWrapperCameraThreshold(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperCameraThreshold", "Constructor()");
}

HfcEvolWrapperCameraThreshold::HfcEvolWrapperCameraThreshold(const char *data, int UNUSED(len), int UNUSED(formatType))
	: EvolWrapperCameraThreshold()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	char pChar = Integer::valueOf(data);
	setWrapperCameraThreshold(&pChar);
}
HfcEvolWrapperCameraThreshold::~HfcEvolWrapperCameraThreshold()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *HfcEvolWrapperCameraThreshold::serialize(int format)const
{
	const char *msgdata = NULL;
	int msglen = 0;

	HfcObject::serializedData = "";

	Debug::trace("HfcEvolWrapperCameraThreshold", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcEvolCameraThresholdCountFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetHfcEvolCameraThresholdCount:
					{
						HfcObject::serializedData.append(Integer::toString((int)getWrapperCameraThreshold()));
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}
		case InternalFormat:
		{
			msgdata = EvolWrapperCameraThreshold::serialize();
			msglen = EvolWrapperCameraThreshold::getSerializedLength();

			HfcObject::serializedData = EvolWrapperCameraThreshold::serializedData;
			msglen = HfcObject::getSerializedLength();
		
			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}
	Debug::trace("HfcEvolWrapperCameraThreshold", "serialize():  " + 	HfcObject::serializedData);
	return 	HfcObject::serializedData.c_str();
}


void HfcEvolWrapperCameraThreshold::show()
{
	if (Debug::isTracing("HfcEvolWrapperCameraThreshold"))
	{
		Debug::trace("HfcEvolWrapperCameraThreshold", "HfcEvolWrapperCameraThreshold: ");
		Debug::trace("HfcEvolWrapperCameraThreshold", HobString("threshold_: ") + HobString(Integer::toString((int)getWrapperCameraThreshold())));
	}
}
