//#include "vxWorks.h"
#include "PrinterRequestStatus.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestStatus::dataType = 0;
std::string PrinterRequestStatus::dataTypeName("PrinterRequestStatus"); 
#endif

PrinterRequestStatus::PrinterRequestStatus(Hobart::CEvolMsgReceiver *pHobThread,
										   PrinterType printerType) : PrinterGeneric(PrReqStatus, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestStatus::PrinterRequestStatus(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestStatus::~PrinterRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

