/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperCycle.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperCycle::dataType = 0;
std::string HfcWrapperCycle::dataTypeName("HfcWrapperCycle"); 
#endif

HfcWrapperCycle::HfcWrapperCycle(const char *data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	data ? WrapperCycle(data) : WrapperCycle();
}

#if 0
HfcWrapperCycle::HfcWrapperCycle(const char *data, int len, int formatType)
	: WrapperCycle()
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
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				if(isReceived(loopCnt))
				{
					switch (loopCnt)
					{
						case fieldOffsetWrCyPackageId:
							setPackageId(Integer::valueOf(token.c_str()));
							break;
						case fieldOffsetWrCyPullFilm:
						{
							bool	isSet;
							isSet = Bool::valueOf(token.c_str());
							(isSet) ? setPullFilm() : clearPullFilm();
						}
							break;
						case fieldOffsetWrCyPackCode:
							setWrPackCode((WrPackCode) Integer::valueOf(token.c_str()));
							break;

						default:
							break;
					}
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
#endif

HfcWrapperCycle::~HfcWrapperCycle()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
#if 0
const char *HfcWrapperCycle::serialize(int format)
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = WrapperCycle::serialize();
			msglen = WrapperCycle::getSerializedLength();

			HfcObject::serializedData = WrapperCycle::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			while(loopCnt < endHfcWrapperCycleFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetWrCyPackageId:
						HfcObject::serializedData.append(Integer::toString(getPackageId()));
						break;
					case fieldOffsetWrCyPullFilm:
						HfcObject::serializedData.append(Bool::toString(isPullFilm()));
						break;
					case fieldOffsetWrCyPackCode:
						HfcObject::serializedData.append(Integer::toString(getWrPackCode()));
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
#endif
