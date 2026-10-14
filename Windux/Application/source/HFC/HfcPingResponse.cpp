/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "HfcPingResponse.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPingResponse::dataType = 0;
std::string HfcPingResponse::dataTypeName("HfcPingResponse"); 
#endif

HfcPingResponse::HfcPingResponse(int attempts, int replies)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->attempts = attempts;
	this->replies = replies;
}

HfcPingResponse::~HfcPingResponse()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


int HfcPingResponse::getAttempts() const
{
	return attempts;
}
void HfcPingResponse::setAttempts(int attempts)
{
	this->attempts = attempts;
}

int HfcPingResponse::getReplies() const
{
	return replies;
}
void HfcPingResponse::setReplies(int replies)
{
	this->replies = replies;
}

const char *HfcPingResponse::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData = "";
			serializedData.append(Integer::toString((int)getAttempts()));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(Integer::toString((int)getReplies()));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}

