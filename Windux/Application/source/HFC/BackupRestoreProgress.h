/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef BACKUPRESTOREPROGRESS_H
#define BACKUPRESTOREPROGRESS_H

#include "evolution.h"
#include "HfcObject.h"
#include "BackupRestoreManager.h"

typedef enum BRProgressFieldOffsets
{
	fieldOffsetBRPGroupIndicator = 1,
	fieldOffsetBRPFileIndicator,
	fieldOffsetBRPFileRecordCount,
	fieldOffsetBRPTotalFileRecordCount,
	fieldOffsetBRPTaskRecordCount,
	fieldOffsetBRPTotalTaskRecordCount,
	fieldOffsetBRPErrorCount,
	fieldOffsetBRPFreeMemory,
	fieldOffsetBRPFilesIgnoredRecordCount,
	fieldOffsetBRPTotalFilesIgnoredRecordCount,
	endBRProgressFieldOffsets
} BRProgressFieldOffsets_t;

class BackupRestoreProgress : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		BackupRestoreProgress();
		BackupRestoreProgress(int groupIndicator, int fileIndicator);
		~BackupRestoreProgress();

		int getGroupIndicator() const;
		void setGroupIndicator(int groupIndicator);

		int getFileIndicator() const;
		void setFileIndicator(int fileIndicator);

		long getFileRecordCount() const;
		void setFileRecordCount(long fileRecordCount);
		void incrementFileRecordCount(long inc = 1L);

		long getTotalFileRecordCount() const;
		void setTotalFileRecordCount(long totalFileRecordCount);

		long getTaskRecordCount() const;
		void setTaskRecordCount(long taskRecordCount);
		void incrementTaskRecordCount(long inc = 1L);

		long getTotalTaskRecordCount() const;
		void setTotalTaskRecordCount(long totalTaskRecordCount);

		long getErrorCount() const;
		void setErrorCount(long errorCount);
		void incrementErrorCount(long inc = 1L);

		unsigned long getFreeMemory() const;

		virtual const char *serialize(int format = BrowserFormat) const;

		long getFilesIgnoredRecordCount() const;
		void setFilesIgnoredRecordCount(long recordCount);
		void incrementFilesIgnoredRecordCount(long inc);

		long getTotalFilesIgnoredRecordCount() const;
		void setTotalFilesIgnoredRecordCount(long recordCount);
		void incrementTotalFilesIgnoredRecordCount(long inc);


	private:

		int groupIndicator;
		int fileIndicator;

		long fileRecordCount;
		long totalFileRecordCount;
		long filesIgnoredRecordCount;
		long totalFilesIgnoredRecordCount;

		long taskRecordCount;
		long totalTaskRecordCount;

		long errorCount;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //BACKUPRESTOREPROGRESS_H
