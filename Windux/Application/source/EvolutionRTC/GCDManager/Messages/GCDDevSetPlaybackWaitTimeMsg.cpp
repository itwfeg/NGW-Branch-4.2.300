#include "GCDDevSetPlaybackWaitTimeMsg.h"
#include "CMReaderWriterGCD.h"

GCDDevSetPlaybackWaitTimeMsg::GCDDevSetPlaybackWaitTimeMsg(const HobString& waittime) :
	EvolMessage(GCDDev, GcdSetPlaybackWaitTime)
{
	serializedData.reserve(16);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, waittime);
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevSetPlaybackWaitTimeMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevSetPlaybackWaitTimeMsg::serialize() const
{
	return serializedData.data();
}