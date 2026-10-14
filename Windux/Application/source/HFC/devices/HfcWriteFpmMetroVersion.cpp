/**
 * @file HfcWriteFpmMetroVersion.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager message to write metrology version to serial flash
 * Created on: July 20, 2016
 */
#include"HfcWriteFpmMetroVersion.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Debug.h"

HfcWriteFpmMetroVersion::HfcWriteFpmMetroVersion(const char* data, int len, int formatType)
{
	switch(formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len), token;
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);

			if( tok.next(&token) != -1 )
			{
				metrologyVersion_ = token;
			}
		}
	}
}

HfcWriteFpmMetroVersion::HfcWriteFpmMetroVersion(Hobart::CEvolMsgReceiver *pHobThread): EvolMessage(MfgTestDev, WriteOfScaleSWConfigMetrologicalVersion, pHobThread)
{
	metrologyVersion_ = "";

}

const char *HfcWriteFpmMetroVersion::serialize(int format) const
{
	HfcObject::serializedData = "";

	HfcObject::serializedData = Integer::toString((int)MfgTestDev);
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(Integer::toString((int)WriteOfScaleSWConfigMetrologicalVersion));
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(metrologyVersion_);

	Debug::trace("HfcWriteFpmManData", "Serialized: " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}



