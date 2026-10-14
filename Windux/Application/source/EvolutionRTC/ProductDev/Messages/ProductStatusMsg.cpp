//#include "vxWorks.h"

#include "ProductDevEvent.h"
#include "ProductStatusMsg.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductStatusMsg::dataType = 0;
std::string ProductStatusMsg::dataTypeName("ProductStatusMsg"); 
#endif

ProductStatusMsg::ProductStatusMsg(Hobart::CEvolMsgReceiver *pHobThread,
									   const Product *pProduct,
									   const ProductTransaction *pProductTransaction,
									   ProductStatus status) : EvolMessage(ProductDev, ProductStatusEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pProduct = (Product *) pProduct;
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
	this->status = status;
}

ProductStatusMsg::ProductStatusMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &pProduct;
	for (unsigned int i = 0; i < sizeof(pProduct); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
	{
		*pChar++ = buffer[j++];
	}
}

ProductStatusMsg::~ProductStatusMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductStatusMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pProduct;
	for (unsigned int i = 0; i < sizeof(pProduct); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

Product *ProductStatusMsg::getProduct()
{
	return (Product *) pProduct;
}

void ProductStatusMsg::setProduct(const Product *pProduct)
{
	this->pProduct = (Product *) pProduct;
}

ProductTransaction *ProductStatusMsg::getProductTransaction()
{
	return (ProductTransaction *) pProductTransaction;
}

void ProductStatusMsg::setProductTransaction(const ProductTransaction *pProductTransaction)
{
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
}

ProductStatusMsg::ProductStatus ProductStatusMsg::getStatus()
{
	return status;
}

void ProductStatusMsg::setStatus(ProductStatusMsg::ProductStatus status)
{
	this->status = status;
}
