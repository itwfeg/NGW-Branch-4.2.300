#include "PerformCGICallMessage.h"


#include "CGICallManagerEvents.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace comm
{


#ifdef USE_ACCOUNTANT
	int PerformCGICallMessage::_dataType_ = 0;
	std::string PerformCGICallMessage::_dataTypeName_("comm::PerformCGICallMessage"); 
#endif


	PerformCGICallMessage::PerformCGICallMessage(const std::string& url, const std::string& payload)
		: EvolMessage(CGICallManagerDev, PerformCGICall), url_(url), payload_(payload)
	{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif
	}


	PerformCGICallMessage::PerformCGICallMessage(const char* buffer)
		: EvolMessage(buffer)
	{
		int stringSize;

	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif

		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&stringSize, buffer, sizeof(stringSize));
		buffer += sizeof(stringSize);

		if ( stringSize )
		{
			url_.assign(buffer, stringSize);
			buffer += stringSize;
		}

		memcpy(&stringSize, buffer, sizeof(stringSize));
		buffer += sizeof(stringSize);

		if ( stringSize )
		{
			payload_.assign(buffer, stringSize);
			buffer += stringSize;
		}
	}


	PerformCGICallMessage::PerformCGICallMessage(const PerformCGICallMessage& m)
		: EvolMessage(m), url_(m.getURL()), payload_(m.getPayload())
	{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif
	}


	PerformCGICallMessage::~PerformCGICallMessage()
	{
	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
	#endif
	}


	PerformCGICallMessage& PerformCGICallMessage::operator = (const PerformCGICallMessage& rhs)
	{
		url_ = rhs.getURL();
		payload_ = rhs.getPayload();

		return *this;
	}


	const char* PerformCGICallMessage::serialize() const
	{
		int stringSize;

		EvolMessage::serialize();

		stringSize = (int)url_.size();
		serializedData.append((const char *)&stringSize, sizeof(stringSize));
		if ( stringSize )
			serializedData.append((const char *)url_.data(), stringSize);

		stringSize = (int)payload_.size();
		serializedData.append((const char *)&stringSize, sizeof(stringSize));
		if ( stringSize )
			serializedData.append((const char *)payload_.data(), stringSize);

		return serializedData.data();
	}


}
