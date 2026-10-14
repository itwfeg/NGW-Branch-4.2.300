/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperStatusTracking.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperStatusTracking::dataType = 0;
std::string HfcWrapperStatusTracking::dataTypeName("HfcWrapperStatusTracking"); 
#endif

HfcWrapperStatusTracking::HfcWrapperStatusTracking(bool isTracking)
	: WrapperStatusTracking(NULL, isTracking)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperStatusTracking::HfcWrapperStatusTracking(const char *data)
	: WrapperStatusTracking(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperStatusTracking::HfcWrapperStatusTracking(const char *data, int len, int formatType)
	: WrapperStatusTracking()
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
		{
			HobString temp(data, len);
			if (len != 0)
				setStatusTrackingState(Bool::valueOf(temp.c_str()));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWrapperStatusTracking::~HfcWrapperStatusTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperStatusTracking::serialize(int format) const
{
	const char *msgdata;
	int msglen;
	switch (format)
	{
		case InternalFormat:
		{
			msgdata = WrapperStatusTracking::serialize();
			msglen = WrapperStatusTracking::getSerializedLength();

			HfcObject::serializedData = WrapperStatusTracking::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcWrapperStatusTracking::setStatusTrackingState(bool statusTracking)
{
	if(statusTracking)
		setStatusTracking();
	else
		clearStatusTracking();
}
