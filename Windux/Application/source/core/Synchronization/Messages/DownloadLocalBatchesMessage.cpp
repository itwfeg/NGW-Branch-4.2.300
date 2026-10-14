
#if defined( __linux__ )
    #include <string.h>
#endif

#include "DownloadLocalBatchesMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int DownloadLocalBatchesMessage::_dataType_ = 0;
	std::string DownloadLocalBatchesMessage::_dataTypeName_("synchronization::DownloadLocalBatchesMessage");
#endif


	DownloadLocalBatchesMessage::DownloadLocalBatchesMessage(int scaleId)
		: EvolMessage(SynchronizationDev, DownloadLocalBatchesEvent), scaleId_(scaleId)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	DownloadLocalBatchesMessage::DownloadLocalBatchesMessage(const char* buffer)
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
		memcpy(&scaleId_, buffer, sizeof(scaleId_));
	}


	DownloadLocalBatchesMessage::~DownloadLocalBatchesMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	const char* DownloadLocalBatchesMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&scaleId_, sizeof(scaleId_));

		return serializedData.data();
	}


}