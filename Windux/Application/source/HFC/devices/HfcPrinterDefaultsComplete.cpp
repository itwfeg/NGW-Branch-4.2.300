/**
 * @file HfcPrinterDefaultsComplete.cpp
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the completion of saving printer configuration defaults.
 * Created on: May 10, 2018
 */

#include "HfcPrinterDefaultsComplete.h"
#include "Bool.h"
#include "Integer.h"
#include "Debug.h"
#include "HobTokenizerCSV.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherMode::dataType = 0;
std::string HfcWeigherMode::dataTypeName("HfcPrinterDefaultsComplete");
#endif

HfcPrinterDefaultsComplete::HfcPrinterDefaultsComplete(const char *data) : PrinterFactoryDefaultsComplete(data)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}
HfcPrinterDefaultsComplete::HfcPrinterDefaultsComplete(const char *data, int len, int formatType) : PrinterFactoryDefaultsComplete()
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
					case fieldOffsetPrinterStatus:
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

HfcPrinterDefaultsComplete::~HfcPrinterDefaultsComplete()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcPrinterDefaultsComplete::serialize(int format) const
{
	HfcObject::serializedData = Integer::toString((int)getDeviceType());
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(Integer::toString((int)getEventType()));
	HfcObject::serializedData.append(BrowserFormatSeparator);
	HfcObject::serializedData.append(Bool::toString(getSavedStatus()));

	Debug::trace("HfcPrinterDefaultsComplete", ": " + HfcObject::serializedData );
	return HfcObject::serializedData.c_str();
}

