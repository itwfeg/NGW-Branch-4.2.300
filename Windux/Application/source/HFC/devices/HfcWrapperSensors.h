/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERSENSE_H
#define HFCWRAPPERSENSE_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperSensors.h"

#include <list>

typedef enum HfcWrapperSensorsFieldOffsets
{
	fieldOffsetWrSHeaterReading = 1,
	fieldOffsetWrSEncoderCount,
	fieldOffsetWrSIntakeCount,
	fieldOffsetWrSStopSwitch,
	fieldOffsetWrSStartSwitch,
	fieldOffsetWrSPackageLengthSwitch,
	fieldOffsetWrSPackageSenseSwitch,
	fieldOffsetWrSPackageWidthSwitch,
	fieldOffsetWrSPusherHomeSwitch,
	fieldOffsetWrSPackageJamSwitch,
	fieldOffsetWrSWideFilmSwitch,
	fieldOffsetWrSNarrowFilmSwitch,
	fieldOffsetWrSSlideBlockState,
	fieldOffsetWrSHeaterState,
	fieldOffsetWrSCompressorState,
	fieldOffsetWrSMainMotorState,
	fieldOffsetWrSDischargeMotorState,
	fieldOffsetWrSFilmLengthState,
	fieldOffsetWrSFilmWidthState,
	fieldOffsetWrSKnifeState,
	fieldOffsetWrSLeftSideClampState,
	fieldOffsetWrSRightSideClampState,
	fieldOffsetWrSGripperState,

	endHfcWrapperSensorsFieldOffsets		//	MUST be last!
} HfcWrapperSensorsFieldOffsets_t;

class HfcWrapperSensors : public WrapperSensors, public HfcObject
{
	public:

		HfcWrapperSensors(const char *data);
		HfcWrapperSensors(const char *data, int len, int formatType);

		~HfcWrapperSensors();

		static void initFieldLists();
		virtual const char *serialize(int format = InternalFormat) const;


	private:
		static std::list<HfcWrapperSensorsFieldOffsets> wrapperTestsSenseFieldList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERSENSE_H
