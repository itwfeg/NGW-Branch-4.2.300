/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperSettings.h"
#include "HfcEvolWrapperConfig.h"
#include "HfcEvolWrapperControl.h"
#include "HobTokenizerCSV.h"

#include "Bool.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperSettings::_dataType_ = 0;
std::string HfcEvolWrapperSettings::_dataTypeName_("HfcEvolWrapperSettings"); 
#endif

HfcEvolWrapperSettings::HfcEvolWrapperSettings(const char *data) : EvolWrapperSettings(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcEvolWrapperSettings", "Constructor (const char *data)");
}

HfcEvolWrapperSettings::HfcEvolWrapperSettings(HfcEvolWrapperConfig *wrConfig) : EvolWrapperSettings()

{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (!wrConfig)
		return;

	Debug::trace("HfcEvolWrapperSettings", "Constructor (HfcEvolWrapperConfig *wrConfig)");
	wrConfig->show();

	EvolWrapperGeneralConfig wrGenConfig = wrConfig->getWrapperConfig();

	setFilmTensionFrontToBack(wrGenConfig.getFilmTensionFrontToBack());
	setFilmTensionRightToLeft(wrGenConfig.getFilmTensionRightToLeft());

	setUpperRollWidth(wrGenConfig.getTopFilmRollDimensions().getRollWidth());
	setLowerRollWidth(wrGenConfig.getBottomFilmRollDimensions().getRollWidth());

	EvolWrapperHeaterConfig heaterCfg = wrConfig->getHeaterConfig();
	EvolWrapperTemperatureInfo tempInfo = getTemperatureInfo();			//	Returns a const, so we need to set it afterwards.

	HeaterConfiguration sealerHeater = heaterCfg.getSealerHeaterConfig();
	tempInfo.setSealerBeltSetting(sealerHeater.getDefaultTempSetting());

	HeaterConfiguration filmHeater = heaterCfg.getFilmHeaterConfig();
	tempInfo.setFilmSetting(filmHeater.getDefaultTempSetting());

	setTemperatureInfo(tempInfo);

	show();
	Debug::trace("HfcEvolWrapperSettings", "Constructor (HfcEvolWrapperConfig *wrConfig)  --  End");
}

HfcEvolWrapperSettings::~HfcEvolWrapperSettings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

void HfcEvolWrapperSettings::show()
{
	HobString myString("");
	HobString myValue("");
	HobString myloop("");
	HobString mySpace("  ");

	EvolWrapperTemperatureInfo tempInfo = getTemperatureInfo();

	if (Debug::isTracing("HfcEvolWrapperSettings"))

	{
		short loopCount = 1;
		while (loopCount < endHfcEvolWrSettingsFieldOffsets)
		{
			myloop = Integer::toString(loopCount);
			switch (loopCount)
			{
				case fieldOffsetEvolWrSettingsIntakeMode:
				{
					myString = "IntakeMode";
					myValue = Integer::toString(getIntakeMode().getIntakeMode());
					break;
				}
				case fieldOffsetEvolWrSettingsFilmTension:
				{
					myString = "FilmTension";
					myValue = Integer::toString(getFilmTension());
					break;
				}
				case fieldOffsetEvolWrSettingsFrontToBackFilmTension:
				{
					myString = "FilmTensionFrontToBack";
					myValue = Integer::toString(getFilmTensionFrontToBack());
					break;
				}
				case fieldOffsetEvolWrSettingsRightToLeftFilmTension:
				{
					myString = "FilmTensionRightToLeft";
					myValue = Integer::toString(getFilmTensionRightToLeft());
					break;
				}
				case fieldOffsetEvolWrSettingsFilmWidth:
				{
					myString = "FilmWidth";
					myValue = Integer::toString(getFilmWidth());
					break;
				}
				case fieldOffsetEvolWrSettingsUpperRollWidth:
				{
					myString = "UpperRollWidth";
					myValue = Integer::toString(getUpperRollWidth());
					break;
				}
				case fieldOffsetEvolWrSettingsLowerRollWidth:
				{
					myString = "LowerRollWidth";
					myValue = Integer::toString(getLowerRollWidth());
					break;
				}
				case fieldOffsetEvolWrSettingsSealerBeltPower:
				{
					myString = "SealerBeltPower";
					myValue = Integer::toString(tempInfo.getSealerBeltHeaterEnabled());
					break;
				}
				case fieldOffsetEvolWrSettingsSealerBeltSetting:
				{
					myString = "SealerBeltSetting";
					myValue = Integer::toString(tempInfo.getSealerBeltSetting());
					break;
				}
				case fieldOffsetEvolWrSettingsFilmSupplyPower:
				{
					myString = "FilmSupplyPower";
					myValue = Integer::toString(tempInfo.getFilmTempEnabled());
					break;
				}
				case fieldOffsetEvolWrSettingsFilmSupplySetting:
				{
					myString = "FilmSupplySetting";
					myValue = Integer::toString(tempInfo.getFilmSetting());
					break;
				}
				default:
				{
					myString = "";
					myValue = "";
					break;
				}
			}
			++loopCount;
			Debug::trace("HfcEvolWrapperSettings", myloop + mySpace + myString + mySpace + myValue);
		}
	}
}

