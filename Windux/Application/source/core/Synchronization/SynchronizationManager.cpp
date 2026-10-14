#include "SynchronizationManager.h"


#include "SynchronizationManagerProxy.h"
#include "DatabaseSynchronizer.h"

#include "QueueManager.h"


#include "ScaleManager.h"
#include "Scale.h"
#include "BatchManager.h"
#include "SynchronizationChangeManager.h"
#include "SynchronizationChange.h"
#include "TimerManager.h"
#include "SynchronizationStatus.h"


#include "CMessageCenter.h"
#include "CEvolMsgManager.h"
#include "TimerContainer.h"

#include "Configuration.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/GoingOffLineMessage.h"
#include "Messages/ProcessScaleOnlineMessage.h"
#include "Messages/CreateBatchMessage.h"
#include "Messages/CreateBatchProcessedMessage.h"
#include "Messages/AddRecordToBatchMessage.h"
#include "Messages/AddRecordToBatchProcessedMessage.h"
#include "Messages/AbortBatchMessage.h"
#include "Messages/AbortBatchProcessedMessage.h"
#include "Messages/CompleteBatchMessage.h"
#include "Messages/CompleteBatchProcessedMessage.h"
#include "Messages/ApplyBatchMessage.h"
#include "Messages/ApplyBatchProcessedMessage.h"
#include "Messages/DeleteBatchMessage.h"
#include "Messages/DeleteBatchProcessedMessage.h"
#include "Messages/CompleteCommandMessage.h"
#include "Messages/RetryCommandMessage.h"
#include "Messages/EmptyQueueMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"

#include "Commands/AbortBatchCommand.h"
#include "Commands/ApplyBatchCommand.h"
#include "Commands/DeleteBatchCommand.h"


#include "DBException.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



using namespace hfc;



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int SynchronizationManager::_dataType_ = 0;
	const string SynchronizationManager::_dataTypeName_("synchronization::SynchronizationManager");
