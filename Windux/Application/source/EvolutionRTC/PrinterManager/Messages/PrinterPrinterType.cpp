//#include "vxWorks.h"
#include "PrinterPrinterType.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterPrinterType::dataType = 0;
std::string PrinterPrinterType::dataTypeName("PrinterPrinterType"); 
#endif

PrinterPrinterType::PrinterPrinterType(Hobart::CEvolMsgReceiver *pHobThread,
									   PrinterType printerType,
									   USBD_NODE_ID usbNodeId,
									   UINT16 physicalAddress) : PrinterGeneric(PrPrinterType,
																				pHobThread,
																				printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->usbNodeId = usbNodeId;
	this->physicalAddress = physicalAddress;
}

PrinterPrinterType::PrinterPrinterType(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &usbNodeId;
	for (unsigned int i = 0; i < sizeof(usbNodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &physicalAddress;
	for (unsigned int i = 0; i < sizeof(physicalAddress); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterPrinterType::~PrinterPrinterType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterPrinterType::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &usbNodeId;
	for (unsigned int i = 0; i < sizeof(usbNodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &physicalAddress;
	for (unsigned int i = 0; i < sizeof(physicalAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterPrinterType::show()
{
//	Debug::addTraceCategory( std::string("PrinterPrinterType") );
	if (Debug::isTracing("PrinterPrinterType"))
	{
		Debug::trace("PrinterPrinterType", "PrinterPrinterType: ");

		PrinterGeneric::show();

		Debug::trace("PrinterPrinterType", HobString("usbNodeId: ") +
									   HobString(Integer::toString64((__int64) usbNodeId)));
		Debug::trace("PrinterPrinterType", HobString("physicalAddress: 0x") +
										   HobString(Hex::toString(physicalAddress)));
	}
//	Debug::removeTraceCategory( std::string("PrinterPrinterType") );
}

//	PTR 1060 - setUsbNodeId() is needed to change the order of the printers. We never send this message back to the printer. [CCD]
void PrinterPrinterType::setUsbNodeId(USBD_NODE_ID usbNodeId)
{
	Debug::trace("PrinterPrinterType", HobString("setUsbNodeId(") +	HobString(Hex::toString((long long) usbNodeId)) + ")");
	this->usbNodeId = usbNodeId;
}

USBD_NODE_ID PrinterPrinterType::getUsbNodeId() const
{
	return usbNodeId;
}

void PrinterPrinterType::setPhysicalAddress(UINT16 physicalAddress)
{
	this->physicalAddress = physicalAddress;
}

UINT16 PrinterPrinterType::getPhysicalAddress() const
{
	return physicalAddress;
}
