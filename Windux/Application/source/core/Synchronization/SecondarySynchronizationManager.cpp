#include "SecondarySynchronizationManager.h"


#include "SynchronizationManagerProxy.h"
#include "QueueManager.h"
#include "CommunicationSynchronizer.h"
#include "DatabaseSynchronizer.h"

#include "ScaleManager.h"
#include "BatchManager.h"
#include "SynchronizationChangeManager.h"
#include "SynchronizationChange.h"


#include "TimerManager.h"

#include "SessionManager.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/StartSyncManagerMessage.h"
#include "Messages/StartSyncManagerProcessedMessage.h"
#include "Messages/ComingUpOnLineMessage.h"
#include "Messages/GoingOffLineMessage.h"
#include "Messages/RequestRegistrationProcessedMessage.h"
#include "Messages/ProcessUserChangesMessage.h"
#include "Messages/ProcessUserChangesProcessedMessage.h"
#include "Messages/ProcessHQBatchMessage.h"
#include "Messages/ProcessHQBatchProcessedMessage.h"
#include "Messages/NotifyLocalBatchesLazilyMessage.h"
#include "Messages/ProcessUserFileMessage.h"
#include "Messages/ProcessUserFileProcessedMessage.h"
#include "Messages/NotifyOnlineProcessedMessage.h"
#include "Messages/ProcessRemoteBatchMessage.h"
#include "Messages/ProcessRemoteBatchProcessedMessage.h"
#include "Messages/ProcessSynchronizedBatchMessage.h"
#include "Messages/ProcessSynchronizedBatchProcessedMessage.h"
#include "Messages/ProcessScaleOnlineMessage.h"
#include "Messages/CreateBatchMessage.h"
#include "Messages/AddRecordToBatchMessage.h"
#include "Messages/AbortBatchMessage.h"
#include "Messages/CompleteBatchMessage.h"
#include "Messages/ApplyBatchProcessedMessage.h"
#include "Messages/DeleteBatchMessage.h"
#include "Messages/DeleteBatchProcessedMessage.h"
#include "Messages/CompleteCommandMessage.h"
#include "Messages/RetryCommandMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"
#include "SynchronizationStatus.h"

#include "Commands/RequestRegistrationCommand.h"
#include "Commands/NotifyLocalBatchesPresentCommand.h"
#include "Commands/NotifyOnlineCommand.h"
#include "Commands/ApplyBatchCommand.h"
#include "Commands/DeleteBatchCommand.h"
#include "Commands/ApplyAndDeleteBatchCommand.h"


#include "Configuration.h"

#include "CMReaderWriterSyncTag.h"
#include "CMReaderWriterUltimaDisk.h"
#include "HobIpAddress.h"


#include "MSGEvent.h"

#include "util/e_string/utilstring.h"
#include "util/thread/lock.h"
#include "util/time/dbtime.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int SecondarySynchronizationManager::_dataType_ = 0;
	const string SecondarySynchronizationManager::_dataTypeName_("synchronization::SecondarySynchronizationManager");
#endif


	SecondarySynchronizationManager::SecondarySynchronizationManager(bool online, bool registration,
			const std::string& primaryScale)
		: SynchronizationManager(ScaleSynchronizationTypeSecondary, "SecondarySynchronizationManager")
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif


		msgManager_->RestartThread();
			
		notifyLocalBatchesTimerId_ = 0;

		/* Trigger the execution of "Use Case 5.2: Starting up Synchronization Manager". */
		Util::CMSGEvent startEvent;
		StartSyncManagerMessage start(online, registration, primaryScale, startEvent);
		msgManager_->dispatchEvent(&start, msgManager_);

		/* The Synchronization Manager's constructor does not return until "Use Case 5.2: Starting
		up Synchronization Manager" completes. */
#ifndef EVOL_NDEBUG
		util_time::Time t0 = util_time::Time();
#endif

		startEvent.Wait();

