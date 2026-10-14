#include "DownloadLocalBatchesUseCase.h"


#include "SynchronizationManagerProxy.h"


#include "SessionManager.h"
#include "ScaleManager.h"
#include "BatchManager.h"
#include "SynchronizationChange.h"


#include "NotifyLocalBatches.h"
#include "ReadBatchesRecord.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/ProcessRemoteBatchMessage.h"
#include "Messages/AddRecordToBatchMessage.h"
#include "Messages/DeleteBatchMessage.h"
#include "Messages/DeleteBatchProcessedMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"


#include "HfcError.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int DownloadLocalBatchesUseCase::_dataType_ = 0;
	std::string DownloadLocalBatchesUseCase::_dataTypeName_("synchronization::DownloadLocalBatchesUseCase");
#endif


	DownloadLocalBatchesUseCase::DownloadLocalBatchesUseCase(CommunicationSynchronizer* synchronizer)
		: synchronizer_(synchronizer), notifiedBatches_(false), startingVersionNumber_(-1), operationFailed_(false)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	DownloadLocalBatchesUseCase::~DownloadLocalBatchesUseCase()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	bool DownloadLocalBatchesUseCase::doInitialize()
	{
		targetScale_ = synchronizer_->getTargetScale();

		notifiedBatches_ = targetScale_->isNotifiedBatches();
		notifiedBatchesDateTime_ = targetScale_->getNotifiedBatchesDateTime();
		startingVersionNumber_ = targetScale_->getVersionReceived();

		return true;
	}


	int DownloadLocalBatchesUseCase::doSendCommand()
	{
		/* (uc6.24) Send the Read local batches command passing the remote version number. */
		Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, ReadBatches));
		message->setFormatType(SynchronizationTaggedFormat);
		message->setHfcObject(new hfc::ReadBatchesRecord(hfc::ReadBatchesRecord::TypeLocal, startingVersionNumber_));
		if ( synchronizer_->sendMessage(message) )
		{
			Debug::trace("CommunicationSynchronizer", "Use Case Download Local Batches: Sent command to read batches from Secondary");

			/* (uc6.24) Clear Local batch present flag in the Secondary scale's database record. */
			targetScale_ = synchronizer_->getScaleManager()->findScaleByIdentifier(targetScale_->getIdentifier());
			if ( targetScale_.notNull() )
			{
				targetScale_->clearNotifiedBatches();
				synchronizer_->getScaleManager()->saveScale(targetScale_);
			}

			currentBatch_ = 0;

			return CommunicationSynchronizer::STATUS_COMPLETED_ALL_COMMANDS;
		}

		return CommunicationSynchronizer::STATUS_FAILED;
	}


	bool DownloadLocalBatchesUseCase::doProcessCommand(Util::CSmartPtr<SessionMessage> message)
	{
		if ( message.notNull() )
		{
			if ( message->getDeviceType() == SynchronizationDev )
			{
				/* (uc6.24) For each command received, if the command is: */
				switch ( message->getEventType() )
				{
					case WriteBatch:
					{
						bool syncError = false;


						/* (uc6.24) Process batch - Begin, then perform the following steps: */
						Debug::trace("CommunicationSynchronizer", "Use Case Download Local Batches: Received batch");

						/* First conclude processing the previously received Batch if any. */
						processBatchEndCommand();


						/* Now deal with the newly received batch. */
						receivedBatch_ = dynamic_cast<hfc::Batch*>(message->getHfcObject());
						if ( receivedBatch_.notNull() )
						{
							message->setHfcObjectCreatedInternally(false);

							/* (SYNCSTATUS) notify UI that we are starting to receive changes.
							The message will be sent once per batch. */
							NewSynchronizationStatusMessage message(ReceivingChangesSyncStatus);
							synchronizer_->getMessageReceiver()->dispatchEvent(&message);


							/* (uc6.24) Execute "Use Case 5.40: Creating batch", passing "remote" and
							the batch information received, receiving the batch identifier. */
							SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
							if ( proxy )
							{
								int batchNumber = hfc::Batch::_invalidBatchNumber_;
								if ( proxy->createBatch(&batchNumber, hfc::Batch::OriginRemote, receivedBatch_->getType()) )
								{
									currentBatch_ = synchronizer_->getBatchManager()->findBatchNumber(batchNumber);
									if ( currentBatch_.notNull() )
									{
										currentBatch_->setRemoteVersion(receivedBatch_->getVersion());
										currentBatch_->setScaleId(targetScale_->getIdentifier());

										synchronizer_->getBatchManager()->saveBatch(currentBatch_.get());
									}
									else syncError = true;
								}
								else syncError = true;
							}
							else syncError = true;
						}

						if ( syncError )
						{
							Util::CSmartPtr<HfcError> error(new HfcError());
							synchronizer_->addError(error);
						}

						break;
					}
					case WriteSynchronizationChange:
					{
						bool syncError = false;


						if ( currentBatch_.notNull() )
						{
							Debug::trace("CommunicationSynchronizer", "Use Case Download Local Batches: Received change");
							
							/* (uc6.24) For each change record received until the Process batch - End
							command, execute "Use Case 5.41: Adding record to batch", passing the batch
							identifier and the change record. */
							Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(dynamic_cast<hfc::SynchronizationChange*>(message->getHfcObject()));
							message->setHfcObjectCreatedInternally(false);

							SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
							if ( proxy )
							{
								if ( !proxy->addRecordToBatch(currentBatch_->getNumber(), receivedChange) )
									syncError = true;
							}
							else syncError = true;

							if (!receivedChange->isConsolidatedOnly())
								receivedBatch_->computeSynchronizedSignature(receivedChange->getChange());

						}
						else
							Debug::trace("CommunicationSynchronizer", "******* Received Change record was dropped because I have no batch");

						if ( syncError )
						{
							Util::CSmartPtr<HfcError> error(new HfcError());
							synchronizer_->addError(error);
						}

						break;
					}
					default:
					{
						Debug::trace("CommunicationSynchronizer", "Use Case Download Local Batches: "
							"received an unknown synchronization command with event = " +
							util_string::uinttostring(message->getEventType()));

						break;
					}
				}
			}
			else
			{
				Debug::trace("CommunicationSynchronizer", "Use Case Download Local Batches: "
					"received an unknown command with device = " + util_string::inttostring(message->getDeviceType()) +
					" and event = " + util_string::uinttostring(message->getEventType()));
			}
		}

		return true;
	}


	void DownloadLocalBatchesUseCase::doProcessError(CommSessionEvent::CommError /*error*/)
	{
		operationFailed_ = true;

		if ( !synchronizer_->isPausingProcessing() )
		{
			if ( notifiedBatches_ )
			{
				/* [Refresh the scale record just in case.] */
				targetScale_ = synchronizer_->getScaleManager()->findScaleByIdentifier(targetScale_->getIdentifier());
				if ( targetScale_.notNull() )
				{
					targetScale_->setNotifiedBatches();
					targetScale_->setNotifiedBatchesDateTime(notifiedBatchesDateTime_);

					synchronizer_->getScaleManager()->saveScale(targetScale_);
				}
			}
		}
	}


	void DownloadLocalBatchesUseCase::doConclude()
	{
		processBatchEndCommand();

		if ( !operationFailed_ )
		{
			/* (SYNCSTATUS) notify UI that we completed receiving changes.
			The message will be sent once per batch. */
			NewSynchronizationStatusMessage message(SynchronizationActiveSyncStatus);
			synchronizer_->getMessageReceiver()->dispatchEvent(&message);
		}
		else
		{
			/* (SYNCSTATUS) notify UI that we failed to completly receive changes. */
			/* [For now there is no status to send] */
		}
	}


	void DownloadLocalBatchesUseCase::processBatchEndCommand()
	{
		if ( currentBatch_.notNull() )
		{
			/* (uc6.24) [... if the command is] Process batch - End, then perform the following steps: */

			/* Refresh the batch from the database to update the signature information. */
			currentBatch_ = synchronizer_->getBatchManager()->findBatchNumber(currentBatch_->getNumber());

			/* Store the version number of the batch received in the Secondary scale record. */
			/* [Refresh the scale record just in case.] */
			targetScale_ = synchronizer_->getScaleManager()->findScaleByIdentifier(targetScale_->getIdentifier());
			if ( targetScale_.notNull() )
			{
				targetScale_->setVersionReceived(currentBatch_->getRemoteVersion());
				synchronizer_->getScaleManager()->saveScale(targetScale_.get());
			}


			/* (uc6.24) If the signature computed for the newly created batch does not match the signature
			received, then perform the following steps: */
			if ( !currentBatch_->isSignatureEmtpy() && receivedBatch_->getSignature() != currentBatch_->getSignature() )
			{
				Debug::trace("CommunicationSynchronizer", "Received a remote batch with signature " + receivedBatch_->getSignature() +
						" while calculated signature is " + currentBatch_->getSignature() + ". Deleting it.");


				/* (uc6.24) Send the Batch failed command to the Secondary scale to inform of the condition. */
				/* (TODO) ? beware that the connection may be closed by now */


				/* (uc6.24) Mark the batch as Pending deletion and trigger the execution of "Use Case 5.46:
				Deleting batch", passing the batch identifier and "delete batch record". */
				/* [We dont mark it here since it happens in processing the DeleteBatchMessage.] */
//				currentBatch_->setPendingDeletion();
//				synchronizer_->getBatchManager()->saveBatch(currentBatch_.get());

				DeleteBatchMessage message(currentBatch_->getNumber(), DeleteBatchProcessedMessage::DeleteChangesAndBatch);
				synchronizer_->getMessageReceiver()->dispatchEvent(&message);


				/* (uc6.24) Continue with next command. */
			}
			else
			{
				/* (uc6.24) Send the Batch processed command to the Secondary scale to confirm the
				reception of the batch. */
				/* (TODO) ? */


				/* We update the signature to reflect the change records received that will not be
				resynchronized to the other secondary scales. */

				if (!(currentBatch_->getSignatureObject() == receivedBatch_->getSynchronizedSignatureObject()))
				{
					currentBatch_->setSignature(receivedBatch_->getSynchronizedSignature());
					synchronizer_->getBatchManager()->saveBatch(currentBatch_.get());
				}

				/* (uc6.24) Trigger the execution of "Use Case 5.36: Processing Remote batch",
				passing the batch id. */
				ProcessRemoteBatchMessage message(currentBatch_->getNumber());
				synchronizer_->getMessageReceiver()->dispatchEvent(&message);
			}
		}
	}


	void DownloadLocalBatchesUseCase::showUseCaseStartingInfo()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Download Local Batches: Starting");
	}


	void DownloadLocalBatchesUseCase::showUseCaseSuccessful()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Download Local Batches: completed successfully");
	}


	void DownloadLocalBatchesUseCase::showUseCaseFailed(const std::string& status)
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Download Local Batches: failed (status = " + status + ")");
	}


}
