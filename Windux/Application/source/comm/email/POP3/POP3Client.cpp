#include "POP3Client.h"



#include "../Email.h"
#include "StateMachine/StateMachine.h"

#include "POP3Parser.h"

#include "Debug.h"
#include "Integer.h"



using namespace std;



namespace email
{
	#ifdef USE_ACCOUNTANT
		#include "util/accountant/accountanti.h"

		int POP3Client::_dataType_ = 0;
		std::string POP3Client::_dataTypeName_("email::POP3Client"); 
	#endif

	const std::string POP3Client::_ThreadName_("tPOP3Client");
	const int POP3Client::_ThreadDelay_ = 2 * 1000;              // In milliseconds
	const int POP3Client::_DefaultPOP3ServerPort_ = 110;         // POP3 well-known port
	const int POP3Client::_MaxRetryTime_ = 15 * 60;              // In Seconds (15 minutes)
	const int POP3Client::_idleDelay_ = 10;                      // in ticks



	POP3Client::POP3Client(const POP3ClientInterface& interface0)
			: interface_(interface0), serverPort_(_DefaultPOP3ServerPort_),
			responseBuffer_(RESPONSE_BUFFER_SIZE), responseReferenceTime_(), responseAnalyzed_(false), maxResponseTime_(20), readResponseDelay_(50) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

		checkingThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, _ThreadName_.c_str() );			

		// Initialize all commands list
		allCapabilities_.push_back(POP3Capability(POP3Capability::Top));
		allCapabilities_.push_back(POP3Capability(POP3Capability::User));
		allCapabilities_.push_back(POP3Capability(POP3Capability::Authentication));
		allCapabilities_.push_back(POP3Capability(POP3Capability::ResponseCodes));
		allCapabilities_.push_back(POP3Capability(POP3Capability::LoginDelay));
		allCapabilities_.push_back(POP3Capability(POP3Capability::Pipelining));
		allCapabilities_.push_back(POP3Capability(POP3Capability::Expire));
		allCapabilities_.push_back(POP3Capability(POP3Capability::UniqueIDList));
		allCapabilities_.push_back(POP3Capability(POP3Capability::SecureTransport));
		allCapabilities_.push_back(POP3Capability(POP3Capability::Implementation));
	}


	POP3Client::~POP3Client()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

		checkingThread->StopThread(); 
	}


