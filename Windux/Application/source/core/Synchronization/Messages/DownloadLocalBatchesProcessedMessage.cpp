
#if defined( __linux__ )
    #include <string.h>
#endif

#include "DownloadLocalBatchesProcessedMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int DownloadLocalBatchesProcessedMessage::_dataType_ = 0;
	std::string DownloadLocalBatchesProcessedMessage::_dataTypeName_("synchronization::DownloadLocalBatchesProcessedMessage");
#endif


	DownloadLocalBatchesProcessedMessage::DownloadLocalBatchesProcessedMessage(int status, int scaleId)
		: ProcessedMessage(SynchronizationDev, DownloadLocalBatchesProcessedEvent, status), scaleId_(scaleId)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	DownloadLocalBatchesProcessedMessage::DownloadLocalBatchesProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

			
		ProcessedMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&scaleId_, buffer, sizeof(scaleId_));
	}


	DownloadLocalBatchesProcessedMessage::~DownloadLocalBatchesProcessedMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	const char* DownloadLocalBatchesProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&scaleId_, sizeof(scaleId_));

		return serializedData.data();
	}


}