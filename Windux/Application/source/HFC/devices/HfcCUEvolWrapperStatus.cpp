/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcCUEvolWrapperStatus.h"

#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcCUEvolWrapperStatus::_dataType_ = 0;
std::string HfcCUEvolWrapperStatus::_dataTypeName_("HfcCUEvolWrapperStatus"); 
#endif

HfcCUEvolWrapperStatus::HfcCUEvolWrapperStatus(const char *data) : CUEvolWrapperStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcCUEvolWrapperStatus", "Constructor 1");
}

HfcCUEvolWrapperStatus::~HfcCUEvolWrapperStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcCUEvolWrapperStatus::serialize(int format) const
{
	Debug::trace("HfcCUEvolWrapperStatus", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(getNodeType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(getNodeStatus()));
		}
		break;
	}

	return HfcObject::serializedData.data();
}
