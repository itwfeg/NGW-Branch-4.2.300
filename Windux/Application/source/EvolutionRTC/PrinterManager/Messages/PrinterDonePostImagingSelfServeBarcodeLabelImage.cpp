#include "PrinterDonePostImagingSelfServeBarcodeLabelImage.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterDonePostImagingSelfServeBarcodeLabelImage::dataType = 0;
std::string PrinterDonePostImagingSelfServeBarcodeLabelImage::dataTypeName("PrinterDonePostImagingSelfServeBarcodeLabelImage");
#endif

PrinterDonePostImagingSelfServeBarcodeLabelImage::PrinterDonePostImagingSelfServeBarcodeLabelImage(Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType, int counter) : PrinterGeneric(PrDonePostImagingSelfServeBarcodeLabelImage, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	filenameCounter = counter;
}

PrinterDonePostImagingSelfServeBarcodeLabelImage::PrinterDonePostImagingSelfServeBarcodeLabelImage(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &filenameCounter;
	for (unsigned int i = 0; i < sizeof(filenameCounter); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterDonePostImagingSelfServeBarcodeLabelImage::~PrinterDonePostImagingSelfServeBarcodeLabelImage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *PrinterDonePostImagingSelfServeBarcodeLabelImage::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &filenameCounter;
	for (unsigned int i = 0; i < sizeof(filenameCounter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
