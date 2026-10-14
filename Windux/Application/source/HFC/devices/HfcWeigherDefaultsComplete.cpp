/**
 * @file HfcWeigherDefaultsComplete.cpp
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the completion of saving weigher configuration defaults.
 * Created on: May 9, 2018
 */
#include "HfcWeigherDefaultsComplete.h"
#include "Bool.h"
#include "Integer.h"
#include "Debug.h"
#include "HobTokenizerCSV.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherMode::dataType = 0;
std::string HfcWeigherMode::dataTypeName("HfcWeigherDefaultsComplete");
#endif

HfcWeigherDefaultsComplete::HfcWeigherDefaultsComplete(const char *data) : WeigherFactoryDefaultsComplete(data)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}
HfcWeigherDefaultsComplete::HfcWeigherDefaultsComplete(const char *data, int len, int formatType) : WeigherFactoryDefaultsComplete()
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
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);

				switch (loopCnt)
				{
					case fieldOffsetWeigherStatus:
						setSavedStatus(Bool::valueOf(token));
						break;

					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			break;
	}
}

HfcWeigherDefaultsComplete::~HfcWeigherDefaultsComplete()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWeigherDefaultsComplete::serialize(int format) const
{
	HfcObject::serializedData = Integer::toString((int)getDeviceType());
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(Integer::toString((int)getEventType()));
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(Bool::toString(getSavedStatus()));

	Debug::trace("HfcWeigherDefaultsComplete", ": " + HfcObject::serializedData );
	return HfcObject::serializedData.c_str();
}
