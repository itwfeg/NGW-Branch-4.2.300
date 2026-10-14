/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperSensorsTracking.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperSensorsTracking::dataType = 0;
std::string HfcWrapperSensorsTracking::dataTypeName("HfcWrapperSensorsTracking"); 
#endif

HfcWrapperSensorsTracking::HfcWrapperSensorsTracking(const char *data)
	: WrapperSensorsTracking(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperSensorsTracking::HfcWrapperSensorsTracking(const char *data, int len, int formatType)
	: WrapperSensorsTracking()
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
			{	
				if (Integer::valueOf(temp) > 0)
					setSensorsTrackingState(true);
				else
					setSensorsTrackingState(false);
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWrapperSensorsTracking::HfcWrapperSensorsTracking(bool sensorsTracking)
	: WrapperSensorsTracking()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setSensorsTrackingState(sensorsTracking);
}

HfcWrapperSensorsTracking::~HfcWrapperSensorsTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperSensorsTracking::serialize(int format) const
{
	const char *msgdata;
	int msglen;
	switch (format)
	{
		case InternalFormat:
		{
			msgdata = WrapperSensorsTracking::serialize();
			msglen = WrapperSensorsTracking::getSerializedLength();

			HfcObject::serializedData = WrapperSensorsTracking::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcWrapperSensorsTracking::setSensorsTrackingState(bool sensorsTracking)
{
	if(sensorsTracking)
		setSensorsTracking();
	else
		clearSensorsTracking();
}
