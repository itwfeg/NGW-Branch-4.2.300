#include "ProcessUserChangesMessage.h"


#include "SynchronizationDevEvent.h"


#include "SynchronizationChange.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int ProcessUserChangesMessage::_dataType_ = 0;
	std::string ProcessUserChangesMessage::_dataTypeName_("synchronization::ProcessUserChangesMessage"); 
#endif


	ProcessUserChangesMessage::ProcessUserChangesMessage(Util::CSmartPtr<hfc::SynchronizationChange> record)
		: EvolMessage(SynchronizationDev, ProcessUserChangesEvent), userChange_(record)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	ProcessUserChangesMessage::ProcessUserChangesMessage(const char* buffer)
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
		userChange_ = new hfc::SynchronizationChange(buffer);
	}


	ProcessUserChangesMessage::~ProcessUserChangesMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* ProcessUserChangesMessage::serialize() const
	{
		EvolMessage::serialize();

		const char * buffer = userChange_->serialize();
		serializedData.append(buffer, userChange_->getSerializedLength());

		return serializedData.data();
	}


}