/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCSYSTEMCALIBRATIONSWITCH_H
#define HFCSYSTEMCALIBRATIONSWITCH_H

#include "evolution.h"
#include "SystemCalibrationSwitch.h"
#include "HfcObject.h"

typedef enum HfcSystemCalibrationSwitchFieldOffsets
{
	fieldOffsetCalibrationSwitch = 1,

	endHfcSystemCalibrationSwitchFieldOffsets	//	MUST be last!
} HfcSystemCalibrationSwitchFieldOffsets_t;

class HfcSystemCalibrationSwitch : public SystemCalibrationSwitch, public HfcObject
{
	public:
		HfcSystemCalibrationSwitch();
		HfcSystemCalibrationSwitch(const char *data);
		HfcSystemCalibrationSwitch(const char *data, int len, int formatType);

		~HfcSystemCalibrationSwitch();

		virtual const char *serialize(int format = InternalFormat) const;
		UINT32 getSerializedLength() const;

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCSYSTEMCALIBRATIONSWITCH_H