#ifndef EVOL_NDEBUG
		util_time::Time t1 = util_time::Time();
		Debug::trace("SecondarySynchronizationManager", "Waited " + 
				util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds in constructor");
#endif

		if ( !initialized_ )
        {
        #if defined(LINUX_FIX)
			throw Util::CPlatformException();
        #else
			throw Util::CNTException();
        #endif
        }
	}


	SecondarySynchronizationManager::~SecondarySynchronizationManager()
	{
#ifndef EVOL_NDEBUG
		util_time::Time t0 = util_time::Time();
		Debug::trace("SecondarySynchronizationManager", "Going to stop the proxy");
#endif

		/* Make proxy unavailable and unusable. */
		Util::CSmartPtr<SynchronizationManagerProxy> proxy = proxy_;
		proxy_ = 0;
		proxy->stopProcessing();
		/* By setting proxy_ to 0 we prevent new uses of the proxy. However, any pending requests may
		be completed by the SynchronizationManager and responses posted back to the proxy as long as
		the latter is still processing message. As soon as we return from msgManager_->ShutdownNicely
		no new requests will be completed and responses posted back to the proxy. Thus, we need to
		keep the proxy object alive until that moment so its message queue will be there if needed
		(there is no harm in processing any response since they will not find a match on a waiting
		thread). */

#ifndef EVOL_NDEBUG
		util_time::Time t1;
		Debug::trace("SecondarySynchronizationManager", "Synchronizing with proxy took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " msecs.");
#endif

		msgManager_->deregisterAllEvents();
		msgManager_->ShutdownNicely();


#ifndef EVOL_NDEBUG
		util_time::Time t2;
		Debug::trace("SecondarySynchronizationManager", "Synchronizing with processEvent thread took " +
			util_string::inttostring((t2 - t1).getTotalMilliseconds()) + " msecs.");
#endif

		/* Execute "Use Case 5.6: Stopping Synchronization Manager". */
		usecaseStoppingSynchronizationManager();

#ifndef EVOL_NDEBUG
		util_time::Time t3;
		Debug::trace("SecondarySynchronizationManager", "Stopping the Synchronization mechanism took " +
			util_string::inttostring((t3 - t2).getTotalMilliseconds()) + " msecs.");
#endif

		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}

	void SecondarySynchronizationManager::processEvent(Util::CSmartPtr<EvolMessageWrapper>& msg)
	{
		if ( msg.notNull() )
		{
			UINT32 result = SessionManager::getInstance()->requestToContinue(msgManager_->GetThreadID(), msgManager_->IsASuperThread());
			switch ( result & (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED) )
			{
				case SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED:
				{
					util_time::Time t0;

					HFCManagerContainer::init();
					DBAccess::uninitDB();
					DBAccess::initDB();

					util_time::Time t1;
					Debug::trace("SecondarySynchronizationManager", "After compressing DB, its reinitialization took " +
						util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " msecs.");

					break;
				}
				default:
				{
					break;
				}
			}

			switch ( msg->getDeviceType() )
			{
				case TimerDev:
				{
					if ( msg->getEventType() == TimerExpired )
					{
						Timeout timeout( msg->Data() );
						if ( timeout.getTimerId() == notifyLocalBatchesTimerId_ )
						{
							TimerManager::getInstance()->removeTimer(notifyLocalBatchesTimerId_);
							notifyLocalBatchesTimerId_ = 0;

							usecaseNotifyingLocalBatchesLazily2();
						}
						else
						{
							Timer* timer = TimerManager::getInstance()->getTimer(timeout.getTimerId());
							if ( timer )
							{
								switch ( timer->getTimerPurpose() )
								{
									case timedFuncRetryCommand:
									{
										RetryTimer* retryTimer = dynamic_cast<RetryTimer*>(timer);
										if ( retryTimer )
										{
											QueueManager::QueueType queue = retryTimer->getQueue();
											Command* command = retryTimer->getCommand();
											if ( command->getRetryTimerId() == timeout.getTimerId() )
											{
												TimerManager::getInstance()->removeTimer(timeout.getTimerId());
												command->setRetryTimerId(0);
												usecaseRetryingCommand2(queue, command);
											}
											else
												Debug::trace("SecondarySynchronizationManager", "Got multiple timeouts for command " +
													util_string::inttostring(command->getSequenceNumber()));
										}
										else
											TimerManager::getInstance()->removeTimer(timeout.getTimerId());

										break;
									}
									default:
									{
										break;
									}
								}
							}
						}
					}

					break;
				}
				case SynchronizationDev:
				{
					processSynchronizationEvent(msg);

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

			SessionManager::getInstance()->changeBlockabilityStatus(msgManager_->GetThreadID(), true, msgManager_->IsASuperThread());

		}
	}


	void SecondarySynchronizationManager::processSynchronizationEvent(Util::CSmartPtr<EvolMessageWrapper>& msg)
	{
		Util::CSmartPtr<EvolMessage> response;

		switch ( msg->getEventType() )
		{
			case StartSyncManagerEvent:
			{
				response = processStartSyncManagerMessage(Util::CSmartPtr<StartSyncManagerMessage>(new StartSyncManagerMessage(msg->Data())));

				break;
			}
			case ComingUpOnLineEvent:                      /* Use Case 5.3 */
			{
				response = processComingUpOnLineMessage(Util::CSmartPtr<ComingUpOnLineMessage>(new ComingUpOnLineMessage(msg->Data())));

				break;
			}
			case RequestRegistrationProcessedEvent:
			{
				response = processComingUpOnLine2Message(Util::CSmartPtr<ProcessedMessage>(new RequestRegistrationProcessedMessage(msg->Data())));

				break;
			}
			case NotifyOnlineProcessedEvent:
			{
				response = processComingUpOnLine2Message(Util::CSmartPtr<ProcessedMessage>(new NotifyOnlineProcessedMessage(msg->Data())));

				break;
			}
			case GoingOffLineEvent:                        /* Use Case 5.5 */
			{
				response = processGoingOffLineMessage(Util::CSmartPtr<GoingOffLineMessage>(new GoingOffLineMessage(msg->Data())));

				break;
			}
			case ProcessUserChangesEvent:                  /* Use Case 5.20 */
			{
				response = processProcessUserChangesMessage(Util::CSmartPtr<ProcessUserChangesMessage>(new ProcessUserChangesMessage(msg->Data())));

				break;
			}
			case ProcessHQBatchEvent:                      /* Use Case 5.21 */
			{
				response = processProcessHQBatchMessage(Util::CSmartPtr<ProcessHQBatchMessage>(new ProcessHQBatchMessage(msg->Data())));

				break;
			}
			case NotifyLocalBatchesLazilyEvent:            /* Use Case 5.27 */
			{
				response = processNotifyLocalBatchesLazilyMessage(Util::CSmartPtr<NotifyLocalBatchesLazilyMessage>(new NotifyLocalBatchesLazilyMessage(msg->Data())));

				break;
			}
			case ProcessUserFileEvent:                     /* Use Case 5.28 */
			{
				response = processProcessUserFileMessage(Util::CSmartPtr<ProcessUserFileMessage>(new ProcessUserFileMessage(msg->Data())));

				break;
			}
			case ProcessRemoteBatchEvent:                  /* Use Case 5.36 */
			{
				response = processProcessRemoteBatchMessage(Util::CSmartPtr<ProcessRemoteBatchMessage>(new ProcessRemoteBatchMessage(msg->Data())));

				break;
			}
			case ProcessSynchronizedBatchEvent:            /* Use Case 5.37 */
			{
				response = processProcessSynchronizedBatchMessage(Util::CSmartPtr<ProcessSynchronizedBatchMessage>(new ProcessSynchronizedBatchMessage(msg->Data())));

				break;
			}
			case ProcessScaleOnlineEvent:
			{
				response = processProcessScaleOnlineMessage(Util::CSmartPtr<ProcessScaleOnlineMessage>(new ProcessScaleOnlineMessage(msg->Data())));

				break;
			}
			case CreateBatchEvent:                         /* Use Case 5.40 */
			{
				response = processCreateBatchMessage(Util::CSmartPtr<CreateBatchMessage>(new CreateBatchMessage(msg->Data())));

				break;
			}
			case AddRecordToBatchEvent:                    /* Use Case 5.41 */
			{
				response = processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage>(new AddRecordToBatchMessage(msg->Data())));

				break;
			}
			case AbortBatchEvent:                          /* Use Case 5.42 */
			{
				response = processAbortBatchMessage(Util::CSmartPtr<AbortBatchMessage>(new AbortBatchMessage(msg->Data())));

				break;
			}
			case CompleteBatchEvent:                       /* Use Case 5.44 */
			{
				response = processCompleteBatchMessage(Util::CSmartPtr<CompleteBatchMessage>(new CompleteBatchMessage(msg->Data())));

				break;
			}
			case ApplyBatchProcessedEvent:                 /* Use Case 5.45 */
			{
				response = processApplyBatch2Message(Util::CSmartPtr<ApplyBatchProcessedMessage>(new ApplyBatchProcessedMessage(msg->Data())));

				break;
			}
			case DeleteBatchEvent:                        /* Use Case 5.46 */
			{
				response = processDeleteBatchMessage(Util::CSmartPtr<DeleteBatchMessage>(new DeleteBatchMessage(msg->Data())));

				break;
			}
			case DeleteBatchProcessedEvent:                /* Use Case 5.46 */
			{
				response = processDeleteBatch2Message(Util::CSmartPtr<DeleteBatchProcessedMessage>(new DeleteBatchProcessedMessage(msg->Data())));

				break;
			}
			case CompleteCommandEvent:                     /* Use Case 5.55 */
			{
				response = processCompleteCommandMessage(Util::CSmartPtr<CompleteCommandMessage>(new CompleteCommandMessage(msg->Data())));

				break;
			}
			case RetryCommandEvent:                        /* Use Case 5.56 */
			{
				response = processRetryCommandMessage(Util::CSmartPtr<RetryCommandMessage>(new RetryCommandMessage(msg->Data())));

				break;
			}
			case NewSynchronizationStatusEvent:
			{
				NewSynchronizationStatusMessage message(msg->Data());
				setDisplayStatus(message.getStatus());

				Debug::trace("SecondarySynchronizationManager", "Received new synchronization status of " +
					util_string::inttostring(message.getStatus()));

				/* Fall through to process next case. */
			}
			case RequestSynchronizationStatusEvent:
			{
				SynchronizationDevError sendingDisplayStatus = filterDisplayStatus();

				Debug::trace("SecondarySynchronizationManager", "Sending synchronization status of " +
					util_string::inttostring(sendingDisplayStatus) + " (actual status is " +
					util_string::inttostring(getDisplayStatus()) + ")");

				SynchronizationStatus message(sendingDisplayStatus);
				msgManager_->dispatchEvent(&message);

				break;
			}
			default:
			{
				Debug::trace("SecondarySynchronizationManager", "Received unknown event: device=" +
					util_string::inttostring(msg->getDeviceType()) + " event=" + util_string::inttostring(msg->getEventType()));

				break;
			}
		}

		if ( response.notNull() )
			dispatchResponseMessage(msg, response);
	}

		
	/* "Use Case 5.2: Starting up Synchronization Manager". */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processStartSyncManagerMessage(Util::CSmartPtr<StartSyncManagerMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseStartingUpSynchronizationManager(msg->isOnLine(), msg->isRegistration(), msg->getPrimaryScale());

			if ( status == _statusOK_ )
				initialized_ = true;
			else
				Debug::trace("SecondarySynchronizationManager", "!!!!!usecaseStartingUpSynchronizationManager failed!!!!!");

			msg->signalEvent();


			/* (uc5.2) Return the Start Synchronization Manager processed message. */
			return Util::CSmartPtr<EvolMessage>(new StartSyncManagerProcessedMessage(status));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.2: Starting up Synchronization Manager". */
	int SecondarySynchronizationManager::usecaseStartingUpSynchronizationManager(bool online, bool registration, const std::string& primaryScale)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Starting up Synchronization Manager: "
				"Starting (online = " + util_string::booltostring(online) +
				", registration = " + util_string::booltostring(registration) + ")");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* Register for any necessary events. */
			msgManager_->registerEvent(SynchronizationDev, ComingUpOnLineEvent);
			msgManager_->registerEvent(SynchronizationDev, GoingOffLineEvent);
			msgManager_->registerEvent(SynchronizationDev, RequestRegistrationProcessedEvent);
			msgManager_->registerEvent(SynchronizationDev, NotifyOnlineProcessedEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessUserChangesEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessHQBatchEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessUserFileEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessRemoteBatchEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessScaleOnlineEvent);
			msgManager_->registerEvent(SynchronizationDev, DeleteBatchEvent);
			msgManager_->registerEvent(SynchronizationDev, ApplyBatchProcessedEvent);
			msgManager_->registerEvent(SynchronizationDev, DeleteBatchProcessedEvent);
			msgManager_->registerEvent(SynchronizationDev, RequestSynchronizationStatusEvent);
			msgManager_->registerEvent(SynchronizationDev, NewSynchronizationStatusEvent);


			/* Initialize Synchronization Reader Writer. */
			CMReaderWriterSyncTag();
			CMReaderWriterUltimaDisk();


			/* (uc5.2) Initialize the current user batch identifier to -1. */
			/* [This happens in the constructor of the base class.] */


			/* (uc5.2) If there is no scale record in the database associated with this scale,
			then create one. */
			verifyThisScaleRecord(online);


			/* (uc5.2) Using the Primary scale's host name/IP address information received as a
			parameter (or otherwise using the one from configuration), check whether there is a
			scale record for the Primary scale. If not, then create a scale record for the
			Primary scale. */
			if ( primaryScale.size() )
			{
				if ( !verifyPrimaryScaleRecord(primaryScale) )
					{ status = _statusPrimaryScaleChanged_; break; }
			}
			else
			{
				if ( !verifyPrimaryScaleRecord(Configuration::getInstance()->toStringCfg(fieldOffsetCfgPrimaryScale)) )
					{ status = _statusPrimaryScaleChanged_; break; }
			}


			/* (uc5.2) Execute "Use Case 5.47: Cleaning up batches" to ensure the database is in
			a stable state. */
			status = usecaseCleaningUpBatches();
			if ( status != _statusOK_ ) break;


			/* (uc5.2) Initialize Queue Managers. */
			// We pass the message receiver with the only purpose of enabling the queue managers to
			// post messages back to the Synchronization Manager.
			commQManager_ = new QueueManager(QueueManager::CommunicationQueue, getMessageReceiver(),
					new CommunicationSynchronizer(getMessageReceiver(), hfc::Scale::RoleSecondary));
			dbQManager_ = new QueueManager(QueueManager::DatabaseQueue, getMessageReceiver(),
					new DatabaseSynchronizer(getMessageReceiver()));


			if ( registration )
			{
				Util::CSmartPtr<hfc::Scale> thisScale(getScaleManager()->getThisScale());
				thisScale->setRegistering();
				getScaleManager()->saveScale(thisScale.get());
			}


			/* (uc5.2) If the scale is off-line, then trigger the execution of "Use Case 5.5: Going
			off-lline". Otherwise, trigger the execution of "Use Case 5.3: Coming up on-line", passing
			the register optional parameter if received. */
			if ( online )
			{
				ComingUpOnLineMessage message(registration);

				msgManager_->dispatchEvent(&message, msgManager_);
			}
			else
			{
				GoingOffLineMessage message;

				msgManager_->dispatchEvent(&message, msgManager_);
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Starting up Synchronization Manager: "
					"completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Starting up Synchronization Manager: "
					"failed (status = " + getStatusText(status) + ")");
#endif


		return status;
	}


	/* "Use Case 5.3: Coming up on-line" */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processComingUpOnLineMessage(Util::CSmartPtr<ComingUpOnLineMessage> msg)
	{
		if ( msg.notNull() )
		{
			usecaseComingUpOnLine(msg->isRegister());

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.3: Coming up on-line" */
	int SecondarySynchronizationManager::usecaseComingUpOnLine(bool registration)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		HobDateTime now;
		Debug::trace("SecondarySynchronizationManager", "Use case Coming up on-line (need to register = " +
				util_string::booltostring(registration) + ", down time " +
				util_string::inttostring((int)now.timeDiff(fullyFunctioningChangedDateTime_)) + " secs.): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.3) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			scaleOnline_ = true;
			/* Turning fullyFunctioning_ on here implies that the Secondary may attempt to talk to
			the Primary even if the online notification failed. */
			fullyFunctioning_ = true;


			/* (uc5.3) If the "register" parameter was passed in, then perform the following steps: */
			Util::CSmartPtr<hfc::Scale> thisScale(getScaleManager()->getThisScale());
			if ( registration || thisScale->isRegistering() )
			{
				if ( !thisScale->isRegistering() )
				{
					thisScale->setRegistering();
					getScaleManager()->saveScale(thisScale.get());
				}


				/* (uc5.3) Execute "Use Case 5.53: Queuing Command", passing "communication" and the
				Request registration command, containing the Request registration processed completion
				message. */
				Util::CSmartPtr<hfc::Scale> primary(getScaleManager()->getPrimaryScale());
				if ( primary.notNull() )
				{
					status = usecaseQueuingCommand(QueueManager::CommunicationQueue, Util::CSmartPtr<Command>(new
							RequestRegistrationCommand(primary->getIdentifier(), new RequestRegistrationProcessedMessage())));
				}


				/* (uc5.3) Wait for the Request Registration processed message. */
			}
			else
			{
				/* (uc5.3) Otherwise, perform the following steps: */

				Util::CSmartPtr<hfc::Scale> primary(getScaleManager()->getPrimaryScale());
				if ( primary.notNull() )
				{
					/* If the Primary scale is marked as Off-line, then mark it as On-line.
					[Rationale: we try the Primary scale since it may now be available.] */
					if ( !primary->isStateActive() )
					{
						primary->setStateActive();
						getScaleManager()->saveScale(primary.get());
					}

						
					/* (uc5.3) Execute "Use Case 5.53: Queuing Command", passing "communication" and the
					Notify On-line command, including the Notify on-line processed completion message. */
					status = usecaseQueuingCommand(QueueManager::CommunicationQueue, Util::CSmartPtr<Command>(new
							NotifyOnlineCommand(primary->getIdentifier(), new NotifyOnlineProcessedMessage())));
				}


				/* (uc5.3) Wait for the Nofity On-line processed message. */
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Coming up on-line (registration = " +
					util_string::booltostring(registration) + "): completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Coming up on-line (registration = " +
					util_string::booltostring(registration) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.3: Coming up on-line". */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processComingUpOnLine2Message(Util::CSmartPtr<ProcessedMessage> msg)
	{
		if ( msg.notNull() )
		{
			usecaseComingUpOnLine2(msg->getStatus());

			return Util::CSmartPtr<EvolMessage>(0);
		}

		return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.3: Coming up on-line". */
	int SecondarySynchronizationManager::usecaseComingUpOnLine2(int result)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Coming up on-line 2 (result = " +
			util_string::inttostring(result) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.3) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.3) Set the Fully-functioning flag. */
			if ( result == 0 ) // success registering/notifying online
			{
				fullyFunctioning_ = true;


				/* (SYNCSTATUS) notify UI that we are fully functioning. */
				NewSynchronizationStatusMessage message(SynchronizationActiveSyncStatus);
				msgManager_->dispatchEvent(&message);


				/* (uc5.3) If there are completed batches marked as Pending synchronization, then execute
				"Use Case 5.53: Queuing Command", passing "communication" and the Notify local batches
				command. */
				/* [This is not necessary since it was already done at the same time as the Notify On-line
				was sent to the Primary scale (if we did not notify on-line, i.e. we registered with the
				Primary scale, we should not have pending synchronization local batches to send).] */
			}
			else
			{
				Debug::trace("SecondarySynchronizationManager", "Unable to register/notify online with Primary scale. "
					"The scale is not fully-functioning yet");
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Coming up on-line 2 (result = " +
					util_string::inttostring(result) + "): completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Coming up on-line 2 (result = " +
					util_string::inttostring(result) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.20: Processing user changes" */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processProcessUserChangesMessage(Util::CSmartPtr<ProcessUserChangesMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseProcessingUserChanges(msg->getUserChange());


			return Util::CSmartPtr<EvolMessage>(new ProcessUserChangesProcessedMessage(status));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.20: Processing user changes" */
	int SecondarySynchronizationManager::usecaseProcessingUserChanges(Util::CSmartPtr<hfc::SynchronizationChange> userChange)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Processing User Changes: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.20) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }

			
			/* (uc5.20) If there is no currrent user batch, then execute "Use Case 5.40: Creating
			batch", passing the parameters "local" and "user batch", and receiving a new batch
			identifier. Make the new batch the current user batch. */
			{
				util_thread::Lock< util_thread::Mutex<> > lock(currentUserBatchIntegrityMutex_);

				int currentUserBatchNumber = getCurrentUserBatchNumber();
				if ( currentUserBatchNumber == hfc::Batch::_invalidBatchNumber_ )
				{
					status = usecaseCreatingBatch(currentUserBatchNumber, hfc::Batch::OriginLocal, hfc::Batch::TypeUser);
					if ( status != _statusOK_ ) break;

					setCurrentUserBatchNumber(currentUserBatchNumber);
				}


				/* (uc5.20) Execute "Use Case 5.41: Adding record to batch", passing the current user
				batch identifier and the change record. */
				status = usecaseAddingRecordToBatch(currentUserBatchNumber, userChange);
				if ( status != _statusOK_ ) break;
			}


			/* (uc5.20) Trigger the execution of "Use Case 5.27: Notifyting local batches lazily",
			in order to notify the Primary scale of the presence of user changes. */
			NotifyLocalBatchesLazilyMessage message;
			msgManager_->dispatchEvent(&message, msgManager_);
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Processing User Changes: completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Processing User Changes: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.21: Processing HQ batch". */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processProcessHQBatchMessage(Util::CSmartPtr<ProcessHQBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseProcessingHQBatchMessage(msg->getBatchNumber());


			/* (uc5.21) Return the Process HQ batch processed message. */
			return Util::CSmartPtr<EvolMessage>(new ProcessHQBatchProcessedMessage(status, msg->getBatchNumber()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.21: Processing HQ batch". */
	int SecondarySynchronizationManager::usecaseProcessingHQBatchMessage(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Processing HQ Batch: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.21) Retrieve the batch record from the database using the identifier passed in. */
			Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }
			if ( batch->getOrigin() != hfc::Batch::OriginLocal || batch->getType() != hfc::Batch::TypeHQ )
				{ status = _statusWrongTypeOfBatch_; break; }


			/* (uc5.21) If the batch is empty (no change records associated to it), then delete the
			batch record from the database. End the execution of this use case. */
			if ( !getSynchronizationChangeManager()->initializeBatchChangesQuery(batchNumber) )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Deleting the batch (batchNumber = " +
					util_string::inttostring(batchNumber) + ") because it is empty.");
#endif

				getBatchManager()->deleteBatch(batch.get());

				break;
			}


			/* (uc5.21) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.21) If this scale has already received the same HQ batch from a Secondary scale
			(there is another remote HQ batch with the same signature), then perform the following
			steps: */
			if ( isHQBatchAlreadyReceived(batch) )
			{
				Debug::trace("SecondarySynchronizationManager", "Use case Processing HQ Batch (batchNumber = " +
						util_string::inttostring(batchNumber) + "): this HQ has already been received. Deleting it.");


				/* (uc5.21) Execution "Use Case 5.46: Deleting batch", passing the batch identifier
				and "delete batch record". */
				status = usecaseDeletingBatch(batchNumber, DeleteBatchProcessedMessage::DeleteChangesAndBatch);


				/* (uc5.21) End the execution of this use case. */
				break;
			}


			/* (uc5.21) Execute "Use Case 5.43: Completing batch", passing the batch identifier. */
			status = usecaseCompletingBatch(batchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.21) Execute "Use Case 5.45: Applying batch", passing the batch identifier. */
			status = usecaseApplyingBatch(batchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.21) Mark the batch as Pending synchronization. */
			/* Refresh the batch from the database. */
			batch = getBatchManager()->findBatchNumber(batch->getNumber());
			batch->setPendingSynchronization();
			getBatchManager()->saveBatch(batch);

				
			/* (uc5.21) If the Fully-functioning flag is set, then execute "Use Case 5.53: Queuing
			Command", passing "communication" and the Notify local batches present command,
			containing the batch identifier. */
			if ( fullyFunctioning_ && scaleOnline_ )
			{
				Util::CSmartPtr<hfc::Scale> primary(getScaleManager()->getPrimaryScale());
				if ( primary.notNull() )
				{
					status = usecaseQueuingCommand(QueueManager::CommunicationQueue,
							Util::CSmartPtr<Command>(new NotifyLocalBatchesPresentCommand(primary->getIdentifier())));
					if ( status != _statusOK_ ) break;
				}
				else
				{ status = _statusCannotFindPrimaryScale_; break; }
			}
			else
			{
				Debug::trace("SecondarySynchronizationManager", "Use case Processing User File (batchNumber = " +
					util_string::inttostring(batchNumber) + "): not attempting to Notify Primary scale since Synchronization Manager is not online.");
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Processing HQ Batch: completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Processing HQ Batch: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.27: Notifying local batches lazily". */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processNotifyLocalBatchesLazilyMessage(Util::CSmartPtr<NotifyLocalBatchesLazilyMessage> msg)
	{
		if ( msg.notNull() )
		{
			/*int status =*/ usecaseNotifyingLocalBatchesLazily();

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.27: Notifying local batches lazily". */
	int SecondarySynchronizationManager::usecaseNotifyingLocalBatchesLazily()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Notifying Local Batches lazily: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- != 0) )
    #endif
		{
			/* (uc5.27) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.27) If the Notify local batches timer (a 2min. timer) is not running, then start
			it now keeping track of the start time. Otherwise, if it has been running for less than
			a specific amount of time (about 15 min.) since the start time, then restart the timer
			now. */
			TimerManager* timerMgr = TimerManager::getInstance();
			if ( notifyLocalBatchesTimerId_ == 0 )
			{
				notifyLocalBatchesTimerStarted_ = HobDateTime();
				notifyLocalBatchesTimerId_ = timerMgr->addTimer(getMessageReceiver(), Configuration::getInstance()->toIntCfgI(CfgInternal::NOTIFYLOCALBATCHESTIMER),
					Timer::RepeatNO, timedFuncNotifyLocalBatchesTimer);
			}
			else
			{
				HobDateTime nowTime;
				nowTime.incrementMinutes((int)-15);
				if ( nowTime < notifyLocalBatchesTimerStarted_ )
				{
					timerMgr->removeTimer(notifyLocalBatchesTimerId_);
					notifyLocalBatchesTimerId_ = timerMgr->addTimer(getMessageReceiver(), Configuration::getInstance()->toIntCfgI(CfgInternal::NOTIFYLOCALBATCHESTIMER),
						Timer::RepeatNO, timedFuncNotifyLocalBatchesTimer);
				}
			}


			/* (uc5.27) Wait until the Notify local batches timer times out. */
			/* (nothing to do here, simply let the timer expire) */
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Notifying Local Batches lazily: completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Notifying Local Batches lazily: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.27: Notifying local batches lazily". */
	int SecondarySynchronizationManager::usecaseNotifyingLocalBatchesLazily2()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Notifying Local Batches lazily 2: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			int userBatchNumber = hfc::Batch::_invalidBatchNumber_;


			/* (uc5.27) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.27) Execute "Use Case 5.44: Completing current user batch". */
			status = usecaseCompletingCurrentUserBatch(userBatchNumber);
			if ( status != _statusOK_ ) break;
			if ( userBatchNumber != hfc::Batch::_invalidBatchNumber_ )
			{
				/* (uc5.27) Mark the batch as Pending Synchronization. */
				Util::CSmartPtr<hfc::Batch> userBatch(getBatchManager()->findBatchNumber(userBatchNumber));
				userBatch->setPendingSynchronization();
				getBatchManager()->saveBatch(userBatch.get());


				/* (uc5.27) If the Fully-functioning flag is set, then execute "Use Case 5.53: Queuing
				command", passing "communication" and the Notify local batches present command. */
				if ( fullyFunctioning_ && scaleOnline_ )
				{
					Util::CSmartPtr<hfc::Scale> primary(getScaleManager()->getPrimaryScale());
					if ( primary.notNull() )
					{
						status = usecaseQueuingCommand(QueueManager::CommunicationQueue,
								Util::CSmartPtr<Command>(new NotifyLocalBatchesPresentCommand(primary->getIdentifier())));
						if ( status != _statusOK_ ) break;
					}
				}
				else
				{
					Debug::trace("SecondarySynchronizationManager", "Use case Notifying Local Batches lazily 2: "
						"not attempting to Notify Primary scale since Synchronization Manager is not online.");
				}
			}
			else
			{
				Debug::trace("SecondarySynchronizationManager", "Use case Notifying Local Batches lazily 2: "
					"not attempting to Notify Primary scale since there is no current user batch.");
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Notifying Local Batches lazily 2: completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Notifying Local Batches lazily 2: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.28: Processing user file". */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processProcessUserFileMessage(Util::CSmartPtr<ProcessUserFileMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseProcessingUserFile(msg->getBatchId());

			return Util::CSmartPtr<EvolMessage>(new ProcessUserFileProcessedMessage(status, msg->getBatchId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.28: Processing user file". */
	int SecondarySynchronizationManager::usecaseProcessingUserFile(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Processing User File (batchNumber = " +
				util_string::inttostring(batchNumber) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.28) Retrieve the batch record from the database using the identifier passed in. */
			Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }


			/* (uc5.28) If the batch is empty (no change records associated to it), then delete the
			batch record from the database. End the execution of this use case. */
			if ( !getSynchronizationChangeManager()->initializeBatchChangesQuery(batchNumber) )
			{
				Debug::trace("SecondarySynchronizationManager", "Deleting the batch (batchNumber = " +
					util_string::inttostring(batchNumber) + ") because it is empty.");

				getBatchManager()->deleteBatch(batch.get());

				break;
			}


			/* (uc5.28) Execute "Use Case 5.44: Completing current user batch". */
			int userBatchNumber = hfc::Batch::_invalidBatchNumber_;
			int status = usecaseCompletingCurrentUserBatch(userBatchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.28) Execute "Use Case 5.43: Completing batch", passing the batch identifier. */
			status = usecaseCompletingBatch(batchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.28) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.28) Execute "Use Case 5.45: Applying batch", passing the batch identifier. */
			status = usecaseApplyingBatch(batchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.28) Mark the batch as Pending Synchronization. */
			/* Refresh the batch */
			batch = getBatchManager()->findBatchNumber(batchNumber);
			batch->setPendingSynchronization();
			getBatchManager()->saveBatch(batch);


			/* (uc5.28) If the Fully-functioning flag is set, then execute "Use Case 5.53: Queuing
			Command", passing "communication" and the Notify local batches present command,
			containing the batch identifier. */
			if ( fullyFunctioning_ && scaleOnline_ )
			{
				Util::CSmartPtr<hfc::Scale> primary(getScaleManager()->getPrimaryScale());
				if ( primary.notNull() )
				{
					status = usecaseQueuingCommand(QueueManager::CommunicationQueue,
							Util::CSmartPtr<Command>(new NotifyLocalBatchesPresentCommand(primary->getIdentifier())));
					if ( status != _statusOK_ ) break;
				}
				else
				{ status = _statusCannotFindPrimaryScale_; break; }
			}
			else
			{
				Debug::trace("SecondarySynchronizationManager", "Use case Processing User File (batchNumber = " +
					util_string::inttostring(batchNumber) + "): not attempting to Notify Primary scale since Synchronization Manager is not online.");
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Processing User File (batchNumber = " +
					util_string::inttostring(batchNumber) + ") completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Processing User File (batchNumber = " +
					util_string::inttostring(batchNumber) + ") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.36: Processing remote batch". */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processProcessRemoteBatchMessage(Util::CSmartPtr<ProcessRemoteBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseProcessingRemoteBatch(msg->getBatchId());

			/* (uc5.36) Return the Process remote batch processed message. */
			return Util::CSmartPtr<EvolMessage>(new ProcessRemoteBatchProcessedMessage(status, msg->getBatchId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.36: Processing remote batch". */
	int SecondarySynchronizationManager::usecaseProcessingRemoteBatch(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
				util_string::inttostring(batchNumber) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.36) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.36) Retrieve the batch record from the database using the identifier passed in. */
			Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }


			/* (uc5.36) If this batch is an HQ batch and the same batch was already received, then
			perform the following steps: */
			if ( batch->getType() == hfc::Batch::TypeHQ && isHQBatchAlreadyReceived(batch) )
			{
				Debug::trace("SecondarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
						util_string::inttostring(batchNumber) + "): this HQ has already been received. Deleting it.");


				/* (uc5.36) Execute "Use Case 5.46: Deleting batch", passing the batch identifier
				and "delete batch record". */
				status = usecaseDeletingBatch(batchNumber, DeleteBatchProcessedMessage::DeleteChangesAndBatch);
				if ( status != _statusOK_ ) break;


				/* (uc5.36) End the execution of this use case. */
				break;
			}


			/* (uc5.36) Execute "Use Case 5.43: Completing batch", passing the batch identifier. */
			status = usecaseCompletingBatch(batchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.36) Execute "Use Case 5.45: Applying batch", passing the batch identifier. */
			status = usecaseApplyingBatch(batchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.36) Execute "Use Case 5.46: Deleting batch", passing the batch identifier. */
			status = usecaseDeletingBatch(batchNumber,
					batch->getType() == hfc::Batch::TypeHQ ? DeleteBatchProcessedMessage::DeleteChangesOnly : DeleteBatchProcessedMessage::DeleteChangesAndBatch);
			if ( status != _statusOK_ ) break;
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.37: Processing synchronized batch". */
	Util::CSmartPtr<EvolMessage> SecondarySynchronizationManager::processProcessSynchronizedBatchMessage(Util::CSmartPtr<ProcessSynchronizedBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseProcessingSynchronizedBatch(msg->getBatchId());

			return Util::CSmartPtr<EvolMessage>(new ProcessSynchronizedBatchProcessedMessage(status, msg->getBatchId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.37: Processing synchronized batch". */
	int SecondarySynchronizationManager::usecaseProcessingSynchronizedBatch(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SecondarySynchronizationManager", "Use case Processing Synchronized Batch (batchNumber = " +
				util_string::inttostring(batchNumber) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.37) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.37) Retrieve the batch record from the database using the identifier passed in. */
			Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }


			/* (uc5.37) Clear the batch's Pending synchronization flag, storing the current date/time. */
			batch->clearPendingSynchronization();
			batch->setSynchronized();
			getBatchManager()->saveBatch(batch.get());


			/* (uc5.37) Execute "Use Case 5.46: Deleting batch", passing the batch identifier and
			"delete change records" if an HQ batch or "delete batch record" if a User batch. */
			status = usecaseDeletingBatch(batch->getNumber(),
					batch->getType() == hfc::Batch::TypeHQ ? DeleteBatchProcessedMessage::DeleteChangesOnly : DeleteBatchProcessedMessage::DeleteChangesAndBatch);
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SecondarySynchronizationManager", "Use case Processing Synchronized Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SecondarySynchronizationManager", "Use case Processing Synchronized Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* This function is called after booting up receiving the value stored in the database for the
	Primary scale ('primaryScale'), and after changing to Secondary Scale synchronization mode or
	after changing the Primary scale in Secondary Scale Synchronization mode receiving the new or
	pre-existing value for the Primary Scale. */
	bool SecondarySynchronizationManager::verifyPrimaryScaleRecord(const std::string& primary)
	{
		bool isIPAddress = HobIpAddress::isValidIPAddress(primary);
		Util::CSmartPtr<hfc::Scale> primaryScale;


		/* (uc5.2) Using the Primary scale's host name/IP address information received as a
		parameter (or otherwise using the one stored in configuration), check whether there
		is a scale record for the Primary scale. If not, then create a scale record for the
		Primary scale. */
		primaryScale = getScaleManager()->getPrimaryScale();
		if ( primaryScale.null() )
		{
			if ( primary.size() )
			{
				primaryScale = new hfc::Scale();
				if ( isIPAddress )
					primaryScale->setIPAddress(primary);
				else
					primaryScale->setHostName(primary);
				primaryScale->setPrimaryScale();

				getScaleManager()->saveScale(primaryScale.get());

				return true;
			}

#ifndef EVOL_NDEBUG
			Debug::trace("SecondarySynchronizationManager", "******* Unable to create a primary scale record in the database "
					"since the value for Primary Scale is empty");
#endif

			return false;
		}
		else
		{
			/* If we are here is because we have just booted up the scale, in which case the value
			in primary is what configuration knows about primaryScale. If that does not match what
			is stored in the Primary's scale record then there have been changes to the Primary's
			scale record. */
			if ( primary.size() )
			{
				if ( isIPAddress )
				{
					if ( primaryScale->getIPAddress() != primary )
					{
#ifndef EVOL_NDEBUG
						Debug::trace("SecondarySynchronizationManager", "******* The Primary scale's record in the database "
							"indicates an IP address different from the one in configuration");
#endif

						/* Get rid of the old record and create a new one to match the value in Configuration.  */
						getScaleManager()->deleteScale( primaryScale.get() );
						//delete primaryScale;

						hfc::Scale *correctPrimaryScale = new hfc::Scale();
						if (correctPrimaryScale)
						{
							correctPrimaryScale->setIPAddress(primary);
							correctPrimaryScale->setPrimaryScale();
							getScaleManager()->saveScale(correctPrimaryScale);
							delete correctPrimaryScale;
							correctPrimaryScale = 0;
						}

						return true;
					}
					else
						return true;
				}
				else
				{
					if ( util_string::strtoupper(primaryScale->getHostName()) != util_string::strtoupper(primary) )
					{
#ifndef EVOL_NDEBUG
						Debug::trace("SecondarySynchronizationManager", "******* The Primary scale's record in the database "
							"indicates a hostname different from the one in configuration");
#endif

						/* Get rid of the old record and create a new one to match the value in Configuration.  */
						getScaleManager()->deleteScale( primaryScale.get() );
						//delete primaryScale;

						hfc::Scale *correctPrimaryScale = new hfc::Scale();
						if (correctPrimaryScale)
						{
							correctPrimaryScale->setHostName(primary);
							correctPrimaryScale->setPrimaryScale();
							getScaleManager()->saveScale(correctPrimaryScale);
							delete correctPrimaryScale;
							correctPrimaryScale = 0;
						}

						return true;
					}
					else
						return true;
				}
			}
			else

			return false;
		}
	}


}
