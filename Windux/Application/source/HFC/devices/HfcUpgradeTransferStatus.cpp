/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcUpgradeTransferStatus.h"

#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcUpgradeTransferStatus::_dataType_ = 0;
std::string HfcUpgradeTransferStatus::_dataTypeName_("HfcUpgradeTransferStatus"); 
#endif


HfcUpgradeTransferStatus::HfcUpgradeTransferStatus(const char *data) : UpgradeTransferStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcUpgradeTransferStatus::~HfcUpgradeTransferStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcUpgradeTransferStatus::serialize(int format) const
{
	Debug::trace("HfcUpgradeTransferStatus", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Long::toString((int)getBytesSent()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Long::toString((int)getTotalBytes()));
		}
		break;
	}

	return HfcObject::serializedData.data();
}
