/**
 * @file HfcKPCLoaderProgress.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Job E Crew
 * @brief notifies the UI to the KPC Loader progress
 * Created on: August 19, 2019
 */
#include "HfcKPCLoaderProgress.h"
#include "Integer.h"

HfcKPCLoaderProgress::HfcKPCLoaderProgress(const char *data) : KPCUpgradeProgress(data)
{

}

HfcKPCLoaderProgress::~HfcKPCLoaderProgress()
{
}

const char *HfcKPCLoaderProgress::serialize(int format) const
{
	Debug::trace("HfcKPCLoaderProgress", "serialize()");
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
			HfcObject::serializedData.append(BrowserFormatSeparator);

			Debug::trace("HfcKPCLoaderProgress", "serialize: " + HfcObject::serializedData);
			break;
		}

		default:
			Debug::trace("HfcKPCLoaderProgress", "serialize(Invalid format!!!)");
			break;
	}
	return HfcObject::serializedData.data();
}




