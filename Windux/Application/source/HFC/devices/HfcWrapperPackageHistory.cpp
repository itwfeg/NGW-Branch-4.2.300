/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcObject.h"
#include "HfcWrapperPackageHistory.h"
#include "HfcWrapperConfig.h"
#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperPackageHistory::_dataType_ = 0;
std::string HfcWrapperPackageHistory::_dataTypeName_("HfcWrapperPackageHistory"); 
#endif

HfcWrapperPackageHistory::HfcWrapperPackageHistory()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcWrapperPackageHistory::HfcWrapperPackageHistory(const char *data)
	: WrapperPackageHistory(data)
{
	bool traceOn = Debug::isTracing("HfcWrapperPackageHistory");
	HobString msgDebug = "";

#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (traceOn)
	{
		msgDebug = "HfcWrapperPackageHistory():  ";
		Debug::trace("HfcWrapperPackageHistory", msgDebug + "constructor, passing in const char *data");
	}

	HobString packCodeNumStr = "", widthStr = "", lengthStr = "";
	int packCodeNum;

	WrapperPackageHistory::const_iterator iter = begin();
	while (iter != end())
	{
		packCodeNum = iter->packCode_;
		packCodeNumStr = Integer::toString(++packCodeNum);
		widthStr = Integer::toString(iter->width_);
		lengthStr = Integer::toString(iter->length_);

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  packCodeNum = [").append(packCodeNumStr).append("]");
			msgDebug2.append("  width = [").append(widthStr).append("]");
			msgDebug2.append("  length = [").append(lengthStr).append("]");
			Debug::trace("HfcWrapperPackageHistory", msgDebug2);
		}
		packCodeNumList.push_back(packCodeNumStr);
		packageWidthList.push_back(widthStr);
		packageLengthList.push_back(lengthStr);
		++iter;
	}
}

HfcWrapperPackageHistory::~HfcWrapperPackageHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcWrapperPackageHistory::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(packCodeNumList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(packageWidthList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(packageLengthList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(packCodeNameList.serialize(format));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

void HfcWrapperPackageHistory::getPackCodeNames(HfcWrapperConfig *wrapConfig)
{
	bool traceOn = Debug::isTracing("HfcWrapperPackageHistory");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "getPackCodeNames():  ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  wrapConfig = ").append(Hex::toString((long long) wrapConfig));
		Debug::trace("HfcWrapperPackageHistory", msgDebug2);
	}

	int packCodeNum;
	HobString packCodeNameStr = "";
	UINT8 packCodeName[PackCodeDescriptionLength]; 

	HobList_type myList = packCodeNumList.getMyList();

	if (packCodeNameList.size() == 0)
	{
		HobList_type::const_iterator iter = myList.begin();

		while (iter != myList.end())
		{
			packCodeNum = Integer::valueOf(*iter);

			if (--packCodeNum >= UndefinedPackCode)
			{
				packCodeNameStr = "Undefined";
			}
			else
			{
				if (!wrapConfig)
					packCodeNameStr = "?";
				else
				{
					memset(packCodeName, 0, PackCodeDescriptionLength);
					WrapperPackCodeInfo packCodeInfo = wrapConfig->getWrapperPackCodeInfo((WrPackCode) packCodeNum);
					packCodeInfo.getDescription(packCodeName);
					HobString temp((const char*) packCodeName);
					temp.quoteForCSV();
					packCodeNameStr = temp;
				}
			}
			packCodeNameList.push_back(packCodeNameStr);
			++iter;
		}
	}
}
