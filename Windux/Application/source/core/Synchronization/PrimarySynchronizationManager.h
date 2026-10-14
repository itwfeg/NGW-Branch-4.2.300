#ifndef PRIMARYSYNCHRONIZATIONMANAGER_H
#define PRIMARYSYNCHRONIZATIONMANAGER_H


#include "SynchronizationManager.h"


namespace synchronization
{


	class StartSyncManagerMessage;
	class ComingUpOnLineMessage;
	class SynchronizePrimaryScaleProcessedMessage;
	class RegisterSecondaryScaleMessage;
	class DeregisterSecondaryScaleMessage;
	class ProcessHQBatchMessage;
	class DownloadLocalBatchesMessage;
	class ProcessUserFileMessage;
	class SynchronizeSecondaryScaleMessage;
	class SynchronizeSecondaryScalesLazilyMessage;
	class ProcessRemoteBatchMessage;


	class PrimarySynchronizationManager : public SynchronizationManager
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		PrimarySynchronizationManager(bool online);
		virtual ~PrimarySynchronizationManager();


		/* --- Function members --- */
		void processEvent(Util::CSmartPtr<EvolMessageWrapper> & msg);
		void processSynchronizationEvent(Util::CSmartPtr<EvolMessageWrapper> & msg);

		/* Functions that process events that trigger use cases. */
		/* "Use Case 5.2: Starting up Synchronization Manager". */
		Util::CSmartPtr<EvolMessage> processStartSyncManagerMessage(Util::CSmartPtr<StartSyncManagerMessage> msg);
		/* "Use Case 5.3: Coming up on-line". */
		Util::CSmartPtr<EvolMessage> processComingUpOnLineMessage(Util::CSmartPtr<ComingUpOnLineMessage> msg);
		Util::CSmartPtr<EvolMessage> processComingUpOnLine2Message(Util::CSmartPtr<SynchronizePrimaryScaleProcessedMessage> msg);
		/* "Use Case 5.13: Registering Secondary scale". */
		Util::CSmartPtr<EvolMessage> processRegisterSecondaryScaleMessage(Util::CSmartPtr<RegisterSecondaryScaleMessage> msg);
		/* "Use Case 5.15: Deregistering Secondary scale". */
		Util::CSmartPtr<EvolMessage> processDeregisterSecondaryScaleMessage(Util::CSmartPtr<DeregisterSecondaryScaleMessage> msg);
		/* "Use Case 5.20: Processing user changes". */
		Util::CSmartPtr<EvolMessage> processProcessUserChangesMessage(Util::CSmartPtr<ProcessUserChangesMessage> msg);
		/* "Use Case 5.21: Processing HQ batch". */
		Util::CSmartPtr<EvolMessage> processProcessHQBatchMessage(Util::CSmartPtr<ProcessHQBatchMessage> msg);
		/* "Use Case 5.24: Downloading Secondary scale local batches". */
		Util::CSmartPtr<EvolMessage> processDownloadLocalBatchesMessage(Util::CSmartPtr<DownloadLocalBatchesMessage> msg);
		/* "Use Case 5.28: Processing user file". */
		Util::CSmartPtr<EvolMessage> processProcessUserFileMessage(Util::CSmartPtr<ProcessUserFileMessage> msg);
		/* "Use Case 5.32: Synchronizing Secondary scale". */
		Util::CSmartPtr<EvolMessage> processSynchronizeSecondaryScaleMessage(Util::CSmartPtr<SynchronizeSecondaryScaleMessage> msg);
		/* "Use Case 5.35: Synchronizing all Secondary scales lazily". */
		Util::CSmartPtr<EvolMessage> processSynchronizeSecondaryScalesLazilyMessage(Util::CSmartPtr<SynchronizeSecondaryScalesLazilyMessage> msg);
		/* "Use Case 5.36: Processing remote batch". */
		Util::CSmartPtr<EvolMessage> processProcessRemoteBatchMessage(Util::CSmartPtr<ProcessRemoteBatchMessage> msg);


	private:
		/* --- Instance data members --- */
		int numberSecondaryScales_;
		HobDateTime synchronizeAllSecondariesTimerStarted_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		PrimarySynchronizationManager(const PrimarySynchronizationManager& psm);

		/* --- Function members --- */
		/* Operators */
		// Operator asignment is not available
		PrimarySynchronizationManager& operator = (const PrimarySynchronizationManager& rhs);

		/* Functions that implement use cases */
		/* "Use Case 5.2: Starting up Synchronization Manager". */
		int usecaseStartingUpSynchronizationManager(bool online);
		/* "Use Case 5.3: Coming up on-line". */
		int usecaseComingUpOnLine();
		int usecaseComingUpOnLine2(int result);
		/* "Use Case 5.13: Registering Secondary scale". */
		int usecaseRegisteringSecondaryScale(int scaleId);
		/* "Use Case 5.15: Deregistering Secondary scale". */
		int usecaseDeregisteringSecondaryScale(int scaleId);
		/* "Use Case 5.20: Processing user changes". */
		int usecaseProcessingUserChanges(Util::CSmartPtr<hfc::SynchronizationChange> userChange);
		/* "Use Case 5.21: Processing HQ batch". */
		int usecaseProcessingHQBatchMessage(int batchNumber);
		/* "Use Case 5.24: Uploading Secondary scale local batches". */
		int usecaseDownloadingLocalBatches(int scaleId);
		/* "Use Case 5.28: Processing user file". */
		int usecaseProcessingUserFile(int batchId);
		/* "Use Case 5.31: Synchronizing all Secondary scales". */
		int usecaseSynchronizingAllSecondaryScales();
		/* "Use Case 5.32: Synchronizing Secondary scale". */
		int usecaseSynchronizingSecondaryScale(int scaleId);
		/* "Use Case 5.35: Synchronizing all Secondary scales lazily". */
		int usecaseSynchronizingSecondaryScalesLazily();
		int usecaseSynchronizingSecondaryScalesLazily2();
		/* "Use Case 5.36: Processing remote batch". */
		int usecaseProcessingRemoteBatch(int batchId);


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static const std::string _dataTypeName_;
#endif
	};


}


#endif /* PRIMARYSYNCHRONIZATIONMANAGER_H */
