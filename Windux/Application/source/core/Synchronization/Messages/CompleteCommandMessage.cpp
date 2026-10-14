#include "CompleteCommandMessage.h"

#include "Commands/CommandWrapper.h"

#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int CompleteCommandMessage::_dataType_ = 0;
	std::string CompleteCommandMessage::_dataTypeName_("synchronization::CompleteCommandMessage"); 
#endif

	CompleteCommandMessage::CompleteCommandMessage(Util::CSmartPtr<Command> command)
		: EvolMessage(SynchronizationDev, CompleteCommandEvent), command_(command)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	CompleteCommandMessage::CompleteCommandMessage(const char* buffer)
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


	CompleteCommandMessage::~CompleteCommandMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* CompleteCommandMessage::serialize() const
	{
		EvolMessage::serialize();

		CommandWrapper wrapper(command_);
		const char* buffer = wrapper.serialize();
		serializedData.append(buffer, wrapper.getSerializedLength());

		return serializedData.data();
	}


}