/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperConfig.h"
#include "HobTokenizerCSV.h"
#include "HobDateTime.h"
#include "Bool.h"
#include "Long.h"
#include "UInteger.h"
#include "HfcWrapperPackCodeInfo.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperConfig::dataType = 0;
std::string HfcWrapperConfig::dataTypeName("HfcWrapperConfig"); 
#endif

HfcWrapperConfig::HfcWrapperConfig() : WrapperConfiguration()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

HfcWrapperConfig::HfcWrapperConfig(const char *data) : WrapperConfiguration(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

HfcWrapperConfig::HfcWrapperConfig(const char *data, int len, int formatType) : WrapperConfiguration()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	switch (formatType)
	{
		case BrowserFormat:
		{
			bool isEnabled;
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetWrConfigType:
						setConfigType((WrConfigType)Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigPauseTimeout:
						setPauseTimeout(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigIdleTimeout:
						setIdleTimeout(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigFilmTension:
						setFilmTension((WrFilmTension) Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigPackWidth:
						setPackWidth((WrPackWidth) Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigPackageGrandTotal:
						setPackageGrandTotal(Long::valueOf(token));
						break;
					case fieldOffsetWrConfigWrapperCycleTotal:
						setWrapperCycleTotal(Long::valueOf(token));
						break;
					case fieldOffsetWrConfigElevatorCollapseTotal:
						setElevatorCollapseTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigIntakeJamTotal:
						setIntakeJamTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigLengthJamTotal:
						setLengthJamTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigWidthJamTotal:
						setWidthJamTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigCoverOpenTotal:
						setCoverOpenTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigSealerBeltOpenTotal:
						setSealerBeltOpenTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigPackageTooTallTotal:
						setPackageTooTallTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigPusherHomeErrorTotal:
						setPusherHomeErrorTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigFilmPullErrorTotal:
						setFilmPullErrorTotal(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigCelsiusEnable:
						if (Integer::valueOf(token) > 0)
							isEnabled = true;
						else
							isEnabled = false;
						isEnabled ? setCelsiusEnable() : clearCelsiusEnable();
						break;
					case fieldOffsetWrConfigWideFilmTestEnable:
						if (Integer::valueOf(token) > 0)
							isEnabled = true;
						else
							isEnabled = false;
						isEnabled ? setWideFilmTestEnable() : clearWideFilmTestEnable();
						break;
					case fieldOffsetWrConfigNarrowFilmTestEnable:
						if (Integer::valueOf(token) > 0)
							isEnabled = true;
						else
							isEnabled = false;
						isEnabled ? setNarrowFilmTestEnable() : clearNarrowFilmTestEnable();
						break;
					case fieldOffsetWrConfigHeaterEnable:
						if (Integer::valueOf(token) > 0)
							isEnabled = true;
						else
							isEnabled = false;
						isEnabled ? setHeaterEnable() : clearHeaterEnable();
						break;
					case fieldOffsetWrConfigHeaterSetting:
						setHeaterSetting(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigPackageLengthAdjustment:
						setPackageLengthAdjustment(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigPackageWidthAdjustment:
						setPackageWidthAdjustment(Integer::valueOf(token));
						break;
					case fieldOffsetWrConfigErrorTotalsDate:
						setErrorTotalsDate(Long::valueOf(token));
						break;
					case fieldOffsetWrConfigPackCodeTotalsDate:
						setPackCodeTotalsDate(Long::valueOf(token));
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWrapperConfig::~HfcWrapperConfig()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void HfcWrapperConfig::init()
{
	hasChanged = false;
	widthHasChanged = false;
	lengthHasChanged = false;
	gripperOnHasChanged = false;
}

const char *HfcWrapperConfig::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			const char *msgdata;
			msgdata = WrapperConfiguration::serialize();
			int msglen;
			msglen = WrapperConfiguration::getSerializedLength();

			HfcObject::serializedData = WrapperConfiguration::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
			break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcWrapperConfigFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetWrConfigType:
			 			HfcObject::serializedData.append(UInteger::toString((INT32) getConfigType()));
						break;
					case fieldOffsetWrConfigPauseTimeout:
			 			HfcObject::serializedData.append(UInteger::toString(getPauseTimeout()));
						break;
					case fieldOffsetWrConfigIdleTimeout:
			 			HfcObject::serializedData.append(UInteger::toString(getIdleTimeout()));
						break;
					case fieldOffsetWrConfigFilmTension:
			 			HfcObject::serializedData.append(UInteger::toString((INT32) getFilmTension()));
						break;
					case fieldOffsetWrConfigPackWidth:
			 			HfcObject::serializedData.append(UInteger::toString((INT32) getPackWidth()));
						break;
					case fieldOffsetWrConfigPackageGrandTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getPackageGrandTotal()));
						break;
					case fieldOffsetWrConfigWrapperCycleTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getWrapperCycleTotal()));
						break;
					case fieldOffsetWrConfigElevatorCollapseTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getElevatorCollapseTotal()));
						break;
					case fieldOffsetWrConfigIntakeJamTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getIntakeJamTotal()));
						break;
					case fieldOffsetWrConfigLengthJamTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getLengthJamTotal()));
						break;
					case fieldOffsetWrConfigWidthJamTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getWidthJamTotal()));
						break;
					case fieldOffsetWrConfigCoverOpenTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getCoverOpenTotal()));
						break;
					case fieldOffsetWrConfigSealerBeltOpenTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getSealerBeltOpenTotal()));
						break;
					case fieldOffsetWrConfigPackageTooTallTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getPackageTooTallTotal()));
						break;
					case fieldOffsetWrConfigPusherHomeErrorTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getPusherHomeErrorTotal()));
						break;
					case fieldOffsetWrConfigFilmPullErrorTotal:
			 			HfcObject::serializedData.append(UInteger::toString(getFilmPullErrorTotal()));
						break;
					case fieldOffsetWrConfigCelsiusEnable:
			 			HfcObject::serializedData.append(Bool::toString(isCelsiusEnable()));
						break;
					case fieldOffsetWrConfigWideFilmTestEnable:
			 			HfcObject::serializedData.append(Bool::toString(isWideFilmTestEnable()));
						break;
					case fieldOffsetWrConfigNarrowFilmTestEnable:
			 			HfcObject::serializedData.append(Bool::toString(isNarrowFilmTestEnable()));
						break;
					case fieldOffsetWrConfigHeaterEnable:
			 			HfcObject::serializedData.append(Bool::toString(isHeaterEnable()));
						break;
					case fieldOffsetWrConfigHeaterSetting:
			 			HfcObject::serializedData.append(UInteger::toString(getHeaterSetting()));
						break;
					case fieldOffsetWrConfigPackageLengthAdjustment:
			 			HfcObject::serializedData.append(UInteger::toString(getPackageLengthAdjustment()));
						break;
					case fieldOffsetWrConfigPackageWidthAdjustment:
			 			HfcObject::serializedData.append(UInteger::toString(getPackageWidthAdjustment()));
						break;
					case fieldOffsetWrConfigErrorTotalsDate:
			 			HfcObject::serializedData.append(UInteger::toString(getErrorTotalsDate()));
						break;
					case fieldOffsetWrConfigPackCodeTotalsDate:
			 			HfcObject::serializedData.append(UInteger::toString(getPackCodeTotalsDate()));
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

