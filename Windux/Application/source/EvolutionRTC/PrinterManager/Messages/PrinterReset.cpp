//#include "vxWorks.h"
#include "PrinterReset.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterReset::dataType = 0;
std::string PrinterReset::dataTypeName("PrinterReset"); 
#endif

PrinterReset::PrinterReset(Hobart::CEvolMsgReceiver *pHobThread,
						   PrinterType printerType) : PrinterGeneric(PrReset, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterReset::PrinterReset(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterReset::~PrinterReset()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

