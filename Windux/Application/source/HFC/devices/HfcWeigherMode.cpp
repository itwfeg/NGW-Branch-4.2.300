/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWeigherMode.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherMode::dataType = 0;
std::string HfcWeigherMode::dataTypeName("HfcWeigherMode"); 
#endif

HfcWeigherMode::HfcWeigherMode(const char *data, int len, int formatType)
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
					case fieldOffsetWeigherMode:
						setMode(Integer::valueOf(token));
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

HfcWeigherMode::~HfcWeigherMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

int HfcWeigherMode::getMode()
{
	return mode;
}
void HfcWeigherMode::setMode(int mode)
{
	this->mode = mode;
}
