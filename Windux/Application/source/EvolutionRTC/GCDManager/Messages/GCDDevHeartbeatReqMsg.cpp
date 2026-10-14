#include "GCDDevHeartbeatReqMsg.h"
#include "CMReaderWriterGCD.h"

GCDDevHeartbeatReqMsg::GCDDevHeartbeatReqMsg() :
	EvolMessage(GCDDev, GcdHeartbeat)
{
	serializedData.reserve(8);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
}

UINT32 GCDDevHeartbeatReqMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevHeartbeatReqMsg::serialize() const
{
	return serializedData.data();
}