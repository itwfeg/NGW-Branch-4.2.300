
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ComingUpOnLineMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int ComingUpOnLineMessage::_dataType_ = 0;
	std::string ComingUpOnLineMessage::_dataTypeName_("synchronization::ComingUpOnLineMessage");
#endif


	ComingUpOnLineMessage::ComingUpOnLineMessage(bool reg)
		: EvolMessage(SynchronizationDev, ComingUpOnLineEvent), register_(reg)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	ComingUpOnLineMessage::ComingUpOnLineMessage(const char* buffer)
		: EvolMessage(buffer)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

			
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&register_, buffer, sizeof(register_));
	}


	ComingUpOnLineMessage::~ComingUpOnLineMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	const char* ComingUpOnLineMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&register_, sizeof(register_));

		return serializedData.data();
	}


}