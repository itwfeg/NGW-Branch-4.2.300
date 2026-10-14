//#include "vxWorks.h"

#include "ProductDevEvent.h"
#include "ProductUpdateMsg.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductUpdateMsg::dataType = 0;
std::string ProductUpdateMsg::dataTypeName("ProductUpdateMsg"); 
#endif

ProductUpdateMsg::ProductUpdateMsg(Hobart::CEvolMsgReceiver *pHobThread, const ProductTransaction *pProductTransaction,
									bool ignoreWeigherStatus, bool ignoreWeighMode) : EvolMessage(ProductDev, ProductUpdate, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
	this->ignoreWeigherStatus = ignoreWeigherStatus;
	this->ignoreWeighMode = ignoreWeighMode;
}

ProductUpdateMsg::ProductUpdateMsg(const char *buffer) : EvolMessage(buffer)
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

	pChar = (char *) &ignoreWeigherStatus;
	for (unsigned int i = 0; i < sizeof(ignoreWeigherStatus); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &ignoreWeighMode;
	for (unsigned int i = 0; i < sizeof(ignoreWeighMode); i++)
	{
		*pChar++ = buffer[j++];
	}
}

ProductUpdateMsg::~ProductUpdateMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductUpdateMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &ignoreWeigherStatus;
	for (unsigned int i = 0; i < sizeof(ignoreWeigherStatus); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &ignoreWeighMode;
	for (unsigned int i = 0; i < sizeof(ignoreWeighMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

const ProductTransaction *ProductUpdateMsg::getProductTransaction()
{
	return (const ProductTransaction *) pProductTransaction;
}

void ProductUpdateMsg::setProductTransaction(const ProductTransaction *pProductTransaction)
{
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
}

bool ProductUpdateMsg::getIgnoreWeigherStatus()
{
	return ignoreWeigherStatus;
}

void ProductUpdateMsg::setIgnoreWeigherStatus(bool ignoreWeigherStatus)
{
	this->ignoreWeigherStatus = ignoreWeigherStatus;
}

bool ProductUpdateMsg::getIgnoreWeighMode()
{
	return ignoreWeighMode;
}

void ProductUpdateMsg::setIgnoreWeighMode(bool ignoreWeighMode)
{
	this->ignoreWeighMode = ignoreWeighMode;
}
