/**
 * @file HfcKPCLoaderInfo.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Joe E. Crew
 * @brief notifies the UI to the Info of the KPC Loader
 * Created on: August 19, 2019
 */
#include "HfcKPCLoaderInfo.h"


HfcKPCLoaderInfo::HfcKPCLoaderInfo(const char *data) : KPCUpgradeInfo(data)
{

}

HfcKPCLoaderInfo::~HfcKPCLoaderInfo()
{
}

const char *HfcKPCLoaderInfo::serialize(int format) const
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
		
		HfcObject::serializedData.append(getCurrentVersion());
		HfcObject::serializedData.append(BrowserFormatSeparator);

		HfcObject::serializedData.append(getAvailableVersion());
		
		break;
	}

	default:
		Debug::trace("HfcKPCLoaderInfo", "serialize(Invalid format!!!)");
		break;

	}

	Debug::trace("HfcKPCLoaderInfo", "serialize(): " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}




