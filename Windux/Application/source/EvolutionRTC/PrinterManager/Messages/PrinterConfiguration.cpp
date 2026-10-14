//#include "vxWorks.h"
#include "PrinterConfiguration.h"
#include "PrinterInquire.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterConfiguration::dataType = 0;
std::string PrinterConfiguration::dataTypeName("PrinterConfiguration"); 
#endif

PrinterConfiguration::PrinterConfiguration(Hobart::CEvolMsgReceiver *pHobThread,
										   PrinterType printerTypeParam) : PrinterGeneric(PrConfig, pHobThread, printerTypeParam)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	//PrDefault means to have the printer reset to factory default values
	//so unless this field is specifically set it should be temporary.
	configType = PrTemporary;

	deviceSignature[0] = 'P';
	deviceSignature[1] = 'R';
	deviceSignature[2] = '\0';
	deviceSignature[3] = '\0';
	deviceSignature[4] = '\0';
	deviceSignature[5] = '\0';

	printerType = FirstPrinter;
	labelWidth = WideLabel;
	mediaSensorAdjustment = 130;
	outOfMediaCount = 200;	// Immutable!! Set to 200 on Ultima
	printContrast = 3;		// "Print Contrast" on Ultima Configuration.
	configWord0 = 0;
	configWord1 = 0;
	configWord2 = 0;
	configWord3 = 0;		// "Expel Position" on Ultima Configuration.
	configWord4 = 0;
	configWord5 = 0;
	configWord6 = 0;
	configWord7 = 0;
	configWord8 = 0;
	configWord9 = 0;
	configWord10 = 0;
	configWord11 = 0;
	configWord12 = 0;
	configWord13 = 0;
	mediaSensorType = PrReflective;
	slaEjectLine = 0;
	printPosition = 0;		// "Print Position" (SKEW) on Ultima Configuration. Only valid for LabelWidth = { Narrow | UFWLabel }
}

