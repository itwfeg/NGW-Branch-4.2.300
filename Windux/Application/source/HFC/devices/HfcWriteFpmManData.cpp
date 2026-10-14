/**
 * @file HfcWriteFpmManData.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager message to write manufacturing data to serial flash
 * Created on: Feb 26, 2013
 */
#include "HfcWriteFpmManData.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "HobVersion.h"
#include "HobString.h"
#include "Debug.h"

HfcWriteFpmManData::HfcWriteFpmManData(const char* data, int len, int formatType)
{
	switch(formatType)
	{
		case BrowserFormat:
		{

			short loopCnt = 1;
			HobString temp(data, len), token;
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);

			while ( tok.next(&token) != -1 )
			{
				switch (loopCnt)
				{
					case OffsetScaleStyle:
					{
						scaleStyle_ = token;
						break;
					}
					case OffsetBoardAssembly:
					{
						boardAssembly_ = token;
						break;
					}
					case OffsetBoardRevision:
					{
						boardRevision_ = token;
						break;
					}
					case OffsetDateOfMan:
					{
						dateOfManufacture_ = token;
						break;
					}
					case OffsetSerialNumb:
					{
						serialNumber_  = token;
						break;
					}
					case OffsetSystemStyle:
					{
						systemStyle_ = token;
						break;
					}
					default:
						break;
				}

				loopCnt++;
			}
			break;
		}
	}
}

HfcWriteFpmManData::HfcWriteFpmManData(MtdManufacturingData *pMfgData, Hobart::CEvolMsgReceiver *pHobThread): EvolMessage(MfgTestDev, ReadMfgData, pHobThread)
{
	Debug::trace( "HfcWriteFpmManData", "Entered constructor: ");

	// sanity check
	if ( !pMfgData )
		Debug::trace( "HfcWriteFpmManData", "BAD MANF DATA" );
	else
	{
		scaleStyle_ 		= pMfgData->getScaleStyle();
		boardAssembly_		= pMfgData->getBoardAssembly();
		boardRevision_ 		= pMfgData->getBoardRevision();
		dateOfManufacture_ 	= pMfgData->getDateOfManufacturing();
		serialNumber_ 		= pMfgData->getSerialNumber();
		systemStyle_		= pMfgData->getSystemStyle();
	}

	Debug::trace( "HfcWriteFpmManData", "scaleStyle_: " + scaleStyle_);
	Debug::trace( "HfcWriteFpmManData", "boardAssembly_: " + boardAssembly_);
	Debug::trace( "HfcWriteFpmManData", "boardRevision_: " + boardRevision_);
	Debug::trace( "HfcWriteFpmManData", "dateOfManufacture_: " + dateOfManufacture_);
	Debug::trace( "HfcWriteFpmManData", "serialNumber_: " + serialNumber_);
	Debug::trace( "HfcWriteFpmManData", "systemStyle_: " + systemStyle_);
}

const char *HfcWriteFpmManData::serialize(int format) const
{
	HfcObject::serializedData = "";

	HfcObject::serializedData = Integer::toString((int)MfgTestDev);
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(Integer::toString((int)ReadMfgData));
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(scaleStyle_);
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(boardAssembly_);
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(boardRevision_);
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(dateOfManufacture_);
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(serialNumber_);
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(systemStyle_);

	Debug::trace("HfcWriteFpmManData", "Serialized: " + HfcObject::serializedData);

	return HfcObject::serializedData.data();
}

