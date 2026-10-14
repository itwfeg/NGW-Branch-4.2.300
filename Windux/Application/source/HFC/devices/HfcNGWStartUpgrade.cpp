/**
 * @file HfcNGWStartUpgrade.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to start the NGW upgrade page
 * Created on: April 5, 2016
 */
#include "HobTokenizerCSV.h"
#include "HfcNGWStartUpgrade.h"
#include "Integer.h"

HfcNGWStartUpgrade::HfcNGWStartUpgrade(const char *data) : NGWStartUpgrade(data)
{
}

HfcNGWStartUpgrade::~HfcNGWStartUpgrade()
{
}

const char *HfcNGWStartUpgrade::serialize(int format) const
{
	Debug::trace("HfcNGWStartUpgrade", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			int i = (int)getNumOfNodes();
			HfcObject::serializedData.append(Integer::toString(i));
			break;
		}

		default:
			Debug::trace("HfcNGWStartUpgrade", "serialize(Invalid format!!!)");
			break;
	}
	return HfcObject::serializedData.data();
}




