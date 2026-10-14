//#include "vxWorks.h"

#include "ProductDevEvent.h"
#include "ProductFinishedPrintingAllLabelsMsg.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductFinishedPrintingAllLabelsMsg::dataType = 0;
std::string ProductFinishedPrintingAllLabelsMsg::dataTypeName("ProductFinishedPrintingAllLabelsMsg"); 
#endif

ProductFinishedPrintingAllLabelsMsg::ProductFinishedPrintingAllLabelsMsg(Hobart::CEvolMsgReceiver *pHobThread,
																								   bool printOk) : EvolMessage(ProductDev, ProductFinishedPrintingAllLabels, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->printOk = printOk;
}

ProductFinishedPrintingAllLabelsMsg::ProductFinishedPrintingAllLabelsMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printOk;
	for (unsigned int i = 0; i < sizeof(printOk); i++)
	{
		*pChar++ = buffer[j++];
	}
}

ProductFinishedPrintingAllLabelsMsg::~ProductFinishedPrintingAllLabelsMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ProductFinishedPrintingAllLabelsMsg::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printOk;
	for (unsigned int i = 0; i < sizeof(printOk); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

bool ProductFinishedPrintingAllLabelsMsg::isPrintOk()
{
	return printOk;
}

void ProductFinishedPrintingAllLabelsMsg::setPrintOk(bool value)
{
	printOk = value;
}

void ProductFinishedPrintingAllLabelsMsg::clearPrintOk()
{
	printOk = false;
}
