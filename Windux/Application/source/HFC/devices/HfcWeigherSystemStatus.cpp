/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWeigherSystemStatus.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherSystemStatus::dataType = 0;
std::string HfcWeigherSystemStatus::dataTypeName("HfcWeigherSystemStatus"); 
#endif

HfcWeigherSystemStatus::HfcWeigherSystemStatus()
	: WeigherSystemStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemStatus::HfcWeigherSystemStatus(const char *data)
	: WeigherSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemStatus::HfcWeigherSystemStatus(const char *data, int len, int formatType)
	: WeigherSystemStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:	// Going to RTC
			break;

		case BrowserFormat:
		{
			/*	This case should never happen. 
				This data will be coming from the Peripheral and we won't be able to change it.
				However, we will need this for testing purposes.
			*/
			SystemStatusInfo *wgSystemStatusInfo = (SystemStatusInfo *) getSystemStatusInfo();

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetHfcWgSysStatusSystemChecksum:
						wgSystemStatusInfo->setSystemChecksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcWgSysStatusRomChecksum:
						wgSystemStatusInfo->setROM_Checksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcWgSysStatusIsEepromError:
						setEepromError(wgSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcWgSysStatusIsBitTestError:
						setBitError(wgSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcWgSysStatusBitTestAddress:
//						wgSystemStatusInfo->setBitTestAddress(Long::valueOf(token.c_str()));
//						break;
					case fieldOffsetHfcWgSysStatusIsAddressError:
						setAddressError(wgSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcWgSysStatusAddressTestAddress:
//						wgSystemStatusInfo->setAddressTestAddress(Long::valueOf(token.c_str()));
//						break;

					default:
						break;
				}
				loopCnt++;
			}
		}
			break;

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWeigherSystemStatus::~HfcWeigherSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWeigherSystemStatus::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			HfcObject::serializedData = "Serialized Weigher System Status : Internal format";
		}
		break;

		case BrowserFormat:
		{
/**/
			const SystemStatusInfo *wgSystemStatusInfo = getSystemStatusInfo();
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWgSysStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
/*
			HfcObject::serializedData = "";
			for (int loopCnt = 1; loopCnt < endHfcWgSysStatusFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					HfcObject::serializedData.append(BrowserFormatSeparator);
*/
// ~~
				switch (loopCnt)
				{
					case fieldOffsetHfcWgSysStatusSystemChecksum:
						HfcObject::serializedData.append(Hex::toString((long)wgSystemStatusInfo->getSystemChecksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcWgSysStatusRomChecksum:
						HfcObject::serializedData.append(Hex::toString((long)wgSystemStatusInfo->getROM_Checksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcWgSysStatusIsEepromError:
						HfcObject::serializedData.append(Bool::toString(wgSystemStatusInfo->isEepromError()));
						break;
					case fieldOffsetHfcWgSysStatusIsBitTestError:
						HfcObject::serializedData.append(Bool::toString(wgSystemStatusInfo->isBitError()));
						break;
//					case fieldOffsetHfcWgSysStatusBitTestAddress:
//						HfcObject::serializedData.append(Long::toString(wgSystemStatusInfo->getBitTestAddress()));
//						break;
					case fieldOffsetHfcWgSysStatusIsAddressError:
						HfcObject::serializedData.append(Bool::toString(wgSystemStatusInfo->isAddressError()));
						break;
//					case fieldOffsetHfcWgSysStatusAddressTestAddress:
//						HfcObject::serializedData.append(Long::toString(wgSystemStatusInfo->getAddressTestAddress()));
//						break;

					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Weigher System Status : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcWeigherSystemStatus::setBitError(SystemStatusInfo *wgSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		wgSystemStatusInfo->setBitError();
	else
		wgSystemStatusInfo->clearBitError();
}

void HfcWeigherSystemStatus::setAddressError(SystemStatusInfo *wgSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		wgSystemStatusInfo->setAddressError();
	else
		wgSystemStatusInfo->clearAddressError();
}

void HfcWeigherSystemStatus::setEepromError(SystemStatusInfo *wgSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		wgSystemStatusInfo->setEepromError();
	else
		wgSystemStatusInfo->clearEepromError();
}
