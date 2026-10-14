/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERSETTINGS_H
#define HFCEVOLWRAPPERSETTINGS_H

#include "evolution.h"
#include "EvolWrapperSettings.h"
#include "EvolWrapperInquire.h"
#include "HfcObject.h"

typedef enum HfcEvolWrapperSettingsFieldOffsets
{
	fieldOffsetEvolWrSettingsIntakeMode = 1,
	fieldOffsetEvolWrSettingsFilmTension,
	fieldOffsetEvolWrSettingsFrontToBackFilmTension,
	fieldOffsetEvolWrSettingsRightToLeftFilmTension,
	fieldOffsetEvolWrSettingsFilmWidth,
	fieldOffsetEvolWrSettingsUpperRollWidth,
	fieldOffsetEvolWrSettingsLowerRollWidth,
	fieldOffsetEvolWrSettingsSealerBeltPower,
	fieldOffsetEvolWrSettingsSealerBeltSetting,
	fieldOffsetEvolWrSettingsFilmSupplyPower,
	fieldOffsetEvolWrSettingsFilmSupplySetting,
//	fieldOffsetEvolWrSettingsIsCelsius,
//	fieldOffsetEvolWrSettingsFilmSupplyPresent,
	endHfcEvolWrSettingsFieldOffsets	//	MUST be last!
} HfcEvolWrapperSettingsFieldOffsets_t;

class HfcEvolWrapperConfig;
class HfcEvolWrapperControl;

class HfcEvolWrapperSettings : public EvolWrapperSettings, public HfcObject
{
	public:
		HfcEvolWrapperSettings(const char *data);
		HfcEvolWrapperSettings(HfcEvolWrapperConfig *wrConfig);
		~HfcEvolWrapperSettings();

		void init();
		void show();

		virtual const char *serialize(int format = BrowserFormat) const;
		void update(HfcEvolWrapperControl *wrControl);

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCEVOLWRAPPERSETTINGS_H

