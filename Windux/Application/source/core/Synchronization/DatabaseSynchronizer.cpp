#include "DatabaseSynchronizer.h"


#include "ScaleManager.h"
#include "Scale.h"
#include "BatchManager.h"
#include "Batch.h"
#include "SynchronizationChangeManager.h"
#include "SynchronizationChange.h"

#include "EvolMessage.h"
#include "SessionManager.h"
#include "Configuration.h"

#include "DBException.h"


#include "Messages/SynchronizationDevEvent.h"
#include "Messages/DeleteBatchMessage.h"
#include "Messages/DeleteBatchProcessedMessage.h"
#include "Messages/RetryCommandMessage.h"
#include "Messages/ExecuteCommandMessage.h"
#include "Messages/CompleteCommandMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"

#include "Commands/PurgeBatchesCommand.h"
#include "Commands/AbortBatchCommand.h"
#include "Commands/ApplyBatchCommand.h"
#include "Commands/DeleteBatchCommand.h"
#include "Commands/ApplyAndDeleteBatchCommand.h"

#include "DBCategoriesRM.h"
#include "DBClassesRM.h"
#include "DBCoolCategoriesRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolTextsRM.h"
#include "DBDevicesRM.h"
#include "DBLabelTextsRM.h"
#include "DBNutritionsRM.h"
#include "DBProductsRM.h"
#include "DBProductTextsRM.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKeyFeaturesRM.h"
#include "DBSoftKeyGroupsRM.h"
#include "DBSoftKeyStylesRM.h"
#include "DBSpecialProgramsRM.h"
#include "DBStreamingGraphicsRM.h"
#include "DBTransactionTypesRM.h"
#include "DBEmailAlertsRM.h"
#include "DBLabelsRM.h"
#include "DBProductStatisticsRM.h"
#include "DBROMLabelsRM.h"

#include "SessionMessage.h"
#include "OperatorManager.h"
#include "OperatorNoteManager.h"
#include "CategoryManager.h"
#include "ClassManager.h"
#include "ConfigurationManager.h"
#include "COOLManager.h"
#include "EmailAlertsManager.h"
#include "GenericLabelTextManager.h"
#include "GraphicManager.h"
#include "LabelTypeManager.h"
#include "NutrifactsManager.h"
#include "PeripheralManager.h"
#include "ProductManager.h"
#include "ProductDefaultManager.h"
#include "ProductTextManager.h"
#include "SoftkeyManager.h"
#include "SpecialsManager.h"
#include "StreamingGraphicManager.h"
#include "TransactionTypeManager.h"
#include "TransactionManager.h"
#include "Transaction.h"

#include "Debug.h"
#include "util/e_string/utilstring.h"
#include "util/thread/thread.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int DatabaseSynchronizer::_dataType_ = 0;
	const string DatabaseSynchronizer::_dataTypeName_("synchronization::DatabaseSynchronizer");
