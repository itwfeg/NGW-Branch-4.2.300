#include "PrimaryCommSession.h"


#include "ScaleManager.h"
#include "SessionMessage.h"

#include "HeaderRecord.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/RegisterSecondaryScaleMessage.h"
#include "Messages/DownloadLocalBatchesMessage.h"
#include "Messages/SynchronizeSecondaryScaleMessage.h"
#include "Messages/ProcessScaleOnlineMessage.h"
#include "Messages/NewSynchronizationStatusMessage.h"


#include "SessionManager.h"
#include "Configuration.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


	#ifdef USE_ACCOUNTANT
		int PrimaryCommSession::_dataType_ = 0;
		string PrimaryCommSession::_dataTypeName_("synchronization::PrimaryCommSession");
	#endif


	PrimaryCommSession::PrimaryCommSession(ScaleSynchronizationType scaleSynchronizationType, CommMechanism * uiClient, bool inputEnabled, int reason)
		: SynchronizationCommSession(scaleSynchronizationType, hfc::Scale::RolePrimary, uiClient, "PrimaryCommSession", inputEnabled, reason),
		scaleWasOffLine_(false), firstCommand_(true), operationFailed_(false)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	PrimaryCommSession::~PrimaryCommSession()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	void PrimaryCommSession::doProcessHeader(Util::CSmartPtr<hfc::HeaderRecord> header)
	{
		/* (uc1.7) If the Scale Type configuration element is set to Stand Alone and
		this is a Primary/Secondary connection, then send the "No Primary/Secondary
		scale" error and close the connection. Return. */
		ScaleSynchronizationType scaleSynchronizationType = getScaleSynchronizationType();
		if ( scaleSynchronizationType == ScaleSynchronizationTypeStandAlone )
		{
			Util::CSmartPtr<HfcError> error(new HfcError(HfcError::ERRORCODE_NOTSYNCHRONIZING));
			addError(error);

			Debug::trace("PrimaryCommSession", "------- Scale configured as Stand Alone. We STOP accepting input");

			setAcceptingInput(false);
		}


		/* (uc1.7) If the Scale Type configuration element is set to Secondary and this
		is a Secondary connection, then send the "No Primary scale" error and close the
		connection. Return. */
		if ( scaleSynchronizationType == ScaleSynchronizationTypeSecondary )
		{
			Util::CSmartPtr<HfcError> error(new HfcError(HfcError::ERRORCODE_NOTAPRIMARYSCALE));
			addError(error);

			Debug::trace("PrimaryCommSession", "------- Scale configured as Primary. We STOP accepting input");

			setAcceptingInput(false);
		}


		/* (uc1.7) If the Input Processing Enabled flag [Session Manager's] is clear
		and this is a non-console connection, then send the "Busy" error and close the
		connection. Return. */
		/* [This happens during the call to pauseProcessing.] */


		if ( isAcceptingInput() )
		{
			/* (uc4.22) Extract Secondary scale information from socket and communication header. */
			hfc::Scale::ScaleRole remoteScaleRole = header->getScaleRole();


			if ( remoteScaleRole == hfc::Scale::RoleSecondary )
			{
				/* (uc4.22) Identify the Secondary scale's record in the database, including the
				scale identifier. ... */
				remoteScale_ = getScaleManager()->findScaleByHostname(header->getHostName());
				if ( remoteScale_.null() )
				{
					remoteScale_ = getScaleManager()->findScaleByIPAddress(header->getIPAddress());
				}


				/* (uc4.22) ... If not present, then create a new record with the information
				received from the header, mark it as Registering, and trigger the execution of
				"Use Case 5.13: Registering Secondary scale", passing the Secondary scale's
				identifier. */
				if ( remoteScale_.null() )
				{
					remoteScale_ = new hfc::Scale();
					remoteScale_->setHostName(header->getHostName());
					remoteScale_->setIPAddress(header->getIPAddress());
					remoteScale_->setSecondaryScaleRole();
					remoteScale_->setRegistering();
					remoteScale_->setVersion(hfc::Batch::_invalidBatchVersion_);

					getScaleManager()->saveScale(remoteScale_.get());

					Util::CSmartPtr<RegisterSecondaryScaleMessage> message(new RegisterSecondaryScaleMessage(remoteScale_->getIdentifier()));
					dispatchEvent(message.get());
				}
				else
				{
					if ( remoteScale_->isStatePassive() )
					{
						Debug::trace("PrimaryCommSession", "Incoming connection from an OFFLINE scale");

						/* (uc4.22) If the Secondary scale's record was marked as off-line, then perform
						the following steps: */

						/* (uc) Mark the Secondary scale's record as on-line, storing the current
						date/time. */
						remoteScale_->setStateActive();
						getScaleManager()->saveScale(remoteScale_.get());


						/* (uc) If the first command is not Deregister scale or Synchronize scale,
						then mark the Secondary scale's record as Needs synchronization and trigger
						the execution of "Use Case 5.32: Synchronizing Secondary scale". [This is
						mostly done later on.] */
						scaleWasOffLine_ = true;
					}
					else if ( remoteScale_->isStateRetrying() )
					{
						/* If we are retrying certain scale communication command and the scale just
						communicated with this scale, then retry the previously failed command. */
						ProcessScaleOnlineMessage message(remoteScale_->getIdentifier());
						dispatchEvent(&message);
					}
				}
			}
			else
			{
				/* (uc4.22) If the other scale did not identifiy itself as a Secondary scale, then
				send the corresponding error and end the execution of this use case after closing
				the connection. */
				/* There is no need for any code here since the reason why we are here is because
				the other scale identified itself as a Secondary scale (criteria used by Session
				Starter to start this session). */
//				Util::CSmartPtr<HfcError> error(new HfcError(HfcError::));
//				addError(error);

				Debug::trace("PrimaryCommSession", "------- Not a Secondary scale. We STOP accepting input");

				setAcceptingInput(false);
			}
		}
	}


	bool PrimaryCommSession::doProcessCommand(Util::CSmartPtr<SessionMessage> command)
	{
		if ( firstCommand_ )
		{
			if ( scaleWasOffLine_ )
			{
				bool synchronizeSecondary = false;


				/* (uc4.22) [If the Secondary scale's record was marked as Off-line...] If the
				first command is not Deregister scale or Synchronize scale, then mark the
				Secondary scale's record as Needs synchronization and trigger the execution of
				"Use Case 5.32: Synchronizing Secondary scale". */
				if ( command->getDeviceType() == SynchronizationDev )
				{
					if ( !(command->getEventType() == synchronization::NotifyOnline ||
							command->getEventType() == synchronization::RegisterSecondaryScale) )
						synchronizeSecondary = true;
				}
				else
					synchronizeSecondary = true;

				if ( synchronizeSecondary )
				{
					remoteScale_ = getScaleManager()->findScaleByIdentifier(remoteScale_->getIdentifier());
					if ( remoteScale_.notNull() )
					{
						remoteScale_->setNotifiedOnline();
						getScaleManager()->saveScale(remoteScale_.get());

						SynchronizeSecondaryScaleMessage message(remoteScale_->getIdentifier());
						dispatchEvent(&message);
					}
				}
			}

			firstCommand_ = false;
		}


		/* (uc4.22) For each command received, perform the following steps: */
		if ( command->getDeviceType() == SynchronizationDev )
		{
			switch ( command->getEventType() )
			{
				case synchronization::RegisterSecondaryScale:
				{
					/* (uc4.22) If the command is Register Secondary scale and the scale is not marked as
					Registering, then mark the scale as Registering and trigger the execution of "Use
					Case 5.13: Registering Secondary scale", passing the Secondary scale's identifier. */
					if ( !remoteScale_->isRegistering() )
					{
						remoteScale_ = getScaleManager()->findScaleByIdentifier(remoteScale_->getIdentifier());
						if ( remoteScale_.notNull() )
						{
							remoteScale_->setRegistering();
							remoteScale_->setVersion(hfc::Batch::_invalidBatchVersion_);
							getScaleManager()->saveScale(remoteScale_.get());

							Debug::trace("PrimaryCommSession", "Received a registration notification from a secondary scale");

							RegisterSecondaryScaleMessage message(remoteScale_->getIdentifier());
							dispatchEvent(&message);
						}
					}

					break;
				}
				case synchronization::DeregisterSecondaryScale:
				{
					/* (uc4.22) If the command is Deregister Secondary scale, then execute "Use Case 5.15:
					Deregistering Secondary Scale", passing the scale identifier. */
					/* So far Secondary scales do not send this command yet. */

					break;
				}
				case synchronization::NotifyLocalBatchesPresent:
				{
					/* (uc4.22) If the command is Notify Local Batches, then mark the Secondary scale's record
					as Local batches notified and trigger the execution of "Use Case 5.24: Uploading
					Secondary scale local batches", passing the scale identifier. */
					remoteScale_ = getScaleManager()->findScaleByIdentifier(remoteScale_->getIdentifier());
					if ( remoteScale_.notNull() )
					{
						remoteScale_->setNotifiedBatches();
						getScaleManager()->saveScale(remoteScale_.get());

						Debug::trace("PrimaryCommSession", "Received a local batches notification from a secondary scale");

						DownloadLocalBatchesMessage message(remoteScale_->getIdentifier());
						dispatchEvent(&message);
					}

					break;
				}
				case synchronization::NotifyOnline:
				{
					/* (SYNCSTATUS) notify UI that we are starting to receive changes. The
					message will be sent once per batch. If that is too much we can put an
					if block around it to only report it once */
					NewSynchronizationStatusMessage statusMessage(SynchronizationActiveSyncStatus);
					dispatchEvent(&statusMessage);


					/* (uc4.22) If the command is Notify Online, then mark the Secondary scale's record
					as Needs synchronization and trigger the execution of "Use Case 5.32: Synchronizing
					Secondary scale ", passing the Secondary scale's identifier. */
					remoteScale_ = getScaleManager()->findScaleByIdentifier(remoteScale_->getIdentifier());
					if ( remoteScale_.notNull() )
					{
						remoteScale_->setNotifiedOnline();
						getScaleManager()->saveScale(remoteScale_.get());


						Debug::trace("PrimaryCommSession", "Received an online notification from a secondary scale");

						SynchronizeSecondaryScaleMessage message(remoteScale_->getIdentifier());
						dispatchEvent(&message);
					}

					break;
				}
				default:
				{
					/* (uc4.22) If the command is unknown, then do nothing. */
					Debug::trace("PrimaryCommSession", "Received an unknown command (" + util_string::inttostring(SynchronizationDev) +
							", " + util_string::inttostring(command->getEventType()) + ") from the remote scale");

					break;
				}
			}
		}
		else
		{
			/* (uc4.22) If the command is unknown, then do nothing. */
			Debug::trace("PrimaryCommSession", "Received an unknown command (" + util_string::inttostring(command->getDeviceType()) +
					", " + util_string::inttostring(command->getEventType()) + ") from the remote scale");
		}

		return true;
	}


	void PrimaryCommSession::doProcessError(CommSessionEvent::CommError /*error*/)
	{
		operationFailed_ = true;
	}


	bool PrimaryCommSession::doConclusion()
	{
		if ( !operationFailed_ )
		{
			/* (SYNCSTATUS) notify UI that we completed being notified from a Secondary. */
			/* [For now there is no status to send] */
		}
		else
		{
			/* (SYNCSTATUS) notify UI that we failed to be notified by a Secondary scale. */
			/* [For now there is no status to send] */
		}

		return true;
	}



}