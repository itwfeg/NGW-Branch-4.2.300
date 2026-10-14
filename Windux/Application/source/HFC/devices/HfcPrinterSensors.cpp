/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterSensors.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterSensors::dataType = 0;
std::string HfcPrinterSensors::dataTypeName("HfcPrinterSensors"); 
#endif

HfcPrinterSensors::HfcPrinterSensors(const char *data)
	: PrinterSensors(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSensors::HfcPrinterSensors(const char *data, int len, int formatType)
	: PrinterSensors()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetPSPrinterType:
						setPrinterType((PrinterType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSHeadup:
						setHeadUp((PrinterHeadUpType) Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetPSHeadTemperature:
						setHeadTemperature((UINT16)Double::valueOf(token.c_str()));
						break;
					case fieldOffsetPSLabelWidth:
						token.unQuoteForCSV();
						setLabelWidth((LabelWidth) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSLabel:
						token.unQuoteForCSV();
						setLabel(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSMedia:
						setMedia(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSHeadVoltage:
						setHeadVoltage(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSRegulatorTemperature:
						/* re-use for freestanding service scale to report low stock and engine state */
						setRegulatorTemperature(Integer::valueOf(token.c_str()));
						break;
/*						
					case fieldOffsetPSHeadCurrent:
						setHeadCurrent(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSLabelHighAverage:
						setLabelHighAverage(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSLabelLowAverage:
						setLabelLowAverage(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSLabelThreshold:
						setLabelThreshold(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSMediaHighAverage:
						setMediaHighAverage(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSMediaLowAverage:
						setMediaLowAverage(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSMediaThreshold:
						setMediaThreshold(Integer::valueOf(token.c_str()));
						break;
*/
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterSensors::~HfcPrinterSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterSensors::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;
	HobString stringForQuoting;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = PrinterSensors::serialize();
			msglen = PrinterSensors::getSerializedLength();

			HfcObject::serializedData = PrinterSensors::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
//			HfcObject::serializedData = "Serialized Printer Sensors : browser format";
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrinterSensorsFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
 					case fieldOffsetPSPrinterType:
						HfcObject::serializedData.append(Integer::toString(getPrinterType()));
						break;
					case fieldOffsetPSHeadup:
						HfcObject::serializedData.append(Bool::toString(isHeadDisengaged()));
						break;
					case fieldOffsetPSHeadTemperature:
						HfcObject::serializedData.append(Integer::toString(getHeadTemperature()));
						break;
					case fieldOffsetPSLabelWidth:
						stringForQuoting = Integer::toString(getLabelWidth());
						stringForQuoting.quoteForCSV();
						HfcObject::serializedData.append(stringForQuoting);
						break;
					case fieldOffsetPSLabel:
						HfcObject::serializedData.append(Integer::toString(getLabel()));
						break;
					case fieldOffsetPSMedia:
						HfcObject::serializedData.append(Integer::toString(getMedia()));
						break;
					case fieldOffsetPSHeadVoltage:
						stringForQuoting = Double::toString(getHeadVoltage(), 2, 1);
						stringForQuoting.quoteForCSV();
						HfcObject::serializedData.append(stringForQuoting);
						
						//HfcObject::serializedData.append(Double::toString(getHeadVoltage(), 2, 1));
						break;
					case fieldOffsetPSRegulatorTemperature:
						/* re-use for freestanding service scale to report low stock and engine state */
						HfcObject::serializedData.append(Integer::toString(getRegulatorTemperature()));
						break;
/*						
					case fieldOffsetPSHeadCurrent:
						HfcObject::serializedData.append(Integer::toString(getHeadCurrent()));
						break;
					case fieldOffsetPSLabelHighAverage:
						HfcObject::serializedData.append(Integer::toString(getLabelHighAverage()));
						break;
					case fieldOffsetPSLabelLowAverage:
						HfcObject::serializedData.append(Integer::toString(getLabelLowAverage()));
						break;
					case fieldOffsetPSLabelThreshold:
						HfcObject::serializedData.append(Integer::toString(getLabelThreshold()));
						break;
					case fieldOffsetPSMediaHighAverage:
						HfcObject::serializedData.append(Integer::toString(getMediaHighAverage()));
						break;
					case fieldOffsetPSMediaLowAverage:
						HfcObject::serializedData.append(Integer::toString(getMediaLowAverage()));
						break;
					case fieldOffsetPSMediaThreshold:
						HfcObject::serializedData.append(Integer::toString(getMediaThreshold()));
						break;
*/
					default:
						break;
				}
				loopCnt++;
			}

			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer Sensors : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
