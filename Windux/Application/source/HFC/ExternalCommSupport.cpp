/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ExternalCommSupport.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobString.h"
#include "Configuration.h"

#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ExternalCommSupport::dataType = 0;
std::string ExternalCommSupport::dataTypeName("ExternalCommSupport"); 
#endif

ExternalCommSupport::ExternalCommSupport()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	scaleConfig = Configuration::getInstance();
}

ExternalCommSupport::ExternalCommSupport(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	scaleConfig = Configuration::getInstance();

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

				if(isReceived(loopCnt))
				{
					switch (loopCnt)
					{
						case fieldOffsetExternalCommSetting:
							scaleConfig->setExternalCommSetting(Integer::valueOf(token.c_str()));
							break;
						case fieldOffsetServerScaleIpAddress:
							scaleConfig->setServerIpAddress(token);
							break;
						case fieldOffsetServerScaleHostName:
							scaleConfig->setServerHostName(token);
							break;
						case fieldOffsetServerAppIpAddress:
							scaleConfig->setServerAppIpAddress(token);
							break;
						case fieldOffsetServerAppHostName:
							scaleConfig->setServerAppHostName(token);
							break;
						default:
							break;
					}
				}
				loopCnt++;
			}
			break;
		}
	}
}

ExternalCommSupport::~ExternalCommSupport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *ExternalCommSupport::serialize(int format) const
{
	HobString addr;
	HobIpAddress ipaddr;
	switch (format)
	{
		case BrowserFormat:
			serializedData = "";
			for (int loopCnt = 1; loopCnt < endExternalCommSupportOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetExternalCommSetting:
						serializedData = Integer::toString(scaleConfig->getExternalCommSetting());
						break;
					case fieldOffsetServerScaleIpAddress:
					{
						ipaddr = scaleConfig->getServerIpAddress();
						ipaddr.getAsString(addr);
						serializedData = addr;
						break;
					}
					case fieldOffsetServerScaleHostName:
						serializedData = scaleConfig->getServerHostName();
						break;
					case fieldOffsetServerAppIpAddress:
					{
						ipaddr = scaleConfig->getServerAppIpAddress();
						ipaddr.getAsString(addr);
						serializedData = addr;
						break;
					}
					case fieldOffsetServerAppHostName:
						serializedData = scaleConfig->getServerAppHostName();
						break;
					default:
						break;
				}
			}
			break;
	}
	return serializedData.c_str();
}
