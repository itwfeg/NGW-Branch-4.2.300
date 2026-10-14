/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterPrinterTypeTracking.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterPrinterTypeTracking::dataType = 0;
std::string HfcPrinterPrinterTypeTracking::dataTypeName("HfcPrinterPrinterTypeTracking"); 
#endif

HfcPrinterPrinterTypeTracking::HfcPrinterPrinterTypeTracking(const char *data)
	: PrinterGeneric(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Session.PrinterType", "HfcPrinterPrinterTypeTracking(const char *data)");
}

HfcPrinterPrinterTypeTracking::HfcPrinterPrinterTypeTracking(const char *data, int len, int formatType)
	: PrinterGeneric(PrPrinterTypeTracking)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Session.PrinterType", "HfcPrinterPrinterTypeTracking(const char *data, int len, int formatType");
	Debug::trace("Session.PrinterType", HobString("HfcPrinterPrinterTypeTracking formatType: ") +
										Integer::toString(formatType));

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
					case fieldOffsetPPTTPrinterType:
						Debug::trace("Session.PrinterType", HobString("HfcPrinterPrinterTypeTracking printerType: ") +
															HobString(token.c_str()));
						setPrinterType((PrinterType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPPTTPrinterTypeTracking:
						Debug::trace("Session.PrinterType", HobString("HfcPrinterPrinterTypeTracking tracking: ") +
															HobString(token.c_str()));
						setPrinterTypeTrackingState((PrinterStateType) Integer::valueOf(token.c_str()));
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

HfcPrinterPrinterTypeTracking::~HfcPrinterPrinterTypeTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterPrinterTypeTracking::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			PrinterGeneric::serialize();
			PrinterGeneric::getSerializedLength();

			Debug::trace("Session.PrinterType", HobString("HfcPrinterPrinterTypeTracking getSerializedLength: ") +
												Integer::toString(PrinterGeneric::getSerializedLength()));

			HfcObject::serializedData = PrinterGeneric::serializedData;
			HfcObject::getSerializedLength();

			Debug::trace("Session.PrinterType", HobString("HfcPrinterPrinterTypeTracking getSerializedLength: ") +
												Integer::toString(HfcObject::getSerializedLength()));
		}
		break;

		case BrowserFormat:
			HfcObject::serializedData = "Serialized Printer PrinterType Tracking : browser format";
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer PrinterType Tracking : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcPrinterPrinterTypeTracking::setPrinterTypeTrackingState(PrinterStateType printerTypeTracking)
{
		this->printerTypeTracking = printerTypeTracking;
}

PrinterStateType HfcPrinterPrinterTypeTracking::getPrinterTypeTrackingState()
{
	return printerTypeTracking;
}
