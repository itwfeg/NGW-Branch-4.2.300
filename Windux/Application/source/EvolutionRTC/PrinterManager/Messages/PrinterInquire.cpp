//#include "vxWorks.h"
#include "PrinterInquire.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterInquire::dataType = 0;
std::string PrinterInquire::dataTypeName("PrinterInquire"); 
#endif

PrinterInquire::PrinterInquire(Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType) : PrinterConfiguration(pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrInquire);
}

PrinterInquire::PrinterInquire(const char *buffer) : PrinterConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

PrinterInquire::~PrinterInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void PrinterInquire::show()
{
	bool isTracing = Debug::isTracing("PrinterInquire");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterInquire") );
#else
		Debug::addTraceCategory("PrinterInquire");
#endif
	}

	Debug::trace("PrinterInquire", "PrinterInquire: ");

	PrinterConfiguration::show();

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterInquire") );
#else
		Debug::removeTraceCategory("PrinterInquire");
#endif
	}
}
