/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERSETTINGS_H
#define HFCWRAPPERSETTINGS_H

#include "HfcObject.h"
#include "WrapperSettings.h"

class HfcWrapperConfig;

class HfcWrapperSettings : public WrapperSettings, public HfcObject
{
	public:
		HfcWrapperSettings(const char *data);
		HfcWrapperSettings(const char *data, int len, int formatType);
		HfcWrapperSettings(HfcWrapperConfig *wrConfig);

		~HfcWrapperSettings();

		const char *serialize(int format) const;

	private:
		typedef enum HfcWrapperSettingsFieldOffsets
		{
			fieldOffsetWrFilmWidth = 1,
			fieldOffsetWrFilmTension,
			fieldOffsetWrIntakeMode,
			fieldOffsetWrHeaterEnable,
			fieldOffsetWrHeaterSetting,
			fieldOffsetWrIsCelsius,

			endHfcWrapperSettingsFieldOffsets	//	MUST be last!
		} HfcWrapperSettingsFieldOffsets_t;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//HFCWRAPPERSETTINGS_H
