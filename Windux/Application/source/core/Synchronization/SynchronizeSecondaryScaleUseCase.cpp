#include "SynchronizeSecondaryScaleUseCase.h"


#include "SynchronizationManagerProxy.h"


#include "Configuration.h"
#include "HeaderRecord.h"
#include "ScaleManager.h"
#include "BatchManager.h"
#include "SynchronizationChange.h"
#include "SynchronizationChangeManager.h"


#include "NotifyLocalBatches.h"
#include "ReadBatchesRecord.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/ProcessRemoteBatchMessage.h"
#include "Messages/AddRecordToBatchMessage.h"
#include "Messages/ApplyBatchMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int SynchronizeSecondaryScaleUseCase::_dataType_ = 0;
	const string SynchronizeSecondaryScaleUseCase::_dataTypeName_("synchronization::SynchronizeSecondaryScaleUseCase");
#endif


	SynchronizeSecondaryScaleUseCase::SynchronizeSecondaryScaleUseCase(CommunicationSynchronizer* synchronizer, int scaleId)
		: synchronizer_(synchronizer), scaleId_(scaleId), startingVersionNumber_(-1), uploadingWholeDatabase_(false), operationFailed_(false)
	{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif

		recordsPerSyncIteration_ = Configuration::getInstance()->toIntCfgI(CfgInternal::RECORDSPERSYNCITERATION);
		delayPerSyncIteration_ = Configuration::getInstance()->toIntCfgI(CfgInternal::DELAYPERSYNCITERATION);
	}


	SynchronizeSecondaryScaleUseCase::~SynchronizeSecondaryScaleUseCase()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	bool SynchronizeSecondaryScaleUseCase::doInitialize()
	{
		/* (SYNCSTATUS) notify UI that we are about to Synchronize a Secondary scale. */
		NewSynchronizationStatusMessage message(SynchronizingScalesSyncStatus);
		synchronizer_->getMessageReceiver()->dispatchEvent(&message);


		targetScale_ = synchronizer_->getTargetScale();
		startingVersionNumber_ = targetScale_->getVersion();


		/* (uc6.32) If the Secondary scale's version number is -1, then perform the following steps: */
		if ( startingVersionNumber_ == hfc::Batch::_invalidBatchVersion_ )
			initializeWholeDatabaseUpload();

		synchronizer_->getBatchManager()->initializeCompleteBatchesQuery(startingVersionNumber_);

		newBatch_ = true;
		currentBatch_ = 0;
		currentBatchChange_ = 0;

		return true;
	}


	int SynchronizeSecondaryScaleUseCase::doSendCommand()
	{
		int result = CommunicationSynchronizer::STATUS_COMPLETED_COMMAND;

		for ( int i = 0; i < recordsPerSyncIteration_; ++i )
		{
			result = sendCommand();
			if ( result != CommunicationSynchronizer::STATUS_COMPLETED_COMMAND )
				break;
		}

    #if defined(LINUX_FIX)
        PAL_absclass::PAL_sleep( delayPerSyncIteration_ );
    #else
		Sleep(delayPerSyncIteration_);
    #endif

		return result;
	}


	int SynchronizeSecondaryScaleUseCase::sendCommand()
	{
		if ( uploadingWholeDatabase_ )
		{
			/* (uc6.32) [ ... If the Secondary scale's version number is -1, then perform the
			following steps:] */

			/* (uc6.32) Upload the whole database, including the corresponding delete all commands
			(only the synchronized record types) to the Secondary scale as a batch that indicates
			"whole database". */
			if ( wholeDatabaseBatch_.null() )
			{
				// If we have not sent the batch for the whole database, send it now.
				wholeDatabaseBatch_ = getWholeDatabaseBatch();

				Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, WriteBatch));
				message->setFormatType(SynchronizationTaggedFormat);
				message->setHfcObject(wholeDatabaseBatch_.get());
				if ( synchronizer_->sendMessage(message) )
				{
					Debug::trace("CommunicationSynchronizer", "Use Case Synchronize Secondary: sent command write whole db batch");
					return CommunicationSynchronizer::STATUS_COMPLETED_COMMAND;
				}
				else
					return CommunicationSynchronizer::STATUS_FAILED;
			}
			else
			{
				if ( wholeDBRecord_.notNull() )
				{
					/* Send a batch record associated with the whole database upload. */
					SessionMessage sessionMessage(wholeDBDataManagerDevice_, wholeDBDataManagerWriteEvent_);
					sessionMessage.setFormatType(SynchronizationTaggedFormat);
					sessionMessage.setHfcObject(wholeDBRecord_.get());
					wholeDBRecord_->setAllReceived(true);

					const char * buffer = sessionMessage.serialize();
					Util::CSmartPtr<hfc::SynchronizationChange> changeRecord(new
							hfc::SynchronizationChange(wholeDBDataManagerDevice_, wholeDBDataManagerWriteEvent_, std::string(buffer, sessionMessage.getSerializedLength())));

					wholeDBRecord_ = wholeDBDataManager_->getNextRecord();

					Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, WriteSynchronizationChange));
					message->setFormatType(SynchronizationTaggedFormat);
					message->setHfcObject(changeRecord.get());
					if ( synchronizer_->sendMessage(message) )
					{
						Debug::trace("CommunicationSynchronizer", "Use Case Synchronize Secondary: sent command whole db write change");
						return CommunicationSynchronizer::STATUS_COMPLETED_COMMAND;
					}
					else
						return CommunicationSynchronizer::STATUS_FAILED;
				}
				else
				{
					// Move to next Data Manager
					if ( wholeDBDataManager_ )
						wholeDBIterator_ = synchronizer_->nextSynchronizedDataManager(wholeDBIterator_);

					while ( wholeDBIterator_ != synchronizer_->endSynchronizedDataManager() )
					{
						wholeDBDataManager_ = synchronizer_->getDataManager(wholeDBIterator_);
						if ( wholeDBDataManager_ )
						{
							wholeDBDataManagerDevice_ = wholeDBDataManager_->getDevice();
							wholeDBDataManagerWriteEvent_  = wholeDBDataManager_->getWriteEvent();

							/* If we have a new Data Manager, send its delete all command and proceed
							to send any records associated with it. */
							if ( sendDeleteAllCommand(wholeDBDataManager_) )
							{
								wholeDBRecord_ = wholeDBDataManager_->getFirstRecord();

								return CommunicationSynchronizer::STATUS_COMPLETED_COMMAND;
							}
							else
								return CommunicationSynchronizer::STATUS_FAILED;
						}

						wholeDBIterator_ = synchronizer_->nextSynchronizedDataManager(wholeDBIterator_);

					}


					/* (uc6.32) Set the Secondary scale's version number to 0. */
					targetScale_->setVersion(0);
					synchronizer_->getScaleManager()->saveScale(targetScale_.get());

					// Continue with regular batches (i.e., newBatch_ == true)
					uploadingWholeDatabase_ = false;
				}
			}
		}


		if ( !newBatch_ )
		{
			/* (uc6.32) For each change record associated with the batch, send it to the Secondary
			scale. */
			if ( currentBatchChange_.null() )
			{
				synchronizer_->getSynchronizationChangeManager()->initializeBatchChangesQuery(currentBatch_->getNumber());
				currentBatchChange_ = synchronizer_->getSynchronizationChangeManager()->getFirstChangeFromBatch(currentBatch_->getNumber());
			}
			else
				currentBatchChange_ = synchronizer_->getSynchronizationChangeManager()->getNextChangeFromBatch(currentBatch_->getNumber());

			while (!currentBatchChange_.null() && currentBatchChange_->isConsolidatedOnly())
			{
				currentBatchChange_ = synchronizer_->getSynchronizationChangeManager()->getNextChangeFromBatch(currentBatch_->getNumber());
			}

			if ( currentBatchChange_.null() )
			{
				/* (uc6.32) Set this Secondary scale's version number to this batch's version number
				in the Secondary scale's record, storing the current date/time. */
				/* [Refresh the scale record just in case.] */
				targetScale_ = synchronizer_->getScaleManager()->findScaleByIdentifier(targetScale_->getIdentifier());
				if ( targetScale_.notNull() )
				{
					targetScale_->setVersion(currentBatch_->getVersion());
					synchronizer_->getScaleManager()->saveScale(targetScale_);
				}


				newBatch_ = true;
			}
			else
			{
				Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, WriteSynchronizationChange));
				message->setFormatType(SynchronizationTaggedFormat);
				message->setHfcObject(currentBatchChange_.get());
				if ( synchronizer_->sendMessage(message) )
				{
					Debug::trace("CommunicationSynchronizer", "Use Case Synchronize Secondary: sent command write change");
					return CommunicationSynchronizer::STATUS_COMPLETED_COMMAND;
				}
				else
					return CommunicationSynchronizer::STATUS_FAILED;
			}
		}

		if ( currentBatchChange_.null() )
		{
			/* (uc6.32) For each completed batch, user or HQ, local or remote, with version number
			greater than the version number in the Secondary scale's record, in version number
			order, perform the following steps: */
			if ( currentBatch_.null() )
				currentBatch_ = synchronizer_->getBatchManager()->getFirstCompleteBatch(startingVersionNumber_);
			else
				currentBatch_ = synchronizer_->getBatchManager()->getNextCompleteBatch(startingVersionNumber_);

			while ( currentBatch_.notNull() )
			{
				if ( !currentBatch_->isApplied() && !currentBatch_->isPendingApplication() )
				{
					currentBatch_ = 0;
					break;
				}

				if ( !currentBatch_->isPendingDeletion() && !currentBatch_->isDeleted() )
					break;

				currentBatch_ = synchronizer_->getBatchManager()->getNextCompleteBatch(startingVersionNumber_);
			}

			if ( currentBatch_.notNull() )
			{
				/* (uc6.32) Send the Process batch - Begin command, passing the batch information. */
				Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, WriteBatch));
				message->setFormatType(SynchronizationTaggedFormat);
				message->setHfcObject(currentBatch_.get());
				if ( synchronizer_->sendMessage(message) )
				{
					Debug::trace("CommunicationSynchronizer", "Use Case Synchronize Secondary: sent command write batch");

					newBatch_ = false;

					return CommunicationSynchronizer::STATUS_COMPLETED_COMMAND;
				}
				else
					return CommunicationSynchronizer::STATUS_FAILED;
			}
			else
				return CommunicationSynchronizer::STATUS_COMPLETED_ALL_COMMANDS;
		}

    #if defined(LINUX_FIX)
		throw Util::CPlatformException();
    #else
		throw Util::CNTException();
    #endif
	}


	void SynchronizeSecondaryScaleUseCase::doProcessError(CommSessionEvent::CommError /*error*/)
	{
		operationFailed_ = true;
	}


	void SynchronizeSecondaryScaleUseCase::doConclude()
	{
		if ( !operationFailed_ )
		{
			/* (SYNCSTATUS) notify UI that synchronizing Secondary succeeded. */
			NewSynchronizationStatusMessage message(SynchronizationActiveSyncStatus);
			synchronizer_->getMessageReceiver()->dispatchEvent(&message);
		}
		else
		{
			/* (SYNCSTATUS) notify UI that synchronizing Secondary failed. */
			/* [There is no status message for this case yet.] */
		}


		/* (TODO) we need to protect db access in case the db closed */
		/* (TODO) also, what if the operation failed? */
		/* [Refresh the scale record just in case.] */
		targetScale_ = synchronizer_->getScaleManager()->findScaleByIdentifier(targetScale_->getIdentifier());
		if ( targetScale_.notNull() )
		{
			targetScale_->clearRegistering();
			targetScale_->clearNotifiedOnline();
			synchronizer_->getScaleManager()->saveScale(targetScale_.get());
		}
	}


	void SynchronizeSecondaryScaleUseCase::showUseCaseStartingInfo()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Synchronize Secondary Scale: Starting");
	}


	void SynchronizeSecondaryScaleUseCase::showUseCaseSuccessful()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Synchronize Secondary Scale: completed successfully");
	}


	void SynchronizeSecondaryScaleUseCase::showUseCaseFailed(const std::string& status)
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Synchronize Secondary Scale: failed (status = " + status + ")");
	}


	void SynchronizeSecondaryScaleUseCase::initializeWholeDatabaseUpload()
	{
		uploadingWholeDatabase_ = true;
		wholeDBIterator_ = synchronizer_->beginSynchronizedDataManager();
		wholeDBDataManager_ = 0;
		wholeDBRecord_ = 0;
	}


	Util::CSmartPtr<hfc::Batch> SynchronizeSecondaryScaleUseCase::getWholeDatabaseBatch()
	{
		Util::CSmartPtr<hfc::Batch> batch(new hfc::Batch());

		batch->setVersion(0);
		batch->setOrigin(hfc::Batch::OriginLocal);
		batch->setType(hfc::Batch::TypeWholeDB);
		batch->setCreatedDateTime();

		return batch;
	}


	bool SynchronizeSecondaryScaleUseCase::sendDeleteAllCommand(DataManager* dataManager)
	{
		UINT8 deleteAllEvent = dataManager->getDeleteAllEvent();
		if (deleteAllEvent != InvalidEvent)
		{
			SessionMessage sessionMessage(wholeDBDataManagerDevice_, deleteAllEvent);
			sessionMessage.setFormatType(SynchronizationTaggedFormat);

			const char * buffer = sessionMessage.serialize();
			Util::CSmartPtr<hfc::SynchronizationChange> changeRecord(new
					hfc::SynchronizationChange(wholeDBDataManagerDevice_, deleteAllEvent, std::string(buffer, sessionMessage.getSerializedLength())));

			Util::CSmartPtr<SessionMessage> actualMessage(new SessionMessage(SynchronizationDev, WriteSynchronizationChange));
			actualMessage->setFormatType(SynchronizationTaggedFormat);
			actualMessage->setHfcObject(changeRecord.get());

			return synchronizer_->sendMessage(actualMessage);
		}
		else
			return true;
	}



}
