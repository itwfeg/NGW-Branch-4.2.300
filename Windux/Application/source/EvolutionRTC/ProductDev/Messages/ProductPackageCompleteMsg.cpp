#include "ProductPackageCompleteMsg.h"


#include "CEvolMsgReceiver.h"
#include "ProductDevEvent.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductPackageCompleteMsg::dataType = 0;
std::string ProductPackageCompleteMsg::dataTypeName("ProductPackageCompleteMsg"); 
#endif



ProductPackageCompleteMsg::ProductPackageCompleteMsg(Hobart::CEvolMsgReceiver *pHobThread, HobRecordNum recordNumber,
		const ProductTransaction * pProductTransaction)
	: EvolMessage(ProductDev, ProductPackageComplete, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->recordNumber_ = recordNumber;
	this->pProductTransaction_ = pProductTransaction;
}


ProductPackageCompleteMsg::ProductPackageCompleteMsg(const char *buffer)
	: EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &recordNumber_;
	for (unsigned int i = 0; i < sizeof(recordNumber_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pProductTransaction_;
	for (unsigned int i = 0; i < sizeof(pProductTransaction_); i++)
	{
		*pChar++ = buffer[j++];
	}
}


ProductPackageCompleteMsg::~ProductPackageCompleteMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *ProductPackageCompleteMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &recordNumber_;
	for (unsigned int i = 0; i < sizeof(recordNumber_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pProductTransaction_;
	for (unsigned int i = 0; i < sizeof(pProductTransaction_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
