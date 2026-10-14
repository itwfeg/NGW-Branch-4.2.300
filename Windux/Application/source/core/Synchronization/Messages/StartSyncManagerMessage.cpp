
#if defined( __linux__ )
    #include <string.h>
#endif

#include "StartSyncManagerMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int StartSyncManagerMessage::_dataType_ = 0;
	std::string StartSyncManagerMessage::_dataTypeName_("synchronization::StartSyncManagerMessage");
#endif


	StartSyncManagerMessage::StartSyncManagerMessage(bool online, Util::CMSGEvent& event)
		: EvolMessage(SynchronizationDev, StartSyncManagerEvent),
		onLine_(online), registration_(false), primaryScale_(), event_(event)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	StartSyncManagerMessage::StartSyncManagerMessage(bool online, bool registration, const std::string & primaryScale, Util::CMSGEvent& event)
		: EvolMessage(SynchronizationDev, StartSyncManagerEvent),
		onLine_(online), registration_(registration), primaryScale_(primaryScale), event_(event)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	StartSyncManagerMessage::StartSyncManagerMessage(const char* buffer)
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
		memcpy(&onLine_, buffer, sizeof(onLine_));
		buffer += sizeof(onLine_);

		memcpy(&registration_, buffer, sizeof(registration_));
		buffer += sizeof(registration_);


		int stringSize;
		memcpy(&stringSize, buffer, sizeof(stringSize));
		buffer += sizeof(stringSize);

		if ( stringSize )
		{
			primaryScale_.assign(buffer, stringSize);
			buffer += stringSize;
		}

		HANDLE handle;
		memcpy(&handle, buffer, sizeof(handle));
		// Kind of clumsy
		Util::CSmartHandle shandle(handle);
		event_ = Util::CMSGEvent(shandle);
		shandle.Release();
	}


	StartSyncManagerMessage::~StartSyncManagerMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	const char* StartSyncManagerMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&onLine_, sizeof(onLine_));
		serializedData.append((char*)&registration_, sizeof(registration_));

		std::string::size_type stringSize = primaryScale_.size();
		serializedData.append((const char*)&stringSize, sizeof(stringSize));
		serializedData.append(primaryScale_, 0, stringSize);

		const HANDLE handle = event_.get();
		serializedData.append((const char*)&handle, sizeof(HANDLE));

		return serializedData.data();
	}


}