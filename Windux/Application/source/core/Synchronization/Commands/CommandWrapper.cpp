
#if defined( __linux__ )
    #include <string.h>
#endif

#include "CommandWrapper.h"



#include "PurgeBatchesCommand.h"
#include "AbortBatchCommand.h"
#include "ApplyBatchCommand.h"
#include "DeleteBatchCommand.h"
#include "ApplyAndDeleteBatchCommand.h"

#include "SynchronizePrimaryScaleCommand.h"
#include "RequestRegistrationCommand.h"
#include "RequestDeregistrationCommand.h"
#include "NotifyLocalBatchesPresentCommand.h"
#include "DownloadLocalBatchesCommand.h"
#include "NotifyOnlineCommand.h"
#include "SynchronizeSecondaryScaleCommand.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int CommandWrapper::_dataType_ = 0;
	std::string CommandWrapper::_dataTypeName_("synchronization::CommandWrapper"); 
#endif


	CommandWrapper::CommandWrapper(const Command* command) : command_(command) 
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	CommandWrapper::~CommandWrapper() 
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	Command* CommandWrapper::createCommand(const char * buffer)
	{
		Command::CommandType type;
		memcpy(&type, buffer, sizeof(type));
		buffer += sizeof(type);

		if ( type != Command::InvalidCommandType )
		{
			Command* command = 0;

			switch ( type )
			{
				case Command::PurgeBatchesCommandType:
						command = new PurgeBatchesCommand(buffer); break;
				case Command::AbortBatchCommandType:
						command = new AbortBatchCommand(buffer); break;
				case Command::ApplyBatchCommandType:
						command = new ApplyBatchCommand(buffer); break;
				case Command::DeleteBatchCommandType:
						command = new DeleteBatchCommand(buffer); break;
				case Command::ApplyAndDeleteBatchCommandType:	
						command = new ApplyAndDeleteBatchCommand(buffer); break;

				case Command::SynchronizePrimaryScaleCommandType:
						command = new SynchronizePrimaryScaleCommand(buffer); break;
				case Command::RequestRegistrationCommandType:
						command = new RequestRegistrationCommand(buffer); break;
				case Command::RequestDeregistrationCommandType:
						command = new RequestDeregistrationCommand(buffer); break;
				case Command::NotifyLocalBatchesPresentCommandType:
						command = new NotifyLocalBatchesPresentCommand(buffer); break;
				case Command::DownloadLocalBatchesCommandType:
						command = new DownloadLocalBatchesCommand(buffer); break;
				case Command::NotifyOnlineCommandType:
						command = new NotifyOnlineCommand(buffer); break;
				case Command::SynchronizeSecondaryScaleCommandType:
						command = new SynchronizeSecondaryScaleCommand(buffer); break;
            #if defined(LINUX_FIX)
                default:
                        command = 0; break;
            #endif
			}

			return command;
		}
		else
			return 0;
	}


	const char* CommandWrapper::serialize() const
	{
		serializedData.clear();

		if ( command_ )
		{
			Command::CommandType type = command_->getType();
			serializedData.append((char*)&type, sizeof(type));

			const char* buffer = command_->serialize();
			serializedData.append(buffer, command_->getSerializedLength());
		}
		else
		{
			Command::CommandType type = Command::InvalidCommandType;
			serializedData.append((char*)&type, sizeof(type));
		}

		return serializedData.data();
	}


}