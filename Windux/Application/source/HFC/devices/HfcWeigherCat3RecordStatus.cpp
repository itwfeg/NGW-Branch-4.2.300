/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "WeigherEvent.h"
#include "HfcWeigherCat3RecordStatus.h"
#include "Debug.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherCat3RecordStatus::_dataType_ = 0;
std::string HfcWeigherCat3RecordStatus::_dataTypeName_("HfcWeigherCat3RecordStatus");
#endif

HfcWeigherCat3RecordStatus::HfcWeigherCat3RecordStatus() : WeigherCat3RecordStatus()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcWeigherCat3RecordStatus::HfcWeigherCat3RecordStatus(const char *data) : WeigherCat3RecordStatus(data)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcWeigherCat3RecordStatus::HfcWeigherCat3RecordStatus(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType) ) : WeigherCat3RecordStatus()
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
    // Nothing else to do, the WeigherCat3AuditRecords was filled in during initialization of this object.
    break;
    */
    //          default:
    // todo - throw exception("invalid format");
    //                  break;
    //  }
}

HfcWeigherCat3RecordStatus::~HfcWeigherCat3RecordStatus()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcWeigherCat3RecordStatus::serialize(int format) const
{
    HfcObject::serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    switch (format)
    {
        case BrowserFormat:
        {
            //HfcObject::serializedData = Integer::toString((int)getDeviceType());
            //HfcObject::serializedData.append(BrowserFormatSeparator);
            //HfcObject::serializedData.append(Integer::toString((int)getEventType()));
            break;
        }

        default:
            HfcObject::serializedData = HobString("HfcWeigherCat3RecordStatus::serialize - unknown format:") + HobString(Integer::toString(format));
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.c_str();
}

