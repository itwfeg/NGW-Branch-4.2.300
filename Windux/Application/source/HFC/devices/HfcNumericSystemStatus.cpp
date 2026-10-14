/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcNumericSystemStatus.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcNumericSystemStatus::dataType = 0;
std::string HfcNumericSystemStatus::dataTypeName("HfcNumericSystemStatus"); 
#endif

HfcNumericSystemStatus::HfcNumericSystemStatus()
	: NumericSystemStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcNumericSystemStatus::HfcNumericSystemStatus(const char *data)
	: NumericSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcNumericSystemStatus::HfcNumericSystemStatus(const char *data, int len, int formatType)
	: NumericSystemStatus()
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
			SystemStatusInfo *nmSystemStatusInfo = (SystemStatusInfo *) getSystemStatusInfo();

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetHfcNmSysStatusRomChecksum:
						nmSystemStatusInfo->setROM_Checksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcNmSysStatusSystemChecksum:
						nmSystemStatusInfo->setSystemChecksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcNmSysStatusIsEepromError:
						setEepromError(nmSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcNmSysStatusIsBitTestError:
						setBitError(nmSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcNmSysStatusBitTestAddress:
//						nmSystemStatusInfo->setBitTestAddress(Long::valueOf(token.c_str()));
//						break;
					case fieldOffsetHfcNmSysStatusIsAddressError:
						setAddressError(nmSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcNmSysStatusAddressTestAddress:
//						nmSystemStatusInfo->setAddressTestAddress(Long::valueOf(token.c_str()));
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

HfcNumericSystemStatus::~HfcNumericSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcNumericSystemStatus::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			HfcObject::serializedData = "Serialized Numeric System Status : Internal format";
		}
		break;

		case BrowserFormat:
		{
			const SystemStatusInfo *nmSystemStatusInfo = getSystemStatusInfo();
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcNmSysStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcNmSysStatusSystemChecksum:
						HfcObject::serializedData.append(Hex::toString((long)nmSystemStatusInfo->getSystemChecksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcNmSysStatusRomChecksum:
						HfcObject::serializedData.append(Hex::toString((long)nmSystemStatusInfo->getROM_Checksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcNmSysStatusIsEepromError:
						HfcObject::serializedData.append(Bool::toString(nmSystemStatusInfo->isEepromError()));
						break;
					case fieldOffsetHfcNmSysStatusIsBitTestError:
						HfcObject::serializedData.append(Bool::toString(nmSystemStatusInfo->isBitError()));
						break;
//					case fieldOffsetHfcNmSysStatusBitTestAddress:
//						HfcObject::serializedData.append(Long::toString(nmSystemStatusInfo->getBitTestAddress()));
//						break;
					case fieldOffsetHfcNmSysStatusIsAddressError:
						HfcObject::serializedData.append(Bool::toString(nmSystemStatusInfo->isAddressError()));
						break;
//					case fieldOffsetHfcNmSysStatusAddressTestAddress:
//						HfcObject::serializedData.append(Long::toString(nmSystemStatusInfo->getAddressTestAddress()));
//						break;
					case fieldOffsetHfcNmSysStatusSoftwareVersion:
						HfcObject::serializedData.append(getSoftwareVersion().getRevMajMinStr());
						break;
					case fieldOffsetHfcNmSysStatusHardwareVersion:
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

void HfcNumericSystemStatus::setBitError(SystemStatusInfo *nmSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		nmSystemStatusInfo->setBitError();
	else
		nmSystemStatusInfo->clearBitError();
}

void HfcNumericSystemStatus::setAddressError(SystemStatusInfo *nmSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		nmSystemStatusInfo->setAddressError();
	else
		nmSystemStatusInfo->clearAddressError();
}

void HfcNumericSystemStatus::setEepromError(SystemStatusInfo *nmSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		nmSystemStatusInfo->setEepromError();
	else
		nmSystemStatusInfo->clearEepromError();
}
