/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterSensorsTracking.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterSensorsTracking::dataType = 0;
std::string HfcPrinterSensorsTracking::dataTypeName("HfcPrinterSensorsTracking"); 
#endif

HfcPrinterSensorsTracking::HfcPrinterSensorsTracking(const char *data)
	: PrinterSensorsTracking(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterSensorsTracking::HfcPrinterSensorsTracking(const char *data, int len, int formatType)
	: PrinterSensorsTracking()
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
					case fieldOffsetPSTPrinterType:
						setPrinterType((PrinterType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPSTSensorsTracking:
						setSensorsTrackingState((PrinterStateType) Integer::valueOf(token.c_str()));
						break;
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

HfcPrinterSensorsTracking::~HfcPrinterSensorsTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterSensorsTracking::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			PrinterSensorsTracking::serialize();   
			int msglen = PrinterSensorsTracking::getSerializedLength();

			HfcObject::serializedData = PrinterSensorsTracking::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
			HfcObject::serializedData = "Serialized Printer Sensors Tracking : browser format";
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer Sensors Tracking : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcPrinterSensorsTracking::setSensorsTrackingState(PrinterStateType sensorsTracking)
{
	if(sensorsTracking == PrinterOn)
		setSensorsTracking();
	else
		clearSensorsTracking();
}

PrinterStateType HfcPrinterSensorsTracking::getSensorsTrackingState()
{
	return getSensorsTracking();
}
