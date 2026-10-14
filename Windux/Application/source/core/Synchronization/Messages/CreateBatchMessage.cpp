
#if defined( __linux__ )
    #include <string.h>
#endif

#include "CreateBatchMessage.h"


#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int CreateBatchMessage::_dataType_ = 0;
	std::string CreateBatchMessage::_dataTypeName_("synchronization::CreateBatchMessage"); 
#endif

	CreateBatchMessage::CreateBatchMessage(hfc::Batch::BatchOrigin origin, hfc::Batch::BatchType type)
		: EvolMessage(SynchronizationDev, CreateBatchEvent), origin_(origin), type_(type)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	CreateBatchMessage::CreateBatchMessage(const char* buffer)
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
		memcpy(&origin_, buffer, sizeof(origin_));
		buffer += sizeof(origin_);

		memcpy(&type_, buffer, sizeof(type_));
	}


	CreateBatchMessage::~CreateBatchMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const char* CreateBatchMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&origin_, sizeof(origin_));
		serializedData.append((char*)&type_, sizeof(type_));

		return serializedData.data();
	}


}