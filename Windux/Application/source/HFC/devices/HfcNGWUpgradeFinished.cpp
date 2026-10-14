/**
 * @file HfcNGWUpgradeFinished.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to close upgrade page
 * Created on: April 5, 2016
 */
#include "HfcNGWUpgradeFinished.h"
#include "Integer.h"

HfcNGWUpgradeFinished::HfcNGWUpgradeFinished(const char *data) : NGWUpgradeFinished(data)
{
}

HfcNGWUpgradeFinished::~HfcNGWUpgradeFinished()
{
}

const char *HfcNGWUpgradeFinished::serialize(int format) const
{
	Debug::trace("HfcNGWUpgradeFinished", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			break;
		}
		default:
			Debug::trace("HfcNGWUpgradeFinished", "serialize(Invalid format!!!)");
			break;
	}
	return HfcObject::serializedData.data();
}





