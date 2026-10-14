#include "PAL_platformheader.h"

#include "CommSession.h"


#include "CommSessionErrorEvent.h"


#include "Configuration.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "ProductDevEvent.h"
#include "COOLDevEvent.h"
#include "SoftkeyDevEvent.h"
#include "Messages/SynchronizationDevEvent.h"

#include "Messages/DisableInputProcessingMessage.h"


#include "HeaderRecord.h"
#include "FooterRecord.h"


#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace comm
{


#ifdef USE_ACCOUNTANT
	int CommSession::_dataType_ = 0;
	string CommSession::_dataTypeName_("comm::CommSession");
#endif


	const std::string CommSession::_statenameInitializing_                     ("INITIALIZING");
	const std::string CommSession::_statenameReadingHeader_                    ("READING HEADER");
	const std::string CommSession::_statenameProcessingHeader_                 ("PROCESSING HEADER");
	const std::string CommSession::_statenameSendingHeader_                    ("SENDING HEADER");
	const std::string CommSession::_statenameReadingCommand_                   ("READING COMMAND");
	const std::string CommSession::_statenameProcessingCommand_                ("PROCESSING COMMAND");
	const std::string CommSession::_statenameProcessingReadAllInitSubstate_    ("PROCESSING READ ALL INIT");
	const std::string CommSession::_statenameProcessingReadAllReadSubstate_    ("PROCESSING READ ALL READ");
	const std::string CommSession::_statenameProcessingReadAllWriteSubstate_   ("PROCESSING READ ALL WRITE");
	const std::string CommSession::_statenameProcessingReadAllDoneSubstate_    ("PROCESSING READ ALL DONE");
	const std::string CommSession::_statenameProcessingDeleteAllInitSubstate_  ("PROCESSING DELETE ALL INIT");
	const std::string CommSession::_statenameProcessingDeleteAllReadSubstate_  ("PROCESSING DELETE ALL READ");
	const std::string CommSession::_statenameProcessingDeleteAllDeleteSubstate_("PROCESSING DELETE ALL DELETE");
	const std::string CommSession::_statenameProcessingDeleteAllDoneSubstate_  ("PROCESSING DELETE ALL DONE");
	const std::string CommSession::_statenameProcessingFooter_                 ("PROCESSING FOOTER");
	const std::string CommSession::_statenameSendingFooter_                    ("SENDING FOOTER");
	const std::string CommSession::_statenameError_                            ("ERROR");
	const std::string CommSession::_statenameDisconnecting_                    ("DISCONNECTING");
	const std::string CommSession::_statenameConcluding_                       ("CONCLUDING");
	const std::string CommSession::_statenameUnknown_                          ("UNKNOWN");


	CommSession::CommSession(CommMechanism * uiClient, const char* name, bool inputEnabled, int reason)
		: AbstractSession(uiClient, name ? name : "CommSession", false,  true, false), pausingProcessing_(false), acceptingInput_(true), textId_(0)
	{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif


		Debug::trace("CommSession", "Constructed new CommSession @" + util_string::ptrtostring(this) +
			"(Input " + (inputEnabled ? "enabled" : "disabled"));

		if ( !inputEnabled )
			pauseProcessing(reason);

		
		
		RestartRunThread();
	}


	CommSession::~CommSession()
	{
		Debug::trace("CommSession", util_string::ptrtostring(this) + " id = " + getName() + " - ~CommSession destructor");

		PAL_absclass::PAL_sleep(100);

	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
	#endif

		if (sessionThread_)
		{
			sessionThread_->StopThread();
			sessionThread_->WaitForCompletion();
		}
	}


	void CommSession::initialize()
	{
		fsm_ = new StateMachine<CommSession>(*this); 
		setupStateMachine(); 
		fsm_->setNotifyTopOfLoop();
	}


	bool CommSession::atTopOfLoop()
	{
		/* With this call we tell the SessionManager that it cannot proceed with a pause until we
		reach the bottom of the loop. */
		UINT32 result = SessionManager::getInstance()->requestToContinue(GetThreadID(), IsASuperThread());
		switch ( result & (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED) )
		{
			case SessionManager::COMMON_BLOCKED:
			{
				/* SessionManager is in the process of Changing Synchronization (uc1.10 or uc1.11).
				From this iteration and on, the Synchronization mechanism may not be available, so
				stop processing communications. */
				pauseProcessing(0);

				break;
			}
			case SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED:
			{
				util_time::Time t0;

				HFCManagerContainer::init();
				DBAccess::uninitDB();
				DBAccess::initDB();

				util_time::Time t1;
				Debug::trace("CommSession", "After compressing DB, its reinitialization took " +
					util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " msecs.");

				break;
			}
			default:
			{
				break;
			}
		}

		return true; 
	}


	/* "Use Case 4.22: Processing Primary/Secondary communications". */
	DWORD CommSession::Run()
	{
		//make sure everything is up before proceeding!!!		
		PAL_absclass::PAL_sleep(100);		
		
		try
		{
			initialize();

			fsm_->start(new CommSessionEvent(CommSessionEvent::START_EVENT), INITIALIZING_STATE);
		                      
			fsm_->runStateMachine();

			if ( (*fsm_)() != stateMachine_type::STOPPED_STATE )
			{
				Debug::trace("CommSession", "StateMachine stopped running unexpectedly (" +
						fsm_->getStateName() + "). Last state is " + getStateName(fsm_->getCurrentState()) +
						" and last event is " + util_string::inttostring(fsm_->getCurrentEvent() ? fsm_->getCurrentEvent()->getId() : -1));
			}
		}
		catch ( const edb::DBException& dbe )
		{
			showDatabaseException("CommSession", dbe);
		}
		catch ( ... )
		{
			Debug::trace("CommSession", "******* Got un unexpected unknown exception in Run thread ******");
		}


		/* (uc4.22) Close the connection. */
		terminate();


		/* Tell the SessionManager to remove this session. */
		Debug::trace("CommSession", getName() + " - Notifying SessionManager to remove me ...");

		EvolMessage e(SessionDev, SnTerminated, this);
		dispatchEvent(&e);


		return 0;
	}


	void CommSession::resumeProcessing()
	{
		/* We do not resume processing once we paused it. */
	}


	void CommSession::pauseProcessing(int reason)
	{
		/* (uc4.9) Store the reference to the text string passed in. */
		/* (TODO) may need to synchronize access to textId_ */
		textId_ = reason;


		/* (uc4.9) Clear the Input processing enabled flag. */
		setAcceptingInput(false);


		/* Synchronize with the top of the loop to ensure that next processing is happening with the
		knowledge that we are pausing processing. */

		pausingProcessing_ = true;


	}


	bool CommSession::processingInitializingState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Initializing State: Started processing");


		/* First the initialization at this level. */
		timeoutReadingHeader_ = Configuration::getInstance()->toIntCfgI(CfgInternal::CONNECTIONIDENTIFICATIONTIMER);
		timeoutReadingMessage_ = Configuration::getInstance()->toIntCfgI(CfgInternal::COMMSESSIONREADMESSAGETIMER);
		readingMessageDelay_ = Configuration::getInstance()->toIntCfgI(CfgInternal::COMMSESSIONREADMESSAGEDELAY);
		/* This will be used to timeout when reading the communication header. */
		startedReadingMessageTime_ = util_time::Time();
		sentBusyError_ = false;


		/* Now the initialization at other levels. */
		if ( doInitialization() )
		{
			Debug::trace("CommSession", "Initializing State: completed successfully.");

			fsm_->postEvent(new CommSessionEvent(CommSessionEvent::INITIALIZED_EVENT));

			return true;
		}

		Debug::trace("CommSession", "Initializing State: not completed yet (try again).");

		return false;
	}


	bool CommSession::processingReadingHeaderState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Reading Header State: Started processing");

		/* (uc4.22) Attempt to read the communication header sent by the other scale.
		On failure, then end the execution of this use case after closing the connection. */
		Util::CSmartPtr<SessionMessage> message = readMessage();

		if ( message.notNull() )
		{
			if ( message->getDeviceType() == SessionDev && message->getEventType() == SnCommStart )
			{
				receivedHeader_ = dynamic_cast<hfc::HeaderRecord*>(message->getHfcObject());
				message->setHfcObjectCreatedInternally(false);

				Debug::trace("CommSession", "Reading Header State: completed successfully.");

				fsm_->postEvent(new event_type(event_type::HEADER_RECEIVED_EVENT));

				return true;
			}

			// We did not receive a header when expecting it (???). Conclude.
			Debug::trace("CommSession", "Expected a header but received a message " +
					util_string::inttostring(message->getDeviceType()) + "," +
					util_string::inttostring(message->getEventType()) + ": " + message->getOriginalBuff());

			Debug::trace("CommSession", "Reading Header State: failed.");

			fsm_->postEvent(new CommSessionErrorEvent(CommSessionEvent::COMMERROR_DIDNOT_RECEIVE_HEADER));

			return true;
		}
		else
		{
			/* We should not timeout when reading the header since SessionStarter already read the
			header to identify the type of session to use for the socket connection. */
			util_time::Time time;
			if ( (time - startedReadingMessageTime_).getTotalSeconds() > timeoutReadingHeader_ )
			{
				Debug::trace("CommSession", "Reading Header State: failed.");

				fsm_->postEvent(new CommSessionErrorEvent(CommSessionEvent::COMMERROR_TIMEDOUT_READING_HEADER));

				return true;
			}
			else
				return false;
		}
	}


	bool CommSession::processingHeaderState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Processing Header State: Started processing");


		doProcessHeader(receivedHeader_);


		Debug::trace("CommSession", "Processing Header State: completed successfully.");

		fsm_->postEvent(new event_type(event_type::HEADER_PROCESSED_EVENT));

		return true;
	}


	bool CommSession::processingSendingHeaderState(const CommSessionEvent& /*event*/)
	{
		CommSessionEvent::CommError returnedError;

		Debug::trace("CommSession", "Sending Header State: Started processing");


		if ( doSendHeader(returnedError) )
		{
			sendExistingErrors();

			if ( returnedError == CommSessionEvent::COMMERROR_NO_ERROR )
			{
				Debug::trace("CommSession", "Sending Header State: completed successfully.");

				fsm_->postEvent(new event_type(event_type::HEADER_SENT_EVENT));
			}
			else
			{
				Debug::trace("CommSession", "Sending Header State: failed with error " +
					util_string::inttostring(returnedError));

				fsm_->postEvent(new CommSessionErrorEvent(returnedError));
			}

			return true;
		}


		Debug::trace("CommSession", "Sending Header State: not completed yet (try again).");

		return false;
	}


	bool CommSession::enteringReadingCommandState(const CommSessionEvent& /*event*/)
	{
		startedReadingMessageTime_ = util_time::Time();

		return true;
	}


	bool CommSession::processingReadingCommandState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Reading Command State: Started processing");


		Util::CSmartPtr<SessionMessage> message = readMessage();
		if ( message.notNull() )
		{
			if ( message->getDeviceType() == SessionDev && message->getEventType() == SnCommEnd )
			{
				receivedFooter_ = message;

				Debug::trace("CommSession", "Reading Command State: completed successfully (read footer).");

				fsm_->postEvent(new event_type(event_type::FOOTER_RECEIVED_EVENT));

				return true;
			}
			else
			{
				receivedCommand_ = message;

				Debug::trace("CommSession", "Reading Command State: completed successfully.");

				if ( isReadAllMessage(message->getDeviceType(), message->getEventType()) )
					fsm_->postEvent(new event_type(event_type::READ_ALL_RECEIVED_EVENT));
				else if ( isDeleteAllMessage(message->getDeviceType(), message->getEventType()) )
					fsm_->postEvent(new event_type(event_type::DELETE_ALL_RECEIVED_EVENT));
				else
					fsm_->postEvent(new event_type(event_type::COMMAND_RECEIVED_EVENT));

				return true;
			}
		}
		else
		{
			util_time::Time t1;
			if ( (t1 - startedReadingMessageTime_).getTotalSeconds() > timeoutReadingMessage_ )
			{
				Debug::trace("CommSession", "Reading Command State: failed.");

				fsm_->postEvent(new CommSessionErrorEvent(CommSessionEvent::COMMERROR_TIMEDOUT_READING_COMMAND));

				return true;
			}
			else
			{
				Debug::trace("CommSession", "Reading Command State: not completed yet (try again).");

            	PAL_absclass::PAL_sleep(readingMessageDelay_);

				return false;
			}
		}
	}


	bool CommSession::processingCommandState(const CommSessionEvent& /*event*/)
	{
		bool nextState = true;

		Debug::trace("CommSession", "Processing Command State: Started processing");


		if ( isAcceptingInput() )
		{
			if ( doProcessCommand(receivedCommand_) )
			{
				Debug::trace("CommSession", "Processing Command State: completed successfully.");

				fsm_->postEvent(new event_type(event_type::COMMAND_PROCESSED_EVENT));
			}
			else
			{
				Debug::trace("CommSession", "Processing Command State: not completed yet (try again).");

				nextState = false;
			}
		}
		else
		{
			addBusyError();

			/* If we do not allow input then any command/record received (other than the footer) is ignored. */
			Debug::trace("CommSession", "Processing Command State: Command (" + util_string::inttostring(receivedCommand_->getDeviceType()) +
				", " + util_string::inttostring(receivedCommand_->getEventType()) + ") ignored since we are not accepting input.");

			fsm_->postEvent(new event_type(event_type::COMMAND_PROCESSED_EVENT));
		}


		sendExistingErrors();

		return nextState;
	}


	bool CommSession::processingReadAllInitSubState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Processing Read All Command State, Init Substate: Started processing");


		if ( isAcceptingInput() )
		{
			if ( doReadAllInit() )
			{
				Debug::trace("CommSession", "Processing Read All Command State, Init Substate: completed successfully.");

				fsm_->postEvent(new event_type(event_type::READ_ALL_INIT_COMPLETED_EVENT));
			}
			else
			{
				Debug::trace("CommSession", "Processing Read All Command State, Init Substate: completed successfully (nothing to do).");

				fsm_->postEvent(new event_type(event_type::READ_ALL_DONE_EVENT));
			}
		}
		else
		{
			addBusyError();

			/* If we do not allow input then any command/record received (other than the footer) is ignored. */
			Debug::trace("CommSession", "Processing Read All Command State, Init Substate: Command (" + util_string::inttostring(receivedCommand_->getDeviceType()) +
				", " + util_string::inttostring(receivedCommand_->getEventType()) + ") ignored since we are not accepting input.");

			fsm_->postEvent(new event_type(event_type::READ_ALL_DONE_EVENT));
		}


		sendExistingErrors();

		return true;
	}


	bool CommSession::processingReadAllReadSubState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Processing Read All Command State, Read Substate: Started processing");

		if ( isAcceptingInput() )
		{
			if ( doReadAllRead() )
			{
				Debug::trace("CommSession", "Processing Read All Command State, Read Substate: completed successfully.");

				fsm_->postEvent(new event_type(event_type::READ_ALL_RECORD_READ_EVENT));

				return true;
			}
			else
			{
				Debug::trace("CommSession", "Processing Read All Command State, Read Substate: completed successfully.");

				fsm_->postEvent(new event_type(event_type::READ_ALL_DONE_EVENT));

				return true;
			}
		}
		else
		{
			addBusyError();
			sendExistingErrors();

			/* If we do not allow input then any command/record received (other than the footer) is ignored. */
			Debug::trace("CommSession", "Processing Read All Command State, Read Substate: skipped since we are not accepting input.");

			fsm_->postEvent(new event_type(event_type::READ_ALL_DONE_EVENT));

			return true;
		}
	}


	bool CommSession::processingReadAllWriteSubState(const CommSessionEvent& /*event*/)
	{
		bool nextState = true;
		CommSessionEvent::CommError returnedError;

		Debug::trace("CommSession", "Processing Read All Command State, Write Substate: Started processing");


		if ( isAcceptingInput() )
		{
			if ( doReadAllWrite(returnedError) )
			{
				if ( returnedError == CommSessionEvent::COMMERROR_NO_ERROR )
				{
					Debug::trace("CommSession", "Processing Read All Command State, Write Substate: completed successfully.");

					fsm_->postEvent(new event_type(event_type::READ_ALL_RECORD_WRITTEN_EVENT));
				}
				else
				{
					Debug::trace("CommSession", "Processing Read All Command State, Write Substate: failed with error " +
						util_string::inttostring(returnedError));

					fsm_->postEvent(new CommSessionErrorEvent(returnedError));
				}
			}
			else
			{
				Debug::trace("CommSession", "Processing Read All Command State, Write Substate: not completed yet (try again).");

				nextState = false;
			}
		}
		else
		{
			addBusyError();

			/* If we do not allow input then any command/record received (other than the footer) is ignored. */
			Debug::trace("CommSession", "Processing Read All Command State, Write Substate: skipped since we are not accepting input.");

			fsm_->postEvent(new event_type(event_type::READ_ALL_RECORD_WRITTEN_EVENT));
		}


		sendExistingErrors();

		return nextState;
	}


	bool CommSession::processingReadAllDoneSubState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Processing Read All Command State, Done Substate: Started processing");

		if ( isAcceptingInput() )
		{
			if ( doReadAllDone() )
			{
				Debug::trace("CommSession", "Processing Read All Command State, Done Substate: completed successfully.");

				fsm_->postEvent(new event_type(event_type::READ_ALL_COMPLETED_EVENT));

				return true;
			}

			Debug::trace("CommSession", "Processing Read All Command State, Done Substate: not completed yet (try again).");

			return false;
		}
		else
		{
			addBusyError();
			sendExistingErrors();

			/* If we do not allow input then any command/record received (other than the footer) is ignored. */
			Debug::trace("CommSession", "Processing Read All Command State, Done Substate: skipped since we are not accepting input.");

			fsm_->postEvent(new event_type(event_type::READ_ALL_COMPLETED_EVENT));

			return true;
		}
	}


	bool CommSession::processingDeleteAllInitSubState(const CommSessionEvent& /*event*/)
	{
		fsm_->postEvent(new event_type(event_type::DELETE_ALL_DONE_EVENT));

		return true;
	}


	bool CommSession::processingDeleteAllReadSubState(const CommSessionEvent& /*event*/)
	{
		return true;
	}


	bool CommSession::processingDeleteAllDeleteSubState(const CommSessionEvent& /*event*/)
	{
		return true;
	}


	bool CommSession::processingDeleteAllDoneSubState(const CommSessionEvent& /*event*/)
	{
		fsm_->postEvent(new event_type(event_type::DELETE_ALL_COMPLETED_EVENT));

		return true;
	}


	bool CommSession::processingFooterState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Processing Footer State: Started processing");

		if ( doProcessFooter() )
		{
			Debug::trace("CommSession", "Processing Footer State: completed successfully.");

			fsm_->postEvent(new event_type(event_type::FOOTER_PROCESSED_EVENT));

			return true;
		}

		Debug::trace("CommSession", "Processing Footer State: not completed yet (try again).");

		return false;
	}


	bool CommSession::processingSendingFooterState(const CommSessionEvent& /*event*/)
	{
		CommSessionEvent::CommError returnedError;

		Debug::trace("CommSession", "Sending Footer State: Started processing");

		sendExistingErrors();

		if ( doSendFooter(returnedError) )
		{
			if ( returnedError == CommSessionEvent::COMMERROR_NO_ERROR )
			{
				Debug::trace("CommSession", "Sending Footer State: completed successfully.");

				fsm_->postEvent(new event_type(event_type::FOOTER_SENT_EVENT));
			}
			else
			{
				Debug::trace("CommSession", "Sending Footer State: failed.");

				fsm_->postEvent(new CommSessionErrorEvent(returnedError));
			}

			return true;
		}

		Debug::trace("CommSession", "Processing Sending Footer State: not completed yet (try again).");

		return false;
	}


	bool CommSession::processingErrorState(const CommSessionEvent& event)
	{
		Debug::trace("CommSession", "Error State: Started processing");


		const CommSessionErrorEvent& error = (const CommSessionErrorEvent&)event;

		doProcessError(error.getError());

		switch ( error.getError() )
		{
			case CommSessionEvent::COMMERROR_CANNOT_CREATE_SOCKET:
			{
				break;
			}
			case CommSessionEvent::COMMERROR_CANNOT_CONNECT:
			{
				break;
			}
			case CommSessionEvent::COMMERROR_READING_FROM_SOCKET:
			{
				break;
			}
			case CommSessionEvent::COMMERROR_WRITING_TO_SOCKET:
			{
				break;
			}
			case CommSessionEvent::COMMERROR_DIDNOT_RECEIVE_HEADER:
			{
				break;
			}
			default:
			{
				break;
			}
		}


		Debug::trace("CommSession", "Error State: completed successfully.");

		fsm_->postEvent(new event_type(event_type::ERROR_PROCESSED_DISCONNECT_EVENT));

		return true;
	}


	bool CommSession::processingDisconnectingState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Disconnecting State: Started processing");

		Debug::trace("CommSession", "Disconnecting State: completed successfully.");

		fsm_->postEvent(new event_type(event_type::DISCONNECTED_EVENT));

		return true;
	}


	bool CommSession::processingConcludingState(const CommSessionEvent& /*event*/)
	{
		Debug::trace("CommSession", "Concluding State: Started processing");

		if ( doConclusion() )
		{
			Debug::trace("CommSession", "Concluding State: completed successfully.");

			fsm_->postEvent(new event_type(event_type::END_EVENT));

			return true;
		}

		Debug::trace("CommSession", "Processing Command State: not completed yet (try again).");

		return false;
	}


	void CommSession::setupStateMachine()
	{
		// Add states
		fsm_->addState(INITIALIZING_STATE,                      &CommSession::processingInitializingState);
		fsm_->addState(READING_HEADER_STATE,                    &CommSession::processingReadingHeaderState);
		fsm_->addState(PROCESSING_HEADER_STATE,                 &CommSession::processingHeaderState);
		fsm_->addState(SENDING_HEADER_STATE,                    &CommSession::processingSendingHeaderState);
		fsm_->addState(READING_COMMAND_STATE,                   &CommSession::processingReadingCommandState,       &CommSession::enteringReadingCommandState);
		fsm_->addState(PROCESSING_COMMAND_STATE,                &CommSession::processingCommandState);
		fsm_->addState(PROCESSING_READ_ALL_INIT_SUBSTATE,       &CommSession::processingReadAllInitSubState);
		fsm_->addState(PROCESSING_READ_ALL_READ_SUBSTATE,       &CommSession::processingReadAllReadSubState);
		fsm_->addState(PROCESSING_READ_ALL_WRITE_SUBSTATE,      &CommSession::processingReadAllWriteSubState);
		fsm_->addState(PROCESSING_READ_ALL_DONE_SUBSTATE,       &CommSession::processingReadAllDoneSubState);
		fsm_->addState(PROCESSING_DELETE_ALL_INIT_SUBSTATE,     &CommSession::processingDeleteAllInitSubState);
		fsm_->addState(PROCESSING_DELETE_ALL_READ_SUBSTATE,		&CommSession::processingDeleteAllReadSubState);
		fsm_->addState(PROCESSING_DELETE_ALL_DELETE_SUBSTATE,   &CommSession::processingDeleteAllDeleteSubState);
		fsm_->addState(PROCESSING_DELETE_ALL_DONE_SUBSTATE,     &CommSession::processingDeleteAllDoneSubState);
		fsm_->addState(PROCESSING_FOOTER_STATE,                 &CommSession::processingFooterState);
		fsm_->addState(SENDING_FOOTER_STATE,                    &CommSession::processingSendingFooterState);
		fsm_->addState(ERROR_STATE,                             &CommSession::processingErrorState);
		fsm_->addState(DISCONNECTING_STATE,                     &CommSession::processingDisconnectingState);
		fsm_->addState(CONCLUDING_STATE,                        &CommSession::processingConcludingState);

		// Add INITIALIZING_STATE transitions
		fsm_->addTransition(INITIALIZING_STATE, CommSessionEvent::INITIALIZED_EVENT, READING_HEADER_STATE);

		// Add READING_HEADER_STATE transitions
		fsm_->addTransition(READING_HEADER_STATE, CommSessionEvent::HEADER_RECEIVED_EVENT, PROCESSING_HEADER_STATE);

		// Add PROCESSING_HEADER_STATE transitions
		fsm_->addTransition(PROCESSING_HEADER_STATE, CommSessionEvent::HEADER_PROCESSED_EVENT, SENDING_HEADER_STATE);

		// Add SENDING_HEADER_STATE transitions
		fsm_->addTransition(SENDING_HEADER_STATE, CommSessionEvent::HEADER_SENT_EVENT, READING_COMMAND_STATE);

		// Add READING_COMMAND_STATE transitions
		fsm_->addTransition(READING_COMMAND_STATE, CommSessionEvent::COMMAND_RECEIVED_EVENT, PROCESSING_COMMAND_STATE);
		fsm_->addTransition(READING_COMMAND_STATE, CommSessionEvent::READ_ALL_RECEIVED_EVENT, PROCESSING_READ_ALL_INIT_SUBSTATE);
		fsm_->addTransition(READING_COMMAND_STATE, CommSessionEvent::DELETE_ALL_RECEIVED_EVENT, PROCESSING_DELETE_ALL_INIT_SUBSTATE);
		fsm_->addTransition(READING_COMMAND_STATE, CommSessionEvent::FOOTER_RECEIVED_EVENT, PROCESSING_FOOTER_STATE);

		// Add PROCESSING_COMMAND_STATE transitions
		fsm_->addTransition(PROCESSING_COMMAND_STATE, CommSessionEvent::COMMAND_PROCESSED_EVENT, READING_COMMAND_STATE);

		// Add PROCESSING_READ_ALL_INIT_SUBSTATE transitions
		fsm_->addTransition(PROCESSING_READ_ALL_INIT_SUBSTATE, CommSessionEvent::READ_ALL_INIT_COMPLETED_EVENT, PROCESSING_READ_ALL_READ_SUBSTATE);
		fsm_->addTransition(PROCESSING_READ_ALL_INIT_SUBSTATE, CommSessionEvent::READ_ALL_DONE_EVENT, PROCESSING_READ_ALL_DONE_SUBSTATE);

		// Add PROCESSING_READ_ALL_READ_SUBSTATE transitions
		fsm_->addTransition(PROCESSING_READ_ALL_READ_SUBSTATE, CommSessionEvent::READ_ALL_RECORD_READ_EVENT, PROCESSING_READ_ALL_WRITE_SUBSTATE);
		fsm_->addTransition(PROCESSING_READ_ALL_READ_SUBSTATE, CommSessionEvent::READ_ALL_DONE_EVENT, PROCESSING_READ_ALL_DONE_SUBSTATE);

		// Add PROCESSING_READ_ALL_WRITE_SUBSTATE transitions
		fsm_->addTransition(PROCESSING_READ_ALL_WRITE_SUBSTATE, CommSessionEvent::READ_ALL_RECORD_WRITTEN_EVENT, PROCESSING_READ_ALL_READ_SUBSTATE);

		// Add PROCESSING_READ_ALL_DONE_SUBSTATE transitions
		fsm_->addTransition(PROCESSING_READ_ALL_DONE_SUBSTATE, CommSessionEvent::READ_ALL_COMPLETED_EVENT, READING_COMMAND_STATE);

		// Add PROCESSING_DELETE_ALL_INIT_SUBSTATE transitions
		fsm_->addTransition(PROCESSING_DELETE_ALL_INIT_SUBSTATE, CommSessionEvent::DELETE_ALL_INIT_COMPLETED_EVENT, PROCESSING_DELETE_ALL_READ_SUBSTATE);
		fsm_->addTransition(PROCESSING_DELETE_ALL_INIT_SUBSTATE, CommSessionEvent::DELETE_ALL_DONE_EVENT, PROCESSING_DELETE_ALL_DONE_SUBSTATE);

		// Add PROCESSING_DELETE_ALL_READ_SUBSTATE transitions
		fsm_->addTransition(PROCESSING_DELETE_ALL_READ_SUBSTATE, CommSessionEvent::DELETE_ALL_RECORD_READ_EVENT, PROCESSING_DELETE_ALL_DELETE_SUBSTATE);
		fsm_->addTransition(PROCESSING_DELETE_ALL_READ_SUBSTATE, CommSessionEvent::DELETE_ALL_DONE_EVENT, PROCESSING_DELETE_ALL_DONE_SUBSTATE);

		// Add PROCESSING_DELETE_ALL_DELETE_SUBSTATE transitions
		fsm_->addTransition(PROCESSING_DELETE_ALL_DELETE_SUBSTATE, CommSessionEvent::DELETE_ALL_RECORD_DELETED_EVENT, PROCESSING_DELETE_ALL_READ_SUBSTATE);

		// Add PROCESSING_DELETE_ALL_DONE_SUBSTATE transitions
		fsm_->addTransition(PROCESSING_DELETE_ALL_DONE_SUBSTATE, CommSessionEvent::DELETE_ALL_COMPLETED_EVENT, READING_COMMAND_STATE);

		// Add PROCESSING_FOOTER_STATE transitions
		fsm_->addTransition(PROCESSING_FOOTER_STATE, CommSessionEvent::FOOTER_PROCESSED_EVENT, SENDING_FOOTER_STATE);

		// Add SENDING_FOOTER_STATE transitions
		fsm_->addTransition(SENDING_FOOTER_STATE, CommSessionEvent::FOOTER_SENT_EVENT, DISCONNECTING_STATE);

		// Add ERROR_STATE transitions
		fsm_->addTransition(ERROR_STATE, CommSessionEvent::ERROR_PROCESSED_DISCONNECT_EVENT, DISCONNECTING_STATE);
		fsm_->addTransition(ERROR_STATE, CommSessionEvent::ERROR_PROCESSED_CONCLUDE_EVENT, CONCLUDING_STATE);

		// Add DISCONNECTING_STATE transitions
		fsm_->addTransition(DISCONNECTING_STATE, CommSessionEvent::DISCONNECTED_EVENT, CONCLUDING_STATE);

		// Add CONCLUDING_STATE transitions
		fsm_->addTransition(CONCLUDING_STATE, CommSessionEvent::END_EVENT);

		// Add default transitions
		fsm_->addDefaultTransition(CommSessionEvent::ERROR_EVENT, ERROR_STATE);
	}


	Util::CSmartPtr<SessionMessage> CommSession::readMessage()
	{
		return Util::CSmartPtr<SessionMessage>( myMessenger.receive() );
	}


	bool CommSession::writeHeader(Util::CSmartPtr<hfc::HeaderRecord> header)
	{
		{
			Util::CSmartPtr<SessionMessage> message(new SessionMessage(SessionDev, SnCommStart));
			message->setFormatType(SynchronizationTaggedFormat);
			message->setHfcObject(header.get());

			int bytesSent = myMessenger.send(message.get(), Messenger::DEST_UI, false);
			if ( bytesSent < 0 )
				Debug::trace("CommSession", "writeMessage failed (bytesSent = " + util_string::inttostring(bytesSent));
			else
				return true;
		}

		return false;
	}


	bool CommSession::writeMessage(Util::CSmartPtr<SessionMessage> message)
	{
		int bytesSent = myMessenger.send(message.get(), Messenger::DEST_UI, false);
		if ( bytesSent < 0 )
			Debug::trace("CommSession", "writeMessage failed (bytesSent = " + util_string::inttostring(bytesSent));
		else
			return true;

		return false;
	}


	bool CommSession::writeFooter(Util::CSmartPtr<SessionMessage> footer)
	{
		/* Ensure the footer is only sent while in the SENDING_FOOTER_STATE state. */
		if ( fsm_->getCurrentState()->getId() == SENDING_FOOTER_STATE )
		{
			int bytesSent = myMessenger.send(footer.get(), Messenger::DEST_UI, false);
			if ( bytesSent < 0 )
				Debug::trace("CommSession", "writeFooter failed (bytesSent = " + util_string::inttostring(bytesSent));
			else
				return true;
		}

		return false;
	}


	void CommSession::addBusyError()
	{
		if ( !sentBusyError_ )
		{
			/* (uc) Send a "Busy" error to the other scale. Close the connection.
			End the execution of this use case. */
			Util::CSmartPtr<HfcError> error(new HfcError(HfcError::ERRORCODE_SCALEBUSY));
			addError(error);

			sentBusyError_ = true;
		}
	}


	void CommSession::addError(Util::CSmartPtr<HfcError> error)
	{
		errorList_.push_back(error);
	}


	bool CommSession::sendExistingErrors()
	{
		while ( errorList_.size() )
		{
			Util::CSmartPtr<HfcError> error = errorList_.front();
			errorList_.pop_front();

			Util::CSmartPtr<SessionMessage> message(new SessionMessage(Error, DtWrite));
			message->setFormatType(SynchronizationTaggedFormat);
			message->setHfcObject(error.get());

			writeMessage(message);
		}

		return true;
	}


	bool CommSession::isReadAllMessage(EvolDeviceType device, UINT8 event)
	{
		switch ( device )
		{
			case ProductDev:
				return event == ProductReadAll;
			case COOLDev:
			{
				switch ( event )
				{
					case COOLDevReadAllCategoryCountriesForUltima:
					case COOLDevReadAllCategoryProdStepsForUltima:
					case COOLDevReadAllShortlistsForUltima:
					case COOLDevReadAllCOOLTotalsForUltima:
					case COOLDevReadAllCountries:
					case COOLDevReadAllProductionSteps:
					case COOLDevReadAllPretexts:
					case COOLDevReadAllCategories:
					case COOLDevReadAllCoolText:
					case COOLDevReadAllCategoryCountries:
					case COOLDevReadAllCategoryProductionSteps:
					case COOLDevReadAllCategoryPretexts:
					case COOLDevReadAllShortlists:
					case COOLDevReadAllShortlistTexts:
//					case COOLDevReadAllCOOLTotals:
						return true;
					default:
						return false;
				}
			}
			case SoftkeyDev:
			{
				switch ( event )
				{
					case ReadAllSoftkeys:
					case ReadAllSoftkeyFeatures:
					case ReadAllSoftkeyStyles:
					case ReadAllSoftkeyGroups:
						return true;
					default:
						return false;
				}
			}
			case SynchronizationDev:
			{
				switch ( event )
				{
					case synchronization::ReadBatches:
						return true;
					default:
						return false;
				}
			}
			default:
				return event == DtReadAll;
		}
	}


	bool CommSession::isDeleteAllMessage(EvolDeviceType device, UINT8 event)
	{
		switch ( device )
		{
			case ProductDev:
				return event == ProductDeleteAll;
			case COOLDev:
			{
				switch ( event )
				{
//					case COOLDevDeleteAllCategoryCountriesForUltima:
//					case COOLDevDeleteAllCategoryProdStepsForUltima:
//					case COOLDevDeleteAllShortlistsForUltima:
//					case COOLDevDeleteAllCOOLTotalsForUltima:
					case COOLDevDeleteAllCountries:
					case COOLDevDeleteAllProductionSteps:
					case COOLDevDeleteAllPretexts:
					case COOLDevDeleteAllCategories:
					case COOLDevDeleteAllCoolText:
					case COOLDevDeleteAllCategoryCountries:
					case COOLDevDeleteAllCategoryProductionSteps:
					case COOLDevDeleteAllCategoryPretexts:
					case COOLDevDeleteAllShortlists:
					case COOLDevDeleteAllShortlistTexts:
//					case COOLDevDeleteAllCOOLTotals:
						return true;
					default:
						return false;
				}
			}
			case SoftkeyDev:
			{
				switch ( event )
				{
					case DeleteAllSoftkeys:
					case DeleteAllSoftkeyFeatures:
					case DeleteAllSoftkeyStyles:
					case DeleteAllSoftkeyGroups:
						return true;
					default:
						return false;
				}
			}
			default:
				return event == DtDeleteAll;
		}
	}


	void CommSession::debug(const std::string& /*category*/, int /*level*/, const std::string& /*message*/) const
	{
	}


	const std::string& CommSession::getStateName(const stateMachine_type::state_type* state)
	{
		if ( state )
		{
			switch ( state->getId() )
			{
				case INITIALIZING_STATE:                   return _statenameInitializing_;
				case READING_HEADER_STATE:                 return _statenameReadingHeader_;
				case PROCESSING_HEADER_STATE:              return _statenameProcessingHeader_;
				case SENDING_HEADER_STATE:                 return _statenameSendingHeader_;
				case READING_COMMAND_STATE:                return _statenameReadingCommand_;
				case PROCESSING_COMMAND_STATE:             return _statenameProcessingCommand_;
				case PROCESSING_READ_ALL_INIT_SUBSTATE:    return _statenameProcessingReadAllInitSubstate_;
				case PROCESSING_READ_ALL_READ_SUBSTATE:    return _statenameProcessingReadAllReadSubstate_;
				case PROCESSING_READ_ALL_WRITE_SUBSTATE:   return _statenameProcessingReadAllWriteSubstate_;
				case PROCESSING_READ_ALL_DONE_SUBSTATE:    return _statenameProcessingReadAllDoneSubstate_;
				case PROCESSING_DELETE_ALL_INIT_SUBSTATE:  return _statenameProcessingDeleteAllInitSubstate_;
				case PROCESSING_DELETE_ALL_READ_SUBSTATE:  return _statenameProcessingDeleteAllReadSubstate_;
				case PROCESSING_DELETE_ALL_DELETE_SUBSTATE:return _statenameProcessingDeleteAllDeleteSubstate_;
				case PROCESSING_DELETE_ALL_DONE_SUBSTATE:  return _statenameProcessingDeleteAllDoneSubstate_;
				case PROCESSING_FOOTER_STATE:              return _statenameProcessingFooter_;
				case SENDING_FOOTER_STATE:                 return _statenameSendingFooter_;
				case ERROR_STATE:                          return _statenameError_;
				case DISCONNECTING_STATE:                  return _statenameDisconnecting_;
				case CONCLUDING_STATE:                     return _statenameConcluding_;
			}
		}

		return _statenameUnknown_;
	}


}
