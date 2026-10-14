/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterSystemStatus.h"
#include "Configuration.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterSystemStatus::dataType = 0;
std::string HfcPrinterSystemStatus::dataTypeName("HfcPrinterSystemStatus"); 
#endif

HfcPrinterSystemStatus::HfcPrinterSystemStatus()
	: PrinterSystemStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSystemStatus::HfcPrinterSystemStatus(const char *data)
	: PrinterSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSystemStatus::HfcPrinterSystemStatus(const char *data, int len, int formatType)
	: PrinterSystemStatus()
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
			SystemStatusInfo *prSystemStatusInfo = (SystemStatusInfo *) getSystemStatusInfo();

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetHfcPrSysStatusPrinterType:
						setPrinterType((PrinterType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcPrSysStatusRomChecksum:
						prSystemStatusInfo->setROM_Checksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcPrSysStatusSystemChecksum:
						prSystemStatusInfo->setSystemChecksum((UINT16)Long::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcPrSysStatusIsEepromError:
						setEepromError(prSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetHfcPrSysStatusIsBitTestError:
						setBitError(prSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcPrSysStatusBitTestAddress:
//						prSystemStatusInfo->setBitTestAddress(Long::valueOf(token.c_str()));
//						break;
					case fieldOffsetHfcPrSysStatusIsAddressError:
						setAddressError(prSystemStatusInfo, Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetHfcPrSysStatusAddressTestAddress:
//						prSystemStatusInfo->setAddressTestAddress(Long::valueOf(token.c_str()));
//						break;

					default:
						break;
				}
				loopCnt++;
			}
#endif
		}
			break;

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterSystemStatus::~HfcPrinterSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterSystemStatus::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			HfcObject::serializedData = "Serialized Printer System Status : Internal format";
		}
		break;

		case BrowserFormat:
		{
			const SystemStatusInfo *prSystemStatusInfo = getSystemStatusInfo();
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrSysStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcPrSysStatusPrinterType:
						HfcObject::serializedData.append(Integer::toString(getPrinterType()));
						break;
					case fieldOffsetHfcPrSysStatusSystemChecksum:
						HfcObject::serializedData.append(Hex::toString((long)prSystemStatusInfo->getSystemChecksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcPrSysStatusRomChecksum:
						HfcObject::serializedData.append(Hex::toString((long)prSystemStatusInfo->getROM_Checksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcPrSysStatusIsEepromError:
						HfcObject::serializedData.append(Bool::toString(prSystemStatusInfo->isEepromError()));
						break;
					case fieldOffsetHfcPrSysStatusIsBitTestError:
						HfcObject::serializedData.append(Bool::toString(prSystemStatusInfo->isBitError()));
						break;
					case fieldOffsetHfcPrSysStatusIsAddressError:
						HfcObject::serializedData.append(Bool::toString(prSystemStatusInfo->isAddressError()));
						break;
					case fieldOffsetHfcPrSysStatusSoftwareVersion:
						/*
						if (getSoftwareVersion().getVersionType() == HobVersion::KpcPrinterSW)
							HfcObject::serializedData.append(getSoftwareVersion().getRevMajMinBuildStr());
						else
						*/
							HfcObject::serializedData.append(getSoftwareVersion().getRevMajMinEngStr());
						break;
					case fieldOffsetHfcPrSysStatusHardwareVersion:
						HfcObject::serializedData.append(getHardwareVersion().getRevMajMinEngStr());
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer System Status : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcPrinterSystemStatus::setBitError(SystemStatusInfo *prSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		prSystemStatusInfo->setBitError();
	else
		prSystemStatusInfo->clearBitError();
}

void HfcPrinterSystemStatus::setAddressError(SystemStatusInfo *prSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		prSystemStatusInfo->setAddressError();
	else
		prSystemStatusInfo->clearAddressError();
}

void HfcPrinterSystemStatus::setEepromError(SystemStatusInfo *prSystemStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		prSystemStatusInfo->setEepromError();
	else
		prSystemStatusInfo->clearEepromError();
}
