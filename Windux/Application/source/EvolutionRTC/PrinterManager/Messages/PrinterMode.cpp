//#include "vxWorks.h"
#include "PrinterMode.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterMode::dataType = 0;
std::string PrinterMode::dataTypeName("PrinterMode"); 
#endif

PrinterMode::PrinterMode(Hobart::CEvolMsgReceiver *pHobThread,
						 PrinterType printerType,
						 LabelOrientation prMode) : PrinterGeneric(PrMode, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->prMode = prMode;
}

PrinterMode::PrinterMode(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &prMode;
	for (unsigned int i = 0; i < sizeof(prMode); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterMode::~PrinterMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterMode::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &prMode;
	for (unsigned int i = 0; i < sizeof(prMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterMode::setPortraitLabel()
{
	prMode = PortraitLabel;
}

bool PrinterMode::isPortraitLabel()
{
	return (prMode == PortraitLabel);
}

void PrinterMode::setLandscapeLabel()
{
	prMode = LandscapeLabel;
}

bool PrinterMode::isLandscapeLabel()
{
	return (prMode == LandscapeLabel);
}

LabelOrientation PrinterMode::getPrinterMode()
{
	return prMode;
}
