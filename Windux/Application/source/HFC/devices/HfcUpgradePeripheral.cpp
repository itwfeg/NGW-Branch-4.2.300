/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcUpgradePeripheral.h"
#include "HobTokenizerCSV.h"

#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcUpgradePeripheral::_dataType_ = 0;
std::string HfcUpgradePeripheral::_dataTypeName_("HfcUpgradePeripheral"); 
#endif


HfcUpgradePeripheral::HfcUpgradePeripheral(const char *data) : UpgradePeripheral(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcUpgradePeripheral", "HfcUpgradePeripheral(const char *data)");
}

HfcUpgradePeripheral::HfcUpgradePeripheral(int event, const char *data, int len, int formatType) : UpgradePeripheral(event)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcUpgradePeripheral", "HfcUpgradePeripheral(const char *data, int len, int formatType)");
	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case 1:
						setDevice((EvolDeviceType) Integer::valueOf(token.c_str()));
						break;
					case 2:
						setSubType(Integer::valueOf(token.c_str()));
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcUpgradePeripheral::~HfcUpgradePeripheral()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcUpgradePeripheral::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			Debug::trace("HfcUpgradePeripheral", "serialize(InternalFormat)");
			UpgradePeripheral::serialize();
			UpgradePeripheral::getSerializedLength();
			HfcObject::serializedData = UpgradePeripheral::serializedData;
		}
		break;

		case BrowserFormat:
		{
			Debug::trace("HfcUpgradePeripheral", "serialize(BrowserFormat)");
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Long::toString((int)getDevice()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Long::toString((int)getSubType()));
		}
		break;
	}

	return HfcObject::serializedData.data();
}
