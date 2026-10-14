//#include "vxWorks.h"
#include "PrinterDeletePackage.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterDeletePackage::dataType = 0;
std::string PrinterDeletePackage::dataTypeName("PrinterDeletePackage"); 
#endif

PrinterDeletePackage::PrinterDeletePackage(Hobart::CEvolMsgReceiver *pHobThread,
		                                   PrinterType printerType,
										   const ProductTransaction *pProductTransaction) : PrinterGeneric(PrDeletePackage,
																			   							   pHobThread,
																			   							   printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
}

PrinterDeletePackage::PrinterDeletePackage(const char *buffer) : PrinterGeneric(buffer)
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
}

PrinterDeletePackage::~PrinterDeletePackage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *PrinterDeletePackage::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pProductTransaction;
	for (unsigned int i = 0; i < sizeof(pProductTransaction); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

const ProductTransaction *PrinterDeletePackage::getProductTransaction()
{
	return (const ProductTransaction *) pProductTransaction;
}

void PrinterDeletePackage::setProductTransaction(const ProductTransaction *pProductTransaction)
{
	this->pProductTransaction = (ProductTransaction *) pProductTransaction;
}
