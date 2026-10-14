/**
 * @file HfcKPCBootLoaderStatus.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Job E. Crew
 * @brief notifies the UI to the status of the KPC upgrade
 * Created on: August 19, 2019
 */
#include "HfcKPCBootLoaderStatus.h"
HfcKPCBootLoaderStatus::HfcKPCBootLoaderStatus(const char *data) : KPCBootStatus(data)
{

}

HfcKPCBootLoaderStatus::~HfcKPCBootLoaderStatus()
{
}

const char *HfcKPCBootLoaderStatus::serialize(int format) const
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

			HfcObject::serializedData.append(Integer::toString(getStatusCode()));				// this is enum	BootLoaderStatusCode
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString( getBootloaderFaults() ));			// this is enum	BootLoaderFaults
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(ULong::toString(getFaultInfo()));
			break;
		}
		default:
		{
			Debug::trace("HfcKPCBootLoaderStartUpgrade", "serialize(Invalid format!!!)");
			break;
		}
	}

	Debug::trace("HfcKPCBootLoaderStatus", "serialize(): " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}




