/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WrapperEvent.h"
#include "HfcWrapperSensors.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperSensors::dataType = 0;
std::string HfcWrapperSensors::dataTypeName("HfcWrapperSensors"); 
#endif

std::list<HfcWrapperSensorsFieldOffsets> HfcWrapperSensors::wrapperTestsSenseFieldList;

HfcWrapperSensors::HfcWrapperSensors(const char *data)
	: WrapperSensors(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperSensors::HfcWrapperSensors(const char *data, int len, int formatType)
	: WrapperSensors()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool	isSet;

	switch (formatType)
	{
		case InternalFormat:
			break;

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
					case fieldOffsetWrSHeaterReading:
						setHeaterReading(Integer::valueOf(token));
						break;
					case fieldOffsetWrSEncoderCount:
						setEncoderCount(Integer::valueOf(token));
						break;
					case fieldOffsetWrSIntakeCount:
						setIntakeCount(Integer::valueOf(token));
						break;
					case fieldOffsetWrSStopSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setStopSwitch() : clearStopSwitch();
						break;
					case fieldOffsetWrSStartSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setStartSwitch() : clearStartSwitch();
						break;
					case fieldOffsetWrSPackageLengthSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setPackageLengthSwitch() : clearPackageLengthSwitch();
						break;
					case fieldOffsetWrSPackageSenseSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setPackageSenseSwitch() : clearPackageSenseSwitch();
						break;
					case fieldOffsetWrSPackageWidthSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setPackageWidthSwitch() : clearPackageWidthSwitch();
						break;
					case fieldOffsetWrSPusherHomeSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setPusherHomeSwitch() : clearPusherHomeSwitch();
						break;
					case fieldOffsetWrSPackageJamSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setPackageJamSwitch() : clearPackageJamSwitch();
						break;
					case fieldOffsetWrSWideFilmSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setWideFilmSwitch() : clearWideFilmSwitch();
						break;
					case fieldOffsetWrSNarrowFilmSwitch:
						isSet = Bool::valueOf(token);
						(isSet) ? setNarrowFilmSwitch() : clearNarrowFilmSwitch();
						break;
					case fieldOffsetWrSSlideBlockState:
						isSet = Bool::valueOf(token);
						(isSet) ? setSlideBlockState() : clearSlideBlockState();
						break;
					case fieldOffsetWrSHeaterState:
						isSet = Bool::valueOf(token);
						(isSet) ? setHeaterState() : clearHeaterState();
						break;
					case fieldOffsetWrSCompressorState:
						isSet = Bool::valueOf(token);
						(isSet) ? setCompressorState() : clearCompressorState();
						break;
					case fieldOffsetWrSMainMotorState:
						isSet = Bool::valueOf(token);
						(isSet) ? setMainMotorState() : clearMainMotorState();
						break;
					case fieldOffsetWrSDischargeMotorState:
						isSet = Bool::valueOf(token);
						(isSet) ? setDischargeMotorState() : clearDischargeMotorState();
						break;
					case fieldOffsetWrSFilmLengthState:
						setFilmLengthState((WrPackLength) Integer::valueOf(token));
						break;
					case fieldOffsetWrSFilmWidthState:
						setFilmWidthState((WrPackWidth) Integer::valueOf(token));
						break;
					case fieldOffsetWrSKnifeState:
						isSet = Bool::valueOf(token);
						(isSet) ? setKnifeState() : clearKnifeState();
						break;
					case fieldOffsetWrSLeftSideClampState:
						isSet = Bool::valueOf(token);
						(isSet) ? setLeftSideClampState() : clearLeftSideClampState();
						break;
					case fieldOffsetWrSRightSideClampState:
						isSet = Bool::valueOf(token);
						(isSet) ? setRightSideClampState() : clearRightSideClampState();
						break;
					case fieldOffsetWrSGripperState:
						isSet = Bool::valueOf(token);
						(isSet) ? setGripperState() : clearGripperState();
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

HfcWrapperSensors::~HfcWrapperSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

// static
void HfcWrapperSensors::initFieldLists()
{
	if (wrapperTestsSenseFieldList.empty())
	{
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSEncoderCount);
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSIntakeCount);
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSSlideBlockState);
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSFilmWidthState);
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSFilmLengthState);
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSGripperState);
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSLeftSideClampState);
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSRightSideClampState);
		wrapperTestsSenseFieldList.push_back(fieldOffsetWrSKnifeState);
	}
}

