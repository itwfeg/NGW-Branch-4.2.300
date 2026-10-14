/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "BackupRestoreErrors.h"
#include "HobList.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BackupRestoreErrors::dataType = 0;
std::string BackupRestoreErrors::dataTypeName("BackupRestoreErrors"); 
#endif

BackupRestoreErrors::BackupRestoreErrors()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

// private
BackupRestoreErrors::~BackupRestoreErrors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void BackupRestoreErrors::init()
{
	fileIndicatorsList = (HobList *) NULL;
	errorRecordsList   = (HobList *) NULL;
}


HobList *BackupRestoreErrors::getFileIndicatorsList()
{
	return fileIndicatorsList;
}
void BackupRestoreErrors::setFileIndicatorsList(HobList *fileIndicatorsList)
{
	this->fileIndicatorsList = fileIndicatorsList;
}

HobList *BackupRestoreErrors::getErrorRecordsList()
{
	return errorRecordsList;
}
void BackupRestoreErrors::setErrorRecordsList(HobList *errorRecordsList)
{
	this->errorRecordsList = errorRecordsList;
}


const char *BackupRestoreErrors::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if (fileIndicatorsList && errorRecordsList)
			{
				serializedData.append(fileIndicatorsList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				errorRecordsList->setQuotingElements(true);
				serializedData.append(errorRecordsList->serialize(format));
			}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}

