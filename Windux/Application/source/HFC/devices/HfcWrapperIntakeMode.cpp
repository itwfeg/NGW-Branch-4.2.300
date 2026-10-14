/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperIntakeMode.h"
//#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperIntakeMode::dataType = 0;
std::string HfcWrapperIntakeMode::dataTypeName("HfcWrapperIntakeMode"); 
#endif

HfcWrapperIntakeMode::HfcWrapperIntakeMode(const char *data)
	: WrapperIntakeMode(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperIntakeMode::HfcWrapperIntakeMode(const char *data, int len, int formatType)
	: WrapperIntakeMode()
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
				setIntakeMode((WrapperIntakeMode::IntakeMode) Integer::valueOf(temp.c_str()));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWrapperIntakeMode::~HfcWrapperIntakeMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperIntakeMode::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = WrapperIntakeMode::serialize();
			msglen = WrapperIntakeMode::getSerializedLength();

			HfcObject::serializedData = WrapperIntakeMode::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getIntakeMode()));

			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
