//#include "vxWorks.h"

#include "ProductDevEvent.h"
#include "ProductCompleteMsg.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductCompleteMsg::dataType = 0;
std::string ProductCompleteMsg::dataTypeName("ProductCompleteMsg"); 
#endif

ProductCompleteMsg::ProductCompleteMsg(Hobart::CEvolMsgReceiver *pHobThread,
									   HobRecordNum recordNumber,
									   const ProductTransaction *pProductTransaction,
									   bool printOk) : EvolMessage(ProductDev, ProductComplete, pHobThread)
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

ProductCompleteMsg::ProductCompleteMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

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

ProductCompleteMsg::~ProductCompleteMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductCompleteMsg::serialize() const
{
	EvolMessage::serialize();

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

HobRecordNum ProductCompleteMsg::getRecordNumber()
{
	return recordNumber;
}

void ProductCompleteMsg::setRecordNumber(HobRecordNum recordNumber)
{
	this->recordNumber = recordNumber;
}

ProductTransaction *ProductCompleteMsg::getProductTransaction()
{
	return (ProductTransaction *) pProductTransaction;
}

void ProductCompleteMsg::setProductTransaction(const ProductTransaction *pProductTransaction)
{
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
}

bool ProductCompleteMsg::isPrintOk()
{
	return printOk;
}

void ProductCompleteMsg::setPrintOk(bool value)
{
	printOk = value;
}

void ProductCompleteMsg::clearPrintOk()
{
	printOk = false;
}
