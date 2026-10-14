//#include "vxWorks.h"
#include "PrinterSizing.h"

#include "Debug.h"
#include "HobString.h"
#include "UInteger.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSizing::dataType = 0;
std::string PrinterSizing::dataTypeName("PrinterSizing"); 
#endif

PrinterSizing::PrinterSizing(Hobart::CEvolMsgReceiver *pHobThread,
							 PrinterType printerType,
							 UINT16 sizingOn) : PrinterGeneric(PrSizing, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (sizingOn > 0)
		this->sizingOn = true;
	else
		this->sizingOn = false;
}

PrinterSizing::PrinterSizing(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &sizingOn;
	for (unsigned int i = 0; i < sizeof(sizingOn); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterSizing::~PrinterSizing()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterSizing::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sizingOn;
	for (unsigned int i = 0; i < sizeof(sizingOn); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterSizing::show()
{
	bool isTracing = Debug::isTracing("PrinterSizing");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterSizing") );
#else
		Debug::addTraceCategory("PrinterSizing");
#endif
	}

	Debug::trace("PrinterSizing", "PrinterSizing: ");

	PrinterGeneric::show();

	Debug::trace("PrinterSizing", HobString("sizingOn: ") +	HobString(UInteger::toString(sizingOn)));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterSizing") );
#else
		Debug::removeTraceCategory("PrinterSizing");
#endif
	}
}

void PrinterSizing::enableSizing()
{
	sizingOn = true;
}

void PrinterSizing::disableSizing()
{
	sizingOn = false;
}

bool PrinterSizing::isSizing()
{
	return sizingOn;
}
