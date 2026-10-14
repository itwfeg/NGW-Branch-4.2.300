//#include "vxWorks.h"

#include "ProductDevEvent.h"
#include "ProductChangeMsg.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductChangeMsg::dataType = 0;
std::string ProductChangeMsg::dataTypeName("ProductChangeMsg"); 
#endif

ProductChangeMsg::ProductChangeMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductManager *pProductMgr) : EvolMessage(ProductDev, ProductChangeSmart, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pProductMgr = (ProductManager *) pProductMgr;
	this->pProduct = 0;
}

ProductChangeMsg::ProductChangeMsg(Hobart::CEvolMsgReceiver *pHobThread, const Product *pProduct) : EvolMessage(ProductDev, ProductChange, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pProduct = (Product *) pProduct;
	this->pProductMgr = 0;
}

ProductChangeMsg::ProductChangeMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	if (getEventType() == ProductChangeSmart)
	{
		char *pChar = (char *) &pProductMgr;
		for (unsigned int i = 0; i < sizeof(pProductMgr); i++)
		{
			*pChar++ = buffer[j++];
		}
		this->pProduct = 0;
	}
	else
	{
		char *pChar = (char *) &pProduct;
		for (unsigned int i = 0; i < sizeof(pProduct); i++)
		{
			*pChar++ = buffer[j++];
		}
		this->pProductMgr = 0;
	}
}

ProductChangeMsg::~ProductChangeMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductChangeMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	if (getEventType() == ProductChangeSmart)
	{
		char *pChar = (char *) &pProductMgr;
		for (unsigned int i = 0; i < sizeof(pProductMgr); i++)
		{
			serializedData.append(1, pChar[i]);
		}
	}
	else
	{
		char *pChar = (char *) &pProduct;
		for (unsigned int i = 0; i < sizeof(pProduct); i++)
		{
			serializedData.append(1, pChar[i]);
		}
	}

	return serializedData.data();
}

const Product *ProductChangeMsg::getProduct()
{
	return (const Product *) pProduct;
}

void ProductChangeMsg::setProduct(const Product *pProduct)
{
	this->pProduct = (Product *) pProduct;
}


const ProductManager *ProductChangeMsg::getProductMgr()
{
	return (const ProductManager *) pProductMgr;
}

void ProductChangeMsg::setProduct(const ProductManager *pProductMgr)
{
	this->pProductMgr = (ProductManager *) pProductMgr;
}