PrinterConfiguration::PrinterConfiguration(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &configType;
	for (unsigned int i = 0; i < sizeof(configType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mediaSensorAdjustment;
	for (unsigned int i = 0; i < sizeof(mediaSensorAdjustment); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &outOfMediaCount;
	for (unsigned int i = 0; i < sizeof(outOfMediaCount); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printContrast;
	for (unsigned int i = 0; i < sizeof(printContrast); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord0;
	for (unsigned int i = 0; i < sizeof(configWord0); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord1;
	for (unsigned int i = 0; i < sizeof(configWord1); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord2;
	for (unsigned int i = 0; i < sizeof(configWord2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord3;
	for (unsigned int i = 0; i < sizeof(configWord3); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord4;
	for (unsigned int i = 0; i < sizeof(configWord4); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord5;
	for (unsigned int i = 0; i < sizeof(configWord5); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord6;
	for (unsigned int i = 0; i < sizeof(configWord6); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord7;
	for (unsigned int i = 0; i < sizeof(configWord7); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord8;
	for (unsigned int i = 0; i < sizeof(configWord8); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord9;
	for (unsigned int i = 0; i < sizeof(configWord9); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord10;
	for (unsigned int i = 0; i < sizeof(configWord10); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord11;
	for (unsigned int i = 0; i < sizeof(configWord11); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord12;
	for (unsigned int i = 0; i < sizeof(configWord12); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &configWord13;
	for (unsigned int i = 0; i < sizeof(configWord13); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mediaSensorType;
	for (unsigned int i = 0; i < sizeof(mediaSensorType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &slaEjectLine;
	for (unsigned int i = 0; i < sizeof(slaEjectLine); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &printPosition;
	for (unsigned int i = 0; i < sizeof(printPosition); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterConfiguration::~PrinterConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterConfiguration::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &configType;
	for (unsigned int i = 0; i < sizeof(configType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printerType;
	for (unsigned int i = 0; i < sizeof(printerType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mediaSensorAdjustment;
	for (unsigned int i = 0; i < sizeof(mediaSensorAdjustment); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &outOfMediaCount;
	for (unsigned int i = 0; i < sizeof(outOfMediaCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printContrast;
	for (unsigned int i = 0; i < sizeof(printContrast); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord0;
	for (unsigned int i = 0; i < sizeof(configWord0); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord1;
	for (unsigned int i = 0; i < sizeof(configWord1); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord2;
	for (unsigned int i = 0; i < sizeof(configWord2); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord3;
	for (unsigned int i = 0; i < sizeof(configWord3); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord4;
	for (unsigned int i = 0; i < sizeof(configWord4); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord5;
	for (unsigned int i = 0; i < sizeof(configWord5); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord6;
	for (unsigned int i = 0; i < sizeof(configWord6); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord7;
	for (unsigned int i = 0; i < sizeof(configWord7); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord8;
	for (unsigned int i = 0; i < sizeof(configWord8); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord9;
	for (unsigned int i = 0; i < sizeof(configWord9); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord10;
	for (unsigned int i = 0; i < sizeof(configWord10); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord11;
	for (unsigned int i = 0; i < sizeof(configWord11); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord12;
	for (unsigned int i = 0; i < sizeof(configWord12); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &configWord13;
	for (unsigned int i = 0; i < sizeof(configWord13); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mediaSensorType;
	for (unsigned int i = 0; i < sizeof(mediaSensorType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &slaEjectLine;
	for (unsigned int i = 0; i < sizeof(slaEjectLine); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &printPosition;
	for (unsigned int i = 0; i < sizeof(printPosition); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterConfiguration::show()
{
	if (!Debug::isTracing("PrinterConfiguration"))
		return;

	Debug::trace("PrinterConfiguration", "PrinterConfiguration: ");

	PrinterGeneric::show();

	if (configType == PrPermanent)
	{
		Debug::trace("PrinterConfiguration", HobString("configType: PrPermanent"));
	}
	else if (configType == PrTemporary)
	{
		Debug::trace("PrinterConfiguration", HobString("configType: PrTemporary"));
	}
	else if (configType == PrDefault)
	{
		Debug::trace("PrinterConfiguration", HobString("configType: PrDefault"));
	}
	else if ((configType & PrCalibInProgress) == PrCalibInProgress)
	{
		Debug::trace("PrinterConfiguration", HobString("configType: PrCalibInProgress"));
	}
	else
	{
		Debug::trace("PrinterConfiguration", HobString("configType: Unknown ") +
											 HobString(Integer::toString((UINT16) configType)));
	}

	Debug::trace("PrinterConfiguration", Hex::toString((const char *) &deviceSignature[0], SystemDevSignatureSize));

	if (printerType == FirstPrinter)
	{
		Debug::trace("PrinterConfiguration", HobString("printerType: FirstPrinter"));
	}
	else if (printerType == SecondPrinter)
	{
		Debug::trace("PrinterConfiguration", HobString("printerType: SecondPrinter"));
	}
	else if (printerType == ThirdPrinter)
	{
		Debug::trace("PrinterConfiguration", HobString("printerType: ThirdPrinter"));
	}
	else
	{
		Debug::trace("PrinterConfiguration", HobString("printerType: Unknown ") +
											 HobString(Integer::toString((UINT16) printerType)));
	}
	
	if (labelWidth == WideLabel)
	{
		Debug::trace("PrinterConfiguration", HobString("labelWidth: WideLabel"));
	}
	else if (labelWidth == NarrowLabel)
	{
		Debug::trace("PrinterConfiguration", HobString("labelWidth: NarrowLabel"));
	}
	else if (labelWidth == UFWLabel)
	{
		Debug::trace("PrinterConfiguration", HobString("labelWidth: UFWLabel"));
	}
	else
	{
		Debug::trace("PrinterConfiguration", HobString("labelWidth: Unknown ") +
											 HobString(Integer::toString((UINT16) labelWidth)));
	}
	
	Debug::trace("PrinterConfiguration", HobString("mediaSensorAdjustment: ") +
										 HobString(Integer::toString(mediaSensorAdjustment)));
	Debug::trace("PrinterConfiguration", HobString("outOfMediaCount: ") +
										 HobString(Integer::toString(outOfMediaCount)));
	Debug::trace("PrinterConfiguration", HobString("printContrast: ") +
										 HobString(Integer::toString(printContrast)));

	Debug::trace("PrinterConfiguration", HobString("configWord0: ") +
										 HobString(Integer::toString(configWord0)));
	Debug::trace("PrinterConfiguration", HobString("configWord1: ") +
										 HobString(Integer::toString(configWord1)));
	Debug::trace("PrinterConfiguration", HobString("configWord2: ") +
										 HobString(Integer::toString(configWord2)));
	Debug::trace("PrinterConfiguration", HobString("configWord3: ") +
										 HobString(Integer::toString(configWord3)));
	Debug::trace("PrinterConfiguration", HobString("configWord4: ") +
										 HobString(Integer::toString(configWord4)));
	Debug::trace("PrinterConfiguration", HobString("configWord5: ") +
										 HobString(Integer::toString(configWord5)));
	Debug::trace("PrinterConfiguration", HobString("configWord6: ") +
										 HobString(Integer::toString(configWord6)));
	Debug::trace("PrinterConfiguration", HobString("configWord7: ") +
										 HobString(Integer::toString(configWord7)));
	Debug::trace("PrinterConfiguration", HobString("configWord8: ") +
										 HobString(Integer::toString(configWord8)));
	Debug::trace("PrinterConfiguration", HobString("configWord9: ") +
										 HobString(Integer::toString(configWord9)));
	Debug::trace("PrinterConfiguration", HobString("configWord10: ") +
										 HobString(Integer::toString(configWord10)));
	Debug::trace("PrinterConfiguration", HobString("configWord11: ") +
										 HobString(Integer::toString(configWord11)));
	Debug::trace("PrinterConfiguration", HobString("configWord12: ") +
										 HobString(Integer::toString(configWord12)));
	Debug::trace("PrinterConfiguration", HobString("configWord13: ") +
										 HobString(Integer::toString(configWord13)));

	if (mediaSensorType == PrShootThrough)
	{
		Debug::trace("PrinterConfiguration", HobString("mediaSensorType: PrShootThrough"));
	}
	else if (mediaSensorType == PrReflective)
	{
		Debug::trace("PrinterConfiguration", HobString("mediaSensorType: PrReflective"));
	}
	else
	{
		Debug::trace("PrinterConfiguration", HobString("mediaSensorType: Unknown ") +
											 HobString(Integer::toString((UINT16) mediaSensorType)));
	}
	
	Debug::trace("PrinterConfiguration", HobString("slaEjectLine: ") +
										 HobString(Integer::toString(slaEjectLine)));
	Debug::trace("PrinterConfiguration", HobString("Vertical printPosition: ") +
										 HobString(Integer::toString(printPosition)));
}

void PrinterConfiguration::clear()
{
	configType = PrPermanent;

	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = '\0';
	}

	printerType = FirstPrinter;
	labelWidth = WideLabel;
	mediaSensorAdjustment = 0;
	outOfMediaCount = 200;	// Immutable!! Set to 200 on Ultima
	printContrast = 0;		// "Print Contrast" on Ultima Configuration.

	configWord0 = 0;
	configWord1 = 0;
	configWord2 = 0;
	configWord3 = 0;
	configWord4 = 0;
	configWord5 = 0;
	configWord6 = 0;
	configWord7 = 0;
	configWord8 = 0;
	configWord9 = 0;
	configWord10 = 0;
	configWord11 = 0;
	configWord12 = 0;
	configWord13 = 0;
	mediaSensorType = PrShootThrough;
	slaEjectLine = 0;
	printPosition = 0;		// "Print Position" (SKEW) on Ultima Configuration. Only valid for LabelWidth = { Narrow | UFWLabel }
}

bool PrinterConfiguration::update(PrinterInquire &prInquire)
{
	bool rv = false;

	if (printerType != prInquire.getPrinterType())
	{
		printerType = prInquire.getPrinterType();
		rv = true;
	}

	if (labelWidth != prInquire.getLabelWidth())
	{
		labelWidth = prInquire.getLabelWidth();
		rv = true;
	}

	if (mediaSensorAdjustment != prInquire.getMediaSensorAdjustment())
	{
		mediaSensorAdjustment = prInquire.getMediaSensorAdjustment();
		rv = true;
	}

	if (outOfMediaCount != prInquire.getOutOfMediaCount())
	{
		outOfMediaCount = prInquire.getOutOfMediaCount();
		rv = true;
	}

	if (printContrast != prInquire.getPrintContrast())
	{
		printContrast = prInquire.getPrintContrast();
		rv = true;
	}

	if (configWord0 != prInquire.getConfigWord0())
	{
		configWord0 = prInquire.getConfigWord0();
		rv = true;
	}

	if (configWord1 != prInquire.getConfigWord1())
	{
		configWord1 = prInquire.getConfigWord1();
		rv = true;
	}

	if (configWord2 != prInquire.getConfigWord2())
	{
		configWord2 = prInquire.getConfigWord2();
		rv = true;
	}

	if (configWord3 != prInquire.getConfigWord3())
	{
		configWord3 = prInquire.getConfigWord3();
		rv = true;
	}

	if (configWord4 != prInquire.getConfigWord4())
	{
		configWord4 = prInquire.getConfigWord4();
		rv = true;
	}

	if (configWord5 != prInquire.getConfigWord5())
	{
		configWord5 = prInquire.getConfigWord5();
		rv = true;
	}

	if (configWord6 != prInquire.getConfigWord6())
	{
		configWord6 = prInquire.getConfigWord6();
		rv = true;
	}

	if (configWord7 != prInquire.getConfigWord7())
	{
		configWord7 = prInquire.getConfigWord7();
		rv = true;
	}

	if (configWord8 != prInquire.getConfigWord8())
	{
		configWord8 = prInquire.getConfigWord8();
		rv = true;
	}

	if (configWord9 != prInquire.getConfigWord9())
	{
		configWord9 = prInquire.getConfigWord9();
		rv = true;
	}

	if (configWord10 != prInquire.getConfigWord10())
	{
		configWord10 = prInquire.getConfigWord10();
		rv = true;
	}

	if (configWord11 != prInquire.getConfigWord11())
	{
		configWord11 = prInquire.getConfigWord11();
		rv = true;
	}

	if (configWord12 != prInquire.getConfigWord12())
	{
		configWord12 = prInquire.getConfigWord12();
		rv = true;
	}

	if (configWord13 != prInquire.getConfigWord13())
	{
		configWord13 = prInquire.getConfigWord13();
		rv = true;
	}

	if (mediaSensorType != prInquire.getMediaSensorType())
	{
		mediaSensorType = prInquire.getMediaSensorType();
		rv = true;
	}

	if (slaEjectLine != prInquire.getSLAEjectLine())
	{
		slaEjectLine = prInquire.getSLAEjectLine();
		rv = true;
	}

	if (printPosition != prInquire.getPrintPosition())
	{
		printPosition = prInquire.getPrintPosition();
		rv = true;
	}

	return rv;
}

bool PrinterConfiguration::equals(PrinterConfiguration &prConfig)
{
	bool rv = false;

	if ((printerType == prConfig.getPrinterType()) &&
		(labelWidth == prConfig.getLabelWidth()) &&
		(mediaSensorAdjustment == prConfig.getMediaSensorAdjustment()) &&
		(outOfMediaCount == prConfig.getOutOfMediaCount()) &&
		(printContrast == prConfig.getPrintContrast()) &&
		(configWord0 == prConfig.getConfigWord0()) &&
		(configWord1 == prConfig.getConfigWord1()) &&
		(configWord2 == prConfig.getConfigWord2()) &&
		(configWord3 == prConfig.getConfigWord3()) &&
		(configWord4 == prConfig.getConfigWord4()) &&
		(configWord5 == prConfig.getConfigWord5()) &&
		(configWord6 == prConfig.getConfigWord6()) &&
		(configWord7 == prConfig.getConfigWord7()) &&
		(configWord8 == prConfig.getConfigWord8()) &&
		(configWord9 == prConfig.getConfigWord9()) &&
		(configWord10 == prConfig.getConfigWord10()) &&
		(configWord11 == prConfig.getConfigWord11()) &&
		(configWord12 == prConfig.getConfigWord12()) &&
		(configWord13 == prConfig.getConfigWord13()) &&
		(mediaSensorType == prConfig.getMediaSensorType()) &&
		(slaEjectLine == prConfig.getSLAEjectLine()) &&
		(printPosition == prConfig.getPrintPosition()))
	{
		rv = true;
	}

	return rv;
}

PrConfigType PrinterConfiguration::getConfigType() const
{
	return configType;
}

void PrinterConfiguration::setConfigType(PrConfigType configType)
{
	this->configType = configType;
}

void PrinterConfiguration::setDeviceSignature(const UINT8 *deviceSignature)
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		this->deviceSignature[i] = deviceSignature[i];
	}
}

void PrinterConfiguration::getDeviceSignature(UINT8 *deviceSignature)
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = this->deviceSignature[i];
	}
}

PrinterType PrinterConfiguration::getPrinterType() const
{
	return printerType;
}

void PrinterConfiguration::setPrinterType(PrinterType printerType)
{
	this->printerType = printerType;
}

LabelWidth PrinterConfiguration::getLabelWidth() const
{
	return labelWidth;
}

void PrinterConfiguration::setLabelWidth(LabelWidth labelWidth)
{
	this->labelWidth = labelWidth;
}

UINT8 PrinterConfiguration::getMediaSensorAdjustment() const
{
	return mediaSensorAdjustment;
}

void PrinterConfiguration::setMediaSensorAdjustment(UINT8 mediaSensorAdjustment)
{
	this->mediaSensorAdjustment = mediaSensorAdjustment;
}

UINT8 PrinterConfiguration::getOutOfMediaCount() const
{
	return outOfMediaCount;
}

void PrinterConfiguration::setOutOfMediaCount(UINT8 outOfMediaCount)
{
	this->outOfMediaCount = outOfMediaCount;
}

UINT16 PrinterConfiguration::getPrintContrast() const
{
	return printContrast;
}

void PrinterConfiguration::setPrintContrast(UINT16 printContrast)
{
	this->printContrast = printContrast;
}

UINT16 PrinterConfiguration::getConfigWord0() const
{
	return configWord0;
}

void PrinterConfiguration::setConfigWord0(UINT16 configWord0)
{
	this->configWord0 = configWord0;
}

UINT16 PrinterConfiguration::getConfigWord1() const
{
	return configWord1;
}

void PrinterConfiguration::setConfigWord1(UINT16 configWord1)
{
	this->configWord1 = configWord1;
}

UINT16 PrinterConfiguration::getConfigWord2() const
{
	return configWord2;
}

void PrinterConfiguration::setConfigWord2(UINT16 configWord2)
{
	this->configWord2 = configWord2;
}

UINT16 PrinterConfiguration::getConfigWord3() const
{
	return configWord3;
}

void PrinterConfiguration::setConfigWord3(UINT16 configWord3)
{
	this->configWord3 = configWord3;
}

UINT16 PrinterConfiguration::getConfigWord4() const
{
	return configWord4;
}

void PrinterConfiguration::setConfigWord4(UINT16 configWord4)
{
	this->configWord4 = configWord4;
}

UINT16 PrinterConfiguration::getConfigWord5() const
{
	return configWord5;
}

void PrinterConfiguration::setConfigWord5(UINT16 configWord5)
{
	this->configWord5 = configWord5;
}

UINT16 PrinterConfiguration::getConfigWord6() const
{
	return configWord6;
}

void PrinterConfiguration::setConfigWord6(UINT16 configWord6)
{
	this->configWord6 = configWord6;
}

UINT16 PrinterConfiguration::getConfigWord7() const
{
	return configWord7;
}

void PrinterConfiguration::setConfigWord7(UINT16 configWord7)
{
	this->configWord7 = configWord7;
}

UINT16 PrinterConfiguration::getConfigWord8() const
{
	return configWord8;
}

void PrinterConfiguration::setConfigWord8(UINT16 configWord8)
{
	this->configWord8 = configWord8;
}

UINT16 PrinterConfiguration::getConfigWord9() const
{
	return configWord9;
}

void PrinterConfiguration::setConfigWord9(UINT16 configWord9)
{
	this->configWord9 = configWord9;
}

UINT16 PrinterConfiguration::getConfigWord10() const
{
	return configWord10;
}

void PrinterConfiguration::setConfigWord10(UINT16 configWord10)
{
	this->configWord10 = configWord10;
}

UINT16 PrinterConfiguration::getConfigWord11() const
{
	return configWord11;
}

void PrinterConfiguration::setConfigWord11(UINT16 configWord11)
{
	this->configWord11 = configWord11;
}

UINT16 PrinterConfiguration::getConfigWord12() const
{
	return configWord12;
}

void PrinterConfiguration::setConfigWord12(UINT16 configWord12)
{
	this->configWord12 = configWord12;
}

UINT16 PrinterConfiguration::getConfigWord13() const
{
	return configWord13;
}

void PrinterConfiguration::setConfigWord13(UINT16 configWord13)
{
	this->configWord13 = configWord13;
}

MediaSensorType PrinterConfiguration::getMediaSensorType() const
{
	return mediaSensorType;
}

void PrinterConfiguration::setMediaSensorType(MediaSensorType mediaSensorType)
{
	this->mediaSensorType = mediaSensorType;
}

UINT16 PrinterConfiguration::getSLAEjectLine() const
{
	return slaEjectLine;
}

void PrinterConfiguration::setSLAEjectLine(UINT16 slaEjectLine)
{
	this->slaEjectLine = slaEjectLine;
}

UINT16 PrinterConfiguration::getPrintPosition() const
{
	return printPosition;
}

void PrinterConfiguration::setPrintPosition(UINT16 printPosition)
{
	this->printPosition = printPosition;
}

UINT16 PrinterConfiguration::getPrintPositionAbsolute() const
{
	return printPosition - 50;
}

void PrinterConfiguration::setPrintPositionAbsolute(UINT16 printPosition)
{
	this->printPosition = printPosition + 50;
}
