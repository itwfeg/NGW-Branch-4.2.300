//#include "vxWorks.h"
#include "PrinterNewPrinterType.h"

#include "Debug.h"
#include "UInteger.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterNewPrinterType::dataType = 0;
std::string PrinterNewPrinterType::dataTypeName("PrinterNewPrinterType"); 
#endif

PrinterNewPrinterType::PrinterNewPrinterType(Hobart::CEvolMsgReceiver *pHobThread,
											 PrinterType printerType,
											 PrinterType newPrinterType,
											 USBD_NODE_ID usbNodeId,
											 UINT16 physicalAddress) : PrinterPrinterType(pHobThread,
																						  printerType,
																						  usbNodeId,
																						  physicalAddress)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setEventType(PrNewOrder);
	this->newPrinterType = newPrinterType;
}

PrinterNewPrinterType::PrinterNewPrinterType(const char *buffer) : PrinterPrinterType(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterPrinterType::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &newPrinterType;
	for (unsigned int i = 0; i < sizeof(newPrinterType); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterNewPrinterType::~PrinterNewPrinterType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterNewPrinterType::serialize() const
{
	PrinterPrinterType::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &newPrinterType;
	for (unsigned int i = 0; i < sizeof(newPrinterType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterNewPrinterType::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("PrinterNewPrinterType") );
#else
	Debug::addTraceCategory("PrinterNewPrinterType");
#endif
	Debug::trace("PrinterNewPrinterType", "PrinterNewPrinterType: ");

	PrinterPrinterType::show();

	Debug::trace("PrinterNewPrinterType", HobString("newPrinterType: ") +
										  HobString(UInteger::toString(newPrinterType)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("PrinterNewPrinterType") );
#else
	Debug::removeTraceCategory("PrinterNewPrinterType");
#endif
}

void PrinterNewPrinterType::setNewPrinterType(PrinterType newPrinterType)
{
	this->newPrinterType = newPrinterType;
}

PrinterType PrinterNewPrinterType::getNewPrinterType()
{
	return newPrinterType;
}
