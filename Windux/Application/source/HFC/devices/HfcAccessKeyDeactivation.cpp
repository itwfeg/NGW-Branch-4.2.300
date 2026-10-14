/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcAccessKeyDeactivation.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcAccessKeyDeactivation::dataType = 0;
std::string HfcAccessKeyDeactivation::dataTypeName("HfcAccessKeyDeactivation");
#endif

    HfcAccessKeyDeactivation::HfcAccessKeyDeactivation()
: AccessKeyDeactivation()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

}

    HfcAccessKeyDeactivation::HfcAccessKeyDeactivation(const char *data)
: AccessKeyDeactivation(data)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

}

    HfcAccessKeyDeactivation::HfcAccessKeyDeactivation(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType))
: AccessKeyDeactivation()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    //  switch (formatType)
    //  {
    /* This case should never happen, we use the above constructor (with the data buffer)
       is used for internal-formatted data.
       case InternalFormat:
       break;
       */
    //          default:
    // todo - throw exception("invalid format");
    //                  break;
    //  }
}

HfcAccessKeyDeactivation::~HfcAccessKeyDeactivation()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

}


const char *HfcAccessKeyDeactivation::serialize(int format) const
{
    HfcObject::serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    switch (format)
    {
        case InternalFormat:
            {
                const char *data = AccessKeyDeactivation::serialize();
                int datalen = AccessKeyDeactivation::getSerializedLength();
                HfcObject::serializedData.assign(data, datalen);
            }
            break;

        default:
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.data();
}
