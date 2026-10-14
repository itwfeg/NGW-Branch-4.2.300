//#include "vxWorks.h"
#include "PrinterLabelTaken.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterLabelTaken::dataType = 0;
std::string PrinterLabelTaken::dataTypeName("PrinterLabelTaken"); 
#endif

PrinterLabelTaken::PrinterLabelTaken(Hobart::CEvolMsgReceiver *pHobThread,
									 PrinterType printerType,
									 HobRecordNum recordNumber,
									 const ProductTransaction *pProductTransaction,
									 UINT8 packId) : PrinterLabelReady(pHobThread,
																	   printerType,
																	   recordNumber,
																	   pProductTransaction,
																	   packId)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrLabelTaken);
}

PrinterLabelTaken::PrinterLabelTaken(const char *buffer) : PrinterLabelReady(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterLabelTaken::~PrinterLabelTaken()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
