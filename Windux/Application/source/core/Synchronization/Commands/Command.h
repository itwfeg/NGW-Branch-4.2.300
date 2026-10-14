#ifndef COMMAND_H
#define COMMAND_H



#include "Messages/ProcessedMessage.h"
//#include "ProcessedMessage.h"

#include "CSmartPtr.h"



namespace synchronization
{


	class Command
	{
	public:
		/* --- Types and subtypes --- */
		typedef enum
		{
			PurgeBatchesCommandType,
			AbortBatchCommandType,
			ApplyBatchCommandType,
			DeleteBatchCommandType,
			ApplyAndDeleteBatchCommandType,

			SynchronizePrimaryScaleCommandType,
			RequestRegistrationCommandType,
			RequestDeregistrationCommandType,
			NotifyLocalBatchesPresentCommandType,
			DownloadLocalBatchesCommandType,
			NotifyOnlineCommandType,
			SynchronizeSecondaryScaleCommandType,
        #if 0
            // Alamy: We don't need it because "RequestSynchronizationCommand.cpp" and "UploadLocalBatchesCommand.cpp"
            // are not compiled. (Guess the 2 files are still under developing or for testing purpose)
            RequestSynchronizationCommandType,
            UploadLocalBatchesCommandType,
        #endif

			InvalidCommandType
		} CommandType;

		typedef enum
		{
			PendingState,
			ExecutingState,
			ExecutedState,
			RetryingState,

			InvalidState
		} CommandState;

		static const int _invalidSequenceNumber_ = 0;


		/* --- Constructors and destructor --- */
		Command(CommandType type, int targetId, ProcessedMessage* message = 0);
		Command(const char * buffer);
		virtual ~Command();


		/* --- Getters and setters --- */
		CommandType getType() const { return type_; }
		void setType(CommandType type) { type_ = type; }

		const std::string& getTypeName() const;

		int getTargetIdentifier() const { return targetId_; }
		void setTargetIdentifier(int targetId) { targetId_ = targetId; }

		int getQueue() const { return queue_; }
		void setQueue(int queue) { queue_ = queue; }

		int getSequenceNumber() const { return sequenceNumber_; }
		void setSequenceNumber(int seq) { sequenceNumber_ = seq; }

		CommandState getState() const { return state_; }
		void setState(CommandState state) { state_ = state; }
		bool isExecutingState() const { return state_ == ExecutingState; }
		void setExecutingState() { state_ = ExecutingState; }
		bool isRetryingState() const { return state_ == RetryingState; }
		void setRetryingState() { state_ = RetryingState; }

		int getRetryTimerId() const { return retryTimerId_; }
		void setRetryTimerId(int timerId) { retryTimerId_ = timerId; }

		bool isRetryingReadyToExecute() const { return retryingReadyToExecute_; }
		void setRetryingReadyToExecute(bool ready) { retryingReadyToExecute_ = ready; }

		int getStatus() const { return status_; }
		void setStatus(int status) { status_ = status; if ( completionMessage_.notNull() ) completionMessage_->setStatus(status); }

		Util::CSmartPtr<ProcessedMessage> getCompletionMessage() const { return completionMessage_; }
		void setCompletionMessage(Util::CSmartPtr<ProcessedMessage> message) { completionMessage_ = message; }

		virtual bool isCommCommand() const { return false; }


		/* --- Instance function members --- */
		virtual const char * serialize() const;
		UINT32 getSerializedLength() const { return (UINT32)serializedData.length(); }


	protected:
		mutable std::string serializedData;


	private:
		/* --- Data members --- */
		CommandType type_;
		int targetId_;
		int queue_;
		int sequenceNumber_;
		CommandState state_;
		int retryTimerId_;
		bool retryingReadyToExecute_;
		int status_;
		Util::CSmartPtr<ProcessedMessage> completionMessage_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		Command(const Command& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		Command& operator = (const Command& rhs);


		/* --- Class data members --- */
		static std::string PurgeBatchesCommandTypeName;
		static std::string AbortBatchCommandTypeName;
		static std::string ApplyBatchCommandTypeName;
		static std::string DeleteBatchCommandTypeName;
		static std::string ApplyAndDeleteBatchCommandTypeName;

		static std::string SynchronizePrimaryScaleCommandTypeName;
		static std::string RequestRegistrationCommandTypeName;
		static std::string RequestDeregistrationCommandTypeName;
		static std::string NotifyLocalBatchesPresentCommandTypeName;
		static std::string DownloadLocalBatchesCommandTypeName;
		static std::string NotifyOnlineCommandTypeName;
		static std::string SynchronizeSecondaryScaleCommandTypeName;

		static std::string InvalidCommandTypeName;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* COMMAND_H */
