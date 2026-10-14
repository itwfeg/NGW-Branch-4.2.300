//#include "vxWorks.h"
#include "PrinterRequestInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestInquire::dataType = 0;
std::string PrinterRequestInquire::dataTypeName("PrinterRequestInquire"); 
#endif

PrinterRequestInquire::PrinterRequestInquire(Hobart::CEvolMsgReceiver *pHobThread,
											 PrinterType printerType) : PrinterGeneric(PrReqInquire, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestInquire::PrinterRequestInquire(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestInquire::~PrinterRequestInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

