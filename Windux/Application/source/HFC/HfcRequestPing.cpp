/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "HfcRequestPing.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcRequestPing::dataType = 0;
std::string HfcRequestPing::dataTypeName("HfcRequestPing"); 
#endif

HfcRequestPing::HfcRequestPing()
	: address(""), pingCount(1), repeatCount(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcRequestPing::HfcRequestPing(const char *data, int len, int formatType)
	: address(""), pingCount(1), repeatCount(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
/* This case should never happen, we use the above constructor (with the data buffer)
   for internal-formatted data.
		case InternalFormat:	// Going to RTC
			break;
*/
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
					case fieldOffsetPingAddress:
						setAddress(token);
						break;

					case fieldOffsetPingCount:
						setPingCount(Integer::valueOf(token));
						break;

					case fieldOffsetRepeatCount:
						setRepeatCount(Integer::valueOf(token));
						break;

					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcRequestPing::~HfcRequestPing()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


HobString &HfcRequestPing::getAddress()
{
	return address;
}
void HfcRequestPing::setAddress(HobString &addr)
{
	address = addr;
}

int HfcRequestPing::getPingCount()
{
	return pingCount;
}
void HfcRequestPing::setPingCount(int count)
{
	pingCount = count;
}

int HfcRequestPing::getRepeatCount()
{
	return repeatCount;
}
void HfcRequestPing::setRepeatCount(int count)
{
	repeatCount = count;
}
