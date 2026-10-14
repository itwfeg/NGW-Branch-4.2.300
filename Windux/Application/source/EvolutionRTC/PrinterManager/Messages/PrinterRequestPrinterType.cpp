//#include "vxWorks.h"

#include "PrinterRequestPrinterType.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestPrinterType::dataType = 0;
std::string PrinterRequestPrinterType::dataTypeName("PrinterRequestPrinterType"); 
#endif

PrinterRequestPrinterType::PrinterRequestPrinterType(Hobart::CEvolMsgReceiver *pHobThread,
													 PrinterType printerType) : PrinterGeneric(PrReqPrinterType,
																							   pHobThread,
																							   printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestPrinterType::PrinterRequestPrinterType(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestPrinterType::~PrinterRequestPrinterType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

