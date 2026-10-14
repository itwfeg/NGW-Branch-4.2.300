/**
 * @file HfcNGWUpgradeRequired.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to start the upgrade page
 * Created on: July 18, 2016
 */
#include "HfcNGWUpgradeRequired.h"
#include "Integer.h"

HfcNGWUpgradeRequired::HfcNGWUpgradeRequired(const char *data) : NGWUpgradeRequired(data)
{

}

HfcNGWUpgradeRequired::~HfcNGWUpgradeRequired()
{
}

const char *HfcNGWUpgradeRequired::serialize(int format) const
{
	Debug::trace("HfcNGWUpgradeRequired", "serialize()");
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
			Debug::trace("HfcNGWUpgradeRequired", "serialize(Invalid format!!!)");
			break;
	}
	return HfcObject::serializedData.data();
}


