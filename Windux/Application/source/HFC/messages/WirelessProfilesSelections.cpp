/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WirelessProfilesSelections.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WirelessProfilesSelections::dataType = 0;
std::string WirelessProfilesSelections::dataTypeName("WirelessProfilesSelections"); 
#endif

WirelessProfilesSelections::WirelessProfilesSelections()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

WirelessProfilesSelections::WirelessProfilesSelections(const char *data, int len, int formatType)
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
			// format : direction, fileName


			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetWPPath:
						token.unQuoteForCSV();
						setFileName(token);
						break;
					case fieldOffsetWPDirection:
						setDirection((WPDirection)Integer::valueOf(token));
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


// private
WirelessProfilesSelections::~WirelessProfilesSelections()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void WirelessProfilesSelections::init()
{
	fileName = "";
	direction = WPBackup;

}

HobString WirelessProfilesSelections::getFileName() const
{
	return fileName;
}
void WirelessProfilesSelections::setFileName(HobString fileName)
{
	this->fileName = fileName;
}

WPDirection WirelessProfilesSelections::getDirection() const
{
	return direction;
}
		
void WirelessProfilesSelections::setDirection(WPDirection dir)
{
	direction = dir;
}

WirelessProfilesSelections& WirelessProfilesSelections::operator=(const WirelessProfilesSelections& wps)
{
	fileName = wps.getFileName();
	direction = wps.getDirection();

	return *this;
}

const char *WirelessProfilesSelections::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endWPSelectionFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetWPPath:
					{
						HobString tempName(fileName);
						tempName.quoteForCSV();
						serializedData.append(tempName);
						break;
					}
					case fieldOffsetWPDirection:
						serializedData.append(Integer::toString((int)direction));
						break;
					default:
						break;
				}
			}
			break;

		default:
			serializedData = HobString("BackupRestoreSelections::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


