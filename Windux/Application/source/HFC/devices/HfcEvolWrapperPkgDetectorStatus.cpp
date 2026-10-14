#include "HfcEvolWrapperPkgDetectorStatus.h"
#include "HobString.h"
#include "Bool.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperPkgDetectorStatus::_dataType_ = 0;
std::string HfcEvolWrapperPkgDetectorStatus::_dataTypeName_("HfcEvolWrapperPkgDetectorStatus"); 
#endif

HfcEvolWrapperPkgDetectorStatus::HfcEvolWrapperPkgDetectorStatus(const char *data) : EvolWrapperPkgDetectorStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperPkgDetectorStatus", "Constructor()");
}

HfcEvolWrapperPkgDetectorStatus::~HfcEvolWrapperPkgDetectorStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}



const char *HfcEvolWrapperPkgDetectorStatus::serialize(int format)const
{
	HfcObject::serializedData = "";

	Debug::trace("HfcEvolWrapperPkgDetectorStatus", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcEvolPkgDetectorStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetEvolSensorLocation:
					{
						HfcObject::serializedData.append(Integer::toString(getSensorLocation()));
						break;
					}
					case fieldOffsetEvolDetectorType:
					{
						HfcObject::serializedData.append(Integer::toString(getDetectorType()));						
						break;
					}
					case fieldOffsetEvolEmitterPowerLevel:
					{
						HfcObject::serializedData.append(Integer::toString(getEmitterPower()));						
						break;
					}
					case fieldOffsetEvolSensorOperational:
					{
						HfcObject::serializedData.append(Bool::toString(getSensorOperational()));						
						break;
					}
					case fieldOffsetEvolIsOldDetector:
					{
						HfcObject::serializedData.append(Bool::toString(getIsOldDetector()));						
						break;
					}
					case fieldOffsetEvolDetectorDescription:
					{
						HfcObject::serializedData.append(getDetectorDescription());
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
	Debug::trace("HfcEvolWrapperPkgDetectorStatus", "serialize():  " + 	HfcObject::serializedData);
	return 	HfcObject::serializedData.c_str();
}


void HfcEvolWrapperPkgDetectorStatus::show()
{
	if (Debug::isTracing("HfcEvolWrapperPkgDetectorStatus"))
	{
		Debug::trace("HfcEvolWrapperPkgDetectorStatus", "HfcEvolWrapperPkgDetectorStatus: ");
		Debug::trace("HfcEvolWrapperPkgDetectorStatus", HobString("sensorLocation: ") + HobString(Integer::toString(getSensorLocation())));
		Debug::trace("HfcEvolWrapperPkgDetectorStatus", HobString("detectorType: ") + HobString(Integer::toString(getDetectorType())));
		Debug::trace("HfcEvolWrapperPkgDetectorStatus", HobString("emitterPowerLevel: ") + HobString(Integer::toString(getEmitterPower())));
		Debug::trace("HfcEvolWrapperPkgDetectorStatus", HobString("sensorOperational: ") + HobString(Bool::toString(getSensorOperational())));
		Debug::trace("HfcEvolWrapperPkgDetectorStatus", HobString("isOldDetector: ") + HobString(Bool::toString(getIsOldDetector())));
		Debug::trace("HfcEvolWrapperPkgDetectorStatus", HobString("detectorDescription: ") + getDetectorDescription());
	}
}
