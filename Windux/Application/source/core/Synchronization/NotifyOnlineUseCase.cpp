#include "NotifyOnlineUseCase.h"


#include "ScaleManager.h"
#include "BatchManager.h"

#include "RegisterSecondaryScale.h"
#include "NotifyLocalBatches.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/NewSynchronizationStatusMessage.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int NotifyOnlineUseCase::_dataType_ = 0;
	const std::string NotifyOnlineUseCase::_dataTypeName_("synchronization::NotifyOnlineUseCase");
#endif


	NotifyOnlineUseCase::NotifyOnlineUseCase(CommunicationSynchronizer* synchronizer)
		: synchronizer_(synchronizer), needToRegister_(false), needToDeregister_(false), needToNotifyBatches_(false), operationFailed_(false)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	NotifyOnlineUseCase::~NotifyOnlineUseCase()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	bool NotifyOnlineUseCase::doInitialize()
	{
		Util::CSmartPtr<hfc::Scale> thisScale(synchronizer_->getScaleManager()->getThisScale());
		if ( thisScale.notNull() )
		{
			needToRegister_ = thisScale->isRegistering();
			needToDeregister_ = thisScale->isDeregistering();
			needToNotifyBatches_ = synchronizer_->getBatchManager()->arePendingSynchronizationLocalBatchesPresent();
		}

		return true;
	}


	int NotifyOnlineUseCase::doSendCommand()
	{
		Util::CSmartPtr<SessionMessage> message;

		if ( needToRegister_ )
		{
			message = new SessionMessage(SynchronizationDev, RegisterSecondaryScale);
			message->setFormatType(SynchronizationTaggedFormat);
			message->setHfcObject(new hfc::RegisterSecondaryScale());
			if ( synchronizer_->sendMessage(message) )
				Debug::trace("CommunicationSynchronizer", "Use Case Notify Online: sent command to register");
			else
				return CommunicationSynchronizer::STATUS_FAILED;
		}
		else
		{
			/* (uc6.30) Send the Notify Online command. */
			message = new SessionMessage(SynchronizationDev, NotifyOnline);
			message->setFormatType(SynchronizationTaggedFormat);
			if ( synchronizer_->sendMessage(message) )
				Debug::trace("CommunicationSynchronizer", "Use Case Notify Online: sent command to notify online");
			else
				return CommunicationSynchronizer::STATUS_FAILED;
		}


		if ( needToNotifyBatches_ )
		{
			message = new SessionMessage(SynchronizationDev, NotifyLocalBatchesPresent);
			message->setFormatType(SynchronizationTaggedFormat);
			message->setHfcObject(new hfc::NotifyLocalBatches());
			if ( synchronizer_->sendMessage(message) )
				Debug::trace("CommunicationSynchronizer", "Use Case Notify Online: sent command to notify local batches");
			else
				return CommunicationSynchronizer::STATUS_FAILED;
		}


		return CommunicationSynchronizer::STATUS_COMPLETED_ALL_COMMANDS;
	}



	void NotifyOnlineUseCase::doProcessError(CommSessionEvent::CommError /*error*/)
	{
		operationFailed_ = true;
	}


	void NotifyOnlineUseCase::doConclude()
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
			thisScale->clearNotifiedOnline();

			synchronizer_->getScaleManager()->saveScale(thisScale.get());
		}

		if ( !operationFailed_ )
		{
			/* (SYNCSTATUS) notify UI that notify online succeeded. */
			NewSynchronizationStatusMessage message(SynchronizationActiveSyncStatus);
			synchronizer_->getMessageReceiver()->dispatchEvent(&message);
		}
		else
		{
			/* (SYNCSTATUS) notify UI that notify online failed. */
			NewSynchronizationStatusMessage message(UnableToContactPrimarySyncStatus);
			synchronizer_->getMessageReceiver()->dispatchEvent(&message);
		}
	}


	void NotifyOnlineUseCase::showUseCaseStartingInfo()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Notify Online: Starting");
	}


	void NotifyOnlineUseCase::showUseCaseSuccessful()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Notify Online: completed successfully");
	}


	void NotifyOnlineUseCase::showUseCaseFailed(const std::string& status)
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Notify Online: failed (status = " + status + ")");
	}



}
