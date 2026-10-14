/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "HfcPrinterRequestInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterRequestInquire::dataType = 0;
std::string HfcPrinterRequestInquire::dataTypeName("HfcPrinterRequestInquire"); 
#endif

HfcPrinterRequestInquire::HfcPrinterRequestInquire()
	: PrinterRequestInquire()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestInquire::HfcPrinterRequestInquire(const char *data)
	: PrinterRequestInquire(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestInquire::HfcPrinterRequestInquire(const char *data, int UNUSED(len), int formatType)
	: PrinterRequestInquire()
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
			setPrinterType((PrinterType) Integer::valueOf(data));
			break;

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterRequestInquire::~HfcPrinterRequestInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterRequestInquire::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			const char *data = PrinterRequestInquire::serialize();
			int datalen = PrinterRequestInquire::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
		break;

		case BrowserFormat:
			HfcObject::serializedData = "Serialized Printer Request Inquire : browser format";
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer Request Inquire : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
