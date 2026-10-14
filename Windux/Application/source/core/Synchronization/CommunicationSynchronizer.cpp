#include "CommunicationSynchronizer.h"


#include "RequestRegistrationUseCase.h"
#include "NotifyLocalBatchesPresentUseCase.h"
#include "DownloadLocalBatchesUseCase.h"
#include "NotifyOnlineUseCase.h"
#include "SynchronizeSecondaryScaleUseCase.h"


#include "SessionManager.h"
#include "Configuration.h"

#include "CMessageCenter.h"

#include "ScaleManager.h"

#include "CommunicationSynchronizerError.h"
#include "HfcError.h"
#include "SessionMessage.h"
#include "SessionEvent.h"
#include "HeaderRecord.h"
#include "FooterRecord.h"
#include "CMReaderWriterSyncTag.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/ChangeSynchronizationTypeMessage.h"
#include "Messages/DeregisterSecondaryScaleMessage.h"
#include "Messages/RetryCommandMessage.h"
#include "Messages/ExecuteCommandMessage.h"
#include "Messages/CompleteCommandMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"

#include "Commands/SynchronizePrimaryScaleCommand.h"
#include "Commands/RequestRegistrationCommand.h"
#include "Commands/DownloadLocalBatchesCommand.h"
#include "Commands/NotifyOnlineCommand.h"
#include "Commands/SynchronizeSecondaryScaleCommand.h"


#include "bootParameters.h"
#include "HobIpAddress.h"
#include "Debug.h"
#include "util/e_string/utilstring.h"
#include "util/thread/thread.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif


namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int CommunicationSynchronizer::_dataType_ = 0;
	const string CommunicationSynchronizer::_dataTypeName_("synchronization::CommunicationSynchronizer");
