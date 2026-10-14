#include "ProductDevEvent.h"
#include "ProductReadAllAccumulatedItemsMsg.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductReadAllAccumulatedItemsMsg::dataType = 0;
std::string ProductReadAllAccumulatedItemsMsg::dataTypeName("ProductReadAllAccumulatedItemsMsg"); 
#endif

ProductReadAllAccumulatedItemsMsg::ProductReadAllAccumulatedItemsMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductTransactionMaster *pProductTransactionMaster) : EvolMessage(ProductDev, ProductReadAllAccumulatedItems, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->pProductTransactionMaster = (ProductTransactionMaster *) pProductTransactionMaster;
}

ProductReadAllAccumulatedItemsMsg::ProductReadAllAccumulatedItemsMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &pProductTransactionMaster;
	for (unsigned int i = 0; i < sizeof(pProductTransactionMaster); i++)
	{
		*pChar++ = buffer[j++];
	}
}

ProductReadAllAccumulatedItemsMsg::~ProductReadAllAccumulatedItemsMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductReadAllAccumulatedItemsMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pProductTransactionMaster;
	for (unsigned int i = 0; i < sizeof(pProductTransactionMaster); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

const ProductTransactionMaster *ProductReadAllAccumulatedItemsMsg::getProductTransactionMaster() const
{
	return (const ProductTransactionMaster *) pProductTransactionMaster;
}

void ProductReadAllAccumulatedItemsMsg::setProductTransactionMaster(const ProductTransactionMaster *pProductTransactionMaster)
{
	this->pProductTransactionMaster = (ProductTransactionMaster *) pProductTransactionMaster;
}
