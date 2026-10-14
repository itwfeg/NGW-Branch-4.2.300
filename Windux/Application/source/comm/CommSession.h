#ifndef COMMSESSION_H
#define COMMSESSION_H



#include <list>


#include "AbstractSession.h"
#include "HFCManagerContainer.h"
#include "StateMachine/StateMachineController.h"


#include "StateMachine/StateMachine.h"


#include "CommSessionErrorEvent.h"
#include "HfcError.h"


#include "CommSessionEvent.h"


#include "HeaderRecord.h"
#include "SessionMessage.h"
#include "FooterRecord.h"


#include "CSmartPtr.h"
#include "MSGEvent.h"



class DisableInputProcessingMessage;


#include "util/time/dbtime.h"



namespace comm
{


	/* (TODO) remove AbstractSession from here replacing it with ISession, CEvolMsgReceiver, and
	CRunnableClass if we are not happy with the socket communications functionality inherited from
	AbstractSession. If we do this, then CommSession will need to have a socket instance and perform
	the reading and writing operations just like CommunicationSynchronizer. */
	class CommSession : public AbstractSession, public hfc::HFCManagerContainer, public IStateMachineController
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				INITIALIZING_STATE,
				READING_HEADER_STATE,
				PROCESSING_HEADER_STATE,
				SENDING_HEADER_STATE,
				READING_COMMAND_STATE,
				PROCESSING_COMMAND_STATE,
				PROCESSING_READ_ALL_INIT_SUBSTATE,
				PROCESSING_READ_ALL_READ_SUBSTATE,
				PROCESSING_READ_ALL_WRITE_SUBSTATE,
				PROCESSING_READ_ALL_DONE_SUBSTATE,
				PROCESSING_DELETE_ALL_INIT_SUBSTATE,
				PROCESSING_DELETE_ALL_READ_SUBSTATE,
				PROCESSING_DELETE_ALL_DELETE_SUBSTATE,
				PROCESSING_DELETE_ALL_DONE_SUBSTATE,
				PROCESSING_FOOTER_STATE,
				SENDING_FOOTER_STATE,
				ERROR_STATE,
				DISCONNECTING_STATE,
				CONCLUDING_STATE
			} stateId_type;
			typedef CommSessionEvent event_type;
			typedef StateMachine<CommSession> stateMachine_type;

			typedef std::list< Util::CSmartPtr<HfcError> > errorList_type;


			/* --- Constructors and destructor --- */
			CommSession(CommMechanism * uiClient = 0, const char* name = 0, bool enableInput = true, int reason = 0);
			virtual ~CommSession();


			/* --- Getters and setters --- */
			bool isPausingProcessing() const { return pausingProcessing_; }
			void setPausingProcessing(bool pausing) { pausingProcessing_ = pausing; }

			bool isAcceptingInput() const { return acceptingInput_; }
			void setAcceptingInput(bool accepting) { acceptingInput_ = accepting; }


			/* --- Instance function members --- */
			/* We override the Run and processEvent functions so that the corresponding AbstractSession
			functions will not be executed in the context of this component. */
			DWORD Run();
			void processEvent( const Util::CSmartPtr<EvolMessageWrapper> & /*msg*/ ) {}

			void debug(const std::string& category, int level, const std::string& message) const;
			bool atTopOfLoop();


		protected:
			/* --- Instance data members --- */
			Util::CSmartPtr<hfc::HeaderRecord> receivedHeader_;
			Util::CSmartPtr<SessionMessage> receivedCommand_;
			Util::CSmartPtr<HfcError> receivedError_;
			Util::CSmartPtr<SessionMessage> receivedFooter_;
			bool sentBusyError_;


			/* --- Instance function members --- */
			Util::CSmartPtr<SessionMessage> readMessage();
			bool writeHeader(Util::CSmartPtr<hfc::HeaderRecord> header);
			bool writeMessage(Util::CSmartPtr<SessionMessage> message);
			bool writeFooter(Util::CSmartPtr<SessionMessage> footer);

			void addError(Util::CSmartPtr<HfcError> error);
			void addBusyError();
			bool sendExistingErrors();

			bool isReadAllMessage(EvolDeviceType device, UINT8 event);
			bool isDeleteAllMessage(EvolDeviceType device, UINT8 event);

			// StateMachine's action functions
			bool processingInitializingState(const CommSessionEvent& event);
			bool processingReadingHeaderState(const CommSessionEvent& event);
			bool processingHeaderState(const CommSessionEvent& event);
			bool processingSendingHeaderState(const CommSessionEvent& event);
			bool enteringReadingCommandState(const CommSessionEvent& event);
			bool processingReadingCommandState(const CommSessionEvent& event);
			bool processingCommandState(const CommSessionEvent& event);
			bool processingReadAllInitSubState(const CommSessionEvent& event);
			bool processingReadAllReadSubState(const CommSessionEvent& event);
			bool processingReadAllWriteSubState(const CommSessionEvent& event);
			bool processingReadAllDoneSubState(const CommSessionEvent& event);
			bool processingDeleteAllInitSubState(const CommSessionEvent& event);
			bool processingDeleteAllReadSubState(const CommSessionEvent& event);
			bool processingDeleteAllDeleteSubState(const CommSessionEvent& event);
			bool processingDeleteAllDoneSubState(const CommSessionEvent& event);
			bool processingFooterState(const CommSessionEvent& event);
			bool processingSendingFooterState(const CommSessionEvent& event);
			bool enteringErrorState(const CommSessionEvent& event);
			bool processingErrorState(const CommSessionEvent& event);
			bool processingDisconnectingState(const CommSessionEvent& event);
			bool processingConcludingState(const CommSessionEvent& event);


			/* --- Interface to be implemented by subclasses --- */
			// doInitialization: implement this method to perform any initialization.
			// Return:
			//     true - the initialization is complete, move on
			//     false - still initializing, call again later
			virtual bool doInitialization() = 0;

			// doProcessHeader: implement this method to process the communication header.
			// Return:
			//     void
			virtual void doProcessHeader(Util::CSmartPtr<hfc::HeaderRecord> header) = 0;
			// doSendHeader: implement this method to send the communication header.
			// Return:
			//     true - the header was sent and it is ok to continue to read commands
			//     false - still sending header, call again later
			virtual bool doSendHeader(CommSessionEvent::CommError& returnedError) = 0;

			// doProcessCommand: implement this method to process a command.
			// Return:
			//     true - the command was processed and it is ok to continue
			//     false - still processing command, call again later
			virtual bool doProcessCommand(Util::CSmartPtr<SessionMessage> command) = 0;

			// doReadAllInit: implement this method to perform any initialization for a read all.
			// Return:
			//     true - the initialization is complete and there is data to be read
			//     false - there is no data to be read (abandon the read all processing)
			virtual bool doReadAllInit() { return true; }
			// doReadAllRead: implement this method to read a record for a read all.
			// Return:
			//     true - a record was read and it is ready to be written
			//     false - all records have been read
			virtual bool doReadAllRead() { return true; }
			// doReadAllWrite: implement this method to write a record for a read all.
			// Return:
			//     true - the record was written
			//     false - still writing the record, call again later
			virtual bool doReadAllWrite(CommSessionEvent::CommError& /*returnedError*/) { return true; }
			// doReadAllDone: implement this method to perform any processing after a read all.
			// Return:
			//     true - the command was processed and it is ok to continue
			//     false - still processing command, call again later
			virtual bool doReadAllDone() { return true; }

			// doProcessError: implement this method to perform any processing after an error occurred
			// Return:
			//     true - the error was processed and it is ok to continue to send footer
			//     false - 
			virtual void doProcessError(CommSessionEvent::CommError error) = 0;

			// doProcessFooter: implement this method to perform any processing before sending the footer
			// Return:
			//     true - the footer was processed and it is ok to continue to send footer
			//     false - still processing footer, call again later
			virtual bool doProcessFooter() = 0;
			// doSendFooter: implement this method to send the communication footer.
			// Return:
			//     true - the footer was sent and it is ok to continue to disconnect
			//     false - still sending footer, call again later
			virtual bool doSendFooter(CommSessionEvent::CommError& returnedError) = 0;

			// doConclusion: implement this method to perform any conclussion.
			// Return:
			//     true - the conclusion is complete, move on
			//     false - still concluding, call again later
			virtual bool doConclusion() = 0;


			virtual void pauseProcessing(int reason);
			virtual void resumeProcessing();


		private:
			/* --- Instance data members --- */
			Util::CSmartPtr<stateMachine_type> fsm_;
			bool pausingProcessing_;
