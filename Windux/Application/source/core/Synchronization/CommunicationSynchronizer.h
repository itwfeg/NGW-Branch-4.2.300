#ifndef COMMUNICATIONSYNCHRONIZER_H
#define COMMUNICATIONSYNCHRONIZER_H



#include "Synchronizer.h"
#include "CTLSThread.h"
#include "StateMachine/StateMachineController.h"


#include "StateMachine/StateMachine.h"
#include "CommunicationSynchronizerEvent.h"


#include "ISynchronizationUseCase.h"


#include "Scale.h"

#include "HobSocket.h"
#include "Commands/Command.h"
#include "HeaderRecord.h"
#include "FooterRecord.h"


#include "CSmartPtr.h"


#include "util/time/dbtime.h"


class SessionMessage;
class EvolMessageWrapper;


namespace Hobart
{
	class CEvolMsgReceiver;
}


namespace hfc
{
	class HeaderRecord;
}


namespace synchronization
{


	class QueueManager;
	class SynchronizationManagerProxy;


	class CommunicationSynchronizer: public Synchronizer, public Util::CRunnableClass, public IStateMachineController
	{
//		friend class DownloadLocalBatchesUseCase;
//		friend class SynchronizeSecondaryScaleUseCase;


	public:
		/* --- Types and subtypes --- */
		typedef enum
		{
			INITIALIZING_STATE,
			CONNECTING_STATE,
			SENDING_HEADER_STATE,
			READING_HEADER_STATE,
			PROCESSING_HEADER_STATE,
			SENDING_COMMAND_STATE,
			READING_COMMAND_STATE,
			PROCESSING_COMMAND_STATE,
			SENDING_FOOTER_STATE,
			PROCESSING_FOOTER_STATE,
			ERROR_STATE,
			DISCONNECTING_STATE,
			CONCLUDING_STATE
		} stateId_type;
		typedef CommunicationSynchronizerEvent event_type;
		typedef StateMachine<CommunicationSynchronizer> stateMachine_type;

		typedef enum
		{
			STATUS_INVALID,

			STATUS_FAILED,

			STATUS_NOT_COMPLETED,

			STATUS_COMPLETED_COMMAND,
			STATUS_COMPLETED_ALL_COMMANDS,

			STATUS_LAST
		} returnStatus_type;

		typedef std::list< Util::CSmartPtr<HfcError> > errorList_type;


		/* --- Constructors and destructor --- */
		CommunicationSynchronizer(Hobart::CEvolMsgReceiver* syncManager, hfc::Scale::ScaleRole scaleRole);
		~CommunicationSynchronizer();


		/* --- Getter and setter methods --- */
		bool isProcessingData() const { return processingData_; }
		void setProcessingData(bool processing) { processingData_ = processing; }

		Util::CSmartPtr<hfc::Scale> getTargetScale() const { return targetScale_; }


		/* --- Instance Function members --- */
		DWORD Run();
		void waitUntilRunThreadEnds();

		void processEvent(Util::CSmartPtr<EvolMessageWrapper> & msg);
		void processCommand(Util::CSmartPtr<Command> command);

		bool sendMessage(Util::CSmartPtr<SessionMessage> message, bool isCommand = true);
		void addError(Util::CSmartPtr<HfcError> error);
		bool sendExistingErrors();

		void debug(const std::string& category, int level, const std::string& message) const;
		bool atTopOfLoop();

		/* Use cases */
		/* "Use Case 5.54: Executing command". */
		Util::CSmartPtr<EvolMessage> processExecuteCommandMessage(Util::CSmartPtr<ExecuteCommandMessage> msg);


	protected:
		/* Use cases */
		/* "Use Case 6.54: Executing command". */
		int usecaseExecutingCommand(Util::CSmartPtr<Command> command);
		/* "Use Case 6.58: Resuming processing". */
		int usecaseResumingProcessing();
		/* "Use Case 6.59: Pausing processing". */
		int usecasePausingProcessing(int reason);


