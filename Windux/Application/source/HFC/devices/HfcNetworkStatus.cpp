/* Fastrack Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "NetworkConnectivityEvents.h"
#include "HfcNetworkStatus.h"
#include "Configuration.h"

#include "Integer.h"
#include "Long.h"
#include "ULong.h"
#include "Hex.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcNetworkStatus::dataType = 0;
std::string HfcNetworkStatus::dataTypeName("HfcNetworkStatus"); 
#endif


HfcNetworkStatus::HfcNetworkStatus(const HfcNetworkStatus& rhs)
	: NetworkStatus(rhs), HfcObject(rhs), lastOnlineDateTime_("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcNetworkStatus::HfcNetworkStatus(const char *data)
	: NetworkStatus(data), lastOnlineDateTime_("")
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}


HfcNetworkStatus::~HfcNetworkStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcNetworkStatus::serialize(int format) const
{
    HfcObject::serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    switch (format)
    {
		case InternalFormat:
		{
			const char *data = NetworkStatus::serialize();
			int datalen = NetworkStatus::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
			break;
		}

		case BrowserFormat:
		{
            HfcObject::serializedData = Integer::toString((int)getDeviceType());
            HfcObject::serializedData.append(BrowserFormatSeparator);
            HfcObject::serializedData.append(Integer::toString((int)getEventType()));
            HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Bool::toString(isOnline()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString(getNetworkType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Integer::toString(getWirelessStrength()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HobString lastOnline = lastOnlineDateTime_;
			lastOnline.quoteForCSV();
			HfcObject::serializedData.append(lastOnline);
			break;
		}

        default:
            HfcObject::serializedData = HobString("HfcNetworkStatus::serialize - unknown format:") + HobString(Integer::toString(format));
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.c_str();
}
