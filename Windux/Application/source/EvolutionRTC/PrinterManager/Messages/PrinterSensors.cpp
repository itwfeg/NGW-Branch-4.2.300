//#include "vxWorks.h"

#include "PrinterSensors.h"

#include "Debug.h"
#include "Bool.h"
#include "Integer.h"
#include "UInteger.h"
#include "Double.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSensors::dataType = 0;
std::string PrinterSensors::dataTypeName("PrinterSensors"); 
#endif

const double PrinterSensors::headVoltageFactor = 0.2071;

const double PrinterSensors::headVoltageFactorEpp = 0.02784;


PrinterSensors::PrinterSensors(Hobart::CEvolMsgReceiver *pHobThread,
							   PrinterType printerType,
							   PrinterHeadUpType headup,
							   LabelWidth labelWidth,
							   UINT16 regulatorTemperature,
							   UINT16 headTemperature,
							   double headVoltage,
							   UINT16 headCurrent,
							   UINT16 labelHighAverage,
							   UINT16 labelLowAverage,
							   UINT16 labelThreshold,
							   UINT16 label,
							   UINT16 mediaHighAverage,
							   UINT16 mediaLowAverage,
							   UINT16 mediaThreshold,
							   UINT16 media) :	PrinterGeneric(PrSensors, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->headup = headup;
	this->labelWidth = labelWidth;
	this->regulatorTemperature = regulatorTemperature;
	this->headTemperature = headTemperature;
	this->headVoltage = headVoltage;
	this->headCurrent = headCurrent;
	this->labelHighAverage = labelHighAverage;
	this->labelLowAverage = labelLowAverage;
	this->labelThreshold = labelThreshold;
	this->label = label;
	this->mediaHighAverage = mediaHighAverage;
	this->mediaLowAverage = mediaLowAverage;
	this->mediaThreshold = mediaThreshold;
	this->media = media;
}

