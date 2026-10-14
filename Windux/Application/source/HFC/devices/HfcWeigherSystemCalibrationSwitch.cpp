/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWeigherSystemCalibrationSwitch.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherSystemCalibrationSwitch::dataType = 0;
std::string HfcWeigherSystemCalibrationSwitch::dataTypeName("HfcWeigherSystemCalibrationSwitch"); 
#endif

HfcWeigherSystemCalibrationSwitch::HfcWeigherSystemCalibrationSwitch()
	: HfcSystemCalibrationSwitch()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemCalibrationSwitch::HfcWeigherSystemCalibrationSwitch(const char *data)
	: HfcSystemCalibrationSwitch(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemCalibrationSwitch::HfcWeigherSystemCalibrationSwitch(const char *data, int len, int formatType)
	: HfcSystemCalibrationSwitch(data, len, formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWeigherSystemCalibrationSwitch::~HfcWeigherSystemCalibrationSwitch()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
