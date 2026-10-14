/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperErrorHistoryTracking.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperErrorHistoryTracking::dataType = 0;
std::string HfcWrapperErrorHistoryTracking::dataTypeName("HfcWrapperErrorHistoryTracking"); 
#endif

HfcWrapperErrorHistoryTracking::HfcWrapperErrorHistoryTracking(const char *data)
	: WrapperErrorHistoryTracking(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperErrorHistoryTracking::HfcWrapperErrorHistoryTracking(const char *data, int len, int formatType)
	: WrapperErrorHistoryTracking()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			if (len != 0)
			{	
				if (Integer::valueOf(temp) > 0)
					setErrorHistoryTrackingState(true);
				else
					setErrorHistoryTrackingState(false);
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWrapperErrorHistoryTracking::HfcWrapperErrorHistoryTracking(bool sensorsTracking)
	: WrapperErrorHistoryTracking()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setErrorHistoryTrackingState(sensorsTracking);
}

HfcWrapperErrorHistoryTracking::~HfcWrapperErrorHistoryTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperErrorHistoryTracking::serialize(int format) const
{
	const char *msgdata;
	int msglen;
	switch (format)
	{
		case InternalFormat:
		{
			msgdata = WrapperErrorHistoryTracking::serialize();
			msglen = WrapperErrorHistoryTracking::getSerializedLength();

			HfcObject::serializedData = WrapperErrorHistoryTracking::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcWrapperErrorHistoryTracking::setErrorHistoryTrackingState(bool sensorsTracking)
{
	if(sensorsTracking)
		setErrorHistoryTracking();
	else
		clearErrorHistoryTracking();
}
