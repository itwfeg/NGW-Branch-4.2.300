#include "AddRecordToBatchMessage.h"


#include "SynchronizationDevEvent.h"


#include "SynchronizationChange.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int AddRecordToBatchMessage::_dataType_ = 0;
	std::string AddRecordToBatchMessage::_dataTypeName_("synchronization::AddRecordToBatchMessage"); 
#endif


	AddRecordToBatchMessage::AddRecordToBatchMessage(int id, Util::CSmartPtr<hfc::SynchronizationChange> record)
		: EvolMessage(SynchronizationDev, AddRecordToBatchEvent), batchId_(id), record_(record)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	AddRecordToBatchMessage::AddRecordToBatchMessage(const char* buffer)
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
		memcpy(&batchId_, buffer, sizeof(batchId_));
		buffer += sizeof(batchId_);

		record_ = new hfc::SynchronizationChange(buffer);
	}


	AddRecordToBatchMessage::~AddRecordToBatchMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* AddRecordToBatchMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		const char * buffer = record_->serialize();
		serializedData.append(buffer, record_->getSerializedLength());

		return serializedData.data();
	}


}