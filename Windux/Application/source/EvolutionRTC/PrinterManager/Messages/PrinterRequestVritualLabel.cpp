//#include "vxWorks.h"
#include "PrinterRequestVirtualLabel.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"
#include "UInteger.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestVirtualLabel::dataType = 0;
std::string PrinterRequestVirtualLabel::dataTypeName("PrinterRequestVirtualLabel"); 
#endif

PrinterRequestVirtualLabel::PrinterRequestVirtualLabel(Hobart::CEvolMsgReceiver *pHobThread,
														 PrinterType printerType,
														 UINT16 totalDataLength,
														 UINT16 *pData,
														 UINT16 linesToPrint) : PrinterGeneric(PrReqVirtualLabel, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ((totalDataLength == 0) || (pData == NULL) || (linesToPrint == 0))
	{
		this->totalDataLength = 0;
		this->pData = NULL;
		this->linesToPrint = 0;
	}
	else
	{
		this->totalDataLength = totalDataLength;
		this->pData = pData;
		this->linesToPrint = linesToPrint;
	}

}

PrinterRequestVirtualLabel::PrinterRequestVirtualLabel(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &totalDataLength;
	for (unsigned int i = 0; i < sizeof(totalDataLength); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pData;
	for (unsigned int i = 0; i < sizeof(pData); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &linesToPrint;
	for (unsigned int i = 0; i < sizeof(pData); i++)
	{
		*pChar++ = buffer[j++];
	}

}

PrinterRequestVirtualLabel::~PrinterRequestVirtualLabel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
 
const char *PrinterRequestVirtualLabel::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &totalDataLength;
	for (unsigned int i = 0; i < sizeof(totalDataLength); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &pData;
	for (unsigned int i = 0; i < sizeof(pData); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &linesToPrint;
	for (unsigned int i = 0; i < sizeof(pData); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterRequestVirtualLabel::show()
{
	bool isTracing = Debug::isTracing("PrinterRequestVirtualLabel");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterRequestVirtualLabel") );
#else
		Debug::addTraceCategory("PrinterRequestVirtualLabel");
#endif
	}

	Debug::trace("PrinterRequestVirtualLabel", "PrinterRequestVirtualLabel: ");

	PrinterGeneric::show();

	Debug::trace("PrinterRequestVirtualLabel", HobString("totalDataLength: ") + HobString(UInteger::toString(totalDataLength)));
	Debug::trace("PrinterRequestVirtualLabel", HobString("pData: 0x") + HobString(Hex::toString((UINT32) pData)));
	Debug::trace("PrinterRequestVirtualLabel", HobString("linesToPrint: ") + HobString(Integer::toString(linesToPrint)));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterRequestVirtualLabel") );
#else
		Debug::removeTraceCategory("PrinterRequestVirtualLabel");
#endif
	}
}

UINT16 PrinterRequestVirtualLabel::getTotalDataLength()
{
	return totalDataLength;
}

void PrinterRequestVirtualLabel::setTotalDataLength(UINT16 totalDataLength)
{
	if (totalDataLength == 0)
	{
		this->totalDataLength = totalDataLength;
		this->pData = pData;
	}
	else
	{
		this->totalDataLength = totalDataLength;
	}
}


const UINT16 *PrinterRequestVirtualLabel::getDataPointer()
{
	return (const UINT16 *) pData;
}

void PrinterRequestVirtualLabel::setDataPointer(const UINT16 * pData)
{
	if (pData == NULL)
	{
		this->totalDataLength = 0;
		this->pData = NULL;
		this->linesToPrint = 0;
	}
	else
	{
		this->pData = (UINT16 *) pData;
	}
}


UINT16 PrinterRequestVirtualLabel::getLinesToPrint()
{
	return linesToPrint;
}

void PrinterRequestVirtualLabel::setLinesToPrint(UINT16 lines)
{
	if (lines == 0)
	{
		this->totalDataLength = 0;
		this->pData = NULL;
		this->linesToPrint = lines;
	}
	else
	{
		this->linesToPrint = lines;
	}
}