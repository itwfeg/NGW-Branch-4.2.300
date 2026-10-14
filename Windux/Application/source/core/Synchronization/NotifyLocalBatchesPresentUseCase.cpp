#include "NotifyLocalBatchesPresentUseCase.h"


#include "NotifyLocalBatches.h"

#include "ScaleManager.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/NewSynchronizationStatusMessage.h"

#include "RegisterSecondaryScale.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int NotifyLocalBatchesPresentUseCase::_dataType_ = 0;
	const std::string NotifyLocalBatchesPresentUseCase::_dataTypeName_("synchronization::NotifyLocalBatchesPresentUseCase");
#endif


	NotifyLocalBatchesPresentUseCase::NotifyLocalBatchesPresentUseCase(CommunicationSynchronizer* synchronizer)
		: synchronizer_(synchronizer), needToRegister_(false), operationFailed_(false)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	NotifyLocalBatchesPresentUseCase::~NotifyLocalBatchesPresentUseCase()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	bool NotifyLocalBatchesPresentUseCase::doInitialize()
	{
		Util::CSmartPtr<hfc::Scale> thisScale(synchronizer_->getScaleManager()->getThisScale());
		if ( thisScale.notNull() )
			needToRegister_ = thisScale->isRegistering();

		return true;
	}


	int NotifyLocalBatchesPresentUseCase::doSendCommand()
	{
		if ( needToRegister_ )
		{
			Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, RegisterSecondaryScale));
			message->setFormatType(SynchronizationTaggedFormat);
			message->setHfcObject(new hfc::RegisterSecondaryScale());
			if ( synchronizer_->sendMessage(message) )
				Debug::trace("CommunicationSynchronizer", "Use Case Notify Local Batches: sent command to register");
			else
				return CommunicationSynchronizer::STATUS_FAILED;
		}


		/* (uc6.23) Send the Notify local batches present command. */
		Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, NotifyLocalBatchesPresent));
		message->setFormatType(SynchronizationTaggedFormat);
		message->setHfcObject(new hfc::NotifyLocalBatches());
		if ( synchronizer_->sendMessage(message) )
		{
			Debug::trace("CommunicationSynchronizer", "Use Case Notify Local Batches: sent command to notify of local batches");
			return CommunicationSynchronizer::STATUS_COMPLETED_ALL_COMMANDS;
		}

		return CommunicationSynchronizer::STATUS_FAILED;
	}


	void NotifyLocalBatchesPresentUseCase::doProcessError(CommSessionEvent::CommError /*error*/)
	{
		operationFailed_ = true;
	}


	void NotifyLocalBatchesPresentUseCase::doConclude()
	{
		/* (TODO) do we do this even if the operation failed? */
		Util::CSmartPtr<hfc::Scale> thisScale(synchronizer_->getScaleManager()->getThisScale());
		if ( thisScale.notNull() )
		{
			/* (TODO) need to protect db access in case the db is closed/closing */
			if ( needToRegister_ )
				thisScale->clearRegistering();
//			if ( needToDeregister_ )
//				thisScale->clearDeregistering();
			thisScale->clearNotifiedBatches();

			synchronizer_->getScaleManager()->saveScale(thisScale.get());
		}

		if ( !operationFailed_ )
		{
			/* (SYNCSTATUS) notify UI that notify changes succeeded. */
			NewSynchronizationStatusMessage message(SynchronizationActiveSyncStatus);
			synchronizer_->getMessageReceiver()->dispatchEvent(&message);
		}
		else
		{
			/* (SYNCSTATUS) notify UI that notify changes failed. */
			NewSynchronizationStatusMessage message(UnableToContactPrimarySyncStatus);
			synchronizer_->getMessageReceiver()->dispatchEvent(&message);
		}
	}


	void NotifyLocalBatchesPresentUseCase::showUseCaseStartingInfo()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Notify Local Batches Present: Starting");
	}


	void NotifyLocalBatchesPresentUseCase::showUseCaseSuccessful()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Notify Local Batches Present: completed successfully");
	}


	void NotifyLocalBatchesPresentUseCase::showUseCaseFailed(const std::string& status)
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Notify Local Batches Present: failed (status = " + status + ")");
	}


}
