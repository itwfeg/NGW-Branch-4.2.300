//#include "vxWorks.h"
#include "PrinterRequestSensors.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestSensors::dataType = 0;
std::string PrinterRequestSensors::dataTypeName("PrinterRequestSensors"); 
#endif

PrinterRequestSensors::PrinterRequestSensors(Hobart::CEvolMsgReceiver *pHobThread,
											 PrinterType printerType) : PrinterGeneric(PrReqSensors, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestSensors::PrinterRequestSensors(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestSensors::~PrinterRequestSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

