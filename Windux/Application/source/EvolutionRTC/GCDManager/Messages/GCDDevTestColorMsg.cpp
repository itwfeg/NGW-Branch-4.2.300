#include "GCDDevTestColorMsg.h"
#include "CMReaderWriterGCD.h"

GCDDevTestColorMsg::GCDDevTestColorMsg(int fType, const HobString &image) :
	EvolMessage(GCDDev, GcdTestColor)
{
	serializedData.reserve(256);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "2", 0);
	if ( 1 == fType )
		CMReaderWriterGCD::concatToData(serializedData, "1");
	else if ( 2 == fType )
		CMReaderWriterGCD::concatToData(serializedData, "2");
	else if (( 3 == fType ) && ( image.length() ))
		CMReaderWriterGCD::concatToData(serializedData, "3");
	else if (4 == fType)
		CMReaderWriterGCD::concatToData(serializedData, "4");
	else if (6 == fType)
		CMReaderWriterGCD::concatToData(serializedData, "6");
	else if (7 == fType)
		CMReaderWriterGCD::concatToData(serializedData, "7");
	else
		CMReaderWriterGCD::concatToData(serializedData, "0");
	CMReaderWriterGCD::concatToData(serializedData, image);
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevTestColorMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevTestColorMsg::serialize() const
{
	return serializedData.data();
}
