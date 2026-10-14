#ifndef SYNCHRONIZATIONMANAGER_H
#define SYNCHRONIZATIONMANAGER_H



#include "HFCManagerContainer.h"
#include "IEvolMsgHandler.h"
#include "Timer.h"

#include "ScaleSynchronizationType.h"

#include "QueueManager.h"
#include "Scale.h"
#include "Batch.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/DeleteBatchProcessedMessage.h"

#include "util/thread/mutex.h"
#include "util/thread/lock.h"


class EvolMessage;


namespace Hobart
{
	class CEvolMsgManager;
	class TimerContainer;
}


namespace hfc
{
	class SynchronizationChange;
}


namespace synchronization
{


	class SynchronizationManagerProxy;
	class QueueManager;
	class Command;
	class GoingOffLineMessage;
	class ProcessUserChangesMessage;
	class ProcessScaleOnlineMessage;
	class CreateBatchMessage;
	class AddRecordToBatchMessage;
	class AbortBatchMessage;
	class CompleteBatchMessage;
	class ApplyBatchMessage;
	class ApplyBatchProcessedMessage;
	class DeleteBatchMessage;
	class DeleteBatchProcessedMessage;
	class CompleteCommandMessage;
	class RetryCommandMessage;
	class EmptyQueueMessage;


	class SynchronizationManager: public hfc::HFCManagerContainer, public Hobart::IEvolMsgHandler
	{
	public:
		/* --- Types and subtypes --- */
		static const int _statusNoCurrentUserBatch_               =   1;
		static const int _statusOK_                               =   0;
		static const int _statusBadParameter_                     =  -1;
		static const int _statusStoppingSyncMgr_                  =  -2;
		static const int _statusCannotCreateBatch_                =  -3;
		static const int _statusBatchNotFound_                    =  -4;
		static const int _statusCannotAbortACompleteBatch_        =  -5;
		static const int _statusBatchNotComplete_                 =  -6;
		static const int _statusCannotCompleteACompleteBatch_     =  -7;
		static const int _statusCannotFindPrimaryScale_           =  -8;
		static const int _statusCannotFindSecondaryScale_         =  -9;
		static const int _statusSyncMgrNotFullyFunctioningYet_    = -10;
		static const int _statusCannotAddRecordToCompleteBatch_   = -11;
		static const int _statusFailedToRemoveCommandFromQueue_   = -12;
		static const int _statusDBException_                      = -13;
		static const int _statusScaleOffLineFailure_              = -14;
		static const int _statusCannotFindCommand_                = -15;
		static const int _statusTooManyRetries_                   = -16;
		static const int _statusWrongTypeOfBatch_                 = -17;
		static const int _statusPrimaryScaleChanged_              = -18;

		class RetryTimer : public Timer
		{
			public:
				RetryTimer(Hobart::CEvolMsgReceiver* syncManager, QueueManager::QueueType queue, Command* command, int retryWait)
					: Timer(syncManager, retryWait, timedFuncRetryCommand),
					queue_(queue), command_(command) { setupEndDate(); }

				QueueManager::QueueType getQueue() const { return queue_; }
				Command* getCommand() const { return command_; }

				
			private:
				QueueManager::QueueType queue_;
				Command* command_;
		};


		class ThisScale
		{
			public:
				ThisScale(SynchronizationManager* syncMgr, int* lastBatchCompleted, bool online);
				~ThisScale() {}

				int getNewBatchNumber();
				int getNewBatchVersionNumber();

				int getCurrentUserBatchNumber() 
				{ 
					util_thread::Lock< util_thread::Mutex<> > lock(thisScaleMutex_); 
					return thisScale_->getVersionReceived(); 
				}
				
				int getAndClearCurrentUserBatchNumber() 
				{ 
					int retVal = thisScale_->getVersionReceived();				
					setCurrentUserBatchNumber(hfc::Batch::_invalidBatchNumber_); 
					return retVal; 
				}
				
				void setCurrentUserBatchNumber(int number);

				int getLastBatchCompletedVersion() 
				{ 
					util_thread::Lock< util_thread::Mutex<> > lock(thisScaleMutex_); 
					return lastBatchCompletedVersion_; 
				}
				
				void setLastBatchCompletedVersion(int version) 
				{ 
					util_thread::Lock< util_thread::Mutex<> > lock(thisScaleMutex_); 
					lastBatchCompletedVersion_ = version; 
				}


			private:
				SynchronizationManager* syncMgr_;
				Util::CSmartPtr<hfc::Scale> thisScale_;
				util_thread::Mutex<> thisScaleMutex_;

				int lastBatchCompletedVersion_;
		};


		/* --- Constructors and destructor --- */
		SynchronizationManager(ScaleSynchronizationType syncType, const char* name = "Synchronization Manager");
		virtual ~SynchronizationManager();


