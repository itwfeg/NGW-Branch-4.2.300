//#include "vxWorks.h"
#include "PrinterSystemCalibrationSwitch.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSystemCalibrationSwitch::dataType = 0;
std::string PrinterSystemCalibrationSwitch::dataTypeName("PrinterSystemCalibrationSwitch"); 
#endif

PrinterSystemCalibrationSwitch::PrinterSystemCalibrationSwitch(Hobart::CEvolMsgReceiver *pHobThread,
															   bool weigherCalibSwitch) : SystemCalibrationSwitch(PrinterSystem,
																												 pHobThread,
																												 weigherCalibSwitch)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterSystemCalibrationSwitch::PrinterSystemCalibrationSwitch(const char *buffer) : SystemCalibrationSwitch(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterSystemCalibrationSwitch::~PrinterSystemCalibrationSwitch()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