const char *HfcWrapperSensors::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = WrapperSensors::serialize();
			msglen = WrapperSensors::getSerializedLength();

			HfcObject::serializedData = WrapperSensors::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWrapperSensorsFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetWrSHeaterReading:
						HfcObject::serializedData.append(Integer::toString(getHeaterReading()));
						break;
					case fieldOffsetWrSEncoderCount:
						HfcObject::serializedData.append(Integer::toString(getEncoderCount()));
						break;
					case fieldOffsetWrSIntakeCount:
						HfcObject::serializedData.append(Integer::toString(getIntakeCount()));
						break;
					case fieldOffsetWrSStopSwitch:
						HfcObject::serializedData.append(Bool::toString(isStopSwitch()));
						break;
					case fieldOffsetWrSStartSwitch:
						HfcObject::serializedData.append(Bool::toString(isStartSwitch()));
						break;
					case fieldOffsetWrSPackageLengthSwitch:
						HfcObject::serializedData.append(Bool::toString(isPackageLengthSwitch()));
						break;
					case fieldOffsetWrSPackageSenseSwitch:
						HfcObject::serializedData.append(Bool::toString(isPackageSenseSwitch()));
						break;
					case fieldOffsetWrSPackageWidthSwitch:
						HfcObject::serializedData.append(Bool::toString(isPackageWidthSwitch()));
						break;
					case fieldOffsetWrSPusherHomeSwitch:
						HfcObject::serializedData.append(Bool::toString(isPusherHomeSwitch()));
						break;
					case fieldOffsetWrSPackageJamSwitch:
						//	The Jam switch uses inverse logic.
						HfcObject::serializedData.append(Bool::toString(!isPackageJamSwitch()));
						break;
					case fieldOffsetWrSWideFilmSwitch:
						HfcObject::serializedData.append(Bool::toString(isWideFilmSwitch()));
						break;
					case fieldOffsetWrSNarrowFilmSwitch:
						HfcObject::serializedData.append(Bool::toString(isNarrowFilmSwitch()));
						break;
					case fieldOffsetWrSSlideBlockState:
						HfcObject::serializedData.append(Bool::toString(isSlideBlockState()));
						break;
					case fieldOffsetWrSHeaterState:
						HfcObject::serializedData.append(Bool::toString(isHeaterState()));
						break;
					case fieldOffsetWrSCompressorState:
						HfcObject::serializedData.append(Bool::toString(isCompressorState()));
						break;
					case fieldOffsetWrSMainMotorState:
						HfcObject::serializedData.append(Bool::toString(isMainMotorState()));
						break;
					case fieldOffsetWrSDischargeMotorState:
						HfcObject::serializedData.append(Bool::toString(isDischargeMotorState()));
						break;
					case fieldOffsetWrSFilmLengthState:
						HfcObject::serializedData.append(Integer::toString(getFilmLengthState()));
						break;
					case fieldOffsetWrSFilmWidthState:
						HfcObject::serializedData.append(Integer::toString(getFilmWidthState()));
						break;
					case fieldOffsetWrSKnifeState:
						HfcObject::serializedData.append(Bool::toString(isKnifeState()));
						break;
					case fieldOffsetWrSLeftSideClampState:
						HfcObject::serializedData.append(Bool::toString(isLeftSideClampState()));
						break;
					case fieldOffsetWrSRightSideClampState:
						HfcObject::serializedData.append(Bool::toString(isRightSideClampState()));
						break;
					case fieldOffsetWrSGripperState:
						HfcObject::serializedData.append(Bool::toString(isGripperState()));
						break;
					default:
						break;
				}
				loopCnt++;
			}
			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case WrapperTestSensorsFormat:
		{
			std::list<HfcWrapperSensorsFieldOffsets>::iterator fieldIter;
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(WrTestSense));

			fieldIter = wrapperTestsSenseFieldList.begin();
			while (fieldIter != wrapperTestsSenseFieldList.end())
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (*fieldIter)
				{
					case fieldOffsetWrSEncoderCount:
						HfcObject::serializedData.append(Integer::toString(getEncoderCount()));
						break;
					case fieldOffsetWrSIntakeCount:
						HfcObject::serializedData.append(Integer::toString(getIntakeCount()));
						break;
					case fieldOffsetWrSSlideBlockState:
						HfcObject::serializedData.append(Bool::toString(isSlideBlockState()));
						break;
					case fieldOffsetWrSFilmWidthState:
						HfcObject::serializedData.append(Integer::toString(getFilmWidthState()));
						break;
					case fieldOffsetWrSFilmLengthState:
						HfcObject::serializedData.append(Integer::toString(getFilmLengthState()));
						break;
					case fieldOffsetWrSGripperState:
						HfcObject::serializedData.append(Bool::toString(isGripperState()));
						break;
					case fieldOffsetWrSLeftSideClampState:
						HfcObject::serializedData.append(Bool::toString(isLeftSideClampState()));
						break;
					case fieldOffsetWrSRightSideClampState:
						HfcObject::serializedData.append(Bool::toString(isRightSideClampState()));
						break;
					case fieldOffsetWrSKnifeState:
						HfcObject::serializedData.append(Bool::toString(isKnifeState()));
						break;
					default:
						break;
				}
				fieldIter++;
			}
			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
