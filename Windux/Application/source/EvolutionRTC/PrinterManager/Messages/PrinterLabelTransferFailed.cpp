//#include "vxWorks.h"
#include "PrinterEvent.h"
#include "PrinterLabelTransferFailed.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterLabelTransferFailed::dataType = 0;
std::string PrinterLabelTransferFailed::dataTypeName("PrinterLabelTransferFailed"); 
#endif

PrinterLabelTransferFailed::PrinterLabelTransferFailed(Hobart::CEvolMsgReceiver *pHobThread,
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

	setEventType(PrLabelTransferFailed);
}

PrinterLabelTransferFailed::PrinterLabelTransferFailed(const char *buffer) : PrinterRequestLabelTransfer(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterLabelTransferFailed::~PrinterLabelTransferFailed()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void PrinterLabelTransferFailed::show()
{
	bool isTracing = Debug::isTracing("PrinterLabelTransferFailed");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterLabelTransferFailed") );
#else
		Debug::addTraceCategory("PrinterLabelTransferFailed");
#endif
	}

	Debug::trace("PrinterLabelTransferFailed", "PrinterLabelTransferFailed: ");

	PrinterRequestLabelTransfer::show();

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterLabelTransferFailed") );
#else
		Debug::removeTraceCategory("PrinterLabelTransferFailed");
#endif
	}
}
