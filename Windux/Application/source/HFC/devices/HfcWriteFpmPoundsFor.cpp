/**
 * @file HfcWriteFpmPoundsFor.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager message to write pounds for setting state to serial flash
 * Created on: July 20, 2016
 */
#include"HfcWriteFpmPoundsFor.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Debug.h"

HfcWriteFpmPoundsFor::HfcWriteFpmPoundsFor(const char* data, int len, int formatType)
{
	switch(formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len), token;
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);

			if( tok.next(&token) != -1 )
			{
				poundsFor_ = token;
			}
		}
	}
}

HfcWriteFpmPoundsFor::HfcWriteFpmPoundsFor(Hobart::CEvolMsgReceiver *pHobThread): EvolMessage(MfgTestDev, WriteOfScaleSWConfigMetrologicalVersion, pHobThread)
{
	poundsFor_ = "";

}

const char *HfcWriteFpmPoundsFor::serialize(int format) const
{
	HfcObject::serializedData = "";

	HfcObject::serializedData = Integer::toString((int)MfgTestDev);
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(Integer::toString((int)WriteOfScaleSWConfigMetrologicalVersion));
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(poundsFor_);

	Debug::trace("HfcWriteFpmPoundsFor", "Serialized: " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}



