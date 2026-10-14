/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterRequestLabelSizing.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterRequestLabelSizing::dataType = 0;
std::string HfcPrinterRequestLabelSizing::dataTypeName("HfcPrinterRequestLabelSizing"); 
#endif

HfcPrinterRequestLabelSizing::HfcPrinterRequestLabelSizing()
	: PrinterGeneric(PrReqLabelSizing, 0, MaxPrinterTypes)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataTy, pe(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestLabelSizing::HfcPrinterRequestLabelSizing(const char *data)
	: PrinterGeneric(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestLabelSizing::HfcPrinterRequestLabelSizing(const char *data, int UNUSED(len), int formatType)
	: PrinterGeneric(PrReqLabelSizing, 0, MaxPrinterTypes)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case BrowserFormat:
			setPrinterType((PrinterType) Integer::valueOf(data));
			break;

		default:
			break;
	}
}

HfcPrinterRequestLabelSizing::~HfcPrinterRequestLabelSizing()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterRequestLabelSizing::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			const char *data = PrinterGeneric::serialize();
			int datalen = PrinterGeneric::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
		break;

		default:
			break;
	}
	return HfcObject::serializedData.data();
}
