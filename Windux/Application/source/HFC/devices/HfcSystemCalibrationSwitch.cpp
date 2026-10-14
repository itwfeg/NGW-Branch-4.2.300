/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcSystemCalibrationSwitch.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
/*
*/
#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcSystemCalibrationSwitch::dataType = 0;
std::string HfcSystemCalibrationSwitch::dataTypeName("HfcSystemCalibrationSwitch"); 
#endif

HfcSystemCalibrationSwitch::HfcSystemCalibrationSwitch()
	: SystemCalibrationSwitch(UnknownDevice)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}
HfcSystemCalibrationSwitch::HfcSystemCalibrationSwitch(const char *data)
	: SystemCalibrationSwitch(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcSystemCalibrationSwitch::HfcSystemCalibrationSwitch(const char *data, int len, int formatType)
	: SystemCalibrationSwitch(UnknownDevice)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetCalibrationSwitch:
						if (Bool::valueOf(token.c_str()) == true)
							setSystemCalibrationSwitch();
						else
							clearSystemCalibrationSwitch();
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcSystemCalibrationSwitch::~HfcSystemCalibrationSwitch()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcSystemCalibrationSwitch::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			HfcObject::serializedData = "Serialized  Info : internal format";
/*
			const char *msgdata = SystemConfiguration::serialize();
			int msglen = SystemConfiguration::getSerializedLength();

			HfcObject::serializedData = SystemConfiguration::serializedData;
*/
			SystemCalibrationSwitch::serialize();
			int msglen = SystemCalibrationSwitch::getSerializedLength();

			HfcObject::serializedData = SystemCalibrationSwitch::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcSystemCalibrationSwitchFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetCalibrationSwitch:
						HfcObject::serializedData.append(Bool::toString(isSystemCalibrationSwitch()));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized  Info : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

UINT32 HfcSystemCalibrationSwitch::getSerializedLength() const
{
	// todo - If serialize() hasn't yet been called, should this function call it first;
	// otherwise, the length will be invalid, yet the programmer may not have known to
	// call serialize() first.
	return (UINT32)HfcObject::serializedData.length();
}
