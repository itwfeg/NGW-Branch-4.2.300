/* HT Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * ITWFEG Hobart. Troy, Ohio  USA
 */

#include "RemotePrintTemplateRequest.h"
#include "Configuration.h"

#include "HobTokenizerBar.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RemotePrintTemplateRequest::dataType = 0;
std::string RemotePrintTemplateRequest::dataTypeName("RemotePrintTemplateRequest"); 
#endif


RemotePrintTemplateRequest::RemotePrintTemplateRequest()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

RemotePrintTemplateRequest::RemotePrintTemplateRequest(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BarFormat:
		{
			HobString temp(data, len);

			HobTokenizerBar tok(&temp);
			HobString token;

			if (tok.next(&token) != -1 && token.size() != 0)
			{
				// We're receiving the original command from the external requester as the first token.  smBackend used to strip this off
				// when it converted it to Device / Event pair, now it just gets passed through. Just ignore it.
				int nextTokenReturnVal = 0;
				if (token.at(0) == 'p')
					nextTokenReturnVal = tok.next(&token);

				// Now get the second token
				if (nextTokenReturnVal == 0 && !token.empty())
					labelTypeNum = Long::valueOf(token);

				while (tok.next(&token) != -1)
					labelStrings.push_back(token);
			}

			break;
		}

		default:
			break;
	}
}

RemotePrintTemplateRequest::~RemotePrintTemplateRequest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("RemotePrintTemplateRequest.Delete", string("    RemotePrintTemplateRequest: ") + Hex::toString((long long) this) + "  labelTypeNum: " + Long::toString(labelTypeNum));
}


//	private
void RemotePrintTemplateRequest::init()
{
	labelTypeNum = 0;
}

std::string RemotePrintTemplateRequest::getLabelString(int offset) const
{
        string result("");

        Debug::trace("ProductManager.remotePrint", string("offset : ") + Integer::toString(offset) + string(" with result: ") + labelStrings[offset]);
	
        if (offset >= 0 && offset < (int) labelStrings.size())
            result = labelStrings[offset];

        return result;
}

std::string RemotePrintTemplateRequest::getBarcodeString() const
{
	string result("");
	int offset = Configuration::getInstance()->toIntCfgI(CfgInternal::TEMPLATELABELBARCODEOFFSET) - 1;

	if (labelStrings.size() > (size_t) offset)
		result = labelStrings[offset];

	return result;
}

const char *RemotePrintTemplateRequest::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
		{
			/*
			serializedData.append(Long::toString(labelTypeNum));
			for (int i = 0; i < labelStrings.size(); i++)
			{
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(labelStrings[i]);
			}
			*/
			serializedData = "RemotePrintTemplateRequest::serialize - currently unsupported";
			break;
		}

		default:
			serializedData = string("RemotePrintTemplateRequest::serialize - unknown format:") + Integer::toString(format);
			break;
	}
	return serializedData.data();
}

void RemotePrintTemplateRequest::show()
{
	bool isTracing = Debug::isTracing("ProductManager.remotePrintShow");
	if (isTracing)
	{
		Debug::trace("ProductManager.remotePrint", "RemotePrintTemplateRequest: ");
		Debug::trace("ProductManager.remotePrint", string("labelTypeNum: ") + Long::toString(labelTypeNum) + "  num of strings: " + Integer::toString((int)labelStrings.size()));
		for (size_t i = 0; i < labelStrings.size(); i++)
		{
			Debug::trace("ProductManager.remotePrint", string("labelString: ") + Long::toString(i) + ": " + labelStrings[i]);
		}
	}
}

