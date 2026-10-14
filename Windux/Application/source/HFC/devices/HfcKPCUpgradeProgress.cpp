/**
 * @file HfcKPCUpgradeProgress.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe Crew
 * @brief notifies the UI to the KPC upgrade progress
 * Created on: August 18, 2019
 */
#include "HfcKPCUpgradeProgress.h"
#include "Integer.h"

HfcKPCUpgradeProgress::HfcKPCUpgradeProgress(const char *data) : KPCUpgradeProgress(data)
{

}

HfcKPCUpgradeProgress::~HfcKPCUpgradeProgress()
{
}

const char *HfcKPCUpgradeProgress::serialize(int format) const
{
	Debug::trace("HfcKPCUpgradeProgress", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString(getProgress()));

			Debug::trace("HfcKPCUpgradeProgress", "serialize: " + HfcObject::serializedData);
			break;
		}

		default:
			Debug::trace("HfcKPCUpgradeProgress", "serialize(Invalid format!!!)");
			break;
	}
	return HfcObject::serializedData.data();
}