/*	void POP3Client::debug(const std::string& category, int level, const std::string& message) const
	{
	}
*/

	DWORD POP3Client::Run()
	{
		// Initialize and setup the State Machine
		fsm_ = new StateMachine<POP3Client>(*this); 
		setupStateMachine(); 

		fsm_->start(new POP3ClientEvent(POP3ClientEvent::START_EVENT), INITIALIZING_STATE);
	                      
		fsm_->runStateMachine();

		if ( (*fsm_)() != stateMachine_type::STOPPED_STATE )
		{
		}
		else
		{
			Sleep(_ThreadDelay_);
		}

		return 0;
	}


	bool POP3Client::processingInitializingState(const POP3ClientEvent&)
	{
		// Initialize available commands list
		availableCommands_.clear();
		availableCommands_.push_back(POP3Command(POP3Command::Statistics));
		availableCommands_.push_back(POP3Command(POP3Command::List));
		availableCommands_.push_back(POP3Command(POP3Command::Retrieve));
		availableCommands_.push_back(POP3Command(POP3Command::Delete));
		availableCommands_.push_back(POP3Command(POP3Command::NoOperation));
		availableCommands_.push_back(POP3Command(POP3Command::Reset));
		availableCommands_.push_back(POP3Command(POP3Command::Quit));


		// Create the list of transaction commands
		transactionCommands_.clear();
		transactionCommands_.push_back(POP3Command(POP3Command::Statistics));
		transactionCommands_.push_back(POP3Command(POP3Command::UniqueIDList));
		transactionCommands_.push_back(POP3Command(POP3Command::Top));


		// Create the list of authentication methods
		authenticationMethods_.clear();


		responseCodes_ = false;


		fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::INITIALIZED_EVENT));

		return true;
	}


	bool POP3Client::enteringConnectingState(const POP3ClientEvent&)
	{
		connectionEstablished_ = false;

		return true;
	}


	bool POP3Client::processingConnectingState(const POP3ClientEvent&)
	{
		Debug::trace("Email.POP3", "processing Connecting State");

		if ( !connectionEstablished_ )
		{
			if ( socket_.notNull() )
			{
				Debug::trace("Email.POP3", "Closing and disposing of old socket.");
				socket_->cmClose();
			}

			socket_ = new Socket();
			socket_->setPort(serverPort_);
			socket_->setDnsName(serverName_);
  			responseReferenceTime_.getCurrentDateTime();

			Debug::trace("Email.POP3", "Connecting socket to host '" + serverName_ + "' at port " + util_string::inttostring(serverPort_));
			socket_->sConnect();
			// Sleep(100);
			if ( !socket_->isValid() )
			{
				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::CONNECTING_FAILED_EVENT));

				return true;
			}

			Debug::trace("Email.POP3", "Setting newly connected socket to non-blocking");
			socket_->setNonBlockingIO();

			initializeServerResponse();
			connectionEstablished_ = true;
		}


		// Check if we have a response from the server
		bool complete = false;
		Debug::trace("Email.POP3", "Attempting to read greeting from POP3 server.");
		switch ( getServerResponse() )
		{
			case INCOMPLETE_RESPONSE:
			{
				Sleep(readResponseDelay_);

				break;
			}
			case POSITIVE_RESPONSE:
			{
				Debug::trace("Email.POP3", "The POP3 server accepted the connection.");

				processGreeting(getSingleLineResponse());

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::CONNECTING_SUCCESS_EVENT));
				complete = true;

				break;
			}
			case NEGATIVE_RESPONSE:
			{
				Debug::trace("Email.POP3", "The POP3 server rejected our attempt to connect.");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::CONNECTING_REJECTED_EVENT));
				complete = true;

				break;
			}
			case TIMEDOUT_RESPONSE:
			{
				Debug::trace("Email.POP3", "The connection with the POP3 server was not successful (timed-out).");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TIMEDOUT_EVENT));
				complete = true;

				break;
			}
		}

		return complete;
	}


	bool POP3Client::enteringCapabilityState(const POP3ClientEvent& event)
	{
		firstPassCapability_ = (event.getId() == POP3ClientEvent::CONNECTING_SUCCESS_EVENT);

		waitingForResponse_ = false;

		return true;
	}


	bool POP3Client::processingCapabilityState(const POP3ClientEvent&)
	{
		if ( !waitingForResponse_ )
		{
			// Send the command to the POP3 server
			POP3Command command = POP3Command(POP3Command::Capability);
			const std::string& commandMessage = command.serialize();
			if ( !sendCommand(commandMessage) )
			{
				// Failed to send the command to the POP3 server
				Debug::trace("Email.POP3", "Failed to send command '" + commandMessage + "' to POP3 server.");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::COMM_ERROR_EVENT));

				return true;
			}

			initializeServerResponse();
			waitingForResponse_ = true;
		}


		// Check if we have a response from the server to the CAPA command
		bool complete = false;
		Debug::trace("Email.POP3", "Attempting to read capabilities from POP3 server.");
		switch ( getServerResponse(true) )
		{
			case INCOMPLETE_RESPONSE:
			{
				Sleep(readResponseDelay_);

				return false;
			}
			case POSITIVE_RESPONSE:
			{
				Debug::trace("Email.POP3", "The POP3 server supplied its capabilities.");

				processCapability(currentResponse_);

				fsm_->postEvent(new POP3ClientEvent(firstPassCapability_ ? POP3ClientEvent::FIRST_CAPABILITY_SUCCEEDED_EVENT : POP3ClientEvent::SECOND_CAPABILITY_SUCCEEDED_EVENT));
				complete = true;

				break;
			}
			case NEGATIVE_RESPONSE:
			{
				Debug::trace("Email.POP3", "The POP3 server failed to process capability command (it probably does not support it).");

				fsm_->postEvent(new POP3ClientEvent(firstPassCapability_ ? POP3ClientEvent::FIRST_CAPABILITY_FAILED_EVENT : POP3ClientEvent::SECOND_CAPABILITY_FAILED_EVENT));
				complete = true;

				break;
			}
			case TIMEDOUT_RESPONSE:
			{
				Debug::trace("Email.POP3", "Timed out waiting for response to capability command " + commandIterator_->getCommandName() + ".");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TIMEDOUT_EVENT));
				complete = true;

				break;
			}
		}


		return complete;
	}


	bool POP3Client::enteringAuthenticatingState(const POP3ClientEvent& event)
	{
		if ( event.getId() != POP3ClientEvent::TRY_NEXT_AUTHENTICATION_EVENT )
			authenticationIterator_ = authenticationMethods_.begin();
		else
			++authenticationIterator_;

		return true;
	}


	bool POP3Client::processingAuthenticatingState(const POP3ClientEvent&)
	{
		// If there are no more authentication methods available, then conclude
		if ( authenticationIterator_ == authenticationMethods_.end() )
		{
			fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::AUTHENTICATION_FAILED_EVENT));

			return true;
		}


		return true;
	}


