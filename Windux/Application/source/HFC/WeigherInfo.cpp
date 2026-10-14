/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WeigherInfo.h"
#include "HobTokenizerCSV.h"
#include "HobDateTime.h"

#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherInfo::dataType = 0;
std::string WeigherInfo::dataTypeName("WeigherInfo"); 
#endif

WeigherInfo::WeigherInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

WeigherInfo::WeigherInfo(const char *UNUSED(data))
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

WeigherInfo::WeigherInfo(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

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
					case fieldOffsetWeigherInfoAddress:
					{
						UINT16 newAddr = Integer::valueOf(token);
						UINT16 addr = 0x0000;

						if ((0x0000 < newAddr) && (newAddr < 0x00ff))
						{
							char *pAddr = (char *)&addr;
							*pAddr = (char) newAddr;
							*(pAddr+1) = (char) newAddr;
						}

						setStationAddress(addr);
						break;
					}

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

WeigherInfo::~WeigherInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (weigherSystemConfig)
		delete weigherSystemConfig;
}


void WeigherInfo::init()
{
	weigherSystemConfig = (HfcWeigherSystemConfig *)NULL;
	stationAddress = 0;
}


HfcWeigherSystemConfig* WeigherInfo::getWeigherSystemConfig()
{
	return weigherSystemConfig;
}

void WeigherInfo::setWeigherSystemConfig(HfcWeigherSystemConfig *weigherSystemConfig)
{
	if(this->weigherSystemConfig)
	{
		delete this->weigherSystemConfig;
	}
	this->weigherSystemConfig = weigherSystemConfig;
}


DeviceAddress WeigherInfo::getStationAddress() const
{
	return stationAddress;
}

void WeigherInfo::setStationAddress(DeviceAddress stationAddress)
{
	this->stationAddress = stationAddress;
}


const char *WeigherInfo::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			UINT16 addr = getStationAddress();
			addr = addr >> 8;
			serializedData = Integer::toString(addr);
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


void WeigherInfo::update(WeigherInfo *weigherInfo)
{
	short loopCnt = 1;

	while (loopCnt < endWeigherInfoFieldOffsets)
	{
		if (weigherInfo->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetWeigherInfoAddress:
					setStationAddress(weigherInfo->getStationAddress());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}
