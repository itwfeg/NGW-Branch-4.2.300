/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "BackupRestoreProgress.h"
#include "HobTokenizerCSV.h"
#include "platform/platform.h"
#include "Configuration.h"

#include "Integer.h"
#include "Long.h"
#include "ULong.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BackupRestoreProgress::dataType = 0;
std::string BackupRestoreProgress::dataTypeName("BackupRestoreProgress"); 
#endif

BackupRestoreProgress::BackupRestoreProgress()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

BackupRestoreProgress::BackupRestoreProgress(int groupIndicator, int fileIndicator)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	this->groupIndicator = groupIndicator;
	this->fileIndicator = fileIndicator;
}

// private
BackupRestoreProgress::~BackupRestoreProgress()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void BackupRestoreProgress::init()
{
	groupIndicator = 0;
	fileIndicator = 0;
	fileRecordCount = 0L;
	totalFileRecordCount = 0L;
	filesIgnoredRecordCount = 0L;
	taskRecordCount = 0L;
	totalTaskRecordCount = 0L;
	errorCount = 0L;
}

int BackupRestoreProgress::getGroupIndicator() const
{
	return groupIndicator;
}
void BackupRestoreProgress::setGroupIndicator(int groupIndicator)
{
	this->groupIndicator = groupIndicator;
}

int BackupRestoreProgress::getFileIndicator() const
{
	return fileIndicator;
}
void BackupRestoreProgress::setFileIndicator(int fileIndicator)
{
	this->fileIndicator = fileIndicator;
}

long BackupRestoreProgress::getFileRecordCount() const
{
	return fileRecordCount;
}
void BackupRestoreProgress::setFileRecordCount(long fileRecordCount)
{
	this->fileRecordCount = fileRecordCount;
}
void BackupRestoreProgress::incrementFileRecordCount(long inc)
{
	fileRecordCount += inc;
}

long BackupRestoreProgress::getTotalFileRecordCount() const
{
	return totalFileRecordCount;
}
void BackupRestoreProgress::setTotalFileRecordCount(long totalFileRecordCount)
{
	this->totalFileRecordCount = totalFileRecordCount;
}

long BackupRestoreProgress::getTaskRecordCount() const
{
	return taskRecordCount;
}
void BackupRestoreProgress::setTaskRecordCount(long taskRecordCount)
{
	this->taskRecordCount = taskRecordCount;
}
void BackupRestoreProgress::incrementTaskRecordCount(long inc)
{
	taskRecordCount += inc;
}

long BackupRestoreProgress::getTotalTaskRecordCount() const
{
	return totalTaskRecordCount;
}
void BackupRestoreProgress::setTotalTaskRecordCount(long totalTaskRecordCount)
{
	this->totalTaskRecordCount = totalTaskRecordCount;
}

long BackupRestoreProgress::getErrorCount() const
{
	return errorCount;
}
void BackupRestoreProgress::setErrorCount(long errorCount)
{
	this->errorCount = errorCount;
}
void BackupRestoreProgress::incrementErrorCount(long inc)
{
	errorCount += inc;
}

unsigned long BackupRestoreProgress::getFreeMemory() const
{
	util_platform::Platform & plat = util_platform::Platform::getInstance();

	// Amount of free flash memory
	long64 free = plat.fileSystemFreeSpace(Configuration::getInstance()->getCompactFlashDriveName());
	return (unsigned long)(free / 1024);
}


const char *BackupRestoreProgress::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
			serializedData = "";
			for (int loopCnt = 1; loopCnt < endBRProgressFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetBRPGroupIndicator:
						serializedData.append(Integer::toString(groupIndicator));
						break;
					case fieldOffsetBRPFileIndicator:
						serializedData.append(Integer::toString(fileIndicator));
						break;
					case fieldOffsetBRPFileRecordCount:
						serializedData.append(Long::toString(fileRecordCount));
						break;
					case fieldOffsetBRPTotalFileRecordCount:
						serializedData.append(Long::toString(totalFileRecordCount));
						break;
					case fieldOffsetBRPTaskRecordCount:
						serializedData.append(Long::toString(taskRecordCount));
						break;
					case fieldOffsetBRPTotalTaskRecordCount:
						serializedData.append(Long::toString(totalTaskRecordCount));
						break;
					case fieldOffsetBRPErrorCount:
						serializedData.append(Long::toString(errorCount));
						break;
					case fieldOffsetBRPFreeMemory:
						serializedData.append(ULong::toString(getFreeMemory()));
						break;
					case fieldOffsetBRPFilesIgnoredRecordCount:
						serializedData.append(Long::toString(filesIgnoredRecordCount));
						break;
					case fieldOffsetBRPTotalFilesIgnoredRecordCount:
						serializedData.append(Long::toString(totalFilesIgnoredRecordCount));
						break;
					default:
						break;
				}
			}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


long BackupRestoreProgress::getFilesIgnoredRecordCount() const
{
	return filesIgnoredRecordCount;
}
void BackupRestoreProgress::setFilesIgnoredRecordCount(long recordCount)
{
	this->filesIgnoredRecordCount = recordCount;
}
void BackupRestoreProgress::incrementFilesIgnoredRecordCount(long inc)
{
	filesIgnoredRecordCount += inc;
}

long BackupRestoreProgress::getTotalFilesIgnoredRecordCount() const
{
	return totalFilesIgnoredRecordCount;
}
void BackupRestoreProgress::setTotalFilesIgnoredRecordCount(long recordCount)
{
	this->totalFilesIgnoredRecordCount = recordCount;
}
void BackupRestoreProgress::incrementTotalFilesIgnoredRecordCount(long inc)
{
	totalFilesIgnoredRecordCount += inc;
}