		/* --- Getter and setter methods --- */
		/* Use of the following methods should be protected under currentUserBatchIntegrityMutex_
		critical section to protect the integrity of the user batch. [That will probably make
		unnecessary the use of currentUserBatchIdMutex_ critical section.] */
		int getCurrentUserBatchNumber() { return thisScale_->getCurrentUserBatchNumber(); }
		int getAndClearCurrentUserBatchNumber() { return thisScale_->getAndClearCurrentUserBatchNumber(); }
		void setCurrentUserBatchNumber(int number) { thisScale_->setCurrentUserBatchNumber(number); }

		int getLastBatchCompletedVersion() const { return thisScale_->getLastBatchCompletedVersion(); }
		void setLastBatchCompletedVersion(int version) { thisScale_->setLastBatchCompletedVersion(version); }

#if defined( __linux__ )
        // This DOES NOT build on Windows.  Does not like in any way shape or form the debug statement. Haven't tried VM or buildserver yet
		SynchronizationManagerProxy* getProxy() const { Debug::trace( "SynchronizationManager", "getProxy() - returning * " + util_string::ptrtostring(proxy_.get()) );
return proxy_.get(); }
#else
		SynchronizationManagerProxy* getProxy() const { return proxy_.get(); }
#endif
		SynchronizationDevError getDisplayStatus() const { return displayStatus_; }
		void setDisplayStatus(SynchronizationDevError status) { displayStatus_ = status; }

		void setShutdownFlag() { shutdownFlag_ = true; }


		/* --- Instance Function members --- */
		const Hobart::CEvolMsgReceiver* getMessageReceiver() const { return msgManager_.get(); }
		Hobart::CEvolMsgReceiver* getMessageReceiver() { return msgManager_.get(); }

		/* "Use Case 5.5: Going off-line". */
		Util::CSmartPtr<EvolMessage> processGoingOffLineMessage(Util::CSmartPtr<GoingOffLineMessage> msg);
		/* "Use Case 5.6: Stopping Synchronization Manager". */
		int usecaseStoppingSynchronizationManager();
		/* "Use Case 5.8: Resuming processing". */
		int usecaseResumingProcessing();
		/* "Use Case 5.9: Pausing processing". */
		int usecasePausingProcessing(int reason);
		/* "Use Case 5.20: Processing user changes". */
		virtual Util::CSmartPtr<EvolMessage> processProcessUserChangesMessage(Util::CSmartPtr<ProcessUserChangesMessage> msg) = 0;

		Util::CSmartPtr<EvolMessage> processProcessScaleOnlineMessage(Util::CSmartPtr<ProcessScaleOnlineMessage> msg);
		/* "Use Case 5.40: Creating batch". */
		Util::CSmartPtr<EvolMessage> processCreateBatchMessage(Util::CSmartPtr<CreateBatchMessage> msg);
		/* "Use Case 5.41: Adding record to batch". */
		Util::CSmartPtr<EvolMessage> processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage> msg);
		/* "Use Case 5.42: Aborting batch". */
		Util::CSmartPtr<EvolMessage> processAbortBatchMessage(Util::CSmartPtr<AbortBatchMessage> msg);
		/* "Use Case 5.43: Completing batch". */
		Util::CSmartPtr<EvolMessage> processCompleteBatchMessage(Util::CSmartPtr<CompleteBatchMessage> msg);
		/* "Use Case 5.45: Applying batch". */
		Util::CSmartPtr<EvolMessage> processApplyBatchMessage(Util::CSmartPtr<ApplyBatchMessage> msg);
		Util::CSmartPtr<EvolMessage> processApplyBatch2Message(Util::CSmartPtr<ApplyBatchProcessedMessage> msg);
		/* "Use Case 5.46: Deleting batch". */
		Util::CSmartPtr<EvolMessage> processDeleteBatchMessage(Util::CSmartPtr<DeleteBatchMessage> msg);
		Util::CSmartPtr<EvolMessage> processDeleteBatch2Message(Util::CSmartPtr<DeleteBatchProcessedMessage> msg);
		/* "Use Case 5.55: Completing command". */
		Util::CSmartPtr<EvolMessage> processCompleteCommandMessage(Util::CSmartPtr<CompleteCommandMessage> msg);
		/* "Use Case 5.56: Retrying command". */
		Util::CSmartPtr<EvolMessage> processRetryCommandMessage(Util::CSmartPtr<RetryCommandMessage> msg);



	protected:
		/* --- Data members --- */
		Util::CSmartPtr<Hobart::CEvolMsgManager> msgManager_;
		Util::CSmartPtr<SynchronizationManagerProxy> proxy_;
		bool initialized_;
		bool scaleOnline_;
		bool fullyFunctioning_;
		HobDateTime fullyFunctioningChangedDateTime_;
		bool synchronizingAllScales_;
		bool pausingProcessing_;
		Util::CMSGEvent pausingProcessingEvent_;
		Util::CMSGEvent processEventThreadTopOfLoopEvent_;
		bool stoppingSynchronizationManager_;

		int currentUserBatchNumber_;
		mutable util_thread::Mutex<> currentUserBatchIdMutex_;
		// critical section to protect the integrity of the Current User Batch 
		mutable util_thread::Mutex<> currentUserBatchIntegrityMutex_;

		int synchronizeAllSecondariesTimerId_;
		int notifyLocalBatchesTimerId_;

