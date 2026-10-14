/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperConfig.h"
#include "HfcEvolWrapperControl.h"
#include "HfcEvolWrapperConfigLabelWidthUpdate.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperConfig::_dataType_ = 0;
std::string HfcEvolWrapperConfig::_dataTypeName_("HfcEvolWrapperConfig"); 
#endif

HfcEvolWrapperConfig::HfcEvolWrapperConfig(const char *data) : EvolWrapperConfiguration(data), sealerBeltEnabled(true), sealerBeltTempWasValid(true)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperConfig", "Constructor() 1");
}

HfcEvolWrapperConfig::HfcEvolWrapperConfig(const char *data, int len, int formatType) : EvolWrapperConfiguration(), sealerBeltEnabled(true), sealerBeltTempWasValid(true)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (Debug::isTracing("HfcEvolWrapperConfig"))
	{
		HobString msgDebug = "Constructor() 2 - formatType = ";
		msgDebug.append(Integer::toString(formatType));
		Debug::trace("HfcEvolWrapperConfig", msgDebug);
	}

	switch (formatType)
	{
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
#if 0
					case fieldOffsetEvolWrConfigIntakeMode:
						getWrapperConfig().setIntakeMode((WrapperIntakeMode::IntakeMode) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigFilmTension:
						getWrapperConfig().setFilmTension((WrFilmTension) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigFilmSelection:
						getWrapperConfig().setFilmWidthSelection((WrFilmWidthSelection) Integer::valueOf(token.c_str()));
						break;
#endif
					case fieldOffsetEvolWrConfigUpperRollWidth:
						getWrapperConfig().getTopFilmRollDimensions().setRollWidth(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigLowerRollWidth:
						getWrapperConfig().getBottomFilmRollDimensions().setRollWidth(Integer::valueOf(token.c_str()));
						break;
#if 0
					case fieldOffsetEvolWrConfigSealerBeltPower:
						getHeaterConfig().getSealerHeaterConfig().setIsEnabled(Bool::valueOf(token.c_str()));
						break;
#endif
					case fieldOffsetEvolWrConfigSealerBeltSetting:
						getHeaterConfig().getSealerHeaterConfig().setDefaultTempSetting(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSideDischargeType:
						getLabelApplierConfig().setSideDischargeType((LaSideDischarge) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigPriPlacementOffset:
						getLabelApplierConfig().setPriLabelOffsetOnPkg(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigPriRotationCalibOffset:
						getLabelApplierConfig().setPriRotationCalibrationOffset(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSecondCLAAttached:
						getLabelApplierConfig().setIsSecondCLAAttached(Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigWrapperOperation:
						getWrapperConfig().setWrapperOperation((WrOperation) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigHardwareAddOn:
						getWrapperConfig().setHardwareAddOn((WrHardwareAddOn)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSecPlacementOffset:
						getLabelApplierConfig().setSecLabelOffsetOnPkg(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigAutoRotation:
						getLabelApplierConfig().setHasAutoRotation(Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigPriWandCalibOffset:
						getLabelApplierConfig().setPriWandCalibrationOffset(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSecWandCalibOffset:
						getLabelApplierConfig().setSecWandCalibrationOffset(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSealerBeltPackageSensorType:
						getWrapperConfig().setSealerBeltPackageSensorType(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigLowerFilmPullTension:
						getWrapperConfig().setLowerFilmPullTension(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigUpperFilmPullTension:
						getWrapperConfig().setUpperFilmPullTension(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigHeaterIsUsedOffet:
						//getHeaterConfig().setFilmHeaterIsUsed(Bool::valueOf(token.c_str()));
						getHeaterConfig().getFilmHeaterConfig().setIsUsed(Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigPriRotationCalibOffset_90:
						getLabelApplierConfig().setPriRotationCalibrationOffset_90Deg(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigPriRotationCalibOffset_180:
						getLabelApplierConfig().setPriRotationCalibrationOffset_180Deg(Integer::valueOf(token.c_str()));
						break;						
					case fieldOffsetEvolWrConfigPriRotationCalibOffset_270:
						getLabelApplierConfig().setPriRotationCalibrationOffset_270Deg(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSecRotationCalibOffset_90:
						getLabelApplierConfig().setSecRotationCalibrationOffset_90Deg(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSecRotationCalibOffset_180:
						getLabelApplierConfig().setSecRotationCalibrationOffset_180Deg(Integer::valueOf(token.c_str()));
						break;						
					case fieldOffsetEvolWrConfigSecRotationCalibOffset_270:
						getLabelApplierConfig().setSecRotationCalibrationOffset_270Deg(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSecRotationCalibOffset:
						getLabelApplierConfig().setSecRotationCalibrationOffset(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigPriApplierHandAdjust:
						getLabelApplierConfig().setPriApplierHandAdjust(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetEvolWrConfigSecApplierHandAdjust:
						getLabelApplierConfig().setSecApplierHandAdjust(Integer::valueOf(token.c_str()));
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

HfcEvolWrapperConfig::~HfcEvolWrapperConfig()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperConfig::serialize(int format)const 
{
	Debug::trace("HfcEvolWrapperConfig", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case InternalFormat:
		{
			EvolWrapperConfiguration::serialize();
			EvolWrapperConfiguration::getSerializedLength();
			HfcObject::serializedData = EvolWrapperConfiguration::serializedData;
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcEvolWrConfigFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
#if 0
					case fieldOffsetEvolWrConfigIntakeMode:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getIntakeMode()));
						break;
					case fieldOffsetEvolWrConfigFilmTension:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getFilmTension()));
						break;
					case fieldOffsetEvolWrConfigFilmSelection:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getFilmWidthSelection()));
						break;
#endif
					case fieldOffsetEvolWrConfigUpperRollWidth:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getTopFilmRollDimensions().getRollWidth()));
						break;
					case fieldOffsetEvolWrConfigLowerRollWidth:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getBottomFilmRollDimensions().getRollWidth()));
						break;
#if 0
					case fieldOffsetEvolWrConfigSealerBeltPower:
						HfcObject::serializedData.append(Bool::toString(getHeaterConfig().getSealerHeaterConfig().isItEnabled()));
						break;
#endif
					case fieldOffsetEvolWrConfigSealerBeltSetting:
						HfcObject::serializedData.append(Integer::toString(getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting()));
						break;
					case fieldOffsetEvolWrConfigSideDischargeType:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getSideDischargeType()));
						break;
					case fieldOffsetEvolWrConfigPriPlacementOffset:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getPriLabelOffsetOnPkg()));
						break;
					case fieldOffsetEvolWrConfigPriRotationCalibOffset:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getPriRotationCalibrationOffset()));
						break;
					case fieldOffsetEvolWrConfigSecondCLAAttached:
						HfcObject::serializedData.append(Bool::toString(getLabelApplierConfig().getIsSecondCLAAttached()));
						break;
					case fieldOffsetEvolWrConfigWrapperOperation:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getWrapperOperation()));
						break;
					case fieldOffsetEvolWrConfigHardwareAddOn:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getHardwareAddOn()));
						break;
					case fieldOffsetEvolWrConfigSecPlacementOffset:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getSecLabelOffsetOnPkg()));
						break;
					case fieldOffsetEvolWrConfigAutoRotation:
						Debug::trace("HfcEvolWrapperConfig", "serialize():  fieldOffsetEvolWrConfigAutoRotation");
						HfcObject::serializedData.append(Bool::toString(getLabelApplierConfig().getHasAutoRotation()));
						break;
					case fieldOffsetEvolWrConfigPriWandCalibOffset:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getPriWandCalibrationOffset()));
						break;
					case fieldOffsetEvolWrConfigSecWandCalibOffset:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getSecWandCalibrationOffset()));
						break;
					case fieldOffsetEvolWrConfigSealerBeltPackageSensorType:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getSealerBeltPackageSensorType()));
						break;
					case fieldOffsetEvolWrConfigUpperFilmPullTension:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getUpperFilmPullTension()));
						break;
					case fieldOffsetEvolWrConfigLowerFilmPullTension:
						HfcObject::serializedData.append(Integer::toString(getWrapperConfig().getLowerFilmPullTension()));
						break;
					case fieldOffsetEvolWrConfigHeaterIsUsedOffet:
						Debug::trace("HfcEvolWrapperConfig", "serialize():  fieldOffsetEvolWrConfigHeaterIsUsedOffet");
						HfcObject::serializedData.append(Bool::toString(getHeaterConfig().getFilmHeaterConfig().isItUsed()));
						break;
					case fieldOffsetEvolWrConfigPriRotationCalibOffset_90:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getPriRotationCalibrationOffset_90Deg()));
						break;
					case fieldOffsetEvolWrConfigPriRotationCalibOffset_180:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getPriRotationCalibrationOffset_180Deg()));
						break;
					case fieldOffsetEvolWrConfigPriRotationCalibOffset_270:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getPriRotationCalibrationOffset_270Deg()));
						break;
					case fieldOffsetEvolWrConfigSecRotationCalibOffset_90:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getSecRotationCalibrationOffset_90Deg()));
						break;
					case fieldOffsetEvolWrConfigSecRotationCalibOffset_180:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getSecRotationCalibrationOffset_180Deg()));
						break;
					case fieldOffsetEvolWrConfigSecRotationCalibOffset_270:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getSecRotationCalibrationOffset_270Deg()));
						break;
					case fieldOffsetEvolWrConfigSecRotationCalibOffset:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getSecRotationCalibrationOffset()));
						break;
					case fieldOffsetEvolWrConfigPriApplierHandAdjust:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getPriApplierHandAdjust()));
						break;
					case fieldOffsetEvolWrConfigSecApplierHandAdjust:
						HfcObject::serializedData.append(Integer::toString(getLabelApplierConfig().getSecApplierHandAdjust()));
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			Debug::trace("HfcEvolWrapperConfig", "serialize(Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

bool HfcEvolWrapperConfig::update(HfcEvolWrapperConfig *wrConfig)
{
	bool traceOn = Debug::isTracing("HfcEvolWrapperConfig");
	HobString msgDebug("update(): ");

	bool	configChanged = false;

	short	loopCnt = 1;
	while (loopCnt < endHfcEvolWrConfigFieldOffsets)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("isReceived(").append(Integer::toString(loopCnt));
			msgDebug2.append(") = ").append(Bool::toString(wrConfig->isReceived(loopCnt)));
			Debug::trace("HfcEvolWrapperConfig", msgDebug2);
		}
		if (wrConfig->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
#if 0
				case fieldOffsetEvolWrConfigIntakeMode:
					if (getWrapperConfig().getIntakeMode() != wrConfig->getWrapperConfig().getIntakeMode())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("IntakeMode changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getIntakeMode()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().setIntakeMode(wrConfig->getWrapperConfig().getIntakeMode());
					}
					break;

				case fieldOffsetEvolWrConfigFilmTension:
					if (getWrapperConfig().getFilmTension() != wrConfig->getWrapperConfig().getFilmTension())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("FilmTension changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getFilmTension()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().setFilmTension(wrConfig->getWrapperConfig().getFilmTension());
					}
					break;

				case fieldOffsetEvolWrConfigFilmSelection:
					if (getWrapperConfig().getFilmWidthSelection() != wrConfig->getWrapperConfig().getFilmWidthSelection())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("FilmWidthSelection changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getFilmWidthSelection()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().setFilmWidthSelection(wrConfig->getWrapperConfig().getFilmWidthSelection());
					}
					break;
#endif
				case fieldOffsetEvolWrConfigUpperRollWidth:
					if (getWrapperConfig().getTopFilmRollDimensions().getRollWidth() != wrConfig->getWrapperConfig().getTopFilmRollDimensions().getRollWidth())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("UpperRollWidth changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getTopFilmRollDimensions().getRollWidth()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().getTopFilmRollDimensions().setRollWidth(wrConfig->getWrapperConfig().getTopFilmRollDimensions().getRollWidth());
					}
					break;

				case fieldOffsetEvolWrConfigLowerRollWidth:
					if (getWrapperConfig().getBottomFilmRollDimensions().getRollWidth() != wrConfig->getWrapperConfig().getBottomFilmRollDimensions().getRollWidth())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("LowerRollWidth changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getBottomFilmRollDimensions().getRollWidth()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().getBottomFilmRollDimensions().setRollWidth(wrConfig->getWrapperConfig().getBottomFilmRollDimensions().getRollWidth());
					}
					break;
#if 0
				case fieldOffsetEvolWrConfigSealerBeltPower:
					if (getHeaterConfig().getSealerHeaterConfig().isItEnabled() != wrConfig->getHeaterConfig().getSealerHeaterConfig().isItEnabled())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SealerBeltPower changed: ").append(Bool::toString(wrConfig->getHeaterConfig().getSealerHeaterConfig().isItEnabled()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getHeaterConfig().getSealerHeaterConfig().setIsEnabled(wrConfig->getHeaterConfig().getSealerHeaterConfig().isItEnabled());
					}
					break;
#endif
				case fieldOffsetEvolWrConfigSealerBeltSetting:
					if (getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting() != wrConfig->getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting())
					{
						sealerBeltTempWasValid = HfcEvolWrapperControl::isSealerBeltTempSettingValid(wrConfig->getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting());
						if (sealerBeltTempWasValid)
						{
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append("SealerBeltSetting changed: ").append(Integer::toString(wrConfig->getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting()));
								Debug::trace("HfcEvolWrapperConfig", msgDebug2);
							}
							configChanged = true;
							getHeaterConfig().getSealerHeaterConfig().setDefaultTempSetting(wrConfig->getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting());
						}
					}
					break;

				case fieldOffsetEvolWrConfigSideDischargeType:
					if (getLabelApplierConfig().getSideDischargeType() != wrConfig->getLabelApplierConfig().getSideDischargeType())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SideDischargeType changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getSideDischargeType()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setSideDischargeType(wrConfig->getLabelApplierConfig().getSideDischargeType());
					}
					break;

				case fieldOffsetEvolWrConfigPriPlacementOffset:
					if (getLabelApplierConfig().getPriLabelOffsetOnPkg() != wrConfig->getLabelApplierConfig().getPriLabelOffsetOnPkg())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("PriLabelOffset changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getPriLabelOffsetOnPkg()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setPriLabelOffsetOnPkg(wrConfig->getLabelApplierConfig().getPriLabelOffsetOnPkg());
					}
					break;

				case fieldOffsetEvolWrConfigPriRotationCalibOffset:
					if (getLabelApplierConfig().getPriRotationCalibrationOffset() != wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("PriRotationCalibOffset changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setPriRotationCalibrationOffset(wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset());
					}
					break;

				case fieldOffsetEvolWrConfigSecondCLAAttached:
					if (getLabelApplierConfig().getIsSecondCLAAttached() != wrConfig->getLabelApplierConfig().getIsSecondCLAAttached())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SecondCLAAttached changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getIsSecondCLAAttached()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setIsSecondCLAAttached(wrConfig->getLabelApplierConfig().getIsSecondCLAAttached());
					}
					break;

				case fieldOffsetEvolWrConfigWrapperOperation:
					if (getWrapperConfig().getWrapperOperation() != wrConfig->getWrapperConfig().getWrapperOperation())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("WrapperOperation changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getWrapperOperation()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().setWrapperOperation((WrOperation) wrConfig->getWrapperConfig().getWrapperOperation());
					}
					break;
				case fieldOffsetEvolWrConfigHardwareAddOn:
				{	
					if (getWrapperConfig().getHardwareAddOn() != wrConfig->getWrapperConfig().getHardwareAddOn())	
					{
						configChanged = true;					
						getWrapperConfig().setHardwareAddOn((WrHardwareAddOn) wrConfig->getWrapperConfig().getHardwareAddOn());					
					}
					//also, always update label width
					if (getLabelApplierConfig().getPriLabelWidth() != wrConfig->getLabelApplierConfig().getPriLabelWidth())
					{
						configChanged = true;
						getLabelApplierConfig().setPriLabelWidth(wrConfig->getLabelApplierConfig().getPriLabelWidth());
					}
					
					if(wrConfig->getWrapperConfig().getHardwareAddOn() == SecondPrinterApplier)
					{
						if (getLabelApplierConfig().getSecLabelWidth() != wrConfig->getLabelApplierConfig().getSecLabelWidth())
						{
							configChanged = true;
							getLabelApplierConfig().setSecLabelWidth(wrConfig->getLabelApplierConfig().getSecLabelWidth());
						}
					}
					break;
				}

				case fieldOffsetEvolWrConfigSecPlacementOffset:
					if (getLabelApplierConfig().getSecLabelOffsetOnPkg() != wrConfig->getLabelApplierConfig().getSecLabelOffsetOnPkg())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("PriLabelOffset changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getSecLabelOffsetOnPkg()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setSecLabelOffsetOnPkg(wrConfig->getLabelApplierConfig().getSecLabelOffsetOnPkg());
					}
					break;

				case fieldOffsetEvolWrConfigAutoRotation:
					if (getLabelApplierConfig().getHasAutoRotation() != wrConfig->getLabelApplierConfig().getHasAutoRotation())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("AutoRotation changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getHasAutoRotation()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setHasAutoRotation(wrConfig->getLabelApplierConfig().getHasAutoRotation());
					}
					break;

				case fieldOffsetEvolWrConfigPriWandCalibOffset:
					if (getLabelApplierConfig().getPriWandCalibrationOffset() != wrConfig->getLabelApplierConfig().getPriWandCalibrationOffset())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("PriWandCalibOffset changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getPriWandCalibrationOffset()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setPriWandCalibrationOffset(wrConfig->getLabelApplierConfig().getPriWandCalibrationOffset());
					}
					break;

				case fieldOffsetEvolWrConfigSecWandCalibOffset:
					if (getLabelApplierConfig().getSecWandCalibrationOffset() != wrConfig->getLabelApplierConfig().getSecWandCalibrationOffset())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SecWandCalibOffset changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getSecWandCalibrationOffset()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setSecWandCalibrationOffset(wrConfig->getLabelApplierConfig().getSecWandCalibrationOffset());
					}
					break;
				case fieldOffsetEvolWrConfigSealerBeltPackageSensorType:
					if (getWrapperConfig().getSealerBeltPackageSensorType() != wrConfig->getWrapperConfig().getSealerBeltPackageSensorType())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SealerBeltPackageSensorType changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getSealerBeltPackageSensorType()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().setSealerBeltPackageSensorType(wrConfig->getWrapperConfig().getSealerBeltPackageSensorType());
					}
					break;

				case fieldOffsetEvolWrConfigLowerFilmPullTension:
					if (getWrapperConfig().getLowerFilmPullTension() != wrConfig->getWrapperConfig().getLowerFilmPullTension())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("LowerFilmPullTension changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getLowerFilmPullTension()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().setLowerFilmPullTension(wrConfig->getWrapperConfig().getLowerFilmPullTension());
					}
					break;

				case fieldOffsetEvolWrConfigUpperFilmPullTension:
					if (getWrapperConfig().getUpperFilmPullTension() != wrConfig->getWrapperConfig().getUpperFilmPullTension())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("UpperFilmPullTension changed: ").append(Integer::toString(wrConfig->getWrapperConfig().getUpperFilmPullTension()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getWrapperConfig().setUpperFilmPullTension(wrConfig->getWrapperConfig().getUpperFilmPullTension());
					}
					break;

				//
				case fieldOffsetEvolWrConfigHeaterIsUsedOffet:
					if ( getHeaterConfig().getFilmHeaterConfig().isItUsed() != wrConfig->getHeaterConfig().getFilmHeaterConfig().isItUsed())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("WrConfigHeaterIsUsed changed: ").append(Integer::toString(wrConfig->getHeaterConfig().getFilmHeaterConfig().isItUsed()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getHeaterConfig().getFilmHeaterConfig().setIsUsed(wrConfig->getHeaterConfig().getFilmHeaterConfig().isItUsed());
					}
					
					break;
				//
				case fieldOffsetEvolWrConfigPriRotationCalibOffset_90:
					if (getLabelApplierConfig().getPriRotationCalibrationOffset_90Deg() != wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_90Deg())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("PriRotationCalibOffset_90 changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_90Deg()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setPriRotationCalibrationOffset_90Deg(wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_90Deg());
					}
					break;

				case fieldOffsetEvolWrConfigPriRotationCalibOffset_180:
					if (getLabelApplierConfig().getPriRotationCalibrationOffset_180Deg() != wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_180Deg())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("PriRotationCalibOffset_180 changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_180Deg()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setPriRotationCalibrationOffset_180Deg(wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_180Deg());
					}
					break;
				
				case fieldOffsetEvolWrConfigPriRotationCalibOffset_270:
					if (getLabelApplierConfig().getPriRotationCalibrationOffset_270Deg() != wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_270Deg())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("PriRotationCalibOffset_270 changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_270Deg()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setPriRotationCalibrationOffset_270Deg(wrConfig->getLabelApplierConfig().getPriRotationCalibrationOffset_270Deg());
					}
					break;
				case fieldOffsetEvolWrConfigSecRotationCalibOffset_90:
					if (getLabelApplierConfig().getSecRotationCalibrationOffset_90Deg() != wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_90Deg())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SecRotationCalibOffset_90 changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_90Deg()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setSecRotationCalibrationOffset_90Deg(wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_90Deg());
					}
					break;

				case fieldOffsetEvolWrConfigSecRotationCalibOffset_180:
					if (getLabelApplierConfig().getSecRotationCalibrationOffset_180Deg() != wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_180Deg())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SecRotationCalibOffset_180 changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_180Deg()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setSecRotationCalibrationOffset_180Deg(wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_180Deg());
					}
					break;
				
				case fieldOffsetEvolWrConfigSecRotationCalibOffset_270:
					if (getLabelApplierConfig().getSecRotationCalibrationOffset_270Deg() != wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_270Deg())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SecRotationCalibOffset_270 changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_270Deg()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setSecRotationCalibrationOffset_270Deg(wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset_270Deg());
					}
					break;
				case fieldOffsetEvolWrConfigSecRotationCalibOffset:	
					if (getLabelApplierConfig().getSecRotationCalibrationOffset() != wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SecRotationCalibrationOffset changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setSecRotationCalibrationOffset(wrConfig->getLabelApplierConfig().getSecRotationCalibrationOffset());
					}
					break;
				case fieldOffsetEvolWrConfigPriApplierHandAdjust:	
					if (getLabelApplierConfig().getPriApplierHandAdjust() != wrConfig->getLabelApplierConfig().getPriApplierHandAdjust())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("PriApplierHandAdjust changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getPriApplierHandAdjust()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setPriApplierHandAdjust(wrConfig->getLabelApplierConfig().getPriApplierHandAdjust());
					}
					break;
				case fieldOffsetEvolWrConfigSecApplierHandAdjust:	
					if (getLabelApplierConfig().getSecApplierHandAdjust() != wrConfig->getLabelApplierConfig().getSecApplierHandAdjust())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("SecApplierHandAdjust changed: ").append(Integer::toString(wrConfig->getLabelApplierConfig().getSecApplierHandAdjust()));
							Debug::trace("HfcEvolWrapperConfig", msgDebug2);
						}
						configChanged = true;
						getLabelApplierConfig().setSecApplierHandAdjust(wrConfig->getLabelApplierConfig().getSecApplierHandAdjust());
					}
					break;
				default:
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("???? ");
						Debug::trace("HfcEvolWrapperConfig", msgDebug2);
					}
					break;
				}
			}
		}
		loopCnt++;
	}
	return configChanged;
}

