//#include "vxWorks.h"
#include "PrinterLabelComplete.h"
#include <Long.h>
#include <Bool.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterLabelComplete::dataType = 0;
std::string PrinterLabelComplete::dataTypeName("PrinterLabelComplete"); 
#endif

PrinterLabelComplete::PrinterLabelComplete(Hobart::CEvolMsgReceiver *pHobThread,
		                                   PrinterType printerType,
										   HobRecordNum recordNumber,
										   const ProductTransaction *pProductTransaction,
										   bool printOk) : PrinterGeneric(PrLabelComplete,
										   								  pHobThread,
										   								  printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->recordNumber = recordNumber;
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
	this->printOk = printOk;
}

PrinterLabelComplete::PrinterLabelComplete(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif


	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &recordNumber;
	for (unsigned int i = 0; i < sizeof(recordNumber); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printOk;
	for (unsigned int i = 0; i < sizeof(printOk); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterLabelComplete::~PrinterLabelComplete()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *PrinterLabelComplete::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &recordNumber;
	for (unsigned int i = 0; i < sizeof(recordNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printOk;
	for (unsigned int i = 0; i < sizeof(printOk); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

HobRecordNum PrinterLabelComplete::getRecordNumber()
{
	return recordNumber;
}

void PrinterLabelComplete::setRecordNumber(HobRecordNum recordNumber)
{
	this->recordNumber = recordNumber;
}

const ProductTransaction *PrinterLabelComplete::getProductTransaction()
{
	return (const ProductTransaction *) pProductTransaction;
}

void PrinterLabelComplete::setProductTransaction(const ProductTransaction *pProductTransaction)
{
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
}

bool PrinterLabelComplete::isPrintOk()
{
	return printOk;
}

void PrinterLabelComplete::setPrintOk(bool value)
{
	printOk = value;
}

void PrinterLabelComplete::clearPrintOk()
{
	printOk = false;
}

void PrinterLabelComplete::show()
{
	bool isTracing = Debug::isTracing("PrinterLabelComplete");

	if (isTracing == false)
	{	
		Debug::addTraceCategory("PrinterLabelComplete");
	}

	Debug::trace("PrinterLabelComplete", "PrinterLabelComplete: ");
	Debug::trace("PrinterLabelComplete", "RecordNumber: " + Long::toString(this->recordNumber));

	

	if (isTracing == false)
	{
		Debug::removeTraceCategory("PrinterLabelComplete");
	}




}
