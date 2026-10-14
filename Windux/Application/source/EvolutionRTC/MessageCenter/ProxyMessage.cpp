#include "ProxyMessage.h"


#include "EventDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace Hobart
{


#ifdef USE_ACCOUNTANT
	int ProxyMessage::_dataType_ = 0;
	std::string ProxyMessage::_dataTypeName_("Hobart::ProxyMessage");
#endif


	ProxyMessage::ProxyMessage(EvolMessage* message, Util::CMSGEvent& event, EvolMessageWrapper* response)
		: EvolMessage(Event, ProxyEvent),
		// copying the CMSGEvent object merely "duplicates" (in the Windows sense) the event handle
		originalMessage_(), event_(event), response_(response)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		const char * buffer = message->serialize();
		originalMessage_.assign(buffer, message->getSerializedLength());
	}


	ProxyMessage::ProxyMessage(const char* buffer)
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
		int stringSize;
		memcpy(&stringSize, buffer, sizeof(stringSize));
		buffer += sizeof(stringSize);

		if ( stringSize )
		{
			originalMessage_.assign(buffer, stringSize);
			buffer += stringSize;
		}


		HANDLE handle;
		memcpy(&handle, buffer, sizeof(handle));
		buffer += sizeof(handle);
		// Kind of clumsy
		Util::CSmartHandle shandle(handle);
		event_ = Util::CMSGEvent(shandle);           // This line duplicates the event handle
		shandle.Release();


		memcpy(&response_, buffer, sizeof(response_));
	}


	ProxyMessage::~ProxyMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	const char* ProxyMessage::serialize() const
	{
		EvolMessage::serialize();


#ifdef MIN_WARN_FIX
		size_t stringSize = originalMessage_.size();
		serializedData.append((const char*)&stringSize, sizeof(stringSize));
		serializedData.append(originalMessage_, 0, stringSize);
#else
		int stringSize = originalMessage_.size();
		serializedData.append((const char*)&stringSize, sizeof(stringSize));
		serializedData.append(originalMessage_, 0, stringSize);
#endif

		const HANDLE handle = event_.get();
		serializedData.append((const char*)&handle, sizeof(HANDLE));

		serializedData.append((const char*)&response_, sizeof(response_));

		return serializedData.data();
	}


}

