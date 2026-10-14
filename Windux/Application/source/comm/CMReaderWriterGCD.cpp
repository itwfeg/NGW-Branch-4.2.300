#include "CMReaderWriterGCD.h"
#include "Integer.h"
#include "UInteger.h"

CMReaderWriterGCD::CMReaderWriterGCD()
{
	setReaderWriterId(GCD_ID);
}

CMReaderWriterGCD::~CMReaderWriterGCD()
{
}

int CMReaderWriterGCD::finalizeRecordForSending(char *buff, const char *msg, int length)
{
	memcpy(buff, msg, length);
	return length;
}

bool CMReaderWriterGCD::finishedReadingRecord(const char *msg, int length)
{
	if (length < sizeof(MessageHeader))
		return false;
	const MessageHeader* pMessageHeader = reinterpret_cast< const MessageHeader* >(msg);
	return (pMessageHeader->length <= length - static_cast< int >(sizeof(MessageHeader)));
}

bool CMReaderWriterGCD::finishedReadingRecord(const HobString &msg)
{
	return finishedReadingRecord(msg.data(), static_cast< int >(msg.size()));
}

int CMReaderWriterGCD::formatFromReceive(char *msg, int length, bool isRecordBeginningBuffer)
{
	UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(isRecordBeginningBuffer);
	return length - sizeof(MessageHeader);
}

int CMReaderWriterGCD::formatFromReceive(HobString &msg)
{
	return formatFromReceive(const_cast< char * >(msg.data()), static_cast< int >(msg.size()), false);
}

void CMReaderWriterGCD::getRTData(const char *msg, char *rtData)
{
	memcpy(rtData, msg, sizeof(MessageHeader));
}

void CMReaderWriterGCD::determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event)
{
	const MessageHeader *pMessageHeader = reinterpret_cast< const MessageHeader * >(recordTypeData);
	*device = GCDDev;
	*event = static_cast< UINT8 >(pMessageHeader->type);
}