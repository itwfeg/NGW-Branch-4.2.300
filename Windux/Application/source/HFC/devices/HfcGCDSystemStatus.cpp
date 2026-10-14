/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcGCDSystemStatus.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcGCDSystemStatus::dataType = 0;
std::string HfcGCDSystemStatus::dataTypeName("HfcGCDSystemStatus"); 
#endif

HfcGCDSystemStatus::HfcGCDSystemStatus()
	: GCDSystemStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcGCDSystemStatus::HfcGCDSystemStatus(const char *data)
	: GCDSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcGCDSystemStatus::~HfcGCDSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcGCDSystemStatus::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			const SystemStatusInfo *gcdSystemStatusInfo = getSystemStatusInfo();
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcGCDSysStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcGCDSysStatusDDRTestPassed:
						HfcObject::serializedData.append(Bool::toString(getDDRTestPassed()));
						break;
					case fieldOffsetHfcGCDSysStatusBoardVersion:
						HfcObject::serializedData.append(getBoardVersion().getRevMajMinStr());
						break;
					case fieldOffsetHfcGCDSysStatusWinCEVersion:
						HfcObject::serializedData.append(getWinCEVersion().getRevMajMinStr());
						break;
					case fieldOffsetHfcGCDSysStatusGCDMGRSoftwareVersion:
						HfcObject::serializedData.append(getGCDMGRSoftwareVersion().getRevMajMinStr());
						break;
					case fieldOffsetHfcGCDSysStatusGCDDLLSoftwareVersion:
						HfcObject::serializedData.append(getGCDDLLSoftwareVersion().getRevMajMinStr());
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// TODO - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
