/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "HfcPrinterRequestPrinterType.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterRequestPrinterType::_dataType_ = 0;
std::string HfcPrinterRequestPrinterType::_dataTypeName_("HfcPrinterRequestPrinterType"); 
#endif


HfcPrinterRequestPrinterType::HfcPrinterRequestPrinterType()
	: PrinterRequestPrinterType()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcPrinterRequestPrinterType::HfcPrinterRequestPrinterType(const char *data)
	: PrinterRequestPrinterType(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcPrinterRequestPrinterType::HfcPrinterRequestPrinterType(const char *data, int len, int formatType)
	: PrinterRequestPrinterType()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	switch (formatType)
	{
		case BrowserFormat:
			setPrinterType((PrinterType) Integer::valueOf(data));
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterRequestPrinterType::~HfcPrinterRequestPrinterType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcPrinterRequestPrinterType::serialize(int format)
{
	switch (format)
	{
		case InternalFormat:
		{
			const char *data = PrinterRequestPrinterType::serialize();
			int datalen = PrinterRequestPrinterType::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
