/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "WeigherEvent.h"
#include "HfcWeigherCat3AuditRecords.h"
#include "Debug.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherCat3AuditRecords::_dataType_ = 0;
std::string HfcWeigherCat3AuditRecords::_dataTypeName_("HfcWeigherCat3AuditRecords");
#endif

HfcWeigherCat3AuditRecords::HfcWeigherCat3AuditRecords(const HfcWeigherCat3AuditRecords& obj): WeigherCat3AuditRecords(obj), HfcObject(obj)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcWeigherCat3AuditRecords::HfcWeigherCat3AuditRecords() : WeigherCat3AuditRecords()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();
}

HfcWeigherCat3AuditRecords::HfcWeigherCat3AuditRecords(const char *data) : WeigherCat3AuditRecords(data)
{
	if (Debug::isTracing("WeigherCat3AuditRecords"))
		Debug::trace("WeigherCat3AuditRecords", std::string("HfcWeigherCat3AuditRecords constructor: ") + util_string::ptrtostring(this) );

#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();
}

HfcWeigherCat3AuditRecords::HfcWeigherCat3AuditRecords(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType) ) : WeigherCat3AuditRecords()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();

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

HfcWeigherCat3AuditRecords::HfcWeigherCat3AuditRecords(const HfcWeigherCat3AuditRecords *hws) : WeigherCat3AuditRecords()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

	init();

	setPageNumber(hws->getPageNumber());
	setPageIndex(hws->getPageIndex());
	setRecordBuffer(hws->getRecordBuffer());
}

HfcWeigherCat3AuditRecords::~HfcWeigherCat3AuditRecords()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

void HfcWeigherCat3AuditRecords::init()
{
}

const char *HfcWeigherCat3AuditRecords::serialize(int format) const
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
            HfcObject::serializedData = HobString("HfcWeigherCat3AuditRecords::serialize - unknown format:") + HobString(Integer::toString(format));
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.c_str();
}