#endif

	const std::string SynchronizationManager::_statusNoCurrentUserBatchText_                ("No current User batch");
	const std::string SynchronizationManager::_statusOKText_                                ("OK");
	const std::string SynchronizationManager::_statusBadParameterText_                      ("Bad Parameter");
	const std::string SynchronizationManager::_statusStoppingSyncMgrText_                   ("Stopping Sync Manager");
	const std::string SynchronizationManager::_statusCannotCreateBatchText_                 ("Cannot create batch");
	const std::string SynchronizationManager::_statusBatchNotFoundText_                     ("Batch Not Found");
	const std::string SynchronizationManager::_statusCannotAbortACompleteBatchText_         ("Cannot abort a complete batch");
	const std::string SynchronizationManager::_statusBatchNotCompleteText_                  ("Batch Not Complete");
	const std::string SynchronizationManager::_statusCannotCompleteACompleteBatchText_      ("Cannot complete a complete batch");
	const std::string SynchronizationManager::_statusCannotFindPrimaryScaleText_            ("Cannot find Primary scale");
	const std::string SynchronizationManager::_statusCannotFindSecondaryScaleText_          ("Cannot find Secondary scale");
	const std::string SynchronizationManager::_statusSyncMgrNotFullyFunctioningYetText_     ("Synchronization Manager not fully functioning yet");
	const std::string SynchronizationManager::_statusCannotAddRecordToCompleteBatchText_    ("Cannot add a record to a complete batch");
	const std::string SynchronizationManager::_statusFailedToRemoveCommandFromQueueText_    ("Failed to remove command from queue");
	const std::string SynchronizationManager::_statusDBExceptionText_                       ("Failed due to a DB exception");
	const std::string SynchronizationManager::_statusScaleOffLineFailureText_               ("Failed due to Scale Off line");
	const std::string SynchronizationManager::_statusCannotFindCommandText_                 ("Cannot find command");
	const std::string SynchronizationManager::_statusTooManyRetriesText_                    ("Too many retries");
	const std::string SynchronizationManager::_statusWrongTypeOfBatchText_                  ("Wrong type of batch");
	const std::string SynchronizationManager::_statusPrimaryScaleChangedText_               ("Primary scale changed");
	const std::string SynchronizationManager::_statusUnknownText_                           ("Unknown");


	SynchronizationManager::SynchronizationManager(ScaleSynchronizationType syncType, const char * name)
    #if defined(LINUX_FIX)
		: msgManager_(0), proxy_(0), initialized_(false), scaleOnline_(false), fullyFunctioning_(false), synchronizingAllScales_(false),
        pausingProcessing_(false), stoppingSynchronizationManager_(false), currentUserBatchNumber_(hfc::Batch::_invalidBatchNumber_),
        commQManager_(0), dbQManager_(0), syncType_(syncType), displayStatus_((SynchronizationDevError)0),
    #else
		: msgManager_(0), initialized_(false), scaleOnline_(false), fullyFunctioning_(false), synchronizingAllScales_(false),
		stoppingSynchronizationManager_(false), currentUserBatchNumber_(hfc::Batch::_invalidBatchNumber_), commQManager_(0),
		dbQManager_(0), syncType_(syncType), proxy_(0), pausingProcessing_(false), displayStatus_((SynchronizationDevError)0),
    #endif
		shutdownFlag_(false)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

			
		/* The subclasses need to call msgManager_->RestartThread() in their constructors. In this
		way we ensure that the assignment below happens before the thread starts to run. */
		msgManager_ = new Hobart::CEvolMsgManager(this, name, true, true, false);

		Debug::trace( "SynchronizationManager", "before instantiating new SynchronizationManagerProxy, * " + util_string::ptrtostring(proxy_.get()) );
		proxy_ = new SynchronizationManagerProxy(msgManager_);
		Debug::trace( "SynchronizationManager", "after instantiating new SynchronizationManagerProxy, * " + util_string::ptrtostring(proxy_.get()) );

		synchronizeAllSecondariesTimerId_ = 0;
		notifyLocalBatchesTimerId_ = 0;

		retryMaximum_ = Configuration::getInstance()->toIntCfgI(CfgInternal::MAXIMUMNUMBERRETRIES);
		retryCommandTimer_ = Configuration::getInstance()->toIntCfgI(CfgInternal::RETRYCOMMANDTIMER);

		processEventThreadTopOfLoopEvent_.EventSet();
	}


	SynchronizationManager::~SynchronizationManager()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	/* "Use Case 5.5: Going off-line" */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processGoingOffLineMessage(Util::CSmartPtr<GoingOffLineMessage> msg)
	{
		if ( msg.notNull() )
		{
			/*int status =*/ usecaseGoingOffLine();


			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.5: Going off-line". */
	int SynchronizationManager::usecaseGoingOffLine()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Going off-line: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (SYNCSTATUS) notify UI that we are going off-line by clearing the status. */
			NewSynchronizationStatusMessage message((SynchronizationDevError)ClearStatusUpInHere);
			msgManager_->dispatchEvent(&message);


			/* (uc5.5) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.5) Clear the Fully-functioning flag, storing the current date/time. */
			scaleOnline_ = false;
			fullyFunctioning_ = false;
			fullyFunctioningChangedDateTime_.setToCurrent();


			/* (uc5.5) Execute "Use Case 5.57: Emptying queue", passing the "communication" parameter.
			[Rationale: if we let those commands execute, then we will trigger unnecessary retries and
			confusion.] */
			status = usecaseEmptyingQueue(QueueManager::CommunicationQueue);
			if ( status != _statusOK_ ) break;
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Going off-line: completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Going off-line: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.6: Stopping Synchronization Manager". */
	int SynchronizationManager::usecaseStoppingSynchronizationManager()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Stopping Synchronization Manager: Starting");
#endif


		/* By the time we are here the SynchronizationManager (its processEvent thread) is no longer
		running since we called msgManager_->ShutdownNicely(). */
		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.6) Set the Stopping Synchronization Manager flag. */
			/* [This is unnecessary now since the processEvent thread is not running anymore.
			Additionally, some of the logic below (i.e., usecaseCompletingBatch) depends on this
			flag being false.] */
//			stoppingSynchronizationManager_ = true;


			/* (uc5.6) Clear the Fully-functioning flag. */
			scaleOnline_ = false;
			fullyFunctioning_ = false;


			/* (SYNCSTATUS) notify UI that we are going off synchronization by clearing the status
			(we notify directly since we have destroyed the processEvent thread which would not be
			able to process a NewSynchronizationStatusMessage). */
			SynchronizationStatus message((SynchronizationDevError)ClearStatusUpInHere);
			Hobart::CMessageCenter::instance() << &message;


			/* (uc5.6) Remove any active timer created by the Synchronization Manager. */
			if ( synchronizeAllSecondariesTimerId_ )
			{
				TimerManager::getInstance()->removeTimer(synchronizeAllSecondariesTimerId_);
				synchronizeAllSecondariesTimerId_ = 0;
			}
			if ( notifyLocalBatchesTimerId_ )
			{
				TimerManager::getInstance()->removeTimer(notifyLocalBatchesTimerId_);
				notifyLocalBatchesTimerId_ = 0;
			}

			/* The following use case will also remove the timers associated with retrying commands. */
			status = usecaseEmptyingQueue(QueueManager::CommunicationQueue);
//			if ( status != _statusOK_ ) break;


			/* (uc5.6) Trigger the execution of "Use Case 5.51: Stopping Synchronizers. */
			status = usecaseStoppingSynchronizers();
//			if ( status != _statusOK_ ) break;


			/* (uc5.6) Execute "Use Case 5.44: Completing current user batch. */
			/* [We implement this by performing the function of completing current user batch.] */
			/* [Calling the following function here is safe because the processEvent thread is not running.] */
			int userBatchNumber = getAndClearCurrentUserBatchNumber();
			if ( userBatchNumber != hfc::Batch::_invalidBatchNumber_ )
				usecaseCompletingBatch(userBatchNumber);


			if ( !shutdownFlag_ )
			{
				/* (uc5.6) For each incomplete batch in the database, perform the function of deleting the
				batch and its change records from the database. */
				/* [We will just delete the batch since its change records will be deleted below.] */
				if ( getBatchManager()->initializeIncompleteBatchesQuery() )
				{
					Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->getFirstIncompleteBatch());
					while ( batch.notNull() )
					{
//						getSynchronizationChangeManager()->deleteBatchChanges(batch->getNumber());

						Util::CSmartPtr<hfc::Batch> tempBatch = batch;
						batch = getBatchManager()->getNextIncompleteBatch();
						getBatchManager()->deleteBatch(tempBatch.get());
					}
				}


				/* (uc5.6) For each batch not applied to the database, perform the function of applying
				the batch to the database. */
				if ( getBatchManager()->initializeCompleteNotAppliedBatchesQuery() )
				{
					Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->getFirstCompleteNotAppliedBatch());
					while ( batch.notNull() )
					{
						if ( !batch->isPendingDeletion() && !batch->isDeleted() )
						{
							int batchNumber = batch->getNumber();

							if ( getSynchronizationChangeManager()->initializeBatchChangesQuery(batchNumber) )
							{
								Util::CSmartPtr<hfc::SynchronizationChange> change(getSynchronizationChangeManager()->getFirstChangeFromBatch(batchNumber));
								for ( ; change.notNull(); change = getSynchronizationChangeManager()->getNextChangeFromBatch(batchNumber) )
									DatabaseSynchronizer::applyChangeToDB(this, change, batch);
							}
						}

						batch = getBatchManager()->getNextCompleteNotAppliedBatch();
					}
				}


				/* (uc5.6) Remove all scale database records from the database. */
				getScaleManager()->deleteAllScales();


				/* (uc5.6) Perform the function of purging batches from the database (basically all batches
				will be deleted since they all have been applied to the database and there is no Secondary
				scale to synchronize). */
				getBatchManager()->deleteAllBatches();
				getSynchronizationChangeManager()->deleteAllSynchronizationChanges();
			}


			/* (uc5.6) Wait until the Stop Synchronizers processed message arrives. */
			/* [There is nothing to do here since it already happened in usecaseStoppingSynchronizers.] */
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Stopping Synchronization Manager: completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Stopping Synchronization Manager: failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.8: Resume processing". */
	int SynchronizationManager::usecaseResumingProcessing()
	{
		pausingProcessing_ = false;
		processEventThreadTopOfLoopEvent_.EventSet();

		return _statusOK_;
	}


	/* "Use Case 5.9: Pausing processing". */
	int SynchronizationManager::usecasePausingProcessing(int reason)
	{
		/* The following code will cause the Synchronization Managers to stop processing in their
		processEvent threads (their only threads). */
		pausingProcessingEvent_.EventReset();
		pausingProcessing_ = true;
		processEventThreadTopOfLoopEvent_.Wait();

		/* At this point the Synchroization Manager's processEvent thread is stopped. Now take care
		of the synchronizers. */
		commQManager_->pauseProcessing(reason);
		dbQManager_->pauseProcessing(reason);


		return _statusOK_;
	}


	Util::CSmartPtr<EvolMessage> SynchronizationManager::processProcessScaleOnlineMessage(Util::CSmartPtr<ProcessScaleOnlineMessage> msg)
	{
		if ( msg.notNull() )
		{
			/*int status =*/ usecaseProcessingScaleOnlineMessage(msg->getScaleId());


			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	int SynchronizationManager::usecaseProcessingScaleOnlineMessage(int scaleId)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Processing Scale Online (scaleId = " +
				util_string::inttostring(scaleId) + "): Starting");
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


			Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->findScaleByIdentifier(scaleId));
			if ( scale.null() )
				{ status = _statusCannotFindSecondaryScale_; break; }


			if ( scale->isStateRetrying() )
			{
				/* If the scale is retrying, we can optimize the wait for the timer to time out . */

				/* Find the command for which this scale is retrying. */
				Util::CSmartPtr<Command> command = commQManager_->findRetryingCommand(scaleId);
				if ( command.null() )
					{ status = _statusCannotFindCommand_; break; }


				/* Remove the timer associated with the retrying command. */
				TimerManager::getInstance()->removeTimer(command->getRetryTimerId());
				command->setRetryTimerId(0);


				/* Retry the command. */
				status = usecaseRetryingCommand2(QueueManager::CommunicationQueue, command.get());
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Processing Scale Online (scaleId = " +
					util_string::inttostring(scaleId) + ") completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Processing Scale Online (scaleId = " +
					util_string::inttostring(scaleId) + ") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.40: Creating batch". */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processCreateBatchMessage(Util::CSmartPtr<CreateBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			int batchId = hfc::Batch::_invalidBatchNumber_;
			int status = usecaseCreatingBatch(batchId, msg->getOrigin(), msg->getType());


			/* (uc5.40) Return the Create batch processed message, passing the batch identifier. */
			return Util::CSmartPtr<EvolMessage>(new CreateBatchProcessedMessage(status, batchId));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.40: Creating batch". */
	int SynchronizationManager::usecaseCreatingBatch(int& batchNumber, hfc::Batch::BatchOrigin origin, hfc::Batch::BatchType type)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Creating Batch (origin = " +
				Batch::getOriginName(origin) + ", type = " + Batch::getTypeName(type) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.40) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.40) Create a new local batch in the database, based on the information passed
			in and the version number. Set creation date/time to current date/time. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->createBatch(thisScale_->getNewBatchNumber(), type, origin));
			if ( batch.null() )
			{ status = _statusCannotCreateBatch_; break; }

			batchNumber = batch->getNumber();
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Creating Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Creating Batch (origin = " +
					Batch::getOriginName(origin) + ", type = " + Batch::getTypeName(type) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.41: Adding record to batch". */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processAddRecordToBatchMessage(Util::CSmartPtr<AddRecordToBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseAddingRecordToBatch(msg->getBatchId(), msg->getRecord());

			if ( msg->getSourceObject() )
				return Util::CSmartPtr<EvolMessage>(new AddRecordToBatchProcessedMessage(status, msg->getBatchId()));
			else
				return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.41: Adding record to batch". */
	int SynchronizationManager::usecaseAddingRecordToBatch(int batchNumber, Util::CSmartPtr<hfc::SynchronizationChange> record)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Adding Record to Batch (batchNumber = " +
				util_string::inttostring(batchNumber) + ", record device = " + record->getDeviceName() +
				" - " + util_string::inttostring(record->getDevice()) + ", event = " + util_string::inttostring(record->getEvent()) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.41) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			if ( record.null() )
				{ status = _statusBadParameter_; break; }


			/* (uc5.41) Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.notNull() )
			{
				/* Pre-conditions: the batch exists and it is not marked as complete. */
				if ( !batch->isComplete() )
				{
					/* (uc5.41) Create change record in the database, linking it to the batch. */
					getBatchManager()->addRecordToBatch(record, batch);
				}
				else
				{ status = _statusCannotAddRecordToCompleteBatch_; break; }
			}
			else
			{ status = _statusBatchNotFound_; break; }
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Adding Record to Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Adding Record to Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + ", record device = " + record->getDeviceName() +
					", event = " + util_string::inttostring(record->getEvent()) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.42: Aborting batch". */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processAbortBatchMessage(Util::CSmartPtr<AbortBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseAbortingBatch(msg->getBatchId());

			/* (uc5.42) Return the Abort batch processed message, passing the batch identifier. */
			return Util::CSmartPtr<EvolMessage>(new AbortBatchProcessedMessage(status, msg->getBatchId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.42: Aborting batch". */
	int SynchronizationManager::usecaseAbortingBatch(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Aborting Batch (batchNumber = " +
				util_string::inttostring(batchNumber) + "): Starting ");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.42) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.42) Pre-conditions: the batch exists and it is not marked as complete. */
			/* Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.notNull() )
			{
				if ( !batch->isComplete() )
				{
					/* (uc5.42) Execute "Use Case 5.53: Queuing Command", passing "database" and the
					Abort batch command, containing the batch identifier. */
					status = usecaseQueuingCommand(QueueManager::DatabaseQueue, Util::CSmartPtr<Command>(new AbortBatchCommand(batchNumber)));
				}
				else
				{ status = _statusCannotAbortACompleteBatch_; break; }
			}
			else
			{ status = _statusBatchNotFound_; break; }
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Aborting Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Aborting Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.43: Completing batch". */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processCompleteBatchMessage(Util::CSmartPtr<CompleteBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			int status = usecaseCompletingBatch(msg->getBatchId());

			/* (uc5.43) Return the Complete batch processed message, passing the batch identifier. */
			return Util::CSmartPtr<EvolMessage>(new CompleteBatchProcessedMessage(status, msg->getBatchId()));
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.43: Completing batch". */
	int SynchronizationManager::usecaseCompletingBatch(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Completing Batch (batchNumber = " +
				util_string::inttostring(batchNumber) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.43) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.43) Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }

			
			/* (uc5.43) Pre-conditions: the batch exists and it is not marked as complete. */
			if ( batch->isComplete() )
			{ status = _statusCannotCompleteACompleteBatch_; break; }

			
			/* (uc5.43) Obtain local batch version number, incrementing it, for the new batch and
			store it back. Assign that version number to this batch. */
			int batchVersion = getNewBatchVersionNumber();
			batch->setVersion(batchVersion);


			/* (uc5.43) Mark batch as Complete setting completion date/time to current date/time. */
			batch->setComplete();
			getBatchManager()->saveBatch(batch);

			if ( getLastBatchCompletedVersion() < batchVersion )
				setLastBatchCompletedVersion(batchVersion);
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Completing Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Completing Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.44: Completing current user batch". */
	int SynchronizationManager::usecaseCompletingCurrentUserBatch(int& userBatchNumber)
	{
		int status = _statusOK_;
		userBatchNumber = hfc::Batch::_invalidBatchNumber_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Completing Current User Batch: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.44) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.44) Retrieve the identifier of the current user batch while clearing it. */
			{
				util_thread::Lock< util_thread::Mutex<> > lock(currentUserBatchIntegrityMutex_);

				userBatchNumber = getAndClearCurrentUserBatchNumber();
			}


			/* (uc5.44) If there is no current user batch, then end the execution of this use case. */
			if ( userBatchNumber == hfc::Batch::_invalidBatchNumber_ )
				{ status = _statusOK_; break; }


			/* (uc5.44) Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(userBatchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }


			/* (uc5.44) If the batch is empty, then execute "Use Case 5.42: Aborting batch",
			passing the batch identifier. End the execution of this use case. */
			if ( batch->isSignatureEmtpy() )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("SynchronizationManager", "Use case Completing Current User Batch (atchNumber = " +
					util_string::inttostring(userBatchNumber) + "): aborting batch: signature " +
					std::string(batch->isSignatureEmtpy() ? "IS" : "is NOT") + " empty");
