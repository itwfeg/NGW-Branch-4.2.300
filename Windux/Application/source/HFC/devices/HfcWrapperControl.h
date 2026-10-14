/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERCONTROL_H
#define HFCWRAPPERCONTROL_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperControl.h"

class HfcWrapperSettings;
class HfcWrapperStatus;

typedef enum HfcWrapperControlFieldOffsets
{
	fieldOffsetWrControlFilmWidth = 1,
	fieldOffsetWrControlFilmTension,
	fieldOffsetWrControlIntakeMode,
	fieldOffsetWrControlHeaterEnable,
	fieldOffsetWrControlHeaterSetting,

	endHfcWrapperControlFieldOffsets		//	MUST be last!
} HfcWrapperControlFieldOffsets_t;

class HfcWrapperControl : public WrapperControl, public HfcObject
{
	public:
		HfcWrapperControl(const char *data);
		HfcWrapperControl(const char *data, int len, int formatType);
		HfcWrapperControl(HfcWrapperSettings *settings);

		~HfcWrapperControl();

		virtual const char *serialize(int format = InternalFormat) const;

		bool update(HfcWrapperControl *wrControl, bool &settingChanged);
		void update(HfcWrapperStatus *wrStatus);

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCWRAPPERCONTROL_H
