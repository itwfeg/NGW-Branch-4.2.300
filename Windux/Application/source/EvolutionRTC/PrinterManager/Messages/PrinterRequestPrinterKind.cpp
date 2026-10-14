//#include "vxWorks.h"
#include "PrinterRequestPrinterKind.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestPrinterKind::dataType = 0;
std::string PrinterRequestPrinterKind::dataTypeName("PrinterRequestPrinterKind"); 
#endif

PrinterRequestPrinterKind::PrinterRequestPrinterKind(Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType) : PrinterGeneric(PrReqKind, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestPrinterKind::PrinterRequestPrinterKind(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestPrinterKind::~PrinterRequestPrinterKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