bool HfcEvolWrapperConfig::update(HfcEvolWrapperControl *wrControl)
{
	bool traceOn = Debug::isTracing("HfcEvolWrapperConfig");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "HfcEvolWrapperConfig::update(*wrControl)";
		Debug::trace("HfcEvolWrapperConfig", msgDebug);
		msgDebug.append("):  ");
	}
	bool	configChanged = false;

	short	loopCnt = 1;

	EvolWrapperGeneralConfig &wrapperConfig = getWrapperConfig();

	//	We will not 'update' the Config with the Control message.
	//	We only want to know if the Config values (UpperRollWidth & LowerRollWidth) have changed.
	//	If the Config did not change, then the Control values changed.
	while (loopCnt < endHfcEvolWrapperControlFieldOffsets)
	{
		if (wrControl->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetEvolWrUpperRollWidth:
				{
					WrapperFilmWidthDimensions &upperRoll = wrapperConfig.getTopFilmRollDimensions();
					if (upperRoll.getRollWidth() != wrControl->getUpperRollWidth())
					{
						Debug::trace("HfcEvolWrapperConfig", "UpperRollWidth changed. ");
						configChanged = true;
						upperRoll.setRollWidth(wrControl->getUpperRollWidth());
					}
				}
				break;

				case fieldOffsetEvolWrLowerRollWidth:
				{
					WrapperFilmWidthDimensions &lowerRoll = wrapperConfig.getBottomFilmRollDimensions();
					if (lowerRoll.getRollWidth() != wrControl->getLowerRollWidth())
					{
						Debug::trace("HfcEvolWrapperConfig", "LowerRollWidth changed.");
						configChanged = true;
						lowerRoll.setRollWidth(wrControl->getLowerRollWidth());
					}
				}
				break;

				default:
					break;
			}
		}
		loopCnt++;
	}

	if (traceOn)
		Debug::trace("HfcEvolWrapperConfig", "update():  configChanged = " + HobString(Bool::toString(configChanged)));
	return configChanged;
}

