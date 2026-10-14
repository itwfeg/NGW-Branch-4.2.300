#include "RequestRegistrationUseCase.h"


#include "ScaleManager.h"


#include "RegisterSecondaryScale.h"
#include "NotifyLocalBatches.h"


#include "Messages/SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int RequestRegistrationUseCase::_dataType_ = 0;
	const std::string RequestRegistrationUseCase::_dataTypeName_("synchronization::RequestRegistrationUseCase");
#endif


	RequestRegistrationUseCase::RequestRegistrationUseCase(CommunicationSynchronizer* synchronizer)
		: synchronizer_(synchronizer)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	RequestRegistrationUseCase::~RequestRegistrationUseCase()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	int RequestRegistrationUseCase::doSendCommand()
	{
		/* (uc6.12) Send the Register Secondary scale command, passing the Secondary scale's hostname/
		IP address information. */
		Util::CSmartPtr<SessionMessage> message(new SessionMessage(SynchronizationDev, RegisterSecondaryScale));
		message->setFormatType(SynchronizationTaggedFormat);
		message->setHfcObject(new hfc::RegisterSecondaryScale());
		if ( synchronizer_->sendMessage(message) )
		{
			Debug::trace("CommunicationSynchronizer", "Use Case Request registration: sent command to register");
			return CommunicationSynchronizer::STATUS_COMPLETED_ALL_COMMANDS;
		}

		return CommunicationSynchronizer::STATUS_FAILED;
	}


	void RequestRegistrationUseCase::doProcessError(CommSessionEvent::CommError /*error*/)
	{
//		operationFailed_ = true;
	}


	void RequestRegistrationUseCase::doConclude()
	{
		Util::CSmartPtr<hfc::Scale> thisScale(synchronizer_->getScaleManager()->getThisScale());

		/* (TODO) we need to protect db access in case the db closed */
		/* (TODO) also, what if the operation failed? */
		thisScale->clearRegistering();
		synchronizer_->getScaleManager()->saveScale(thisScale.get());
	}


	void RequestRegistrationUseCase::showUseCaseStartingInfo()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Requesting Registration: Starting");
	}


	void RequestRegistrationUseCase::showUseCaseSuccessful()
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Requesting Registration: completed successfully");
	}


	void RequestRegistrationUseCase::showUseCaseFailed(const std::string& status)
	{
		Debug::trace("CommunicationSynchronizer", "Use Case Requesting Registration: failed (status = " + status + ")");
	}


}
