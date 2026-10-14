/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierSensorsTracking.h"
#include "HobTokenizerCSV.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierSensorsTracking::dataType = 0;
std::string HfcLabelApplierSensorsTracking::dataTypeName("HfcLabelApplierSensorsTracking"); 
#endif

HfcLabelApplierSensorsTracking::HfcLabelApplierSensorsTracking(const char *data, int len, int formatType) : LabelApplierSensorsTracking()
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
					case fieldOffsetLaSensorsTracking:
						setSensorsTracking(Bool::valueOf(token.c_str()));
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

HfcLabelApplierSensorsTracking::~HfcLabelApplierSensorsTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcLabelApplierSensorsTracking::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			LabelApplierSensorsTracking::serialize();
			LabelApplierSensorsTracking::getSerializedLength();

			HfcObject::serializedData = LabelApplierSensorsTracking::serializedData;
			HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}

	return HfcObject::serializedData.data();
}
