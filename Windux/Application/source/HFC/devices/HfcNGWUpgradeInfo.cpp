/**
 * @file HfcNGWUpgradeInfo.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to the Info of the NGW upgrade
 * Created on: April 6, 2016
 */
#include "HfcNGWUpgradeInfo.h"

#include <HobList.h>

HfcNGWUpgradeInfo::HfcNGWUpgradeInfo(const char *data) : NGWUpgradeInfo(data)
{

}

HfcNGWUpgradeInfo::~HfcNGWUpgradeInfo()
{
}

const char *HfcNGWUpgradeInfo::serialize(int format) const
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

		HobList nodeIdsList;
		HobList nodeVersionsList;

		HobList firmwareVersionsList;

		for ( const auto &nodeId : nodeIds )
			nodeIdsList.push_back( Integer::toString( nodeId ) );
		for ( const auto &nodeVersion : nodeVersions )
			nodeVersionsList.push_back( nodeVersion );

		for ( const auto &firmwareVersion : firmwareVersions )
			firmwareVersionsList.push_back( firmwareVersion );

		HfcObject::serializedData.append(nodeIdsList.serialize(format));
		HfcObject::serializedData.append(BrowserFormatSeparator);

		HfcObject::serializedData.append(nodeVersionsList.serialize(format));
		HfcObject::serializedData.append(BrowserFormatSeparator);

		HfcObject::serializedData.append(firmwareVersionsList.serialize(format));
		HfcObject::serializedData.append(BrowserFormatSeparator);

		HfcObject::serializedData.append(isRequired() ? "1" : "0");

		break;
	}

	default:
		Debug::trace("HfcNGWUpgradeInfo", "serialize(Invalid format!!!)");
		break;

	}

	Debug::trace("HfcNGWUpgradeInfo", "serialize(): " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}




