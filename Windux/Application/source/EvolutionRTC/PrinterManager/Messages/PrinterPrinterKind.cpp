//#include "vxWorks.h"
#include "PrinterPrinterKind.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterPrinterKind::dataType = 0;
std::string PrinterPrinterKind::dataTypeName("PrinterPreImage"); 
#endif

PrinterPrinterKind::PrinterPrinterKind(Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType, PrinterKind printerKind) : PrinterGeneric(PrKind, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->printerKind = printerKind;
}

PrinterPrinterKind::PrinterPrinterKind(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printerKind;
	for (unsigned int i = 0; i < sizeof(printerKind); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterPrinterKind::~PrinterPrinterKind()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterPrinterKind::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printerKind;
	for (unsigned int i = 0; i < sizeof(printerKind); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterPrinterKind::show()
{
	bool isTracing = Debug::isTracing("PrinterPrinterKind");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterPrinterKind") );
#else
		Debug::addTraceCategory("PrinterPrinterKind");
#endif
	}

	Debug::trace("PrinterPrinterKind", "PrinterPrinterKind: ");

	PrinterGeneric::show();

	if (printerKind == NoPrinter)
	{
		Debug::trace("PrinterPrinterKind", HobString("printerKind: NoPrinter"));
	}
	else if (printerKind == EppPrinter)
	{
		Debug::trace("PrinterPrinterKind", HobString("printerKind: EppPrinter"));
	}
	else if (printerKind == EssPrinter)
	{
		Debug::trace("PrinterPrinterKind", HobString("printerKind: EssPrinter"));
	}
	else if (printerKind == EscPrinter)
	{
		Debug::trace("PrinterPrinterKind", HobString("printerKind: EscPrinter"));
	}
	else if (printerKind == ApcPrinter)
	{
		Debug::trace("PrinterPrinterKind", HobString("printerKind: ApcPrinter"));
	}
	else if (printerKind == KPCPrinter)
	{
		Debug::trace("PrinterPrinterKind", HobString("printerKind: KPCPrinter"));
	}
	else if (printerKind == SSKPCPrinter)
	{
		Debug::trace("PrinterPrinterKind", HobString("printerKind: SSKPCPrinter"));
	}
	else
	{
		Debug::trace("PrinterPrinterKind", HobString("printerKind: Unknown ") +
										   HobString(Integer::toString((UINT16) printerKind)));
	}

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterPrinterKind") );
#else
		Debug::removeTraceCategory("PrinterPrinterKind");
#endif
	}
}

PrinterKind PrinterPrinterKind::getPrinterKind()
{
	return printerKind;
}

void PrinterPrinterKind::setPrinterKind(PrinterKind printerKind)
{
	this->printerKind = printerKind;
}
