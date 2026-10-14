//#include "vxWorks.h"
#include "PrinterPostImage.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterPostImage::dataType = 0;
std::string PrinterPostImage::dataTypeName("PrinterPostImage"); 
#endif

PrinterPostImage::PrinterPostImage(Hobart::CEvolMsgReceiver *pHobThread,
                                   PrinterType printerType,
								   const ProductTransaction *pProductTransaction,
								   UINT8 packId,
								   const Util::CSmartPtr<Product> *pProductSmartPtr) : PrinterGeneric(PrPostImage, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pProductTransaction = pProductTransaction;
	this->packId = packId;
	this->pProductSmartPtr = (Util::CSmartPtr<Product> *) pProductSmartPtr;
}

PrinterPostImage::PrinterPostImage(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &packId;
	for (unsigned int i = 0; i < sizeof(packId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pProductSmartPtr;
	for (unsigned int i = 0; i < sizeof(pProductSmartPtr); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterPostImage::~PrinterPostImage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterPostImage::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	
	pChar = (char *) &packId;
	for (unsigned int i = 0; i < sizeof(packId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pProductSmartPtr;
	for (unsigned int i = 0; i < sizeof(pProductSmartPtr); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterPostImage::show()
{
	bool isTracing = Debug::isTracing("PrinterPostImage");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterPostImage") );
#else
		Debug::addTraceCategory("PrinterPostImage");
#endif
	}

	Debug::trace("PrinterPostImage", "PrinterPostImage: ");

	PrinterGeneric::show();

	Debug::trace("PrinterPostImage", HobString("pProductTransaction: 0x") + HobString(Hex::toString((long long) pProductTransaction)));
	Debug::trace("PrinterPostImage", HobString("packId: ") + HobString(Integer::toString(packId)));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterPostImage") );
#else
		Debug::removeTraceCategory("PrinterPostImage");
#endif
	}
}

const ProductTransaction *PrinterPostImage::getProductTransaction()
{
	return pProductTransaction;
}

void PrinterPostImage::setProductTransaction(const ProductTransaction *pProductTransaction)
{
	this->pProductTransaction = pProductTransaction;
}

UINT8 PrinterPostImage::getPackId()
{
	return packId;
}

void PrinterPostImage::setPackId(UINT8 packId)
{
	this->packId = packId;
}

const Util::CSmartPtr<Product> *PrinterPostImage::getProductSmartPtr()
{
	return (const Util::CSmartPtr<Product> *) pProductSmartPtr;
}