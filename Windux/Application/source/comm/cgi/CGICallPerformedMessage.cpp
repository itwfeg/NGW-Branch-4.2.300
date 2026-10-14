#include "CGICallPerformedMessage.h"


#include "CGICallManagerEvents.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace comm
{


#ifdef USE_ACCOUNTANT
	int CGICallPerformedMessage::_dataType_ = 0;
	std::string CGICallPerformedMessage::_dataTypeName_("comm::CGICallPerformedMessage"); 
#endif


	CGICallPerformedMessage::CGICallPerformedMessage(bool result)
		: EvolMessage(CGICallManagerDev, CGICallPerformed), result_(result)
	{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif
	}


	CGICallPerformedMessage::CGICallPerformedMessage(const char* buffer)
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
		memcpy(&result_, buffer, sizeof(result_));
	}


	CGICallPerformedMessage::CGICallPerformedMessage(const CGICallPerformedMessage& m)
		: EvolMessage(m), result_(m.getResult())
	{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif
	}


	CGICallPerformedMessage::~CGICallPerformedMessage()
	{
	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
	#endif
	}


	CGICallPerformedMessage& CGICallPerformedMessage::operator = (const CGICallPerformedMessage& rhs)
	{
		result_ = rhs.getResult();

		return *this;
	}


	const char* CGICallPerformedMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((const char *)&result_, sizeof(result_));

		return serializedData.data();
	}


}
