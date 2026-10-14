#include "HfcEvolWrapperSystemInfo.h"
#include "HobString.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperSystemInfo::_dataType_ = 0;
std::string HfcEvolWrapperSystemInfo::_dataTypeName_("HfcEvolWrapperSystemInfo"); 
#endif


HfcEvolWrapperSystemInfo::HfcEvolWrapperSystemInfo(const char *data) : EvolWrapperSystemInfo(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


HfcEvolWrapperSystemInfo::~HfcEvolWrapperSystemInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperSystemInfo::serialize(int format)const
{
	HfcObject::serializedData = "";
	HobString result;
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < fieldOffsetEvolWrapperSystemInfoEnd)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetBoardChecksumStatus:
					{
						HfcObject::serializedData.append(Bool::toString(getBoardChecksumStatus()));
						break;
					}					
					case fieldOffsetAwsChecksumStatus:
					{
						HfcObject::serializedData.append(Bool::toString(getAwsChecksumStatus()));
						break;
					}
					case fieldOffsetMotorBoardPartNumber:
					{
						HfcObject::serializedData.append(getMotorBoardPartNumber());
						break;
					}
					case fieldOffsetMotorBoardRev:
					{
						HfcObject::serializedData.append(getMotorBoardRevision());
						break;
					}
					case fieldOffsetMotorBoardDateOfManufacture:
					{
						HfcObject::serializedData.append(getMotorBoardDateOfManufacture());
						break;
					}
					case fieldOffsetAwsSerialNumber:
					{
						HfcObject::serializedData.append(getAwsSerialNumber());
						break;
					}
					case fieldOffsetAwsDateOfManufacture:
					{
						HfcObject::serializedData.append(getAwsDateOfManufacture());
						break;
					}
					case fieldOffsetAwsDateOfInstall:
					{
						HfcObject::serializedData.append(getAwsDateOfInstall());
						break;
					}
					case fieldOffsetAwsModelNumber:
					{
						HfcObject::serializedData.append(Integer::toString(getModelNumber()));
						break;
					}
					case fieldOffsetSystemWrapperCycleCount:
					{
						HfcObject::serializedData.append(Long::toString(getAwsCycleCount()));
						break;
					}
				}
				loopCnt++;
			}
			break;
		}
		default:
			// to do - throw exception("invalid format");
			break;
	}
	return 	HfcObject::serializedData.c_str();
}
