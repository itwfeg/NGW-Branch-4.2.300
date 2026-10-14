/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterAdvanceLabelStock.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterAdvanceLabelStock::dataType = 0;
std::string HfcPrinterAdvanceLabelStock::dataTypeName("HfcPrinterAdvanceLabelStock"); 
#endif

HfcPrinterAdvanceLabelStock::HfcPrinterAdvanceLabelStock(const char *data)
	: PrinterAdvanceLabelStock(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterAdvanceLabelStock::HfcPrinterAdvanceLabelStock(const char *data, int len, int formatType)
	: PrinterAdvanceLabelStock()
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
					case fieldOffsetPALSPrinterType:
						setPrinterType((PrinterType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPALSNumSteps:
						setNumSteps(Integer::valueOf(token.c_str()));
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

HfcPrinterAdvanceLabelStock::~HfcPrinterAdvanceLabelStock()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcPrinterAdvanceLabelStock::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = PrinterAdvanceLabelStock::serialize();
			msglen = PrinterAdvanceLabelStock::getSerializedLength();

			HfcObject::serializedData = PrinterAdvanceLabelStock::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrinterAdvanceLabelStockFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
 					case fieldOffsetPALSPrinterType:
						HfcObject::serializedData.append(Integer::toString(getPrinterType()));
						break;
					case fieldOffsetPALSNumSteps:
						HfcObject::serializedData.append(Integer::toString(getNumSteps()));
						break;
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
			HfcObject::serializedData = "Serialized Printer Advance Label Stock : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
