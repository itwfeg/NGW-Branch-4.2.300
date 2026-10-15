/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */



#include "HfcPrinterConfig.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "Long.h"
#include "Integer.h"
#include "Configuration.h"
#include "CMReaderWriterEvolTag.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterConfig::dataType = 0;
std::string HfcPrinterConfig::dataTypeName("HfcPrinterConfig"); 
#endif
						  
std::list<PrinterTags::PrinterTagFieldOffsets> HfcPrinterConfig::fieldList_FirstPrinter;
std::list<PrinterTags::PrinterTagFieldOffsets> HfcPrinterConfig::fieldList_SecondPrinter;
std::list<PrinterTags::PrinterTagFieldOffsets> HfcPrinterConfig::fieldList_ThirdPrinter;


HfcPrinterConfig::HfcPrinterConfig()
	: PrinterConfiguration(),
	  eepConfigNeeded(false),
	  dbSaveNeeded(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();

	horizPrintPosition = Configuration::getInstance()->getHorizLabelSkew(getPrinterType());
	Debug::trace("HfcPrinterConfig", HobString("HfcPrinterConfig()"));
}

HfcPrinterConfig::HfcPrinterConfig(const char *data)
	: PrinterConfiguration(data),
	  eepConfigNeeded(false),
	  dbSaveNeeded(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();

	horizPrintPosition = Configuration::getInstance()->getHorizLabelSkew(getPrinterType());
	Debug::trace("HfcPrinterConfig", HobString("HfcPrinterConfig2()"));
}

