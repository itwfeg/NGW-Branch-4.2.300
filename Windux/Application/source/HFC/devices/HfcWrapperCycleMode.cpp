/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperCycleMode.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperCycleMode::dataType = 0;
std::string HfcWrapperCycleMode::dataTypeName("HfcWrapperCycleMode"); 
#endif

HfcWrapperCycleMode::HfcWrapperCycleMode(const char *data, int len, int formatType)
	: mode(WRAPPERCYCLEINVALID), packCode(UndefinedPackCode), cycleCount(0)
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
					
				switch (loopCnt)
				{
					case fieldOffsetWrapperCycleMode:
						setMode(Integer::valueOf(token));
						break;
					case fieldOffsetWrapperCyclePackCode:
						setPackCode((WrPackCode) Integer::valueOf(token));
						break;
					case fieldOffsetWrapperCycleCount:
						setCycleCount(Integer::valueOf(token));
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

HfcWrapperCycleMode::~HfcWrapperCycleMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

int HfcWrapperCycleMode::getMode()
{
	return mode;
}
void HfcWrapperCycleMode::setMode(int mode)
{
	this->mode = mode;
}

WrPackCode HfcWrapperCycleMode::getPackCode()
{
	return packCode;
}
void HfcWrapperCycleMode::setPackCode(WrPackCode packCode)
{
	this->packCode = packCode;
}

int HfcWrapperCycleMode::getCycleCount()
{
	return cycleCount;
}
void HfcWrapperCycleMode::setCycleCount(int count)
{
	cycleCount = count;
}

bool HfcWrapperCycleMode::isValidMode()
{
	if ((mode == WRAPPERCYCLEPACKAGESIZE) || (mode == WRAPPERCYCLEAUTODETECT) || (mode == WRAPPERCYCLEAUTOCONFIG))
		return true;
	else
		return false;
}

bool HfcWrapperCycleMode::isPackageSizeMode()
{
	if (mode == WRAPPERCYCLEPACKAGESIZE)
		return true;
	else
		return false;
}

bool HfcWrapperCycleMode::isAutoDetectMode()
{
	if (mode == WRAPPERCYCLEAUTODETECT)
		return true;
	else
		return false;
}

bool HfcWrapperCycleMode::isAutoConfigMode()
{
	if (mode == WRAPPERCYCLEAUTOCONFIG)
		return true;
	else
		return false;
}
