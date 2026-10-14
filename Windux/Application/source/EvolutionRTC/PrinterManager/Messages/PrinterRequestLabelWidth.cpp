//#include "vxWorks.h"
#include "PrinterRequestLabelWidth.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestLabelWidth::dataType = 0;
std::string PrinterRequestLabelWidth::dataTypeName("PrinterRequestLabelWidth"); 
#endif

PrinterRequestLabelWidth::PrinterRequestLabelWidth(Hobart::CEvolMsgReceiver *pHobThread,
												   PrinterType printerType) : PrinterGeneric(PrReqLabelWidth,
												   											 pHobThread,
												   											 printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestLabelWidth::PrinterRequestLabelWidth(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestLabelWidth::~PrinterRequestLabelWidth()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

