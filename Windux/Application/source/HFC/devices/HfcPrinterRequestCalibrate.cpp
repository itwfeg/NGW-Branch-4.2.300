/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterRequestCalibrate.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterRequestCalibrate::dataType = 0;
std::string HfcPrinterRequestCalibrate::dataTypeName("HfcPrinterRequestCalibrate"); 
#endif

HfcPrinterRequestCalibrate::HfcPrinterRequestCalibrate()
	: PrinterRequestCalibrate()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestCalibrate::HfcPrinterRequestCalibrate(const char *data)
	: PrinterRequestCalibrate(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestCalibrate::HfcPrinterRequestCalibrate(const char *data, int len, int formatType)
	: PrinterRequestCalibrate()
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
					case fieldOffsetPrReqCalibratePrinterType:
						setPrinterType((PrinterType)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetPrReqCalibratePrinterCalibrateType:
						setPrinterCalibrateType((PrinterCalibrateType)Integer::valueOf(token.c_str()));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterRequestCalibrate::~HfcPrinterRequestCalibrate()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterRequestCalibrate::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			const char *data = PrinterRequestCalibrate::serialize();
			int datalen = PrinterRequestCalibrate::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
