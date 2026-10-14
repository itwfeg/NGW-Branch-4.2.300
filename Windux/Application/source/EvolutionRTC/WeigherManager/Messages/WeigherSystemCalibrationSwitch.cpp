//#include "vxWorks.h"
#include "WeigherSystemCalibrationSwitch.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherSystemCalibrationSwitch::dataType = 0;
std::string WeigherSystemCalibrationSwitch::dataTypeName("WeigherSystemCalibrationSwitch"); 
#endif

WeigherSystemCalibrationSwitch::WeigherSystemCalibrationSwitch(Hobart::CEvolMsgReceiver *pHobThread,
															   bool weigherCalibSwitch) : SystemCalibrationSwitch(WeigherSystem,
																												 pHobThread,
																												 weigherCalibSwitch)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherSystemCalibrationSwitch::WeigherSystemCalibrationSwitch(const char *buffer) : SystemCalibrationSwitch(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

WeigherSystemCalibrationSwitch::~WeigherSystemCalibrationSwitch()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
