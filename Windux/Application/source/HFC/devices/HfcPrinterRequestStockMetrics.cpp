/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PAL_platformheader.h"

#include "Integer.h"
#include "HfcPrinterRequestStockMetrics.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterRequestStockMetrics::dataType = 0;
std::string HfcPrinterRequestStockMetrics::dataTypeName("HfcPrinterRequestStockMetrics"); 
#endif

HfcPrinterRequestStockMetrics::HfcPrinterRequestStockMetrics()
: PrinterRequestStockMetrics()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestStockMetrics::HfcPrinterRequestStockMetrics(const char *data)
: PrinterRequestStockMetrics(data)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

}

HfcPrinterRequestStockMetrics::HfcPrinterRequestStockMetrics(const char *data, int UNUSED(len), int formatType)
: PrinterRequestStockMetrics()
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

HfcPrinterRequestStockMetrics::~HfcPrinterRequestStockMetrics()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterRequestStockMetrics::serialize(int format) const
{
    switch (format)
    {
        case InternalFormat:
            {
                const char *data = PrinterRequestStockMetrics::serialize();
                int datalen = PrinterRequestStockMetrics::getSerializedLength();
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
