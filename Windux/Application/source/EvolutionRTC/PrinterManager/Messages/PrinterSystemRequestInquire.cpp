//#include "vxWorks.h"
#include "PrinterSystemRequestInquire.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSystemRequestInquire::dataType = 0;
std::string PrinterSystemRequestInquire::dataTypeName("PrinterSystemRequestInquire"); 
#endif

PrinterSystemRequestInquire::PrinterSystemRequestInquire(Hobart::CEvolMsgReceiver *pHobThread,
														 PrinterType printerType) : SystemRequestInquire(PrinterSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->printerType = printerType;
}

PrinterSystemRequestInquire::PrinterSystemRequestInquire(const char *buffer) : SystemRequestInquire(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	SystemRequestInquire::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterSystemRequestInquire::~PrinterSystemRequestInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterSystemRequestInquire::serialize() const
{
	SystemRequestInquire::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterSystemRequestInquire::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("PrinterSystemRequestInquire") );
#else
	Debug::addTraceCategory("PrinterSystemRequestInquire");
#endif
	Debug::trace("PrinterSystemRequestInquire", "PrinterSystemRequestInquire: ");

	SystemRequestInquire::show();

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("PrinterSystemRequestInquire") );
#else
	Debug::removeTraceCategory("PrinterSystemRequestInquire");
#endif
}

void PrinterSystemRequestInquire::setPrinterType(PrinterType printerType)
{
	this->printerType = printerType;
}

PrinterType PrinterSystemRequestInquire::getPrinterType()
{
	return printerType;
}