HfcPrinterConfig::HfcPrinterConfig(const char *data, int len, int formatType)
	: PrinterConfiguration(),
	  eepConfigNeeded(false),
	  dbSaveNeeded(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();

	horizPrintPosition = Configuration::getInstance()->getHorizLabelSkew(getPrinterType());

	switch (formatType)
	{
/* This case should never happen, we use the above constructor (with the data buffer)
   is used for internal-formatted data.
		case InternalFormat:	// Going to RTC
			break;
*/
		case BrowserFormat:
		{
			Debug::trace("HfcPrinterConfig", HobString("HfcPrinterConfig3(BrowserFormat)"));
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetPrPrinterType:
						setPrinterType((PrinterType)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrPrintContrast:
						setPrintContrast((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrHorizPrintPosition:
						setHorizPrintPosition((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrVertPrintPosition:
						setPrintPosition((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrLabelWidth:
						token.unQuoteForCSV();
						setLabelWidth((LabelWidth) (Integer::valueOf(token.c_str())));
						break;
					case fieldOffsetPrExpelPosition:	//	ConfigWord3
						setExpelPosition((UINT16)Integer::valueOf(token.c_str()));
						break;
/*
					case fieldOffsetPrConfigType:
						setConfigType((PrConfigType)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrDeviceSignature:
			 			setDeviceSignature((UINT8*) token.c_str());
						break;
					case fieldOffsetPrOutOfMediaCount:
						setOutOfMediaCount((UINT8)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord0:
						setConfigWord0((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord1:
						setConfigWord1((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord2:
						setConfigWord2((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord4:
						setConfigWord4((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord5:
						setConfigWord5((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord6:
						setConfigWord6((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord7:
						setConfigWord7((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord8:
						setConfigWord8((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord9:
						setConfigWord9((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord10:
						setConfigWord10((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord11:
						setConfigWord11((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord12:
						setConfigWord12((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrConfigWord13:
						setConfigWord13((UINT16)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrMediaSensorType:
						setMediaSensorType((MediaSensorType)Integer::valueOf(token.c_str()));
						break;
*/
					case fieldOffsetPrSlaEjectLine:
						setSLAEjectLine((UINT16)Integer::valueOf(token.c_str()));
						break;

					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case DBIndependentFormatFormat:
		case EvolutionTaggedFormat:
		{
			Debug::trace("HfcPrinterConfig", HobString("HfcPrinterConfig3(") + HobString(formatType == EvolutionTaggedFormat ? "EvolutionTaggedFormat" : "DBIndependentFormatFormat") + ")");
			PrinterTags printerTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0, tagOffset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				HobString tokenData = "";
				while (tok.next(token) != -1)
				{
					temp = token->getTag();
					tagOffset = (temp.size()) ? printerTags.getOffset(temp) : 0;
					switch (tagOffset)
					{
						case PrinterTags::fieldOffsetHorizontalPrintPosition1:
						case PrinterTags::fieldOffsetHorizontalPrintPosition2:
						case PrinterTags::fieldOffsetHorizontalPrintPosition3:
							offset = fieldOffsetPrHorizPrintPosition;
						break;

						case PrinterTags::fieldOffsetVerticalPrintPosition1:
						case PrinterTags::fieldOffsetVerticalPrintPosition2:
						case PrinterTags::fieldOffsetVerticalPrintPosition3:
							offset = fieldOffsetPrVertPrintPosition;
						break;

						case PrinterTags::fieldOffsetPrintContrast1:
						case PrinterTags::fieldOffsetPrintContrast2:
						case PrinterTags::fieldOffsetPrintContrast3:
							offset = fieldOffsetPrPrintContrast;
						break;
					}
					if (offset)
					{
						setReceived(offset, true);
						tokenData = token->getData();
						updateField(offset, tokenData);

						switch (tagOffset)
						{
							case PrinterTags::fieldOffsetHorizontalPrintPosition2:
							case PrinterTags::fieldOffsetVerticalPrintPosition2:
							case PrinterTags::fieldOffsetPrintContrast2:
								Debug::trace("HfcPrinterConfig", HobString("setPrinterType(SecondPrinter)"));
								setPrinterType(SecondPrinter);
								break;
							case PrinterTags::fieldOffsetHorizontalPrintPosition3:
							case PrinterTags::fieldOffsetVerticalPrintPosition3:
							case PrinterTags::fieldOffsetPrintContrast3:
								Debug::trace("HfcPrinterConfig", HobString("setPrinterType(ThirdPrinter)"));
								setPrinterType(ThirdPrinter);
								break;
							case PrinterTags::fieldOffsetHorizontalPrintPosition1:
							case PrinterTags::fieldOffsetVerticalPrintPosition1:
							case PrinterTags::fieldOffsetPrintContrast1:
							default:
								Debug::trace("HfcPrinterConfig", HobString("setPrinterType(FirstPrinter)"));
								setPrinterType(FirstPrinter);
								break;
						}
					}
				}
			}
			break;
		}

		default:
			Debug::trace("HfcPrinterConfig", HobString("HfcPrinterConfig3(default)"));
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterConfig::HfcPrinterConfig(HfcPrinterConfig *hpc)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::setPrinterType(hpc->getPrinterType());

	setConfigType(hpc->getConfigType());

	UINT8 deviceSignature1[SystemDevSignatureSize];
	hpc->getDeviceSignature(deviceSignature1);
	setDeviceSignature(deviceSignature1);

	setPrinterType(hpc->getPrinterType());
	setLabelWidth(hpc->getLabelWidth());
	setMediaSensorAdjustment(hpc->getMediaSensorAdjustment());
	setOutOfMediaCount(hpc->getOutOfMediaCount());
	setPrintContrast(hpc->getPrintContrast());
	setConfigWord0(hpc->getConfigWord0());
	setConfigWord1(hpc->getConfigWord1());
	setConfigWord2(hpc->getConfigWord2());
	setConfigWord3(hpc->getConfigWord3());
	setConfigWord4(hpc->getConfigWord4());
	setConfigWord5(hpc->getConfigWord5());
	setConfigWord6(hpc->getConfigWord6());
	setConfigWord7(hpc->getConfigWord7());
	setConfigWord8(hpc->getConfigWord8());
	setConfigWord9(hpc->getConfigWord9());
	setConfigWord10(hpc->getConfigWord10());
	setConfigWord11(hpc->getConfigWord11());
	setConfigWord12(hpc->getConfigWord12());
	setConfigWord13(hpc->getConfigWord13());
	setMediaSensorType(hpc->getMediaSensorType());
	setSLAEjectLine(hpc->getSLAEjectLine());
	setPrintPosition(hpc->getPrintPosition());
	setHorizPrintPosition(hpc->getHorizPrintPosition());
	setEepConfigNeeded(hpc->isEepConfigNeeded());
	setDbSaveNeeded(hpc->isDbSaveNeeded());
}

HfcPrinterConfig::~HfcPrinterConfig()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void HfcPrinterConfig::initFieldList()
{
	if (fieldList_FirstPrinter.empty())
	{
		fieldList_FirstPrinter.push_back(PrinterTags::fieldOffsetHorizontalPrintPosition1);
		fieldList_FirstPrinter.push_back(PrinterTags::fieldOffsetVerticalPrintPosition1);
		fieldList_FirstPrinter.push_back(PrinterTags::fieldOffsetPrintContrast1);

		PrinterTags printerTags((FieldOffsetsList *)&fieldList_FirstPrinter);
	}
	if (fieldList_SecondPrinter.empty())
	{
		fieldList_SecondPrinter.push_back(PrinterTags::fieldOffsetHorizontalPrintPosition2);
		fieldList_SecondPrinter.push_back(PrinterTags::fieldOffsetVerticalPrintPosition2);
		fieldList_SecondPrinter.push_back(PrinterTags::fieldOffsetPrintContrast2);

		PrinterTags printerTags((FieldOffsetsList *)&fieldList_SecondPrinter);
	}
	if (fieldList_ThirdPrinter.empty())
	{
		fieldList_ThirdPrinter.push_back(PrinterTags::fieldOffsetHorizontalPrintPosition3);
		fieldList_ThirdPrinter.push_back(PrinterTags::fieldOffsetVerticalPrintPosition3);
		fieldList_ThirdPrinter.push_back(PrinterTags::fieldOffsetPrintContrast3);

		PrinterTags printerTags((FieldOffsetsList *)&fieldList_ThirdPrinter);
	}
}

const char *HfcPrinterConfig::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;
	HobString strFormat = "";
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat:
		{
			strFormat = "InternalFormat";
			msgdata = PrinterConfiguration::serialize();
			msglen = PrinterConfiguration::getSerializedLength();

			HfcObject::serializedData = PrinterConfiguration::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			strFormat = "BrowserFormat";
			short loopCnt = 1;
			HobString stringForQuoting;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcPrinterConfigFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetPrPrinterType:
						HfcObject::serializedData.append(Integer::toString(getPrinterType()));
						break;
					case fieldOffsetPrPrintContrast:
						HfcObject::serializedData.append(Integer::toString(getPrintContrast()));
						break;
					case fieldOffsetPrVertPrintPosition:
						HfcObject::serializedData.append(Integer::toString(getPrintPosition()));
						break;
					case fieldOffsetPrHorizPrintPosition:
						HfcObject::serializedData.append(Integer::toString(getHorizPrintPosition()));
						break;
					case fieldOffsetPrLabelWidth:
						stringForQuoting = Integer::toString(getLabelWidth());
						stringForQuoting.quoteForCSV();
						HfcObject::serializedData.append(stringForQuoting);
						break;
					case fieldOffsetPrExpelPosition:
						HfcObject::serializedData.append(Integer::toString(getExpelPosition()));
						break;
/*
					case fieldOffsetPrConfigType:
						HfcObject::serializedData.append(Integer::toString(getConfigType()));
						break;
					case fieldOffsetPrDeviceSignature:
					{
						UINT8 tempDeviceSignature[SystemDevSignatureSize] = { 0 };

						getDeviceSignature((UINT8 *) &tempDeviceSignature);

						for (unsigned int i = 0; i < SystemDevSignatureSize; i++)
						{
							HfcObject::serializedData.append(1, tempDeviceSignature[i]);
						}
						msgdata = HfcObject::serializedData.c_str();
						msglen = HfcObject::getSerializedLength();
					}
						break;
					case fieldOffsetPrOutOfMediaCount:
						HfcObject::serializedData.append(Integer::toString(getOutOfMediaCount()));
						break;
					case fieldOffsetPrConfigWord0:
						HfcObject::serializedData.append(Integer::toString(getConfigWord0()));
						break;
					case fieldOffsetPrConfigWord1:
						HfcObject::serializedData.append(Integer::toString(getConfigWord1()));
						break;
					case fieldOffsetPrConfigWord2:
						HfcObject::serializedData.append(Integer::toString(getConfigWord2()));
						break;
					case fieldOffsetPrConfigWord4:
						HfcObject::serializedData.append(Integer::toString(getConfigWord4()));
						break;
					case fieldOffsetPrConfigWord5:
						HfcObject::serializedData.append(Integer::toString(getConfigWord5()));
						break;
					case fieldOffsetPrConfigWord6:
						HfcObject::serializedData.append(Integer::toString(getConfigWord6()));
						break;
					case fieldOffsetPrConfigWord7:
						HfcObject::serializedData.append(Integer::toString(getConfigWord7()));
						break;
					case fieldOffsetPrConfigWord8:
						HfcObject::serializedData.append(Integer::toString(getConfigWord8()));
						break;
					case fieldOffsetPrConfigWord9:
						HfcObject::serializedData.append(Integer::toString(getConfigWord9()));
						break;
					case fieldOffsetPrConfigWord10:
						HfcObject::serializedData.append(Integer::toString(getConfigWord10()));
						break;
					case fieldOffsetPrConfigWord11:
						HfcObject::serializedData.append(Integer::toString(getConfigWord11()));
						break;
					case fieldOffsetPrConfigWord12:
						HfcObject::serializedData.append(Integer::toString(getConfigWord12()));
						break;
					case fieldOffsetPrConfigWord13:
						HfcObject::serializedData.append(Integer::toString(getConfigWord13()));
						break;
					case fieldOffsetPrMediaSensorType:
						HfcObject::serializedData.append(Integer::toString(getMediaSensorType()));
						break;
*/
					case fieldOffsetPrSlaEjectLine:
						HfcObject::serializedData.append(Integer::toString(getSLAEjectLine()));
						break;

					default:
						break;
				}
				loopCnt++;
			}

			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
			break;
		}

//		case DBIndependentFormatFormat:
		case EvolutionTaggedFormat:
		{
			if (format == EvolutionTaggedFormat)
				strFormat = "EvolutionTaggedFormat";
			else
				strFormat = "DBIndependentFormatFormat";

			//	Initialize serializedData with the 'RT' & function code, because AbstractSession::send() won't do it.
			HfcObject::serializedData = CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER;

			char rtData[10];
			CMReaderWriterEvolTag::determineRecordType(rtData, getDeviceType(), getEventType());
			HfcObject::serializedData.append(rtData);
			HfcObject::serializedData.append(HobString(1, CMReaderWriterEvolTag::UNIT_SEPARATOR));		// ALS-5

			std::list<PrinterTags::PrinterTagFieldOffsets>::iterator fieldIter;
			PrinterTags printerTags;
			HobString tag = "";
			HobString fieldData = "";
			std::list<PrinterTags::PrinterTagFieldOffsets> *fieldList;

			switch (getPrinterType())
			{
				case SecondPrinter:
					fieldList = &fieldList_SecondPrinter;
					break;
				case ThirdPrinter:
					fieldList = &fieldList_ThirdPrinter;
					break;
				case FirstPrinter:
				default:
					fieldList = &fieldList_FirstPrinter;
					break;
			}
			
			fieldIter = fieldList->begin();
			while (fieldIter != fieldList->end())
			{
				tag = printerTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData);

					HfcObject::serializedData.append(tag);
					HfcObject::serializedData.append(fieldData);
					HfcObject::serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}
		
		default:
			// todo - throw exception("invalid format");
			strFormat = "Invalid format";
			Debug::trace("HfcPrinterConfig", "serialize(Invalid format!!!)");
			break;
	}
	if (format != InternalFormat)
		Debug::trace("HfcPrinterConfig", "serialize(" + strFormat + "):  [" + HfcObject::serializedData + "]");
	return HfcObject::serializedData.c_str();
}


void HfcPrinterConfig::getFieldAsString(int fieldCode, HobString &result) const
{
	HobString msgDebug = "getFieldAsString(";

	result = "";

	switch (fieldCode)
	{
		case PrinterTags::fieldOffsetHorizontalPrintPosition1:
		case PrinterTags::fieldOffsetHorizontalPrintPosition2:
		case PrinterTags::fieldOffsetHorizontalPrintPosition3:
			msgDebug.append("HorizontalPrintPosition");
			result = Integer::toString(getHorizPrintPosition());
			break;

		case PrinterTags::fieldOffsetVerticalPrintPosition1:
		case PrinterTags::fieldOffsetVerticalPrintPosition2:
		case PrinterTags::fieldOffsetVerticalPrintPosition3:
			msgDebug.append("VerticalPrintPosition");
			result = Integer::toString(getPrintPosition());
			break;

		case PrinterTags::fieldOffsetPrintContrast1:
		case PrinterTags::fieldOffsetPrintContrast2:
		case PrinterTags::fieldOffsetPrintContrast3:
			msgDebug.append("PrintContrast");
			result = Integer::toString(getPrintContrast());
			break;

		default:
			Debug::trace("HfcPrinterConfig", HobString("getFieldAsString():  Invalid offset!"));
			break;
	}
	msgDebug.append(Integer::toString(fieldCode)).append("):  ");
	Debug::trace("HfcPrinterConfig", msgDebug + "[" + result + "]");
}


void HfcPrinterConfig::update(HfcPrinterConfig *printerConfig)
{
	if (getPrinterType() != printerConfig->getPrinterType())
		return;

	short loopCnt = 1;
	while (loopCnt < endHfcPrinterConfigFieldOffsets)
	{
		if (printerConfig->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetPrPrinterType:
					// This field cannot be updated.  It is used to verify they are trying to update the
					// configuration for the printer for which they previously read the config.
					//setPrinterType(printerConfig->getPrinterType());
					break;

				case fieldOffsetPrPrintContrast:
					if (getPrintContrast() != printerConfig->getPrintContrast())
					{
						setPrintContrast(printerConfig->getPrintContrast());
						eepConfigNeeded = true;
					}
					break;

				case fieldOffsetPrHorizPrintPosition:
					if (getHorizPrintPosition() != printerConfig->getHorizPrintPosition())
					{
						setHorizPrintPosition(printerConfig->getHorizPrintPosition());
						dbSaveNeeded = true;
						eepConfigNeeded = true;
					}
					break;

				case fieldOffsetPrVertPrintPosition:
					if (getPrintPosition() != printerConfig->getPrintPosition())
					{
						setPrintPosition(printerConfig->getPrintPosition());
						eepConfigNeeded = true;
					}
					break;

				case fieldOffsetPrLabelWidth:
					if (getLabelWidth() != printerConfig->getLabelWidth())
					{
						setLabelWidth(printerConfig->getLabelWidth());
						eepConfigNeeded = true;
					}
					break;

				case fieldOffsetPrExpelPosition:	//	ConfigWord3
					if (getExpelPosition() != printerConfig->getExpelPosition())
					{
						setExpelPosition(printerConfig->getExpelPosition());
						eepConfigNeeded = true;
					}
					break;

/*
				case fieldOffsetPrConfigType:
					setConfigType(printerConfig->getConfigType());
					break;
				case fieldOffsetPrDeviceSignature:
				{
					UINT8 tempDeviceSignature[SystemDevSignatureSize] = { 0 };

					printerConfig->getDeviceSignature((UINT8 *) &tempDeviceSignature);
					setDeviceSignature(tempDeviceSignature);
				}
					break;
				case fieldOffsetPrOutOfMediaCount:
					setOutOfMediaCount(printerConfig->getOutOfMediaCount());
					break;
				case fieldOffsetPrConfigWord0:
					setConfigWord0(printerConfig->getConfigWord0());
					break;
				case fieldOffsetPrConfigWord1:
					setConfigWord1(printerConfig->getConfigWord1());
					break;
				case fieldOffsetPrConfigWord2:
					setConfigWord2(printerConfig->getConfigWord2());
					break;
				case fieldOffsetPrConfigWord4:
					setConfigWord4(printerConfig->getConfigWord4());
					break;
				case fieldOffsetPrConfigWord5:
					setConfigWord4(printerConfig->getConfigWord5());
					break;
				case fieldOffsetPrConfigWord6:
					setConfigWord4(printerConfig->getConfigWord6());
					break;
				case fieldOffsetPrConfigWord7:
					setConfigWord4(printerConfig->getConfigWord7());
					break;
				case fieldOffsetPrConfigWord8:
					setConfigWord4(printerConfig->getConfigWord8());
					break;
				case fieldOffsetPrConfigWord9:
					setConfigWord4(printerConfig->getConfigWord9());
					break;
				case fieldOffsetPrConfigWord10:
					setConfigWord4(printerConfig->getConfigWord10());
					break;
				case fieldOffsetPrConfigWord11:
					setConfigWord4(printerConfig->getConfigWord11());
					break;
				case fieldOffsetPrConfigWord12:
					setConfigWord4(printerConfig->getConfigWord12());
					break;
				case fieldOffsetPrConfigWord13:
					setConfigWord4(printerConfig->getConfigWord13());
					break;
				case fieldOffsetPrMediaSensorType:
					setMediaSensorType(printerConfig->getMediaSensorType());
					break;
*/
				case fieldOffsetPrSlaEjectLine:
					setSLAEjectLine(printerConfig->getSLAEjectLine());
					eepConfigNeeded = true;
					break;

				default:
					break;
			}
		}
		loopCnt++;
	}
}


void HfcPrinterConfig::updateField(int fieldCode, const HobString &token)
{
	if (fieldCode <= 0)
		return;

	if (Debug::isTracing("HfcPrinterConfig"))
	{
		Debug::trace("HfcPrinterConfig", "updateField() fieldCode = " + HobString(Integer::toString(fieldCode)) + "  token = " + token + "*");
	}
	bool foundField = true;
	switch (fieldCode)
	{
		case fieldOffsetPrHorizPrintPosition:
			setHorizPrintPosition(Integer::valueOf(token));
			break;

		case fieldOffsetPrVertPrintPosition:
			setPrintPosition(Integer::valueOf(token));
			break;

		case fieldOffsetPrPrintContrast:
			setPrintContrast(Integer::valueOf(token));
			break;

		default:
			foundField = false;
			break;
	}

	if (foundField)
		setReceived(fieldCode, true);
}

UINT16 HfcPrinterConfig::getExpelPosition() const
{
	return getConfigWord3();
}

void HfcPrinterConfig::setExpelPosition(UINT16 expelPosition)
{
	setConfigWord3(expelPosition);
}

UINT16 HfcPrinterConfig::getHorizPrintPosition() const
{
	return horizPrintPosition;
}

void HfcPrinterConfig::setHorizPrintPosition(UINT16 horizPrintPosition)
{
	this->horizPrintPosition = horizPrintPosition;
	dbSaveNeeded = true;

	Configuration::getInstance()->setHorizLabelSkew(getPrinterType(), horizPrintPosition);
}

bool HfcPrinterConfig::isEepConfigNeeded() const
{
	return eepConfigNeeded;
}

void HfcPrinterConfig::setEepConfigNeeded(bool eepConfigNeeded)
{
	this->eepConfigNeeded = eepConfigNeeded;
}

bool HfcPrinterConfig::isDbSaveNeeded() const
{
	return dbSaveNeeded;
}

void HfcPrinterConfig::setDbSaveNeeded(bool dbSaveNeeded)
{
	this->dbSaveNeeded = dbSaveNeeded;
}

void HfcPrinterConfig::show()
{
	if (!Debug::isTracing("HfcPrinterConfig"))
		return;

	Debug::trace("HfcPrinterConfig", "HfcPrinterConfig: ");

	PrinterConfiguration::show();

	Debug::trace("HfcPrinterConfig", HobString("Horizsontal printPosition: ") +
									 HobString(Integer::toString((UINT16) horizPrintPosition)));
}