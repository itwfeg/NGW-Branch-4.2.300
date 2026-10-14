#include "HfcEvolWrapperMotorProfile.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperMotorProfile::_dataType_ = 0;
std::string HfcEvolWrapperMotorProfile::_dataTypeName_("HfcEvolWrapperMotorProfile"); 
#endif


HfcEvolWrapperMotorProfile::HfcEvolWrapperMotorProfile(const char *data) : EvolWrapperMotorProfile(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


HfcEvolWrapperMotorProfile::~HfcEvolWrapperMotorProfile()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperMotorProfile::serialize(int format)const
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

			while (loopCnt < fieldOffsetMotorProfileEnd)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetMotorProfileMechanism:
					{
						HfcObject::serializedData.append(Integer::toString(getBldcMechanism()));
						break;
					}					
					case fieldOffsetMotorProfileStatus:
					{
						HfcObject::serializedData.append(Integer::toString(getBldcMotorStatus()));
						break;
					}
					case fieldOffsetMotorProfileCapacity:
					{
						HfcObject::serializedData.append(Integer::toString(getBldcMotorCapacityUsage()));
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
