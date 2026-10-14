//#include "vxWorks.h"
#include "PrinterNewAddress.h"

#include "Debug.h"
#include "UInteger.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterNewAddress::dataType = 0;
std::string PrinterNewAddress::dataTypeName("PrinterNewAddress"); 
#endif

PrinterNewAddress::PrinterNewAddress(Hobart::CEvolMsgReceiver *pHobThread,
									 PrinterType printerType,
									 UINT16 physicalAddress) : PrinterGeneric(PrNewAddress,
																			  pHobThread,
																			  printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->physicalAddress = physicalAddress;
}

PrinterNewAddress::PrinterNewAddress(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &physicalAddress;
	for (unsigned int i = 0; i < sizeof(physicalAddress); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterNewAddress::~PrinterNewAddress()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterNewAddress::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &physicalAddress;
	for (unsigned int i = 0; i < sizeof(physicalAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterNewAddress::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("PrinterNewAddress") );
#else
	Debug::addTraceCategory("PrinterNewAddress");
#endif
	Debug::trace("PrinterNewAddress", "PrinterNewAddress: ");

	PrinterGeneric::show();

	Debug::trace("PrinterNewAddress", HobString("physicalAddress: 0x") +
									   HobString(Hex::toString(physicalAddress)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("PrinterNewAddress") );
#else
	Debug::removeTraceCategory("PrinterNewAddress");
#endif
}

void PrinterNewAddress::setPhysicalAddress(UINT16 physicalAddress)
{
	this->physicalAddress = physicalAddress;
}

UINT16 PrinterNewAddress::getPhysicalAddress() const
{
	return physicalAddress;
}
