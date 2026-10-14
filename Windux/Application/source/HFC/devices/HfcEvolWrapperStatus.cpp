/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperStatus.h"

#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperStatus::dataType = 0;
std::string HfcEvolWrapperStatus::dataTypeName("HfcEvolWrapperStatus"); 
#endif

HfcEvolWrapperStatus::HfcEvolWrapperStatus() : EvolWrapperStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcEvolWrapperStatus::HfcEvolWrapperStatus(const char *data) : EvolWrapperStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcEvolWrapperStatus::~HfcEvolWrapperStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcEvolWrapperStatus::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcEvolWrapperStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetEvolWrSealerBeltTemperature:
					{
						HobString temp = Integer::toString(getTemperatureInfo().getSealerBeltTemp());
						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetEvolWrFilmSupplyTemperature:
					{
						HobString temp = Integer::toString(getTemperatureInfo().getFilmTemp());
						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetEvolWrAmbientTemperature:
					{
						HobString temp = Integer::toString(getTemperatureInfo().getAmbientTemp());
						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetEvolWrSealerBeltPower:
					{
						HobString temp = Bool::toString(getTemperatureInfo().getSealerBeltHeaterEnabled());
						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetEvolWrMainPower:
					{
						HobString temp( "0" );

						if (( isPeripheralComm() ) && ( !isManagerDeleted() ))
							if ( getStatusInfo().isMainPower() )
								temp = "1";

						HfcObject::serializedData.append(temp);
						break;
					}
					case fieldOffsetEvolWrBuildingPressure:
					{
						HobString temp( "0" );

						if (( isPeripheralComm() ) && ( !isManagerDeleted() ))
							if ( EvolWrapperStatusInfo::BuildingAirPressure == getStatusInfo().getWrapperState() )
							temp = "1";

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
	}

	return HfcObject::serializedData.data();
}
