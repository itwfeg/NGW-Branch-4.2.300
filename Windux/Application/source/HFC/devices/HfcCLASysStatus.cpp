#include "HfcCLASysStatus.h"
#include "HobString.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcCLASysStatus::_dataType_ = 0;
std::string HfcCLASysStatus::_dataTypeName_("HfcCLASysStatus"); 
#endif


HfcCLASysStatus::HfcCLASysStatus(const char *data) : CLASystemStatus(data)
{
	Debug::trace("HfcCLASysStatus", "HfcCLASysStatus(const char *data)");
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


HfcCLASysStatus::~HfcCLASysStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcCLASysStatus::serialize(int format)const
{
	HfcObject::serializedData = "";
	HobString result;
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;
			Debug::trace("HfcCLASysStatus", "serialize(BrowserFormat)");
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < fieldOffsetCLASystemStatusEnd)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					/*	CLASysInfo	*/
					case fieldOffsetCLAConfigurationChecksum:
					{
						HfcObject::serializedData.append(Hex::toString(getConfigurationChecksum()));
						HfcObject::serializedData.append("H");
						Debug::trace("HfcCLASysStatus", "Configuration Checksum: " + HobString(Hex::toString(getConfigurationChecksum()))+ HobString("H"));
						break;
					}
					case fieldOffsetCLAAppFlash:
					{
						HfcObject::serializedData.append(HobString(Integer::toString(getAppFlashMajor()))+HobString(".")+HobString(Integer::toString(getAppFlashMinor())));
						Debug::trace("HfcCLASysStatus", "AppFlash: " + HobString(Integer::toString(getAppFlashMajor()))+HobString(".")+HobString(Integer::toString(getAppFlashMinor())));
						break;
					}
					case fieldOffsetCLADataFlash:
					{
						HfcObject::serializedData.append(HobString(Integer::toString(getDataFlashMajor()))+HobString(".")+HobString(Integer::toString(getDataFlashMinor())));
						Debug::trace("HfcCLASysStatus", "DataFlash: " + HobString(Integer::toString(getDataFlashMajor()))+HobString(".")+HobString(Integer::toString(getDataFlashMinor())));
						break;
					}
					case fieldOffsetCLABootFlash:
					{
						HfcObject::serializedData.append(HobString(Integer::toString(getBootFlashMajor()))+HobString(".")+HobString(Integer::toString(getBootFlashMinor())));
						Debug::trace("HfcCLASysStatus", "BootFlash: " + HobString(Integer::toString(getBootFlashMajor()))+HobString(".")+HobString(Integer::toString(getBootFlashMinor())));
						break;
					}
					///*	EEP93C56Board	*/
					//case fieldOffsetCLAPartNumber:
					//{
					//	HfcObject::serializedData.append(HobString((const char *) getPartNumber(), PART_NUMBER_SIZE));
					//	Debug::trace("HfcCLASysStatus", "PartNumber: " + HobString((const char *) getPartNumber(), PART_NUMBER_SIZE));
					//	break;
					//}
					//case fieldOffsetCLABoardRev:
					//{
					//	HfcObject::serializedData.append(HobString((const char *) getBoardRev(), REVISION_SIZE));
					//	Debug::trace("HfcCLASysStatus", "BoardRev: " + HobString((const char *) getBoardRev(), REVISION_SIZE));
					//	break;
					//}
					//case fieldOffsetCLADateOfManufacture:
					//{
					//	HfcObject::serializedData.append(HobString((const char *) getDateOfManufacture(), DATE_SIZE));
					//	Debug::trace("HfcCLASysStatus", "DateOfManufacture: " + HobString((const char *) getDateOfManufacture(), DATE_SIZE));
					//	break;
					//}
					//case fieldOffsetCLASerialNumber:
					//{
					//	HfcObject::serializedData.append(HobString((const char *) getSerialNumber(), SERIAL_NUMBER_SIZE));
					//	Debug::trace("HfcCLASysStatus", "SerialNumber: " + HobString((const char *) getSerialNumber(), SERIAL_NUMBER_SIZE));
					//	break;
					//}
					//case fieldOffsetCLABoardSectionChecksum:
					//{
					//	HfcObject::serializedData.append(Long::toString(getBoardSectionChecksum()));
					//	Debug::trace("HfcCLASysStatus", "BoardSectionChecksum: " + HobString(Long::toString(getBoardSectionChecksum())));
					//	break;
					//}
					///*	EEP93C56AWS	*/
					//case fieldOffsetCLAAwsSerialNumber:
					//{
					//	HfcObject::serializedData.append(HobString((const char *) getAwsSerialNumber(), AWS_SERIAL_NUMBER_SIZE));
					//	Debug::trace("HfcCLASysStatus", "AwsSerialNumber: " + HobString((const char *) getAwsSerialNumber(), AWS_SERIAL_NUMBER_SIZE));
					//	break;
					//}
					//case fieldOffsetCLAAwsDateOfManufacture:
					//{
					//	HfcObject::serializedData.append(HobString((const char *) getAwsDateOfManufacture(), DATE_SIZE));
					//	Debug::trace("HfcCLASysStatus", "AwsDateOfManufacture: " + HobString((const char *) getAwsDateOfManufacture(), DATE_SIZE));
					//	break;
					//}
					//case fieldOffsetCLAAwsDateOfInstall:
					//{
					//	HfcObject::serializedData.append(HobString((const char *) getAwsDateOfInstall(), DATE_SIZE));
					//	Debug::trace("HfcCLASysStatus", "AwsDateOfInstall: " + HobString((const char *) getAwsDateOfInstall(), DATE_SIZE));
					//	break;
					//}
					//case fieldOffsetCLAAwsModelNumber:
					//{
					//	HfcObject::serializedData.append(Integer::toString(getModelNumber()));
					//	Debug::trace("HfcCLASysStatus", "AwsModelNumber: " + HobString(Integer::toString(getModelNumber())));
					//	break;
					//}
					//case fieldOffsetCLACycleCount:
					//{
					//	HfcObject::serializedData.append(Long::toString(getCycleCount()));
					//	Debug::trace("HfcCLASysStatus", "CycleCount: " + HobString(Long::toString(getCycleCount())));
					//	break;
					//}
					//case fieldOffsetCLAAwsSectionChecksum:
					//{
					//	HfcObject::serializedData.append(Long::toString(getAwsSectionChecksum()));
					//	Debug::trace("HfcCLASysStatus", "AwsSectionChecksum: " + HobString(Long::toString(getAwsSectionChecksum())));
					//	break;
					//}
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
