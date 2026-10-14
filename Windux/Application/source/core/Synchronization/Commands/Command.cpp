
#if defined( __linux__ )
    #include <string.h>
#endif

#include "Command.h"


#include "Messages/SynchronizationDevEvent.h"
#include "Messages/SynchronizePrimaryScaleProcessedMessage.h"
#include "Messages/RequestRegistrationProcessedMessage.h"
#include "Messages/NotifyOnlineProcessedMessage.h"
#include "Messages/ApplyBatchProcessedMessage.h"
#include "Messages/DeleteBatchProcessedMessage.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


	std::string Command::PurgeBatchesCommandTypeName("PurgeBatches");
	std::string Command::AbortBatchCommandTypeName("AbortBatch");
	std::string Command::ApplyBatchCommandTypeName("ApplyBatch");
	std::string Command::DeleteBatchCommandTypeName("DeleteBatch");
	std::string Command::ApplyAndDeleteBatchCommandTypeName("ApplyAndDeleteBatch");

	std::string Command::SynchronizePrimaryScaleCommandTypeName("SynchronizePrimaryScale");
	std::string Command::RequestRegistrationCommandTypeName("RequestRegistration");
	std::string Command::RequestDeregistrationCommandTypeName("RequestDeregistration");
	std::string Command::NotifyLocalBatchesPresentCommandTypeName("NotifyLocalBatchesPresent");
	std::string Command::DownloadLocalBatchesCommandTypeName("DownloadLocalBatches");
	std::string Command::NotifyOnlineCommandTypeName("NotifyOnline");
	std::string Command::SynchronizeSecondaryScaleCommandTypeName("SynchronizeSecondaryScale");

	std::string Command::InvalidCommandTypeName("Invalid");

		
#ifdef USE_ACCOUNTANT
	int Command::_dataType_ = 0;
	std::string Command::_dataTypeName_("synchronization::Command"); 
#endif


	Command::Command(CommandType type, int targetId, ProcessedMessage* message)
			: type_(type), targetId_(targetId), sequenceNumber_(_invalidSequenceNumber_),
			state_(InvalidState), status_(0), completionMessage_(message)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	Command::Command(const char * buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif


		memcpy(&type_, buffer, sizeof(type_));
		buffer += sizeof(type_);

		memcpy(&targetId_, buffer, sizeof(targetId_));
		buffer += sizeof(targetId_);

		memcpy(&queue_, buffer, sizeof(queue_));
		buffer += sizeof(queue_);

		memcpy(&sequenceNumber_, buffer, sizeof(sequenceNumber_));
		buffer += sizeof(sequenceNumber_);

		memcpy(&state_, buffer, sizeof(state_));
		buffer += sizeof(state_);

		memcpy(&retryTimerId_, buffer, sizeof(retryTimerId_));
		buffer += sizeof(retryTimerId_);

		memcpy(&retryingReadyToExecute_, buffer, sizeof(retryingReadyToExecute_));
		buffer += sizeof(retryingReadyToExecute_);

		memcpy(&status_, buffer, sizeof(status_));
		buffer += sizeof(status_);

		int present = 0;
		memcpy(&present, buffer, sizeof(present));
		buffer += sizeof(present);

		if ( present )
		{
			Util::CSmartPtr<EvolMessage> message(new EvolMessage(buffer));

			if ( message->getDeviceType() == SynchronizationDev )
			{
				switch ( message->getEventType() )
				{
					case SynchronizePrimaryScaleProcessedEvent:
					{
						completionMessage_ = new SynchronizePrimaryScaleProcessedMessage(buffer);

						break;
					}
					case RequestRegistrationProcessedEvent:
					{
						completionMessage_ = new RequestRegistrationProcessedMessage(buffer);

						break;
					}
					case NotifyOnlineProcessedEvent:
					{
						completionMessage_ = new NotifyOnlineProcessedMessage(buffer);

						break;
					}
					case ApplyBatchProcessedEvent:
					{
						completionMessage_ = new ApplyBatchProcessedMessage(buffer);

						break;
					}
					case DeleteBatchProcessedEvent:
					{
						completionMessage_ = new DeleteBatchProcessedMessage(buffer);

						break;
					}
					default:
					{
						completionMessage_ = 0;

						break;
					}
				}
			}
		}
		else
			completionMessage_ = 0;
	}


	Command::~Command()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* Command::serialize() const
	{
		serializedData.clear();
		serializedData.append((char*)&type_, sizeof(type_));
		serializedData.append((char*)&targetId_, sizeof(targetId_));
		serializedData.append((char*)&queue_, sizeof(queue_));
		serializedData.append((char*)&sequenceNumber_, sizeof(sequenceNumber_));
		serializedData.append((char*)&state_, sizeof(state_));
		serializedData.append((char*)&retryTimerId_, sizeof(retryTimerId_));
		serializedData.append((char*)&retryingReadyToExecute_, sizeof(retryingReadyToExecute_));
		serializedData.append((char*)&status_, sizeof(status_));

		if ( completionMessage_.notNull() )
		{
			int present = 1;
			serializedData.append((char*)&present, sizeof(present));

			const char* buffer = completionMessage_->serialize();
			serializedData.append(buffer, completionMessage_->getSerializedLength());
		}
		else
		{
			int absent = 0;
			serializedData.append((char*)&absent, sizeof(absent));
		}

		return serializedData.data();
	}


	const std::string& Command::getTypeName() const
	{
		switch ( type_ )
		{
			case PurgeBatchesCommandType:
				return PurgeBatchesCommandTypeName;
			case AbortBatchCommandType:
				return AbortBatchCommandTypeName;
			case ApplyBatchCommandType:
				return ApplyBatchCommandTypeName;
			case DeleteBatchCommandType:
				return DeleteBatchCommandTypeName;
			case ApplyAndDeleteBatchCommandType:
				return ApplyAndDeleteBatchCommandTypeName;

			case SynchronizePrimaryScaleCommandType:
				return SynchronizePrimaryScaleCommandTypeName;
			case RequestRegistrationCommandType:
				return RequestRegistrationCommandTypeName;
			case RequestDeregistrationCommandType:
				return RequestDeregistrationCommandTypeName;
			case NotifyLocalBatchesPresentCommandType:
				return NotifyLocalBatchesPresentCommandTypeName;
			case DownloadLocalBatchesCommandType:
				return DownloadLocalBatchesCommandTypeName;
			case NotifyOnlineCommandType:
				return NotifyOnlineCommandTypeName;
			case SynchronizeSecondaryScaleCommandType:
				return SynchronizeSecondaryScaleCommandTypeName;

			default:
				return InvalidCommandTypeName;
		}
	}


}