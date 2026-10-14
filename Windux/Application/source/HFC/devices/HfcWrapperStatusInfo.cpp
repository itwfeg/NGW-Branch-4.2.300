/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperStatusInfo.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperStatusInfo::dataType = 0;
std::string HfcWrapperStatusInfo::dataTypeName("HfcWrapperStatusInfo"); 
#endif

HfcWrapperStatusInfo::HfcWrapperStatusInfo(const char *data)
	: WrapperStatusInfo(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperStatusInfo::HfcWrapperStatusInfo(const char *UNUSED(data), int UNUSED(len), int formatType)
	: WrapperStatusInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
#if 0
			bool	isSet;

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetWrSIRate:
						setRate(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetWrSIReportStatus:
						isSet = Bool::valueOf(token.c_str());
						(isSet) ? setReportStatus() : clearReportStatus();
						break;
					case fieldOffsetWrSIPusherHome:
						isSet = Bool::valueOf(token.c_str());
						(isSet) ? setPusherHome() : clearPusherHome();
						break;
					case fieldOffsetWrSIRateAnalysis:
						isSet = Bool::valueOf(token.c_str());
						(isSet) ? setRateAnalysis() : clearRateAnalysis();
						break;
					case fieldOffsetWrSIOpenWindow:
						isSet = Bool::valueOf(token.c_str());
						(isSet) ? setOpenWindow() : clearOpenWindow();
						break;
					case fieldOffsetWrSIMainPower:
						isSet = Bool::valueOf(token.c_str());
						(isSet) ? setMainPower() : clearMainPower();
						break;
					case fieldOffsetWrSIMainMotor:
						isSet = Bool::valueOf(token.c_str());
						(isSet) ? setMainMotor() : clearMainMotor();
						break;
					case fieldOffsetWrSILimpHome:
						isSet = Bool::valueOf(token.c_str());
						(isSet) ? setLimpHome() : clearLimpHome();
						break;
					case fieldOffsetWrSIPackWidth:
						setPackWidth((WrPackWidth) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetWrSIFilmTension:
						setFilmTension((WrFilmTension) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetWrSIStateInfo:
					{
						WrapperStateInfo *info =  new WrapperStateInfo(token.c_str());
						setStateInfo(info);
					}
						break;

					default:
						break;
				}
				loopCnt++;
			}
#endif
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWrapperStatusInfo::~HfcWrapperStatusInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperStatusInfo::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = WrapperStatusInfo::serialize();
			msglen = WrapperStatusInfo::getSerializedLength();

			HfcObject::serializedData = msgdata;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			while(loopCnt < endHfcWrapperStatusInfoFieldOffsets)
			{
				if (loopCnt > 1)
					HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetWrSIRate:
						HfcObject::serializedData.append(Integer::toString(getRate()));
						break;
					case fieldOffsetWrSIReportStatus:
						HfcObject::serializedData.append(Bool::toString(isReportStatus()));
						break;
					case fieldOffsetWrSIPusherHome:
						HfcObject::serializedData.append(Bool::toString(isPusherHome()));
						break;
					case fieldOffsetWrSIRateAnalysis:
						HfcObject::serializedData.append(Bool::toString(isRateAnalysis()));
						break;
					case fieldOffsetWrSIOpenWindow:
						HfcObject::serializedData.append(Bool::toString(isOpenWindow()));
						break;
					case fieldOffsetWrSIMainPower:
						HfcObject::serializedData.append(Bool::toString(isMainPower()));
						break;
					case fieldOffsetWrSIMainMotor:
						HfcObject::serializedData.append(Bool::toString(isMainMotor()));
						break;
					case fieldOffsetWrSILimpHome:
						HfcObject::serializedData.append(Bool::toString(isLimpHome()));
						break;
					case fieldOffsetWrSIPackWidth:
						HfcObject::serializedData.append(Integer::toString(getPackWidth()));
						break;
					case fieldOffsetWrSIFilmTension:
						HfcObject::serializedData.append(Integer::toString(getFilmTension()));
						break;
					case fieldOffsetWrSIStateInfo:
						HfcObject::serializedData.append(Integer::toString(getStateInfo().getWrapperState()));
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

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
