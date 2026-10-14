/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcMarqueeSystemStatus.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcMarqueeSystemStatus::dataType = 0;
std::string HfcMarqueeSystemStatus::dataTypeName("HfcMarqueeSystemStatus"); 
#endif

HfcMarqueeSystemStatus::HfcMarqueeSystemStatus()
	: MarqueeSystemStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcMarqueeSystemStatus::HfcMarqueeSystemStatus(const char *data)
	: MarqueeSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcMarqueeSystemStatus::HfcMarqueeSystemStatus(const char *data, int len, int formatType)
	: MarqueeSystemStatus()
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
#ifdef NeedForTestBrowser
			/*	This case should never happen. 
				This data will be coming from the Peripheral and we won't be able to change it.
				However, we will need this for testing purposes.
			*/
			SystemStatusInfo *mqSystemStatusInfo = (SystemStatusInfo *) getSystemStatusInfo();

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetHfcMqSysStatusRomChecksum:
						mqSystemStatusInfo->setROM_Checksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcMqSysStatusSystemChecksum:
						mqSystemStatusInfo->setSystemChecksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcMqSysStatusIsEepromError:
						setEepromError(mqSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcMqSysStatusIsBitTestError:
						setBitError(mqSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcMqSysStatusBitTestAddress:
//						mqSystemStatusInfo->setBitTestAddress(Long::valueOf(token.c_str()));
//						break;
					case fieldOffsetHfcMqSysStatusIsAddressError:
						setAddressError(mqSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcMqSysStatusAddressTestAddress:
//						mqSystemStatusInfo->setAddressTestAddress(Long::valueOf(token.c_str()));
//						break;

					default:
						break;
				}
				loopCnt++;
			}
#endif
		}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcMarqueeSystemStatus::~HfcMarqueeSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcMarqueeSystemStatus::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			HfcObject::serializedData = "Serialized Marquee System Status : Internal format";
		}
		break;

		case BrowserFormat:
		{
			const SystemStatusInfo *mqSystemStatusInfo = getSystemStatusInfo();
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcMqSysStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcMqSysStatusSystemChecksum:
						HfcObject::serializedData.append(Hex::toString((long)mqSystemStatusInfo->getSystemChecksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcMqSysStatusRomChecksum:
						HfcObject::serializedData.append(Hex::toString((long)mqSystemStatusInfo->getROM_Checksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcMqSysStatusIsEepromError:
						HfcObject::serializedData.append(Bool::toString(mqSystemStatusInfo->isEepromError()));
						break;
					case fieldOffsetHfcMqSysStatusIsBitTestError:
						HfcObject::serializedData.append(Bool::toString(mqSystemStatusInfo->isBitError()));
						break;
//					case fieldOffsetHfcMqSysStatusBitTestAddress:
//						HfcObject::serializedData.append(Long::toString(mqSystemStatusInfo->getBitTestAddress()));
//						break;
					case fieldOffsetHfcMqSysStatusIsAddressError:
						HfcObject::serializedData.append(Bool::toString(mqSystemStatusInfo->isAddressError()));
						break;
//					case fieldOffsetHfcMqSysStatusAddressTestAddress:
//						HfcObject::serializedData.append(Long::toString(mqSystemStatusInfo->getAddressTestAddress()));
//						break;
					case fieldOffsetHfcMqSysStatusSoftwareVersion:
						HfcObject::serializedData.append(getSoftwareVersion().getRevMajMinStr());
						break;
					case fieldOffsetHfcMqSysStatusHardwareVersion:
						HfcObject::serializedData.append(getHardwareVersion().getRevMajMinStr());
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcMarqueeSystemStatus::setBitError(SystemStatusInfo *mqSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		mqSystemStatusInfo->setBitError();
	else
		mqSystemStatusInfo->clearBitError();
}

void HfcMarqueeSystemStatus::setAddressError(SystemStatusInfo *mqSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		mqSystemStatusInfo->setAddressError();
	else
		mqSystemStatusInfo->clearAddressError();
}

void HfcMarqueeSystemStatus::setEepromError(SystemStatusInfo *mqSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		mqSystemStatusInfo->setEepromError();
	else
		mqSystemStatusInfo->clearEepromError();
}
