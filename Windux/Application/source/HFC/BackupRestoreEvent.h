#ifndef BACKUPRESTOREEVENT_H
#define BACKUPRESTOREEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	typedef enum BackupRestoreEvent
	{
		UnknownBackRestore,
		Backup,				//tell session to perform a backup
		Restore,			//tell session to perform a restore
		BadRecordScan,		//tell session to perform a BadRecordScan of the database
		Intranet,
		BRCancel,			//tell session to cancel current operation
		Terminate,
		Progress,
		ProgressBadRecords,
		InitiateReadAll,
		BackupEntireDB,
		ProcessFile			//	New, to process files in the pending folder and potentially write to another file.
	} BackupRestoreEvent_t;

	typedef enum BackupRestoreError
	{
		BackupRestoreUnknownError = BackupRestore * DEVICE_ERROR_OFFSET,

		BackupRestoreNoFileName,
		BackupRestoreInvalidFileName,
		BackupRestoreNoSlashAllowed,
		BackupRestoreNoSemicolonAllowed,
		BackupRestoreUndefined,
		BackupRestoreNoAsteriskAllowed,
		BackupRestoreNoQuestionMarkAllowed,
		BackupRestoreNoQuoteAllowed,
		BackupRestoreNoGreaterThanAllowed,
		BackupRestoreNoPlusMinusAllowed,
		BackupRestoreNoSeparatorAllowed,
		BackupRestoreNoSpaceAllowed,
		BackupRestoreNoDataSelected,

		BackupRestoreDeletingProductData,
		BackupRestoreDeletingResources,
		BackupRestoreDeletingOperators,
		BackupRestoreDeletingSelfServeData,
		BackupRestoreDeletingCOOLData,
		BackupRestoreDeletingSoftkeyData,

		BackupRestoreFileWriteError,
		BackupRestoreCountingRecords,
		BackupRestoreSelfServeModified

	} BackupRestoreError_t;

#endif // BACKUPRESTOREEVENT_H