	private:
		/* --- Instance Data members --- */
		Util::CSmartPtr<Util::CTLSThread> thread_;
		Util::CSmartPtr<stateMachine_type> fsm_;
		hfc::Scale::ScaleRole scaleRole_;
		CommSessionEvent::CommError result_;
		bool connected_;
		Util::CSmartPtr<Command> command_;
		Util::CSmartPtr<hfc::Scale> targetScale_;
		Util::CSmartPtr<ISynchronizationUseCase> usecase_;
		Util::CSmartPtr<Socket> socket_;
		Util::CSmartPtr<hfc::HeaderRecord> receivedHeader_;
		Util::CSmartPtr<SessionMessage> receivedCommand_;
		Util::CSmartPtr<HfcError> receivedError_;
		Util::CSmartPtr<SessionMessage> receivedFooter_;
		errorList_type errorList_;
		/* processingData_:
			true - indicates output is generated and input processed normally
			false - no more output will be generated nor input processed with the exception of the footer */
		bool processingData_;
		int textId_;     // Reason for no longer processing data
		util_time::Time waitingForConnectTime_;
		int waitBeforeConnTimer_;
		int waitBeforeConnDelay_;
    #if defined(LINUX_FIX)
		bool commandSent_;
		bool headerReceived_;
    #else
		bool headerReceived_;
		bool commandSent_;
    #endif
		int timeoutReadingMessage_;
		int readingMessageDelay_;
		bool sentBusyError_;
		bool footerSent_;
		bool waitingForFooter_;
		util_time::Time waitingForFooterTime_;
		bool processedFooter_;
		bool needToDisconnect_;
		bool targetBusy_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		CommunicationSynchronizer(const CommunicationSynchronizer& sm);


		/* --- Instance function members --- */
		/* Operators */
		// Operator asignment is not available
		CommunicationSynchronizer& operator = (const CommunicationSynchronizer& rhs);

    #if defined(LINUX_FIX)
		Util::CSmartPtr<SessionMessage> readMessage();
    #else
		Util::CSmartPtr<::SessionMessage> readMessage();
    #endif
		void sendBusyError();

		void setupStateMachine();

		// StateMachine's action functions
		bool processingInitializingState(const event_type& event);
		bool processingConnectingState(const event_type& event);
		bool processingSendingHeaderState(const event_type& event);
		bool processingReadingHeaderState(const event_type& event);
		bool processingProcessingHeaderState(const event_type& event);
		bool processingSendingCommandState(const event_type& event);
		bool processingReadingCommandState(const event_type& event);
		bool processingProcessingCommandState(const event_type& event);
		bool processingSendingFooterState(const event_type& event);
		bool processingErrorState(const event_type& event);
		bool processingProcessingFooterState(const event_type& event);
		bool processingDisconnectingState(const event_type& event);
		bool processingConcludingState(const event_type& event);

		void processErrorCode(CommSessionEvent::CommError error);
		int processResult();


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static const std::string _dataTypeName_;
#endif

		static const std::string _statenameInitializing_;
		static const std::string _statenameConnecting_;
		static const std::string _statenameSendingHeader_;
		static const std::string _statenameReadingHeader_;
		static const std::string _statenameProcessingHeader_;
		static const std::string _statenameSendingCommand_;
		static const std::string _statenameReadingCommand_;
		static const std::string _statenameProcessingCommand_;
		static const std::string _statenameSendingFooter_;
		static const std::string _statenameProcessingFooter_;
		static const std::string _statenameError_;
		static const std::string _statenameDisconnecting_;
		static const std::string _statenameConcluding_;
		static const std::string _statenameUnknown_;


		/* --- Class function members --- */
		const std::string& getStateName(const stateMachine_type::state_type* state);
	};


}



#endif /* COMMUNICATIONSYNCHRONIZER_H */