#endif


	DatabaseSynchronizer::DatabaseSynchronizer(Hobart::CEvolMsgReceiver* syncManager)
		: Synchronizer(syncManager, "DatabaseSynchronizer"), thread_(0), command_(0)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		msgManager_->RestartThread();			

		thread_ = new Util::CTLSThread(*this, THREAD_PRIORITY_NORMAL, 0, "DatabaseSynchronizer Thread", false, true, true);
	}


	DatabaseSynchronizer::~DatabaseSynchronizer()
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


	void DatabaseSynchronizer::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
	{
		if ( msg.notNull() )
		{
			if ( msg->getDeviceType() == SynchronizationDev && !stoppingSynchronizer_ )
			{

				UINT32 result = SessionManager::getInstance()->requestToContinue(msgManager_->GetThreadID(), msgManager_->IsASuperThread());
				restoreDB(result & (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED));


				if ( msg->getEventType() == ExecuteCommandEvent )
				{
						Util::CSmartPtr<EvolMessage> response =
								processExecuteCommandMessage(Util::CSmartPtr<ExecuteCommandMessage>(new ExecuteCommandMessage(msg->Data())));

						if ( response.notNull() )
							msgManager_->dispatchEvent(response.get(), qMsgManager_);
				}
				else
				{
					Debug::trace("DatabaseSynchronizer", "processEvent: got an unknown SynchronizationDev message (event = " +
							util_string::uinttostring(msg->getEventType()) + ")");
				}

				//processEventThreadTopOfLoopEvent_.EventSet();
				SessionManager::getInstance()->changeBlockabilityStatus(msgManager_->GetThreadID(), true, msgManager_->IsASuperThread());

			}
			else
			{
				Debug::trace("DatabaseSynchronizer", "processEvent: could not process message (device = " +
						util_string::inttostring(msg->getDeviceType()) + ", event = " + util_string::uinttostring(msg->getEventType()) +
						") while Synchronizer " + std::string(stoppingSynchronizer_ ? "IS" : "is NOT") + " stopping");
			}
		}
		else
		{
			Debug::trace("DatabaseSynchronizer", "processEvent: got a NULL message");
		}
	}


	DWORD DatabaseSynchronizer::Run()
	{
		int status = _statusNothingToDo_;


		if ( !stoppingSynchronizer_ )
		{
#ifndef EVOL_NDEBUG
			Debug::trace("DBSynchronizer", "Starting to process command of type " + command_->getTypeName());

			util_time::Time t0 = util_time::Time();
#endif

			try
			{
				switch ( command_->getType() )
				{
					case Command::PurgeBatchesCommandType:
					{
						status = usecasePurgingBatches();

						break;
					}
					case Command::AbortBatchCommandType:
					{
						const AbortBatchCommand* abort = dynamic_cast<const AbortBatchCommand *>(command_.get());
						if ( abort )
						{
							status = usecaseAbortingBatch(abort->getTargetIdentifier());
						}

						break;
					}
					case Command::ApplyBatchCommandType:
					{
						const ApplyBatchCommand* apply = dynamic_cast<const ApplyBatchCommand *>(command_.get());
						if ( apply )
						{
							/* (SYNCSTATUS) notify UI that we are starting to apply changes.
							The message will be sent once per batch. */
							{
								NewSynchronizationStatusMessage message(ApplyingChangesSyncStatus);
								msgManager_->dispatchEvent(&message);
							}

							status = usecaseApplyingBatch(apply->getTargetIdentifier());

							/* (SYNCSTATUS) notify UI that we completed applying a batch.
							The message will be sent once per batch. */
							{
								NewSynchronizationStatusMessage message(SynchronizationActiveSyncStatus);
								msgManager_->dispatchEvent(&message);
							}
						}

						break;
					}
					case Command::DeleteBatchCommandType:
					{
						const DeleteBatchCommand* del = dynamic_cast<const DeleteBatchCommand *>(command_.get());
						if ( del )
						{
							status = usecaseDeletingBatch(del->getTargetIdentifier());
						}

						break;
					}
					case Command::ApplyAndDeleteBatchCommandType:
					{
						const ApplyAndDeleteBatchCommand* appdel = dynamic_cast<const ApplyAndDeleteBatchCommand *>(command_.get());
						if ( appdel )
						{
							status = usecaseApplyingDeletingBatch(appdel->getTargetIdentifier());
						}

						break;
					}
                #if defined(LINUX_FIX)
                    default:
                    {
                        // Do nothing
                        break;
                    }
                #endif
				}
			}
			catch ( edb::DBException& dbe )
			{
				showDatabaseException("DBSynchronizer", dbe);
				status = _statusDBException_;
			}


#ifndef EVOL_NDEBUG
			util_time::Time t1 = util_time::Time();

			Debug::trace("DBSynchronizer", "Performed task in " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) +
					" milliseconds");
#endif
		}
		else
			status = _statusStoppingSynchronizer_;

		command_->setStatus(status);

		if ( status != _statusOK_ )
		{
			Debug::trace("DBSynchronizer", "FAILED TO EXECUTE A DB SYNCHRONIZER COMMAND ?????????????");

			/* We do not retry database commands. There is no reason why they would fail, but it they
			fail, there is nothing we can do right now. */
		}


		CompleteCommandMessage message(command_);

		/* (TODO) do not dispatch the event; use the proxy to guarantee that upon return the
		synchronizerProcessing_ flag is false so that no further calls to pauseProcessing
		will be issued from now own (we are setting the event below). */
		msgManager_->dispatchEvent(&message, qMsgManager_);


		command_ = 0;


		/* This is the last thing we do so a call to pauseProcessing won't block for ever. */

		return 0;
	}


	void DatabaseSynchronizer::waitUntilRunThreadEnds()
	{
		msgManager_->ShutdownNicely();
		thread_->WaitForCompletion();
	}


	/* "Use Case 7.33: Purging batches". */
	int DatabaseSynchronizer::usecasePurgingBatches()
	{
		int status = _statusOK_;
		int delayPurgingBatches = Configuration::getInstance()->toIntCfgI(CfgInternal::DELAYPURGINGBATCHES);
		int hoursBeforeRemovingDeletedBatches = Configuration::getInstance()->toIntCfgI(CfgInternal::HOURSBEFOREREMOVINGDELETEDBATCHES);


#ifndef EVOL_NDEBUG
		Debug::trace("DBSynchronizer", "Use case Purging Batches: Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc7.33) If the Stopping Synchronizer flag is set, then end the execution of this use
			case. */
			if ( stoppingSynchronizer_ )
				{ status = _statusStoppingSynchronizer_; break; }


			/* (uc7.33) For each batch marked as complete in the database, in ascending order of
			version number, perform the following steps. */
			bool done = false;
			getBatchManager()->initializeCompleteBatchesQuery(hfc::Batch::_invalidBatchVersion_);
			Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->getFirstCompleteBatch(hfc::Batch::_invalidBatchVersion_));
			for ( ; batch.notNull() && !done; batch = getBatchManager()->getNextCompleteBatch(hfc::Batch::_invalidBatchVersion_) )
			{
				/* This is like the main loop of the DatabaseSynchronizer's thread when it is
				executing this type of command. Thus here we perform the corresponding type of
				synchronization with pausing processing functionality. */
				UINT32 result = SessionManager::getInstance()->requestToContinue(thread_->GetThreadID(),thread_->IsASuperThread());
				restoreDB(result & (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED));


				if ( stoppingSynchronizer_ ) break;


				// Give other threads (tasks) a chance to do something else
            #if defined(LINUX_FIX)
                PAL_absclass::PAL_sleep( delayPurgingBatches );
            #else
				Sleep( delayPurgingBatches ); 
            #endif


				/* If the batch is marked as Pending deletion it will be deleted very soon. Let it happen. */
				if ( batch->isPendingDeletion() )
					continue;


				/* If the batch has been marked as deleted, but is not actually deleted yet, then
				actually delete the batch if a certain amount of time has elapsed. */
				if ( batch->isDeleted() )
				{
					HobDateTime deletedDT(batch->getDeletedDateTime());
					deletedDT.incrementHours((int)hoursBeforeRemovingDeletedBatches);
					if ( deletedDT < HobDateTime() )
					{
						DeleteBatchMessage message(batch->getNumber(), DeleteBatchProcessedMessage::DeleteChangesAndBatch);
						getMessageReceiver()->dispatchEvent(&message);
					}

					continue;
				}


				/* (uc7.33) If the batch has not been applied to the database, then end the execution
				of this use case. */
				if ( !batch->isApplied() )
					{ status = _statusOK_; break; }


				/* (uc7.33) For each Secondary scale in the database, perform the following steps: */
				getScaleManager()->initializeSecondaryScalesQuery();
				Util::CSmartPtr<hfc::Scale> scale(getScaleManager()->getFirstSecondaryScale());
				for ( ; scale.notNull() && !done; scale = getScaleManager()->getNextSecondaryScale() )
				{
					/* There is no need to do different things based on the state of the scale. [Other
					areas of the Synchronization do that already, like deregistering the scale happens
					when trying to synchronize all scales (which brought us here) and synchronizing
					the scale happens when new changes are received (synchronizing all scales).] Thus,
					we merely check the version sent to any Secondary scale last: if the current batch
					has been sent to all the scales, regardless their state, then delete it, otherwise
					keep it. */
					if ( scale->getVersion() < batch->getVersion() )
					{
						status = _statusOK_;
						done = true;
					}
				}

				if ( !done )
				{
					/* (uc7.33) Trigger the execution of "Use Case 5.46: Deleting batch", passing
					the batch identifier and the "delete all changes" parameter if HQ batch or the
					"delete batch" parameter if a user batch. */
					DeleteBatchMessage message(batch->getNumber(), batch->getType() == hfc::Batch::TypeHQ ? DeleteBatchProcessedMessage::DeleteChangesOnly : DeleteBatchProcessedMessage::DeleteChangesAndBatch);
					getMessageReceiver()->dispatchEvent(&message);
				}
			}

			if ( stoppingSynchronizer_ )
				{ status = _statusStoppingSynchronizer_; break; }
		}



