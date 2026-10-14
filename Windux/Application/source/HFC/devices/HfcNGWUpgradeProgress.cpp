/**
 * @file HfcNGWUpgradeProgress.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to the NGW upgrade progress
 * Created on: April 5, 2016
 */
#include "HfcNGWUpgradeProgress.h"
#include "Integer.h"

HfcNGWUpgradeProgress::HfcNGWUpgradeProgress(const char *data) : NGWUpgradeProgress(data)
{

}

HfcNGWUpgradeProgress::~HfcNGWUpgradeProgress()
{
}

const char *HfcNGWUpgradeProgress::serialize(int format) const
{
	Debug::trace("HfcNGWUpgradeProgress", "serialize()");
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
			HfcObject::serializedData.append(Integer::toString(getPercentComplete()));

			Debug::trace("HfcNGWUpgradeProgress", "serialize: " + HfcObject::serializedData);
			break;
		}

		default:
			Debug::trace("HfcNGWUpgradeProgress", "serialize(Invalid format!!!)");
			break;
	}
	return HfcObject::serializedData.data();
}




