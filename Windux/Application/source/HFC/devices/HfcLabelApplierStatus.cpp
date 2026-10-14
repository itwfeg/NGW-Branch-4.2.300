/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierStatus.h"
#include "Configuration.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierStatus::_dataType_ = 0;
std::string HfcLabelApplierStatus::_dataTypeName_("HfcLabelApplierStatus");
#endif

HfcLabelApplierStatus::HfcLabelApplierStatus() : LabelApplierStatus()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcLabelApplierStatus::HfcLabelApplierStatus(const char *data) : LabelApplierStatus(data)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcLabelApplierStatus::HfcLabelApplierStatus(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType) ) : LabelApplierStatus()
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

HfcLabelApplierStatus::~HfcLabelApplierStatus()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}


const char *HfcLabelApplierStatus::serialize(int format) const
{
    HfcObject::serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    switch (format)
    {
        case InternalFormat:
            {
                const char *data = LabelApplierStatus::serialize();
                int datalen = LabelApplierStatus::getSerializedLength();

                HfcObject::serializedData.assign(data, datalen);
            }
            break;

        default:
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.data();
}


bool HfcLabelApplierStatus::operator==(HfcLabelApplierStatus &hlas)
{
    return (equals(hlas));
}

