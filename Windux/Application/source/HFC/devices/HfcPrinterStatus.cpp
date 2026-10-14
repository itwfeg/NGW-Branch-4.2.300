/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterStatus.h"
#include "Configuration.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterStatus::_dataType_ = 0;
std::string HfcPrinterStatus::_dataTypeName_("HfcPrinterStatus");
#endif

HfcPrinterStatus::HfcPrinterStatus()
	: PrinterStatus()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcPrinterStatus::HfcPrinterStatus(const char *data)
	: PrinterStatus(data)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcPrinterStatus::HfcPrinterStatus(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType) )
	: PrinterStatus()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    //  switch (formatType)
    //  {
    /* This case should never happen, we use the above constructor (with the data buffer)
       is used for internal-formatted data.
       case InternalFormat:
       setAllReceived(true);
    // Nothing else to do, the PrinterStatus was filled in during initialization of this object.
    break;
    */
    //          default:
    // todo - throw exception("invalid format");
    //                  break;
    //  }
}

HfcPrinterStatus::~HfcPrinterStatus()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcPrinterStatus::serialize(int format) const
{
    HfcObject::serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    switch (format)
    {
        case InternalFormat:
            {
                const char *data = PrinterStatus::serialize();
                int datalen = PrinterStatus::getSerializedLength();
                HfcObject::serializedData.assign(data, datalen);
            }
            break;

        default:
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.data();
}


bool HfcPrinterStatus::operator==(HfcPrinterStatus &hps)
{
    return (getState() == hps.getState() &&
            getError() == hps.getError() &&
            getSensorMask() == hps.getSensorMask() &&
            getUserMask() == hps.getUserMask()) ? true : false;
}

bool HfcPrinterStatus::notificationReasonExists()
{
	bool result = false;
	if (isHeadDisengaged() ||
		(isLabelSizing() && !isTakeLabelError()) ||
		isMediaJammed() ||
		//isLabelMissing() ||   commented out per FT-448
		isOutOfMedia() ||
		isCassetteMissing() ||
		isThermalOverload() ||
		isThermalShutdown() ||
		isHardwareError() ||
		isMediaShutdown() ||
		!isPeripheralComm() ||
		isManagerDeleted() ||
		isLabelTakenError() ||
		isTakeLabelError())
		result = true;

	if ( result == true && Debug::isTracing( "PrinterStatus" ) )
	{
		if (isHeadDisengaged())		Debug::trace("PrinterStatus", "notificationReasonExists: HeadDisengaged");
		if (isLabelSizing() && !isTakeLabelError())	Debug::trace("PrinterStatus", "notificationReasonExists: LabelSizing && !TakeLabelError");
		if (isMediaJammed())		Debug::trace("PrinterStatus", "notificationReasonExists: MediaJammed");
		if (isOutOfMedia())			Debug::trace("PrinterStatus", "notificationReasonExists: OutOfMedia");
		if (isCassetteMissing())	Debug::trace("PrinterStatus", "notificationReasonExists: CassetteMissing");
		if (isThermalOverload())	Debug::trace("PrinterStatus", "notificationReasonExists: ThermalOverload");
		if (isThermalShutdown())	Debug::trace("PrinterStatus", "notificationReasonExists: ThermalShutdown");
		if (isHardwareError())		Debug::trace("PrinterStatus", "notificationReasonExists: HardwareError");
		if (isMediaShutdown())		Debug::trace("PrinterStatus", "notificationReasonExists: MediaShutdown");
		if (!isPeripheralComm())	Debug::trace("PrinterStatus", "notificationReasonExists: PeripheralComm");
		if (isManagerDeleted())		Debug::trace("PrinterStatus", "notificationReasonExists: ManagerDeleted");
		if (isLabelTakenError())	Debug::trace("PrinterStatus", "notificationReasonExists: LabelTakenError");
		if (isTakeLabelError())		Debug::trace("PrinterStatus", "notificationReasonExists: TakeLabelError");
	}
	return result;
}
