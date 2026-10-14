#include <iostream>
using namespace std;

//#include "vxWorks.h"
#include "PrinterWakeup.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterWakeup::dataType = 0;
std::string PrinterWakeup::dataTypeName("PrinterWakeup"); 
#endif

PrinterWakeup::PrinterWakeup(Hobart::CEvolMsgReceiver *pHobThread,
							 PrinterType printerType,
							 LabelWidth labelWidth,
							 UINT16 bufferSize,
							 UINT16 printheadSize,
							 PrCfgStatus prCfgStatus) : PrinterGeneric(PrWakeup, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->labelWidth = labelWidth;
	this->bufferSize = bufferSize;
	this->printheadSize = printheadSize;
	this->prCfgStatus = prCfgStatus;
}

PrinterWakeup::PrinterWakeup(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &bufferSize;
	for (unsigned int i = 0; i < sizeof(bufferSize); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printheadSize;
	for (unsigned int i = 0; i < sizeof(printheadSize); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &prCfgStatus;
	for (unsigned int i = 0; i < sizeof(prCfgStatus); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterWakeup::~PrinterWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
 
const char *PrinterWakeup::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &bufferSize;
	for (unsigned int i = 0; i < sizeof(bufferSize); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printheadSize;
	for (unsigned int i = 0; i < sizeof(printheadSize); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &prCfgStatus;
	for (unsigned int i = 0; i < sizeof(prCfgStatus); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterWakeup::show()
{
	cout << "Printer Wakeup: " << endl;

	PrinterGeneric::show();

	cout << "\tlabelWidth:" << (UINT16) labelWidth << endl;
	cout << "\tbufferSize:" << bufferSize << endl;
	cout << "\tprintheadSize:" << printheadSize << endl;
	cout << "\tprCfgStatus:" << (UINT16) prCfgStatus << endl;
}
LabelWidth PrinterWakeup::getLabelWidth()
{
	return labelWidth;
}

void PrinterWakeup::setLabelWidth(LabelWidth labelWidth)
{
	this->labelWidth = labelWidth;
}

UINT16 PrinterWakeup::getBufferSize()
{
	return bufferSize;
}

void PrinterWakeup::setBufferSize(UINT16 bufferSize)
{
	this->bufferSize = bufferSize;
}

UINT16 PrinterWakeup::getPrintheadSize()
{
	return printheadSize;
}

void PrinterWakeup::setPrinterheadSize(UINT16 printheadSize)
{
	this->printheadSize = printheadSize;
}

PrCfgStatus PrinterWakeup::getPrCfgStatus()
{
	return prCfgStatus;
}

void PrinterWakeup::setPrCfgStatus(PrCfgStatus prCfgStatus)
{
	this->prCfgStatus = prCfgStatus;
}
