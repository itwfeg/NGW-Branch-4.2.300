#include "GCDDevGetFileInfoMsg.h"
#include "CMReaderWriterGCD.h"

GCDDevGetFileInfoMsg::GCDDevGetFileInfoMsg(LPCSTR szRemoteFilePath) :
	EvolMessage(GCDDev, GcdGetFileInfo)
{
	serializedData.reserve(64);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, szRemoteFilePath);
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevGetFileInfoMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevGetFileInfoMsg::serialize() const
{
	return serializedData.data();
}