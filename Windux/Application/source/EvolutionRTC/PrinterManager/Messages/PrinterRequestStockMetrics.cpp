//#include "vxWorks.h"
#include "PrinterRequestStockMetrics.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestStockMetrics::dataType = 0;
std::string PrinterRequestStockMetrics::dataTypeName("PrinterRequestStockMetrics"); 
#endif

PrinterRequestStockMetrics::PrinterRequestStockMetrics(Hobart::CEvolMsgReceiver *pHobThread,
													   PrinterType printerType) : PrinterGeneric(PrReqStockMetrics, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterRequestStockMetrics::PrinterRequestStockMetrics(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif


}

PrinterRequestStockMetrics::~PrinterRequestStockMetrics()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void PrinterRequestStockMetrics::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("PrinterRequestStockMetrics") );
#else
	Debug::addTraceCategory("PrinterRequestStockMetrics");
#endif
	Debug::trace("PrinterRequestStockMetrics", "PrinterRequestStockMetrics: ");

	PrinterGeneric::show();

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("PrinterRequestStockMetrics") );
#else
	Debug::removeTraceCategory("PrinterRequestStockMetrics");
#endif
}
