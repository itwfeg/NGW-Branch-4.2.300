/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierMode.h"
#include "HobTokenizerCSV.h"

#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierMode::dataType = 0;
std::string HfcLabelApplierMode::dataTypeName("HfcLabelApplierMode"); 
#endif

HfcLabelApplierMode::HfcLabelApplierMode(const char *data, int len, int formatType) : LabelApplierMode()
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
					case fieldOffsetLaMode:
						setMode((LaOperatingMode) Integer::valueOf(token.c_str()));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcLabelApplierMode::~HfcLabelApplierMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcLabelApplierMode::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			LabelApplierMode::serialize();

			HfcObject::serializedData = LabelApplierMode::serializedData;
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}

	return HfcObject::serializedData.data();
}
