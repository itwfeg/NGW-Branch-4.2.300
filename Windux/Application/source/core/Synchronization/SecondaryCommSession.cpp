#include "SecondaryCommSession.h"


#include "SessionManager.h"
#include "SynchronizationManagerProxy.h"

#include "Configuration.h"
#include "ScaleManager.h"
#include "BatchManager.h"
#include "SynchronizationChangeManager.h"
#include "SessionMessage.h"


#include "HeaderRecord.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/ProcessRemoteBatchMessage.h"
#include "Messages/ProcessScaleOnlineMessage.h"
#include "Messages/DeleteBatchMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


	#ifdef USE_ACCOUNTANT
		int SecondaryCommSession::_dataType_ = 0;
		string SecondaryCommSession::_dataTypeName_("synchronization::SecondaryCommSession");
	#endif


	SecondaryCommSession::SecondaryCommSession(ScaleSynchronizationType scaleSynchronizationType, CommMechanism * uiClient, bool enableInput)
		: SynchronizationCommSession(scaleSynchronizationType, hfc::Scale::RoleSecondary, uiClient, "SecondaryCommSession", enableInput), newBatch_(false), operationFailed_(false)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	SecondaryCommSession::~SecondaryCommSession()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	void SecondaryCommSession::doProcessHeader(Util::CSmartPtr<hfc::HeaderRecord> header)
	{
		/* (uc1.7) If the Scale Type configuration element is set to Stand Alone and
		this is a Primary/Secondary connection, then send the "No Primary/Secondary
		scale" error and close the connection. Return. */
		ScaleSynchronizationType scaleSynchronizationType = Configuration::getInstance()->getScaleSynchronizationType();
		if ( scaleSynchronizationType == ScaleSynchronizationTypeStandAlone )
		{
			Util::CSmartPtr<HfcError> error(new HfcError(HfcError::ERRORCODE_NOTSYNCHRONIZING));
			addError(error);

			Debug::trace("SecondaryCommSession", "------- Scale configured as Stand Alone. We STOP accepting input");

			setAcceptingInput(false);
		}


		/* (uc1.7) If the Scale Type configuration element is set to Primary and this
		is a Primary connection, then send the "No Secondary scale" error and close the
		connection. Return. */
		if ( scaleSynchronizationType == ScaleSynchronizationTypePrimary )
		{
			Util::CSmartPtr<HfcError> error(new HfcError(HfcError::ERRORCODE_NOTASECONDARYSCALE));
			addError(error);

			Debug::trace("SecondaryCommSession", "------- Scale configured as Primary. We STOP accepting input");

			setAcceptingInput(false);
		}


		/* (uc1.7) If the Input Processing Enabled flag [Session Manager's] is clear
		and this is a non-console connection, then send the "Busy" error and close the
		connection. Return. */
		/* [This happens during the call to pauseProcessing.] */


		if ( isAcceptingInput() )
		{
			/* (uc) Extract Primary scale information from socket and communication header. */
			hfc::Scale::ScaleRole remoteScaleRole = header->getScaleRole();


			if ( remoteScaleRole == hfc::Scale::RolePrimary )
			{
				/* (uc4.22) Identify the Primary scale's record in the database, including the
				scale identifier. */
				remoteScale_ = getScaleManager()->getPrimaryScale();
				if ( remoteScale_.notNull() )
				{
					bool hostnameChanged = false, ipAddressChanged = false;
					const std::string& hostname = receivedHeader_->getHostName();
					const std::string& ipAddress = receivedHeader_->getIPAddress();

					if ( (hostname.size() && hostname != remoteScale_->getHostName()) ||
						(hostname.empty() && remoteScale_->getHostName().size()) )
						hostnameChanged = true;
					if ( (ipAddress.size() && HobIpAddress(ipAddress) != HobIpAddress(remoteScale_->getIPAddress())) ||
						(ipAddress.empty() && remoteScale_->getIPAddress().size()) )
						ipAddressChanged = true;

					if ( hostnameChanged && ipAddressChanged )
					{
						/* Exception: Not my Primary scale
						This exception is triggered when the scale that initiated the connection
						identified itself as a Primary scale, but does not match this scale's Primary
						scale. Perform the following steps:
						- Send the "Not my Primary scale" error.
						- Close the connection and end the use case. */
						Util::CSmartPtr<HfcError> error(new HfcError(HfcError::ERRORCODE_NOTYOURSECONDARYSCALE));
						addError(error);

						Debug::trace("SecondaryCommSession", "------- Not my known Primary scale. We STOP accepting input");

						setAcceptingInput(false);
					}
					else
					{
						const std::string& cfgPrimary = Configuration::getInstance()->toStringCfg(fieldOffsetCfgPrimaryScale);
						bool isIPAddress = HobIpAddress::isValidIPAddress(cfgPrimary);

						if ( hostnameChanged )
						{
							remoteScale_->setHostName(hostname);
							if (!isIPAddress)
								Configuration::getInstance()->updateField(fieldOffsetCfgPrimaryScale, hostname);
						}

						if ( ipAddressChanged )
						{
							remoteScale_->setIPAddress(ipAddress);
							if (isIPAddress)
								Configuration::getInstance()->updateField(fieldOffsetCfgPrimaryScale, ipAddress);
						}

						if ( hostnameChanged || ipAddressChanged )
						{
							getScaleManager()->saveScale(remoteScale_.get());

							Configuration::getInstance()->save(this);
							//EvolMessage message(ConfigurationDev, CfgSaved);
							//dispatchEvent(&message);
						}


						/* (uc4.22) If the Primary scale's record was marked as off-line, then mark
						it as on-line and store the current date/time. */
						if ( remoteScale_->isStatePassive() )
						{
							remoteScale_->setStateActive();
							getScaleManager()->saveScale(remoteScale_.get());

							Debug::trace("SecondaryCommSession", "Primary scale was off-line, is now back on-line");
						}
						else if ( remoteScale_->isStateRetrying() )
						{
							ProcessScaleOnlineMessage message(remoteScale_->getIdentifier());
							dispatchEvent(&message);
						}
					}
				}
			}
			else
			{
				/* (uc4.22) If the other scale did not identifiy itself as a Primary scale, then
				send the corresponding error and end the execution of this use case after closing
				the connection. */
				/* This code is probably unnecessary since the reason why we are here is because
				the other scale identified itself as a Primary scale (criteria used by Session
				Starter to start this session). */
				/* (TODO) uncomment this code */
//				Util::CSmartPtr<HfcError> error(new HfcError(error_code));
//				addError(error);

				Debug::trace("SecondaryCommSession", "------- Not a Primary scale. We STOP accepting input");

				setAcceptingInput(false);
			}
		}
	}


	bool SecondaryCommSession::doProcessCommand(Util::CSmartPtr<SessionMessage> command)
	{
		/* (uc4.22) For each command received, perform the following steps: */
		if ( command->getDeviceType() == SynchronizationDev )
		{
			switch ( command->getEventType() )
			{
				case WriteBatch:
				{
					bool syncError = false;

					/* First conclude processing the previously received Batch if any. */
					concludeReceivedBatch();

					Debug::trace("SecondaryCommSession", "Processing command WriteBatch");

					/* Now deal with the newly received batch. */
					receivedBatch_ = dynamic_cast<hfc::Batch*>(command->getHfcObject());
					if ( receivedBatch_.notNull() )
					{
						command->setHfcObjectCreatedInternally(false);

						/* (SYNCSTATUS) notify UI that we are starting to receive changes. The
						message will be sent once per batch. If that is too much we can put an
						if block around it to only report it once */
						NewSynchronizationStatusMessage message(ReceivingChangesSyncStatus);
						dispatchEvent(&message);

						Debug::trace("SecondaryCommSession", "Receiving a remote batch with signature " + receivedBatch_->getSignature());


						/* (uc4.32) Execute "Use Case 5.40: Creating batch", passing "remote"
						and "user batch" or "HQ batch" based on the batch information received,
						receiving the new batch identifier. */
						SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
						if ( proxy )
						{
							int batchNumber = hfc::Batch::_invalidBatchNumber_;
							if ( proxy->createBatch(&batchNumber, hfc::Batch::OriginRemote, receivedBatch_->getType()) )
							{
								receivingBatch_ = getBatchManager()->findBatchNumber(batchNumber);
								if ( receivingBatch_.notNull() )
								{
									/* (TODO) this is probably the place to set the scale that sent this batch */
									receivingBatch_->setRemoteVersion(receivedBatch_->getVersion());
									if ( remoteScale_.notNull() )
										receivingBatch_->setScaleId(remoteScale_->getIdentifier());

									getBatchManager()->saveBatch(receivingBatch_.get());
								}
								else syncError = true;
							}
							else syncError = true;
						}
						else syncError = true;
					}

					if ( syncError )
					{
						Util::CSmartPtr<HfcError> error(new HfcError(HfcError::ERRORCODE_SCALEBUSY));

						addError(error);
					}

					break;
				}
				case WriteSynchronizationChange:
				{
					bool syncError = false;

					if ( receivingBatch_.notNull() )
					{
						Debug::trace("SecondaryCommSession", "Processing command WriteSynchronizationChange");

						/* (uc4.32) For each change record received from Primary scale until
						Process batch - End, perform the following steps: */

						Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(dynamic_cast<hfc::SynchronizationChange*>(command->getHfcObject()));
						if ( receivedChange.notNull() )
						{
							command->setHfcObjectCreatedInternally(false);


							/* (uc4.32) If the Input processing enabled flag is clear, then
							perform the following steps: */
							/* [This was already checked before we got into this function.] */


							/* (uc4.32) Execute "Use Case 5.42: Adding record to batch", passing
							the batch identifier and the change record. */
							SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
							if ( proxy )
							{
								if ( !proxy->addRecordToBatch(receivingBatch_->getNumber(), receivedChange) )
									syncError = true;
							}
							else syncError = true;
						}
					}
					else
						Debug::trace("SecondaryCommSession", "------- Received a change record but I dont have a batch for it");

					if ( syncError )
					{
						Util::CSmartPtr<HfcError> error(new HfcError(HfcError::ERRORCODE_SCALEBUSY));

						addError(error);
					}

					break;
				}
				default:
				{
					/* (uc4.22) If the command is unknown, then do nothing. */
					Debug::trace("SecondaryCommSession", "Received an unknown command (" + util_string::inttostring(SynchronizationDev) +
							", " + util_string::inttostring(command->getEventType()) + ") from the remote scale");
					/* (TODO) send an error? it is probably better to ignore it quietly */

					break;
				}
			}
		}
		else
		{
			/* (uc4.22) If the command is unknown, then do nothing. */
			Debug::trace("SecondaryCommSession", "Received an unknown command (" + util_string::inttostring(command->getDeviceType()) +
					", " + util_string::inttostring(command->getEventType()) + ") from the remote scale");
			/* (TODO) send an error? it is probably better to ignore it quietly */
		}

		return true;
	}


	bool SecondaryCommSession::doReadAllInit()
	{
		/* (uc4.22) For each command received, perform the following steps: */
		if ( receivedCommand_->getDeviceType() == SynchronizationDev )
		{
			switch ( receivedCommand_->getEventType() )
			{
				case ReadBatches:
				{
					/* (uc4.22) If the command is Read local batches, then execute "Use Case 4.24:
					Uploading Secondary scale local batches". Skip to next command. */

					batch_ = 0;
					newBatch_ = true;


					/* (TODO) this should probably be initializePendingSynchronization(Local)BatchesQuery */
					return getBatchManager()->initializeCompleteLocalBatchesQuery();
				}
				default:
				{
					Debug::trace("SecondaryCommSession", "Received an unknown command (" + util_string::inttostring(SynchronizationDev) +
							", " + util_string::inttostring(receivedCommand_->getEventType()) + ") from the remote scale");

					break;
				}
			}
		}
		else
		{
			Debug::trace("SecondaryCommSession", "Received an unknown command (" + util_string::inttostring(receivedCommand_->getDeviceType()) +
					", " + util_string::inttostring(receivedCommand_->getEventType()) + ") from the remote scale");
		}

		return false;
	}


	bool SecondaryCommSession::doReadAllRead()
	{
		if ( receivedCommand_->getDeviceType() == SynchronizationDev )
		{
			switch ( receivedCommand_->getEventType() )
			{
				case ReadBatches:
				{
					if ( !newBatch_ )
					{
						/* (uc4.24) c. For each change record associated with the batch perform the
						following steps: */
						if ( change_.null() )
						{
							getSynchronizationChangeManager()->initializeBatchChangesQuery(batch_->getNumber());
							change_ = getSynchronizationChangeManager()->getFirstChangeFromBatch(batch_->getNumber());
						}
						else
							change_ = getSynchronizationChangeManager()->getNextChangeFromBatch(batch_->getNumber());

						if ( change_.null() )
							newBatch_ = true;
					}

					/* (uc4.24) For each completed local batch, in version order, perform the following
					steps: */
					if ( newBatch_ )
					{
						if ( batch_.null() )
						{
							/* (TODO) this should probably be getFirstPendingSynchronization(Local)Batch
							but it works fine because of the while loop below */
							batch_ = getBatchManager()->getFirstCompleteLocalBatch();
						}
						else
						{
							/* Update remote scale record. */
							remoteScale_->setVersion(batch_->getVersion());
							getScaleManager()->saveScale(remoteScale_.get());


							/* (uc4.24) f. Execute "Use Case 5.37: Processing synchronized batch",
							passing the batch identifier. */
							SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
							if ( proxy )
								proxy->processSynchronizedBatch(batch_->getNumber());


							batch_ = getBatchManager()->getNextCompleteLocalBatch();
						}


						/* (uc4.24) a. If the batch is not marked as Pending synchronization,
						skip to the next batch. */
						while ( batch_.notNull() && !batch_->isPendingSynchronization() )
							/* (TODO) this should probably be getNextPendingSynchronization(Local)Batch
							but it works fine because of the while loop */
							batch_ = getBatchManager()->getNextCompleteLocalBatch();

						change_ = 0;

						if ( batch_.notNull() )
						{
							/* (SYNCSTATUS) notify UI that we are starting to send changes. The message
							will be sent once per batch. If that is too much we can move it up to
							doReadAllInit function */
							NewSynchronizationStatusMessage message(SendingChangesSyncStatus);
							dispatchEvent(&message);
						}
						else
						{
							/* (SYNCSTATUS) notify UI that we completed sending changes. The message
							will be sent only once. */
							NewSynchronizationStatusMessage message(SynchronizationActiveSyncStatus);
							dispatchEvent(&message);
						}

						return batch_.notNull();
					}
				}

				return change_.notNull();
			}
		}

		return false;
	}


	bool SecondaryCommSession::doReadAllWrite(CommSessionEvent::CommError& returnedError)
	{
		returnedError = CommSessionEvent::COMMERROR_NO_ERROR;


		if ( receivedCommand_->getDeviceType() == SynchronizationDev )
		{
			switch ( receivedCommand_->getEventType() )
			{
				case ReadBatches:
				{
					bool failed = false;

					if ( newBatch_ )
					{
						/* (uc4.24) Send the Process Batch - Begin command, passing the batch
						information. */
						if ( writeBatch(batch_) )
						{
							newBatch_ = false;

							return true;
						}
						else
							failed = true;
					}

					if ( !failed )
					{
						/* (uc4.24) Send the corresponding change record command to the Primary scale. */
						if ( writeSynchronizationChange(change_) )
							return true;
					}

					/* (SYNCSTATUS) notify UI that we failed to completely send changes.
					The message will be sent only once. */
					NewSynchronizationStatusMessage message(UnableToContactPrimarySyncStatus);
					dispatchEvent(&message);

					returnedError = CommSessionEvent::COMMERROR_WRITING_TO_SOCKET;

					return true;
				}
				default:
					return true;
			}
		}

		return true;
	}


	void SecondaryCommSession::doProcessError(CommSessionEvent::CommError /*error*/)
	{
		operationFailed_ = true;
	}


	bool SecondaryCommSession::doProcessFooter()
	{
		concludeReceivedBatch();

		return true;
	}


	bool SecondaryCommSession::doConclusion()
	{
		if ( !operationFailed_ )
		{
			/* (SYNCSTATUS) notify UI that we completed operation with Primary. */
			/* [For now there is no status to send] */
		}
		else
		{
			/* (SYNCSTATUS) notify UI that we failed to complete operation with Primary. */
			NewSynchronizationStatusMessage message(UnableToContactPrimarySyncStatus);
			dispatchEvent(&message);
		}

		return true;
	}


	void SecondaryCommSession::concludeReceivedBatch()
	{
		if ( isAcceptingInput() )
		{
			if ( receivingBatch_.notNull() )
			{
				/* Refresh the batch from the database to update the signature information. */
				receivingBatch_ = getBatchManager()->findBatchNumber(receivingBatch_->getNumber());

				/* Store the version number of the batch received in the Primary scale record. */
				remoteScale_->setVersionReceived(receivingBatch_->getRemoteVersion());
				getScaleManager()->saveScale(remoteScale_.get());


				/* (uc4.32) If the received batch contained a signature, compare it to the computed
				signature. If they do not match, then perform the following steps: */
				if ( !receivedBatch_->isSignatureEmtpy() && receivedBatch_->getSignature() != receivingBatch_->getSignature() )
				{
					Debug::trace("SecondaryCommSession", "Received a remote batch with signature " + receivedBatch_->getSignature() +
							" while calculated signature is " + receivingBatch_->getSignature() + ". Deleting it.");


					/* (uc4.32) Send Batch failed command back to the Primary scale to indicate the problem. */
					/* (TODO) ? */


					/* (uc4.32) Mark the batch record as Pending deletion and trigger the execution of
					"Use Case 5.46: Deleting batch", passing the batch identifier and "delete batch record". */
					/* [We dont mark it here since it happens in processing the DeleteBatchMessage.] */
//					receivingBatch_->setPendingDeletion();
//					getBatchManager()->saveBatch(receivingBatch_.get());

					Util::CSmartPtr<DeleteBatchMessage> message(new DeleteBatchMessage(receivingBatch_->getNumber(), DeleteBatchProcessedMessage::DeleteChangesAndBatch));
					dispatchEvent(message.get());


					/* (uc4.32) End the execution of this use case. */
				}
				else
				{
					/* (uc4.32) Send Batch processed back to the Primary scale to confirm reception. */
					/* (TODO) ? */


					/* (uc4.32) Trigger the execution of "Use Case 5.36: Processing remote batch",
					passing the batch identifier. */
					Util::CSmartPtr<ProcessRemoteBatchMessage> message(new ProcessRemoteBatchMessage(receivingBatch_->getNumber()));
					dispatchEvent(message.get());
				}


				receivingBatch_ = 0;
			}
		}
		else
		{
			if ( !isPausingProcessing() )
			{
				if ( receivingBatch_.notNull() )
				{
					/* [We dont mark it here since it happens in processing the DeleteBatchMessage.] */
//					receivingBatch_->setPendingDeletion();
//					getBatchManager()->saveBatch(receivingBatch_.get());

					Util::CSmartPtr<DeleteBatchMessage> message(new DeleteBatchMessage(receivingBatch_->getNumber(), DeleteBatchProcessedMessage::DeleteChangesAndBatch));
					dispatchEvent(message.get());
				}
			}
		}
	}



}