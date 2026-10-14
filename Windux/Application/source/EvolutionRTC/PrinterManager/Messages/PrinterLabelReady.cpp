//#include "vxWorks.h"
#include "PrinterLabelReady.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterLabelReady::dataType = 0;
std::string PrinterLabelReady::dataTypeName("PrinterLabelReady"); 
#endif

PrinterLabelReady::PrinterLabelReady(Hobart::CEvolMsgReceiver *pHobThread,
									 PrinterType printerType,
									 HobRecordNum recordNumber,
									 const ProductTransaction *pProductTransaction,
									 UINT8 packId) : PrinterGeneric(PrLabelReady, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->recordNumber = recordNumber;
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
	this->packId = packId;
}

PrinterLabelReady::PrinterLabelReady(const char *buffer) : PrinterGeneric(buffer)
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

	pChar = (char *) &packId;
	for (unsigned int i = 0; i < sizeof(packId); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterLabelReady::~PrinterLabelReady()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *PrinterLabelReady::serialize() const
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

	pChar = (char *) &packId;
	for (unsigned int i = 0; i < sizeof(packId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

HobRecordNum PrinterLabelReady::getRecordNumber()
{
	return recordNumber;
}

void PrinterLabelReady::setRecordNumber(HobRecordNum recordNumber)
{
	this->recordNumber = recordNumber;
}

const ProductTransaction *PrinterLabelReady::getProductTransaction()
{
	return (const ProductTransaction *) pProductTransaction;
}

void PrinterLabelReady::setProductTransaction(const ProductTransaction *pProductTransaction)
{
	this->pProductTransaction = pProductTransaction;
}

UINT8 PrinterLabelReady::getPackId()
{
	return packId;
}

void PrinterLabelReady::setPackId(UINT8 packId)
{
	this->packId = packId;
}