const char *HfcEvolWrapperSettings::serialize(int format)const
{
	HfcObject::serializedData = "";

	bool traceOn = Debug::isTracing("HfcEvolWrapperSettings");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "serialize(";
		msgDebug.append(Integer::toString(format)).append("):  [");
	}
	switch (format)
	{
		case BrowserFormat:
		{
			EvolWrapperTemperatureInfo tempInfo = getTemperatureInfo();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Show tempInfo   ************************************");
				Debug::trace("HfcEvolWrapperSettings", msgDebug2);
				tempInfo.show();
				Debug::trace("HfcEvolWrapperSettings", msgDebug2);
			}
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());

			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcEvolWrSettingsFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);


				switch (loopCnt)
				{
					case fieldOffsetEvolWrSettingsIntakeMode:
						HfcObject::serializedData.append(Integer::toString(getIntakeMode().getIntakeMode()));
						break;
					case fieldOffsetEvolWrSettingsFilmTension:
						HfcObject::serializedData.append(Integer::toString(getFilmTension()));
						break;
					case fieldOffsetEvolWrSettingsFrontToBackFilmTension:
						HfcObject::serializedData.append(Integer::toString(getFilmTensionFrontToBack()));
						break;
					case fieldOffsetEvolWrSettingsRightToLeftFilmTension:
						HfcObject::serializedData.append(Integer::toString(getFilmTensionRightToLeft()));
						break;
					case fieldOffsetEvolWrSettingsFilmWidth:
						HfcObject::serializedData.append(Integer::toString(getFilmWidth()));
						break;
					case fieldOffsetEvolWrSettingsUpperRollWidth:
						HfcObject::serializedData.append(Integer::toString(getUpperRollWidth()));
						break;
					case fieldOffsetEvolWrSettingsLowerRollWidth:
						HfcObject::serializedData.append(Integer::toString(getLowerRollWidth()));
						break;
					case fieldOffsetEvolWrSettingsSealerBeltPower:
						HfcObject::serializedData.append(Bool::toString(tempInfo.isSealerBeltHeaterEnabled()));
						break;
					case fieldOffsetEvolWrSettingsSealerBeltSetting:
						HfcObject::serializedData.append(Integer::toString(tempInfo.getSealerBeltSetting()));
						break;
					case fieldOffsetEvolWrSettingsFilmSupplyPower:
						HfcObject::serializedData.append(Bool::toString(tempInfo.isFilmTempEnabled()));
						break;
					case fieldOffsetEvolWrSettingsFilmSupplySetting:
						HfcObject::serializedData.append(Integer::toString(tempInfo.getFilmSetting()));
						break;
					default:
						HfcObject::serializedData.append("Invalid format");
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
	if (traceOn)
	{
		msgDebug.append(HfcObject::serializedData).append("]");
		Debug::trace("HfcEvolWrapperSettings", msgDebug);
	}
	return HfcObject::serializedData.data();
}

void HfcEvolWrapperSettings::update(HfcEvolWrapperControl *wrControl)

{
	if (!wrControl)
		return;

	setIntakeMode(wrControl->getIntakeMode());
	setFilmTension(wrControl->getFilmTension());
	setFilmTensionFrontToBack(wrControl->getFilmTensionFrontToBack());
	setFilmTensionRightToLeft(wrControl->getFilmTensionRightToLeft());
	setFilmWidth(wrControl->getFilmWidthSelection());

	EvolWrapperTemperatureInfo tempInfo = getTemperatureInfo();			//	Returns a const, so we need to set it afterwards.
	tempInfo.setSealerBeltHeaterEnabled(wrControl->getSealerBeltHeaterEnabled());
	tempInfo.setSealerBeltSetting(wrControl->getSealerBeltTempSetting());
	tempInfo.setFilmTempEnabled(wrControl->getFilmSupplyHeaterEnabled());
	tempInfo.setFilmSetting(wrControl->getFilmSupplyTempSetting());
	setTemperatureInfo(tempInfo);
}


