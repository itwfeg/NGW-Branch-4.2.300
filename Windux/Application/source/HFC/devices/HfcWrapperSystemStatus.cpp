/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperSystemStatus.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperSystemStatus::dataType = 0;
std::string HfcWrapperSystemStatus::dataTypeName("HfcWrapperSystemStatus"); 
#endif

HfcWrapperSystemStatus::HfcWrapperSystemStatus()
	: WrapperSystemStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperSystemStatus::HfcWrapperSystemStatus(const char *data)
	: WrapperSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperSystemStatus::HfcWrapperSystemStatus(const char *data, int len, int formatType)
	: WrapperSystemStatus()
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
			SystemStatusInfo *wrSystemStatusInfo = (SystemStatusInfo *) getSystemStatusInfo();

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetHfcWrSysStatusRomChecksum:
						wrSystemStatusInfo->setROM_Checksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcWrSysStatusSystemChecksum:
						wrSystemStatusInfo->setSystemChecksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcWrSysStatusIsEepromError:
						setEepromError(wrSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcWrSysStatusIsBitTestError:
						setBitError(wrSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcWrSysStatusBitTestAddress:
//						wrSystemStatusInfo->setBitTestAddress(Long::valueOf(token.c_str()));
//						break;
					case fieldOffsetHfcWrSysStatusIsAddressError:
						setAddressError(wrSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcWrSysStatusAddressTestAddress:
//						wrSystemStatusInfo->setAddressTestAddress(Long::valueOf(token.c_str()));
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

HfcWrapperSystemStatus::~HfcWrapperSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperSystemStatus::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			HfcObject::serializedData = "Serialized Wrapper System Status : Internal format";
		}
		break;

		case BrowserFormat:
		{
			const SystemStatusInfo *wrSystemStatusInfo = getSystemStatusInfo();
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWrSysStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcWrSysStatusSystemChecksum:
						HfcObject::serializedData.append(Hex::toString((long)wrSystemStatusInfo->getSystemChecksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcWrSysStatusRomChecksum:
						HfcObject::serializedData.append(Hex::toString((long)wrSystemStatusInfo->getROM_Checksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcWrSysStatusIsEepromError:
						HfcObject::serializedData.append(Bool::toString(wrSystemStatusInfo->isEepromError()));
						break;
					case fieldOffsetHfcWrSysStatusIsBitTestError:
						HfcObject::serializedData.append(Bool::toString(wrSystemStatusInfo->isBitError()));
						break;
//					case fieldOffsetHfcWrSysStatusBitTestAddress:
//						HfcObject::serializedData.append(Long::toString(wrSystemStatusInfo->getBitTestAddress()));
//						break;
					case fieldOffsetHfcWrSysStatusIsAddressError:
						HfcObject::serializedData.append(Bool::toString(wrSystemStatusInfo->isAddressError()));
						break;
//					case fieldOffsetHfcWrSysStatusAddressTestAddress:
//						HfcObject::serializedData.append(Long::toString(wrSystemStatusInfo->getAddressTestAddress()));
//						break;

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

void HfcWrapperSystemStatus::setBitError(SystemStatusInfo *wrSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		wrSystemStatusInfo->setBitError();
	else
		wrSystemStatusInfo->clearBitError();
}

void HfcWrapperSystemStatus::setAddressError(SystemStatusInfo *wrSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		wrSystemStatusInfo->setAddressError();
	else
		wrSystemStatusInfo->clearAddressError();
}

void HfcWrapperSystemStatus::setEepromError(SystemStatusInfo *wrSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		wrSystemStatusInfo->setEepromError();
	else
		wrSystemStatusInfo->clearEepromError();
}
