#include "GCDDevNoParamMsg.h"
#include "CMReaderWriterGCD.h"

GCDDevNoParamMsg::GCDDevNoParamMsg(GCDEvent event) :
	EvolMessage(GCDDev, static_cast< UINT8 >(event))
{
	//build the data stream to send
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
}

UINT32 GCDDevNoParamMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevNoParamMsg::serialize() const
{
	return serializedData.data();
}