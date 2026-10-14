#ifndef SECONDARYSYNCHRONIZATIONMANAGER_H
#define SECONDARYSYNCHRONIZATIONMANAGER_H



#include "SynchronizationManager.h"



namespace hfc
{
	class SynchronizationChange;
}


class EvolMessage;
class EvolMessageWrapper;



namespace synchronization
{


	class StartSyncManagerMessage;
	class ComingUpOnLineMessage;
	class GoingOffLineMessage;
	class ProcessUserChangesMessage;
	class ProcessHQBatchMessage;
	class ProcessUserFileMessage;
	class NotifyLocalBatchesLazilyMessage;
	class ProcessRemoteBatchMessage;
	class ProcessSynchronizedBatchMessage;
	class CreateBatchMessage;


	class SecondarySynchronizationManager : public SynchronizationManager
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		SecondarySynchronizationManager(bool online, bool registration = false,
				const std::string& primaryScale = "");
		virtual ~SecondarySynchronizationManager();


		/* --- Function members --- */
		void processEvent(Util::CSmartPtr<EvolMessageWrapper>& msg);
		void processSynchronizationEvent(Util::CSmartPtr<EvolMessageWrapper>& msg);

		/* Functions that process events that trigger use cases. */
		/* "Use Case 5.2: Starting up Synchronization Manager". */
		Util::CSmartPtr<EvolMessage> processStartSyncManagerMessage(Util::CSmartPtr<StartSyncManagerMessage> msg);
		/* "Use Case 5.3: Coming up on-line". */
		Util::CSmartPtr<EvolMessage> processComingUpOnLineMessage(Util::CSmartPtr<ComingUpOnLineMessage> msg);
		Util::CSmartPtr<EvolMessage> processComingUpOnLine2Message(Util::CSmartPtr<ProcessedMessage> msg);
		/* "Use Case 5.20: Processing user changes". */
		Util::CSmartPtr<EvolMessage> processProcessUserChangesMessage(Util::CSmartPtr<ProcessUserChangesMessage> msg);
		/* "Use Case 5.21: Processing HQ batch". */
		Util::CSmartPtr<EvolMessage> processProcessHQBatchMessage(Util::CSmartPtr<ProcessHQBatchMessage> msg);
		/* "Use Case 5.27: Notifying local batches lazily". */
		Util::CSmartPtr<EvolMessage> processNotifyLocalBatchesLazilyMessage(Util::CSmartPtr<NotifyLocalBatchesLazilyMessage> msg);
		/* "Use Case 5.28: Processing user file". */
		Util::CSmartPtr<EvolMessage> processProcessUserFileMessage(Util::CSmartPtr<ProcessUserFileMessage> msg);
		/* "Use Case 5.36: Processing remote batch". */
		Util::CSmartPtr<EvolMessage> processProcessRemoteBatchMessage(Util::CSmartPtr<ProcessRemoteBatchMessage> msg);
		/* "Use Case 5.37: Processing synchronized batch". */
		Util::CSmartPtr<EvolMessage> processProcessSynchronizedBatchMessage(Util::CSmartPtr<ProcessSynchronizedBatchMessage> msg);


	private:
		/* --- Instance data members --- */
		HobDateTime notifyLocalBatchesTimerStarted_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		SecondarySynchronizationManager(const SecondarySynchronizationManager& ssm);


		/* --- Function members --- */
		/* Operators */
		// Operator asignment is not available
		SecondarySynchronizationManager& operator = (const SecondarySynchronizationManager& rhs);


		/* Functions that implement use cases */
		/* "Use Case 5.2: Starting up Synchronization Manager". */
		int usecaseStartingUpSynchronizationManager(bool online, bool registration, const std::string& primaryScale);
		/* "Use Case 5.3: Coming up on-line". */
		int usecaseComingUpOnLine(bool registration);
		int usecaseComingUpOnLine2(int status);
		/* "Use Case 5.20: Processing user changes". */
		int usecaseProcessingUserChanges(Util::CSmartPtr<hfc::SynchronizationChange> userChange);
		/* "Use Case 5.21: Processing HQ batch". */
		int usecaseProcessingHQBatchMessage(int batchNumber);
		/* "Use Case 5.27: Notifying local batches lazily". */
		int usecaseNotifyingLocalBatchesLazily();
		int usecaseNotifyingLocalBatchesLazily2();
		/* "Use Case 5.28: Processing user file". */
		int usecaseProcessingUserFile(int batchId);
		/* "Use Case 5.36: Processing remote batch". */
		int usecaseProcessingRemoteBatch(int batchId);
		/* "Use Case 5.37: Processing synchronized batch". */
		int usecaseProcessingSynchronizedBatch(int batchId);


		bool verifyPrimaryScaleRecord(const std::string& primaryScale);


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static const std::string _dataTypeName_;
#endif
	};


}


#endif /* SECONDARYSYNCHRONIZATIONMANAGER_H */
