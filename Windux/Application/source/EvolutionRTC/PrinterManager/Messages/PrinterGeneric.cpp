//#include "vxWorks.h"
#include "PrinterGeneric.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("PrinterGeneric"); 
#endif

PrinterGeneric::PrinterGeneric(const PrinterGeneric& obj):EvolMessage(obj), printerType(obj.printerType) 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

PrinterGeneric::PrinterGeneric(PrEvent prEvent, Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType) : EvolMessage(Printer, (UINT8)prEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->printerType = printerType;
}

PrinterGeneric::PrinterGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterGeneric::~PrinterGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

PrinterGeneric& PrinterGeneric::operator=(const PrinterGeneric& rhs)
{
	printerType = rhs.printerType;
	return *this;
}

const char *PrinterGeneric::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterGeneric::show() const
{
	if (!Debug::isTracing( std::string("PrinterGeneric") ))
		return;
	
	Debug::trace("PrinterGeneric", "PrinterGeneric: ");

	EvolMessage::show();

	if (printerType == FirstPrinter)
	{
		Debug::trace("PrinterGeneric", HobString("printerType: FirstPrinter"));
	}
	else if (printerType == SecondPrinter)
	{
		Debug::trace("PrinterGeneric", HobString("printerType: SecondPrinter"));
	}
	else if (printerType == ThirdPrinter)
	{
		Debug::trace("PrinterGeneric", HobString("printerType: ThirdPrinter"));
	}
	else
	{
		Debug::trace("PrinterGeneric", HobString("printerType: Unknown ") +
										HobString(Integer::toString((UINT16) printerType)));
	}
}

PrinterType PrinterGeneric::getPrinterType() const
{
	return printerType;
}

void PrinterGeneric::setPrinterType(PrinterType printerType)
{
	this->printerType = printerType;
}
