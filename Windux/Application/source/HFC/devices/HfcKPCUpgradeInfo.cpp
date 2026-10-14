/**
 * @file HfcKPCUpgradeInfo.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe Crew
 * @brief notifies the UI to the Info of the KPC upgrade
 * Created on: August 17, 2019
 */
#include "HfcKPCUpgradeInfo.h"


HfcKPCUpgradeInfo::HfcKPCUpgradeInfo(const char *data) : KPCUpgradeInfo(data)
{

}

HfcKPCUpgradeInfo::~HfcKPCUpgradeInfo()
{
}

const char *HfcKPCUpgradeInfo::serialize(int format) const
{
	HfcObject::serializedData = "";
	Debug::trace("HfcKPCUpgradeInfo", "HfcKPCUpgradeInfo.serialize() call KPCUpgradeInfo show()");

	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
		
			Debug::trace("HfcKPCUpgradeInfo", "serialize() Current Version: " + getCurrentVersion() );
			Debug::trace("HfcKPCUpgradeInfo", "serialize() Available Version: " + getAvailableVersion() );

			HfcObject::serializedData.append(getCurrentVersion());
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(getAvailableVersion());
		
			Debug::trace("HfcKPCUpgradeInfo", "serialize(): " + HfcObject::serializedData);

			break;
		}

		default:
			Debug::trace("HfcKPCUpgradeInfo", "serialize(Invalid format!!!)");
			break;

	}

	Debug::trace("HfcKPCUpgradeInfo", "serialize(): " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}




