#include "GCDDevLoadDisplayObjectXMLMsg.h"
#include "CMReaderWriterGCD.h"
#include "UInteger.h"

GCDDevLoadDisplayObjectXMLMsg::GCDDevLoadDisplayObjectXMLMsg(LPCSTR szFilename, const DWORD dwIdleAdvertType, LPCSTR szWebsite) :
	EvolMessage(GCDDev, GcdLoadDisplayObjectXML)
{
	serializedData.reserve(1024);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "2", 0);
	CMReaderWriterGCD::concatToData(serializedData, szFilename);
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(dwIdleAdvertType));
	CMReaderWriterGCD::concatToData(serializedData, szWebsite);
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevLoadDisplayObjectXMLMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevLoadDisplayObjectXMLMsg::serialize() const
{
	return serializedData.data();
}
