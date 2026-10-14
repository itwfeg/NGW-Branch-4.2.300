/**
 * @file HfcKPCBootUpgraderStatus.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe Crew
 * @brief notifies the UI to the status of the KPC upgrade
 * Created on: August 17, 2019
 */
#include "HfcKPCBootUpgraderStatus.h"
HfcKPCBootUpgraderStatus::HfcKPCBootUpgraderStatus(const char *data) : KPCBootUpgraderStatus(data)
{

}

HfcKPCBootUpgraderStatus::~HfcKPCBootUpgraderStatus()
{
}

const char *HfcKPCBootUpgraderStatus::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(ULong::toString(getId()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString(getStatusCode()));				// this is enum	BootUpgraderStatusCode
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString(getBootUpgraderFaults()));		// this is enum	BootUpgraderFaults
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(ULong::toString(getFaultInfo()));
			break;
		}
		default:
		{
			Debug::trace("HfcKPCBootUpgraderStartUpgrade", "serialize(Invalid format!!!)");
			break;
		}
	}

	Debug::trace("HfcKPCBootUpgraderStatus", "serialize(): " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}




