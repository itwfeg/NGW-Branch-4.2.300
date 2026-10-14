/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperStatus.h"
#include "HobTokenizerCSV.h"
#include "Session.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperStatus::dataType = 0;
std::string HfcWrapperStatus::dataTypeName("HfcWrapperStatus"); 
#endif

HfcWrapperStatus::HfcWrapperStatus(const char *data) : WrapperStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	invalidPackSize = false;
}

HfcWrapperStatus::~HfcWrapperStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperStatus::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWrapperStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetWrStFilmWidth:
						HfcObject::serializedData.append(Integer::toString(getStatusInfo().getPackWidth()));
						break;
					case fieldOffsetWrStFilmTension:
						HfcObject::serializedData.append(Integer::toString(getStatusInfo().getFilmTension()));
						break;
					case fieldOffsetWrStBeltOnOff:
					{
						HobString temp = Bool::toString(getHeaterInfo().isState());
						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetWrStBeltTempSetting:
					{
						HobString temp = Integer::toString(getHeaterInfo().getSetting());
						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetWrStBeltTemperature:
					{
						HobString temp = Integer::toString(getHeaterInfo().getTemperature());
						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetWrStRate:
					{
						HobString temp = Integer::toString(getStatusInfo().getRate());
						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetWrStMainMotor:
					{
						HobString temp = Bool::toString(getStatusInfo().isMainMotor());
						HfcObject::serializedData.append(temp);
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
	return HfcObject::serializedData.data();
}

bool HfcWrapperStatus::getInvalidPackSize()
{
	return invalidPackSize;
}

bool HfcWrapperStatus::isInvalidPackSize()
{
	return invalidPackSize;
}

void HfcWrapperStatus::setInvalidPackSize(bool value)
{
	invalidPackSize = value;
}
