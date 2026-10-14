/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ImportSelections.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ImportSelections::dataType = 0;
std::string ImportSelections::dataTypeName("ImportSelections"); 
#endif

ImportSelections::ImportSelections(): dirName_(""), moveFile_(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

ImportSelections::ImportSelections(const char *data, int len, int formatType): dirName_(""), moveFile_(false)
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
					case fieldOffsetIDirName:
						token.unQuoteForCSV();
						dirName_ = token;
						break;
					case fieldOffsetIMoveFile:
						setMoveFile(Bool::valueOf(token));
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

ImportSelections::~ImportSelections()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


HobString ImportSelections::getDirName() const
{
	return dirName_;
}
void ImportSelections::setDirName(HobString dirName)
{
	this->dirName_ = dirName;
}

bool ImportSelections::getMoveFile() const
{
	return moveFile_;
}
void ImportSelections::setMoveFile(bool moveIt)
{
	moveFile_ = moveIt;
}

const char *ImportSelections::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endISelectionFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetIDirName:
					{
						HobString tempName(dirName_);
						tempName.quoteForCSV();
						serializedData.append(tempName);
						break;
					}
					case fieldOffsetIMoveFile:
					{
						serializedData.append(Bool::toString(moveFile_));
						break;
					}

					default:
						break;
				}
			}
			break;

		default:
			serializedData = HobString("ImportSelections::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