#endif

		
	const std::string CommunicationSynchronizer::_statenameInitializing_          ("Initializing");
	const std::string CommunicationSynchronizer::_statenameConnecting_            ("Connecting");
	const std::string CommunicationSynchronizer::_statenameSendingHeader_         ("Sending Header");
	const std::string CommunicationSynchronizer::_statenameReadingHeader_         ("Reading Header");
	const std::string CommunicationSynchronizer::_statenameProcessingHeader_      ("Processing Header");
	const std::string CommunicationSynchronizer::_statenameSendingCommand_        ("Sending Command");
	const std::string CommunicationSynchronizer::_statenameReadingCommand_        ("Reading Command");
	const std::string CommunicationSynchronizer::_statenameProcessingCommand_     ("Processing Command");
	const std::string CommunicationSynchronizer::_statenameSendingFooter_         ("Sending Footer");
	const std::string CommunicationSynchronizer::_statenameProcessingFooter_      ("Processing Footer");
	const std::string CommunicationSynchronizer::_statenameError_                 ("Error");
	const std::string CommunicationSynchronizer::_statenameDisconnecting_         ("Disconnecting");
	const std::string CommunicationSynchronizer::_statenameConcluding_            ("Concluding");
	const std::string CommunicationSynchronizer::_statenameUnknown_               ("Unknown");


	CommunicationSynchronizer::CommunicationSynchronizer(Hobart::CEvolMsgReceiver* syncManager, hfc::Scale::ScaleRole scaleRole)
			: Synchronizer(syncManager, "CommunicationSynchronizer"),
			thread_(0), scaleRole_(scaleRole), result_(CommSessionEvent::COMMERROR_NO_ERROR), connected_(false),
			commandSent_(false), headerReceived_(false), footerSent_(false), waitingForFooter_(false), processedFooter_(false),
			needToDisconnect_(false), targetBusy_(false)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		msgManager_->RestartThread();

		fsm_ = new StateMachine<CommunicationSynchronizer>(*this); 
		setupStateMachine(); 
		fsm_->setNotifyTopOfLoop();

		thread_ = new Util::CTLSThread(*this, THREAD_PRIORITY_NORMAL, 0, "CommunicationSynchronizer Thread", false, true, true);
	}


	CommunicationSynchronizer::~CommunicationSynchronizer()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
		if (thread_)
		{
			thread_->StopThread();
			thread_->WaitForCompletion();
		}
	}


	void CommunicationSynchronizer::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
	{
		if ( msg.notNull() )
		{
			if ( msg->getDeviceType() == SynchronizationDev && !stoppingSynchronizer_ )
			{


				UINT32 result = SessionManager::getInstance()->requestToContinue(msgManager_->GetThreadID(), msgManager_->IsASuperThread());
				result &= (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED);

				restoreDB (result);


				if ( msg->getEventType() == ExecuteCommandEvent )
				{
					Util::CSmartPtr<EvolMessage> response =
						processExecuteCommandMessage(Util::CSmartPtr<ExecuteCommandMessage>(new ExecuteCommandMessage(msg->Data())));

					if ( response.notNull() )
						msgManager_->dispatchEvent(response.get(), qMsgManager_);
				}
				else
				{
					Debug::trace("CommunicationSynchronizer", "processEvent: got an unknown SynchronizationDev message (event = " +
							util_string::uinttostring(msg->getEventType()) + ")");
				}

				//processEventThreadTopOfLoopEvent_.EventSet();
				SessionManager::getInstance()->changeBlockabilityStatus(msgManager_->GetThreadID(), true, msgManager_->IsASuperThread());

			}
			else
			{
				Debug::trace("CommunicationSynchronizer", "processEvent: could not process message (device = " +
						util_string::inttostring(msg->getDeviceType()) + ", event = " + util_string::uinttostring(msg->getEventType()) +
						") while Synchronizer " + std::string(stoppingSynchronizer_ ? "IS" : "is NOT") + " stopping");
			}
		}
		else
		{
			Debug::trace("CommunicationSynchronizer", "processEvent: got a NULL message");
		}
	}


	bool CommunicationSynchronizer::atTopOfLoop()
	{ 
		UINT32 result = SessionManager::getInstance()->requestToContinue(thread_->GetThreadID(),thread_->IsASuperThread());
		restoreDB(result & (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED));

		/* We return !stoppingSynchronizer_ so that if we are stopping (destroying) the Synchronization
		Manager (procedure that sets this flag) we will return false which makes us drop off the while
		loop in the state machine. */
		return !stoppingSynchronizer_; 
	}


	DWORD CommunicationSynchronizer::Run()
	{
		int status = _statusNothingToDo_;


		targetScale_ = 0;
		footerSent_ = false;


		if ( !stoppingSynchronizer_ )
		{
			/* (uc6.54) If the command is: */
			switch ( command_->getType() )
			{
				case Command::SynchronizePrimaryScaleCommandType:
				{
					/* (uc6.54) a. Synchronize Primary scale, then execute "Use Case 6.4: Synchronizing
					Primary scale", passing the command received, receiving the status result. */
					usecase_ = new DownloadLocalBatchesUseCase(this);

					break;
				}
				case Command::RequestRegistrationCommandType:
				{
					/* (uc6.54) a. Request registration, then execute "Use Case 6.12: Requesting
					registration", passing the command received, receiving the status result. */
					usecase_ = new RequestRegistrationUseCase(this);

					break;
				}
				case Command::RequestDeregistrationCommandType:
				{
					/* (uc6.54) b. Request deregistration, then execute "Use Case 6.14: Requesting
					deregistraton", passing the command received, receiving the status result. */
					/* (TODO) This use case is currently not supported (i.e., no Secondary currently
					requests to deregister from its Primary scale) */
					status = _statusOK_;

					break;
				}
				case Command::NotifyLocalBatchesPresentCommandType:
				{
					/* (uc6.54) c. Notify local batches present, then execute "Use Case 6.23: Notifying
					local batches present", passing the command received, receiving the status result. */
					usecase_ = new NotifyLocalBatchesPresentUseCase(this);

					break;
				}
				case Command::DownloadLocalBatchesCommandType:
				{
					/* (uc6.54) b. Download local batches, then execute "Use Case 6.24: Downloading
					Secondary scale local batches", passing the command received, receiving the
					status result. */
					usecase_ = new DownloadLocalBatchesUseCase(this);

					break;
				}
				case Command::NotifyOnlineCommandType:
				{
					/* (uc6.54) d. Notify On-line, then execute "Use Case 6.30: Requesting
					synchronization", passing the command received, receiving the status result. */
					usecase_ = new NotifyOnlineUseCase(this);

					break;
				}
				case Command::SynchronizeSecondaryScaleCommandType:
				{
					/* (uc6.54) c. Synchronize Secondary scale, then execute "Use Case 6.32: Synchronizing
					Secondary scale", passing the command received, receiving the status result. */
					const SynchronizeSecondaryScaleCommand* syncPrimary = dynamic_cast<const SynchronizeSecondaryScaleCommand *>(command_.get());
					if ( syncPrimary )
					{
						usecase_ = new SynchronizeSecondaryScaleUseCase(this, syncPrimary->getTargetIdentifier());
					}

					break;
				}
            #if defined(LINUX_FIX)
                default:
                {
                    // Nothing to do
                    break;
                }
            #endif
			}


			if ( usecase_.notNull() && !stoppingSynchronizer_ )
			{
				status = _statusOK_;
				usecase_->showUseCaseStartingInfo();


				/* Retrieve the scale record corresponding to the target scale. */
				if ( scaleRole_ == hfc::Scale::RolePrimary )
				{
					/* (uc) Retrieve the Secondary scale's database record. */
					targetScale_ = getScaleManager()->findScaleByIdentifier(util_string::inttostring(command_->getTargetIdentifier()));
				}
				else
				{
					/* (uc) Retrieve the Primary scale's database record. */
					targetScale_ = getScaleManager()->getPrimaryScale();
				}


				if ( targetScale_.notNull() && targetScale_->getErrorCode() == RECORD_OK )
				{
					Debug::trace("CommunicationSynchronizer", "Target scale id = " + util_string::inttostring(targetScale_->getIdentifier()) +
							", hostname = " + targetScale_->getHostName());


					fsm_->start(new event_type(event_type::START_EVENT), INITIALIZING_STATE);
				                      
					fsm_->runStateMachine();

					if ( (*fsm_.get())() != stateMachine_type::STOPPED_STATE )
					{
						Debug::trace("CommunicationSynchronizer", "StateMachine stopped running unexpectedly (" +
								fsm_->getStateName() + "). Last state is " + getStateName(fsm_->getCurrentState()) +
								" and last event is " + util_string::inttostring(fsm_->getCurrentEvent() ? fsm_->getCurrentEvent()->getId() : -1) +
								". Synchronizer " + std::string(stoppingSynchronizer_ ? "IS" : "is NOT") + " stopping.");

						status = _statusStateMachineFailed_;
					}
					else
						status = processResult();
				}
				else
				{ status = _statusTargetScaleNotFound_; }
			}
			else
				status = _statusNothingToDo_;
		}
		else
			status = _statusStoppingSynchronizer_;


		if ( !stoppingSynchronizer_ )
		{
			/* (uc6.54) If the command included a completion message, then set the received status in it. */
			command_->setStatus(status);


			switch ( status )
			{
				case _statusOK_:
				{
					/* (uc6.54) If the result was success, then trigger the execution of "Use Case 5.55:
					Completing command", passing the reference to the queue and the command. ... */
					if ( usecase_.notNull() )
						usecase_->showUseCaseSuccessful();

					CompleteCommandMessage message(command_);
					msgManager_->dispatchEvent(&message, qMsgManager_);

					break;
				}
				default:
				{
					if ( scaleRole_ == hfc::Scale::RoleSecondary )
					{
						/* (SYNCSTATUS) notify UI that we failed to complete operation with Primary. */
						NewSynchronizationStatusMessage message(UnableToContactPrimarySyncStatus);
						msgManager_->dispatchEvent(&message);
					}


					/* (uc6.54) ... Otherwise, trigger the execution of "Use Case 5.56: Retrying command",
					passing the same parameters. */
					usecase_->showUseCaseFailed(getStatusText(status));

					RetryCommandMessage message(command_);
					msgManager_->dispatchEvent(&message, qMsgManager_);

					break;
				}
			}
		}

		command_ = 0;
		usecase_ = 0;


		return 0;
	}


	void CommunicationSynchronizer::waitUntilRunThreadEnds()
	{
		msgManager_->ShutdownNicely();
		thread_->WaitForCompletion();
	}

	/* "Use Case 6.54: Executing command". */
	Util::CSmartPtr<EvolMessage> CommunicationSynchronizer::processExecuteCommandMessage(Util::CSmartPtr<ExecuteCommandMessage> msg)
	{
		if ( msg.notNull() )
		{
			usecaseExecutingCommand(msg->getCommand());

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 6.54: Executing command". */
	int CommunicationSynchronizer::usecaseExecutingCommand(Util::CSmartPtr<Command> command)
	{
		if ( command.notNull() )
		{
			if ( !stoppingSynchronizer_ )
			{
				{
					Debug::trace("CommunicationSynchronizer", "Use Case Executing Command: "
							"!!!??? the thread is still running from a previous instance !!!??? Waiting from completion.");

					util_time::Time t0;

					thread_->WaitForCompletion();

					util_time::Time t1;
					Debug::trace("CommunicationSynchronizer", "Use Case Executing Command: "
							"!!!??? the thread finished running from a previous instance !!!??? (had to wait for " +
							util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " ms.) Continuing ...");
				}

				command_ = command;

				setProcessingData(true);

				thread_->RestartThread();
			}
			else
				Debug::trace("CommunicationSynchronizer", "********* Use Case Executing Command: "
						"Did not attempt to execute command since we are stopping the Synchronzier");
		}
		else
			Debug::trace("CommunicationSynchronizer", "********* Use Case Executing Command: !!!??? no command was received !!!???");

		return _statusOK_;
	}


	/* "Use Case 6.58: Resuming processing". */
	int CommunicationSynchronizer::usecaseResumingProcessing()
	{
//		pausingProcessing_ = false;

		return _statusOK_;
	}


	/* "Use Case 6.59: Pausing processing". */
	int CommunicationSynchronizer::usecasePausingProcessing(int reason)
	{
		textId_ = reason;
		setProcessingData(false);


		/* Synchronize with the top of the run thread's and the processEvent's main loop to ensure
		that they will not execute code in their main loops until the resumeProcessing arrives. */
		util_time::Time t0 = util_time::Time();

		pausingProcessing_ = true;


		util_time::Time t1 = util_time::Time();
		Debug::trace("Synchronizer", "usecasePausingProcessing: Waited " + 
				util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");


		return _statusOK_;
	}


	/* --- State Machine-related functionality --- */
	bool CommunicationSynchronizer::processingInitializingState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Initializing State: Started processing");


		waitBeforeConnTimer_ = Configuration::getInstance()->toIntCfgI(CfgInternal::WAITBEFORECONNECTINGTIMER);
		waitBeforeConnDelay_ = Configuration::getInstance()->toIntCfgI(CfgInternal::WAITBEFORECONNECTINGDELAY);
		timeoutReadingMessage_ = Configuration::getInstance()->toIntCfgI(CfgInternal::COMMSESSIONREADMESSAGETIMER);
		readingMessageDelay_ = Configuration::getInstance()->toIntCfgI(CfgInternal::COMMSESSIONREADMESSAGEDELAY);
		receivedHeader_ = 0;
		receivedCommand_ = 0;
		receivedFooter_ = 0;
		processedFooter_ = false;
		waitingForFooter_ = false;
		needToDisconnect_ = false;
		sentBusyError_ = false;

		connected_ = false;
		targetBusy_ = false;
		result_ = CommSessionEvent::COMMERROR_NO_ERROR;

		if ( isProcessingData() )
		{
			if ( usecase_->doInitialize() )
			{
				Debug::trace("CommunicationSynchronizer", "Initializing State: completed successfully.");

				waitingForConnectTime_ = util_time::Time();
				fsm_->postEvent(new event_type(event_type::INITIALIZATION_COMPLETE_EVENT));

				return true;
			}
			else
			{
				Debug::trace("CommunicationSynchronizer", "Initializing State: completed successfully and trying again");

				return false;
			}
		}
		else
		{
			Debug::trace("CommunicationSynchronizer", "Initializing State: did not attempt since we are not processing data.");

			fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_NOT_PROCESSING_DATA));

			return true;
		}
	}


	bool CommunicationSynchronizer::processingConnectingState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Connecting State: Started processing");


		if ( isProcessingData() )
		{
			util_time::Time t1;
			if ( scaleRole_ == hfc::Scale::RolePrimary && (t1 - waitingForConnectTime_).getTotalSeconds() < waitBeforeConnTimer_ )
			{
            #if defined(LINUX_FIX)
                PAL_absclass::PAL_sleep( waitBeforeConnDelay_ );
            #else
				Sleep(waitBeforeConnDelay_);
            #endif

				return false;
			}


			/* Establish a connection with the scale. */
			socket_ = new Socket();
			if ( !socket_->isValid() )
			{
				Debug::trace("CommunicationSynchronizer", "Connecting State: failed. Unable to create socket.");

				fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_CANNOT_CREATE_SOCKET));

				return true;
			}


			/* Determine the TCP port from configuration. */
			int tcpPort = Configuration::getInstance()->getWellKnownPort();


			/* Attempt to connect using the hostname. */
			if ( targetScale_->getHostName().size() > 0 )
			{
				Debug::trace("CommunicationSynchronizer", "Connecting State: before connecting with " + targetScale_->getHostName());

				STATUS result = socket_->setDnsName(targetScale_->getHostName());
				
				if (result != ERROR_CODE)
				{				
					socket_->setPort(tcpPort);
					socket_->setNonBlockingIO();
					socket_->sConnect2();
					if ( !socket_->isValid() )
					{
						Debug::trace("CommunicationSynchronizer", "Connecting State: unable to connect to target scale through hostname \'" +
							targetScale_->getHostName() + "\'");

						socket_ = new Socket();
						if ( !socket_->isValid() )
						{
							Debug::trace("CommunicationSynchronizer", "Connecting State: failed. Unable to create socket.");

							fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_CANNOT_CREATE_SOCKET));

							return true;
						}
					}
					else
					{
						connected_ = true;

						Debug::trace("CommunicationSynchronizer", "Connecting State: connected to target scale through hostname \'" +
								targetScale_->getHostName() + "\'");
					}
				}
				else
				{
					Debug::trace("CommunicationSynchronizer", "Could not use host name, going to use IP Address!");
				}			
			}

			/* If HostName did not get us to connect, use IPAddress. */
			if ( !connected_ )
			{
				const std::string& ipAddress = targetScale_->getIPAddress();

				if ( ipAddress.size() > 0 )
				{
					socket_->setAddress(ipAddress.c_str());
					socket_->setPort(tcpPort);
					socket_->setNonBlockingIO();
					socket_->sConnect2();
					if ( socket_->isValid() )
					{
						connected_ = true;

						Debug::trace("CommunicationSynchronizer", "Connecting State: connected to target scale through IP address \'" +
								ipAddress + "\'");
					}
					else
						Debug::trace("CommunicationSynchronizer", "Connecting State: unable to connect to target scale through IP address \'" +
								ipAddress + "\'");
				}
			}


			/* If we have not connected yet, return an error. */
			if ( !connected_ )
			{
				Debug::trace("CommunicationSynchronizer", "Connecting State: failed. Unable to connect to target scale.");

				fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_CANNOT_CONNECT));

				return true;
			}
			else
			{
				socket_->setMyReaderWriter(new CMReaderWriterSyncTag());

				Debug::trace("CommunicationSynchronizer", "Connecting State: completed successfully.");

				fsm_->postEvent(new event_type(event_type::CONNECTED_EVENT));

				return true;
			}
		}
		else
		{
			Debug::trace("CommunicationSynchronizer", "Connecting State: did not attempt since we are not processing data.");

			fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_NOT_PROCESSING_DATA));

			return true;
		}
	}


	bool CommunicationSynchronizer::processingSendingHeaderState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Sending Header State: Started processing");


		/* Create the Header record. */
		hfc::HeaderRecord header;

		header.setDataType(hfc::HeaderRecord::TypeSyncComm);
		header.setScaleRole(scaleRole_);
		header.setHostName(bootParameters::getDNSHostName());
		header.setIPAddress(bootParameters::getIPAddress());
		header.setCurrentDateTime();


		/* Send the Header. */
		Util::CSmartPtr<SessionMessage> message(new SessionMessage(SessionDev, SnCommStart));
		message->setFormatType(SynchronizationTaggedFormat);
		message->setHfcObject(&header);

		if ( sendMessage(message, false) )
		{
			Debug::trace("CommunicationSynchronizer", "Sending Header State: completed successfully.");

			fsm_->postEvent(new event_type(event_type::HEADER_SENT_EVENT));
		}
		else
		{
			Debug::trace("CommunicationSynchronizer", "Sending Header State: failed to send data.");

			fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_WRITING_TO_SOCKET));
		}


		return true;
	}


	bool CommunicationSynchronizer::processingReadingHeaderState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Reading Header State: Started processing");


		Util::CSmartPtr<SessionMessage> message = readMessage();
		if ( message.notNull() )
		{
			if ( message->getDeviceType() == SessionDev && message->getEventType() == SnCommStart )
			{
				headerReceived_ = true;
				receivedHeader_ = dynamic_cast<hfc::HeaderRecord*>(message->getHfcObject());
				message->setHfcObjectCreatedInternally(false);

				Debug::trace("CommunicationSynchronizer", "Reading Header State: completed successfully");

				fsm_->postEvent(new event_type(event_type::HEADER_RECEIVED_EVENT));

				return true;
			}
			else
			{
				// We did non receive a header when expecting it (???). Conclude.
				Debug::trace("CommunicationSynchronizer", "Reading Header State: Expected a header but received a message " +
						util_string::inttostring(message->getDeviceType()) + "," +
						util_string::inttostring(message->getEventType()) + ": " + message->getOriginalBuff());

				fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_DIDNOT_RECEIVE_HEADER));

				return true;
			}
		}
		else
		{
			Debug::trace("CommunicationSynchronizer", "Reading Header State: failed.");

			fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_DIDNOT_RECEIVE_HEADER));

			return true;
		}
	}


	bool CommunicationSynchronizer::processingProcessingHeaderState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Processing Header State: Started processing");


		/* Extract host information out of the header. */
		bool changed = false;
		const std::string& hostname = receivedHeader_->getHostName();
		const std::string& ipAddress = receivedHeader_->getIPAddress();

		Debug::trace("CommunicationSynchronizer", "Processing Header State: Header indicates hostname = " +
				hostname + ", IP address = " + ipAddress);


		if ( isProcessingData() )
		{
			/* Verify header information (primary vs. secondary, etc.). */

			/* Verify the data type to correspond to synchronization. */
			if ( receivedHeader_->getDataType() != hfc::HeaderRecord::TypeSyncComm )
			{
				Debug::trace("CommunicationSynchronizer", "Processing Header State: completed, but got a header with data type " +
					util_string::inttostring(receivedHeader_->getDataType()) + ", not synchronization data type");

				fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_WRONG_DATA_TYPE));

				return true;
			}


			/* Verify scale role. */
			if ( scaleRole_ == hfc::Scale::RolePrimary && receivedHeader_->getScaleRole() != hfc::Scale::RoleSecondary )
			{
				Debug::trace("CommunicationSynchronizer",
						"Processing Header State: completed but received a header from a non-secondary scale");

				fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_NOT_A_SECONDARY_SCALE));

				return true;
			}

			if ( scaleRole_ == hfc::Scale::RoleSecondary && receivedHeader_->getScaleRole() != hfc::Scale::RolePrimary )
			{
				Debug::trace("CommunicationSynchronizer",
						"Processing Header State: received a header from a non-primary scale");

				fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_NOT_A_PRIMARY_SCALE));

				return true;
			}


			/* Update scale record as needed. */
			if ( hostname.size() && targetScale_->getHostName() != hostname )
			{
				Debug::trace("CommunicationSynchronizer", "Processing Header State: old hostname = " +
						targetScale_->getHostName() + " is different from received hostname. Changing it.");

				targetScale_->setHostName(hostname);
				changed = true;
			}

			if ( ipAddress.size() && HobIpAddress(targetScale_->getIPAddress()) != HobIpAddress(ipAddress) )
			{
				Debug::trace("CommunicationSynchronizer", "Processing Header State: old IP address = " +
						targetScale_->getIPAddress() + " is different from received IP address. Changing it.");

				targetScale_->setIPAddress(ipAddress);

				changed = true;
			}

			if ( changed )
				getScaleManager()->saveScale(targetScale_);
		}


		Debug::trace("CommunicationSynchronizer", "Processing Header State: completed successfully");

		fsm_->postEvent(new event_type(event_type::HEADER_PROCESSED_EVENT));

		return true;
	}


	bool CommunicationSynchronizer::processingSendingCommandState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Sending Command State: Started processing");


		sendExistingErrors();

		if ( isProcessingData() )
		{
			switch ( usecase_->doSendCommand() )
			{
				case STATUS_COMPLETED_ALL_COMMANDS:
				{
					Debug::trace("CommunicationSynchronizer", "Sending Command State: completed successfully all commands");

					fsm_->postEvent(new event_type(event_type::LAST_COMMAND_SENT_EVENT));

					return true;
				}
				case STATUS_COMPLETED_COMMAND:
				{
					Debug::trace("CommunicationSynchronizer", "Sending Command State: completed successfully");

					fsm_->postEvent(new event_type(event_type::COMMAND_SENT_EVENT));

					return true;
				}
				case STATUS_FAILED:
				{
					Debug::trace("CommunicationSynchronizer", "Sending Command State: failed");

					fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_WRITING_TO_SOCKET));

					return true;
				}
				case STATUS_NOT_COMPLETED:
				default:
				{
					Debug::trace("CommunicationSynchronizer", "Sending Command State: not completed yet");

					return false;
				}
			}
		}
		else
		{
			if ( commandSent_ )
				sendBusyError();

			Debug::trace("CommunicationSynchronizer", "Sending Command State: not processing data, moving to sending footer");

			fsm_->postEvent(new event_type(event_type::LAST_COMMAND_SENT_EVENT));

			return true;
		}
	}


	bool CommunicationSynchronizer::processingReadingCommandState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Reading Command State: Started processing");


		/* Read a command/message in a NON-BLOCKING fashion. */
		if ( socket_->cmNumMsgs() > 0 )
		{
			waitingForFooter_ = false;

			Util::CSmartPtr<SessionMessage> message = readMessage();
			if ( message.notNull() )
			{
				if ( footerSent_ )
				{
					if ( message->getDeviceType() == SessionDev && message->getEventType() == SnCommEnd )
					{
						receivedFooter_ = message;

						Debug::trace("CommunicationSynchronizer", "Reading Command State: completed successfully (received footer)");

						fsm_->postEvent(new event_type(event_type::FOOTER_RECEIVED_EVENT));

						return true;
					}
				}

				if ( isProcessingData() )
				{
					if ( message->getDeviceType() == Error && message->getEventType() == DtWrite )
					{
						receivedError_ = dynamic_cast<HfcError*>(message->getHfcObject());
						message->setHfcObjectCreatedInternally(false);

						Debug::trace("CommunicationSynchronizer", "Reading Command State: completed successfully (received error)");

						fsm_->postEvent(new event_type(event_type::ERROR_RECEIVED_EVENT));

						return true;
					}
					else
					{
						receivedCommand_ = message;

						Debug::trace("CommunicationSynchronizer", "Reading Command State: completed successfully.");

						fsm_->postEvent(new event_type(event_type::COMMAND_RECEIVED_EVENT));

						return true;
					}
				}
				/* If it is not processing data then ignore the received message and assume nothing
				was read. */
			}
			else
			{
				Debug::trace("CommunicationSynchronizer", "Reading Command State: failed.");

				fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_READING_FROM_SOCKET));

				return true;
			}
		}


		/* At this point nothing has been read (or it was ignored). */
		if ( !footerSent_ )
		{
			Debug::trace("CommunicationSynchronizer", "Reading Command State: completed successfully (read nothing).");

			fsm_->postEvent(new event_type(event_type::NOTHING_RECEIVED_EVENT));

			return true;
		}
		else
		{
			if ( !waitingForFooter_ )
			{
				waitingForFooterTime_ = util_time::Time();
				waitingForFooter_ = true;
			}
			else
			{
				util_time::Time t1;
				if ( (t1 - waitingForFooterTime_).getTotalSeconds() > timeoutReadingMessage_ )
				{
					Debug::trace("CommunicationSynchronizer", "Reading Command State: failed.");

					fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_TIMEDOUT_WAITING_FOOTER));

					return true;
				}
			}

			Debug::trace("CommunicationSynchronizer", "Reading Command State: not completed yet");

        #if defined(LINUX_FIX)
            PAL_absclass::PAL_sleep( readingMessageDelay_ );
        #else
			Sleep(readingMessageDelay_);
        #endif

			return false;
		}
	}


	bool CommunicationSynchronizer::processingProcessingCommandState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Processing Command State: Started processing");


		if ( isProcessingData() )
		{
			if ( usecase_->doProcessCommand(receivedCommand_) )
			{
				Debug::trace("CommunicationSynchronizer", "Processing Command State: completed successfully.");

				fsm_->postEvent(new event_type(event_type::COMMAND_PROCESSED_EVENT));

				return true;
			}
			else
			{
				Debug::trace("CommunicationSynchronizer", "Processing Command State: not completed yet.");

				return false;
			}
		}
		else
		{
			Debug::trace("CommunicationSynchronizer", "Processing Command State: ignored since we are not processing data.");

			fsm_->postEvent(new event_type(event_type::COMMAND_PROCESSED_EVENT));

			return true;
		}
	}


	bool CommunicationSynchronizer::processingSendingFooterState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Sending Footer State: Started processing");


		sendExistingErrors();


		/* Create the Footer record. */
		hfc::FooterRecord footer = hfc::FooterRecord();


		/* Send the Footer. */
		Util::CSmartPtr<SessionMessage> message(new SessionMessage(SessionDev, SnCommEnd));
		message->setFormatType(SynchronizationTaggedFormat);
		message->setHfcObject(&footer);

		if ( sendMessage(message, false) )
		{
			footerSent_ = true;

			Debug::trace("CommunicationSynchronizer", "Sending Footer State: completed successfully.");

			fsm_->postEvent(new event_type(event_type::FOOTER_SENT_EVENT));
		}
		else
		{
			Debug::trace("CommunicationSynchronizer", "Sending Footer State: failed to send data.");

			fsm_->postEvent(new CommunicationSynchronizerError(CommSessionEvent::COMMERROR_WRITING_TO_SOCKET));
		}

		return true;
	}


	bool CommunicationSynchronizer::processingProcessingFooterState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Processing Footer State: Started processing");


		/* So far we don't do anything with the footer. */
		processedFooter_ = true;


		Debug::trace("CommunicationSynchronizer", "Processing Footer State: completed successfully (ignored).");

		fsm_->postEvent(new event_type(event_type::FOOTER_PROCESSED_EVENT));

		return true;
	}


	bool CommunicationSynchronizer::processingErrorState(const event_type& event)
	{
		Debug::trace("CommunicationSynchronizer", "Error State (event = " + util_string::inttostring(event.getId()) +
				"): Started processing");

		setProcessingData(false);


		if ( event.getId() == CommunicationSynchronizerEvent::ERROR_RECEIVED_EVENT )
		{
			/* This event indicates that we received an error from the target scale (the scale we
			are communicating with. The error was received and placed in "receivedError_". */
			if ( receivedError_.notNull() )
				result_ = (CommSessionEvent::CommError)receivedError_->getErrorCode();
		}
		else
		{
			/* This event (ERROR_EVENT) indicates that we generated an error condition during the
			processing of the different states of the state machine (probably due to input received
			through the socket open with the target scale. The error was passed with the event. */
			const CommunicationSynchronizerError& error = (const CommunicationSynchronizerError&)event;

			needToDisconnect_ = true;

			result_ = error.getError();
		}

		Debug::trace("CommunicationSynchronizer", "Error State: received error = " + util_string::inttostring(result_));


		processErrorCode(result_);
		/* (TODO) beware that usecase_ may need to deal with the not-processing-data condition in its doProcessError */
		usecase_->doProcessError(result_);

	
		Debug::trace("CommunicationSynchronizer", "Error State: completed successfully.");

		if ( connected_ )
		{
			if ( needToDisconnect_ )
			{
				fsm_->postEvent(new event_type(event_type::ERROR_PROCESSED_DISCONNECT_EVENT));
			}
			else
			{
				if ( footerSent_ /*|| !headerReceived_*/ )
				{
					if ( processedFooter_ )
						fsm_->postEvent(new event_type(event_type::ERROR_PROCESSED_DISCONNECT_EVENT));
					else
						fsm_->postEvent(new event_type(event_type::ERROR_PROCESSED_READ_FOOTER_EVENT));
				}
				else
					fsm_->postEvent(new event_type(event_type::ERROR_PROCESSED_SEND_FOOTER_EVENT));
			}
		}
		else
			fsm_->postEvent(new event_type(event_type::ERROR_PROCESSED_CONCLUDE_EVENT));

		return true;
	}


	void CommunicationSynchronizer::processErrorCode(CommSessionEvent::CommError error)
	{
		if ( scaleRole_ == hfc::Scale::RolePrimary )
		{
			switch ( error )
			{
				case CommSessionEvent::COMMERROR_CANNOT_CONNECT:
				{
					/* Exception: Secondary scale not found
					This exception is triggered when the Synchronizer fails to establish a
					connection with a Secondary scale due to not finding such host on the network.
					When this happens, perform the following steps:
					- Update the Secondary scale's database record with the latest information
					(status, date/time, et.) [this is done in processingConcludingState]
					- Return a status of failure [the current error code gets translated into a
					failure status in processResult] */

					break;
				}
				case CommSessionEvent::COMMERROR_NOT_SYNCHRONIZING_SCALE: 
				case CommSessionEvent::COMMERROR_NOT_A_SECONDARY_SCALE:
				case CommSessionEvent::COMMERROR_NOT_YOUR_SECONDARY_SCALE:
				{
					/* Exception: Not my Secondary scale
					This exception is triggered when the scale we connected with replies with an error
					indicating Not a Secondary scale (or Not your Secondary scale). When this happens,
					perform the following steps:
					- Close the connection. [this is done after exchanging footers]
					- Trigger the execution of "Use Case 5.15: Deregistering Secondary scale", passing
					the scale identifier. */
					Util::CSmartPtr<DeregisterSecondaryScaleMessage> message(new DeregisterSecondaryScaleMessage(targetScale_->getIdentifier()));
					msgManager_->dispatchEvent(message.get(), qMsgManager_);


					/*
					- Update the Secondary scale's database record with the latest information (status,
					date/time, etc.) [this is done in processConcludingState]
					- Return a status of success. [this is done in processResult] */

					break;
				}
				case CommSessionEvent::COMMERROR_SCALE_BUSY:
				{
					/* Exception: Secondary scale replied with error
					This exception is triggered by the Secondary scale replying with an error to our
					establishing of a connection. When this happens, perform the following steps:
					- Update the Secondary scale's database record with the latest information (status,
					date/time, etc.) [this is done in processingConcludingState]
					- Return a status of failure. [the current error code gets translated into a
					failure status in processResult] */

					targetBusy_ = true;
					needToDisconnect_ = true;

					break;
				}
            #if defined(LINUX_FIX)
                default:
                {
                    // Nothing to do
                    break;
                }
            #endif
			}
		}
		else
		{
			switch ( error )
			{
				case CommSessionEvent::COMMERROR_CANNOT_CONNECT:
				{
					/* Exception: Primary scale not found
					This exception is triggered when the Synchronizer fails to establish a
					connection with the Primary scale due to not finding such host on the network.
					When this happens, perform the following steps:
					- Update the Primary scale's database record with the latest information
					(status, date/time, et.) [this is done in processingConcludingState]
					- Return a status of failure [the current error code gets translated into a
					failure status in processResult] */

					break;
				}
				case CommSessionEvent::COMMERROR_NOT_SYNCHRONIZING_SCALE: 
				case CommSessionEvent::COMMERROR_NOT_A_PRIMARY_SCALE:
				{
					/* Exception: Not a Primary scale
					This exception is triggered when establishing a connection to the Primary scale
					and receiving an error indicating Not a Primary scale (or Not your Primary scale).
					When this happens, perform the following steps:
					- If the error message included the host name/IP address information of a new
					Primary scale, then store it temporarily. */
					/* [This may be a feature implemented in the future.] */


					/*
					- Close the connection. [this is done after exchanging footers]
					- If a new Primary scale's host name/IP address information was received, then
					trigger the execution of "Use Cas 1.11: Changing Primary scale", passing that
					information (make internally configurable whether to do this or not). */
					/* [This may be a feature implemented in the future.] */


					/*
					- Otherwise, trigger the execution of "Use Case 1.10: Changing Synchronization
					Type", passing Stand Alone scale as the new Scale Type configuration value. */
					/* [We will not execute the following lines of code (commented out) because the
					user may have configured this secondary to point to a primary not configured as
					such yet. The communication will fail and the primary scale will be marked as
					not active for a while until eventually will get removed, at which point this
					scale will reconfigure itself as a stand alone scale. */
//					ChangeSynchronizationTypeMessage message(ScaleSynchronizationTypeStandAlone, "");
//					Hobart::CMessageCenter::instance() << &message;


					/*
					- Update the Primary scale's database record with the latest information (status,
					date/time, etc.) [this is done in processingConcludingState]
					- Return a status of failure. [the current error code gets translated into a
					failure status in processResult] */

					break;
				}
				case CommSessionEvent::COMMERROR_SCALE_BUSY:
				{
					/* Exception: Primary scale replied with error
					This exception is triggered by the Primary scale replying with an error to our
					establishing of a connection. When this happens, perform the following steps:
					- Return a status of failure. [the current error code gets translated into a
					failure status in processResult] */

					targetBusy_ = true;
					needToDisconnect_ = true;

					break;
				}
            #if defined(LINUX_FIX)
                default:
                {
                    // Nothing to do
                    break;
                }
            #endif
			}
		}

		return;
	}


	bool CommunicationSynchronizer::processingDisconnectingState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Disconnecting State: Started processing");


		if ( connected_ )
		{
			socket_->cmClose();

			connected_ = false;
		}


		Debug::trace("CommunicationSynchronizer", "Disconnecting State: completed successfully.");

		fsm_->postEvent(new event_type(event_type::DISCONNECTED_EVENT));

		return true;
	}


	bool CommunicationSynchronizer::processingConcludingState(const event_type& /*event*/)
	{
		Debug::trace("CommunicationSynchronizer", "Concluding State: Started processing");


		/* (TODO) here call usecase_->doConclude() before the following if block and let each use
		case worry about isProcessingData or pausingProcessing_ in relation to what they have to do
		upon concluding/cleaning up. */
		usecase_->doConclude();


		if ( !pausingProcessing_ )
		{
			/* [Refresh the scale record just in case.] */
			targetScale_ = getScaleManager()->findScaleByIdentifier(targetScale_->getIdentifier());
			if ( targetScale_.notNull() )
			{
				targetScale_->setLastCommSuccessful(result_ == CommSessionEvent::COMMERROR_NO_ERROR);
				if ( targetBusy_ )
					targetScale_->incrementTotalBusyCommunications();

				getScaleManager()->saveScale(targetScale_);
			}
		}


		receivedHeader_ = 0;
		receivedCommand_ = 0;
		receivedFooter_ = 0;


		Debug::trace("CommunicationSynchronizer", "Concluding State: completed successfully.");

		fsm_->postEvent(new event_type(event_type::END_EVENT));

		return true;
	}


	Util::CSmartPtr<SessionMessage> CommunicationSynchronizer::readMessage()
	{
		HobString messageData;
		int len = socket_->cmReceive(messageData);
		if ( len > 0 )
			return Util::CSmartPtr<SessionMessage>( SessionMessage::create(messageData, SynchronizationTaggedFormat) );

		return Util::CSmartPtr<SessionMessage>( 0 );
	}


	bool CommunicationSynchronizer::sendMessage(Util::CSmartPtr<SessionMessage> message, bool isCommand)
	{
		if ( isCommand ) commandSent_ = true;

		const char* data = message->serialize();
		int bytesToSend = message->getSerializedLength();
		int bytesSent = socket_->cmSend(data, bytesToSend);
		if ( bytesSent < bytesToSend )
		{
			Debug::trace("CommunicationSynchronizer", "sendMessage: attempted to send " +
					util_string::inttostring(bytesToSend) + " bytes but only sent " +
					util_string::inttostring(bytesSent) + " bytes.");

			return false;
		}

		return true;
	}


	void CommunicationSynchronizer::sendBusyError()
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


	void CommunicationSynchronizer::addError(Util::CSmartPtr<HfcError> error)
	{
		errorList_.push_back(error);
	}


	bool CommunicationSynchronizer::sendExistingErrors()
	{
		if ( !footerSent_ )
		{
			while ( errorList_.size() )
			{
				Util::CSmartPtr<HfcError> error = errorList_.front();
				errorList_.pop_front();

				Util::CSmartPtr<SessionMessage> message(new SessionMessage(Error, DtWrite));
				message->setFormatType(SynchronizationTaggedFormat);
				message->setHfcObject(error.get());

				sendMessage(message, false);
			}
		}
		else
		{
			if ( errorList_.size() )
				Debug::trace("CommunicationSynchronizer", "***** There are errors to send but we already sent the footer.");
		}

		return true;
	}


	int CommunicationSynchronizer::processResult()
	{
		switch ( result_ )
		{
			case CommSessionEvent::COMMERROR_NO_ERROR:
			case CommSessionEvent::COMMERROR_NOT_A_SECONDARY_SCALE:
			case CommSessionEvent::COMMERROR_NOT_YOUR_SECONDARY_SCALE:
			{
				return _statusOK_;
			}
			case CommSessionEvent::COMMERROR_NOT_SYNCHRONIZING_SCALE:
			{
				if ( scaleRole_ == hfc::Scale::RolePrimary )
					return _statusOK_;
				else
					return _statusCommunicationFailed_;
			}
			default:
			{
				return _statusCommunicationFailed_;
			}
		}
	}


	void CommunicationSynchronizer::setupStateMachine()
	{
		// Add states
		fsm_->addState(INITIALIZING_STATE,        &CommunicationSynchronizer::processingInitializingState);
		fsm_->addState(CONNECTING_STATE,          &CommunicationSynchronizer::processingConnectingState);
		fsm_->addState(SENDING_HEADER_STATE,      &CommunicationSynchronizer::processingSendingHeaderState);
		fsm_->addState(READING_HEADER_STATE,      &CommunicationSynchronizer::processingReadingHeaderState);
		fsm_->addState(PROCESSING_HEADER_STATE,   &CommunicationSynchronizer::processingProcessingHeaderState);
		fsm_->addState(SENDING_COMMAND_STATE,     &CommunicationSynchronizer::processingSendingCommandState);
		fsm_->addState(READING_COMMAND_STATE,     &CommunicationSynchronizer::processingReadingCommandState);
		fsm_->addState(PROCESSING_COMMAND_STATE,  &CommunicationSynchronizer::processingProcessingCommandState);
		fsm_->addState(SENDING_FOOTER_STATE,      &CommunicationSynchronizer::processingSendingFooterState);
		fsm_->addState(PROCESSING_FOOTER_STATE,   &CommunicationSynchronizer::processingProcessingFooterState);
		fsm_->addState(ERROR_STATE,               &CommunicationSynchronizer::processingErrorState);
		fsm_->addState(DISCONNECTING_STATE,       &CommunicationSynchronizer::processingDisconnectingState);
		fsm_->addState(CONCLUDING_STATE,          &CommunicationSynchronizer::processingConcludingState);

		// Add INITIALIZING_STATE transitions
		fsm_->addTransition(INITIALIZING_STATE, CommunicationSynchronizerEvent::INITIALIZATION_COMPLETE_EVENT, CONNECTING_STATE);

		// Add CONNECTING_STATE transitions
		fsm_->addTransition(CONNECTING_STATE, CommunicationSynchronizerEvent::CONNECTED_EVENT, SENDING_HEADER_STATE);

		// Add SENDING_HEADER_STATE transitions
		fsm_->addTransition(SENDING_HEADER_STATE, CommunicationSynchronizerEvent::HEADER_SENT_EVENT, READING_HEADER_STATE);

		// Add READING_HEADER_STATE transitions
		fsm_->addTransition(READING_HEADER_STATE, CommunicationSynchronizerEvent::HEADER_RECEIVED_EVENT, PROCESSING_HEADER_STATE);

		// Add PROCESSING_HEADER_STATE transitions
		fsm_->addTransition(PROCESSING_HEADER_STATE, CommunicationSynchronizerEvent::HEADER_PROCESSED_EVENT, SENDING_COMMAND_STATE);

		// Add SENDING_COMMAND_STATE transitions
		fsm_->addTransition(SENDING_COMMAND_STATE, CommunicationSynchronizerEvent::COMMAND_SENT_EVENT, READING_COMMAND_STATE);
		fsm_->addTransition(SENDING_COMMAND_STATE, CommunicationSynchronizerEvent::LAST_COMMAND_SENT_EVENT, SENDING_FOOTER_STATE);

		// Add READING_COMMAND_STATE transitions
		fsm_->addTransition(READING_COMMAND_STATE, CommunicationSynchronizerEvent::COMMAND_RECEIVED_EVENT, PROCESSING_COMMAND_STATE);
		fsm_->addTransition(READING_COMMAND_STATE, CommunicationSynchronizerEvent::ERROR_RECEIVED_EVENT, ERROR_STATE);
		fsm_->addTransition(READING_COMMAND_STATE, CommunicationSynchronizerEvent::FOOTER_RECEIVED_EVENT, PROCESSING_FOOTER_STATE);
		fsm_->addTransition(READING_COMMAND_STATE, CommunicationSynchronizerEvent::NOTHING_RECEIVED_EVENT, SENDING_COMMAND_STATE);

		// Add PROCESSING_COMMAND_STATE transitions
		fsm_->addTransition(PROCESSING_COMMAND_STATE, CommunicationSynchronizerEvent::COMMAND_PROCESSED_EVENT, READING_COMMAND_STATE);

		// Add SENDING_FOOTER_STATE transitions
		fsm_->addTransition(SENDING_FOOTER_STATE, CommunicationSynchronizerEvent::FOOTER_SENT_EVENT, READING_COMMAND_STATE);

		// Add PROCESSING_FOOTER_STATE transitions
		fsm_->addTransition(PROCESSING_FOOTER_STATE, CommunicationSynchronizerEvent::FOOTER_PROCESSED_EVENT, DISCONNECTING_STATE);

		// Add ERROR_STATE transitions
		fsm_->addTransition(ERROR_STATE, CommunicationSynchronizerEvent::ERROR_PROCESSED_SEND_FOOTER_EVENT, SENDING_FOOTER_STATE);
		fsm_->addTransition(ERROR_STATE, CommunicationSynchronizerEvent::ERROR_PROCESSED_READ_FOOTER_EVENT, READING_COMMAND_STATE);
		fsm_->addTransition(ERROR_STATE, CommunicationSynchronizerEvent::ERROR_PROCESSED_DISCONNECT_EVENT, DISCONNECTING_STATE);
		fsm_->addTransition(ERROR_STATE, CommunicationSynchronizerEvent::ERROR_PROCESSED_CONCLUDE_EVENT, CONCLUDING_STATE);

		// Add DISCONNECTING_STATE transitions
		fsm_->addTransition(DISCONNECTING_STATE, CommunicationSynchronizerEvent::DISCONNECTED_EVENT, CONCLUDING_STATE);

		// Add CONCLUDING_STATE transitions
		fsm_->addTransition(CONCLUDING_STATE, CommunicationSynchronizerEvent::END_EVENT);

		// Add default transitions
		fsm_->addDefaultTransition(CommunicationSynchronizerEvent::ERROR_EVENT, ERROR_STATE);
	}


	void CommunicationSynchronizer::debug(const std::string& /*category*/, int /*level*/, const std::string& /*message*/) const
	{
	}


	const std::string& CommunicationSynchronizer::getStateName(const stateMachine_type::state_type* state)
	{
		if ( state )
		{
			switch ( state->getId() )
			{
				case INITIALIZING_STATE:          return _statenameInitializing_;
				case CONNECTING_STATE:            return _statenameConnecting_;
				case SENDING_HEADER_STATE:        return _statenameSendingHeader_;
				case READING_HEADER_STATE:        return _statenameReadingHeader_;
				case PROCESSING_HEADER_STATE:     return _statenameProcessingHeader_;
				case SENDING_COMMAND_STATE:       return _statenameSendingCommand_;
				case READING_COMMAND_STATE:       return _statenameReadingCommand_;
				case PROCESSING_COMMAND_STATE:    return _statenameProcessingCommand_;
				case SENDING_FOOTER_STATE:        return _statenameSendingFooter_;
				case PROCESSING_FOOTER_STATE:     return _statenameProcessingFooter_;
				case ERROR_STATE:                 return _statenameError_;
				case DISCONNECTING_STATE:         return _statenameDisconnecting_;
				case CONCLUDING_STATE:            return _statenameConcluding_;
			}
		}

		return _statenameUnknown_;
	}


}
