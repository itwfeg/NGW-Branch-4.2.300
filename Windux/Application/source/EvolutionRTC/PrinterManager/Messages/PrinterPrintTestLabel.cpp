//#include "vxWorks.h"
#include "PrinterPrintTestLabel.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterPrintTestLabel::dataType = 0;
std::string PrinterPrintTestLabel::dataTypeName("PrinterPrintTestLabel"); 
#endif

PrinterPrintTestLabel::PrinterPrintTestLabel(Hobart::CEvolMsgReceiver *pHobThread,
											 PrinterType printerType,
											 PrTestLabelType prTestLabel,
											 PrTestLabelQuantity quantity,
											 PrinterCommandOptions prTestCommandOptions) : PrinterGeneric(PrTestLabel,
																							pHobThread,
																							printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->prTestLabel = prTestLabel;
	this->quantity = quantity;
	this->prTestCommandOptions = prTestCommandOptions;
}

PrinterPrintTestLabel::PrinterPrintTestLabel(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &prTestLabel;
	for (unsigned int i = 0; i < sizeof(prTestLabel); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &quantity;
	for (unsigned int i = 0; i < sizeof(quantity); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &prTestCommandOptions;
	for (unsigned int i = 0; i < sizeof(prTestCommandOptions); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterPrintTestLabel::~PrinterPrintTestLabel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *PrinterPrintTestLabel::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &prTestLabel;
	for (unsigned int i = 0; i < sizeof(prTestLabel); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &quantity;
	for (unsigned int i = 0; i < sizeof(quantity); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &prTestCommandOptions;
	for (unsigned int i = 0; i < sizeof(prTestCommandOptions); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterPrintTestLabel::show()
{
	bool isTracing = Debug::isTracing("PrinterPrintTestLabel");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterPrintTestLabel") );
#else
		Debug::addTraceCategory("PrinterPrintTestLabel");
#endif
	}

	Debug::trace("PrinterPrintTestLabel", "PrinterPrintTestLabel::show() ");

	PrinterGeneric::show();

	if (prTestLabel == PrNoTestLabel)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestLabel: PrNoTestLabel"));
	}
	else if (prTestLabel == PrVoidLabel)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestLabel: PrVoidLabel"));
	}
	else if (prTestLabel == PrCheckerboard)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestLabel: PrCheckerboard"));
	}
	else if (prTestLabel == PrHorizontalBars)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestLabel: PrHorizontalBars"));
	}
	else if (prTestLabel == PrVerticalLines)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestLabel: PrVerticalLines"));
	}
	else if (prTestLabel == PrHighPower)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestLabel: PrHighPower"));
	}
	else if (prTestLabel == PrLabelSensorLabel)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestLabel: PrLabelSensorLabel"));
	}
	else
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestLabel: Unknown ") +
											  HobString(Integer::toString((UINT16) prTestLabel)));
	}

	if (quantity == PrOneTestLabel)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() quantity: PrOneTestLabel"));
	}
	else if (quantity == PrStartContinuous)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() quantity: PrStartContinuous"));
	}
	else if (quantity == PrStopContinuous)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() quantity: PrStopContinuous"));
	}
	else
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() quantity: Unknown ") +
											  HobString(Integer::toString((UINT16) quantity)));
	}

	if (prTestCommandOptions == PrNoSecurityLabel)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestCommandOptions: PrNoSecurityLabel"));
	}
	else if(prTestCommandOptions == PrApplySecurityLabel)
	{
		Debug::trace("PrinterPrintTestLabel", HobString("prTestCommandOptions: PrApplySecurityLabel"));
	}
	else
	{
		Debug::trace("PrinterPrintTestLabel", HobString("PrinterPrintTestLabel::show() prTestCommandOptions: Unknown") +
											  HobString(Integer::toString((UINT16) prTestCommandOptions)));
	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory( std::string("PrinterPrintTestLabel::show() PrintheadPower") );
	}
}

void PrinterPrintTestLabel::setNoTestLabel()
{
	prTestLabel = PrNoTestLabel;
}

bool PrinterPrintTestLabel::isNoTestLabel()
{
	return (prTestLabel == PrNoTestLabel);
}

void PrinterPrintTestLabel::setVoidLabel()
{
	prTestLabel = PrVoidLabel;
}

bool PrinterPrintTestLabel::isVoidLabel()
{
	return (prTestLabel == PrVoidLabel);
}

void PrinterPrintTestLabel::setCheckerboard()
{
	prTestLabel = PrCheckerboard;
}

bool PrinterPrintTestLabel::isCheckerboard()
{
	return (prTestLabel == PrCheckerboard);
}

void PrinterPrintTestLabel::setLabelSensorLabel()
{
	prTestLabel = PrLabelSensorLabel;
}

bool PrinterPrintTestLabel::isLabelSensorLabel()
{
	return (prTestLabel == PrLabelSensorLabel);
}

void PrinterPrintTestLabel::setHorizontalBars()
{
	prTestLabel = PrHorizontalBars;
}

bool PrinterPrintTestLabel::isHorizontalBars()
{
	return (prTestLabel == PrHorizontalBars);
}

void PrinterPrintTestLabel::setVerticalLines()
{
	prTestLabel = PrVerticalLines;
}

bool PrinterPrintTestLabel::isVerticalLines()
{
	return (prTestLabel == PrVerticalLines);
}

void PrinterPrintTestLabel::setHighPower()
{
	prTestLabel = PrHighPower;
}

bool PrinterPrintTestLabel::isHighPower()
{
	return (prTestLabel == PrHighPower);
}

void PrinterPrintTestLabel::setPrTestLabelType(PrTestLabelType prTestLabel)
{
	this->prTestLabel = prTestLabel;
}

PrTestLabelType PrinterPrintTestLabel::getPrTestLabelType()
{
	return prTestLabel;
}

void PrinterPrintTestLabel::setTestLabelQuantity(PrTestLabelQuantity quantity)
{
	this->quantity = quantity;
}

PrTestLabelQuantity PrinterPrintTestLabel::getTestLabelQuantity()
{
	return quantity;
}

void PrinterPrintTestLabel::setPrTestCommandOptions(PrinterCommandOptions prTestCommandOptions_)
{
	this->prTestCommandOptions = prTestCommandOptions_;
}

PrinterCommandOptions PrinterPrintTestLabel::getPrTestCommandOptions()
{
	return prTestCommandOptions;
}
