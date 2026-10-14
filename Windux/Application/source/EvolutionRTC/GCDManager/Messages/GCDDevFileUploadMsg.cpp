#include "GCDDevFileUploadMsg.h"
#include "CMReaderWriterGCD.h"
#include "UInteger.h"

GCDDevFileUploadMsg::GCDDevFileUploadMsg(LPCSTR szRemoteFilePath, const DWORD dwFileSize, const DWORD dwLastWriteTime) :
	EvolMessage(GCDDev, GcdFileUpload)
{
	serializedData.reserve(64);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, szRemoteFilePath);
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(dwFileSize));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(dwLastWriteTime));
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevFileUploadMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevFileUploadMsg::serialize() const
{
	return serializedData.data();
}