void HfcWrapperConfig::update(HfcWrapperPackCodeInfo *newPackCode)
{
	bool traceOn = Debug::isTracing("HfcWrapperConfig");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "HfcWrapperConfig::update(*newPackCode)";
		Debug::trace("HfcWrapperConfig", msgDebug);
		msgDebug.append("):  ");
	}

	if (!newPackCode)
	{
		if (traceOn)
		{
			HobString msgDebug2 = "newPackCode is 0. Exiting!!!";
			Debug::trace("HfcWrapperConfig", msgDebug + msgDebug2);
		}
		return;
	}

	WrPackCode packCodeNum = newPackCode->getPackCode();
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("packCodeNum = ").append(Integer::toString(packCodeNum));
		Debug::trace("HfcWrapperConfig", msgDebug2);
	}

	if (packCodeNum > PackCode11)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("(packCodeNum > PackCode11) - We definitely have a problem here!!!");
			Debug::trace("HfcWrapperConfig", msgDebug2);
		}
	}
	else
	{
		WrapperPackCodeInfo currentPackCode = getWrapperPackCodeInfo(packCodeNum);
		short loopCnt = 1;
		while (loopCnt < endHfcWrapperPackCodeFieldOffsets)
		{
			if(newPackCode->isReceived(loopCnt))
			{
				switch (loopCnt)
				{
					case fieldOffsetWrPackCodeValue:
					//	This won't change.
						break;
					case fieldOffsetWrPackCodeDescription:
					{
						UINT8 currentDesc[PackCodeDescriptionLength]; 
						UINT8 newDesc[PackCodeDescriptionLength]; 
			 			currentPackCode.getDescription(currentDesc);
			 			newPackCode->getDescription(newDesc);
						int i = 0;
						bool descChanged = false;
						while ((currentDesc[i] != '\0') && (newDesc[i] != '\0') && (descChanged == false))
						{
							if (currentDesc[i] != newDesc[i])
								descChanged = true;
							i++;
						}

						if (descChanged)
						{
							currentPackCode.setDescription(newDesc);
							hasChanged = true;
						}
						break;
					}

					case fieldOffsetWrPackCodeWidthMin:
						if (currentPackCode.getPackSizeMinWidth() != newPackCode->getPackSizeMinWidth())
						{
							currentPackCode.setPackSizeMinWidth(newPackCode->getPackSizeMinWidth());
							widthHasChanged = true;
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeWidthMax:
						if (currentPackCode.getPackSizeMaxWidth() != newPackCode->getPackSizeMaxWidth())
						{
							currentPackCode.setPackSizeMaxWidth(newPackCode->getPackSizeMaxWidth());
							widthHasChanged = true;
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeLengthMin:
						if (currentPackCode.getPackSizeMinLength() != newPackCode->getPackSizeMinLength())
						{
							currentPackCode.setPackSizeMinLength(newPackCode->getPackSizeMinLength());
							lengthHasChanged = true;
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeLengthMax:
						if (currentPackCode.getPackSizeMaxLength() != newPackCode->getPackSizeMaxLength())
						{
							currentPackCode.setPackSizeMaxLength(newPackCode->getPackSizeMaxLength());
							lengthHasChanged = true;
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeSideClampOnValue:
						if (currentPackCode.getSideClampOnValue() != newPackCode->getSideClampOnValue())
						{
							currentPackCode.setSideClampOnValue(newPackCode->getSideClampOnValue());
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeSideClampOffValue:
						if (currentPackCode.getSideClampOffValue() != newPackCode->getSideClampOffValue())
						{
							currentPackCode.setSideClampOffValue(newPackCode->getSideClampOffValue());
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeGripperOnValue:
						if (currentPackCode.getGripperOnValue() != newPackCode->getGripperOnValue())
						{
							currentPackCode.setGripperOnValue(newPackCode->getGripperOnValue());
							gripperOnHasChanged = true;
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeGripperOffValue:
						if (currentPackCode.getGripperOffValue() != newPackCode->getGripperOffValue())
						{
							currentPackCode.setGripperOffValue(newPackCode->getGripperOffValue());
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeKnifeOffValue:
						if (currentPackCode.getKnifeOnValue() != newPackCode->getKnifeOnValue())
						{
							currentPackCode.setKnifeOnValue(newPackCode->getKnifeOnValue());
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeSlideBlock:
						if (currentPackCode.isSlideBlock() != newPackCode->isSlideBlock())
						{
							newPackCode->isSlideBlock() ? currentPackCode.setSlideBlock() : currentPackCode.clearSlideBlock();
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodePackageBooster:
						if (currentPackCode.isBooster() != newPackCode->isBooster())
						{
							newPackCode->isBooster() ? currentPackCode.setBooster() : currentPackCode.clearBooster();
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeFilmWidth:
						if (currentPackCode.getPackWidth() != newPackCode->getPackWidth())
						{
							currentPackCode.setPackWidth((WrPackWidth)newPackCode->getPackWidth());
							hasChanged = true;
						}
						break;
					case fieldOffsetWrPackCodeFilmLength:
						if (currentPackCode.getPackLength() != newPackCode->getPackLength())
						{
							currentPackCode.setPackLength((WrPackLength)newPackCode->getPackLength());
							hasChanged = true;
						}
						break;

					default:
						break;
				}
			}
			loopCnt++;
		}
		setWrapperPackCodeInfo(currentPackCode);
		checkToAdjustAdjacentPackCodes(packCodeNum);
	}
}

bool HfcWrapperConfig::update(HfcWrapperConfig &wrConfig)
{
	bool rv = false;

	if (getPauseTimeout() != wrConfig.getPauseTimeout())
	{
		setPauseTimeout(wrConfig.getPauseTimeout());
		rv = true;
	}
	if (getIdleTimeout() != wrConfig.getIdleTimeout())
	{
		setIdleTimeout(wrConfig.getIdleTimeout());
		rv = true;
	}
	if (getFilmTension() != wrConfig.getFilmTension())
	{
		setFilmTension(wrConfig.getFilmTension());
		rv = true;
	}
	if (getPackWidth() != wrConfig.getPackWidth())
	{
		setPackWidth(wrConfig.getPackWidth());
		rv = true;
	}
	if (getPackageGrandTotal() != wrConfig.getPackageGrandTotal())
	{
		setPackageGrandTotal(wrConfig.getPackageGrandTotal());
		rv = true;
	}
	if (getWrapperCycleTotal() != wrConfig.getWrapperCycleTotal())
	{
		setWrapperCycleTotal(wrConfig.getWrapperCycleTotal());
		rv = true;
	}
	if (getElevatorCollapseTotal() != wrConfig.getElevatorCollapseTotal())
	{
		setElevatorCollapseTotal(wrConfig.getElevatorCollapseTotal());
		rv = true;
	}
	if (getIntakeJamTotal() != wrConfig.getIntakeJamTotal())
	{
		setIntakeJamTotal(wrConfig.getIntakeJamTotal());
		rv = true;
	}
	if (getLengthJamTotal() != wrConfig.getLengthJamTotal())
	{
		setLengthJamTotal(wrConfig.getLengthJamTotal());
		rv = true;
	}
	if (getWidthJamTotal() != wrConfig.getWidthJamTotal())
	{
		setWidthJamTotal(wrConfig.getWidthJamTotal());
		rv = true;
	}
	if (getCoverOpenTotal() != wrConfig.getCoverOpenTotal())
	{
		setCoverOpenTotal(wrConfig.getCoverOpenTotal());
		rv = true;
	}
	if (getSealerBeltOpenTotal() != wrConfig.getSealerBeltOpenTotal())
	{
		setSealerBeltOpenTotal(wrConfig.getSealerBeltOpenTotal());
		rv = true;
	}
	if (getPackageTooTallTotal() != wrConfig.getPackageTooTallTotal())
	{
		setPackageTooTallTotal(wrConfig.getPackageTooTallTotal());
		rv = true;
	}
	if (getPusherHomeErrorTotal() != wrConfig.getPusherHomeErrorTotal())
	{
		setPusherHomeErrorTotal(wrConfig.getPusherHomeErrorTotal());
		rv = true;
	}
	if (getFilmPullErrorTotal() != wrConfig.getFilmPullErrorTotal())
	{
		setFilmPullErrorTotal(wrConfig.getFilmPullErrorTotal());
		rv = true;
	}
	if (getCelsiusEnable() != wrConfig.getCelsiusEnable())
	{
		setCelsiusEnable(wrConfig.getCelsiusEnable());
		rv = true;
	}
	if (getWideFilmTestEnable() != wrConfig.getWideFilmTestEnable())
	{
		setWideFilmTestEnable(wrConfig.getWideFilmTestEnable());
		rv = true;
	}
	if (getNarrowFilmTestEnable() != wrConfig.getNarrowFilmTestEnable())
	{
		setNarrowFilmTestEnable(wrConfig.getNarrowFilmTestEnable());
		rv = true;
	}
	if (getHeaterEnable() != wrConfig.getHeaterEnable())
	{
		setHeaterEnable(wrConfig.getHeaterEnable());
		rv = true;
	}
	if (getHeaterSetting() != wrConfig.getHeaterSetting())
	{
		setHeaterSetting(wrConfig.getHeaterSetting());
		rv = true;
	}
	if (getPackageLengthAdjustment() != wrConfig.getPackageLengthAdjustment())
	{
		setPackageLengthAdjustment(wrConfig.getPackageLengthAdjustment());
		rv = true;
	}
	if (getPackageWidthAdjustment() != wrConfig.getPackageWidthAdjustment())
	{
		setPackageWidthAdjustment(wrConfig.getPackageWidthAdjustment());
		rv = true;
	}
	if (getErrorTotalsDate() != wrConfig.getErrorTotalsDate())
	{
		setErrorTotalsDate(wrConfig.getErrorTotalsDate());
		rv = true;
	}
	if (getPackCodeTotalsDate() != wrConfig.getPackCodeTotalsDate())
	{
		setPackCodeTotalsDate(wrConfig.getPackCodeTotalsDate());
		rv = true;
	}

	return rv;
}

void HfcWrapperConfig::resetErrorTotals()
{
	HobDateTime todaysDate;

	setElevatorCollapseTotal(0);
	setIntakeJamTotal(0);
	setLengthJamTotal(0);
	setWidthJamTotal(0);
	setCoverOpenTotal(0);
	setSealerBeltOpenTotal(0);
	setPackageTooTallTotal(0);
	setPusherHomeErrorTotal(0);
	setFilmPullErrorTotal(0);
	setErrorTotalsDate(todaysDate.getLongGregDate(Configuration::getInstance()->getDateDisplayFormat()));
}

void HfcWrapperConfig::resetPackageTotals()
{
	HobDateTime todaysDate;

	WrapperPackCodeInfo packCode;
	int 		packCodeNum;

	for (packCodeNum = PackCode0; packCodeNum < NumberPackCodes; packCodeNum++)
	{
		packCode = getWrapperPackCodeInfo((WrPackCode) packCodeNum);

		//	Package counts
		packCode.setPackageTotal(0);
		//	Jam counts
		packCode.setJamTotal(0);

		setWrapperPackCodeInfo(packCode);
	}
	setPackCodeTotalsDate(todaysDate.getLongGregDate(Configuration::getInstance()->getDateDisplayFormat()));
}

bool HfcWrapperConfig::itHasChanged()
{
	return hasChanged;
}

void HfcWrapperConfig::clearHasChanged()
{
	hasChanged = false;
}

void HfcWrapperConfig::showPackCodeNames()
{
	UINT8 packCodeName[PackCodeDescriptionLength]; 

	if (Debug::isTracing("WrapperPackCodeNames"))
	{
		Debug::trace("WrapperPackCodeNames", "Begin showPackCodeNames()");
		for (int packCodeNum = PackCode0; packCodeNum <= PackCode11; packCodeNum++)
		{
			WrapperPackCodeInfo packCodeInfo = getWrapperPackCodeInfo((WrPackCode) packCodeNum);

			//	Pack code name/description
			packCodeInfo.getDescription(packCodeName);
//			HobString temp((const char*) packCodeName);
			Debug::trace("WrapperPackCodeNames", "PackCode" + HobString(Integer::toString(packCodeNum)) + "    Name: " + HobString((const char*) packCodeName));
		}
		Debug::trace("WrapperPackCodeNames", "End showPackCodeNames()");
	}
}

void HfcWrapperConfig::checkToAdjustAdjacentPackCodes(WrPackCode packCodeNum)
{
	if ((PackCode0 <= packCodeNum) && (packCodeNum < PackCode6))
	{
		if (widthHasChanged == true)
		{
			updatePackCodeWidths(packCodeNum);
			widthHasChanged = false;
		}

		if (lengthHasChanged == true)
		{
			updatePackCodeLengths(packCodeNum);
			lengthHasChanged = false;
		}
	}

	if (gripperOnHasChanged)
	{
		changeGripperOnForAllPackCodes(packCodeNum);
		gripperOnHasChanged = false;
	}
}

void HfcWrapperConfig::updatePackCodeWidths(WrPackCode packCodeNum)
{
	WrapperPackCodeInfo packCode = getWrapperPackCodeInfo(packCodeNum);

	switch (packCodeNum)
	{
		case PackCode0:
		case PackCode2:
		case PackCode4:
			updateNarrowFilmMin(packCode.getPackSizeMinWidth());
			updateWideFilmMax(packCode.getPackSizeMinWidth() - 1);
			break;
		case PackCode1:
		case PackCode3:
		case PackCode5:
			updateNarrowFilmMin(packCode.getPackSizeMinWidth() + 1);
			updateWideFilmMax(packCode.getPackSizeMinWidth());
			break;
		default:
			break;
	}
}


void HfcWrapperConfig::updatePackCodeLengths(WrPackCode packCodeNum)
{
	WrapperPackCodeInfo packCode = getWrapperPackCodeInfo(packCodeNum);

	switch (packCodeNum)
	{
		case PackCode0:
		case PackCode1:
			updateShortFilmMin(packCode.getPackSizeMinLength());
			updateMediumFilmMax(packCode.getPackSizeMinLength() - 1);
			break;
		case PackCode2:
		case PackCode3:
			updateMediumFilmMax(packCode.getPackSizeMaxLength());
			updateShortFilmMin(packCode.getPackSizeMaxLength() + 1);
			updateMediumFilmMin(packCode.getPackSizeMinLength());
			updateLongFilmMax(packCode.getPackSizeMinLength() - 1);
			break;
		case PackCode4:
		case PackCode5:
			updateLongFilmMax(packCode.getPackSizeMaxLength());
			updateMediumFilmMin(packCode.getPackSizeMaxLength() + 1);
			break;
		default:
			break;
	}
}


void HfcWrapperConfig::changeGripperOnForAllPackCodes(WrPackCode packCodeNum)
{
	short gripperOnValue = getWrapperPackCodeInfo(packCodeNum).getGripperOnValue();

	for (int index = PackCode0; index < UndefinedPackCode; index++)
	{
		WrapperPackCodeInfo thisPackCode = getWrapperPackCodeInfo((WrPackCode) index);
		thisPackCode.setGripperOnValue((UINT8)gripperOnValue);
		setWrapperPackCodeInfo(thisPackCode);
	}
}

void HfcWrapperConfig::updateWideFilmMax(int value)
{
	WrapperPackCodeInfo packCode1 = getWrapperPackCodeInfo(PackCode1);
	WrapperPackCodeInfo packCode3 = getWrapperPackCodeInfo(PackCode3);
	WrapperPackCodeInfo packCode5 = getWrapperPackCodeInfo(PackCode5);

	packCode1.setPackSizeMaxWidth(value);
	packCode3.setPackSizeMaxWidth(value);
	packCode5.setPackSizeMaxWidth(value);

	setWrapperPackCodeInfo(packCode1);
	setWrapperPackCodeInfo(packCode3);
	setWrapperPackCodeInfo(packCode5);
}

void HfcWrapperConfig::updateNarrowFilmMin(int value)
{
	WrapperPackCodeInfo packCode0 = getWrapperPackCodeInfo(PackCode0);
	WrapperPackCodeInfo packCode2 = getWrapperPackCodeInfo(PackCode2);
	WrapperPackCodeInfo packCode4 = getWrapperPackCodeInfo(PackCode4);

	packCode0.setPackSizeMinWidth(value);
	packCode2.setPackSizeMinWidth(value);
	packCode4.setPackSizeMinWidth(value);

	setWrapperPackCodeInfo(packCode0);
	setWrapperPackCodeInfo(packCode2);
	setWrapperPackCodeInfo(packCode4);
}

void HfcWrapperConfig::updateLongFilmMax(int value)
{
	WrapperPackCodeInfo packCode4 = getWrapperPackCodeInfo(PackCode4);
	WrapperPackCodeInfo packCode5 = getWrapperPackCodeInfo(PackCode5);

	packCode4.setPackSizeMaxLength(value);
	packCode5.setPackSizeMaxLength(value);

	setWrapperPackCodeInfo(packCode4);
	setWrapperPackCodeInfo(packCode5);
}

void HfcWrapperConfig::updateMediumFilmMin(int value)
{
	WrapperPackCodeInfo packCode2 = getWrapperPackCodeInfo(PackCode2);
	WrapperPackCodeInfo packCode3 = getWrapperPackCodeInfo(PackCode3);

	packCode2.setPackSizeMinLength(value);
	packCode3.setPackSizeMinLength(value);

	setWrapperPackCodeInfo(packCode2);
	setWrapperPackCodeInfo(packCode3);
}

void HfcWrapperConfig::updateMediumFilmMax(int value)
{
	WrapperPackCodeInfo packCode2 = getWrapperPackCodeInfo(PackCode2);
	WrapperPackCodeInfo packCode3 = getWrapperPackCodeInfo(PackCode3);

	packCode2.setPackSizeMaxLength(value);
	packCode3.setPackSizeMaxLength(value);

	setWrapperPackCodeInfo(packCode2);
	setWrapperPackCodeInfo(packCode3);
}

void HfcWrapperConfig::updateShortFilmMin(int value)
{
	WrapperPackCodeInfo packCode0 = getWrapperPackCodeInfo(PackCode0);
	WrapperPackCodeInfo packCode1 = getWrapperPackCodeInfo(PackCode1);

	packCode0.setPackSizeMinLength(value);
	packCode1.setPackSizeMinLength(value);

	setWrapperPackCodeInfo(packCode0);
	setWrapperPackCodeInfo(packCode1);
}

void HfcWrapperConfig::adjustPackCodeWidthsLengths(WrPackCode packCodeNum)
{
	WrapperPackCodeInfo changedPackCode = getWrapperPackCodeInfo(packCodeNum);
}

