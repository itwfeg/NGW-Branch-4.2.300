#include "PrimarySynchronizationManager.h"


#include "SynchronizationManagerProxy.h"
#include "QueueManager.h"
#include "CommunicationSynchronizer.h"
#include "DatabaseSynchronizer.h"

#include "ScaleManager.h"
#include "BatchManager.h"
#include "SynchronizationChangeManager.h"


#include "SessionManager.h"
#include "TimerManager.h"
#include "Configuration.h"


#include "Commands/SynchronizePrimaryScaleCommand.h"
#include "Commands/DownloadLocalBatchesCommand.h"
#include "Commands/SynchronizeSecondaryScaleCommand.h"
#include "Commands/PurgeBatchesCommand.h"
#include "Commands/ApplyBatchCommand.h"
#include "Commands/DeleteBatchCommand.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/StartSyncManagerMessage.h"
#include "Messages/StartSyncManagerProcessedMessage.h"
#include "Messages/ComingUpOnLineMessage.h"
#include "Messages/SynchronizePrimaryScaleProcessedMessage.h"
#include "Messages/GoingOffLineMessage.h"
#include "Messages/RegisterSecondaryScaleMessage.h"
#include "Messages/RegisterSecondaryScaleProcessedMessage.h"
#include "Messages/DeregisterSecondaryScaleMessage.h"
#include "Messages/DeregisterSecondaryScaleProcessedMessage.h"
#include "Messages/ProcessUserChangesMessage.h"
#include "Messages/ProcessUserChangesProcessedMessage.h"
#include "Messages/ProcessHQBatchMessage.h"
#include "Messages/ProcessHQBatchProcessedMessage.h"
#include "Messages/ProcessUserFileMessage.h"
#include "Messages/ProcessUserFileProcessedMessage.h"
#include "Messages/SynchronizeSecondaryScaleMessage.h"
#include "Messages/SynchronizeSecondaryScaleProcessedMessage.h"
#include "Messages/SynchronizeSecondaryScalesLazilyMessage.h"
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
#include "Messages/DownloadLocalBatchesMessage.h"
#include "Messages/DownloadLocalBatchesProcessedMessage.h"
#include "Messages/ProcessRemoteBatchMessage.h"
#include "Messages/ProcessRemoteBatchProcessedMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"
#include "SynchronizationStatus.h"


#include "CMReaderWriterSyncTag.h"
#include "CMReaderWriterUltimaDisk.h"


#include "util/e_string/utilstring.h"
#include "util/time/dbtime.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif


namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int PrimarySynchronizationManager::_dataType_ = 0;
	const string PrimarySynchronizationManager::_dataTypeName_("synchronization::PrimarySynchronizationManager");
#endif


	PrimarySynchronizationManager::PrimarySynchronizationManager(bool online)
		: SynchronizationManager(ScaleSynchronizationTypePrimary, "PrimarySynchronizationManager"), numberSecondaryScales_(0)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif


		msgManager_->RestartThread();

		synchronizeAllSecondariesTimerId_ = 0;

		/* Trigger the execution of "Use Case 5.2: Starting up Synchronization Manager". */
		Util::CMSGEvent startEvent;
		StartSyncManagerMessage start(online, startEvent);
		msgManager_->dispatchEvent(&start, msgManager_);

		/* The Synchronization Manager's constructor does not return until "Use Case 5.2: Starting
		up Synchronization Manager" completes. */
#ifndef EVOL_NDEBUG
		util_time::Time t0 = util_time::Time();
#endif

		startEvent.Wait();

