/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PrinterInfo.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterInfo::dataType = 0;
std::string PrinterInfo::dataTypeName("PrinterInfo"); 
#endif

PrinterInfo::PrinterInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

PrinterInfo::PrinterInfo(const char *UNUSED(data))
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

PrinterInfo::PrinterInfo(const char *data, int len, int formatType)
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
					case fieldOffsetPrinterInfoAddress:
					{
						UINT8 newAddr = Integer::valueOf(token);
						UINT16 addr;
						char *pAddr = (char *)&addr;
						*pAddr = (char) newAddr;
						*(pAddr+1) = (char) newAddr;
						Debug::trace("Printer", HobString("new printer address being set to: ") +
														HobString(Integer::toString(addr)) + HobString("   hex:") +
														HobString(Hex::toString(addr)));
						setStationAddress(addr);
						break;
					}

					case fieldOffsetPrinterInfoPrinterType:
						setPrinterType((PrinterType) Integer::valueOf(token));
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

PrinterInfo::~PrinterInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (printerSystemConfig)
		delete printerSystemConfig;
}


void PrinterInfo::init()
{
	printerSystemConfig = (HfcPrinterSystemConfig *)NULL;

	stationAddress = 0;
	//didStationRespond = false;
}


HfcPrinterSystemConfig* PrinterInfo::getPrinterSystemConfig()
{
	return printerSystemConfig;
}

void PrinterInfo::setPrinterSystemConfig(HfcPrinterSystemConfig *printerSystemConfig)
{
	if(this->printerSystemConfig)
	{
		delete this->printerSystemConfig;
	}
	this->printerSystemConfig = printerSystemConfig;
}


DeviceAddress PrinterInfo::getStationAddress() const
{
	return stationAddress;
}

void PrinterInfo::setStationAddress(DeviceAddress stationAddress)
{
	this->stationAddress = stationAddress;
}


PrinterType PrinterInfo::getPrinterType() const
{
//	return ((printerSystemConfig) ? printerSystemConfig->getPrinterType() : 0);
	return printerType;
}

void PrinterInfo::setPrinterType(PrinterType printerType)
{
	this->printerType = printerType;
/*	if(printerSystemConfig)
		printerSystemConfig->setPrinterType(printerType);
*/
}


/*
bool PrinterInfo::getDidStationRespond()
{
	return didStationRespond;
}

void PrinterInfo::setDidStationRespond(bool didStationRespond)
{
	this->didStationRespond = didStationRespond;
}
*/
 

const char *PrinterInfo::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;
			serializedData = "";

			while(loopCnt < endPrinterInfoFieldOffsets)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetPrinterInfoAddress:
					{
						UINT16 addr = getStationAddress();
						addr = addr >> 8;
						serializedData.append(Integer::toString(addr));
						break;
					}

					case fieldOffsetPrinterInfoPrinterType:
						serializedData.append(Integer::toString(getPrinterType()));
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
	return serializedData.c_str();
}


void PrinterInfo::update(PrinterInfo *printerInfo)
{
	short loopCnt = 1;

	while (loopCnt < endPrinterInfoFieldOffsets)
	{
		if (printerInfo->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetPrinterInfoAddress:
					setStationAddress(printerInfo->getStationAddress());
					break;

				case fieldOffsetPrinterInfoPrinterType:
					setPrinterType(printerInfo->getPrinterType());
					break;

				default:
					break;
			}
		}
		loopCnt++;
	}
}
