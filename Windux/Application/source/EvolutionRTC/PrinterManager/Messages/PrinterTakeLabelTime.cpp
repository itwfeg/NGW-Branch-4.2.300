//#include "vxWorks.h"
#include "PrinterTakeLabelTime.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterTakeLabelTime::dataType = 0;
std::string PrinterTakeLabelTime::dataTypeName("PrinterTakeLabelTime"); 
#endif

PrinterTakeLabelTime::PrinterTakeLabelTime(Hobart::CEvolMsgReceiver *pHobThread,
										   PrinterType printerType,
										   UINT32 takeLabelTime) : PrinterGeneric(PrTakeLabelTime,
																				  pHobThread,
								   												  printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->takeLabelTime = takeLabelTime;
}

PrinterTakeLabelTime::PrinterTakeLabelTime(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &takeLabelTime;
	for (unsigned int i = 0; i < sizeof(takeLabelTime); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterTakeLabelTime::~PrinterTakeLabelTime()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterTakeLabelTime::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &takeLabelTime;
	for (unsigned int i = 0; i < sizeof(takeLabelTime); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 PrinterTakeLabelTime::getTakeLabelTime()
{
	return takeLabelTime;
}

void PrinterTakeLabelTime::setTakeLabelTime(UINT32 takeLabelTime)
{
	this->takeLabelTime = takeLabelTime;
}
