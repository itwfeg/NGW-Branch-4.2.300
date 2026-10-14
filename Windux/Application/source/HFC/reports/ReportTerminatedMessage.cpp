#include "ReportTerminatedMessage.h"


#include "ReportEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int ReportTerminatedMessage::_dataType_ = 0;
	std::string ReportTerminatedMessage::_dataTypeName_("ReportTerminatedMessage");
#endif


ReportTerminatedMessage::ReportTerminatedMessage(bool cancelled)
	: EvolMessage(ReportManagerDev, TerminateReport), cancelled_(cancelled)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


ReportTerminatedMessage::ReportTerminatedMessage(const char * buffer)
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
	memcpy(&cancelled_, buffer, sizeof(cancelled_));
}


ReportTerminatedMessage::~ReportTerminatedMessage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *ReportTerminatedMessage::serialize() const
{
	EvolMessage::serialize();

	serializedData.append((char*)&cancelled_, sizeof(cancelled_));

	return serializedData.data();
}