		Util::CSmartPtr<QueueManager> commQManager_;
		util_thread::Mutex<> commQMutex_;
		Util::CSmartPtr<QueueManager> dbQManager_;
		util_thread::Mutex<> dbQMutex_;


		/* Use cases */
		/* "Use Case 5.5: Going off-line". */
		int usecaseGoingOffLine();

		int usecaseProcessingScaleOnlineMessage(int scaleId);
		/* "Use Case 5.40: Creating batch". */
		int usecaseCreatingBatch(int& batchId, hfc::Batch::BatchOrigin origin, hfc::Batch::BatchType type);
		/* "Use Case 5.41: Adding record to batch". */
		int usecaseAddingRecordToBatch(int batchId, Util::CSmartPtr<hfc::SynchronizationChange> record);
		/* "Use Case 5.42: Aborting batch". */
		int usecaseAbortingBatch(int batchId);
		/* "Use Case 5.43: Completing batch". */
		int usecaseCompletingBatch(int batchId);
		/* "Use Case 5.44: Completing current user batch". */
		int usecaseCompletingCurrentUserBatch(int& batchId);
		/* "Use Case 5.45: Applying batch". */
		int usecaseApplyingBatch(int batchId);
		int usecaseApplyingBatch2(int batchId);
		/* "Use Case 5.46: Deleting batch". */
		int usecaseDeletingBatch(int batch, DeleteBatchProcessedMessage::DeleteBatchMethod deleteMethod);
		int usecaseDeletingBatch2(int batchId, DeleteBatchProcessedMessage::DeleteBatchMethod deleteMethod);
		/* "Use Case 5.47: Cleaning up batches". */
		int usecaseCleaningUpBatches();

		/* "Use Case 5.51: Stopping Synchronizers". */
		int usecaseStoppingSynchronizers();
		/* "Use Case 5.53: Queuing command". */
		int usecaseQueuingCommand(QueueManager::QueueType queue, Util::CSmartPtr<Command> command);
		/* "Use Case 5.54: Executing command". */
		int usecaseExecutingCommand(QueueManager::QueueType queue);
		/* "Use Case 5.55: Completing command". */
		int usecaseCompletingCommand(QueueManager::QueueType queue, Util::CSmartPtr<Command> command);
		/* "Use Case 5.56: Retrying command". */
		int usecaseRetryingCommand1(QueueManager::QueueType queue, Util::CSmartPtr<Command> command);
		int usecaseRetryingCommand2(QueueManager::QueueType queue, Command* command);
		/* "Use Case 5.57: Emptying queue". */
		int usecaseEmptyingQueue(QueueManager::QueueType queue);


		void verifyThisScaleRecord(bool online);
		const std::string& getStatusText(int status) const;
		SynchronizationDevError filterDisplayStatus();
		void dispatchResponseMessage(Util::CSmartPtr<EvolMessageWrapper> originalMessage, Util::CSmartPtr<EvolMessage> response);

		bool isHQBatchAlreadyReceived(Util::CSmartPtr<hfc::Batch> batch);


		/* --- Class data members --- */


	private:
		/* --- Instance data members --- */
		ScaleSynchronizationType syncType_;
		Util::CSmartPtr<ThisScale> thisScale_;
		SynchronizationDevError displayStatus_;
		int retryMaximum_;                      // Up to 3 retries
		int retryCommandTimer_;                 // 5 minute timer
		bool shutdownFlag_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		SynchronizationManager(const SynchronizationManager& sm);


		/* --- Function members --- */
		/* Operators */
		// Operator asignment is not available
		SynchronizationManager& operator = (const SynchronizationManager& rhs);

		int getNewBatchVersionNumber() { return thisScale_->getNewBatchVersionNumber(); }


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static const std::string _dataTypeName_;
#endif

		static const std::string _statusNoCurrentUserBatchText_;
		static const std::string _statusOKText_;
		static const std::string _statusBadParameterText_;
		static const std::string _statusStoppingSyncMgrText_;
		static const std::string _statusCannotCreateBatchText_;
		static const std::string _statusBatchNotFoundText_;
		static const std::string _statusCannotAbortACompleteBatchText_;
		static const std::string _statusBatchNotCompleteText_;
		static const std::string _statusCannotCompleteACompleteBatchText_;
		static const std::string _statusCannotFindPrimaryScaleText_;
		static const std::string _statusCannotFindSecondaryScaleText_;
		static const std::string _statusSyncMgrNotFullyFunctioningYetText_;
		static const std::string _statusCannotAddRecordToCompleteBatchText_;
		static const std::string _statusFailedToRemoveCommandFromQueueText_;
		static const std::string _statusDBExceptionText_;
		static const std::string _statusScaleOffLineFailureText_;
		static const std::string _statusCannotFindCommandText_;
		static const std::string _statusTooManyRetriesText_;
		static const std::string _statusWrongTypeOfBatchText_;
		static const std::string _statusPrimaryScaleChangedText_;
		static const std::string _statusUnknownText_;
	};


}



#endif /* SYNCHRONIZATIONMANAGER_H */
