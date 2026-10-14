#ifndef SYNCHRONIZATIONDEVEVENT_H
#define SYNCHRONIZATIONDEVEVENT_H



#include "Event.h"



namespace synchronization
{


	typedef enum SynchronizationEvent
	{
		SynchronizationEventUnknown,

		StartSyncManagerEvent,                     
		StartSyncManagerProcessedEvent,
		ComingUpOnLineEvent,
		SynchronizePrimaryScaleProcessedEvent,
		GoingOffLineEvent,                            // 005
		RequestRegistrationProcessedEvent,
		RegisterSecondaryScaleEvent,
		RegisterSecondaryScaleProcessedEvent,
		DeregisterSecondaryScaleEvent,
		DeregisterSecondaryScaleProcessedEvent,       // 010
		ProcessUserChangesEvent,
		ProcessUserChangesProcessedEvent,
		ProcessHQBatchEvent,
		ProcessHQBatchProcessedEvent,
		DownloadLocalBatchesEvent,                    // 015
		DownloadLocalBatchesProcessedEvent,
		NotifyLocalBatchesLazilyEvent,
		ProcessUserFileEvent,
		ProcessUserFileProcessedEvent,
		NotifyOnlineProcessedEvent,                   // 020
		SynchronizeSecondaryScaleEvent,
		SynchronizeSecondaryScaleProcessedEvent,
		SynchronizeSecondaryScalesLazilyEvent,
		ProcessRemoteBatchEvent,
		ProcessRemoteBatchProcessedEvent,             // 025
		ProcessSynchronizedBatchEvent,
		ProcessSynchronizedBatchProcessedEvent,
		ProcessScaleOnlineEvent,
		CreateBatchEvent,
		CreateBatchProcessedEvent,                    // 030
		AddRecordToBatchEvent,
		AddRecordToBatchProcessedEvent,
		AbortBatchEvent,
		AbortBatchProcessedEvent,
		CompleteBatchEvent,                           // 035
		CompleteBatchProcessedEvent,
		ApplyBatchEvent,
		ApplyBatchProcessedEvent,
		DeleteBatchEvent,
		DeleteBatchProcessedEvent,                    // 040
		StopSynchronizerEvent,
		ExecuteCommandEvent,
		CompleteCommandEvent,
		RetryCommandEvent,

		/* Commands sent through comm. */
		RegisterSecondaryScale = 100,                 // 100
		DeregisterSecondaryScale,
		NotifyOnline,
		NotifyLocalBatchesPresent,
		ReadBatches,
		WriteBatch,                                   // 105
		WriteSynchronizationChange,

		/* Synchronization Status Events */
		SynchronizationStatusUpdateEvent = 200,       // 200
		RequestSynchronizationStatusEvent,
		NewSynchronizationStatusEvent,

		SynchronizationEventLast
	}   SynchronizationEvent_t;


	typedef enum SynchronizationDevError
	{
		UnknownSyncError = SynchronizationDev * DEVICE_ERROR_OFFSET,

		SynchronizationActiveSyncStatus,					//  146001 - "Synchronization Active"

		SendingChangesSyncStatus,							//  146002 - "Sending Changes"
		ReceivingChangesSyncStatus,							//  146003 - "Receiving Changes"
		ApplyingChangesSyncStatus,							//  146004 - "Applying Changes"
		UnableToContactPrimarySyncStatus,					//  146005 - "Unable to contact Primary"
		ClearStatusUpInHere,								//	146006 ->

		SynchronizingScalesSyncStatus,						//  146020 - "Synchronizing Scales"

		NoErrorSyncError = 146100,							//  146100
		InitSynchronizationMechanismSyncError,				//  146101 - "Initializing Synchronization mechanism. Wait until procedure completes"
		ChangingSynchronizationTypeSyncError,				//  146102 - "Changing Synchronization Type. Wait until procedure completes"
		ChangingPrimaryScaleSyncError,						//  146103 - "Changing Primary Scale. Wait until procedure completes"
		ShutdownSynchronizationMechanismSyncError,			//  146104 - "Shutting down Synchronization mechanism."

		SynchronizationErrLast
	}   SynchronizationDevError_t;


}



#endif /* SYNCHRONIZATIONDEVEVENT_H */
