/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcObject.h"
#include "HfcWrapperErrorHistory.h"
#include "Integer.h"
#include "HobList.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperErrorHistory::dataType = 0;
std::string HfcWrapperErrorHistory::dataTypeName("HfcWrapperErrorHistory"); 
#endif

HfcWrapperErrorHistory::HfcWrapperErrorHistory(const char *data)
	: WrapperErrorHistory(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	int index;

	WrErrorCode	errorCode = getFirstErrorCode(&index);
	for (int i = 0; i < MaxErrorHistoryBuffers; i++)
	{
		errorCodeList->push_back(Integer::toString(errorCode));
		if (errorCode != WrNoErrorCode)
			packCodeList->push_back(Integer::toString(getPackageInfo(index).getPackCode()));
		else
			packCodeList->push_back(" ");

		errorCode = getNextErrorCode(&index);
	}
}

HfcWrapperErrorHistory::~HfcWrapperErrorHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	if (errorCodeList)
		delete errorCodeList;
	if (packCodeList)
		delete packCodeList;
}

void HfcWrapperErrorHistory::init()
{
	errorCodeList = new HobList();
	packCodeList = new HobList();
}

const char *HfcWrapperErrorHistory::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			if (errorCodeList->size() && packCodeList->size())
			{
				HfcObject::serializedData = Integer::toString((int)getDeviceType());
				HfcObject::serializedData.append(BrowserFormatSeparator);
				HfcObject::serializedData.append(Integer::toString((int)getEventType()));
				HfcObject::serializedData.append(BrowserFormatSeparator);
				HfcObject::serializedData.append(errorCodeList->serialize(format));
				HfcObject::serializedData.append(BrowserFormatSeparator);
				HfcObject::serializedData.append(packCodeList->serialize(format));
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
