/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierSystemStatus.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierSystemStatus::dataType = 0;
std::string HfcLabelApplierSystemStatus::dataTypeName("HfcLabelApplierSystemStatus"); 
#endif

HfcLabelApplierSystemStatus::HfcLabelApplierSystemStatus(const char *data)
	: LabelApplierSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcLabelApplierSystemStatus::~HfcLabelApplierSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcLabelApplierSystemStatus::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			HfcObject::serializedData = "Serialized LabelApplier System Status : Internal format";
		}
		break;

		case BrowserFormat:
		{
			const SystemStatusInfo *masterStatus = getMasterStatus();
			const SystemStatusInfo *slave1StatusInfo = getSlave1Status();
			const SystemStatusInfo *slave2StatusInfo = getSlave2Status();
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcLASysStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcLASysStatusSystemChecksum:
						HfcObject::serializedData.append(Hex::toString((long)masterStatus->getSystemChecksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcLASysStatusRomChecksum:
						HfcObject::serializedData.append(Hex::toString((long)masterStatus->getROM_Checksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcLASysStatusIsEepromError:
						HfcObject::serializedData.append(Bool::toString(masterStatus->isEepromError()));
						break;
					case fieldOffsetHfcLASysStatusIsBitTestError:
						HfcObject::serializedData.append(Bool::toString(masterStatus->isBitError()));
						break;
//					case fieldOffsetHfcLASysStatusBitTestAddress:
//						HfcObject::serializedData.append(Long::toString(masterStatus->getBitTestAddress()));
//						break;
					case fieldOffsetHfcLASysStatusIsAddressError:
						HfcObject::serializedData.append(Bool::toString(masterStatus->isAddressError()));
						break;
//					case fieldOffsetHfcLASysStatusAddressTestAddress:
//						HfcObject::serializedData.append(Long::toString(masterStatus->getAddressTestAddress()));
//						break;

					case fieldOffsetHfcLASysStatusSlave1SystemChecksum:
						HfcObject::serializedData.append(Hex::toString((long)slave1StatusInfo->getSystemChecksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcLASysStatusSlave1RomChecksum:
						HfcObject::serializedData.append(Hex::toString((long)slave1StatusInfo->getROM_Checksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcLASysStatusSlave1IsEepromError:
						HfcObject::serializedData.append(Bool::toString(slave1StatusInfo->isEepromError()));
						break;
					case fieldOffsetHfcLASysStatusSlave1IsBitTestError:
						HfcObject::serializedData.append(Bool::toString(slave1StatusInfo->isBitError()));
						break;
//					case fieldOffsetHfcLASysStatusSlave1BitTestAddress:
//						HfcObject::serializedData.append(Long::toString(slave1StatusInfo->getBitTestAddress()));
//						break;
					case fieldOffsetHfcLASysStatusSlave1IsAddressError:
						HfcObject::serializedData.append(Bool::toString(slave1StatusInfo->isAddressError()));
						break;
//					case fieldOffsetHfcLASysStatusSlave1AddressTestAddress:
//						HfcObject::serializedData.append(Long::toString(slave1StatusInfo->getAddressTestAddress()));
//						break;

					case fieldOffsetHfcLASysStatusSlave2SystemChecksum:
						HfcObject::serializedData.append(Hex::toString((long)slave2StatusInfo->getSystemChecksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcLASysStatusSlave2RomChecksum:
						HfcObject::serializedData.append(Hex::toString((long)slave2StatusInfo->getROM_Checksum()));
						HfcObject::serializedData.append("H");
						break;
					case fieldOffsetHfcLASysStatusSlave2IsEepromError:
						HfcObject::serializedData.append(Bool::toString(slave2StatusInfo->isEepromError()));
						break;
					case fieldOffsetHfcLASysStatusSlave2IsBitTestError:
						HfcObject::serializedData.append(Bool::toString(slave2StatusInfo->isBitError()));
						break;
//					case fieldOffsetHfcLASysStatusSlave2BitTestAddress:
//						HfcObject::serializedData.append(Long::toString(slave2StatusInfo->getBitTestAddress()));
//						break;
					case fieldOffsetHfcLASysStatusSlave2IsAddressError:
						HfcObject::serializedData.append(Bool::toString(slave2StatusInfo->isAddressError()));
						break;
//					case fieldOffsetHfcLASysStatusSlave2AddressTestAddress:
//						HfcObject::serializedData.append(Long::toString(slave2StatusInfo->getAddressTestAddress()));
//						break;

					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized LabelApplier System Status : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcLabelApplierSystemStatus::setBitError(SystemStatusInfo *sysStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		sysStatusInfo->setBitError();
	else
		sysStatusInfo->clearBitError();
}

void HfcLabelApplierSystemStatus::setAddressError(SystemStatusInfo *sysStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		sysStatusInfo->setAddressError();
	else
		sysStatusInfo->clearAddressError();
}

void HfcLabelApplierSystemStatus::setEepromError(SystemStatusInfo *sysStatusInfo, bool trueOrFalse)
{
	if(trueOrFalse == true)
		sysStatusInfo->setEepromError();
	else
		sysStatusInfo->clearEepromError();
}