/*	bool POP3Client::enteringAuthenticatingPasswordState(const POP3ClientEvent&)
	{
		sentUserName_ = false;
		sentPassword_ = false;

		return true;
	}


	bool POP3Client::processingAuthenticatingPasswordState(const POP3ClientEvent& event)
	{
		if ( !sentUserName_ )
		{
			std::string userMessage;
			
			userMessage.assign("USER ").append(mailbox_).append("\r\n");

			Debug::trace("Email.POP3", "Sending command: '" + userMessage + "'");
			sendCommand(userMessage);

			sentUserName_ = true;
		}

		if ( sentUserName_ && !sentPassword_ )
		{
			// Check if we have a response to the USER command from the server
			bool complete = false, failed = false;
			switch ( getServerResponse(20) )
			{
				case INCOMPLETE_RESPONSE:
				{
					Sleep (readResponseDelay_);

					break;
				}
				case POSITIVE_RESPONSE:
				{
					complete = true;

					break;
				}
				case NEGATIVE_RESPONSE:
				{
//					if ( ++authenticationMethod_ != authenticationMethods_.end() )
//						fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TRY_NEXT_AUTHENTICATION_EVENT));
//					else
						fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::AUTHENTICATION_FAILED_EVENT));
					complete = true;
					failed = true;

					break;
				}
				case TIMEDOUT_RESPONSE:
				{
					Debug::trace("Email.POP3", "Failed to authenticate (timed-out).");

					fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TIMEDOUT_EVENT));
					complete = true;
					failed = true;

					break;
				}
			}

			if ( !complete )
				return false;
			else
			{
				if ( failed )
					return true;
			}


			{
				std::string passwordMessage;
				
				passwordMessage.assign("PASS ").append(password_).append("\r\n");

				Debug::trace("Email.POP3", "Sending command: '" + passwordMessage + "'");
				sendCommand(passwordMessage);

				sentPassword_ = true;
			}
		}


		if ( sentPassword_ )
		{
			// Check if we have a response from the server to the PASS command
			bool complete = false;
			switch ( getServerResponse(20) )
			{
				case INCOMPLETE_RESPONSE:
				{
					Sleep (readResponseDelay_);

					break;
				}
				case POSITIVE_RESPONSE:
				{
					fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::AUTHENTICATION_SUCCEEDED_EVENT));
					complete = true;

					break;
				}
				case NEGATIVE_RESPONSE:
				{
					Debug::trace("Email.POP3", "Failed to authenticate using Password method.");

//					if ( ++authenticationMethod_ != authenticationMethods_.end() )
//						fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TRY_NEXT_AUTHENTICATION_EVENT));
//					else
						fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::AUTHENTICATION_FAILED_EVENT));
					complete = true;

					break;
				}
				case TIMEDOUT_RESPONSE:
				{
					Debug::trace("Email.POP3", "Failed to authenticate (timed-out) using Password method.");

					fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TIMEDOUT_EVENT));
					complete = true;

					break;
				}
			}

			return complete;
		}
	}


	bool POP3Client::enteringAuthenticatingDigestState(const POP3ClientEvent& event)
	{
		sentDigest_ = false;

		return true;
	}


	bool POP3Client::processingAuthenticatingDigestState(const POP3ClientEvent& event)
	{
		if ( !sentDigest_ )
		{
			std::string digestMessage;
			std::string digestMD5 = generateMD5Digest(timestamp_, username_);
			
			digestMessage.assign("APOP ").append(mailbox_).append(1, ' ').append(digestMD5).append("\r\n");

			Debug::trace("Email.POP3", "Sending command: '" + digestMessage + "'");
			sendCommand(digestMessage);

			sentDigest_ = true;
		}


		if ( sentDigest_ )
		{
			// Check if we have a response from the server to the APOP command
			bool complete = false;
			switch ( getServerResponse(20) )
			{
				case INCOMPLETE:
				{
					Sleep (readResponseDelay_);

					break;
				}
				case POSITIVE_RESPONSE:
				{
					fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::AUTHENTICATION_SUCCEEDED_EVENT));
					complete = true;

					break;
				}
				case NEGATIVE_RESPONSE:
				{
					Debug::trace("Email.POP3", "Failed to authenticate using Digest method.");

					if ( ++authenticationMethod_ != authenticationMethods_.end() )
						fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TRY_NEXT_AUTHENTICATION_EVENT));
					else
						fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::AUTHENTICATION_FAILED_EVENT));
					complete = true;

					break;
				}
				case TIMEDOUT:
				{
					Debug::trace("Email.POP3", "Failed to authenticate (timed-out) using Digest method.");

					fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TIMEDOUT_EVENT));
					complete = true;

					break;
				}
			}

			return complete;
		}
	}
*/

	bool POP3Client::enteringTransactingState(const POP3ClientEvent& event)
	{
		if ( event.getId() != POP3ClientEvent::NEXT_TRANSACTION_EVENT )
			commandIterator_ = transactionCommands_.begin();
		else
			++commandIterator_;

		waitingForResponse_ = false;

		return true;
	}


	bool POP3Client::processingTransactingState(const POP3ClientEvent&)
	{
		// If there are no more commands to send, move on to the next state
		if ( commandIterator_ == transactionCommands_.end() )
		{
			fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TRANSACTING_SUCCEEDED_EVENT));

			return true;
		}


		if ( !waitingForResponse_ )
		{
			// Assign values to the command's parameters
			switch ( commandIterator_->getType() )
			{
				case POP3Command::UniqueIDList:
				{
					commandIterator_->setParameter(lastEmailNumber_);

					break;
				}
				case POP3Command::Top:
				{
					commandIterator_->setParameter(lastEmailNumber_);
					commandIterator_->setParameter2("0");

					break;
				}
				case POP3Command::Retrieve:
				{
					commandIterator_->setParameter(lastEmailNumber_);

					break;
				}
				case POP3Command::Statistics:
				default:
				{
					// No parameters to set

					break;
				}
			}


			// Send the command to the POP3 server
			const std::string& commandMessage = commandIterator_->serialize();
			if ( !sendCommand(commandMessage) )
			{
				// Failed to send the command to the POP3 server
				Debug::trace("Email.POP3", "Failed to send command '" + commandMessage + "' to POP3 server.");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::COMM_ERROR_EVENT));

				return true;
			}

			waitingForResponse_ = true;
		}


		// Check if we have a response from the server to the APOP command
		bool succeeded = false;
		switch ( getServerResponse() )
		{
			case INCOMPLETE_RESPONSE:
			{
				Sleep (readResponseDelay_);

				return false;
			}
			case POSITIVE_RESPONSE:
			{
				succeeded = true;

				break;
			}
			case NEGATIVE_RESPONSE:
			{
				Debug::trace("Email.POP3", "The POP3 server failed to process transaction command " + commandIterator_->getCommandName() + ".");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TRANSACTION_COMMAND_FAILED_EVENT));

				break;
			}
			case TIMEDOUT_RESPONSE:
			{
				Debug::trace("Email.POP3", "Timed out waiting for response to transaction command " + commandIterator_->getCommandName() + ".");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TIMEDOUT_EVENT));

				break;
			}
		}


		if ( succeeded )
		{
			switch ( commandIterator_->getType() )
			{
				case POP3Command::Statistics:
				{

					break;
				}
			}

			waitingForResponse_ = false;

			fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::NEXT_TRANSACTION_EVENT));

			return true;
		}

		return true;
	}


	bool POP3Client::enteringConcludingState(const POP3ClientEvent&)
	{
		sentQuitCommand_ = false;

		return true;
	}


	bool POP3Client::processingConcludingState(const POP3ClientEvent&)
	{
		if ( !sentQuitCommand_ )
		{
			std::string quitCommand("QUIT");
			
			Debug::trace("Email.POP3", "Sending command: '" + quitCommand + "'");

			if ( !sendCommand(quitCommand) )
			{
				// Failed to send the command to the POP3 server
				Debug::trace("Email.POP3", "Failed to send command '" + quitCommand + "' to POP3 server.");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::COMM_ERROR_EVENT));

				return true;
			}

			sentQuitCommand_ = true;
		}


		// Check if we have a response from the server to the QUIT command
		bool complete = false;
		switch ( getServerResponse() )
		{
			case INCOMPLETE_RESPONSE:
			{
				Sleep (readResponseDelay_);

				break;
			}
			case POSITIVE_RESPONSE:
			{
				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::CONCLUCING_SUCCEEDED_EVENT));
				complete = true;

				break;
			}
			case NEGATIVE_RESPONSE:
			{
				Debug::trace("Email.POP3", "POP3 server reported an error ('" + getSingleLineResponse() + "') when concluding session.");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::CONCLUCING_SUCCEEDED_EVENT));
				complete = true;

				break;
			}
			case MALFORMED_RESPONSE:
			{
				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::CONCLUCING_SUCCEEDED_EVENT));
				complete = true;

				break;
			}
			case COMM_ERROR_RESPONSE:
			{
				Debug::trace("Email.POP3", "Failed to concluded POP3 session.");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::CONCLUCING_FAILED_EVENT));
				complete = true;

				break;
			}
			case TIMEDOUT_RESPONSE:
			{
				Debug::trace("Email.POP3", "Failed to conclude POP3 session (timed-out).");

				fsm_->postEvent(new POP3ClientEvent(POP3ClientEvent::TIMEDOUT_EVENT));
				complete = true;

				break;
			}
		}

		return complete;
	}


	void POP3Client::processGreeting(const std::string& greeting)
	{
		// Determine whether there is a timestamp in the greeting
		std::string::size_type i = 0, length = greeting.size();
		while ( i < length && greeting[i] != '<' )
			++i;

		if ( i < length )
		{
			std::string::size_type begin = i + 1;
			do ++i;
			while ( i < length && greeting[i] != '>' );

			if ( i < length )
				receivedTimestamp(greeting.substr(begin, i - begin));
		}
	}


	bool POP3Client::receivedTimestamp(const std::string& timestamp)
	{
		// Validate the timestamp
		std::string::size_type pos = timestamp.find('@');
		if ( pos != std::string::npos && pos < timestamp.size() - 1 && timestamp.find('@', pos + 1) == std::string::npos )
		{
			serverTimestamp_ = timestamp;

			// If the POP3 server supplied a timestamp is because it supports the APOP command and password digest authentication
			availableCommands_.push_back(POP3Command(POP3Command::Digest));
			authenticationMethods_.push_front(POP3AuthenticationMethod::createMethod(POP3AuthenticationMethod::PasswordDigest));

			return true;
		}

		return false;
	}


	void POP3Client::processCapability(responseContainer_type response)
	{
		responseContainer_iterator iter = response.begin();

		// Skip the first response line that only has the response code
		while ( ++iter != response.end() )
		{
			bool found = false;

			// Extract the capability name
			std::string capability = *iter;
			std::string::size_type separator = capability.find(' ');
			if ( separator != std::string::npos )
				capability.erase(separator);

			// Identify the capability
			for ( capabilityContainer_iterator capabilityIter = allCapabilities_.begin(); !found && capabilityIter != allCapabilities_.end(); ++capabilityIter )
			{
				if ( !iter->compare(0, capabilityIter->getCapabilityName().size(), capabilityIter->getCapabilityName()) )
				{
					switch ( capabilityIter->getType() )
					{
						case POP3Capability::Top:
						{
							availableCommands_.push_back(POP3Command(POP3Command::Top));

							found = true;
							break;
						}
						case POP3Capability::User:
						{
							availableCommands_.push_back(POP3Command(POP3Command::User));
							availableCommands_.push_back(POP3Command(POP3Command::Password));

							authenticationMethods_.push_back(POP3AuthenticationMethod::createMethod(POP3AuthenticationMethod::PlainPassword));

							found = true;
							break;
						}
						case POP3Capability::Authentication:
						{
							bool foundOne = false;
							POP3Parser parser(iter->substr(separator));

							for ( POP3Parser::iterator token = parser.begin(); token != parser.end(); ++token )
							{
								if ( *token == "KERBEROS_V4" )
								{
									authenticationMethods_.push_back(
											POP3AuthenticationMethod::createMethod(POP3AuthenticationMethod::KERBEROS));
									foundOne = true;
								}
								else if ( *token == "GSSAPI" )
								{
									authenticationMethods_.push_back(
											POP3AuthenticationMethod::createMethod(POP3AuthenticationMethod::GSSAPI));
									foundOne = true;
								}
								else if ( *token == "SKEY" )
								{
									authenticationMethods_.push_back(
											POP3AuthenticationMethod::createMethod(POP3AuthenticationMethod::SKEY));
									foundOne = true;
								}
								else if ( *token == "DIGEST-MD5" )
								{
									authenticationMethods_.push_back(
											POP3AuthenticationMethod::createMethod(POP3AuthenticationMethod::DIGEST_MD5));
									foundOne = true;
								}
								else if ( *token == "ANONYMOUS" )
								{
									authenticationMethods_.push_back(
											POP3AuthenticationMethod::createMethod(POP3AuthenticationMethod::ANONYMOUS));
									foundOne = true;
								}
								else if ( *token == "PLAIN" )
								{
									authenticationMethods_.push_back(
											POP3AuthenticationMethod::createMethod(POP3AuthenticationMethod::PLAIN_SASL));
									foundOne = true;
								}
							}

							if ( foundOne )
								availableCommands_.push_back(POP3Command(POP3Command::Authentication));

							found = true;
							break;
						}
						case POP3Capability::ResponseCodes:
						{
							responseCodes_ = true;

							found = true;
							break;
						}
						case POP3Capability::LoginDelay:
						{
							found = true;
							break;
						}
						case POP3Capability::Pipelining:
						{
							found = true;
							break;
						}
						case POP3Capability::Expire:
						{
							found = true;
							break;
						}
						case POP3Capability::UniqueIDList:
						{
							availableCommands_.push_back(POP3Command(POP3Command::UniqueIDList));

							found = true;
							break;
						}
						case POP3Capability::SecureTransport:
						{
							availableCommands_.push_back(POP3Command(POP3Command::SecureTransport));

							found = true;
							break;
						}
						case POP3Capability::Implementation:
						{
							Debug::trace("Email.POP3", "POP3 Server Implementation '" + iter->substr(separator) + "'");

							found = true;
							break;
						}
					}

					break;
				}
			}

			if ( !found )
			{
				Debug::trace("Email.POP3", "Capability '" + capability + "' not supported.");
			}
		}
	}


	void POP3Client::initializeServerResponse()
	{
		if ( responseBuffer_.size() )
		{
			std::string content;

			responseBuffer_.extract(content, responseBuffer_.end(), false);

			Debug::trace("POP3", "Initializing server response buffer while it still had data in it ('" + content + "').");
		}
		responseBuffer_.clear();
		rbCurrent_ = responseBuffer_.begin();
		responseReferenceTime_.getCurrentDateTime();
		responseAnalyzed_ = false;

		currentResponse_.clear();
		currentResponseCode_.clear();
	}


	POP3Client::ServerResponse POP3Client::getServerResponse(bool multiline)
	{
		// Clear the current response if it was already analyzed
		if ( responseAnalyzed_ )
			initializeServerResponse();


		// Let's read more data
		if ( socket_->cmNumMsgs() )
		{
			buffer_type::size_type available = responseBuffer_.getAvailableSpace();
			int bytesRead = socket_->receive(responseBuffer_.getAppendPointer(available), available);

			if ( bytesRead == ERROR_CODE )
			{
				Debug::trace("Email.POP3", "COMM_ERROR_RESPONSE: received a socket error (turn on 'Socket' debugging).");

				responseAnalyzed_ = true;
				return COMM_ERROR_RESPONSE;
			}
			else
				responseBuffer_.appendedQuantity(bytesRead);
		}


		// If we have any unprocessed data in the buffer, process it
		while ( rbCurrent_ != responseBuffer_.end() )
		{
			// Check whether there is a line separator in the buffer (bearing in mind that the CR may have appeared
			// at the end of the previous buffer and the LF may be the at the beginning of this one).
			buffer_iterator iter = rbCurrent_;
			bool possibleLineBreak = (responseBuffer_.begin() != rbCurrent_ && *(rbCurrent_ - 1) == '\r');
			for ( ; rbCurrent_ != responseBuffer_.end(); ++rbCurrent_ )
			{
				if ( possibleLineBreak )
				{
					if ( *rbCurrent_ == '\n' )
					{
						std::string newLine;
						responseBuffer_.extract(newLine, rbCurrent_ - 1);    // extract up to the CR and LF
						responseBuffer_.discardQuantity(2);                  // skip over the CR and LF as well

						if ( multiline )
						{
							if ( newLine.size() == 1 && newLine[0] == '.' )
							{
								// We just read indication that the multi-line response is complete. Process it
								return analyzeResponse();
							}
							else
							{
								// We just read an additional line in a multi-line response. Store it and continue
								currentResponse_.push_back(newLine);
								possibleLineBreak = false;
							}
						}
						else
						{
							// We just the single line response. Process it
							currentResponse_.push_back(newLine);
							return analyzeResponse();
						}
					}
					else
					{
						if ( *rbCurrent_ != '\r' )
							possibleLineBreak = false;
					}
				}
				else
				{
					if ( *rbCurrent_ == '\r' )
						possibleLineBreak = true;
				}
			}
		}


		// Since we did not receive a full response check for timed out
		if ( (DateTime() - responseReferenceTime_).getTotalSeconds() >= maxResponseTime_ )
		{
			std::string partialLine;
			responseBuffer_.extract(partialLine, responseBuffer_.end(), false);

			Debug::trace("Email.POP3", "Did not receive full/valid response from POP3 server within " +
				util_string::inttostring(maxResponseTime_) + " seconds (only received '" + partialLine + "').");

			responseAnalyzed_ = true;
			return TIMEDOUT_RESPONSE;
		}


		return INCOMPLETE_RESPONSE;
	}


	POP3Client::ServerResponse POP3Client::analyzeResponse()
	{
		std::string::size_type length = currentResponse_.size() ? currentResponse_[0].size() : 0;
		currentResponseCode_.clear();
		responseAnalyzed_ = true;


		// Check for positive or negative response
		if ( length )
		{
			ServerResponse response = MALFORMED_RESPONSE;
			std::string& firstLine = currentResponse_[0];

			if ( firstLine[0] == '+' )
			{
				if ( length >= 3 && firstLine.compare(1, 2, "OK") == 0 )
				{
					std::string::size_type i = 3;

					while ( i < length && firstLine[i] == ' ' )
						++i;
					
					firstLine.erase(0, i);

					Debug::trace("Email.POP3", "Received a POSITIVE response from POP3 server ('" + firstLine + "').");

					response = POSITIVE_RESPONSE;
				}
				else
					Debug::trace("Email.POP3", "Received a MALFORMED positive response from POP3 server ('" + firstLine + "').");
			}
			else if ( firstLine[0] == '-' )
			{
				if ( firstLine.compare(1, 3, "ERR") == 0 )
				{
					std::string::size_type i = 4;

					while ( i < length && firstLine[i] == ' ' )
						++i;
					
					firstLine.erase(0, i);

					Debug::trace("Email.POP3", "Received a NEGATIVE response from POP3 server ('" + firstLine + "').");

					response = NEGATIVE_RESPONSE;
				}
				else
					Debug::trace("Email.POP3", "Received a MALFORMED negative response from POP3 server ('" + firstLine + "').");
			}
			else
				Debug::trace("Email.POP3", "Received a MALFORMED response from POP3 server ('" + firstLine + "').");

			if ( response == MALFORMED_RESPONSE )
				return response;



			// Check for response codes and extract them if present
			length = firstLine.size();
			if ( length )
			{
				if ( firstLine[0] == '[' )
				{
					std::string::size_type i = 1, begin = 1;
					while ( i < length )
					{
						if ( firstLine[i] == '/' )
						{
							currentResponseCode_.push_back(firstLine.substr(begin, i - begin));
							begin = i + 1;
						}
						else if ( firstLine[i] == ']' )
						{
							currentResponseCode_.push_back(firstLine.substr(begin, i - begin));
							break;
						}

						++i;
					}

					if ( i < length )
					{
						do ++i;
						while ( i < length && firstLine[i] == ' ' );
						
						firstLine.erase(0, i);
					}
					else
					{
						Debug::trace("Email.POP3", "Received a MALFORMED response from POP3 server with incomplete response code ('" +
								firstLine + "').");

						currentResponseCode_.clear();

						response = MALFORMED_RESPONSE;
					}
				}
			}


			return response;
		}
		else
		{
			Debug::trace("Email.POP3", "Received a MALFORMED response from POP3 server with an empty first line.");

			return MALFORMED_RESPONSE;
		}
	}



	bool POP3Client::sendCommand(const std::string& message)
	{
		Debug::trace("Email.POP3", "Sending command '" + message + "' to POP3 server.");

		return socket_->cmSend(message.c_str(), (int)message.size()) == (int)message.size();
	}


	void POP3Client::setupStateMachine()
	{
		// Add states
		fsm_->addState(INITIALIZING_STATE, &POP3Client::processingInitializingState);
		fsm_->addState(CONNECTING_STATE, &POP3Client::processingConnectingState, &POP3Client::enteringConnectingState);
		fsm_->addState(CAPABILITY_STATE, &POP3Client::processingCapabilityState, &POP3Client::enteringCapabilityState);
		fsm_->addState(AUTHENTICATING_STATE, &POP3Client::processingAuthenticatingState, &POP3Client::enteringAuthenticatingState);
		fsm_->addState(TRANSACTING_STATE, &POP3Client::processingTransactingState, &POP3Client::enteringTransactingState);
		fsm_->addState(CONCLUDING_STATE, &POP3Client::processingConcludingState, &POP3Client::enteringConcludingState);
		fsm_->addState(DISCONNECTING_STATE, &POP3Client::processingDisconnectingState);
		fsm_->addState(ERROR_STATE, &POP3Client::processingErrorState);

		// Add INITIALIZING_STATE transitions
		fsm_->addTransition(INITIALIZING_STATE, POP3ClientEvent::INITIALIZED_EVENT, CONNECTING_STATE);

		// Add CONNECTING_STATE transitions
		fsm_->addTransition(CONNECTING_STATE, POP3ClientEvent::CONNECTING_SUCCESS_EVENT, CAPABILITY_STATE);

		// Add CAPABILITY_STATE transitions
		fsm_->addTransition(CAPABILITY_STATE, POP3ClientEvent::FIRST_CAPABILITY_SUCCEEDED_EVENT, AUTHENTICATING_STATE);
		fsm_->addTransition(CAPABILITY_STATE, POP3ClientEvent::FIRST_CAPABILITY_FAILED_EVENT, AUTHENTICATING_STATE);
		fsm_->addTransition(CAPABILITY_STATE, POP3ClientEvent::SECOND_CAPABILITY_SUCCEEDED_EVENT, TRANSACTING_STATE);
		fsm_->addTransition(CAPABILITY_STATE, POP3ClientEvent::SECOND_CAPABILITY_FAILED_EVENT, TRANSACTING_STATE);

		// Add AUTHENTICATING_STATE transitions
		fsm_->addTransition(AUTHENTICATING_STATE, POP3ClientEvent::AUTHENTICATION_SUCCEEDED_EVENT, TRANSACTING_STATE);

		// Add TRANSACTING_STATE transitions
		fsm_->addTransition(TRANSACTING_STATE, POP3ClientEvent::TRANSACTING_SUCCEEDED_EVENT, CONCLUDING_STATE);
		fsm_->addTransition(TRANSACTING_STATE, POP3ClientEvent::NEXT_TRANSACTION_EVENT, TRANSACTING_STATE);

		// Add UPDATING_STATE transitions
		fsm_->addTransition(CONCLUDING_STATE, POP3ClientEvent::CONCLUCING_SUCCEEDED_EVENT, DISCONNECTING_STATE);

		// Add DISCONNECTING_STATE transitions
		fsm_->addTransition(DISCONNECTING_STATE, POP3ClientEvent::END_EVENT);
	}


}
