/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcResponse.h"
#include "HobTokenizerCSV.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcResponse::dataType = 0;
std::string HfcResponse::dataTypeName("HfcResponse"); 
#endif

HfcResponse::HfcResponse(long code, short style, const HobString &msgData)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	this->code = code;
	this->style = style;
	this->msgData = msgData;
	errorDevice = (ErrorEvent)((int)(this->code / DEVICE_ERROR_OFFSET));
}

HfcResponse::HfcResponse(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

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
					case fieldOffsetHfcResponseCode:
						code = Long::valueOf(token);
						if (code)
							errorDevice = (ErrorEvent)((int)(code / DEVICE_ERROR_OFFSET));
						break;
					case fieldOffsetHfcResponseStyle:
						style = (short)Integer::valueOf(token);
						break;
					case fieldOffsetHfcUserAnswer:
						answer = (short)Integer::valueOf(token);
						break;
					case fieldOffsetHfcResponseUserInput:
						userInput = token;
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


HfcResponse::~HfcResponse()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

// private
void HfcResponse::init()
{
	errorDevice = ErrUnknown;
	code = 0;
	style = RESPONSESTYLE_STATUS;
	msgData = "";
	answer = RESPONSEANSWER_CANCEL;
	userInput = "";
	dismissed = false;
}


const char *HfcResponse::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
			serializedData = "";

			serializedData.append(Long::toString(code));
			serializedData.append(BrowserFormatSeparator);

			serializedData.append(Integer::toString(style));

			if (msgData.size())
			{
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(msgData);
			}

			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}

