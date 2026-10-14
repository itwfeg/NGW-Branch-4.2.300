/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcSystemConfig.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
/*
*/
#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcSystemConfig::dataType = 0;
std::string HfcSystemConfig::dataTypeName("HfcSystemConfig"); 
#endif

HfcSystemConfig::HfcSystemConfig()
	: SystemConfiguration(UnknownDevice)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}
HfcSystemConfig::HfcSystemConfig(const char *data)
	: SystemConfiguration(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcSystemConfig::HfcSystemConfig(const char *data, int len, int formatType)
	: SystemConfiguration(UnknownDevice)
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
					case fieldOffsetSysConfigType:
						setSysConfigType((SysConfigType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetDeviceSignature:
						setDeviceSignature((UINT8*) token.c_str());
						break;
					case fieldOffsetStationAddress:
						setStationAddress(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetControllerAddress:
						setControllerAddress(Integer::valueOf(token.c_str()));
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

HfcSystemConfig::~HfcSystemConfig()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcSystemConfig::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			SystemConfiguration::serialize();
			HfcObject::serializedData = SystemConfiguration::serializedData;
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcSystemConfigFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetSysConfigType:
						HfcObject::serializedData.append(Integer::toString(getSysConfigType()));
						break;
					case fieldOffsetDeviceSignature:
					{
						UINT8 tempDeviceSignature[SystemDevSignatureSize];

						getDeviceSignature((UINT8 *) &tempDeviceSignature);
						HfcObject::serializedData.append((const char *) &tempDeviceSignature);
					}
						break;
					case fieldOffsetStationAddress:
						HfcObject::serializedData.append(Integer::toString(getStationAddress()));
						break;
					case fieldOffsetControllerAddress:
						HfcObject::serializedData.append(Integer::toString(getControllerAddress()));
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

UINT32 HfcSystemConfig::getSerializedLength() const
{
	// todo - If serialize() hasn't yet been called, should this function call it first;
	// otherwise, the length will be invalid, yet the programmer may not have known to
	// call serialize() first.
	return (UINT32)HfcObject::serializedData.length();
}