PrinterSensors::PrinterSensors(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &headup;
	for (unsigned int i = 0; i < sizeof(headup); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &regulatorTemperature;
	for (unsigned int i = 0; i < sizeof(regulatorTemperature); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &headTemperature;
	for (unsigned int i = 0; i < sizeof(headTemperature); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &headVoltage;
	for (unsigned int i = 0; i < sizeof(headVoltage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &headCurrent;
	for (unsigned int i = 0; i < sizeof(headCurrent); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelHighAverage;
	for (unsigned int i = 0; i < sizeof(labelHighAverage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelLowAverage;
	for (unsigned int i = 0; i < sizeof(labelLowAverage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelThreshold;
	for (unsigned int i = 0; i < sizeof(labelThreshold); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &label;
	for (unsigned int i = 0; i < sizeof(label); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mediaHighAverage;
	for (unsigned int i = 0; i < sizeof(mediaHighAverage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mediaLowAverage;
	for (unsigned int i = 0; i < sizeof(mediaLowAverage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &mediaThreshold;
	for (unsigned int i = 0; i < sizeof(mediaThreshold); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &media;
	for (unsigned int i = 0; i < sizeof(media); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterSensors::~PrinterSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterSensors::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &headup;
	for (unsigned int i = 0; i < sizeof(headup); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelWidth;
	for (unsigned int i = 0; i < sizeof(labelWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &regulatorTemperature;
	for (unsigned int i = 0; i < sizeof(regulatorTemperature); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &headTemperature;
	for (unsigned int i = 0; i < sizeof(headTemperature); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &headVoltage;
	for (unsigned int i = 0; i < sizeof(headVoltage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &headCurrent;
	for (unsigned int i = 0; i < sizeof(headCurrent); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelHighAverage;
	for (unsigned int i = 0; i < sizeof(labelHighAverage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelLowAverage;
	for (unsigned int i = 0; i < sizeof(labelLowAverage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelThreshold;
	for (unsigned int i = 0; i < sizeof(labelThreshold); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &label;
	for (unsigned int i = 0; i < sizeof(label); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mediaHighAverage;
	for (unsigned int i = 0; i < sizeof(mediaHighAverage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mediaLowAverage;
	for (unsigned int i = 0; i < sizeof(mediaLowAverage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &mediaThreshold;
	for (unsigned int i = 0; i < sizeof(mediaThreshold); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &media;
	for (unsigned int i = 0; i < sizeof(media); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterSensors::show()
{
#if 1
	if (Debug::isTracing("PrinterSensors"))
	{
#else
	#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("PrinterSensors") );
	#else
	Debug::addTraceCategory("PrinterSensors");
	#endif
#endif
	Debug::trace("PrinterSensors", "PrinterSensors: ");

	PrinterGeneric::show();

	Debug::trace("PrinterSensors", HobString("headEngaged: ") +
								   HobString(Bool::toString(isHeadEngaged())));

	if (labelWidth == NarrowLabel)
	{
		Debug::trace("PrinterSensors", HobString("labelWidth: NarrowLabel"));
	}
	else if (labelWidth == WideLabel)
	{
		Debug::trace("PrinterSensors", HobString("labelWidth: WideLabel"));
	}
	else if (labelWidth == UFWLabel)
	{
		Debug::trace("PrinterSensors", HobString("labelWidth: UFWLabel"));
	}
	else
	{
		Debug::trace("PrinterSensors", HobString("labelWidth: Unknown ") +
											 HobString(Integer::toString((UINT16) labelWidth)));
	}

	Debug::trace("PrinterSensors", HobString("regulatorTemperature: ") +
								   HobString(UInteger::toString(regulatorTemperature)));
	Debug::trace("PrinterSensors", HobString("headTemperature: ") +
								   HobString(UInteger::toString(headTemperature)));
	Debug::trace("PrinterSensors", HobString("headVoltage: ") +
								   HobString(Double::toString(headVoltage, 2, 1)));
	Debug::trace("PrinterSensors", HobString("headCurrent: ") +
								   HobString(UInteger::toString(headCurrent)));
	Debug::trace("PrinterSensors", HobString("labelHighAverage: ") +
								   HobString(UInteger::toString(labelHighAverage)));
	Debug::trace("PrinterSensors", HobString("labelLowAverage: ") +
								   HobString(UInteger::toString(labelLowAverage)));
	Debug::trace("PrinterSensors", HobString("labelThreshold: ") +
								   HobString(UInteger::toString(labelThreshold)));
	Debug::trace("PrinterSensors", HobString("label: ") +
								   HobString(UInteger::toString(label)));
	Debug::trace("PrinterSensors", HobString("mediaHighAverage: ") +
								   HobString(UInteger::toString(mediaHighAverage)));
	Debug::trace("PrinterSensors", HobString("mediaLowAverage: ") +
								   HobString(UInteger::toString(mediaLowAverage)));
	Debug::trace("PrinterSensors", HobString("mediaThreshold: ") +
								   HobString(UInteger::toString(mediaThreshold)));
	Debug::trace("PrinterSensors", HobString("media: ") +
								   HobString(UInteger::toString(media)));

#if 0
	#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("PrinterSensors") );
	#else
	Debug::removeTraceCategory("PrinterSensors");
	#endif
#else
	}
#endif
}

PrinterHeadUpType PrinterSensors::getHeadUp()
{
	return headup;
}

void PrinterSensors::setHeadUp(PrinterHeadUpType headup)
{
	if (headup >= PrinterHeadDisengagedThreshold)
	{
		this->headup = PrinterHeadDisengaged;
	}
	else
	{
		this->headup = PrinterHeadEngaged;
	}
}

bool PrinterSensors::isHeadDisengaged() const
{
	return (headup == PrinterHeadDisengaged);
}

bool PrinterSensors::isHeadEngaged() const
{
	return (headup == PrinterHeadEngaged);
}

LabelWidth PrinterSensors::getLabelWidth() const
{
	return labelWidth;
}

void PrinterSensors::setLabelWidth(LabelWidth labelWidth)
{
	if ((labelWidth == NarrowLabel) ||
		(labelWidth == WideLabel) ||
		(labelWidth == UFWLabel) ||
		(labelWidth == UnknownLabel))
	{
		this->labelWidth = labelWidth;
	}
}

bool PrinterSensors::isWideLabelStock() const
{
	return (labelWidth && WideLabel);
}

bool PrinterSensors::isNarrowLabelStock() const
{
	return (labelWidth && NarrowLabel);
}

bool PrinterSensors::isUWPLabelStock() const
{
	return (labelWidth && UFWLabel);
}

UINT16 PrinterSensors::getRegulatorTemperature() const
{
	return regulatorTemperature;
}

void PrinterSensors::setRegulatorTemperature(UINT16 regulatorTemperature)
{
	this->regulatorTemperature = regulatorTemperature;
}

UINT16 PrinterSensors::getHeadTemperature() const
{
	return headTemperature;
}

void PrinterSensors::setHeadTemperature(UINT16 headTemperature)
{
	this->headTemperature = headTemperature;
}

double PrinterSensors::getHeadVoltage() const
{
	return headVoltage;
}

void PrinterSensors::setHeadVoltage(double headVoltage)
{
	this->headVoltage = headVoltage;
}

void PrinterSensors::setHeadVoltageCounts(UINT16 headVoltageCounts, PrinterKind printerKind)
{
	if (headVoltageCounts == 65535)
	{
		headVoltage = -1.0;
	}
	else
	{
		if( ( ( printerKind == ApcPrinter ) || 
		      ( printerKind == KPCPrinter ) ||
			  ( printerKind == SSKPCPrinter ) ) )
			headVoltage = headVoltageCounts * headVoltageFactor;
		else
			headVoltage = headVoltageCounts * headVoltageFactorEpp;
	}
}

UINT16 PrinterSensors::getHeadCurrent() const
{
	return headCurrent;
}

void PrinterSensors::setHeadCurrent(UINT16 headCurrent)
{
	this->headCurrent = headCurrent;
}

UINT16 PrinterSensors::getLabelHighAverage() const
{
	return labelHighAverage;
}

void PrinterSensors::setLabelHighAverage(UINT16 labelHighAverage)
{
	this->labelHighAverage = labelHighAverage;
}

UINT16 PrinterSensors::getLabelLowAverage() const
{
	return labelLowAverage;
}

void PrinterSensors::setLabelLowAverage(UINT16 labelLowAverage)
{
	this->labelLowAverage = labelLowAverage;
}

UINT16 PrinterSensors::getLabelThreshold() const
{
	return labelThreshold;
}

void PrinterSensors::setLabelThreshold(UINT16 labelThreshold)
{
	this->labelThreshold = labelThreshold;
}

UINT16 PrinterSensors::getLabel() const
{
	return label;
}

void PrinterSensors::setLabel(UINT16 label)
{
	this->label = label;
}

UINT16 PrinterSensors::getMediaHighAverage() const
{
	return mediaHighAverage;
}

void PrinterSensors::setMediaHighAverage(UINT16 mediaHighAverage)
{
	this->mediaHighAverage = mediaHighAverage;
}

UINT16 PrinterSensors::getMediaLowAverage() const
{
	return mediaLowAverage;
}

void PrinterSensors::setMediaLowAverage(UINT16 mediaLowAverage)
{
	this->mediaLowAverage = mediaLowAverage;
}

UINT16 PrinterSensors::getMediaThreshold() const
{
	return mediaThreshold;
}

void PrinterSensors::setMediaThreshold(UINT16 mediaThreshold)
{
	this->mediaThreshold = mediaThreshold;
}

UINT16 PrinterSensors::getMedia() const
{
	return media;
}

void PrinterSensors::setMedia(UINT16 media)
{
	this->media = media;
}

bool PrinterSensors::equals(PrinterSensors &prSensors)
{
	bool rv = false;

	if ((headup == prSensors.getHeadUp()) &&
		(labelWidth == prSensors.getLabelWidth()) &&
		(headTemperature == prSensors.getHeadTemperature()) &&
		(headVoltage == prSensors.getHeadVoltage()) &&
		(label == prSensors.getLabel()) &&
		(media == prSensors.getMedia()))
	{
		rv = true;
	}

	return rv;
}
