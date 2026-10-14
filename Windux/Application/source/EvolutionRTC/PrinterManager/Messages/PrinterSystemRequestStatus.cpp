//#include "vxWorks.h"
#include "PrinterSystemRequestStatus.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSystemRequestStatus::dataType = 0;
std::string PrinterSystemRequestStatus::dataTypeName("PrinterSystemRequestStatus"); 
#endif

PrinterSystemRequestStatus::PrinterSystemRequestStatus(Hobart::CEvolMsgReceiver *pHobThread,
													   PrinterType printerType) : SystemRequestStatus(PrinterSystem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->printerType = printerType;
}

PrinterSystemRequestStatus::PrinterSystemRequestStatus(const char *buffer) : SystemRequestStatus(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	SystemRequestStatus::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterSystemRequestStatus::~PrinterSystemRequestStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterSystemRequestStatus::serialize() const
{
	SystemRequestStatus::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterSystemRequestStatus::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("PrinterSystemRequestStatus") );
#else
	Debug::addTraceCategory("PrinterSystemRequestStatus");
#endif
	Debug::trace("PrinterSystemRequestStatus", "PrinterSystemRequestStatus: ");

	SystemRequestStatus::show();

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("PrinterSystemRequestStatus") );
#else
	Debug::removeTraceCategory("PrinterSystemRequestStatus");
#endif
}

void PrinterSystemRequestStatus::setPrinterType(PrinterType printerType)
{
	this->printerType = printerType;
}

PrinterType PrinterSystemRequestStatus::getPrinterType() const
{
	return printerType;
}