#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("DBSynchronizer", "Use case Purging Batches: completed successfully");
		else
			Debug::trace("DBSynchronizer", "Use case Purging Batches: failed (status = " +
					util_string::inttostring(status) + " )");
#endif


		return status;
	}


	/* "Use Case 7.42: Aborting batch". */
	int DatabaseSynchronizer::usecaseAbortingBatch(int batchNumber)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("DBSynchronizer", "Use case Aborting Batch: Starting (batchNumber = " +
				util_string::inttostring(batchNumber) + ")");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{

			UINT32 result = SessionManager::getInstance()->requestToContinue(thread_->GetThreadID(),thread_->IsASuperThread());
			result &= (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED);

			restoreDB(result);

			/* (uc7.42) If the Stopping Synchronizer flag is set, then end the execution of this use
			case. */
			if ( stoppingSynchronizer_ )
				{ status = _statusStoppingSynchronizer_; break; }


			/* (uc7.42) Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }

			
			/* (uc7.42) Remove all change records associated with the batch. */
			getSynchronizationChangeManager()->deleteBatchChanges(batchNumber);


			/* (uc7.42) Remove the batch itself. */
			getBatchManager()->deleteBatch(batch);
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("DBSynchronizer", "Use case Aborting Batch (batchNumber = " + util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("DBSynchronizer", "Use case Aborting Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + util_string::inttostring(status) + " )");
#endif


		return _statusOK_;
	}

		
	/* "Use Case 7.45: Applying batch". */
	int DatabaseSynchronizer::usecaseApplyingBatch(int batchNumber)
	{
		int status = _statusOK_;
		int recordsPerApplyIteration = Configuration::getInstance()->toIntCfgI(CfgInternal::RECORDSPERAPPLYITERATION);
		int delayPerApplyIteration = Configuration::getInstance()->toIntCfgI(CfgInternal::DELAYPERAPPLYITERATION);


#ifndef EVOL_NDEBUG
		Debug::trace("DBSynchronizer", "Use case Applying Batch: Starting (batchNumber = " +
				util_string::inttostring(batchNumber) + ")");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{
			/* (uc7.45) If the Stopping Synchronizer flag is set, then end the execution of this use
			case. */
			if ( stoppingSynchronizer_ )
				{ status = _statusStoppingSynchronizer_; break; }


			/* (uc7.45) Retrieve the batch record from the database. */
			Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusNotFound_; break; }


			/* (uc7.45) For each record associated with the identified batch, apply the batch's
			associated changes to the actual database. */
#ifndef EVOL_NDEBUG
			util_time::Time t0;
#endif
			if ( getSynchronizationChangeManager()->initializeBatchChangesQuery(batch->getNumber()) )
			{
#ifndef EVOL_NDEBUG
				util_time::Time t1;
				Debug::trace("DBSynchronizer", "Time elapsed during initializeBatchChangesQuery was " +
						util_string::inttostring((t1 - t0).getTotalMilliseconds()) + "msecs.");
#endif

				Util::CSmartPtr<hfc::SynchronizationChange> change(getSynchronizationChangeManager()->getFirstChangeFromBatch(batch->getNumber()));

#ifndef EVOL_NDEBUG
				util_time::Time t2;
				Debug::trace("DBSynchronizer", "Time elapsed during getFirstChangeFromBatch was " +
						util_string::inttostring((t2 - t1).getTotalMilliseconds()) + "msecs.");
#endif

				int counter = 0;
				for ( ; change.notNull(); change = getSynchronizationChangeManager()->getNextChangeFromBatch(batch->getNumber()) )
				{
					/* This is like the main loop of the DatabaseSynchronizer's thread when it is
					executing this type of command. Thus here we perform the corresponding type of
					synchronization with pausing processing functionality. */
					UINT32 result = SessionManager::getInstance()->requestToContinue(thread_->GetThreadID(),thread_->IsASuperThread());
					result &= (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED);

					restoreDB(result);


					/* (uc7.45) If the Stopping Synchronizer flag is set, then end the execution of
					this use case. */
					if ( stoppingSynchronizer_ ) break;

					applyChangeToDB(this, change, batch);
					++counter;


					if ( counter % recordsPerApplyIteration == 0 )
					{
						// Give other threads (tasks) a chance to do something else
                    #if defined(LINUX_FIX)
                        PAL_absclass::PAL_sleep( delayPerApplyIteration );
                    #else
						Sleep( delayPerApplyIteration ); 
                    #endif
					}
				}

#ifndef EVOL_NDEBUG
				util_time::Time t3;
				Debug::trace("DBSynchronizer", "Time elapsed applying all change records was " +
						util_string::inttostring((t3 - t2).getTotalMilliseconds()) + "msecs. for a total of " +
						util_string::inttostring(counter) + " records applied.");
#endif


				/* Now call the finishedProcessingChanges method of each HFC manager */
				DataManager* synchronizedDM = 0;
				syncListIterator_type synchronizedDMIterator = beginSynchronizedDataManager();
				while ( synchronizedDMIterator != endSynchronizedDataManager() )
				{
					synchronizedDM = getExistingDataManager(synchronizedDMIterator);
					if ( synchronizedDM )
						synchronizedDM->finishedProcessingChanges();

					synchronizedDMIterator = nextSynchronizedDataManager(synchronizedDMIterator);

				}
			}

			if ( stoppingSynchronizer_ )
				{ status = _statusStoppingSynchronizer_; break; }

			
			/* (uc7.45) Return a status of success. */
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("DBSynchronizer", "Use case Applying Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("DBSynchronizer", "Use case Applying Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + util_string::inttostring(status) + " )");
#endif


		return status;
	}


	/* "Use Case 7.46: Deleting batch". */
	int DatabaseSynchronizer::usecaseDeletingBatch(int batchNumber)
	{
		int status = _statusOK_;
		int delayPerDeleteIteration = Configuration::getInstance()->toIntCfgI(CfgInternal::DELAYPERDELETEITERATION);


#ifndef EVOL_NDEBUG
		Debug::trace("DBSynchronizer", "Use case Deleting Batch (batchNumber = " + util_string::inttostring(batchNumber) + "): Starting");
#endif


		int doFakeLoop = 1;
    #if defined(LINUX_FIX)
		while (doFakeLoop-- == 1)   // This is not meant to be a loop
    #else
		while ( bool thisIsNotMeantToBeALoop = (doFakeLoop-- == 1) )
    #endif
		{



			/* (uc7.46) Retrieve the batch record from the database through the batch identifier. */
			Util::CSmartPtr<hfc::Batch> batch(getBatchManager()->findBatchNumber(batchNumber));
			if ( batch.null() )
				{ status = _statusBatchNotFound_; break; }

			
			if ( getSynchronizationChangeManager()->initializeBatchChangesQuery(batchNumber) )
			{
				Util::CSmartPtr<hfc::SynchronizationChange> change(getSynchronizationChangeManager()->getFirstChangeFromBatch(batchNumber));
				for ( ; change.notNull(); change = getSynchronizationChangeManager()->getNextChangeFromBatch(batchNumber) )
				{
					/* This is like the main loop of the DatabaseSynchronizer's thread when it is
					executing this type of command. Thus here we perform the corresponding type of
					synchronization with pausing processing functionality. */

					UINT32 result = SessionManager::getInstance()->requestToContinue(thread_->GetThreadID(),thread_->IsASuperThread());
					result &= (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED);

					restoreDB(result);

					/* (uc7.46) If the Stopping Synchronizer flag is set, then end the execution of
					this use case. */
					if ( stoppingSynchronizer_ ) break;


					/* (uc7.46) For each record associated with the identified batch, delete that change
					record from the database. */
					getSynchronizationChangeManager()->deleteSynchronizationChange(change.get());


					// Give other threads (tasks) a chance to do something else
                #if defined(LINUX_FIX)
                    PAL_absclass::PAL_sleep( delayPerDeleteIteration );
                #else
					Sleep( delayPerDeleteIteration );
                #endif
				}
			}


			/* (uc7.46) Return a status of success. */
		}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("DBSynchronizer", "Use case Deleting Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): completed successfully");
		else
			Debug::trace("DBSynchronizer", "Use case Deleting Batch (batchNumber = " +
					util_string::inttostring(batchNumber) + "): failed (status = " + util_string::inttostring(status) + " )");
#endif


		return status;
	}


	/* "Use Case 7.47: Applying and deleting batch". */
	int DatabaseSynchronizer::usecaseApplyingDeletingBatch(int batchId)
	{
		int status = _statusOK_;


#ifndef EVOL_NDEBUG
		Debug::trace("DBSynchronizer", "Use case Applying and Deleting Batch (batchId = " + util_string::inttostring(batchId) + "): Starting");
#endif


		status = usecaseApplyingBatch(batchId);
		if ( status == _statusOK_ )
			status = usecaseDeletingBatch(batchId);


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("DBSynchronizer", "Use case Applying and Deleting Batch (batchId = " +
					util_string::inttostring(batchId) + ") completed successfully");
		else
			Debug::trace("DBSynchronizer", "Use case Applying and Deleting Batch (batchId = " +
					util_string::inttostring(batchId) + ") failed (status = " +
					util_string::inttostring(status) + " )");
#endif


		return status;
	}


	/* "Use Case 7.54: Executing command". */
	Util::CSmartPtr<EvolMessage> DatabaseSynchronizer::processExecuteCommandMessage(Util::CSmartPtr<ExecuteCommandMessage> msg)
	{
		if ( msg.notNull() )
		{
			usecaseExecutingCommand(msg->getCommand());

			return Util::CSmartPtr<EvolMessage>(0);
		}
		else
			return Util::CSmartPtr<EvolMessage>(0);
	}


	/* "Use Case 7.54: Executing command". */
	int DatabaseSynchronizer::usecaseExecutingCommand(Util::CSmartPtr<Command> command)
	{
		if ( command.notNull() )
		{
			{
				Debug::trace("DatabaseSynchronizer", "Use Case Executing Command: "
						"!!!??? the thread is still running from a previous instance !!!??? Waiting from completion.");

				thread_->WaitForCompletion();

				Debug::trace("DatabaseSynchronizer", "Use Case Executing Command: "
						"!!!??? the thread finished running from a previous instance !!!??? Continuing ...");
			}

			command_ = command;

			thread_->RestartThread();
		}
		else
		{
			Debug::trace("DatabaseSynchronizer", "Use Case Executing Command: !!!??? no command was received !!!???");

			CompleteCommandMessage message(command);

			msgManager_->dispatchEvent(&message, qMsgManager_);
		}

		return _statusOK_;
	}


	/* "Use Case 7.58: Resuming processing". */
	int DatabaseSynchronizer::usecaseResumingProcessing()
	{
		pausingProcessing_ = false;

		return _statusOK_;
	}


	/* "Use Case 7.59: Pausing processing". */
	int DatabaseSynchronizer::usecasePausingProcessing(int /*reason*/)
	{
		/* Synchronize with the top of the run thread's and the processEvent's main loop to ensure
		that they will not execute code in their main loops until the resumeProcessing arrives. */
		util_time::Time t0 = util_time::Time();

		pausingProcessing_ = true;

		/* At this point the run thread and the processEvent thread are both either blocked or not
		executing code inside their main loops. */

		util_time::Time t1 = util_time::Time();
		Debug::trace("Synchronizer", "usecasePausingProcessing: Waited " + 
				util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

		return _statusOK_;
	}


	void DatabaseSynchronizer::dispatchMessage(EvolMessage *message)
	{
		if (message)
			msgManager_->dispatchEvent(message);
	}


	int DatabaseSynchronizer::applyChangeToDB(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change, Util::CSmartPtr<hfc::Batch> batch)
	{
		if ( change.notNull() )
		{
#ifndef EVOL_NDEBUG
			Debug::trace("DBSynchronizer", "Going to apply a change to the database (device = " +
					util_string::inttostring(change->getDevice()) + ", event = " +
					util_string::inttostring(change->getEvent()) + "): " + change->getChange());
#endif

					
			switch ( change->getDevice() )
			{
				case ProductDev:            return applyProductChange(container, change);
				case ClassDev:              return applyClassChange(container, change);
				case ExpandedTextDev:       return applyExpandedTextChange(container, change);
				case SpecialMessageDev:     return applySpecialMessageChange(container, change);
				case NutrifactsDev:         return applyNutrifactsChange(container, change);
				case ProductNotesDev:       return applyProductNotesChange(container, change);
				case MarqueeTextDev:        return applyMarqueeChange(container, change);
				case CategoryDev:           return applyCategoryChange(container, change);
				case SpecialsDev:           return applySpecialsChange(container, change);
				case SoftkeyDev:            return applySoftkeyChange(container, change);
				case COOLDev:               return applyCOOLChange(container, change);
				case TransactionDev:		return applyTransactionChange(container, change, batch->getScaleId());
				case TransactionTypeDev:    return applyTransactionTypeChange(container, change);
				case ProductDefaultsDev:    return applyProductDefaultsChange(container, change);
				case OperatorDev:           return applyOperatorChange(container, change);
				// The following is not used yet; currently Operator Note messages are events using OperatorDev as their Device.
				case OperatorNotesDev:      return applyOperatorNotesChange(container, change);
				case GenericTextDev:        return applyGenericTextChange(container, change);
				case LabelTypeDev:          return applyLabelTypeChange(container, change);
				case GraphicDev:            return applyGraphicChange(container, change);
				case ConfigurationDev:      return applyConfigurationChange(container, change);
				case EmailAlertsDev:        return applyEmailAlertsChange(container, change);

				// lap todo: Are we going to have a different ProductTextDev definition for each of the new ProductTexts,
				// or use one device, but have multiple event groupings
				case ProductText5Dev:       return applyProductTextChange(psttProductText5, container, change);
				case ProductText6Dev:       return applyProductTextChange(psttProductText6, container, change);
				case ProductText7Dev:       return applyProductTextChange(psttProductText7, container, change);
				case ProductText8Dev:       return applyProductTextChange(psttProductText8, container, change);
				case ProductText9Dev:       return applyProductTextChange(psttProductText9, container, change);

				default:
				{
#ifndef EVOL_NDEBUG
					Debug::trace("DBSynchronizer", "Trying to apply an unknown change to the database (device = " +
							util_string::inttostring(change->getDevice()) + ", event = " +
							util_string::inttostring(change->getEvent()) + "). Ignoring.");
#endif
				}
			}
		}
	
		return _statusNotFound_;
	}


	int DatabaseSynchronizer::applyProductChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getProductManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyOperatorChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getOperatorManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyGraphicChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getGraphicManager()->handleMessage(sm);

			delete sm;
		}
		return _statusOK_;
	}


	int DatabaseSynchronizer::applyExpandedTextChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getProductTextManager(psttExpandedText)->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applySpecialMessageChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getSpecialMessageManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyProductNotesChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getProductNoteManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyProductTextChange(ProdSupportTextType type, HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getProductTextManager(type)->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyClassChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getClassManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyLabelTypeChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getLabelTypeManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyConfigurationChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getConfigurationManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyNutrifactsChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getNutrifactsManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyGenericTextChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getGenericLabelTextManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyCategoryChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getCategoryManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applySpecialsChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getSpecialsManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyOperatorNotesChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getOperatorNoteManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyProductDefaultsChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getProductDefaultManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyEmailAlertsChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getEmailAlertsManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyCOOLChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getCOOLManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	// Handles Softkeys, SoftkeyFeatures, SoftkeyGroups, and SoftkeyStyles
	int DatabaseSynchronizer::applySoftkeyChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getSoftkeyManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyMarqueeChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getMarqueeTextManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}


	int DatabaseSynchronizer::applyTransactionTypeChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());
		if ( sm )
		{
			if ( container )
				container->getTransactionTypeManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}

	int DatabaseSynchronizer::applyTransactionChange(HFCManagerContainer* container, Util::CSmartPtr<hfc::SynchronizationChange> change, int scaleID)
	{
		const std::string& changeString = change->getChange();
		SessionMessage* sm = SessionMessage::create(changeString.data(), (int)changeString.length(), change->getFormat());

		if ( sm )
		{
			hfc::Transaction* transaction = dynamic_cast<hfc::Transaction*>(sm->getHfcObject());

			if (transaction)
			{
				transaction->setScaleId(scaleID);
			}

			if ( container )
				container->getTransactionManager()->handleMessage(sm);

			delete sm;
		}

		return _statusOK_;
	}

}
