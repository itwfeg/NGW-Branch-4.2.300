/**
 * @file HfcNGWUpgradeStatus.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to the status of the NGW upgrade
 * Created on: April 6, 2016
 */
#include "HfcNGWUpgradeStatus.h"
HfcNGWUpgradeStatus::HfcNGWUpgradeStatus(const char *data) : NGWUpgradeStatus(data)
{

}

HfcNGWUpgradeStatus::~HfcNGWUpgradeStatus()
{
}

const char *HfcNGWUpgradeStatus::serialize(int format) const
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
			HfcObject::serializedData.append(Integer::toString(getNodeId()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(getState()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(getVersion());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(getError()));
			break;
		}
		default:
		{
			Debug::trace("HfcNGWStartUpgrade", "serialize(Invalid format!!!)");
			break;
		}
	}

	Debug::trace("HfcNGWUpgradeStatus", "serialize(): " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}




