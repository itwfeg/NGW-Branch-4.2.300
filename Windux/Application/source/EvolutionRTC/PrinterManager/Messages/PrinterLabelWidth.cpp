//#include "vxWorks.h"
#include "PrinterLabelWidth.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterLabelWidth::dataType = 0;
std::string PrinterLabelWidth::dataTypeName("PrinterLabelWidth"); 
#endif

PrinterLabelWidth::PrinterLabelWidth(Hobart::CEvolMsgReceiver *pHobThread,
									 PrinterType printerType,
									 LabelWidth labelWidth) : PrinterGeneric(PrLabelWidth,
									 										 pHobThread,
									 										 printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->labelWidth = labelWidth;
}

PrinterLabelWidth::PrinterLabelWidth(const char *buffer) : PrinterGeneric(buffer)
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
}

PrinterLabelWidth::~PrinterLabelWidth()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterLabelWidth::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterLabelWidth::setLabelWidthUFWLabel()
{
	labelWidth = UFWLabel;
}

bool PrinterLabelWidth::isLabelWidthUFWLabel()
{
	return (labelWidth == UFWLabel);
}

void PrinterLabelWidth::setLabelWidthNarrowLabel()
{
	labelWidth = NarrowLabel;
}

bool PrinterLabelWidth::isLabelWidthNarrowLabel()
{
	return (labelWidth == NarrowLabel);
}

void PrinterLabelWidth::setLabelWidthWideLabel()
{
	labelWidth = WideLabel;
}

bool PrinterLabelWidth::isLabelWidthWideLabel()
{
	return (labelWidth == WideLabel);
}

void PrinterLabelWidth::setLabelWidth(LabelWidth labelWidth)
{
	this->labelWidth = labelWidth;
}

LabelWidth PrinterLabelWidth::getLabelWidth()
{
	return labelWidth;
}

