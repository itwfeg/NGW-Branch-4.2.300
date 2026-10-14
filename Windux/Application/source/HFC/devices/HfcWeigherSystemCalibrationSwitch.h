/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERSYSTEMCALIBRATIONSWITCH_H
#define HFCWEIGHERSYSTEMCALIBRATIONSWITCH_H

#include "HfcSystemCalibrationSwitch.h"

class HfcWeigherSystemCalibrationSwitch : public HfcSystemCalibrationSwitch
{
	public:

		HfcWeigherSystemCalibrationSwitch();
		HfcWeigherSystemCalibrationSwitch(const char *data);
		HfcWeigherSystemCalibrationSwitch(const char *data, int len, int formatType);

		~HfcWeigherSystemCalibrationSwitch();


	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWEIGHERSYSTEMCALIBRATIONSWITCH_H