#ifndef EVOL_NDEBUG
		util_time::Time t1 = util_time::Time();
		Debug::trace("PrimarySynchronizationManager", "Waited " + 
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


	PrimarySynchronizationManager::~PrimarySynchronizationManager()
	{
#ifndef EVOL_NDEBUG
		util_time::Time t0 = util_time::Time();
		Debug::trace("PrimarySynchronizationManager", "Going to stop the proxy");
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
		Debug::trace("PrimarySynchronizationManager", "Synchronizing with proxy took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " msecs.");
#endif

		msgManager_->deregisterAllEvents();
		msgManager_->ShutdownNicely();


#ifndef EVOL_NDEBUG
		util_time::Time t2;
		Debug::trace("PrimarySynchronizationManager", "Synchronizing with processEvent thread took " +
			util_string::inttostring((t2 - t1).getTotalMilliseconds()) + " msecs.");
#endif



		/* Execute "Use Case 5.6: Stopping Synchronization Manager". */
		usecaseStoppingSynchronizationManager();

#ifndef EVOL_NDEBUG
		util_time::Time t3;
		Debug::trace("PrimarySynchronizationManager", "Stopping the Synchronization mechanism took " +
			util_string::inttostring((t3 - t2).getTotalMilliseconds()) + " msecs.");
#endif


		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	void PrimarySynchronizationManager::processEvent(Util::CSmartPtr<EvolMessageWrapper> & msg)
	{
		if ( msg.notNull() )
		{


			UINT32 result = SessionManager::getInstance()->requestToContinue(msgManager_->GetThreadID(), msgManager_->IsASuperThread());
			switch ( result & (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED) )
			{

				case SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED:
				{
#ifndef EVOL_NDEBUG
					util_time::Time t0;
#endif

					HFCManagerContainer::init();
					DBAccess::uninitDB();
					DBAccess::initDB();

#ifndef EVOL_NDEBUG
					util_time::Time t1;
					Debug::trace("PrimarySynchronizationManager", "After compressing DB, its reinitialization took " +
						util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " msecs.");
#endif

					break;
				}
				default:
				{
					break;
				}
			}


			{
				switch ( msg->getDeviceType() )
				{
					case TimerDev:
					{
						if ( msg->getEventType() == TimerExpired )
						{
							Timeout timeout( msg->Data() );
							if ( timeout.getTimerId() == synchronizeAllSecondariesTimerId_ )
							{
								TimerManager::getInstance()->removeTimer(synchronizeAllSecondariesTimerId_);
								synchronizeAllSecondariesTimerId_ = 0;

								usecaseSynchronizingSecondaryScalesLazily2();
							}
							else
							{
								Timer* timer = TimerManager::getInstance()->getTimer(timeout.getTimerId());
								if ( timer && timer->getTimerPurpose() == timedFuncRetryCommand )
								{
									RetryTimer* retryTimer = dynamic_cast<RetryTimer*>(timer);
									if ( retryTimer )
									{
										QueueManager::QueueType queue = retryTimer->getQueue();
										Command* command = retryTimer->getCommand();
										TimerManager::getInstance()->removeTimer(timeout.getTimerId());

										command->setRetryTimerId(0);
										usecaseRetryingCommand2(queue, command);
									}
									else
										TimerManager::getInstance()->removeTimer(timeout.getTimerId());
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

			}

			SessionManager::getInstance()->changeBlockabilityStatus(msgManager_->GetThreadID(), true, msgManager_->IsASuperThread());

		}
	}


	void PrimarySynchronizationManager::processSynchronizationEvent(Util::CSmartPtr<EvolMessageWrapper> & msg)
	{
		Util::CSmartPtr<EvolMessage> response;

		switch ( msg->getEventType() )
		{
			case StartSyncManagerEvent:                    /* Use Case 5.2 */
			{
				response = processStartSyncManagerMessage(
						Util::CSmartPtr<StartSyncManagerMessage>(new StartSyncManagerMessage(msg->Data())));

				break;
			}
			case ComingUpOnLineEvent:                      /* Use Case 5.3 */
			{
				response = processComingUpOnLineMessage(Util::CSmartPtr<ComingUpOnLineMessage>(new ComingUpOnLineMessage(msg->Data())));

				break;
			}
			case SynchronizePrimaryScaleProcessedEvent:    /* Use Case 5.3 */
			{
				response = processComingUpOnLine2Message(Util::CSmartPtr<SynchronizePrimaryScaleProcessedMessage>(new
						SynchronizePrimaryScaleProcessedMessage(msg->Data())));

				break;
			}
			case GoingOffLineEvent:                        /* Use Case 5.5 */
			{
				response = processGoingOffLineMessage(Util::CSmartPtr<GoingOffLineMessage>(new GoingOffLineMessage(msg->Data())));

				break;
			}
			case RegisterSecondaryScaleEvent:              /* Use Case 5.13 */
			{
				response = processRegisterSecondaryScaleMessage(
						Util::CSmartPtr<RegisterSecondaryScaleMessage>(new RegisterSecondaryScaleMessage(msg->Data())));

				break;
			}
			case DeregisterSecondaryScaleEvent:              /* Use Case 5.15 */
			{
				response = processDeregisterSecondaryScaleMessage(
						Util::CSmartPtr<DeregisterSecondaryScaleMessage>(new DeregisterSecondaryScaleMessage(msg->Data())));

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
			case DownloadLocalBatchesEvent:                /* Use Case 5.24 */
			{
				response = processDownloadLocalBatchesMessage(
						Util::CSmartPtr<DownloadLocalBatchesMessage>(new DownloadLocalBatchesMessage(msg->Data())));

				break;
			}
			case ProcessUserFileEvent:                     /* Use Case 5.28 */
			{
				response = processProcessUserFileMessage(Util::CSmartPtr<ProcessUserFileMessage>(new ProcessUserFileMessage(msg->Data())));

				break;
			}
			case SynchronizeSecondaryScaleEvent:           /* Use Case 5.32 */
			{
				response = processSynchronizeSecondaryScaleMessage(Util::CSmartPtr<SynchronizeSecondaryScaleMessage>(new
						SynchronizeSecondaryScaleMessage(msg->Data())));

				break;
			}
			case SynchronizeSecondaryScalesLazilyEvent:    /* Use Case 5.35 */
			{
				response = processSynchronizeSecondaryScalesLazilyMessage(Util::CSmartPtr<SynchronizeSecondaryScalesLazilyMessage>(new
						SynchronizeSecondaryScalesLazilyMessage(msg->Data())));

				break;
			}
			case ProcessRemoteBatchEvent:                  /* Use Case 5.36 */
			{
				response = processProcessRemoteBatchMessage(Util::CSmartPtr<ProcessRemoteBatchMessage>(new
						ProcessRemoteBatchMessage(msg->Data())));

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

#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Received new synchronization status of " +
					util_string::inttostring(message.getStatus()));
#endif

				/* Fall through to process next case. */
			}
			case RequestSynchronizationStatusEvent:
			{
				SynchronizationDevError sendingDisplayStatus = filterDisplayStatus();

#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Sending synchronization status of " +
					util_string::inttostring(sendingDisplayStatus) + " (actual status is " +
					util_string::inttostring(getDisplayStatus()) + ")");
#endif

				SynchronizationStatus message(sendingDisplayStatus);
				msgManager_->dispatchEvent(&message);

				break;
			}
			default:
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Received unknown event: device=" +
					util_string::inttostring(msg->getDeviceType()) + " event=" + util_string::inttostring(msg->getEventType()));
#endif

				break;
			}
		}

		if ( response.notNull() )
			dispatchResponseMessage(msg, response);
	}


	/* "Use Case 5.2: Starting up Synchronization Manager". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processStartSyncManagerMessage(Util::CSmartPtr<StartSyncManagerMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseStartingUpSynchronizationManager(msg->isOnLine());

			if ( status == _statusOK_ )
				initialized_ = true;
#ifndef EVOL_NDEBUG
			else
				Debug::trace("PrimarySynchronizationManager", "!!!!!usecaseStartingUpSynchronizationManager failed!!!!!");
#endif

			msg->signalEvent();


			/* (uc5.2) Return the Start Synchronization Manager processed message. */
			return Util::CSmartPtr<EvolMessage>(new StartSyncManagerProcessedMessage(status));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.2: Starting up Synchronization Manager". */
	int PrimarySynchronizationManager::usecaseStartingUpSynchronizationManager(bool online)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Starting up Synchronization Manager: "
				"Starting (online = " + util_string::booltostring(online) + ")");
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
			msgManager_->registerEvent(SynchronizationDev, SynchronizePrimaryScaleProcessedEvent);
			msgManager_->registerEvent(SynchronizationDev, GoingOffLineEvent);
			msgManager_->registerEvent(SynchronizationDev, RegisterSecondaryScaleEvent);
			msgManager_->registerEvent(SynchronizationDev, DeregisterSecondaryScaleEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessUserChangesEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessHQBatchEvent);
			msgManager_->registerEvent(SynchronizationDev, DownloadLocalBatchesEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessUserFileEvent);
			msgManager_->registerEvent(SynchronizationDev, SynchronizeSecondaryScaleEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessRemoteBatchEvent);
			msgManager_->registerEvent(SynchronizationDev, ProcessScaleOnlineEvent);
			msgManager_->registerEvent(SynchronizationDev, AddRecordToBatchEvent);
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


			/* (uc5.2) Execute "Use Case 5.47: Cleaning up batches" to ensure the database is in
			a stable state. */
			status = usecaseCleaningUpBatches();
			if ( status != _statusOK_ ) break;


			/* (uc5.2) Initialize Queue Managers, which will start their respective Synchronizers. */
			// We pass the message receiver with the only purpose of enabling the queue managers to
			// post messages back to the Synchronization Manager.
			commQManager_ = new QueueManager(QueueManager::CommunicationQueue, getMessageReceiver(),
					new CommunicationSynchronizer(getMessageReceiver(), hfc::Scale::RolePrimary));
			dbQManager_ = new QueueManager(QueueManager::DatabaseQueue, getMessageReceiver(),
					new DatabaseSynchronizer(getMessageReceiver()));


			/* (uc5.2) If the scale is off-line, then trigger the execution of "Use Case 5.5: Going
			off-lline". Otherwise, trigger the execution of "Use Case 5.3: Coming up on-line". */
			if ( online )
			{
				ComingUpOnLineMessage message;

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
			Debug::trace("PrimarySynchronizationManager", "Use case Starting up Synchronization Manager: "
					"completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Starting up Synchronization Manager: "
					"failed (status = " + getStatusText(status) + ")");
#endif


		return status;
	}


	/* "Use Case 5.3: Coming up on-line" */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processComingUpOnLineMessage(Util::CSmartPtr<ComingUpOnLineMessage> msg)
	{
		if ( msg.notNull() )
		{
			usecaseComingUpOnLine();

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.3: Coming up on-line" */
	int PrimarySynchronizationManager::usecaseComingUpOnLine()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		HobDateTime now;
		Debug::trace("PrimarySynchronizationManager", "Use case Coming up on-line (down time " +
				util_string::inttostring((int)now.timeDiff(fullyFunctioningChangedDateTime_)) + "secs.): Starting");
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
			/* By commenting out next line, when the Primary scale is trying to synchronize itself
			with its Secondaries, we force the Primary to wait until it successfully communicates
			with all of them or until it gives up retrying any that fails (up to 15 minutes for 3
			retry attempts). */
//			fullyFunctioning_ = true;


			/* (uc5.3) For each Secondary scale, perform the following steps: */
			numberSecondaryScales_ = 0;
			if ( getScaleManager()->initializeSecondaryScalesQuery() )
			{
				Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->getFirstSecondaryScale());
				while ( scale.notNull() )
				{
					/* (uc5.3) a. If the Secondary scale is marked as Off-line, then mark it as On-line.
					[Rationale: we try all the Secondary scales, including the ones marked as Off-line,
					since they may now be available and may have received HQ uploads.] */
					if ( !scale->isStateActive() )
					{
						scale->setStateActive();
						getScaleManager()->saveScale(scale);
					}


					/* (uc5.3) Execute "Use Case 5.53: Queuing Command", passing "communication" and
					the Synchronize Primary scale command, containing the Secondary scale identifier
					and the Synchronize Primary scale processed completion message. */
					status = usecaseQueuingCommand(QueueManager::CommunicationQueue, Util::CSmartPtr<Command>(
							new SynchronizePrimaryScaleCommand(scale->getIdentifier(),
							new SynchronizePrimaryScaleProcessedMessage(false, scale->getIdentifier()))));


					++numberSecondaryScales_;
					scale = getScaleManager()->getNextSecondaryScale();
				}
			}


			/* Ensure 2nd half of use case gets executed even if no Secondary scales are registered. */
			if ( !numberSecondaryScales_ )
				usecaseComingUpOnLine2(0);   // success
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Coming up on-line: completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Coming up on-line: failed (status = " + getStatusText(status) + ")");
#endif


		return status;
	}


	/* "Use Case 5.3: Coming up on-line" */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processComingUpOnLine2Message(Util::CSmartPtr<SynchronizePrimaryScaleProcessedMessage> msg)
	{
		if ( msg.notNull() )
		{
			usecaseComingUpOnLine2(msg->getStatus());


			return Util::CSmartPtr<EvolMessage>(0 /*new ScaleOnLineMessage()*/);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.3: Coming up on-line" */
	int PrimarySynchronizationManager::usecaseComingUpOnLine2(int result)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Coming up on-line 2: Starting");
#else
		UNUSED1(result);
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			if ( numberSecondaryScales_ )
				--numberSecondaryScales_;

			if ( !numberSecondaryScales_ )
			{
				/* (uc5.3) If the Stopping Synchronization Manager flag is set, then end the execution
				of this use case. */
				if ( stoppingSynchronizationManager_ )
					{ status = _statusStoppingSyncMgr_; break; }


				if ( !scaleOnline_ )
					{ status = _statusScaleOffLineFailure_; break; }


				/* (uc5.3) Set the Fully-functioning flag. */
				fullyFunctioning_ = true;


				/* (SYNCSTATUS) notify UI that we are fully functioning. */
				NewSynchronizationStatusMessage message(SynchronizationActiveSyncStatus);
				msgManager_->dispatchEvent(&message);


				/* (uc5.3) Execute "Use Case 5.31: Synchronizing all Secondary Scales", in order to get
				all Secondary scales back in synchrony immediately. */
				status = usecaseSynchronizingAllSecondaryScales();
				if ( status != _statusOK_ ) break;


				/* (uc5.3) For each Secondary scale marked as Local batch notified, trigger the execution
				of "Use Case 5.24: Download Secondary scale local batches". */
				/* We do this so that if a secondary scale notified of local batches while the Primary
				was not fully functioning, now will go about retrieving those batches. */
				if ( getScaleManager()->initializeSecondaryScalesQuery() )
				{
					Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->getFirstSecondaryScale());
					while ( scale.notNull() )
					{
						if ( scale->isNotifiedBatches() )
						{
#ifndef EVOL_NDEBUG
							Debug::trace("PrimarySynchronizationManager", "Found scale (with id = " +
									util_string::inttostring(scale->getIdentifier()) + ") notified presence "
									"of local batches. Triggering Download of local changes.");
#endif

							Util::CSmartPtr<DownloadLocalBatchesMessage> message(new DownloadLocalBatchesMessage(scale->getIdentifier()));
							msgManager_->dispatchEvent(message, msgManager_);
						}


						scale = getScaleManager()->getNextSecondaryScale();
					}
				}


				/* (uc5.3) If there are completed batches marked as Pending synchronization, then execute
				"Use Case 5.31: Synchronizing all Secondary scales". */
				if ( getBatchManager()->arePendingSynchronizationBatchesPresent() )
				{
#ifndef EVOL_NDEBUG
					Debug::trace("PrimarySynchronizationManager", "Found pending synchronization batches. "
							"Triggering the Synchronization of All Secondary scales.");
#endif

					status = usecaseSynchronizingAllSecondaryScales();
					if ( status != _statusOK_ ) break;
				}

			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Coming up on-line 2 (result = " +
				util_string::inttostring(result) + "): completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Coming up on-line 2 (result = " +
				util_string::inttostring(result) + "): failed (status = " + getStatusText(status) + ")");
#endif


		return status;
	}


	/* "Use Case 5.13: Registering Secondary scale". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processRegisterSecondaryScaleMessage(Util::CSmartPtr<RegisterSecondaryScaleMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseRegisteringSecondaryScale(msg->getScaleId());


			/* (uc5.13) Return the Register Secondary scale processed message, passing the scale
			identifier. */
			return Util::CSmartPtr<EvolMessage>(new RegisterSecondaryScaleProcessedMessage(status, msg->getScaleId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.13: Registering Secondary scale". */
	int PrimarySynchronizationManager::usecaseRegisteringSecondaryScale(int scaleId)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Registering Secondary Scale (scaleId = " +
				util_string::inttostring(scaleId) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.13) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.13) Retrieve the scale record from the database using the scale identifier
			passed in. */
			Util::CSmartPtr<hfc::Scale> secondary(getScaleManager()->findScaleByIdentifier(scaleId));
			if ( secondary.notNull() )
			{
				/* (uc5.13) Reset the Secondary scale's values as needed, marking it as Needs
				Synchronization and setting its version number to -1. */
				hfc::Scale resetSecondary = hfc::Scale();
				resetSecondary.setIdentifier(scaleId);
				resetSecondary.setSecondaryScaleRole();
				resetSecondary.setHostName(secondary->getHostName());
				resetSecondary.setIPAddress(secondary->getIPAddress());
				resetSecondary.setCreationDateTime(secondary->getCreationDateTimeAsString());
				resetSecondary.setRegistering();
				resetSecondary.setVersion(hfc::Batch::_invalidBatchVersion_);
				/* (TODO) need to call a procedure to stop any retrying that may be going on with
				this scale. */

				getScaleManager()->saveScale(&resetSecondary);
			}
			else
				{ status = _statusCannotFindSecondaryScale_; break; }


			/* (uc5.13) If the Fully-functioning flag is set, then trigger the execution of "Use
			Case 5.32: Synchornizing Secondary scale", passing the scale identifier. */
			if ( fullyFunctioning_ && scaleOnline_ )
			{
				status = usecaseQueuingCommand(QueueManager::CommunicationQueue,
						Util::CSmartPtr<Command>(new SynchronizeSecondaryScaleCommand(scaleId)));
			}
			else
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Use case Registering Secondary Scale (scaleId = " +
					util_string::inttostring(scaleId) + "): ignored request since Synchronization Manager is not fully functioning yet.");
#endif

				status = _statusSyncMgrNotFullyFunctioningYet_;
			}


			break;
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Registering Secondary Scale (scaleId = " +
					util_string::inttostring(scaleId) + ") completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Registering Secondary Scale (scaleId = " +
					util_string::inttostring(scaleId) + ") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.15: Deregistering Secondary scale". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processDeregisterSecondaryScaleMessage(Util::CSmartPtr<DeregisterSecondaryScaleMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseDeregisteringSecondaryScale(msg->getScaleId());


			/* (uc5.15) Return the Deregister Secondary scale processed message. */
			return Util::CSmartPtr<EvolMessage>(new DeregisterSecondaryScaleProcessedMessage(status, msg->getScaleId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.15: Deregistering Secondary scale". */
	int PrimarySynchronizationManager::usecaseDeregisteringSecondaryScale(int scaleId)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Deregistering Secondary Scale (scaleId = " +
				util_string::inttostring(scaleId) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.15) If the Stopping Synchronization Manager flag is set, then end
			the execution of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.15) Remove the Secondary scale record from the database. */
			Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->findScaleByIdentifier(util_string::inttostring(scaleId)));
			getScaleManager()->deleteScale(scale.get());


			/* (uc5.15) Execute "Use Case 5.53: Queuing Command", passing "database" and Purge
			batches command. */
			status = usecaseQueuingCommand(QueueManager::DatabaseQueue, Util::CSmartPtr<Command>(new PurgeBatchesCommand()));
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Deregistering Secondary Scale (scaleId = " +
					util_string::inttostring(scaleId) + ") completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Deregistering Secondary Scale (scaleId = " +
					util_string::inttostring(scaleId) + ") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.20: Processing user changes". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processProcessUserChangesMessage(Util::CSmartPtr<ProcessUserChangesMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseProcessingUserChanges(msg->getUserChange());


			return Util::CSmartPtr<EvolMessage>(new ProcessUserChangesProcessedMessage(status));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.20: Processing user changes". */
	int PrimarySynchronizationManager::usecaseProcessingUserChanges(Util::CSmartPtr<hfc::SynchronizationChange> userChange)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Processing User Changes: Starting");
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


			/* (uc5.20) Trigger the execution of "Use Case 5.35: Synchronizing all Secondary scales
			lazily", to get all Secondary scales back in synchrony. */
			SynchronizeSecondaryScalesLazilyMessage message;
			msgManager_->dispatchEvent(&message, msgManager_);
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Processing User Changes: completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Processing User Changes: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.21: Processing HQ batch". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processProcessHQBatchMessage(Util::CSmartPtr<ProcessHQBatchMessage> msg)
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
	int PrimarySynchronizationManager::usecaseProcessingHQBatchMessage(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Processing HQ Batch: Starting");
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
				Debug::trace("PrimarySynchronizationManager", "Use case Processing HQ Batch (batchNumber = " +
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


			/* (uc5.21) If the Fully-functioning flag is set, then execute "Use Case 5.31:
			Synchronizing all Secondary scales". */
			if ( fullyFunctioning_ && scaleOnline_ )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager",
						"Going to synchronize all Secondary scales because of the processing of a remote batch.");
#endif

				status = usecaseSynchronizingAllSecondaryScales();
			}
			else
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): not attempting to Synchronize all scales since Synchronization Manager is not online.");
#endif
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Processing HQ Batch: completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Processing HQ Batch: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.24: Downloading Secondary scale local batches" */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processDownloadLocalBatchesMessage(Util::CSmartPtr<DownloadLocalBatchesMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseDownloadingLocalBatches(msg->getScaleId());


			/* (uc) Return the Local batch notification processed message. */
			return Util::CSmartPtr<EvolMessage>(new DownloadLocalBatchesProcessedMessage(status, msg->getScaleId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.24: Downloading Secondary scale local batches" */
	int PrimarySynchronizationManager::usecaseDownloadingLocalBatches(int scaleId)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Downloading Local Batches (scaleId = " +
				util_string::inttostring(scaleId) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.24) If the Stopping Synchronization Manager flag is set, then end
			the execution of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.24) If the Fully-functioning flag is set, then execute "Use Case 5.53:
			Queuing Command", passing "communication" and Download local batches command,
			containing the Secondary scale's identifier. */
			if ( fullyFunctioning_ && scaleOnline_ )
			{
				status = usecaseQueuingCommand(QueueManager::CommunicationQueue,
						Util::CSmartPtr<Command>(new DownloadLocalBatchesCommand(scaleId)));
			}
			else
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Use case Downloading Local Batches (scaleId = " +
					util_string::inttostring(scaleId) + "): ignored request since Synchronization Manager is not fully functioning yet.");
#endif

				status = _statusSyncMgrNotFullyFunctioningYet_;
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Downloading Local Batches (scaleId = " +
					util_string::inttostring(scaleId) + ") completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Downloading Local Batches (scaleId = " +
					util_string::inttostring(scaleId) + ") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.28: Processing user file". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processProcessUserFileMessage(Util::CSmartPtr<ProcessUserFileMessage> msg)
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
	int PrimarySynchronizationManager::usecaseProcessingUserFile(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Processing User File (batchNumber = " +
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
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Deleting the batch (batchNumber = " +
					util_string::inttostring(batchNumber) + ") because it is empty.");
#endif

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


			/* (uc5.28) If the Fully-functioning flag is set, then execute "Use Case 5.31:
			Synchronizing all Secondary scales". */
			if ( fullyFunctioning_ && scaleOnline_ )
			{
				status = usecaseSynchronizingAllSecondaryScales();
				if ( status != _statusOK_ ) break;
			}
			else
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Use case Processing User File (batchNumber = " +
					util_string::inttostring(batchNumber) + "): not attempting to Synchronize all scales since Synchronization Manager is not online.");
#endif
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Processing User File (batchNumber = " +
					util_string::inttostring(batchNumber) + ") completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Processing User File (batchNumber = " +
					util_string::inttostring(batchNumber) + ") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.31: Synchronizing all Secondary scales". */
	int PrimarySynchronizationManager::usecaseSynchronizingAllSecondaryScales()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing all Secondary scales: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.31) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.31) If the Fully-functioning flag is clear, then end the execution of this use
			case. */
			if ( !(fullyFunctioning_ && scaleOnline_) )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing all Secondary scales: " +
						std::string("ignored request since Synchronization Manager is not fully functioning yet."));
#endif

				status = _statusSyncMgrNotFullyFunctioningYet_;

				break;
			}


			/* (uc5.31) For each Secondary scale in the database, perform the following steps: */
			if ( getScaleManager()->initializeSecondaryScalesQuery() )
			{
				Util::CSmartPtr<hfc::Scale> secondary(getScaleManager()->getFirstSecondaryScale());
				while ( secondary.notNull() )
				{
					int loopStatus = _statusOK_;

					if ( secondary->isStatePassive() )
					{
						/* (uc5.31) If the scale is marked as Off-line and the time elapsed since it
						was marked as such is greater than a certain amount of time (about 3 days),
						then execute "Use Case 5.15: Deregistering Secondary scale", passing the scale
						identifier. Skip to next scale. */
						HobDateTime passiveDT(secondary->getFirstFailedCommDateTime());
						passiveDT.incrementHours((int)Configuration::getInstance()->toIntCfgI(CfgInternal::HOURSBEFOREDEREGISTERING));
						if ( passiveDT < HobDateTime() )
						{
#ifndef EVOL_NDEBUG
							Debug::trace("PrimarySynchronizationManager", "Going to deregister scale with id = " +
								util_string::inttostring(secondary->getIdentifier()) + " since it has been off-line too long");
#endif

							loopStatus = usecaseDeregisteringSecondaryScale(secondary->getIdentifier());
						}
#ifndef EVOL_NDEBUG
						else
							Debug::trace("PrimarySynchronizationManager", "Not synchronizing scale with id = " +
								util_string::inttostring(secondary->getIdentifier()) + " since it is off-line");
#endif
					}
					else
					{
						/* (uc5.31) If the scale is marked as On-line, then mark it as Needs synchronization
						and execute "Use Case 5.32: Synchronizing Secondary Scale", passing the scale
						identifier. */
						secondary->setNotifiedOnline();
						getScaleManager()->saveScale(secondary);

						loopStatus = usecaseSynchronizingSecondaryScale(secondary->getIdentifier());
					}

					if ( loopStatus != _statusOK_ )
						status = loopStatus;

					secondary = getScaleManager()->getNextSecondaryScale();
				}
			}
			if ( status != _statusOK_ ) break;


			/* (uc5.31) Execute "Use Case 5.53: Queuing Command", passing "database" and Purge
			batches command. */
			status = usecaseQueuingCommand(QueueManager::DatabaseQueue, Util::CSmartPtr<Command>(new PurgeBatchesCommand()));
			if ( status != _statusOK_ ) break;
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing all Secondary scales: completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing all Secondary scales: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.32: Synchronizing Secondary scale". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processSynchronizeSecondaryScaleMessage(Util::CSmartPtr<SynchronizeSecondaryScaleMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseSynchronizingSecondaryScale(msg->getScaleId());


			/* (uc5.32) Return the Synchronize Secondary scale processed message. */
			return Util::CSmartPtr<EvolMessage>(new SynchronizeSecondaryScaleProcessedMessage(status, msg->getScaleId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.32: Synchronizing Secondary scale". */
	int PrimarySynchronizationManager::usecaseSynchronizingSecondaryScale(int scaleId)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scale (scaleId = " +
				util_string::inttostring(scaleId) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.32) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.32) If the Fully-functioning flag is clear, then end the execution of this use
			case. */
			if ( !(fullyFunctioning_ && scaleOnline_) )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scale (scaleId = " +
					util_string::inttostring(scaleId) + "): ignored request since Synchronization Manager is not fully functioning yet.");
#endif

				status = _statusSyncMgrNotFullyFunctioningYet_;

				break;
			}


			/* (uc5.32) Retrieve the scale record from the databse usnig the identifier passed in. */
			Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->findScaleByIdentifier(util_string::inttostring(scaleId)));
			if ( scale.null() )
				{ status = _statusCannotFindSecondaryScale_; break; }


			/* (uc5.32) If the Secondary scale's version number matches the current version number,
			then clear the Needs synchronization flag and end the execution of this use case. */
			if ( scale->getVersion() < getLastBatchCompletedVersion() )
			{
				/* (uc5.32) Execute "Use Case 5.53: Queuing Command", passing "communication" and
				Synchronize Secondary scale command, containing the scale identifier. */
				status = usecaseQueuingCommand(QueueManager::CommunicationQueue, Util::CSmartPtr<Command>(
						new SynchronizeSecondaryScaleCommand(scale->getIdentifier())));
			}
			else
			{
				scale->clearNotifiedOnline();
				getScaleManager()->saveScale(scale.get());
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scale (scaleId = " +
					util_string::inttostring(scaleId) + ") completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scale (scaleId = " +
					util_string::inttostring(scaleId) + ") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.35: Synchronizing all Secondary scales lazily". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processSynchronizeSecondaryScalesLazilyMessage(Util::CSmartPtr<SynchronizeSecondaryScalesLazilyMessage> msg)
	{
		if ( msg.notNull() )
		{
			/*int status =*/ usecaseSynchronizingSecondaryScalesLazily();


			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.35: Synchronizing all Secondary scales lazily". */
	int PrimarySynchronizationManager::usecaseSynchronizingSecondaryScalesLazily()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scales lazily: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.35) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.35) If the Synchronizing all Secondary scales timer (a 2min. timer) is not running,
			then start it now keeping track of the start time. Otherwise, if it has been running for
			less than a specific amount of time (about 15 min.) since the start time, then restart the
			timer now. */
			TimerManager* timerMgr = TimerManager::getInstance();
			if ( synchronizeAllSecondariesTimerId_ == 0 )
			{
				synchronizeAllSecondariesTimerStarted_ = HobDateTime();
				synchronizeAllSecondariesTimerId_ = timerMgr->addTimer(getMessageReceiver(), Configuration::getInstance()->toIntCfgI(CfgInternal::SYNCHRONIZEALLSECONDARIESTIMER),
					Timer::RepeatNO, timedFuncSynchronizeAllSecondariesTimer);
			}
			else
			{
				HobDateTime nowTime;
				nowTime.incrementMinutes((int)-15);
				if ( nowTime < synchronizeAllSecondariesTimerStarted_ )
				{
					timerMgr->removeTimer(synchronizeAllSecondariesTimerId_);
					synchronizeAllSecondariesTimerId_ = timerMgr->addTimer(getMessageReceiver(), Configuration::getInstance()->toIntCfgI(CfgInternal::SYNCHRONIZEALLSECONDARIESTIMER),
						Timer::RepeatNO, timedFuncSynchronizeAllSecondariesTimer);
				}
			}


			/* (uc5.35) Wait until the Synchronize all Secondary scales timer times out. */
			/* (nothing to do here, simply let the timer expire) */
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scales lazily: completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scales lazily: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.35: Synchronizing all Secondary scales lazily". */
	int PrimarySynchronizationManager::usecaseSynchronizingSecondaryScalesLazily2()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scales lazily 2: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			int userBatchNumber = hfc::Batch::_invalidBatchNumber_;


			/* (uc5.35) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.35) Execute "Use Case 5.44: Completing current user batch". */
			status = usecaseCompletingCurrentUserBatch(userBatchNumber);
			if ( status != _statusOK_ ) break;
			if ( userBatchNumber != hfc::Batch::_invalidBatchNumber_ )
			{
				Util::CSmartPtr<hfc::Batch> userBatch(getBatchManager()->findBatchNumber(userBatchNumber));


				/* (uc5.35) Mark the batch as Pending Synchronization. */
				userBatch->setPendingSynchronization();
				getBatchManager()->saveBatch(userBatch.get());
			}


			/* (uc5.35) If the Fully-functioning flag is set, then execute "Use Case 5.31: Synchronizing
			all Secondary scales". */
			if ( fullyFunctioning_ && scaleOnline_ )
			{
				status = usecaseSynchronizingAllSecondaryScales();
				if ( status != _statusOK_ ) break;
			}
			else
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scales lazily 2: "
					"not attempting to Synchronize all scales since Synchronization Manager is not online.");
#endif
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scales lazily 2: completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Synchronizing Secondary scales lazily 2: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.36: Processing remote batch". */
	Util::CSmartPtr<EvolMessage> PrimarySynchronizationManager::processProcessRemoteBatchMessage(Util::CSmartPtr<ProcessRemoteBatchMessage> msg)
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
	int PrimarySynchronizationManager::usecaseProcessingRemoteBatch(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("PrimarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
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
				Debug::trace("PrimarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
						util_string::inttostring(batchNumber) + "): this HQ has already been received. Deleting it.");


				/* (uc5.36) Execution "Use Case 5.46: Deleting batch", passing the batch identifier
				and "delete batch record". */
				status = usecaseDeletingBatch(batchNumber, DeleteBatchProcessedMessage::DeleteChangesAndBatch);


				/* (uc5.36) End the execution of this use case. */
				break;
			}


			/* (uc5.36) Execute "Use Case 5.43: Completing batch", passing the batch identifier. */
			status = usecaseCompletingBatch(batchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.36) Execute "Use Case 5.45: Applying batch", passing the batch identifier. */
			status = usecaseApplyingBatch(batchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.36) Mark the batch as Pending synchronization. */
			/* Refresh the batch from the database. */
			batch = getBatchManager()->findBatchNumber(batch->getNumber());
			batch->setPendingSynchronization();
			getBatchManager()->saveBatch(batch);

				
			/* (uc5.36) If the Fully-functioning flag is set, then execute "Use Case 5.31:
			Synchronizing all Secondary scales". */
			if ( fullyFunctioning_ && scaleOnline_ )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager",
						"Going to synchronize all Secondary scales because of the processing of a remote batch.");
#endif

				status = usecaseSynchronizingAllSecondaryScales();
			}
			else
			{
#ifndef EVOL_NDEBUG
				Debug::trace("PrimarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): not attempting to Synchronize all scales since Synchronization Manager is not online.");
#endif
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("PrimarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + ") completed successfully");
		else
			Debug::trace("PrimarySynchronizationManager", "Use case Processing Remote Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + ") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


}
