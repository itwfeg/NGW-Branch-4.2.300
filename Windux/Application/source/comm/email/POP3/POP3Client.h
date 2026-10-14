#ifndef POP3CLIENT_H
#define POP3CLIENT_H



#include <string>
#include <list>


#include "CTLSThread.h"
#include "CSmartPtr.h"
#include "StateMachine/StateMachineController.h"
template <class Controller> class StateMachine;


#include "POP3ClientEvent.h"
#include "POP3Command.h"
#include "POP3Capability.h"
#include "POP3AuthenticationMethod.h"

#include "HobSocket.h"
#include "util/containers/circularbuffer.h"
#include "util/time/datetime.h"



namespace email
{


	class POP3ClientInterface;


	class POP3Client : public Util::CRunnableClass, public IStateMachineController
	{
		public:
			// --- Types and subtypes ---
			enum ServerResponses
			{
				COMM_ERROR_RESPONSE,                // There was a socket error
				POSITIVE_RESPONSE,                  // +OK response
				NEGATIVE_RESPONSE,                  // -ERR response
				MALFORMED_RESPONSE,                 // Invalid/malformed response
				INCOMPLETE_RESPONSE,                // Keep reading ...
				TIMEDOUT_RESPONSE                   // Timed out
			};
			typedef enum ServerResponses ServerResponse;

			enum States
			{
				INITIALIZING_STATE,
				CONNECTING_STATE,
				CAPABILITY_STATE,
				AUTHENTICATING_STATE,
				TRANSACTING_STATE,
				CONCLUDING_STATE,
				DISCONNECTING_STATE,
				ERROR_STATE
			};
			typedef enum States stateId_type;
			typedef POP3ClientEvent event_type;
			typedef StateMachine<POP3Client> stateMachine_type;

			typedef container::CircularBuffer<char> buffer_type;
			typedef buffer_type::iterator buffer_iterator;
			typedef buffer_type::const_iterator buffer_const_iterator;

			typedef std::vector<std::string> responseContainer_type;
			typedef responseContainer_type::iterator responseContainer_iterator;

			typedef std::list<POP3Capability> capabilityContainer_type;
			typedef capabilityContainer_type::iterator capabilityContainer_iterator;

			typedef std::list<POP3Command> commandContainer_type;
			typedef commandContainer_type::iterator commandContainer_iterator;

			typedef std::list< Util::CSmartPtr<POP3AuthenticationMethod> > authenticationContainer_type;
			typedef authenticationContainer_type::iterator authenticationContainer_iterator;

			static const int RESPONSE_BUFFER_SIZE = 1024;


			// --- Constructors and destructor ---
			POP3Client(const POP3ClientInterface& interface1);
			~POP3Client();

			// --- Getters and setters ---
			// Most of these methods are protected by critical sections so there will be no
			// conflict when either the POP3Client or the POP3ClientInterface, which run in
			// different threads, call them at the same time
			const std::string& getPOP3ServerName() const
					{ Util::CLockCriticalSection lock(serverParametersMutex_); return serverName_; }
			void setPOP3ServerName(const std::string& name)
					{ Util::CLockCriticalSection lock(serverParametersMutex_); serverName_ = name; }

			const int getPOP3ServerPort() const
					{ Util::CLockCriticalSection lock(serverParametersMutex_); return serverPort_; }
			void setPOP3ServerPort(int port) 
					{ Util::CLockCriticalSection lock(serverParametersMutex_); serverPort_ = port; }

			const std::string& getMailbox() const
					{ Util::CLockCriticalSection lock(serverParametersMutex_); return mailbox_; }
			void setMailbox(const std::string& mailbox)
					{ Util::CLockCriticalSection lock(serverParametersMutex_); mailbox_ = mailbox; }

			const std::string& getPassword() const
					{ Util::CLockCriticalSection lock(serverParametersMutex_); return password_; }
			void setPassword(const std::string& password)
					{ Util::CLockCriticalSection lock(serverParametersMutex_); password_ = password; }

