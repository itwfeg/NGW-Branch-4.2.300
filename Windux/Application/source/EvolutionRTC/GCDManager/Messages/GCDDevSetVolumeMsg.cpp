#include "GCDDevSetVolumeMsg.h"
#include "CMReaderWriterGCD.h"
#include "Integer.h"

GCDDevSetVolumeMsg::GCDDevSetVolumeMsg(int percent) :
	EvolMessage(GCDDev, GcdSetVolume)
{
	serializedData.reserve(16);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, Integer::toString(percent));
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevSetVolumeMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevSetVolumeMsg::serialize() const
{
	return serializedData.data();
}