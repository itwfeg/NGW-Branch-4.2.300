#include "ProductDevEvent.h"
#include "ProductRemoveAccumItemMsg.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductRemoveAccumItemMsg::dataType = 0;
std::string ProductRemoveAccumItemMsg::dataTypeName("ProductRemoveAccumItemMsg"); 
#endif

ProductRemoveAccumItemMsg::ProductRemoveAccumItemMsg(Hobart::CEvolMsgReceiver *pHobThread, int itemIndex) : EvolMessage(ProductDev, ProductRemoveSingleAccumulatedItem, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	indexOfItemToRemove = itemIndex;
}

ProductRemoveAccumItemMsg::ProductRemoveAccumItemMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &indexOfItemToRemove;
	for (unsigned int i = 0; i < sizeof(indexOfItemToRemove); i++)
	{
		*pChar++ = buffer[j++];
	}
}

ProductRemoveAccumItemMsg::~ProductRemoveAccumItemMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductRemoveAccumItemMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &indexOfItemToRemove;
	for (unsigned int i = 0; i < sizeof(indexOfItemToRemove); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

int ProductRemoveAccumItemMsg::getIndexOfItemToRemove() const
{
	return indexOfItemToRemove;
}

