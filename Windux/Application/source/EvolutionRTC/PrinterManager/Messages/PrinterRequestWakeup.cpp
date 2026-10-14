//#include "vxWorks.h"
#include "PrinterRequestWakeup.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestWakeup::dataType = 0;
std::string PrinterRequestWakeup::dataTypeName("PrinterRequestWakeup"); 
#endif

PrinterRequestWakeup::PrinterRequestWakeup(Hobart::CEvolMsgReceiver *pHobThread,
										   PrinterType printerType,
										   UINT16 defHardwareAddr) : PrinterGeneric(PrReqWakeup, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->defHardwareAddr = defHardwareAddr;
}

PrinterRequestWakeup::PrinterRequestWakeup(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &defHardwareAddr;
	for (unsigned int i = 0; i < sizeof(defHardwareAddr); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterRequestWakeup::~PrinterRequestWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterRequestWakeup::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &defHardwareAddr;
	for (unsigned int i = 0; i < sizeof(defHardwareAddr); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT16 PrinterRequestWakeup::getDefaultHardwareAddress()
{
	return defHardwareAddr;
}

void PrinterRequestWakeup::setDefaultHardwareAddress(UINT16 defHardwareAddr)
{
	this->defHardwareAddr = defHardwareAddr;
}

