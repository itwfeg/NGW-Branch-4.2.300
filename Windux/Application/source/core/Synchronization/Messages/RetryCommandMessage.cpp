#include "RetryCommandMessage.h"

#include "Commands/CommandWrapper.h"

#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int RetryCommandMessage::_dataType_ = 0;
	std::string RetryCommandMessage::_dataTypeName_("synchronization::RetryCommandMessage"); 
#endif

	RetryCommandMessage::RetryCommandMessage(Util::CSmartPtr<Command> command)
		: EvolMessage(SynchronizationDev, RetryCommandEvent), command_(command)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	RetryCommandMessage::RetryCommandMessage(const char* buffer)
		: EvolMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		command_ = CommandWrapper::createCommand(buffer);
	}


	RetryCommandMessage::~RetryCommandMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* RetryCommandMessage::serialize() const
	{
		EvolMessage::serialize();

		CommandWrapper wrapper(command_);
		const char* buffer = wrapper.serialize();
		serializedData.append(buffer, wrapper.getSerializedLength());

		return serializedData.data();
	}


}