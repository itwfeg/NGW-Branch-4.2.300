/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "WeigherEvent.h"
#include "HfcWeigherCat3Statistics.h"
#include "Debug.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherCat3Statistics::_dataType_ = 0;
std::string HfcWeigherCat3Statistics::_dataTypeName_("HfcWeigherCat3Statistics");
#endif

HfcWeigherCat3Statistics::HfcWeigherCat3Statistics(const HfcWeigherCat3Statistics& obj): WeigherCat3Statistics(obj), HfcObject(obj)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcWeigherCat3Statistics::HfcWeigherCat3Statistics() : WeigherCat3Statistics()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();
}

HfcWeigherCat3Statistics::HfcWeigherCat3Statistics(const char *data) : WeigherCat3Statistics(data)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();
}

HfcWeigherCat3Statistics::HfcWeigherCat3Statistics(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType) ) : WeigherCat3Statistics()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();

}

HfcWeigherCat3Statistics::HfcWeigherCat3Statistics(const HfcWeigherCat3Statistics *hws) : WeigherCat3Statistics()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();

	setTotalPages(hws->getTotalPages());
	setTotalRecordsPerPage(hws->getTotalRecordsPerPage());
	setCurrentPage(hws->getCurrentPage());
	setTotalFreeRecords(hws->getTotalFreeRecords());
	setTotalRecordedEvents(hws->getTotalRecordedEvents());
	setTotalCalibrationRecords(hws->getTotalCalibrationRecords());
	setTotalConfigurationRecords(hws->getTotalConfigurationRecords());
	setGainCoefficientCounter(hws->getGainCoefficientCounter());
	setOffsetCoefficientCounter(hws->getOffsetCoefficientCounter());
	setGainFactorCounter(hws->getGainFactorCounter());
	setCenterOfMainCounter(hws->getCenterOfMainCounter());
	setZeroReferenceCounter(hws->getZeroReferenceCounter());
	setWeigherModelCounter(hws->getWeigherModelCounter());
	setMinWeightPrintCounter(hws->getMinWeightPrintCounter());
	setMaxWeightCounter(hws->getMaxWeightCounter());
	setFilterSpeedCounter(hws->getFilterSpeedCounter());
 	setDivisionSizeCounter(hws->getDivisionSizeCounter());
 	setWeigherModeCounter(hws->getWeigherModeCounter());
}

HfcWeigherCat3Statistics::~HfcWeigherCat3Statistics()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

void HfcWeigherCat3Statistics::init()
{
}

const char *HfcWeigherCat3Statistics::serialize(int format) const
{
    HfcObject::serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    switch (format)
    {
        case BrowserFormat:
        {
            break;
        }

        default:
            HfcObject::serializedData = HobString("HfcWeigherCat3Statistics::serialize - unknown format:") + HobString(Integer::toString(format));
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.c_str();
}

