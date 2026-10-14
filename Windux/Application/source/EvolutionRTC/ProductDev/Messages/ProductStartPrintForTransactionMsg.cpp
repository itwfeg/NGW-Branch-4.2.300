#include "ProductDevEvent.h"
#include "ProductStartPrintForTransactionMsg.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductStartPrintForTransactionMsg::dataType = 0;
std::string ProductStartPrintForTransactionMsg::dataTypeName("ProductStartPrintForTransactionMsg"); 
#endif


ProductStartPrintForTransactionMsg::ProductStartPrintForTransactionMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductTransaction *pProductTransaction) : EvolMessage(ProductDev, ProductStartPrintForTransaction, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
	okToPrintTransaction = false;
}

ProductStartPrintForTransactionMsg::ProductStartPrintForTransactionMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &okToPrintTransaction;
	for (unsigned int i = 0; i < sizeof(okToPrintTransaction); i++)
	{
		*pChar++ = buffer[j++];
	}
}

ProductStartPrintForTransactionMsg::~ProductStartPrintForTransactionMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductStartPrintForTransactionMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &okToPrintTransaction;
	for (unsigned int i = 0; i < sizeof(okToPrintTransaction); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

const ProductTransaction *ProductStartPrintForTransactionMsg::getProductTransaction()
{
	return (const ProductTransaction *) pProductTransaction;
}

void ProductStartPrintForTransactionMsg::setProductTransaction(const ProductTransaction *pProductTransaction)
{
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
}

bool ProductStartPrintForTransactionMsg::isOkToPrintTransaction() const
{
	return okToPrintTransaction;
}
void ProductStartPrintForTransactionMsg::setOkToPrintTransaction(bool ok)
{
	okToPrintTransaction = ok;
}
