//#include "vxWorks.h"
#include "PrinterPreImage.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterPreImage::dataType = 0;
std::string PrinterPreImage::dataTypeName("PrinterPreImage"); 
#endif




PrinterPreImage::PrinterPreImage(Hobart::CEvolMsgReceiver *pHobThread,
								 PrinterType printerType,
								 const Util::CSmartPtr<Product> *pProductSmartPtr) : PrinterGeneric(PrPreImage,
								 																   pHobThread,
								 																   printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pProduct = 0;
	this->pProductSmartPtr = (Util::CSmartPtr<Product> *) pProductSmartPtr;
}


PrinterPreImage::PrinterPreImage(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &pProduct;
	for (unsigned int i = 0; i < sizeof(pProduct); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pProductSmartPtr;
	for (unsigned int i = 0; i < sizeof(pProductSmartPtr); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterPreImage::~PrinterPreImage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *PrinterPreImage::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pProduct;
	for (unsigned int i = 0; i < sizeof(pProduct); i++)
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

const Product *PrinterPreImage::getProduct()
{
	return (const Product *) pProduct;
}

void PrinterPreImage::setProduct(const Product *pProduct)
{
	this->pProduct = (Product *) pProduct;
}

const Util::CSmartPtr<Product> *PrinterPreImage::getProductSmartPtr()
{
	return (const Util::CSmartPtr<Product> *) pProductSmartPtr;
}