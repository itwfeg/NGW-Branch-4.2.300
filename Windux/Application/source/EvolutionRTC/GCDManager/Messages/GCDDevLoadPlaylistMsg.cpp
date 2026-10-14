#include "GCDDevLoadPlaylistMsg.h"
#include "CMReaderWriterGCD.h"

GCDDevLoadPlaylistMsg::GCDDevLoadPlaylistMsg(const HobString& playlist) :
	EvolMessage(GCDDev, GcdLoadPlaylist)
{
	serializedData.reserve(16);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, playlist);
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevLoadPlaylistMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevLoadPlaylistMsg::serialize() const
{
	return serializedData.data();
}