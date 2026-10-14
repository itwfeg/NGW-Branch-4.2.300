#include "GCDDevSetTimeInfo.h"
#include "CMReaderWriterGCD.h"
#include "Integer.h"
#include "UInteger.h"

GCDDevSetTimeInfo::GCDDevSetTimeInfo(const PAL_PTIME_ZONE_STRUCT pTimeZone, const PAL_PTIME_STRUCT pLocalTime) :
	EvolMessage(GCDDev, GcdSetTimeInfo)
{
	//build the data stream to send
	serializedData.reserve(64);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, Integer::toString(pTimeZone->Bias));
	CMReaderWriterGCD::concatToData(serializedData, Integer::toString(pTimeZone->StandardBias));
	CMReaderWriterGCD::concatToData(serializedData, util_string::wchartostring(pTimeZone->StandardName).c_str());
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pTimeZone->StandardDate.wMonth));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pTimeZone->StandardDate.wDayOfWeek));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pTimeZone->StandardDate.wDay));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pTimeZone->StandardDate.wHour));
	CMReaderWriterGCD::concatToData(serializedData, Integer::toString(pTimeZone->DaylightBias));
	CMReaderWriterGCD::concatToData(serializedData, util_string::wchartostring(pTimeZone->DaylightName).c_str());
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pTimeZone->DaylightDate.wMonth));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pTimeZone->DaylightDate.wDayOfWeek));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pTimeZone->DaylightDate.wDay));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pTimeZone->DaylightDate.wHour));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pLocalTime->wYear));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pLocalTime->wMonth));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pLocalTime->wDayOfWeek));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pLocalTime->wDay));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pLocalTime->wHour));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pLocalTime->wMinute));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pLocalTime->wSecond));
	CMReaderWriterGCD::concatToData(serializedData, UInteger::toString(pLocalTime->wMilliseconds));
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevSetTimeInfo::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevSetTimeInfo::serialize() const
{
	return serializedData.data();
}