#endif

				status = usecaseAbortingBatch(userBatchNumber);
				if ( status != _statusOK_ ) break;

				break;
			}

			
			/* (uc5.44) Execute "Use Case 5.43: Completing batch", passing the batch identifier. */
			status = usecaseCompletingBatch(userBatchNumber);
			if ( status != _statusOK_ ) break;


			/* (uc5.44) Execute "Use Case 5.45: Applying batch", passing the batch identifier. */
			/* [For Secondary scales we do not need to apply the current user batch because it gets
			applied as the changes are generated and also when it comes back from the Primary.] */
			/* (TODO) what about this? */
//			if ( /*syncType_ == ScaleSynchronizationTypePrimary*/ )
			{
				status = usecaseApplyingBatch(userBatchNumber);
				if ( status != _statusOK_ ) break;
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Completing Current User Batch (batchNumber = " +
					util_string::inttostring(userBatchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Completing Current User Batch (batchNumber = " +
					util_string::inttostring(userBatchNumber) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.45: Applying batch". */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processApplyBatchMessage(Util::CSmartPtr<ApplyBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			usecaseApplyingBatch(msg->getBatchId());

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.45: Applying batch". */
	int SynchronizationManager::usecaseApplyingBatch(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Applying Batch (batchNumber = " +
				util_string::inttostring(batchNumber) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.45) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.45) Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }

			
			/* (uc5.45) Pre-conditions: the batch exists and it is marked as complete. */
			if ( !batch->isComplete() )
			{ status = _statusBatchNotComplete_; break; }


			/* (uc5.45) Mark batch as Pending Application. */
			batch->setPendingApplication();
			getBatchManager()->saveBatch(batch);


			/* (uc5.45) Execute "Use Case 5.53: Queuing Command", passing "database" and the
			 Apply batch command, containing the batch identifier. */
			status = usecaseQueuingCommand(QueueManager::DatabaseQueue, 
					Util::CSmartPtr<Command>(new ApplyBatchCommand(batchNumber, new ApplyBatchProcessedMessage(batchNumber))));
			if ( status != _statusOK_ ) break;


			/* (uc5.45) Wait for the Apply batch processed completion message to arrive. */
			/* (nothing to do here) */
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Applying Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Applying Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.45: Applying batch". (2) */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processApplyBatch2Message(Util::CSmartPtr<ApplyBatchProcessedMessage> msg)
	{
		if ( msg.notNull() )
		{
			/*int status =*/ usecaseApplyingBatch2(msg->getBatchId());

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.45: Applying batch". (2)*/
	int SynchronizationManager::usecaseApplyingBatch2(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Applying Batch 2 (batchNumber = " +
				util_string::inttostring(batchNumber) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = this->_statusBatchNotFound_; break; }


			/* (uc5.45) Clear the batch's Pending Application flag and mark it as Applied, including
			the date/time. */
			batch->clearPendingApplication();
			batch->setApplied();
			getBatchManager()->saveBatch(batch.get());
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Applying Batch 2 (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Applying Batch 2 (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.46: Deleting batch". */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processDeleteBatchMessage(Util::CSmartPtr<DeleteBatchMessage> msg)
	{
		if ( msg.notNull() )
		{
			usecaseDeletingBatch(msg->getBatchId(), msg->getDeletionMethod());

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.46: Deleting batch". */
	int SynchronizationManager::usecaseDeletingBatch(int batchNumber, DeleteBatchProcessedMessage::DeleteBatchMethod deleteMethod)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Deleting Batch (batchNumber = " + util_string::inttostring(batchNumber) +
				", delete method = " + DeleteBatchProcessedMessage::getDeleteMethodName(deleteMethod) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.46) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.46) Retrieve the batch record from the database using the identifier passed in. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
			{ status = _statusBatchNotFound_; break; }


			if ( !batch->isPendingDeletion() )
			{
				/* (uc5.46) Mark the batch as Pending deletion. */
				batch->setPendingDeletion();
				getBatchManager()->saveBatch(batch.get());


				/* (uc5.46) Execute "Use Case 5.53: Queuing command", passing "database" and the
				Delete batch command, containing the batch identifier and the Delete batch processed
				completion message (which in turn contains both parameters received). */
				status = usecaseQueuingCommand(QueueManager::DatabaseQueue,
						Util::CSmartPtr<Command>(new DeleteBatchCommand(batchNumber, new DeleteBatchProcessedMessage(batchNumber, deleteMethod))));
				if ( status != _statusOK_ ) break;
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Deleting Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Deleting Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + ", delete method = " +
					DeleteBatchProcessedMessage::getDeleteMethodName(deleteMethod) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.46: Deleting batch". (2) */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processDeleteBatch2Message(Util::CSmartPtr<DeleteBatchProcessedMessage> msg)
	{
		if ( msg.notNull() )
		{
			/*int status =*/ usecaseDeletingBatch2(msg->getBatchId(), msg->getDeleteMethod());

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.46: Deleting batch". (2)*/
	int SynchronizationManager::usecaseDeletingBatch2(int batchNumber, DeleteBatchProcessedMessage::DeleteBatchMethod deleteMethod)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Deleting Batch 2 (batchNumber = " +
					util_string::inttostring(batchNumber) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = this->_statusBatchNotFound_; break; }


			/* (uc5.46) Clear the batch's Pending deletion flag and store the current date/time. */
			batch->clearPendingDeletion();
			batch->setDeleted();
			getBatchManager()->saveBatch(batch.get());


			/* (uc5.46) If the optional value of "delete change records" was not received, then
			remove the batch itself. */
			if ( deleteMethod != DeleteBatchProcessedMessage::DeleteChangesOnly )
				getBatchManager()->deleteBatch(batch);
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Deleting Batch 2 (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Deleting Batch 2 (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.47: Cleaning up batches". */
	int SynchronizationManager::usecaseCleaningUpBatches()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Cleaning up Batches: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.47) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			/* (uc5.47) Retrieve the identifier of the current user batch while clearing it. */
			int userBatchNumber = hfc::Batch::_invalidBatchNumber_;
			{
				util_thread::Lock< util_thread::Mutex<> > lock(currentUserBatchIntegrityMutex_);

				userBatchNumber = getAndClearCurrentUserBatchNumber();
			}


			/* (uc5.47) Retrieve the batch record from the database using the batch identifier. */
			if ( userBatchNumber != hfc::Batch::_invalidBatchNumber_ )
			{
				Util::CSmartPtr<Batch> batch(getBatchManager()->findBatchNumber(userBatchNumber));


				Debug::trace("SynchronizationManager", "Use case Cleaning up Batches: Found current user batch with number " +
					util_string::inttostring(userBatchNumber));


				/* (uc5.47) If there is a current user batch (therefore it is not completed yet),
				and it is not empty, then execute "Use Case 5.43: Completing batch", passing the
				batch identifier. */
				if ( batch.notNull() && !batch->isSignatureEmtpy() )
				{
					status = usecaseCompletingBatch(userBatchNumber);
					if ( status != _statusOK_ ) break;

					Debug::trace("SynchronizationManager", "Use case Cleaning up Batches: Completed current user batch");
				}
			}


			/* (uc5.47) For each batch not marked as Complete, perform the following steps: */
			if ( getBatchManager()->initializeIncompleteBatchesQuery() )
			{
				Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->getFirstIncompleteBatch());
				while ( batch.notNull() )
				{
					Debug::trace("SynchronizationManager",
						"Going to delete incomplete batch with number=" + util_string::inttostring(batch->getNumber()));


					/* (uc5.47) Remove all change records associated with the batch. */
					getSynchronizationChangeManager()->deleteBatchChanges(batch->getNumber());


					/* (uc5.47) Remove the batch record. */
					Util::CSmartPtr<hfc::Batch> tempBatch = batch;
					batch = getBatchManager()->getNextIncompleteBatch();
					getBatchManager()->deleteBatch(tempBatch.get());
				}
			}


			/* For each batch not applied to the database (like the current user batch from above
			that we just completed), perform the function of applying the batch to the database. */
			if ( getBatchManager()->initializeCompleteNotAppliedBatchesQuery() )
			{
				Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->getFirstCompleteNotAppliedBatch());
				while ( batch.notNull() )
				{
					if ( !batch->isPendingDeletion() && !batch->isDeleted() )
					{
						int batchNumber = batch->getNumber();

						if ( getSynchronizationChangeManager()->initializeBatchChangesQuery(batchNumber) )
						{
							Util::CSmartPtr<hfc::SynchronizationChange> change(getSynchronizationChangeManager()->getFirstChangeFromBatch(batchNumber));
							for ( ; change.notNull(); change = getSynchronizationChangeManager()->getNextChangeFromBatch(batchNumber) )
								DatabaseSynchronizer::applyChangeToDB(this, change, batch);
						}

						batch->clearPendingApplication();
						batch->setApplied();
						batch->setPendingSynchronization();
						getBatchManager()->saveBatch(batch.get());
					}

					batch = getBatchManager()->getNextCompleteNotAppliedBatch();
				}
			}


			/* (uc5.47) For each batch marked as Pending delete, perform the following steps: */
			if ( getBatchManager()->initializePendingDeleteBatchesQuery() )
			{
				Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->getFirstPendingDeleteBatch());
				while ( batch.notNull() )
				{
					Debug::trace("SynchronizationManager",
						"Going to delete pending delete batch with number=" + util_string::inttostring(batch->getNumber()));

			
					/* (uc5.47) Remove all change records associated with the batch. */
					getSynchronizationChangeManager()->deleteBatchChanges(batch->getNumber());


					/* (uc5.47) Remove the batch record. */
					batch->setDeleted();
					getBatchManager()->saveBatch(batch.get());
					getBatchManager()->deleteBatch(batch.get());


					batch = getBatchManager()->getNextPendingDeleteBatch();
				}
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Cleaning up Batches: completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Cleaning up Batches: failed (status = " +
					getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.51: Stopping Synchronizers". */
	int SynchronizationManager::usecaseStoppingSynchronizers()
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		util_time::Time t0;
		Debug::trace("SynchronizationManager", "Use case Stopping Synchronizers: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* We don't check the stoppingSynchronizationManager_ flag since we are stopping it. */


			/* (uc5.51) If the Communication Synchronizer processing flag is set, then execute "Use
			Case 6.51: Stopping Synchronizer". */
			if ( commQManager_.notNull() /*&& commQManager_->isSynchronizerProcessing()*/ )
				commQManager_->stopSynchronizer();


			/* (uc5.51) If the Database Synchronizer processing flag is set, then execute "Use Case
			7.51: Stopping Synchronizer". */
			if ( dbQManager_.notNull() /*&& dbQManager_->isSynchronizerProcessing()*/ )
				dbQManager_->stopSynchronizer();


			/* (uc5.51) If the Communication Synchronizer had been processing, then wait until receiving
			the Communication Synchronizer stopped message. */
			if ( commQManager_.notNull() /*&& commQManager_->isSynchronizerProcessing()*/ )
				commQManager_->waitUntilSynchronizerStops();


			/* (uc5.51) If the Database Synchronizer had been processing, then wait until receiving
			the Database Synchronizer stopped message. */
			if ( dbQManager_.notNull() /*&& dbQManager_->isSynchronizerProcessing()*/ )
				dbQManager_->waitUntilSynchronizerStops();
		}


#ifndef EVOL_NDEBUG
		util_time::Time t1;
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Stopping Synchronizers: completed successfully (took " +
					util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds)");
		else
			Debug::trace("SynchronizationManager", "Use case Stopping Synchronizers: failed (status = " + getStatusText(status) +
					" taking " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds)");
#endif


		return status;
	}


	/* "Use Case 5.53: Queuing command". */
	int SynchronizationManager::usecaseQueuingCommand(QueueManager::QueueType queue, Util::CSmartPtr<Command> command)
	{
		int status = _statusOK_;
		QueueManager* queueManager = (queue == QueueManager::CommunicationQueue) ? commQManager_ : dbQManager_;
		util_thread::Mutex<>& queueMutex = (queue == QueueManager::CommunicationQueue) ? commQMutex_ : dbQMutex_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Queuing Command (queue = " +
				QueueManager::getTypeName(queue) + ", command type = " + command->getTypeName() + ", target = " +
				util_string::inttostring(command->getTargetIdentifier()) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.53) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


			if ( queue == QueueManager::CommunicationQueue && !scaleOnline_ )
			{
				/* If the command has a retry timer associated with it, remove it. */
				if ( command->getRetryTimerId() )
				{
					TimerManager::getInstance()->removeTimer(command->getRetryTimerId());
					command->setRetryTimerId(0);
				}


				/* If the command has a processed message associated with it, dispatch it. */
				Util::CSmartPtr<ProcessedMessage> completionMessage = command->getCompletionMessage();
				if ( completionMessage.notNull() )
				{
					completionMessage->setStatus(_statusScaleOffLineFailure_);
					msgManager_->dispatchEvent(completionMessage.get(), completionMessage->getSourceObject());
				}


				status = _statusScaleOffLineFailure_;
				break;
			}


			{
				util_thread::Lock< util_thread::Mutex<> > lock(queueMutex);

				/* (uc5.53) Push the command at the end of the corresponding queue, assigning it the
				queue sequence number (after incrementing it). */
				queueManager->queueCommand(command);


				/* If the corresponding Synchronizer processing flag is clear, then execute "Use
				Case 5.54: Executing command", passing a reference ot the queue. */
				if ( !queueManager->isSynchronizerProcessing() )
					status = usecaseExecutingCommand(queue);
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Queuing Command (command type = " +
				command->getTypeName() + ") completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Queuing Command (queue = " +
				QueueManager::getTypeName(queue) + ", command type = " + command->getTypeName() +
				") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.54: Executing command". */
	int SynchronizationManager::usecaseExecutingCommand(QueueManager::QueueType queue)
	{
		int status = _statusOK_;
		QueueManager* queueManager = (queue == QueueManager::CommunicationQueue) ? commQManager_ : dbQManager_;
//		util_thread::Mutex<>& queueMutex = (queue == QueueManager::CommunicationQueue) ? commQMutex_ : dbQMutex_;


		/* (uc5.54) Pre-conditions: The synchronizer is running but not processing any
		command; there is at least one command in the queue. */
    #if defined(LINUX_FIX)
		Assert(!queueManager->isSynchronizerProcessing(), Util::CPlatformException());
		Assert(queueManager->size() > 0, Util::CPlatformException());
    #else
		Assert(!queueManager->isSynchronizerProcessing(), Util::CNTException());
		Assert(queueManager->size() > 0, Util::CNTException());
    #endif


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Executing Command (queue = " +
				QueueManager::getTypeName(queue) + "): Starting");
#endif


		if ( queue == QueueManager::CommunicationQueue )
		{
			int doFakeLoop = 1;
        #if defined(LINUX_FIX)
            while (doFakeLoop-- == 1)   // This is not meant to be a loop
        #else
            while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
        #endif
			{
				/* (uc5.54) If the Stopping Synchronization Manager flag is set, then end the execution
				of this use case. */
				if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


				{
					bool executingCommand = false;
//					util_thread::Lock< util_thread::Mutex<> > lock(queueMutex);


					/* (uc5.54) Set the corresponding Synchronizer processing flag. */
					queueManager->setSynchronizerProcessing(true);


					/* (uc5.54) For each command in the queue, perform the following steps: */
					int lastCommandSeqNumber = Command::_invalidSequenceNumber_;
					QueueManager::QueueIterator iterator = queueManager->begin();
					while ( iterator != queueManager->end() && (*iterator)->getSequenceNumber() != lastCommandSeqNumber )
					{
						/* (uc5.54) Determine the identifier of the scale this command targets and
						retrieve its record. */
						int scaleId = (*iterator)->getTargetIdentifier();
						Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->findScaleByIdentifier(scaleId));
						if ( scale.notNull() )
						{
							/* (uc5.54) If the scale is marked as Retrying, then perform the following steps: */
							if ( scale->isStateRetrying() )
							{
								if ( !((*iterator)->isRetryingState() && (*iterator)->isRetryingReadyToExecute()) )
								{
									Util::CSmartPtr<Command> command(*iterator);
									if ( command->getSequenceNumber() == lastCommandSeqNumber )
									{
#ifndef EVOL_NDEBUG
										Debug::trace("SynchronizationManager", "All commands are for scales in retrying state. "
											"No command executed.");
#endif

										break;    // nothing to execute (all commands are for retrying scales)
									}
									else
									{
										/* (uc5.54) Move this command to the end of the queue. */
										if ( lastCommandSeqNumber == Command::_invalidSequenceNumber_ )
											lastCommandSeqNumber = command->getSequenceNumber();

#ifndef EVOL_NDEBUG
										Debug::trace("SynchronizationManager", "Moving command to bottom of Communication queue "
											" because the target scale is in retrying state.");
#endif

										queueManager->removeCommand(iterator);
										queueManager->queueCommand(command);
									}
								}
								else
									executingCommand = true;
							}
							else
								executingCommand = true;


							if ( executingCommand )
							{
								/* (uc5.54) If the scale is marked as off-line, then ... [?] */
								/* (TODO) we probably do not want to do anything here so that we can "ping"
								the scale to try to bring it back on-line */


								/* (uc5.54) Mark the command as Executing. */
								(*iterator)->setExecutingState();


								/* (uc5.54) Trigger the execution of "Use Case 6.54: Executing command" for the corresponding
								Synchronizer, passing the command. */
								queueManager->sendCommandForExecution(*iterator);


								/* (uc5.54) End the execution of this use case. */
								break;
							}
						}
						else
						{
							Debug::trace("SynchronizationManager", "Could not find the scale this command targets. Removing it.");

							queueManager->removeCommand(iterator);
						}


						/* (uc5.54) Skip to the next command. */
						iterator = queueManager->begin();
					}


					/* (uc5.54) Clear the Communication Synchronizer processing flag. [Rationale: all
					commands in the queue, if any, are being retried.] */
					if ( !executingCommand )
						queueManager->clearSynchronizerProcessing();
				}
			}
		}
		else if ( queue == QueueManager::DatabaseQueue )
		{
			int doFakeLoop = 1;
        #if defined(LINUX_FIX)
            while (doFakeLoop-- == 1)   // This is not meant to be a loop
        #else
            while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
        #endif
			{
				/* (uc5.54) If the Stopping Synchronization Manager flag is set, then end the execution
				of this use case. */
				if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


				{
//					util_thread::Lock< util_thread::Mutex<> > lock(queueMutex);


					/* (uc5.54) Set the corresponding Synchronizer processing flag. */
					queueManager->setSynchronizerProcessing(true);


					/* (uc5.54) Get the first command in the queue. */
					QueueManager::QueueIterator firstCommand = queueManager->begin();


					/* (uc5.54) Mark the command as Executing. */
					(*firstCommand)->setExecutingState();


					/* (uc5.54) Trigger the execution of "Use Case 7.54: Executing command", passing the command. */
					queueManager->sendCommandForExecution(*firstCommand);
				}
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Executing Command: completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Executing Command (queue = " +
					QueueManager::getTypeName(queue) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.55: Completing command". */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processCompleteCommandMessage(Util::CSmartPtr<CompleteCommandMessage> msg)
	{
		if ( msg.notNull() )
		{
			Util::CSmartPtr<Command> command = msg->getCommand();
			usecaseCompletingCommand((QueueManager::QueueType)command->getQueue(), command);

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.55: Completing command". */
	int SynchronizationManager::usecaseCompletingCommand(QueueManager::QueueType queue, Util::CSmartPtr<Command> command)
	{
		int status = _statusOK_;
		QueueManager* queueManager = (queue == QueueManager::CommunicationQueue) ? commQManager_ : dbQManager_;
		util_thread::Mutex<>& queueMutex = (queue == QueueManager::CommunicationQueue) ? commQMutex_ : dbQMutex_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Completing Command (queue = " +
			QueueManager::getTypeName(queue) + ", command type = " + command->getTypeName() + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc5.55) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			{
				util_thread::Lock< util_thread::Mutex<> > lock(queueMutex);


				/* (uc5.55) Clear the corresponding Synchronizer processing flag. */
				queueManager->clearSynchronizerProcessing();


				/* (uc5.55) Find and remove the command from the corresponding queue. */
				if ( queueManager->removeCommand(command).null() )
					{ status = _statusFailedToRemoveCommandFromQueue_; break; }


				if ( queue == QueueManager::CommunicationQueue )
				{
					/* Mark the scale as On-line, storing the current date/time. */
					Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->findScaleByIdentifier(command->getTargetIdentifier()));
					if ( scale.notNull() && !scale->isStateActive() )
					{
						scale->setStateActive();
						getScaleManager()->saveScale(scale.get());
					}
				}


				/* (uc5.55) If a completion message was present in the command, then post it. */
				Util::CSmartPtr<ProcessedMessage> completionMessage = command->getCompletionMessage();
				if ( completionMessage.notNull() )
					msgManager_->dispatchEvent(completionMessage.get(), completionMessage->getSourceObject());


				/* (uc5.55) If the queue has more commands, then execute "Use Case 5.54: Executing
				command", passing a reference to the queue. */
				if ( queueManager->size() > 0 )
					usecaseExecutingCommand(queue);
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Completing Command (command type = " +
					command->getTypeName() + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Completing Command (queue = " +
					QueueManager::getTypeName(queue) + ", command type = " + command->getTypeName() +
					"): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.56: Retrying command". */
	Util::CSmartPtr<EvolMessage> SynchronizationManager::processRetryCommandMessage(Util::CSmartPtr<RetryCommandMessage> msg)
	{
		if ( msg.notNull() )
		{
			Util::CSmartPtr<Command> command = msg->getCommand();

			usecaseRetryingCommand1((QueueManager::QueueType)command->getQueue(), command);


			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 5.56: Retrying command". */
	int SynchronizationManager::usecaseRetryingCommand1(QueueManager::QueueType queue, Util::CSmartPtr<Command> command)
	{
		int status = _statusOK_;
		QueueManager* queueManager = (queue == QueueManager::CommunicationQueue) ? commQManager_ : dbQManager_;
//		util_thread::Mutex<>& queueMutex = (queue == QueueManager::CommunicationQueue) ? commQMutex_ : dbQMutex_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Retrying Command 1 (queue = " +
				QueueManager::getTypeName(queue) + ", command type = " + command->getTypeName() + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			if ( queue != QueueManager::CommunicationQueue )
			{ status = _statusOK_; break; }


			/* (uc5.56) If the Stopping Synchronization Manager flag is set, then end the execution
			of this use case. */
			if ( stoppingSynchronizationManager_ )
			{ status = _statusStoppingSyncMgr_; break; }


			/* (TODO) figure out the correct scope for the lock below (removing as much as possible out
			of that scope) */
			{
//				util_thread::Lock< util_thread::Mutex<> > lock(queueMutex);


				/* (uc5.56) Clear the corresponding (i.e., Communication) Synchronizer processing flag. */
				queueManager->clearSynchronizerProcessing();


				/* (uc5.56) Find and remove the command from the corresponding queue. */
				if ( queueManager->removeCommand(command).null() )
					{ status = _statusFailedToRemoveCommandFromQueue_; break; }


				/* If we are transitioning to off-line, do not perform any additional processing. */
				if ( scaleOnline_ )
				{
					/* (uc5.56) Determine the identifier of the scale this command targets and retrieve
					its record, performing the following steps: */
					Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->findScaleByIdentifier(command->getTargetIdentifier()));
					if ( scale.null() )
					{ status = _statusCannotFindSecondaryScale_; break; }


					/* (uc5.56) If the scale is marked as On-line and not marked as Retrying,
					then perform the following steps: */
					if ( !scale->isStateRetrying() )
					{
#ifndef EVOL_NDEBUG
						Debug::trace("SynchronizationManager", "Use case Retrying Command 1: target scale (id = " +
							util_string::inttostring(scale->getIdentifier()) + ") not retrying. Changing to retrying with count = 1");
#endif

					
						/* (uc5.56) Mark the scale as Retrying, storing the current date/time. */
						scale->setStateRetrying();


						/* (uc5.56) Set the scale's Retry counter to 1 and store the command's
						counter. */
						scale->setRetryCount(1);
						/* (TODO) what about the command counter (i.e. sequence number?)? probably not needed */

						getScaleManager()->saveScale(scale.get());
					}
					else
					{
						int retryCount = scale->getRetryCount();

						if ( retryCount < retryMaximum_ )
						{
#ifndef EVOL_NDEBUG
							Debug::trace("SynchronizationManager", "Use case Retrying Command 1: target scale (id = " +
								util_string::inttostring(scale->getIdentifier()) + ") retrying. Incrementing count to " +
								util_string::inttostring(retryCount + 1));
#endif

						
							/* (uc5.56) If the scale is marked as On-line and Retrying, but the
							Retry counter is less than 3, then increment the scale's Retry counter. */
							scale->setRetryCount(retryCount + 1);

							getScaleManager()->saveScale(scale.get());
						}
						else
						{
							/* (uc5.56) If the scale is marked as On-line and Retrying, and the
							Retry counter is 3 (or greater), then perform the following steps: */
#ifndef EVOL_NDEBUG
							Debug::trace("SynchronizationManager", "Use case Retrying Command 1: target scale (id = " +
								util_string::inttostring(scale->getIdentifier()) + ") already reached maximum retries. Changing to Passive");
#endif

							/* (uc5.56) Mark the scale as Off-line, storing the current date/time. */
							scale->setStatePassive();


							/* (uc5.56) Clear any flags like Retrying, Local batch present, etc. */
							/* (TODO) should we clear all these flags? at least we would be in a known state */
							scale->clearRegistering();
							scale->clearDeregistering();
							scale->clearNotifiedBatches();
							scale->clearNotifiedOnline();

							getScaleManager()->saveScale(scale.get());


							/* (uc5.56) If a completion message was present in the command, then post it. */
							Util::CSmartPtr<ProcessedMessage> completionMessage = command->getCompletionMessage();
							if ( completionMessage.notNull() )
							{
								completionMessage->setStatus(_statusTooManyRetries_);
								msgManager_->dispatchEvent(completionMessage.get(), completionMessage->getSourceObject());
							}


							/* (uc5.56) Drop this command. */
							/* [nothing to do here] */


							/* (uc5.56: For each command in the "communication" queue that targets
							the same scale, perform the following steps: ... */
							queueManager->purgeSameTargetCommands(scale->getIdentifier(), _statusScaleOffLineFailure_);


							/* (uc5.56) If the queue has more commands, then execute "Use Case 5.54: Executing
							Command", passing a reference to the queue. */
							if ( queueManager->size() )
								status = usecaseExecutingCommand(queue);


							/* (uc5.56) End the execution of this use case. */
							break;
						}
					}


					/* (uc5.56) Start a Retry command timer, a 5 min. timer, passing the command's
					sequence number. */
					command->setRetryingState();
					command->setRetryingReadyToExecute(false);
					command->setRetryTimerId(TimerManager::getInstance()->addTimer(new
							RetryTimer(getMessageReceiver(), QueueManager::CommunicationQueue, command.get(), retryCommandTimer_)));


					/* (uc5.56) Execute "Use Case 5.53: Queuing command", passing the corresponding
					queue reference and the current command. [Rationale: we queue the command to retry.] */
					status = usecaseQueuingCommand(queue, command);


					/* (uc5.56) Wait until the Retry command timer times out. */
					/* [see next function] */
				}
				else
				{
#ifndef EVOL_NDEBUG
					Debug::trace("SynchronizationManager", "Use case Retrying Command 1 (command type = " +
						command->getTypeName() + ") not retried anymore because the scale is not on-line");
#endif

					/* If a completion message was present in the command, then post it. */
					Util::CSmartPtr<ProcessedMessage> completionMessage = command->getCompletionMessage();
					if ( completionMessage.notNull() )
					{
						completionMessage->setStatus(_statusScaleOffLineFailure_);
						msgManager_->dispatchEvent(completionMessage.get(), completionMessage->getSourceObject());
					}
				}
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Retrying Command 1 (command type = " +
				command->getTypeName() + ") completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Retrying Command 1 (queue = " +
				QueueManager::getTypeName(queue) + ", command type = " + command->getTypeName() +
				") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	int SynchronizationManager::usecaseRetryingCommand2(QueueManager::QueueType queue, Command* command)
	{
		int status = _statusOK_;
		QueueManager* queueManager = (queue == QueueManager::CommunicationQueue) ? commQManager_ : dbQManager_;
		util_thread::Mutex<>& queueMutex = (queue == QueueManager::CommunicationQueue) ? commQMutex_ : dbQMutex_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Retrying Command 2 (queue = " +
				QueueManager::getTypeName(queue) + ", command type = " + command->getTypeName() + "): Starting");
#endif


		if ( queue == QueueManager::CommunicationQueue )
		{
			int doFakeLoop = 1;
        #if defined(LINUX_FIX)
            while (doFakeLoop-- == 1)   // This is not meant to be a loop
        #else
            while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
        #endif
			{
				/* (uc5.56) If the Stopping Synchronization Manager flag is set, then end the execution
				of this use case. */
				if ( stoppingSynchronizationManager_ )
				{ status = _statusStoppingSyncMgr_; break; }


				{
					util_thread::Lock< util_thread::Mutex<> > lock(queueMutex);


					queueManager->processRetryingCommand(command);
					command->setRetryingReadyToExecute(true);


					/* (uc5.56) If the corresponding Synchronizer processing flag is not set, then
					execute "Use Case 5.54: Executing command". */
					if ( !queueManager->isSynchronizerProcessing() )
						status = usecaseExecutingCommand(queue);
				}
			}
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Retrying Command 2 (command type = " +
				command->getTypeName() + ") completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Retrying Command 2 (queue = " +
				QueueManager::getTypeName(queue) + ", command type = " + command->getTypeName() +
				") failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	/* "Use Case 5.57: Emptying queue". */
	int SynchronizationManager::usecaseEmptyingQueue(QueueManager::QueueType queue)
	{
		int status = _statusOK_;
		QueueManager* queueManager = (queue == QueueManager::CommunicationQueue) ? commQManager_ : dbQManager_;
		util_thread::Mutex<>& queueMutex = (queue == QueueManager::CommunicationQueue) ? commQMutex_ : dbQMutex_;


#ifndef EVOL_NDEBUG
		Debug::trace("SynchronizationManager", "Use case Emptying Queue (queue = " + QueueManager::getTypeName(queue) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			if ( queue != QueueManager::CommunicationQueue )
			{ status = _statusOK_; break; }


			{
				util_thread::Lock< util_thread::Mutex<> > lock(queueMutex);


				/* (uc5.57) For each command in the queue that is not marked as Executing, remove
				the command from the queue. */
				if ( queueManager )
					queueManager->purgeNonExecutingCommands();


				/* Executing commands will be disposed of when the QueueManager is disposed of. */
			}


			break;
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SynchronizationManager", "Use case Emptying Queue (queue = " + QueueManager::getTypeName(queue) + "): completed successfully");
		else
			Debug::trace("SynchronizationManager", "Use case Emptying Queue (queue = " +
					QueueManager::getTypeName(queue) + "): failed (status = " + getStatusText(status) + " )");
#endif


		return status;
	}


	const std::string& SynchronizationManager::getStatusText(int status) const
	{
		switch ( status )
		{
			case _statusNoCurrentUserBatch_:               return _statusNoCurrentUserBatchText_;
			case _statusOK_:                               return _statusOKText_;
			case _statusBadParameter_:                     return _statusBadParameterText_;
			case _statusStoppingSyncMgr_:                  return _statusStoppingSyncMgrText_;
			case _statusCannotCreateBatch_:                return _statusCannotCreateBatchText_;
			case _statusBatchNotFound_:                    return _statusBatchNotFoundText_;
			case _statusCannotAbortACompleteBatch_:        return _statusCannotAbortACompleteBatchText_;
			case _statusBatchNotComplete_:                 return _statusBatchNotCompleteText_;
			case _statusCannotCompleteACompleteBatch_:     return _statusCannotCompleteACompleteBatchText_;
			case _statusCannotFindPrimaryScale_:           return _statusCannotFindPrimaryScaleText_;
			case _statusCannotFindSecondaryScale_:         return _statusCannotFindSecondaryScaleText_;
			case _statusSyncMgrNotFullyFunctioningYet_:    return _statusSyncMgrNotFullyFunctioningYetText_;
			case _statusCannotAddRecordToCompleteBatch_:   return _statusCannotAddRecordToCompleteBatchText_;
			case _statusFailedToRemoveCommandFromQueue_:   return _statusFailedToRemoveCommandFromQueueText_;
			case _statusDBException_:                      return _statusDBExceptionText_;
			case _statusScaleOffLineFailure_:              return _statusScaleOffLineFailureText_;
			case _statusCannotFindCommand_:                return _statusCannotFindCommandText_;
			case _statusTooManyRetries_:                   return _statusTooManyRetriesText_;
			case _statusWrongTypeOfBatch_:                 return _statusWrongTypeOfBatchText_;
			case _statusPrimaryScaleChanged_:              return _statusPrimaryScaleChangedText_;
			default:                                       return _statusUnknownText_;
		}
	}


	void SynchronizationManager::verifyThisScaleRecord(bool online)
	{
		int lastBatchCompleted = 0;

		thisScale_ = new ThisScale(this, &lastBatchCompleted, online);

		setLastBatchCompletedVersion(lastBatchCompleted);
	}


	void SynchronizationManager::dispatchResponseMessage(Util::CSmartPtr<EvolMessageWrapper> originalMessage, Util::CSmartPtr<EvolMessage> response)
	{
		if ( originalMessage->getSourceObject() )
			response->setSequenceNumber(originalMessage->getSequenceNumber());

		msgManager_->dispatchEvent(response.get(), originalMessage->getSourceObject());
	}


	SynchronizationDevError SynchronizationManager::filterDisplayStatus()
	{
		if ( !scaleOnline_ )
			return (SynchronizationDevError)ClearStatusUpInHere;

		return getDisplayStatus();
	}


	bool SynchronizationManager::isHQBatchAlreadyReceived(Util::CSmartPtr<hfc::Batch> batch)
	{
		return getBatchManager()->initializeSameSignatureBatchesQuery(batch->getSignature());
	}


	SynchronizationManager::ThisScale::ThisScale(SynchronizationManager* syncMgr, int* lastBatchCompleted, bool online)
		: syncMgr_(syncMgr)
	{
		/* (TODO) if this scale's record already exists in the db then we will not take note of
		whether the scale is online or not; is this important? */
		thisScale_ = syncMgr_->getScaleManager()->getThisScale();
		if ( thisScale_.null() )
		{
			thisScale_ = new hfc::Scale(online ? hfc::Scale::StateActive : hfc::Scale::StatePassive);
			thisScale_->setThisScale();
			thisScale_->setRetryCount(hfc::Batch::_invalidBatchNumber_);
			thisScale_->setVersion(*lastBatchCompleted = hfc::Batch::_invalidBatchVersion_ + 1);
			thisScale_->setVersionReceived(hfc::Batch::_invalidBatchNumber_);

			syncMgr_->getScaleManager()->saveScale(thisScale_);
		}
		else
			*lastBatchCompleted = thisScale_->getVersion();
	}


	int SynchronizationManager::ThisScale::getNewBatchNumber()
	{
		util_thread::Lock< util_thread::Mutex<> > lock(thisScaleMutex_);

		int batchNumber = thisScale_->getRetryCount() + 1;

		thisScale_->setRetryCount(batchNumber);

		Util::CSmartPtr<hfc::Scale> scale(syncMgr_->getScaleManager()->getThisScale());
		if ( scale.notNull() )
		{
			scale->setRetryCount(batchNumber);
			syncMgr_->getScaleManager()->saveScale(scale.get());
		}

		return batchNumber;
	}


	int SynchronizationManager::ThisScale::getNewBatchVersionNumber()
	{
		util_thread::Lock< util_thread::Mutex<> > lock(thisScaleMutex_);

		int versionNumber = thisScale_->getVersion() + 1;

		thisScale_->setVersion(versionNumber);

		Util::CSmartPtr<hfc::Scale> scale(syncMgr_->getScaleManager()->getThisScale());
		if ( scale.notNull() )
		{
			scale->setVersion(versionNumber);
			syncMgr_->getScaleManager()->saveScale(scale.get());
		}

		return versionNumber;
	}


	void SynchronizationManager::ThisScale::setCurrentUserBatchNumber(int batchNumber)
	{
		util_thread::Lock< util_thread::Mutex<> > lock(thisScaleMutex_);

		thisScale_->setVersionReceived(batchNumber);

		Util::CSmartPtr<hfc::Scale> scale(syncMgr_->getScaleManager()->getThisScale());
		if ( scale.notNull() )
		{
			scale->setVersionReceived(batchNumber);
			syncMgr_->getScaleManager()->saveScale(scale.get());
		}
	}


}