			const int getReadResponseDelay() const
					{ Util::CLockCriticalSection lock(serverParametersMutex_); return readResponseDelay_; }
			void setReadResponseDelay(int delay)
					{ Util::CLockCriticalSection lock(serverParametersMutex_); readResponseDelay_ = delay; }

			// Other getter and setter members
			const std::string& getSingleLineResponse() const { return *currentResponse_.begin(); }

			bool isCommandAvailable(const POP3Command& command) const
					{ return find(availableCommands_.begin(), availableCommands_.end(), command) != availableCommands_.end(); }


			// --- Other instance function members ---
//			void debug(const std::string& category, int level, const std::string& message) const;


		protected:
			DWORD Run();


		private:
			const POP3ClientInterface& interface_;

			// The following are user configurable parameters to access the POP3 server
			std::string serverName_;                      // Hostname/IP address of POP3 server host
			int serverPort_;                              // Port to be used to connect to POP3 server
			std::string mailbox_;                         // Maildrop to inquire about
			std::string password_;
			mutable Util::CSmartCriticalSection serverParametersMutex_;     // Mutex needed to access/modify previous parameters

			Util::CSmartPtr< StateMachine<POP3Client> > fsm_;               // State machine
			Util::CSmartPtr<Socket> socket_;                                // Socket to communicate with POP3 server
			commandContainer_type availableCommands_;                       // Available commands at POP3 server
			std::string serverTimestamp_;
			bool responseCodes_;                                            // The POP3 server sends response codes

			capabilityContainer_type allCapabilities_;
			commandContainer_type transactionCommands_;
			commandContainer_iterator commandIterator_;
			authenticationContainer_type authenticationMethods_;
			authenticationContainer_iterator authenticationIterator_;
			std::string lastEmailNumber_;

			// Variables used to handle responses from the server
			buffer_type responseBuffer_;
			buffer_iterator rbCurrent_;
			responseContainer_type currentResponse_;
			responseContainer_type currentResponseCode_;
			DateTime responseReferenceTime_;
			bool responseAnalyzed_;
			int maxResponseTime_;
			int readResponseDelay_;
			bool waitingForResponse_;

			bool connectionEstablished_;
			bool firstPassCapability_;
			bool sentUserName_;
			bool sentPassword_;
			bool sentQuitCommand_;


			// Do not allow the default copy constructor.
			POP3Client(const POP3Client&);

			// Do not allow the default assignment operator.
			POP3Client operator=(const POP3Client& rhs);


			void initializeServerResponse();
			ServerResponse getServerResponse(bool multiline = false);
			ServerResponse analyzeResponse();
			bool sendCommand(const std::string& message);
			void processGreeting(const std::string& greeting);
			bool receivedTimestamp(const std::string& greeting);
			void processCapability(responseContainer_type response);


			// --- State Machine related methods ---
			void setupStateMachine();

			// Action functions
			bool processingInitializingState(const POP3ClientEvent& event);
			bool enteringConnectingState(const POP3ClientEvent& event);
			bool processingConnectingState(const POP3ClientEvent& event);
			bool enteringCapabilityState(const POP3ClientEvent& event);
			bool processingCapabilityState(const POP3ClientEvent& event);
			bool enteringAuthenticatingState(const POP3ClientEvent& event);
			bool processingAuthenticatingState(const POP3ClientEvent& event);
			bool enteringTransactingState(const POP3ClientEvent& event);
			bool processingTransactingState(const POP3ClientEvent& event);
			bool enteringConcludingState(const POP3ClientEvent& event);
			bool processingConcludingState(const POP3ClientEvent& event);
			bool processingDisconnectingState(const POP3ClientEvent& event);
			bool processingErrorState(const POP3ClientEvent& event);


			static const std::string _ThreadName_;
			static const int _ThreadDelay_;
			static const int _DefaultPOP3ServerPort_;
			static const int _MaxRetryTime_;
			static const int _idleDelay_;

			Util::CSmartPtr<Util::CTLSThread> checkingThread;

#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //POP3CLIENT_H