bool HfcEvolWrapperConfig::update(HfcEvolWrapperConfigLabelWidthUpdate *pWrWidthUpdate)
{
	bool configChanged = false;
	
	if(pWrWidthUpdate)
	{
		if(pWrWidthUpdate->isFirstPrinter())
		{
			if (getLabelApplierConfig().getPriLabelWidth() != pWrWidthUpdate->getLabelWidth())
			{			
				setPriLabelWidth(pWrWidthUpdate->getLabelWidth());
				configChanged = true;
			}
		}
		else
		{
			if(pWrWidthUpdate->isSecondPrinter())
			{
				if (getLabelApplierConfig().getSecLabelWidth() != pWrWidthUpdate->getLabelWidth())
				{			
					setSecLabelWidth(pWrWidthUpdate->getLabelWidth());
					configChanged = true;
				}
			}
		}
	}
	return configChanged;
}

bool HfcEvolWrapperConfig::isSealerBeltEnabled() const
{
	return sealerBeltEnabled;
}

void HfcEvolWrapperConfig::setIsSealerBeltEnabled(bool value)
{
	sealerBeltEnabled = value;
}

void HfcEvolWrapperConfig::clearIsSealerBeltEnabled()
{
	sealerBeltEnabled = false;
}

bool HfcEvolWrapperConfig::wasSealerBeltTempValid() const
{
	return sealerBeltTempWasValid;
}

WrHardwareAddOn  HfcEvolWrapperConfig::getHardwareAddOn() const
{
	return getWrapperConfig().getHardwareAddOn();
}
