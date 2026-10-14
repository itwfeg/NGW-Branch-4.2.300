/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterPrintTestLabel.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterPrintTestLabel::dataType = 0;
std::string HfcPrinterPrintTestLabel::dataTypeName("HfcPrinterPrintTestLabel"); 
#endif

HfcPrinterPrintTestLabel::HfcPrinterPrintTestLabel()
	: PrinterPrintTestLabel()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterPrintTestLabel::HfcPrinterPrintTestLabel(const char *data)
	: PrinterPrintTestLabel(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterPrintTestLabel::HfcPrinterPrintTestLabel(const char *data, int len, int formatType)
	: PrinterPrintTestLabel()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
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
					case fieldOffsetPrTestLabelPrinterType:
						Debug::trace("HfcPrinterPrintTestLabel()",HobString("fieldOffsetPrTestLabelPrinterType: ") + token);
						setPrinterType((PrinterType)Integer::valueOf(token));
						break;
					case fieldOffsetPrTestLabelLabelType:
						Debug::trace("HfcPrinterPrintTestLabel()",HobString("fieldOffsetPrTestLabelLabelType: ") + token);
						setPrTestLabelType((PrTestLabelType)Integer::valueOf(token));
						break;
					case fieldOffsetPrTestLabelContinuous:
						Debug::trace("HfcPrinterPrintTestLabel()",HobString("fieldOffsetPrTestLabelContinuous: ") + token);
						setTestLabelQuantity((PrTestLabelQuantity) Integer::valueOf(token));
						break;
					case fieldOffsetPrTestCommandOptions:
						Debug::trace("HfcPrinterPrintTestLabel()",HobString("fieldOffsetPrTestCommandOptions: ") + token);
						setPrTestCommandOptions((PrinterCommandOptions) Integer::valueOf(token));
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterPrintTestLabel::~HfcPrinterPrintTestLabel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterPrintTestLabel::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			const char *data = PrinterPrintTestLabel::serialize();
			int datalen = PrinterPrintTestLabel::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
