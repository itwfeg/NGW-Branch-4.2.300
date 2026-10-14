#include "ProductDevEvent.h"
#include "ProductLookupMsg.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductLookupMsg::dataType = 0;
std::string ProductLookupMsg::dataTypeName("ProductLookupMsg"); 
#endif

ProductLookupMsg::ProductLookupMsg(Hobart::CEvolMsgReceiver *pHobThread, const long pluNum) : EvolMessage(ProductDev, ProductReadToo, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	pluNumber = pluNum;
}

ProductLookupMsg::ProductLookupMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &pluNumber;
	for (unsigned int i = 0; i < sizeof(pluNumber); i++)
		*pChar++ = buffer[j++];
}

ProductLookupMsg::~ProductLookupMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductLookupMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pluNumber;
	for (unsigned int i = 0; i < sizeof(pluNumber); i++)
		serializedData.append(1, pChar[i]);

	return serializedData.data();
}

long ProductLookupMsg::getPluNumber() const
{
	return pluNumber;
}

void ProductLookupMsg::setPluNumber(const long pluNum)
{
	pluNumber = pluNum;
}
