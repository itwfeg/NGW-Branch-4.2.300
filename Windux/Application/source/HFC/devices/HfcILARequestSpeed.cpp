/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcILARequestSpeed.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcILARequestSpeed::dataType = 0;
std::string HfcILARequestSpeed::dataTypeName("HfcILARequestSpeed"); 
#endif

HfcILARequestSpeed::HfcILARequestSpeed(const char *data)
	: ILARequestSpeed(data),
	  speedState(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

/* This case should never happen, since this message is never received from the RTC.
 * It is received from the browser, in which case, we use the below constructor
 */
}

HfcILARequestSpeed::HfcILARequestSpeed(const char *data, int len, int formatType)
	: ILARequestSpeed(),
	  speedState(0)
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
					case fieldOffsetILASpeedState:
						setSpeedState(Integer::valueOf(token.c_str()));
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

HfcILARequestSpeed::~HfcILARequestSpeed()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

short HfcILARequestSpeed::getSpeedState() const
{
	return speedState;
}
void HfcILARequestSpeed::setSpeedState(short speedState)
{
	this->speedState = speedState;
}

const char *HfcILARequestSpeed::serialize(int format) const
{
	switch (format)
	{
		// No other format needs to be supported, we don't send this message to the browser;
		// We only receive it from the browser and pass it on to the RTC.
		
		case InternalFormat:
		{
			const char *data = ILARequestSpeed::serialize();
			int datalen = ILARequestSpeed::getSerializedLength();
			HfcObject::serializedData.assign(data, datalen);
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