#ifdef LINUX_FIX    // reordered to emliminate a gcc warning
			/* acceptingInput_:
				true - indicates input is processed normally
				false - no more input will be processed until receiving the footer */
			bool acceptingInput_;
			int textId_;       // Reason for pausing processing
#else
			int textId_;       // Reason for pausing processing
			/* acceptingInput_:
				true - indicates input is processed normally
				false - no more input will be processed until receiving the footer */
			bool acceptingInput_;
#endif
			util_time::Time startedReadingMessageTime_;
			int timeoutReadingHeader_;
			int timeoutReadingMessage_;
			int readingMessageDelay_;
			errorList_type errorList_;


			/* --- Constructors and destructor --- */
			// Copy constructor is not available
			CommSession(const CommSession& cs);


			/* --- Instance function members --- */
			void initialize();
			void setupStateMachine();

			// Operator asignment is not available
			CommSession& operator = (const CommSession& rhs);


			/* --- Class Data Members --- */
	#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
	#endif

			static const std::string _statenameInitializing_;
			static const std::string _statenameReadingHeader_;
			static const std::string _statenameProcessingHeader_;
			static const std::string _statenameSendingHeader_;
			static const std::string _statenameReadingCommand_;
			static const std::string _statenameProcessingCommand_;
			static const std::string _statenameProcessingReadAllInitSubstate_;
			static const std::string _statenameProcessingReadAllReadSubstate_;
			static const std::string _statenameProcessingReadAllWriteSubstate_;
			static const std::string _statenameProcessingReadAllDoneSubstate_;
			static const std::string _statenameProcessingDeleteAllInitSubstate_;
			static const std::string _statenameProcessingDeleteAllReadSubstate_;
			static const std::string _statenameProcessingDeleteAllDeleteSubstate_;
			static const std::string _statenameProcessingDeleteAllDoneSubstate_;
			static const std::string _statenameProcessingFooter_;
			static const std::string _statenameSendingFooter_;
			static const std::string _statenameError_;
			static const std::string _statenameDisconnecting_;
			static const std::string _statenameConcluding_;
			static const std::string _statenameUnknown_;


			/* --- Class function members --- */
			const std::string& getStateName(const stateMachine_type::state_type* state);
	};


}



#endif /* COMMSESSION_H */
