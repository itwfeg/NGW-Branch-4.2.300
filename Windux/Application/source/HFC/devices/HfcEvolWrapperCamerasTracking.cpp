/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperCamerasTracking.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperCamerasTracking::_dataType_ = 0;
std::string HfcEvolWrapperCamerasTracking::_dataTypeName_("HfcEvolWrapperCamerasTracking"); 
#endif

HfcEvolWrapperCamerasTracking::HfcEvolWrapperCamerasTracking(const char *data)
	: EvolWrapperCamerasTracking(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcEvolWrapperCamerasTracking::HfcEvolWrapperCamerasTracking(const char *data, int UNUSED(len), int formatType)
	: EvolWrapperCamerasTracking()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	switch (formatType)
	{
		case BrowserFormat:
			if (Bool::valueOf(data))
				setCamerasTracking();
			else
				clearCamerasTracking();
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcEvolWrapperCamerasTracking::~HfcEvolWrapperCamerasTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperCamerasTracking::serialize(int format)const
{
	const char *msgdata;
	int msglen;
	switch (format)
	{
		case InternalFormat:
		{
			msgdata = EvolWrapperCamerasTracking::serialize();
			msglen = EvolWrapperCamerasTracking::getSerializedLength();

			HfcObject::serializedData = EvolWrapperCamerasTracking::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
