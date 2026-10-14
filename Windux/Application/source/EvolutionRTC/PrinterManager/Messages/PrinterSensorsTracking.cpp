//#include "vxWorks.h"
#include "PrinterSensorsTracking.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSensorsTracking::dataType = 0;
std::string PrinterSensorsTracking::dataTypeName("PrinterSensorsTracking"); 
#endif

PrinterSensorsTracking::PrinterSensorsTracking(Hobart::CEvolMsgReceiver *pHobThread,
											   PrinterType printerType,
											   PrinterStateType sensorsTracking) : PrinterGeneric(PrSensorsTracking, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->sensorsTracking = sensorsTracking;
}

PrinterSensorsTracking::PrinterSensorsTracking(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &sensorsTracking;
	for (unsigned int i = 0; i < sizeof(sensorsTracking); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterSensorsTracking::~PrinterSensorsTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterSensorsTracking::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sensorsTracking;
	for (unsigned int i = 0; i < sizeof(sensorsTracking); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterSensorsTracking::setSensorsTracking()
{
	sensorsTracking = PrinterOn;
}

void PrinterSensorsTracking::clearSensorsTracking()
{
	sensorsTracking = PrinterOff;
}

bool PrinterSensorsTracking::isSensorsTracking()
{
	return (sensorsTracking == PrinterOn);
}

PrinterStateType PrinterSensorsTracking::getSensorsTracking()
{
	return sensorsTracking;
}
