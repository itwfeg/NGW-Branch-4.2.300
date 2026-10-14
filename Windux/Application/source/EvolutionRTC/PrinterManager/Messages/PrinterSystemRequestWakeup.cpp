//#include "vxWorks.h"
#include "PrinterSystemRequestWakeup.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSystemRequestWakeup::dataType = 0;
std::string PrinterSystemRequestWakeup::dataTypeName("PrinterSystemRequestWakeup"); 
#endif

PrinterSystemRequestWakeup::PrinterSystemRequestWakeup(Hobart::CEvolMsgReceiver *pHobThread,
													   PrinterType printerType) : SystemRequestWakeup(PrinterSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif


	this->printerType = printerType;
}

PrinterSystemRequestWakeup::PrinterSystemRequestWakeup(const char *buffer) : SystemRequestWakeup(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	SystemRequestWakeup::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterSystemRequestWakeup::~PrinterSystemRequestWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterSystemRequestWakeup::serialize() const
{
	SystemRequestWakeup::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterSystemRequestWakeup::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("PrinterSystemRequestWakeup") );
#else
	Debug::addTraceCategory("PrinterSystemRequestWakeup");
#endif
	Debug::trace("PrinterSystemRequestWakeup", "PrinterSystemRequestWakeup: ");

	SystemRequestWakeup::show();

	if (printerType == FirstPrinter)
	{
		Debug::trace("PrinterSystemRequestWakeup", HobString("printerType: FirstPrinter"));
	}
	else if (printerType == SecondPrinter)
	{
		Debug::trace("PrinterSystemRequestWakeup", HobString("printerType: SecondPrinter"));
	}
	else if (printerType == ThirdPrinter)
	{
		Debug::trace("PrinterSystemRequestWakeup", HobString("printerType: ThirdPrinter"));
	}
	else
	{
		Debug::trace("PrinterSystemRequestWakeup", HobString("printerType: Unknown ") +
											 HobString(Integer::toString((UINT16) printerType)));
	}

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("PrinterSystemRequestWakeup") );
#else
	Debug::removeTraceCategory("PrinterSystemRequestWakeup");
#endif
}

void PrinterSystemRequestWakeup::setPrinterType(PrinterType printerType)
{
	this->printerType = printerType;
}

PrinterType PrinterSystemRequestWakeup::getPrinterType()
{
	return printerType;
}
