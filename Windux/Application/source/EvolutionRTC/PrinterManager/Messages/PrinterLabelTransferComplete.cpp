//#include "vxWorks.h"
#include "PrinterEvent.h"
#include "PrinterLabelTransferComplete.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterLabelTransferComplete::dataType = 0;
std::string PrinterLabelTransferComplete::dataTypeName("PrinterLabelTransferComplete"); 
#endif

PrinterLabelTransferComplete::PrinterLabelTransferComplete(Hobart::CEvolMsgReceiver *pHobThread,
														 PrinterType printerType,
														 UINT16 totalDataLength,
														 UINT16 actualDataLength,
														 UINT16 *pData) : PrinterRequestLabelTransfer(pHobThread,
														 											  printerType,
														 											  totalDataLength,
																									  actualDataLength,
														 											  pData)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrLabelTransferComplete);
}

PrinterLabelTransferComplete::PrinterLabelTransferComplete(const char *buffer) : PrinterRequestLabelTransfer(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterLabelTransferComplete::~PrinterLabelTransferComplete()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void PrinterLabelTransferComplete::show()
{
	bool isTracing = Debug::isTracing("PrinterLabelTransferComplete");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterLabelTransferComplete") );
#else
		Debug::addTraceCategory("PrinterLabelTransferComplete");
#endif
	}

	Debug::trace("PrinterLabelTransferComplete", "PrinterLabelTransferComplete: ");

	PrinterRequestLabelTransfer::show();

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterLabelTransferComplete") );
#else
		Debug::removeTraceCategory("PrinterLabelTransferComplete");
#endif
	}
}
