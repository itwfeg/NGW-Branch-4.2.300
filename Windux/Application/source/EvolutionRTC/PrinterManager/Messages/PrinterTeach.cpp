#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "PrinterTeach.h"
#include "CommandOperationList.h"
#include "Debug.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterTeach::dataType = 0;
std::string PrinterTeach::dataTypeName("PrinterTeach"); 
#endif

PrinterTeach::PrinterTeach(Hobart::CEvolMsgReceiver *pHobThread,
						   PrinterType printerType,
						   PrinterCommandType printerCommandType,
						   CommandOperationList *ops) :  PrinterGeneric(PrTeach, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->printerCommandType = printerCommandType;
	this->ops = ops;
}

PrinterTeach::PrinterTeach(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif


	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &printerCommandType;
	for (unsigned int i = 0; i < sizeof(printerCommandType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &ops;
	for (unsigned int i = 0; i < sizeof(ops); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterTeach::~PrinterTeach()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterTeach::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &printerCommandType;
	for (unsigned int i = 0; i < sizeof(printerCommandType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &ops;
	for (unsigned int i = 0; i < sizeof(ops); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterTeach::show()
{
	Debug::trace("PrinterTeach", "PrinterTeach::show() printerCommandType: " + Integer::toString(printerCommandType)); 
	Debug::trace("PrinterTeach", "PrinterTeach::show() numEntries: " + Integer::toString(getNumEntries())); 
	ops->show();
}

UINT16 PrinterTeach::getNumEntries()
{
	return (UINT16)ops->size();	
}

PrinterCommandType PrinterTeach::getPrinterCommandType()
{
	return printerCommandType;
}

void PrinterTeach::setPrinterCommandType(PrinterCommandType printerCommandType)
{
	this->printerCommandType = printerCommandType;
}

CommandOperationList *PrinterTeach::getCommandOperationList()
{
	return ops;
}

void PrinterTeach::setCommandOperationList(CommandOperationList *ops)
{
	this->ops = ops;
}

